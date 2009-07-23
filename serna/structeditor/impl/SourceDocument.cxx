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
#include "structeditor/se_defs.h"
#include "structeditor/StructEditor.h"
#include "structeditor/StructDocument.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/XsUtils.h"
#include "structeditor/impl/SchemaCommandMaker.h"
#include "structeditor/impl/entity_utils.h"

#include "docview/SernaDoc.h"
#include "docview/MessageView.h"
#include "docview/MessageTree.h"
#include "docview/DocumentStateEventData.h"

#include "utils/Properties.h"
#include "utils/DocTemplate.h"
#include "utils/Properties.h"
#include "utils/Config.h"
#include "utils/file_utils.h"
#include "utils/message_utils.h"
#include "utils/DocSrcInfo.h"
#include "utils/SernaMessages.h"
#include "utils/SernaCatMgr.h"
#include "utils/ElementHelp.h"

#include "grove/XmlNs.h"
#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "grove/Grove.h"
#include "grove/IdManager.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/ElementMatcher.h"
#include "grove/SectionSyncher.h"
#include "grove/Origin.h"
#include "grove/udata.h"
#include "grove/xinclude.h"
#include "spgrovebuilder/SpGroveBuilder.h"
#include "xslt/GroveMatchPattern.h"
#include "ui/UiItemSearch.h"

#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/XTreeIterator.h"
#include "common/DiffuseSearch.h"
#include "common/Url.h"
#include "common/StringCvt.h"
#include "common/PropertyTreeEventData.h"

#include "catmgr/CatalogManager.h"


#include "xs/XsMessages.h"
#include "xs/Origin.h"
#include "xs/XsNodeExt.h"
#include "xs/Schema.h"
#include "xs/SchemaResource.h"
#include "xs/XsElement.h"
#include "xs/XsValidatorProvider.h"

using namespace Common;
using namespace GroveLib;
using namespace FileUtils;
using namespace AttributesSpace;

class SernaValidatorProvider : public Xs::XsValidatorProvider,
                               public Xs::SchemaResourceBuilder {
public:
    SernaValidatorProvider(MessageTreeNode* mtn,
                            Common::MessageStream* pstream,
                            PropertyNode* pdsi);

    DocUriMapper&   docUriMapper() { return docUriMapper_; }

    virtual XmlValidatorPtr getValidator(const Grove*, bool* found) const; 

private:
    PropertyNode* pdsi_;
    DocUriMapper                   docUriMapper_;  
    Common::OwnerPtr<DocUriMapper> tmplUriMapper_;
};

SernaValidatorProvider::SernaValidatorProvider(MessageTreeNode* mtn,
  Common::MessageStream* pstream, PropertyNode* pdsi)
    : XsValidatorProvider(static_cast<Xs::SchemaResourceBuilder&>(*this)),
          SchemaResourceBuilder(mtn, pstream),
          pdsi_(pdsi)
{
    docUriMapper().catalogManager()->setMessenger(mtn);
}

XmlValidatorPtr 
SernaValidatorProvider::getValidator(const Grove* grove, bool* found) const
{
    using namespace DocSrcInfo;

    bool ok = false;
    if (found)
        *found = true;
    SernaValidatorProvider* svp = const_cast<SernaValidatorProvider*>(this);
    svp->setCatalogManager(grove->groveBuilder()->getCatalogManager());
    XmlValidatorPtr xv = XsValidatorProvider::getValidator(grove, &ok);
    if (ok)
        return xv;
    PropertyTree pt;
    if (!DocTemplate::DocTemplateHolder::getDocProperties
        (grove->topSysid(), pt.root()))
            return xv;
    DocSrcInfo::resolve_dsi(pt.root());
    DBG(SE.TEST) << "ValidatorProvier: matched doctemplate:\n";
    DBG_IF(SE.TEST) pt.root()->dump();
    DBG(SE.TEST) << "****************************************\n";
    const PropertyNode* pn = pt.root()->getProperty(RESOLVED_CATALOGS);
    const PropertyNode* pn2 = pdsi_->getProperty(RESOLVED_CATALOGS);
    if (pn && !pn->getString().isEmpty() && 
        pn->getString() != pn2->getString()) {
            DBG(SE.TEST) << "ValidatorProvider: setting uri mapper, catpath="
                << pn->getString() << std::endl;
            if (!tmplUriMapper_) {
                svp->tmplUriMapper_ = new DocUriMapper;
                svp->tmplUriMapper_->prependCatalogs(pn->getString());
            }
            svp->setCatalogManager(tmplUriMapper_->catalogManager());
    }
    String schema_url = pt.root()->
        getSafeProperty(DocSrcInfo::RESOLVED_SCHEMA_PATH)->getString();
    DBG(SE.TEST) << "ValidatorProvider:: resolved xspath=<" 
        << schema_url << ">\n";
    RefCntPtr<Xs::SchemaResource> xsres = 
        dynamic_cast<Xs::SchemaResource*>(Uri::uriManager().
            getTopResource(NOTR("xml-schema"), *this, schema_url));
    if (xsres.isNull())
        return xv;
    xsres->schema()->parse2();
    return xsres->schema()->clone().pointer();
}

