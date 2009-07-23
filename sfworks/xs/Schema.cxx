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
#include "xs/xs_defs.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/Origin.h"
#include "xs/XsMessages.h"
#include "xs/XsElement.h"
#include "xs/XsType.h"
#include "xs/EnumFetcher.h"
#include "xs/ExtEnumCache.h"
#include "xs/ImportMap.h"
#include "xs/SchemaDefaultMessenger.h"
#include "xs/XsNodeExt.h"
#include "common/MessageUtils.h"
#include "common/PropertyTree.h"
#include "grove/Nodes.h"
#include "grove/IdManager.h"

#include "xs/complex/Particle.h"
#include "xs/complex/ElementParticle.h"
#include "xs/complex/InputTokenSequence.h"
#include "xs/complex/ComplexContent.h"
#include "xs/complex/Connector.h"
#include <list>

USING_COMMON_NS
USING_XS_NAMESPACE

void Schema::setValidationFlags(int f)
{
    validationFlags_ = (vFlags)f;
    if ( validationFlags_ & Schema::beSilent)
       setMessenger(new SilentMessenger);
}

void Schema::cleanInvalidities()
{
    invalidities_.clear();
}

const String& Schema::targetNsUri() const
{
    return xsd_->targetNsUri();
}

void Schema::parse(GROVE_NAMESPACE::Element* root)
{
    this->xsi_->parseSchema(root, this);
}

void Schema::setMessenger(Messenger* m)
{
    mstream_.setMessenger(m);
}

void Schema::setEnumFetcher(EnumFetcher* f)
{
    fetcher_ = f;
}

RefCntPtr<Schema> Schema::cloneDefault() const
{
    Schema* s = new Schema(getMessenger());
    s->xsd_ = xsd_;
    s->xsi_ = xsi_;
    return s;
}

RefCntPtr<Schema> Schema::clone() const
{
    RefCntPtr<Schema> s = cloneDefault();
    s->validationFlags_ = validationFlags_;
    return s;
}

Schema::Schema(Messenger* messenger)
    : validationFlags_(defaultMode), mstream_(XsMessages::getFacility()),
      pstream_(0), commandMaker_(0), reqAttrsProvider_(0),
      idq_(new IdrefCheckQueue), catMgr_(0)
{
    enumCache_ = new ExtEnumCache;
    invalidities_ = new STQueue<GROVE_NAMESPACE::Invalidity>;
    xsi_ = new XsDataImpl(this);
    xsd_ = xsi_.pointer();
    mstream_.setMessenger(messenger);
}

bool Schema::validate(GROVE_NAMESPACE::Element* root)
{
    Schema* s = xsi_->importedSchema(root->xmlNsUri());
    XsElement* et;
    if (s)
        et = s->xsd()->elementSpace().lookup
            (root->localName(), root->xmlNsUri());
    else
        et = xsd_->elementSpace().lookup(root->localName(), root->xmlNsUri());
    if (0 == et) {
        mstream_ << XsMessages::elementTypeNotFound  << Message::L_ERROR
          << root->xmlNsUri() << root->localName()
          << SRC_ORIGIN(root);
        return false;
    }
    return et->validate(this, root, true) && check_delayed_idrefs();
}

bool Schema::partialValidate(GROVE_NAMESPACE::Element* elem, bool dontCreate)
{
    if (!(validationFlags() & editMode)) {
        mstream() << XsMessages::partialOnlyInEditMode
            << Message::L_ERROR << SRC_ORIGIN(elem);
        return false;
    }
    const XsElement* xse = XsNodeExt::xsElement(elem);
    if (0 == xse) {
        mstream() << XsMessages::mustValidateParent
            << Message::L_ERROR << SRC_ORIGIN(elem);
        return false;
    }
    MessageStream* tmp = pstream();
    setProgressStream(0);
    vFlags origFlags = validationFlags_;
    if (dontCreate)
        validationFlags_ = (vFlags) (validationFlags_ | dontMakeElems);
    bool ok = xse->validate(this, elem, true);
    if (dontCreate)
        validationFlags_ = origFlags;
    setProgressStream(tmp);
    return ok && check_delayed_idrefs();
}

