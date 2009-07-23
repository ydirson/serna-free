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

#include "xs/xs_defs.h"
#include "xs/SchemaDefaultMessenger.h"
#include "common/MessageUtils.h"
#include "xs/XsMessages.h"
#include "xs/Piece.h"
#include "grove/Nodes.h"
#include <iostream>

USING_COMMON_NS

XS_NAMESPACE_BEGIN
void SchemaDefaultMessenger::dispatch(RefCntPtr<Message>& m)
{
    if (m->severityLevel() > 0)
        std::cerr << m->severityLevelName() << ": ";

    String os = m->format(BuiltinMessageFetcher::instance());
    if (static_cast<UintIdMessage*>(m.pointer())->messageId() < XsMessages::VALIDATION_ERRORS_FOLLOW)
        std::cerr << NOTR("SCHEMA: ") << os << std::endl;
    else
        std::cerr << NOTR("VALIDATION: ") << os;

    const GroveLib::CompositeOrigin* origin = 0;
    for (ulong i = 0; i < m->nArgs(); ++i) {
        const MessageArgBase* ma = m->getArg(i);
        if (ma->type() == MessageArgBase::USER_DEFINED + 1) {
            origin = &static_cast
                <const MessageArg<GroveLib::CompositeOrigin>*>(ma)->value();
            break;
        }
    }
    if (0 == origin) {
        std::cerr << std::endl;
        return;
    }
    if (origin->docOrigin())
        std::cerr << NOTR("(Source: ") << origin->docOrigin()->asString() << NOTR(")\n");
    if (origin->moduleOrigin())
        std::cerr << NOTR("(Schema: ") << origin->moduleOrigin()->asString() << NOTR(")\n");
}

XS_NAMESPACE_END