DocUriMapper* SourceDocument::uriMapper() const
{
    return &validatorProvider_->docUriMapper();
}

XmlValidatorProvider* SourceDocument::validatorProvider() const
{
    return validatorProvider_.pointer();
}

class RootElementDialog;
class SetRequiredAttributes;

bool SourceDocument::makeRootElement(const PropertyNode* dsi)
{
    PropertyTreeEventData iparam, result;
    if (schema())
        schema()->getRootElementList(iparam.root()->
            makeDescendant("all-elements"));
    iparam.root()->makeDescendant(DocSrcInfo::ROOT_ELEMENTS, 
        dsi->getSafeProperty(DocSrcInfo::ROOT_ELEMENTS)->getString());
    sernaDoc()->showStageInfo();
    if (!makeCommand<RootElementDialog>(&iparam)->execute(sernaDoc(), &result))
        return false;
    String name = result.root()->getSafeProperty("root-element")->getString();
    String uri = result.root()->getSafeProperty("uri")->getString();
    String prefix = result.root()->getSafeProperty("prefix")->getString();

    String qname = prefix.isEmpty() ? name : (prefix + ":" + name);
    Element* element = new Element(qname);
    grove()->document()->appendChild(element);
    XmlNs::makePredefinedNamespaces(grove());
    if (!prefix.isNull())
        element->addToPrefixMap(prefix, uri);
    return true;
}

void SchemaRequiredAttrs::fillAttributeValues(const PropertyNode* inSpecs,
                                              PropertyNode* outSpecs,
                                              const Element* elem) const
{
    srd_->sernaDoc()->showStageInfo();
    PropertyTreeEventData attrs;
    PropertyNodePtr attr_specs = inSpecs->copy(true);
    attr_specs->setName(ATTR_SPEC_LIST);
    attrs.root()->appendChild(attr_specs.pointer());
    attrs.root()->makeDescendant(
        ElementSpace::ELEMENT_NAME)->setString(elem->nodeName());
    if (!makeCommand<SetRequiredAttributes>(&attrs)->execute(srd_, &attrs))
        return;
    outSpecs->removeAllChildren();
    outSpecs->merge(attrs.root()->makeDescendant(EXISTING_ATTRS));
}

SourceDocument::SourceDocument(SernaDoc* doc)
    : doc_(doc),
      messageStream_(SernaMessages::getFacility(), messageTree()),
      requiredAttrsProvider_(new SchemaRequiredAttrs(this)),
      isValidationOn_(false)
{
}

PropertyNode* SourceDocument::getDsi() const
{
    return sernaDoc()->getDsi();
}

SourceDocument::~SourceDocument()
{
}

void SourceDocument::initElementHelp(PropertyNode* dsi)
{
    helpHandle_ = get_help_handle(dsi->getSafeProperty(
        DocSrcInfo::RESOLVED_ELEM_HELP_FILE)->getString());
}

void SourceDocument::setCatalogs(PropertyNode* dsi)
{
    validatorProvider_ = new SernaValidatorProvider
        (messageTree(), sernaDoc()->progressStream(), dsi);
    const PropertyNode* pn =
        dsi->getProperty(DocSrcInfo::RESOLVED_CATALOGS);
    if (pn) {
        DDBG << "Prepend catalogs: " << pn->getString() << std::endl;
        uriMapper()->prependCatalogs(pn->getString());
    }
    validatorProvider_->setCatalogManager
        (validatorProvider_->docUriMapper().catalogManager());
}

