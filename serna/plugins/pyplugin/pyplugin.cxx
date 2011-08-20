//
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
//
// This file is part of Syntext Serna XML Editor.
//
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
//
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General
// Public License versions 2.0 or 3.0 as published by the Free Software
// Foundation and appearing in the file LICENSE.GPL included in the packaging
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL
// Exception for Syntext Serna Free Edition, included in the file
// GPL_EXCEPTION.txt in this package.
//
// You should have received a copy of appropriate licenses along with this
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales
// department at sales@syntext.com.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
#include <Python.h>
#include "sapi/common/PropertyNode.h"
#include "sapi/app/Config.h"
#include "sapi/app/DocumentPlugin.h"
#include "common/common_defs.h"
#include "common/OwnerPtr.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <list>
#include <iostream>

#ifdef WIN32
# define VC_EXTRALEAN
# include <windows.h>
# define DL_OPEN(fn) LoadLibraryA(fn)
# define DL_SYM(sym) GetProcAddress(py_handle, sym)

typedef HINSTANCE dlHandle;

#else // WIN32
# include <dlfcn.h>
# define DL_OPEN(fn) dlopen(fn, RTLD_GLOBAL|RTLD_LAZY)
# define DL_SYM(sym) dlsym(py_handle, sym)

typedef void* dlHandle;

#endif // WIN32

// START_IGNORE_LITERALS

using namespace SernaApi;

static dlHandle py_handle = 0;
static const char SERNA_API_MODULE[] = "SernaApi";
static const char PYTHON_RSS_PROP[]  = "#PythonInterp-PyRunSimpleString";
static const char PYTHON_RSF_PROP[]  = "#PythonInterp-py-run-file";

#define PM_DECL(meth) static void* (*_##meth)(...);

PM_DECL(PyRun_SimpleString);    PM_DECL(PyRun_SimpleFileEx);
PM_DECL(Py_IsInitialized);      PM_DECL(Py_SetProgramName);
PM_DECL(Py_Initialize);         PM_DECL(PyEval_InitThreads);
PM_DECL(PyErr_Clear);           PM_DECL(PyImport_GetModuleDict);
PM_DECL(PyDict_GetItemString);  PM_DECL(PyImport_ReloadModule);
PM_DECL(PyImport_ImportModule); PM_DECL(PyImport_AddModule);
PM_DECL(PyModule_GetDict);      PM_DECL(PyErr_Occurred);
PM_DECL(Py_BuildValue);         PM_DECL(PyObject_CallObject);

class PythonDocInstance : public SernaApiBase {
public:
    PythonDocInstance(SernaApiBase*, SernaApiBase*, char **msgbuf)
        : inst_(0), msgbuf_(msgbuf) {}
    virtual ~PythonDocInstance()
    {
        Py_XDECREF(inst_);
    }
    typedef void* (*VFP)(...);
    VFP             pyResolve(const char*);
    void            setPyErr(const SString& message);
    void            setErrorMsg(const SString& message);
    bool            initPythonMethods();
    PyObject*       inst_;
    char**          msgbuf_;
};

PythonDocInstance::VFP PythonDocInstance::pyResolve(const char* name)
{
    void* rp = DL_SYM(name);
    if (rp)
        return (VFP) rp;
    setErrorMsg("Cannot resolve required Python symbol: " + SString(name));
    return 0;
}

void PythonDocInstance::setErrorMsg(const SString& msg)
{
    *msgbuf_ = new char[1024];
    msg.toUtf8(*msgbuf_, 1023);
}

bool PythonDocInstance::initPythonMethods()
{
#define PM_INIT(meth) if (!(_##meth = pyResolve(#meth))) return false; 
    static bool dl_initialized = false;
    if (!dl_initialized) {
        PM_INIT(PyRun_SimpleString);    PM_INIT(PyRun_SimpleFileEx);
        PM_INIT(Py_IsInitialized);      PM_INIT(Py_SetProgramName);
        PM_INIT(Py_Initialize);         PM_INIT(PyEval_InitThreads);
        PM_INIT(PyErr_Clear);           PM_INIT(PyImport_GetModuleDict);
        PM_INIT(PyDict_GetItemString);  PM_INIT(PyImport_ReloadModule);
        PM_INIT(PyImport_ImportModule); PM_INIT(PyImport_AddModule);
        PM_INIT(PyModule_GetDict);      PM_INIT(PyErr_Occurred);
        PM_INIT(Py_BuildValue);         PM_INIT(PyObject_CallObject);
        dl_initialized = true;
    }
    return true;
}