bool Schema::makeSkeleton(const String& elementName,
                          GROVE_NAMESPACE::Grove* referenceGrove,
                          GROVE_NAMESPACE::ElementPtr& ep,
                          const GROVE_NAMESPACE::Element* pe)
{
    XS_NAMESPACE::XsElement* et;
    if (elementName.isNull() || elementName.isEmpty())
        et = xsd_->elementSpace().getFirst();
    else
        et = xsd_->elementSpace().lookup(elementName);
    if (0 == et) {
        mstream_ << XsMessages::elementTypeNotFound << Message::L_ERROR
          << xsd()->targetNsUri() << elementName;
        return false;
    }
    return et->makeSkeleton(this, referenceGrove, ep, pe);
}

void Schema::getRootElementList(Common::PropertyNode* root) const
{
    Vector<Component*> component_vec;
    component_vec.reserve(1024);
    xsd_->elementSpace().getAllComponents(component_vec);
    for (uint i = 0; i < component_vec.size(); ++i) {
        const NcnCred* cred = component_vec[i]->constCred();
        root->appendChild(new PropertyNode(cred->name(), cred->xmlns()));
    }
}

void Schema::getAttrSpecsForTopElement(const String& localName,
                                       const String& uri,
                                       PropertyNode* specList,
                                       const GroveLib::Element* elem) const
{
    const Schema* s = xsi_->importedSchema(uri);
    if (0 == s)
        s = this;
    XsElement* et = s->xsd()->elementSpace().lookup(localName, uri);
    if (0 == et)
        return;
    XsNodeExt::getAttrs(specList, et, elem); // todo: pass Grove
}

bool Schema::validate(const String& dt,
                      const String& data,
                      String* result)
{
    XS_NAMESPACE::XsType* tt = xsd_->typeSpace().lookup(dt);
    if (0 == tt || tt->typeClass() != XS_NAMESPACE::XsType::simpleType) {
        mstream_ << XsMessages::simpleTypeNotFound << Message::L_ERROR
                 << xsd()->targetNsUri() << dt;
        return false;
    }
    return tt->validate(this, data, result) && check_delayed_idrefs();
}
    
bool Schema::validate(GroveLib::Element* elem, int flags) const
{
    Schema* schema = const_cast<Schema*>(this);
    MessageStream* pstream = schema->pstream();
    int vflags = 0;
    int savedFlags = validationFlags();
    if (flags & OPEN_MODE)
        vflags |= Schema::makeChanges | Schema::fixupComplex;
    if (flags & EDIT_MODE)
        vflags |= Schema::editMode;
    if (flags & DONT_MAKE_ELEMS)
        vflags |= Schema::dontMakeElems;
    if (flags & HIDE_PROGRESS)
        schema->setProgressStream(0);
    if (flags & XINCLUDE)
        vflags |= Schema::dontCheckIdRefs;
    if (flags & SILENT_MODE)
        vflags |= Schema::beSilent;
    schema->setValidationFlags(vflags);
    bool ok = false;
    if (flags & PARTIAL)
        ok = schema->partialValidate(elem, flags & DONT_MAKE_ELEMS);
    else
        ok = schema->validate(elem);
    schema->setValidationFlags(savedFlags);
    schema->setProgressStream(pstream);
    return ok;
}

void Schema::idrefDelayedCheck(const GroveLib::Attr* a)
{
    idq_->push_back(const_cast<GroveLib::Attr*>(a));
}

bool Schema::check_delayed_idrefs()
{
    bool ok = true;
    if (!(validationFlags() & dontCheckIdRefs)) {
        for (uint i = 0; i < idq_->size(); ++i) {
            GroveLib::Attr* a = (*idq_)[i].pointer();
            if (!a->element())
                continue;
            GroveLib::Grove* const grove = a->grove();
            if (!grove || !grove->idManager())
                continue;
            if (!grove->idManager()->lookupElement(a->value())) {
                mstream_ << XsMessages::notId << Message::L_ERROR
                    << a->value() << SRC_ORIGIN(a);
                    ok = false;
            }
        }
    } 
    idq_->clear();
    return ok;
}

void Schema::parse2()
{
    xsi()->parse2(this);
}

void Schema::resolveComponents()
{
    xsi()->resolveComponents();
}

void Schema::importedSchemas(Vector<Schema*>& sv)
{
    resolveComponents();
    xsi()->importMap()->importedSchemas(sv);
}

MessageStream& Schema::mstream()
{
    return mstream_;
}

void Schema::setProgressStream(COMMON_NS::MessageStream* pstream)
{
    pstream_ = pstream;
}

COMMON_NS::MessageStream* Schema::pstream()
{
    return pstream_;
}

Schema::~Schema()
{
}

