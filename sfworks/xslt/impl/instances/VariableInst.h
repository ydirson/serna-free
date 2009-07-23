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

#ifndef XSLT_IMPL_VARIABLE_INST_H
#define XSLT_IMPL_VARIABLE_INST_H

#include "xslt/xslt_defs.h"
#include "xslt/impl/Instance.h"

namespace Xslt {

class Variable;

class VariableInstance : public Instance {
public:
    XSLT_OALLOC(VariableInstance);

    VariableInstance(const Variable* variable,
                     const InstanceInit& init, Instance* p);

    const COMMON_NS::String& name() const;
    Xpath::ValueHolder*     getVariable() const;

    //! Reimplemented from Instance
    Xpath::ValueHolder*     getVariable(const COMMON_NS::String& name) const;

    void                    update() {}
    void                    updateContext(int);
    virtual GroveLib::Node* rtfRoot() const;
    GroveLib::Node*         rtf_root() const;
    void                    dump() const;

protected:
    Xpath::ValueHolderPtr   variableInst_;
};

///////////////////////////////////////////////////////////

class Param;

class ParamInstance : public VariableInstance {
public:
    XSLT_OALLOC(ParamInstance);

    void                    dump() const;
    void                    setTopValueHolder();

    ParamInstance(const Param* param,
                  const InstanceInit& init, Instance* p);
    virtual ~ParamInstance() {}
};

///////////////////////////////////////////////////////////

class WithParam;

class WithParamInstance : public VariableInstance {
public:
    XSLT_OALLOC(WithParamInstance);

    void                    dump() const;

    WithParamInstance(const WithParam* withParam,
                      const InstanceInit& init, Instance* p);
    virtual ~WithParamInstance() {}
};

} // namespace Xslt

#endif // XSLT_IMPL_VARIABLE_INST_H