static void py_run(const SString& str)
{
    char buf[8192];
    str.toUtf8(buf, sizeof(buf));
    for (char* p = buf; *p; ++p)
        if (*p == '\\')
            *p = '/';
    _PyRun_SimpleString(buf);
}

static void py_run_file(FILE* fp, const char* filename)
{
    _PyRun_SimpleFileEx(fp, filename, 1);
}

void PythonDocInstance::setPyErr(const SString& msg)
{
    setErrorMsg(msg);
    py_run("import sys"); // kick python to produce traceback
}

static void checked_insert(const SString& what, bool doAppend = false)
{
    SString str = "if not sys.path.__contains__('";
    str += what;
    str += "'): ";
    if (doAppend)
        str += "sys.path.append('";
    else
        str += "sys.path.insert(0, '";
    str += what;
    str += "')";
    py_run(str);
}

static SString find_system_python_lib()
{
#if !defined(_WIN32)
    static const char* paths[] = {
        "/lib/", "/usr/lib/", "/usr/local/lib/", 0
    };
    char basename[32];
    ::snprintf(basename, sizeof(basename), "libpython%d.%d.so.1.0",
               PY_MAJOR_VERSION, PY_MINOR_VERSION);
    char libpath[64];
    for (const char** dir = &paths[0]; 0 != *dir; ++dir) {
        strcpy(libpath, *dir);
        strcat(libpath, basename);
        if (0 == ::access(libpath, F_OK))
            return SString(libpath);
    }
#endif
    return SString();
des}

#define PERR(m) { inst->setErrorMsg(m); return 0; }
#define PERR_IF(t,m) { if (t) PERR(m); }
#define PYERR_IF(t,m) { \
    if (_PyErr_Occurred()) { inst->setPyErr(m); return 0; } \
    else if (t) { inst->setErrorMsg(m); return 0; }}

