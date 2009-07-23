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
#include "utils/NameChecker.h"
#include "common/String.h"
#include "common/Vector.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityDecl.h"

#include <QApplication>
#include <QMessageBox>
#include <algorithm>

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace {

bool is_bad_name_char(const Char& ch)
{
    return (':' == ch || ' ' == ch || '\t' == ch || '\n' == ch || '\r' == ch ||
            (!ch.isLetter() && !ch.isDigit() && '$' != ch  &&
             '_' != ch && '-' != ch && '.' != ch));
}

};

bool NameChecker::isValidQname(const String& name)
{
    int colon = name.find(':');
    if (-1 != colon)
        return (isValidPrefix(name.left(colon)) &&
                isValidLocalName(name.mid(colon + 1)));
    return isValidLocalName(name);
}

bool NameChecker::isValidLocalName(const String& name)
{
    //if (starts_with(name.lower(), "xml"))
    //    return false;
    return isValidPrefix(name);
}

bool NameChecker::isValidPrefix(const String& name)
{
    if (name.isEmpty())
        return false;
    if (!name[0].isLetter() && ('_' != name[0]))
        return false;
    const Char* bad_char =
        std::find_if(name.begin() + 1, name.end(), is_bad_name_char);
    if (name.end() != bad_char)
        return false;
    return true;
}
/*
bool checkNamechar(const Char& ch)
{
    return (!ch.isLetter() && !ch.isDigit() && ':' != ch  &&
            '_' != ch && '-' != ch && '.' != ch);
}

bool NameChecker::checkName(const String& name)
{
    if (name.isEmpty())
        return true;
    if (0 <= name.find(' ') || 0 <= name.find('\t') ||
        0 <= name.find('\n') || 0 <= name.find('\r'))
        return false;
    if (starts_with(name.lower(), "xml:"))
        return true;
    if ((starts_with(name.lower(), "xml")) ||
       ((!name[0].isLetter()) && ('_' != name[0])))
        return false;
    if (1 == name.length())
        return true;
    const Char* result = std::find_if(name.begin() + 1, name.end(),
                                      checkNamechar);
    if (result != name.end())
        return false;
    //TODO: exact checking for Combining chars and Extenders.
    return true;
}
*/
bool NameChecker::checkEntity(const COMMON_NS::String& name,
                             GroveLib::EntityDeclSet& entities)
{
    if (!isValidQname(name)) {
        QMessageBox::critical(QApplication::activeWindow(), tr("Error"),
                              tr("Invalid entity name: %1").arg(name));
        return false;
    }
    const EntityDecl* decl = entities.lookupDecl(name);
    if (0 == decl)
        return true;
    if (decl->declOrigin() == EntityDecl::dtd) {
        int v = QMessageBox::warning(QApplication::activeWindow(),
                                     tr("Warning"),
                                     tr("This operation will shadow "
                                        "entity declared in external subset"),
                                     QMessageBox::Ok|QMessageBox::Cancel);
        return 1 == v;
    }
    QMessageBox::critical(QApplication::activeWindow(), tr("Error"),
        tr("Entity with this name already exists:%1").arg(name));
    return false;
}
