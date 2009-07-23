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

#include "xs/datatypes/SimpleDerivedType.h"

#include "grove/Nodes.h"
#include "grove/ChoiceNode.h"
#include "xs/XsMessages.h"
#include "xs/XsMessages.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"
#include "qregexp.h"
#include "qstringlist.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

SimpleDerivedType::SimpleDerivedType(DerivationMethod derivedBy,
                                     const Origin& origin,
                                     const NcnCred& cred)
    : XsSimpleTypeImpl(XsSimpleTypeImpl::DERIVED, origin, cred),
      derivationMethod_(derivedBy),
      trim_(String::null()),
      trimResult_(String::null()),
      pyPreFunc_(String::null()),
      pyPostFunc_(String::null()),
      final_(SimpleDerivedType::NONE)
{
}

SimpleDerivedType::~SimpleDerivedType()
{
}

SimpleDerivedType::DerivationMethod SimpleDerivedType::derivationMethod() const
{
    return derivationMethod_;
}

void SimpleDerivedType::setFinal(SimpleDerivedType::DerivationMethod final)
{
    final_ = final;
}

SimpleDerivedType::DerivationMethod SimpleDerivedType::final() const
{
    return final_;
}

void SimpleDerivedType::addBaseType(const XsTypeInst& stype)
{
    baseVector_.push_back(stype);
}

void SimpleDerivedType::setBaseType(const XsTypeInst& stype)
{
    baseType_ = stype;
}

void SimpleDerivedType::setTrim(String& trim, String& trimResult)
{
    trim_ = trim;
    trimResult_ = trimResult;
}

const String& SimpleDerivedType::trim() const
{
    return trim_;
}

const String& SimpleDerivedType::trimResult() const
{
    return trimResult_;
}

String SimpleDerivedType::getTrimmed(Schema* schema,
                                     const GroveLib::Node* o,
                                     const String& source) const
{
    String src = source;
    if (!trim().isEmpty()) {
        if (!trimResult().isEmpty()) {
            QRegExp rx(trim().qstring());
            if (!rx.isValid()) {
                schema->mstream() << XsMessages::regexError
                                  << Message::L_ERROR
                                  << NOTR("trim is not valid")
                                  << DV_ORIGIN(o);
            }
            int pos = rx.search(source.qstring());
            if (0 > pos) {
                schema->mstream() << XsMessages::regexError
                                  << Message::L_INFO
                                  << NOTR("trim does not match any string")
                                  << DV_ORIGIN(o);;
            }
            QStringList list = rx.capturedTexts();

            String temp;
            int start = 0;
            pos = trimResult().find('\\');
            if (-1 == pos) {
                temp = trimResult();
            }
            else {
                while (-1 != pos) {
                    temp += trimResult().mid(start, pos - start);
                    if (trimResult()[pos+1].isDigit()) {
                        String s = trimResult()[pos+1];
                        int n = s.toInt();
                        if (n > int(list.count())) {
                            schema->mstream() << XsMessages::regexError
                                              << Message::L_ERROR
                                              << "incorrect number of trim regex"
                                              << DV_ORIGIN(o);;
                        }
                        else
                            temp += list[n];
                        start = pos+2;
                    }
                    else {
                        temp += '\\';
                        start = pos++;
                    }
                    pos = trimResult().find('\\', start);
                }
            }
            src = temp;
        }
        else
            src = "";
    }
    return src;
}

GroveLib::Text* make_text_choice(const XsType* xst)
{
    const XsSimpleTypeImpl* xsti = 0;
    if (0 == xst || !(xsti = xst->getSimpleType()))
        return new GroveLib::Text;
    const SimpleDerivedType* sdt = xsti->asConstSimpleDerivedType();
    if (0 == sdt)
        return new TextChoiceNode(TextChoiceNode::SIMPLE_CHOICE);
    EnumList lst;
    bool has_list = const_cast<SimpleDerivedType*>(sdt)->
        fillPossibleEnums(lst);
    return has_list ? new TextChoiceNode(TextChoiceNode::ENUM_CHOICE) 
        : new TextChoiceNode(TextChoiceNode::SIMPLE_CHOICE);
}

void SimpleDerivedType::setPyPreFunc(String& funcName)
{
    pyPreFunc_ = funcName;
}

const String& SimpleDerivedType::pyPreFunc() const
{
    return pyPreFunc_;
}

void SimpleDerivedType::setPyPostFunc(String& funcName)
{
    pyPostFunc_ = funcName;
}

const String& SimpleDerivedType::pyPostFunc() const
{
    return pyPostFunc_;
}

PRTTI_IMPL(SimpleDerivedType)
PRTTI_BASE_STUB(SimpleDerivedType, Restriction)
PRTTI_BASE_STUB(SimpleDerivedType, List)
PRTTI_BASE_STUB(SimpleDerivedType, Union)

XS_NAMESPACE_END
