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
/*! \file
 */

#include "xs/ExtPythonScript.h"

#ifdef USE_PYTHON

#ifndef NO_XPATH
#include "xpath/ExprContext.h"
#include "xpath/Engine.h"
#include "xpath/ValueHolder.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#endif  //NO_XPATH

#include "xs/ExtPythonScript.h"
#include "xs/Schema.h"
#include "xs/Piece.h"
#include "xs/Origin.h"
#include "xs/XsMessages.h"
#include "common/String.h"
#include <qstringlist.h>

#ifdef  USE_PYTHON
#include <Python.h>
#include <import.h>
//#include <graminit.h> //it is in conflict with xpath, define expr is used
#include <pythonrun.h>
#endif  //USE_PYTHON

XS_NAMESPACE_BEGIN

class ExtPythonScriptImpl : public ExtPythonScript {
public:
    virtual void  parsePyScript(Schema* schema, String& script);

    virtual bool  pyValidate(Schema* schema,
                             const DvOrigin& o,
                             const Piece* pc,
                             const String& funcName,
                             String& result) const;

    virtual bool  pyValidateComplex(Schema* schema,
                                    const DvOrigin& o,
                                    const Piece* pc,
                                    const String& funcName) const;
    ExtPythonScriptImpl();
    virtual ~ExtPythonScriptImpl();

private:
#ifdef  USE_PYTHON
    PyObject*                    pdict_;
#endif  //USE_PYTHON
    bool                         pyIsInitialized_;
};

void ExtPythonScriptImpl::parsePyScript(Schema* schema, String& script)
{
#ifdef  USE_PYTHON
    if (!pyIsInitialized_) {
        Py_Initialize();
        pyIsInitialized_ = true;
        PyObject* pmod;
        pmod = PyImport_ImportModule("__main__");
        if (pmod == NULL) {
            if (0 != schema)
                schema->mstream() << XsMessages::fetchScriptError
                        << Message::L_ERROR
                        << _tr("can't import module __main__");
        }
        pdict_ = PyModule_GetDict(pmod);
        Py_DECREF(pmod);
        if (pdict_ == NULL) {
            if (0 != schema)
                schema->mstream() << XsMessages::fetchScriptError
                        << Message::L_ERROR
                        << _tr("can't get module dict");
        }
    }
    String content = script.stripWhiteSpace()+'\n';
    PyRun_String(const_cast<char*>(content.utf8()),
        257 /*file_input constant*/, pdict_, pdict_);
#else  // USE_PYTHON
    (void) schema;
    (void) script; // kill warnings
#endif // USE_PYTHON
}

bool ExtPythonScriptImpl::pyValidate(Schema* schema,
                                     const DvOrigin& o,
                                     const Piece* pc,
                                     const String& funcName,
                                     String& result) const
{
#ifdef  USE_PYTHON
    if (String::null() == result) {
        result = "";
        if (0 != schema)
            schema->mstream() << XsMessages::parseScriptError
                    << Message::L_WARNING
                    << _tr("parameter is null string - make it empty") << PieceArg(pc) << o;
    }

    if (!pyIsInitialized_) {
        if (0 != schema)
            schema->mstream() << XsMessages::parseScriptError
                    << Message::L_ERROR
                    << _tr("script not found or not initialized") << PieceArg(pc) << o;
        return false;
    }

    PyObject* pres;
    String fetch;
    if ( result.find('\''))
        fetch = funcName + " (\"" + result + "\")";
    else
        fetch = funcName + " ('" + result + "')";



    pres = PyRun_String(const_cast<char*>(fetch.utf8()),
                        258 /*eval_input constant*/, pdict_, pdict_);

    if (NULL == pres) {
        if (0 != schema)
            schema->mstream() << XsMessages::parseScriptError
                    << Message::L_ERROR
                    << _tr("script or function's name is not correct") << PieceArg(pc) << o;
        return false;
    }

    int ok = 0;
    char* str = 0;
    if (0 == PyTuple_Check(pres)) {
        if (0 != schema)
            schema->mstream() << XsMessages::getResultScriptError
                    << Message::L_ERROR
                    << _tr("result is not tuple object") << PieceArg(pc) << o;
        return false;
    }

    if (!PyArg_ParseTuple(pres, "is", &ok, &str)) { /* Int and a string */
        if (0 != schema)
            schema->mstream() << XsMessages::getResultScriptError
                    << Message::L_ERROR
                    << _tr("can't convert result - check type of returning values") << PieceArg(pc) << o;
        return false;
    }
    Py_DECREF(pres);
    result = str;

    bool valid = method_->validate(schema, o, pc, funcName, result);

    return ((ok != 0) && valid);
#endif  //USE_PYTHON
#ifndef  USE_PYTHON
    return method_->validate(schema, o, pc, funcName, result);
#endif  //USE_PYTHON
}