// fill required data in new grove
static bool fill_new_grove(PropertyNode* dsi, 
                           GrovePtr& grove,
                           SourceDocument* srd)
{
    const String& sysid =
        dsi->getSafeProperty(DocSrcInfo::DOCINFO_SYSID)->getString();
    const String& pubid =
        dsi->getSafeProperty(DocSrcInfo::DOCINFO_PUBID)->getString();
    const String& encoding =
        dsi->getSafeProperty(DocSrcInfo::ENCODING)->getString();
    const String& doc =
        dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    grove->setTopSysid(doc);
    OwnerPtr<GroveBuilder> builder(srd->makeGroveBuilder(dsi));
    builder->setCatalogManager(srd->uriMapper()->catalogManager());
    builder->setMessenger(srd->messageTree());
    builder->setEncoding(Encodings::encodingByName(encoding));
    set_grovebuilder_lock_flags(doc, builder.pointer());
    builder->setValidatorProvider(srd->validatorProvider());
    if (!sysid.isEmpty()) {
        srd->sernaDoc()->showStageInfo(tr("Parsing %1").arg(doc));
        RefCntPtr<GroveLib::ExternalEntityDecl> decl =
            new GroveLib::ExternalEntityDecl;
        decl->setName(grove->document()->documentElement()->nodeName());
        decl->setDeclType(GroveLib::EntityDecl::doctype);
        decl->setDataType(GroveLib::EntityDecl::sgml);
        decl->setDeclOrigin(GroveLib::EntityDecl::prolog);
        decl->setDeclModified();
        decl->setSysid(sysid);
        decl->setPubid(pubid);
        grove->setDoctypeName(decl->name());
        grove->setDoctypeEntity(decl.pointer());
        String plain_doc;
        grove->saveAsXmlString(plain_doc);
        GroveBuilder* sp_builder = srd->makeGroveBuilder(dsi);
        sp_builder->setCatalogManager(srd->uriMapper()->catalogManager());
        sp_builder->setSearchPath(
            config().getProperty("vars/dtds")->getString());
        sp_builder->setMessenger(srd->messageTree());
        sp_builder->setEncoding(Encodings::encodingByName(encoding));
        sp_builder->setValidatorProvider(srd->validatorProvider());
        grove = sp_builder->buildGrove(plain_doc, String(), false);
        if (grove.isNull() || !grove->document()->documentElement()) {
            srd->sernaDoc()->showMessageBox(
                SernaDoc::MB_CRITICAL, tr("Cannot parse document skeleton"),
                tr("Cannot parse document skeleton.\n"
                   "This may happen e.g. if you have syntax errors"
                   " in DOCTYPE string."), tr("&Ok"));
                return false;
        }
        grove->setTopSysid(doc);
        grove->setDoctypeName(decl->name());
    }
    DDBG << "Created new grove, flags=" << builder->flagsAsString() << "\n";
    grove->setGroveBuilder(builder.release());
    if (encoding == NOTR("UTF-8"))
        dsi->getProperty(DocSrcInfo::ENCODING)->remove();
    if (!grove->idManager()) {
        grove->setIdManager(new GroveLib::IdManager);
        grove->idManager()->enable(grove->document());
    }
    return true;
}

void SourceDocument::firstValidate()
{
    if (!schema())
        return;
    ElementPtr top_elem(grove()->document()->documentElement());
    if (top_elem)
        XmlNs::makePredefinedNamespaces(grove());
    schema()->validate(top_elem.pointer());
    schema()->setValidationFlags(Schema::editMode|Schema::makeChanges);
}

void SourceDocument::createGrove(PropertyNode* dsi)
{
    setCatalogs(dsi);
    grove_ = new Grove;
    const PropertyNode* pn =
        dsi->getProperty(DocSrcInfo::TEMPLATE_DOCROOTGROVE);
    if (pn) {
        const DocTemplate::GroveProperty* grove_prop =
            dynamic_cast<const DocTemplate::GroveProperty*>(pn);
        if (grove_prop) {
            Grove* sdt_grove = grove_prop->grove();
            GroveLib::Document* sdt_doc = 0;
            if (sdt_grove) 
                sdt_doc = sdt_grove->document();
            if (sdt_doc && sdt_doc->firstChild())
                grove()->document()->appendChild(sdt_doc->firstChild()->
                    copyAsFragment(sdt_doc->lastChild()));
        }
    }
    sernaDoc()->showStageInfo();
}

bool SourceDocument::initGrove(PropertyNode* dsi)
{
    if (!useSchema(dsi)) 
        return false;
    if (!grove()->document()->documentElement()) {
        if (!makeRootElement(dsi)) {
            grove_ = 0;
            schema_ = 0;
            return false;
        }
    }
    if (!fill_new_grove(dsi, grove_, this))
        return false;
    processEntities();
    SchemaCommandMaker scm;
    if (schema()) {
        schema()->setCommandMaker(&scm);
        schema()->setRequiredAttrsProvider(requiredAttrsProvider());
    }
    firstValidate();
    if (schema()) {
        schema()->setRequiredAttrsProvider(0);
        schema()->setCommandMaker(0);
    }
    initElementHelp(dsi);
    return true;
}