static PyObject* init_pyclass(SernaApiBase* props, 
                              SString& className, 
                              PythonDocInstance* inst)
{
    char buf[8192];
    PropertyNode ptn(props);
    SString currentPath = ptn.getProperty("resolved-path").getString();
    ptn = ptn.getProperty("data");
    PERR_IF(!ptn, 
        "No plugin <data> section specified in .spd file in " + currentPath);
    className = ptn.getProperty("instance-class").getString();
    SString dllpath;
    if (!py_handle) {
        PropertyNode dll_prop = ptn.getProperty("python-dll");
        if (dll_prop) 
            dllpath = SernaConfig::resolveResource(SString(),
                                                   dll_prop.getString(), "");
#if !defined(_WIN32)
        if (dllpath.empty() ||
            0 != ::access(dllpath.toLocal8Bit(buf, sizeof(buf)), F_OK)) {
            dllpath = find_system_python_lib();
        }
#endif
        PERR_IF(dllpath.empty(), "Python shared library cannot be found");
        py_handle = DL_OPEN(dllpath.toUtf8(buf, sizeof(buf)));
        PERR_IF(!py_handle, "DLL open <" + dllpath + "> failed");
        if (!inst->initPythonMethods())
            return 0;
    }
    if (!_Py_IsInitialized()) {
#ifdef _WIN32
        SString::size_type pos = dllpath.find("/PCBuild");
        if (pos != SString::npos)
            dllpath = dllpath.erase(pos);
        Char PATH_SEP(';');
#else
        char PH[] = "PYTHONHOME";
        putenv(PH);
        Char PATH_SEP(':');
#endif // _WIN32
        SString ext_path =
            SernaConfig().root().getProperty("vars/ext_plugins").getString();
        _Py_SetProgramName(dllpath.toUtf8(buf, sizeof(buf)));
        _Py_Initialize();
        _PyEval_InitThreads();
        PropertyNode cfgRoot(SernaConfig::root());
        cfgRoot.makeDescendant(PYTHON_RSS_PROP).
            setPtr((void*) _PyRun_SimpleString);
        cfgRoot.makeDescendant(PYTHON_RSF_PROP).
            setPtr((void*) py_run_file);
        SString plugins_dir     = SernaConfig::getProperty("vars/plugins");
        SString plugins_bin_dir = SernaConfig::getProperty("vars/plugins_bin");
        SString data_dir = SernaConfig::getProperty("vars/data_dir");
        py_run("import sys");
#if defined(__WIN32) && defined(NDEBUG)
        py_run("sys.path = []");
#endif
        checked_insert(plugins_dir);
        if (!ext_path.isEmpty()) {
            const Char* cp = ext_path.unicode();
            const Char* ce = cp + ext_path.length();
            while (cp < ce) {
                const Char* pcp = cp;
                while (cp < ce && *cp != PATH_SEP)
                    ++cp;
                if (pcp != cp)
                    checked_insert(SString(pcp, cp - pcp));
                while (cp < ce && *cp == PATH_SEP)
                    ++cp;
            }
        }
        checked_insert(plugins_bin_dir + "/pyplugin");
        checked_insert(plugins_dir + "/pyplugin");
        checked_insert(data_dir + "/python/libs");
        checked_insert(data_dir + "/python/lib");
        checked_insert(data_dir + "/python/lib/site-packages");
        py_run("import SernaApi");
    } else
        _PyErr_Clear();
    int dir_pos = currentPath.rfind('/');
    if (dir_pos <= 0)
        dir_pos = currentPath.rfind('\\');
    SString module_dir = currentPath.mid(dir_pos + 1);
    PERR_IF(module_dir.isEmpty(), 
        "Module directory is not defined for " + currentPath);
    if (ptn)
        ptn = ptn.firstChild();
    PyObject* pclass = 0;
    PyObject* module = 0;
    PyObject* dict   = 0;
    for (; ptn; ptn = ptn.nextSibling()) {
        ptn.getString().toUtf8(buf, sizeof(buf));
        if (ptn.name() == "codestr") {
            _PyRun_SimpleString(buf);
        } else if (ptn.name() == "instance-module") {
            char module_buf[8192];
            SString mod_path = module_dir + '.' + ptn.getString();
            mod_path.toUtf8(module_buf, sizeof(module_buf));
            PERR_IF(module, "<instance-module> is already defined");
            if (ptn.getString() != "__main__") {
                PyObject* md = (PyObject*) _PyImport_GetModuleDict();
                PyObject* modp = (PyObject*)
                    _PyDict_GetItemString(md, module_buf);
                if (modp) {
                    modp = (PyObject*) _PyImport_ReloadModule(modp);
                    PYERR_IF(!modp, "Cannot reload Python module: " + mod_path);
                    module = modp;
                } else {
                    module = (PyObject*) _PyImport_ImportModule(module_buf);
                    PYERR_IF(!module, "Cannot load Python module: " + mod_path);
                }
                Py_XDECREF(module);
            } else {
                module = (PyObject*) _PyImport_AddModule(module_buf);
                PYERR_IF(!module, "Cannot load instance module");
            }
        }
        else if (ptn.name() == "instance-class") {
            PERR_IF(!module, "No instance-module defined");
            PERR_IF(pclass,  "<instance-class> is already defined");
            dict = (PyObject*) _PyModule_GetDict(module);
            PERR_IF(!dict,   "No valid dict in instance-module");
            pclass = (PyObject*) _PyDict_GetItemString(dict, buf);
            PYERR_IF(!pclass, "Module does not contain specified class");
        }
    }
    return pclass;
}

extern "C" {

COMMON_EXPORT PLUGIN_EXPORT SernaApiBase*
init_serna_plugin(SernaApiBase* sernaDoc,
                  SernaApiBase* pluginProps,
                  char** errMsgBuf)
{
    if (!errMsgBuf)
        return new SernaApiBase;
    *errMsgBuf = 0;
    Common::OwnerPtr<PythonDocInstance> inst(
        new PythonDocInstance(sernaDoc, pluginProps, errMsgBuf));
    PyObject* pclass = 0;
    SString className;
    if (!(pclass = init_pyclass(pluginProps, className, &*inst))) {
        if (*errMsgBuf)
            return 0;
        return inst.release();    // special case for preloading
    }
    PyObject* args = (PyObject*) _Py_BuildValue("ll", sernaDoc, pluginProps);
    if (!args) {
        Py_XDECREF(pclass);
        inst->setPyErr("Cannot build Python argument list");
        return 0;
    }
    inst->inst_ = (PyObject*) _PyObject_CallObject(pclass, args, 0);
    if (inst->inst_) 
        return inst.release();
    inst->setPyErr("Cannot create instance of Python class: " + className);
    Py_XDECREF(args);
    Py_XDECREF(pclass);
    return 0;
}

} // extern "C"

