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
#include "structeditor/StructEditor.h"
#include "structeditor/impl/entity_utils.h"
#include "grove/Grove.h"
#include "grove/EntityReferenceTable.h"
#include "grove/EntityDeclSet.h"
#include "grove/GroveBuilder.h"
#include "groveeditor/GrovePos.h"
#include "common/PathName.h"
#include "common/Url.h"

using namespace Common;

GroveLib::Grove* get_current_grove(StructEditor* se)
{
    GroveEditor::GrovePos pos = se->editViewSrcPos();
    if (pos.isNull())
        return se->grove();
    return GroveEditor::get_current_grove(pos.getErs());
}

Common::String get_current_entity_url(const StructEditor* se)
{
    GroveEditor::GrovePos pos = se->editViewSrcPos();
    if (pos.isNull())
        return se->grove()->topSysid();
    return pos.getErs()->xmlBase();
}

Common::String get_docname(StructEditor* se)
{
    return PathName(get_current_grove(se)->topSysid()).filename();
}

bool has_entity_loop(const GroveLib::EntityReferenceStart* ers,
                     const Common::String& name)
{
    const GroveLib::EntityReferenceTable::ErtEntry* ee =
        ers->document()->ert()->lookup(ers->entityDecl());
    if (0 == ee)
        return false;
    for (uint i = 0; i < ee->numOfRefs(); ++i) {
        const GroveLib::EntityReferenceStart* cur_ers = ee->node(i);
        for (; cur_ers; cur_ers = CONST_ERS_CAST(cur_ers->getSectParent()))
            if (cur_ers->entityDecl() && cur_ers->entityDecl()->name() == name)
                return true;
    }
    return false;
}

void set_grovebuilder_lock_flags(const Url& doc_url,
                                 GroveLib::GroveBuilder* gb)
{
    using namespace GroveLib;
    // apg: should check for other protocols, too?
    String lock_type(NOTR(NOTR("check-lock")));
    if (doc_url[Url::PROTOCOL] == NOTR("http") || doc_url[Url::PROTOCOL] == NOTR("https")) {
        lock_type = config().root()->
            getSafeProperty("dav/protocols/http/lock-type")->getString();
        if (NOTR("lock") == lock_type)
            gb->setFlags((GroveBuilder::Flags)
                (gb->flags() | GroveBuilder::doLocks));
        if (NOTR("check-lock") == lock_type)
            gb->setFlags((GroveBuilder::Flags)
                (gb->flags() | GroveBuilder::checkLocks));
    } else if (doc_url[Url::PROTOCOL] == NOTR("file"))
            gb->setFlags((GroveBuilder::Flags)
                (gb->flags() | GroveBuilder::checkLocks));
}

