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

static void* checked_resolve(const char* name)
{
    void* rp = DL_SYM(name);
    if (0 == rp)
        throw SString("Cannot resolve required Python symbol: ") +
            SString(name);
    return rp;
}

typedef void* (*vfp)(...);  // pointer to void func with varargs

# define DYNCALL(sym)              (*(vfp)(checked_resolve(sym)))()
# define DYNCALL1(sym, a1)         (*(vfp)(checked_resolve(sym)))(a1)
# define DYNCALL2(sym, a1, a2)     (*(vfp)(checked_resolve(sym)))(a1, a2)
# define DYNCALL3(sym, a1, a2, a3) (*(vfp)(checked_resolve(sym)))(a1, a2, a3)

class PythonDocInstance : public SernaApiBase {
public:
    PythonDocInstance(SernaApiBase*, SernaApiBase*)
        : inst_(0) {}
    virtual ~PythonDocInstance()
    {
        Py_XDECREF(inst_);
    }
    PyObject*       inst_;
};

#define ALLOC_MSGBUF *errMsgBuf = new char[1024]

static void checkPyErr()
{
    if (DYNCALL("PyErr_Occurred"))
        DYNCALL("PyErr_Print");
}

static void checkPyImportErr()
{
    if (DYNCALL("PyErr_Occurred")) {
#if defined(_MSC_VER) && (!defined(NDEBUG) && defined(_DEBUG))
        if (PyErr_ExceptionMatches(PyExc_NotImplementedError)) {
#else
        if (DYNCALL1("PyErr_ExceptionMatches",
                    checked_resolve("PyExc_NotImplementedError"))) {
#endif
            DYNCALL("PyErr_Clear");
        }
        else
            DYNCALL("PyErr_Print");
    }
}

static void py_run(const SString& str)
{
    char buf[8192];
    str.toLatin1(buf, sizeof(buf));
    for (char* p = buf; *p; ++p)
        if (*p == '\\')
            *p = '/';
    DYNCALL1("PyRun_SimpleString", buf);
    checkPyErr();
}

static void py_run_file(FILE* fp, const char* filename)
{
    DYNCALL3("PyRun_SimpleFileEx", fp, filename, 1);
    checkPyErr();
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
}

static PyObject* init_pyclass(SernaApiBase* props, SString& className)
{
    char buf[8192];
    PropertyNode ptn(props);
    SString currentPath = ptn.getProperty("resolved-path").getString();
    ptn = ptn.getProperty("data");
    if (!ptn)
        throw SString("No plugin <data> section specified in .spd file in " +
                      currentPath);
    className = ptn.getProperty("instance-class").getString();
    SString dllpath;
    if (!py_handle) {
        PropertyNode dll_prop = ptn.getProperty("python-dll");
        if (dll_prop) {
//            throw SString("No <python-dll> property defined in .spd file " +
//                          currentPath);
            dllpath = SernaConfig::resolveResource(SString(),
                                                   dll_prop.getString(), "");
        }
#if !defined(_WIN32)
        if (dllpath.empty() ||
            0 != ::access(dllpath.toLocal8Bit(buf, sizeof(buf)), F_OK)) {
            dllpath = find_system_python_lib();
        }
#endif
        if (dllpath.empty())
            throw SString("Python shared library cannot be found");

        py_handle = DL_OPEN(dllpath.toLatin1(buf, sizeof(buf)));
        if (0 == py_handle)
            throw SString("DLL open <" + dllpath + "> failed");
    }
    if (!DYNCALL("Py_IsInitialized")) {
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
        DYNCALL1("Py_SetProgramName", dllpath.toLatin1(buf, sizeof(buf)));
        DYNCALL("Py_Initialize");
        DYNCALL("PyEval_InitThreads");
        PropertyNode cfgRoot(SernaConfig::root());
        void* pfunc = (void *)DL_SYM("PyRun_SimpleString");
        cfgRoot.makeDescendant(PYTHON_RSS_PROP).setPtr(pfunc);
        pfunc = (void *)py_run_file;
        cfgRoot.makeDescendant(PYTHON_RSF_PROP).setPtr(pfunc);
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
    }
    int dir_pos = currentPath.rfind('/');
    if (dir_pos <= 0)
        dir_pos = currentPath.rfind('\\');
    SString module_dir = currentPath.mid(dir_pos + 1);
    if (module_dir.isEmpty())
        throw SString("Module directory is not defined for " + currentPath);
    if (ptn)
        ptn = ptn.firstChild();
    PyObject* pclass = 0;
    PyObject* module = 0;
    PyObject* dict   = 0;
    for (; ptn; ptn = ptn.nextSibling()) {
        ptn.getString().toLatin1(buf, sizeof(buf));
        if (ptn.name() == "codestr")
            DYNCALL1("PyRun_SimpleString", buf);
        else if (ptn.name() == "instance-module") {
            char module_buf[8192];
            SString mod_path = module_dir + '.' + ptn.getString();
            mod_path.toLatin1(module_buf, sizeof(module_buf));
            if (module)
                throw SString("<instance-module> is already defined");
            if (ptn.getString() != "__main__") {
                PyObject* md = (PyObject*) DYNCALL("PyImport_GetModuleDict");
                PyObject* modp = (PyObject*) DYNCALL2("PyDict_GetItemString",
                                                      md, module_buf);
                if (modp) {
                    modp = (PyObject*) DYNCALL1("PyImport_ReloadModule", modp);
                    checkPyImportErr();
                    if (0 == modp)
                        throw SString("Cannot reload module: " + mod_path);
                    module = modp;
                } else {
                    module = (PyObject*) DYNCALL1("PyImport_ImportModule",
                                                  module_buf);
                    checkPyImportErr();
                }
                Py_XDECREF(module);
            } else {
                module = (PyObject*) DYNCALL1("PyImport_AddModule", module_buf);
                checkPyErr();
                if (!module)
                    throw SString("Cannot load instance module");
            }
        }
        else if (ptn.name() == "instance-class") {
            if (!module)
                throw SString("No instance-module defined");
            if (pclass)
                throw SString("<instance-class> is already defined");
            dict = (PyObject*) DYNCALL1("PyModule_GetDict", module);
            if (!dict)
                throw SString("No valid dict in instance-module");
            pclass = (PyObject*) DYNCALL2("PyDict_GetItemString", dict, buf);
            checkPyErr();
            if (0 == pclass)
                throw SString("Module does not contain specified class");
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

    PythonDocInstance* inst = new PythonDocInstance(sernaDoc, pluginProps);
    PyObject* pclass = 0;
    SString className;
    try {
        pclass = init_pyclass(pluginProps, className);
        if (pclass) {
            PyObject* args = (PyObject*) DYNCALL3("Py_BuildValue", "ll",
                                                  sernaDoc, pluginProps);
            inst->inst_ = (PyObject*) DYNCALL3("PyObject_CallObject",
                                               pclass, args, 0);
            checkPyErr();
        }
    } catch (SString& s) {
        ALLOC_MSGBUF;
        s.toLatin1(*errMsgBuf, 1024);
        return 0;
    }
    catch (...) {
        ALLOC_MSGBUF;
        strcpy(*errMsgBuf, "Unknown exception\n");
        return 0;
    }
    if (pclass && !inst->inst_) {
        ALLOC_MSGBUF;
        char classNameBuf[1024];
        strcpy(*errMsgBuf, "Cannot create instance of Python class: ");
        className.toLatin1(classNameBuf, 1024);
        strcat(*errMsgBuf, classNameBuf);
        strcat(*errMsgBuf, "\n");
        delete inst;
        return 0;
    }
    return inst;
}

} // extern "C"

