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
// Copyright (c) 2008 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "sapi/app/MimeHandler.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/common/PropertyNode.h"
#include "docview/SernaDoc.h"
#include "structeditor/SernaDragData.h"
#include "ui/MimeHandler.h"

typedef Sui::MimeHandler  SernaMimeHandler;
typedef Sui::MimeCallback SernaMimeCallback;
typedef ::SernaDoc     SernaSernaDoc;

#define MC_IMPL(mc) static_cast<SernaMimeCallback*>( \
    static_cast<MimeCallbackWrap*>(mc))
#define MH_IMPL    (static_cast<SernaMimeHandler&>(*getRep()))

#include <iostream>
#include <typeinfo>

namespace {

class MimeCallbackWrap : public Common::SernaApiRefCounted, 
                         public Sui::MimeCallback {
public:
    MimeCallbackWrap(SernaApi::MimeCallback* obj)
        : obj_(obj) {}
    virtual Common::String  mimeType() const { return obj_->mimeType(); }
    virtual CallbackClass   callbackClass() const;
    virtual bool            execute(Sui::MimeHandler&, QDropEvent*);


private:
    SernaApi::MimeCallback* obj_;
};

MimeCallbackWrap::CallbackClass MimeCallbackWrap::callbackClass() const 
{
    return (CallbackClass) obj_->callbackClass();
}

bool MimeCallbackWrap::execute(Sui::MimeHandler& mh, QDropEvent* event)
{
    SernaApi::MimeHandler smh(&mh);
    return obj_->execute(smh, event);
}

} // namespace

///////////////////////////////////////////////////////////////////////

namespace SernaApi {

MimeCallback::MimeCallback(SernaApiBase* base)
    : RefCountedWrappedObject(base)
{
}

MimeCallback::MimeCallback()
    : RefCountedWrappedObject(new MimeCallbackWrap(this))
{
}

SString MimeCallback::mimeType() const
{
    return SString();
}

double MimeCallback::priority() const
{
    return getRep() ? MC_IMPL(getRep())->priority() : 0.;
}

void MimeCallback::setPriority(double p)
{
    if (!getRep())
        return;
    MC_IMPL(getRep())->setPriority(p);
}

MimeCallback::~MimeCallback()
{
}

///////////////////////////////////////////////////////////////////////

MimeHandler::MimeHandler(SernaApiBase* base)
    : SimpleWrappedObject(base)
{
}

void MimeHandler::registerCallback(MimeCallback& mc, double prio)
{
    if (!getRep() || !mc.getRep())
        return;
    MH_IMPL.registerCallback(MC_IMPL(mc.getRep()), prio);    
}

bool MimeHandler::callback(MimeCallback::CallbackClass cc, QDropEvent* event)
{
    return getRep() ? 
        MH_IMPL.callback((Sui::MimeCallback::CallbackClass) cc, event) : false;
}

QDrag* MimeHandler::makeDrag(QWidget* dragSource) const
{
    return getRep() ? MH_IMPL.makeDrag(dragSource) : 0;
}

void MimeHandler::dragEnter(QDragEnterEvent* event)
{
    if (getRep())
        MH_IMPL.dragEnter(event);
}

QMimeData* MimeHandler::mimeData() const
{
    return getRep() ? MH_IMPL.mimeData() : 0;
}

MimeHandler::~MimeHandler()
{
}

//////////////////////////////////////////////////////////////

typedef ::SernaDocFragment SDF;
#define SDF_IMPL static_cast<SDF&>(*getRep())

SernaDocFragment::SernaDocFragment(SernaApiBase* base)
    : SimpleWrappedObject(base)
{
}

SernaDocFragment::~SernaDocFragment()
{
}

void SernaDocFragment::setFragment(const GroveDocumentFragment& df)
{
    if (getRep())
        SDF_IMPL.setFragment(
            static_cast<GroveLib::DocumentFragment*>(df.getRep()));
}

GroveDocumentFragment SernaDocFragment::fragment() const
{
    return SDF_IMPL.clipboard();
}

SString SernaDocFragment::fragMimeType(const PropertyNode& ptn)
{
    if (!ptn.getRep())
        return SString();
    return SDF::fragMimeType(
        static_cast<const Common::PropertyNode*>(ptn.getRep()));
}

SernaDocFragment SernaDocFragment::newFragment(MimeHandler& mh, 
                                               const SString& mimeType)
{
    if (!mh.getRep())
        return SernaDocFragment();
    return SDF::newFragment(static_cast<SernaMimeHandler&>(*mh.getRep()),
        mimeType);
}

SernaDocFragment SernaDocFragment::getFragment(const MimeHandler& mh, 
                                               const SString& mimeType)
{
    if (!mh.getRep())
        return SernaDocFragment();
    return SDF::fragment(static_cast<const SernaMimeHandler&>(*mh.getRep()),
        mimeType);
}

SernaDocFragment SernaDocFragment::getSeFragment(const SernaDoc& sd)
{
    if (!sd.getRep())
        return SernaDocFragment();
    return SDF::fragment(static_cast<SernaSernaDoc*>(sd.getRep()));   
}

SernaDocFragment SernaDocFragment::newSeFragment(const SernaDoc& sd)
{
    if (!sd.getRep())
        return SernaDocFragment();
    const SernaSernaDoc* ssd = static_cast<SernaSernaDoc*>(sd.getRep());
    return SDF::newFragment(ssd->mimeHandler(), 
        SDF::fragMimeType(ssd->getDsi()));
}

} // namespace SernaApi