bool ExtPythonScriptImpl::pyValidateComplex(Schema* schema,
                                            const DvOrigin& o,
                                            const Piece* pc,
                                            const String& funcName) const
{
#ifdef  USE_PYTHON
    if (!pyIsInitialized_) {
        if (0 != schema)
                schema->mstream() << XsMessages::parseScriptError
                        << Message::L_ERROR
                        << _tr("script not found or not initialized") << PieceArg(pc) << o;
        return false;
    }
    PyObject* pres;
    String fetch;
    String params = "";
    QString quot;
    if (funcName.find('\''))
        quot = "\"";
    else
        quot = "\'";
    QStringList list = QStringList::split(' ', funcName);
    int sz = list.count();
#ifndef NO_XPATH
    if ( 0 == o.element() ) {
        if (0 != schema)
            schema->mstream() << XsMessages::noDvOrigin
                        << Message::L_WARNING
                        << PieceArg(pc) << o;
#endif  //NO_XPATH
        for (uint i = 1; i < list.count(); i++)
            list[i] = quot + "" + quot;
#ifndef NO_XPATH
    }
    else if (1 < sz) {
        for (uint i = 1; i < sz; i++) {
            String exprstr = NOTR("string(") + (String)list[i] + NOTR(")");
            RefCntPtr<Xpath::Expr> expr;
            try {
                expr = XPATH_NAMESPACE::Engine::makeExpr(exprstr);
            }
            catch (...) {
                if (0 != schema)
                    schema->mstream() << XsMessages::parseXpathExprError
                        << Message::L_ERROR << PieceArg(pc) << o;
                return false;
            }

            XPATH_NAMESPACE::NodeSetItem nsi(o.element());
            XPATH_NAMESPACE::ExprContext exprContext(o.element(), 0);
            XPATH_NAMESPACE::ValueHolderPtr  value;
            try {
                value = expr->makeInst(nsi, exprContext);
            }
            catch (...) {
                if (0 != schema)
                    schema->mstream() << XsMessages::evalXpathExprError
                            << Message::L_ERROR
                            << PieceArg(pc) << o;
                return false;
            }
            String val = quot + value->value()->getString().qstring() + quot;
            val = val.replace( "\t", "" )
                     .replace("\n", "")
                     .replace("\r", "");
                     //'[' + QChar(0x9) + QChar(0xA) + QChar(0xD) + ']';
            list[i] = val.qstring();
        }
    }
#endif  //NO_XPATH
    String name = list[0];
    QStringList::Iterator it = list.begin();
    list.remove(it);
    params = (String)list.join(",");
    fetch = name + " (" + params + ")";

    pres = PyRun_String(const_cast<char*>(fetch.utf8()),
                        258 /*eval_input constant*/, pdict_, pdict_);

    if (NULL == pres) {
        if (0 != schema)
            schema->mstream() << XsMessages::parseScriptError
                    << Message::L_ERROR
                    << _tr("script or function's name is not correct") << PieceArg(pc) << o;
        return false;
    }

    int ok = 0;

    if (!PyArg_Parse(pres, "i", &ok)) { /* Int */
        if (0 != schema)
            schema->mstream() << XsMessages::getResultScriptError
                    << Message::L_ERROR
                    << _tr("can't convert result - check type of returning values") << PieceArg(pc) << o;
        return false;
    }
    Py_DECREF(pres);

    bool valid = method_->validateComplex(schema, o, pc, name, params);

    return ((ok != 0) && valid);
#else  // USE_PYTHON
    (void) schema; (void) pc; (void) o; (void) funcName; // kill warnings
    return false;
#endif // USE_PYTHON
}

ExtPythonScriptImpl::ExtPythonScriptImpl()
    : pyIsInitialized_(false)
{}

ExtPythonScriptImpl::~ExtPythonScriptImpl()
{
#ifdef USE_PYTHON
    if (pyIsInitialized_) {
        Py_DECREF(pdict_);
        Py_Finalize();
    }
#endif // USE_PYTHON
}

ExtPythonScript::ExtPythonScript()
{
    method_ = new ExtValidationCallback();
}

void ExtPythonScript::setExtCallback(ExtValidationCallback* method)
{
    method_ = method;
}

ExtPythonScript* ExtPythonScript::make()
{
    return new ExtPythonScriptImpl();
}

ExtPythonScript::~ExtPythonScript()
{}

XS_NAMESPACE_END

#endif // USE_PYTHON