bool SourceDocument::useSchema(PropertyNode* dsi)
{
    String schema_url = dsi->getSafeProperty
        (DocSrcInfo::RESOLVED_SCHEMA_PATH)->getString();
    isValidationOn_ = false;
    if (!schema_url.isEmpty()) {
        DBG(SE.TEST) << "Using Schema: " << schema_url << std::endl;
        RefCntPtr<Xs::SchemaResource> xsres =
            dynamic_cast<Xs::SchemaResource*>(Uri::uriManager().
                getTopResource(NOTR("xml-schema"), *validatorProvider_,
                schema_url));
        if (xsres.isNull()) {
            if (0 != sernaDoc()->showMessageBox(
                SernaDoc::MB_CRITICAL, tr("XML Schema Loading Error"),
                tr("<qt><nobr>Cannot load schema: %1"
                "<br/><b>Continue with validation switched off?"
                "</b></nobr></qt>").arg(schema_url),
                tr("Yes"), tr("No")))
                    return false;
                // clear grovebuilder messages; otherwise open_document
                // will be confused and will switch to text mode
                messageTree()->clearMessagesFrom(1);
        }
        else {
            xsres->schema()->parse2();
            xsres->schema()->setProgressStream(0);
            schema_ = xsres->schema()->clone();
            schema()->setProgressStream(sernaDoc()->progressStream());
            validatorProvider_->setDefaultSchema(schema());
            if (messageTree()->firstChild()) {
                String err_msg;
                int max_msgs = 20;
                XTreeDocOrderIterator<MessageTreeNode> mit(messageTree());
                for (; mit.node() && max_msgs >= 0; ++mit, --max_msgs) {
                    if (mit.node()->getMessage())
                        err_msg += NOTR("<nobr>") + MessageUtils::msg_str(
                            mit.node()->getMessage()) + NOTR("</nobr><br/>");
                }
                sernaDoc()->showMessageBox(
                    SernaDoc::MB_WARNING, tr("XML Schema Warning"),
                    tr("<qt><nobr><b>Failed to parse schema:</b><br/>%1"
                    "<br/><b>Validation will not be performed.</b>"
                    "</nobr><br/>Schema: %2").arg(err_msg).arg(schema_url),
                    tr("Ok"));
                messageTree()->removeAllChildren();
                xsres->remove();
                schema_ = 0;
                return true;
            }
            schema()->setMessenger(messageTree());
            schema()->setValidationFlags(Schema::makeChanges|
                                        Schema::fixupComplex);
        }
        isValidationOn_ = true;
    }
    return true;
}

void SourceDocument::processEntities()
{
    EntityDeclSet::iterator it = grove_->entityDecls()->begin();
    for (; it != grove_->entityDecls()->end(); ++it) {
        if ((*it)->declOrigin() != EntityDecl::invalidDeclOrigin)
            continue;
        messageStream() << SernaMessages::undeclaredEntity << (*it)->name();
        (*it)->setReadOnly(true);
    }
}


GroveLib::GroveBuilder* 
SourceDocument::makeGroveBuilder(const PropertyNode* dsi)
{
    int gb_flags = GroveBuilder::noExtSubsetDecls|GroveBuilder::noAttrGroups|
        GroveBuilder::noSSep|GroveBuilder::lineInfo;
    const PropertyNode* xinc_prop = 
        dsi->getProperty(DocSrcInfo::PROCESS_XINCLUDE);
    if (!xinc_prop || xinc_prop->getBool())
        gb_flags |= GroveBuilder::processXinclude;
    SpGroveBuilder* builder = new SpGroveBuilder(gb_flags);
    PropertyNode* scope_defs = dsi->getProperty(IdManager::SCOPE_DEFS);
    if (scope_defs) 
        builder->setIdManagerParams(scope_defs,
            dsi->getProperty(DocSrcInfo::NS_MAP),
            new Xslt::GroveMatchPatternFactory);
    return builder;
}

bool SourceDocument::createGroveFromTemplate(const PropertyNode* dt,
                                             const String& path,
                                             const String& skelName)
{
    PropertyNodePtr dsi(dt->copy(true));
    DocSrcInfo::resolve_dsi(dsi.pointer());
    dsi->makeDescendant(DocSrcInfo::DOC_PATH, path, true);
    PropertyNode* pn = dsi->firstChild();
    PropertyNodePtr skel;
    for (; pn; pn = pn->nextSibling()) {
        if (pn->name() == DocSrcInfo::DOCUMENT_SKELETON && 
            (skelName.isEmpty() || 
             pn->getSafeProperty("name")->getString() == skelName)) {
                skel = pn->getProperty(DocSrcInfo::TEMPLATE_DOCROOTGROVE);
                break;
        }
    }
    // Move skeleton around
    if (skel) {
        skel->remove();                     // remove from old place
        dsi->appendChild(skel.pointer());   // put into shelf-case
    }
    createGrove(dsi.pointer());
    bool ok = initGrove(dsi.pointer());
    sernaDoc()->showStageInfo();
    return ok;
}
