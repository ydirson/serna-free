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
#include "sapi/sapi_defs.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/app/impl/StructEditorData.h"
#include "sapi/app/DocumentPlugin.h"
#include "sapi/app/XsltEngine.h"
#include "sapi/grove/XpathUtils.h"
#include "sapi/grove/Grove.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/grove/GroveEditor.h"
#include "sapi/grove/GroveStripInfo.h"
#include "sapi/grove/XmlValidator.h"

#include "editableview/EditableView.h"
#include "structeditor/ElementList.h"
#include "docview/SernaDoc.h"
#include "utils/SernaCatMgr.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/XmlValidator.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "catmgr/CatalogManager.h"
#include "xslt/ResultOrigin.h"
#include "xslt/Engine.h"
#include "xslt/ExternalFunction.h"
#include "formatter/AreaPos.h"

#define SELF            ((::StructEditor*) getRep())
#define NODE_IMPL(n)    static_cast<GroveLib::Node*>(n.getRep())

typedef SernaApi::StructEditorData SeImpl;

SIMPLE_COMMAND_EVENT_IMPL(SapiSeImplElemPosChange, SeImpl)

bool SapiSeImplElemPosChange::doExecute(SeImpl* sei, Common::EventData*)
{
    sei->positionChanged();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(SapiSeImplSelectionChange, SeImpl)

bool SapiSeImplSelectionChange::doExecute(SeImpl* sei, Common::EventData*)
{
    sei->selectionChanged();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(SapiSeImplDoubleClick, SeImpl)

bool SapiSeImplDoubleClick::doExecute(SeImpl* sei, Common::EventData*)
{
    return sei->doubleClick();
}

SIMPLE_COMMAND_EVENT_IMPL(SapiSeImplTripleClick, SeImpl)

bool SapiSeImplTripleClick::doExecute(SeImpl* sei, Common::EventData*)
{
    return sei->tripleClick();
}

namespace SernaApi {

void StructEditorData::setPosWatcher(SimpleWatcher* spw)
{
    posWatcher_ = spw;
    se_->elementContextChange().unsubscribe(this);
    if (spw)
        se_->elementContextChange().subscribe
            (Common::makeCommand<SapiSeImplElemPosChange>, this);
} 

void StructEditorData::setSelectionWatcher(SernaApi::SimpleWatcher* spw)
{
    selectionWatcher_ = spw;
    se_->selectionChange().unsubscribe(this);
    if (spw) 
        se_->elementContextChange().subscribe
            (Common::makeCommand<SapiSeImplSelectionChange>, this);
} 

void StructEditorData::setDoubleClickWatcher(SernaApi::SimpleWatcher* spw)
{
    doubleClickWatcher_ = spw;
    se_->doubleClick().unsubscribe(this);
    if (spw) 
        se_->doubleClick().subscribe
            (Common::makeCommand<SapiSeImplDoubleClick>, this);
} 

void StructEditorData::setTripleClickWatcher(SernaApi::SimpleWatcher* spw)
{
    tripleClickWatcher_ = spw;
    se_->tripleClick().unsubscribe(this);
    if (spw) 
        se_->tripleClick().subscribe
            (Common::makeCommand<SapiSeImplTripleClick>, this);
} 

StructEditor::StructEditor(SernaApiBase* s, DocumentPlugin* dp)
    : UiItem(0), impl_(0)
{
    SimpleWrappedObject::setRep(s);
    if (0 == dp)
        return;
    if (!dp->sd_)
        dp->sd_ = new StructEditorData(SELF);
    impl_ = dp->sd_;
}
    
GroveEditor StructEditor::groveEditor() const
{
    return SELF->groveEditor();
}

bool StructEditor::executeAndUpdate(const Command& command)
{
    return SELF->executeAndUpdate
        (static_cast<Common::Command*>(command.getRep()));   
}

bool StructEditor::setCursorBySrcPos(const GrovePos& srcPos,
                                     const GroveNode& foHint,
                                     bool isTop)
{
    GrovePos result_pos = 
        SELF->toResultPos(srcPos, NODE_IMPL(foHint), true);
    return SELF->setCursor(srcPos, SELF->toAreaPos(result_pos), isTop);
}

GrovePos StructEditor::toResultPos(const GrovePos& srcPos,
                                   const GroveNode& foHint,
                                   bool diffuse) const
{
    return SELF->toResultPos(srcPos, NODE_IMPL(foHint), diffuse);
}
    
GrovePos StructEditor::getCheckedPos() const
{
    ::GroveEditor::GrovePos gpos;
    if (SELF->getCheckedPos(gpos))
        return gpos;
    return GrovePos();
}

bool StructEditor::getSelection(GrovePos& from,
                                GrovePos& to,
                                bool mustBeEditable)
{
    ::GroveEditor::GrovePos gfrom, gto;
    if (::StructEditor::POS_OK != SELF->getSelection(
            gfrom, gto, mustBeEditable))
        return false;
    from = gfrom;
    to   = gto;
    return true;
}
   
void StructEditor::setSelection(const GrovePos& from,
                                const GrovePos& to,
                                const GrovePos& foHint)
{
    ::Formatter::AreaPos hint_pos = SELF->toAreaPos(foHint);
    SELF->setSelection(::Formatter::ChainSelection(hint_pos, hint_pos), 
        ::GroveEditor::GroveSelection(from, to));
}

SString StructEditor::generateId(const SString& idFormat) const
{
    return SELF->generateId(idFormat);
}

void StructEditor::resetSelection()
{
    SELF->removeSelection();    
}
    
Grove StructEditor::sourceGrove() const
{
    return SELF->grove();
}

Grove StructEditor::foGrove() const
{
    return SELF->fot();
}

XsltEngine StructEditor::xsltEngine() const
{
    return SELF->xsltEngine();
}

GrovePos StructEditor::getSrcPos() const
{
    return SELF->editViewSrcPos();
}
    
GrovePos StructEditor::getFoPos() const
{
    return SELF->editViewFoPos();
}

GroveStripInfo StructEditor::stripInfo() const
{
    return SELF->stripInfo();
}

void StructEditor::setElementPositionWatcher(SimpleWatcher* sw)
{
    if (impl_)
        impl_->setPosWatcher(sw);
}

void StructEditor::setSelectionWatcher(SimpleWatcher* sw)
{
    if (impl_)
        impl_->setSelectionWatcher(sw);
}

void StructEditor::setDoubleClickWatcher(SimpleWatcher* sw)
{
    if (impl_)
        impl_->setDoubleClickWatcher(sw);
}
    
void StructEditor::setTripleClickWatcher(SimpleWatcher* sw)
{
    if (impl_)
        impl_->setTripleClickWatcher(sw);
}
    
CatalogManager StructEditor::catalogManager() const
{
    return SELF->uriMapper()->catalogManager();
}

bool StructEditor::canInsertElement(const SString& elemName, 
                                    const SString& ns,
                                    const GrovePos& gpos)
{
    if (!getRep())
        return true;
    ::ElementList& elist = SELF->elementList();
    if (gpos.isNull())
        return elist.canInsert(elemName, ns, 0);
    ::GroveEditor::GrovePos gpos2(gpos);
    return elist.canInsert(elemName, ns, &gpos2);
}

XmlValidatorProvider StructEditor::validatorProvider() const
{
    return SELF->validatorProvider();
}

/////////////////////////////////////////////////////////////////////
    
StructEditor::StructEditor(const StructEditor& se)
    : UiItem(0)
{
    operator=(se);
}

void StructEditor::setRep(SernaApiBase* b)
{
    SimpleWrappedObject::setRep(b);
}
 
StructEditor& StructEditor::operator=(const StructEditor& se)
{
    SimpleWrappedObject::setRep(se.getRep());
    impl_ = se.impl_;
    return *this;
}

StructEditor::~StructEditor()
{
    releaseRep();
}

////////////////////////////////////////////////////////////////////////

XsltEngine::XsltEngine(SernaApiBase* b)
    : SimpleWrappedObject(b)
{
}
    
void XsltEngine::registerExternalFunction(const XsltExternalFunction& fp)
{
    if (!getRep() || !fp.getRep())
        return;
    static_cast<Xslt::Engine*>(getRep())->registerExternalFunction
        (static_cast<Xslt::ExternalFunction*>(fp.getRep()));
}

void XsltEngine::deregisterExternalFunction(const XsltExternalFunction& fp)
{
    if (!getRep() || !fp.getRep())
        return;
    static_cast<Xslt::Engine*>(getRep())->deregisterExternalFunction
        (static_cast<Xslt::ExternalFunction*>(fp.getRep()));
}

////////////////////////////////////////////////////////////////////////

GroveNode StructEditor::getOrigin(const GroveNode& foNode)
{
    return const_cast<GroveLib::Node*>(Xslt::resultOrigin(NODE_IMPL(foNode)));
}
    
} // namespace SernaApi
