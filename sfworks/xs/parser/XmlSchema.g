//-*-mode:C++;-*-

// (c) 2000 by Syntext, Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/*! \file
 *  
 *
 *  This file contains ANTLR grammar for XML Schemas.
 */

header {
#include "xs/xs_defs.h"
#include "xs/parser/GroveAst.h"
#include <iostream>
#include "common/RefCntPtr.h"

#include "xs/XsDataImpl.h"
#include "xs/ExtPythonScript.h"
#include "xs/Origin.h"
#include "xs/Appinfo.h"
#include "xs/parser/AttributeParser.h"

#include "grove/Nodes.h"
#include "grove/XmlNs.h"
#include "urimgr/GroveResource.h"
#include "xs/SchemaResource.h"
#include "xs/XsMessages.h"
    
#include "xs/EnumFetcher.h"
    
#include "xs/Piece.h"
#include "xs/Component.h"
#include "xs/ComponentRef.h"
#include "xs/ComponentSpace.h"
#include "xs/ImportMap.h"
#include "xs/XsSchema.h"
#include "xs/XsIdentityConstraint.h"    
#include "xs/components/XsAttributeImpl.h"
#include "xs/components/XsAttributeGroupImpl.h"
#include "xs/components/AttributeSet.h"
#include "xs/components/XsElementImpl.h"
#include "xs/components/XsUniqueImpl.h"
#include "xs/components/XsKeyImpl.h"
#include "xs/components/XsKeyRefImpl.h"      
#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "xs/datatypes/SimpleDerivedType.h"
#include "xs/datatypes/DerivationTypes.h"
#include "xs/datatypes/Facets.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/complex/XsContent.h"
#include "xs/complex/SimpleContent.h"
#include "xs/complex/ComplexContent.h"
#include "xs/complex/Connectors.h"
#include "xs/complex/WildcardParticle.h"
#include "xs/complex/Wildcard.h"
#include "xs/complex/ElementParticle.h"
#include "xs/components/XsAnyAttribute.h"
#include "xs/components/XsNotationImpl.h"
#include "xs/components/XsGroupImpl.h"
#include "common/Url.h"

#define SCHEMA  ((static_cast<XS_NAMESPACE::SchemaParser*>(this))->schema())
#define XSD XSD_IMPL(SCHEMA)

static Xs::NcnCred::Qualification parse_form(Schema* schema, 
                                             Xs::AttributeParser& attrs, 
                                             const char* attName)
{
    Common::String value;
    if (attrs.get(attName, &value)) {
        if ("qualified" == value.lower()) 
            return Xs::NcnCred::QUALIFIED;
        if ("unqualified" == value.lower())
            return Xs::NcnCred::UNQUALIFIED;
        schema->mstream() << XsMessages::noValidForm
            << COMMON_NS::Message::L_ERROR 
            << value << XSN_ORIGIN(attrs.origin());
        return Xs::NcnCred::UNDEFINED;
    } else
        return Xs::NcnCred::UNDEFINED;
}

}

options {
    language = "Cpp";
//    namespaceStd = "std";
//    namespaceAntlr = "antlr";
}

class GroveAstParser extends TreeParser;

options {
    ASTLabelType = "RefGroveAst";
}

tokens {
    XmlSchema;
    Annotation; 
    Appinfo;
    Script;
    Documentation;
    Import;
    Include;
    Redefine;
    Element;
    Unique;
    Key;
    Keyref;
    Selector;
    Field;
    Attribute;
    SimpleType;
    List;
    Restriction;
    Union;
    ComplexType;
    SimpleContent;
    Extension;
    AttributeGroup;
    AnyAttribute;
    ComplexContent;
    Group;
    All;
    Choice;
    Sequence;
    Any;
    Notation;
    Enumeration;
    Length;
    MaxExclusive;
    MaxInclusive;
    MaxLength;
    MinExclusive;
    MinInclusive;
    MinLength;
    Pattern;
    TotalDigits;
    FractionDigits;
    Whitespace;
    EndToken;
}

xmlSchema returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsSchema> schema_]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann_1;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann_2; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Component> st;
}
    :
        #(t:XmlSchema
            {
                schema_ = new XS_NAMESPACE::XsSchema(t->origin());
                COMMON_NS::String tns(GROVE_NAMESPACE::XmlNs::defaultNs());
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                XSD->attrForm_ = 
                    parse_form(SCHEMA, attrs, "attributeFormDefault");
                XSD->elemForm_ = 
                    parse_form(SCHEMA, attrs, "elementFormDefault");
                if (XSD->attrForm_ == Xs::NcnCred::UNDEFINED)
                    XSD->attrForm_ = Xs::NcnCred::UNQUALIFIED;
                if (XSD->elemForm_ == Xs::NcnCred::UNDEFINED)
                    XSD->elemForm_ = Xs::NcnCred::UNQUALIFIED;
                attrs.get("finalDefault");
                attrs.get("blockDefault");
		attrs.get("version");
                attrs.get("targetNamespace", &tns);         
                if (SCHEMA->pstream()) {
                    COMMON_NS::String s = "XSV: Parsing XML Schema";
                    if (!tns.isEmpty())
                        s += " (Target NS: " + tns + ")";
                    *(SCHEMA->pstream()) << s;
                }
                XSD->targetNs_ = tns;
                XSD->init(SCHEMA);
            }
            (
                ann_1 = annotation
                {
                    schema_->addAppinfo(ann_1);      
                }
                | import
                | include
                | redefine
            )*
            (
                    topLevel[false]
                (
                    ann_2 = annotation
                    {
                        schema_->addAppinfo(ann_2);          
                    }
                    | import
                    | include
                    | redefine

                )*
            )*
        (EndToken)? 
    )
        {
            if (SCHEMA->pstream()) 
                *(SCHEMA->pstream()) << "";
        }
    ;

annotation returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> app]
{ 
    COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Element> app_elem;
    COMMON_NS::String doc;
    COMMON_NS::String spt;
}
    :
        #(t:Annotation  
            {
                app = new XS_NAMESPACE::Appinfo();
            } 
            ((
                app_elem = appinfo 
                {
                    app->addAppinfo(app_elem);   
                }
            )|
            (
                doc = documentation
                {
                    app->addDocumentation(doc);   
                }
            ))*
            (
                spt = script 
                {
                    //XSD->pyScript()->parsePyScript(SCHEMA, spt);
                    //app->setPythonScript(spt);   
                }
            )?
        )
    ;

documentation returns [ COMMON_NS::String s]
    : t:Documentation
            {
                COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Element> elem = t->element();
                COMMON_NS::String content;
                for (GROVE_NAMESPACE::Node* t = elem->firstChild(); 
                     t ; t = t->nextSibling()) { 
                    if (GROVE_NAMESPACE::Node::TEXT_NODE == t->nodeType()) 
                        content += CONST_TEXT_CAST(t)->data();
                }
                s = content;
            }
    ;  

script returns [ COMMON_NS::String s]
{ 
    COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Element> py_elem;
}
    :
        t:Script
            {
                py_elem = t->element();
                if (!XSD->isSchemaExtNode(py_elem.pointer()))
                    SCHEMA->mstream() << XsMessages::isNotAnExtElem 
                              << COMMON_NS::Message::L_ERROR 
                      << py_elem->nodeName() ;
                COMMON_NS::String content;
                for (GROVE_NAMESPACE::Node* t = py_elem->firstChild(); 
                     t ; t = t->nextSibling()) { 
                    if (GROVE_NAMESPACE::Node::TEXT_NODE == t->nodeType()) {
                        content += CONST_TEXT_CAST(t)->data();
                    } else if (GROVE_NAMESPACE::Node::ELEMENT_NODE == 
                        t->nodeType()) {
                            SCHEMA->mstream() << XsMessages::elemsInScript 
                                  << COMMON_NS::Message::L_ERROR;
                    }
                }
                s = content;
            }
    ;  

import
    :   t:Import
        {
        XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
        COMMON_NS::String nsprefix, loc, xns;
        COMMON_NS::String prefix2;
        attrs.get("namespace", &xns);
        attrs.get("schemaLocation", &loc);
        if (xns.isEmpty() && loc.isEmpty()) 
            goto done;
        if (loc.isEmpty())
            loc = xns;      // try to obtain via URI
        else
            loc = Common::Url(t->origin()->grove()->topSysid()).
                combinePath2Path(loc);
        if (SCHEMA->pstream()) {
            COMMON_NS::String s = "XSV: import " + loc;
            *(SCHEMA->pstream()) << s;
        }
        XSD->importMap()->addImportItem(xns, loc);
        done:;
        }
    ;

include
    :   #(t:Include
        {
            XSD->processInclude(t->origin(), SCHEMA);
        }
        );
            

redefine
    :   #(t:Redefine
        {
            XSD->processInclude(t->origin(), SCHEMA);
        }
        (topLevel[true])*
    )
    ;

appinfo returns [ COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Element> app_elem]
    :
        t:Appinfo
        {
            app_elem = t->element();
        }
    ;

topLevel[bool redefine]
    :
        topSimpleType[redefine]
    | 
        topComplexType[redefine]
    |   
        topElement[redefine]
    | 
        topAttribute[redefine]
    | 
        topGroup[redefine]
    | 
        topAttributeGroup[redefine]
    | 
        topNotation[redefine]
    ;

topElement[bool redefine]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsElementImpl> elem;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::XsIdentityConstraintInst uni;
    XS_NAMESPACE::XsTypeInst type;
}
    :
        #( e:Element 
            {
                XS_NAMESPACE::AttributeParser attrs(e->origin(), SCHEMA);
                COMMON_NS::String id;
                COMMON_NS::String value;
                COMMON_NS::String xs_type;
                bool bv;
                XS_NAMESPACE::XsData::ElementSpace& espace = XSD->elementSpace();

                if (attrs.get("ref", &value)) {
                    SCHEMA->mstream() << XsMessages::refProhibited
                                      << COMMON_NS::Message::L_ERROR  
                                       << XSP_ORIGIN(e);    
                    if (e->origin() && e->origin()->firstChild()) {
                        throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(
                                  static_cast<ANTLR_USE_NAMESPACE(antlr)RefAST>(_t));
                    }
                    goto done;
                }
                else if (attrs.get("name", &value)) {
                    elem = new XS_NAMESPACE::XsElementImpl(e->origin(),
                               XSD->makeCred(value, redefine), true);
                    if (attrs.get("id", &id)) 
                        elem->putIntoTable(SCHEMA, id);
                    elem->isAnyType_ = true;
                    if (attrs.get("type", &xs_type)) {               
                        bool is_any = ("anyType" == xs_type.right(7));
                        if (is_any)  {
                             GROVE_NAMESPACE::QualifiedName anytype_name;
                             anytype_name.parse(xs_type);
                             GROVE_NAMESPACE::ExpandedName en;
                             en.resolve(e->element(), anytype_name);
                             COMMON_NS::String xns = e->element()->
                                getXmlNsByPrefix(anytype_name.prefix());        
                             if (en.uri() != xns)
                                 is_any = false;
                        }
                        if (!is_any)  {
                             XSD->makeRefInst(elem->baseType_, SCHEMA,
                                              e->element(), xs_type,
                                              XS_NAMESPACE::Component::type);
                             elem->isAnyType_ = false;
                        }
                    }
                }
                else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(e);
                    goto done;
                }
                
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        elem->setCustomMessage(errmsg);
                }

                if ((attrs.has("minOccurs")) || (attrs.has("maxOccurs"))) {
                    SCHEMA->mstream() << XsMessages::noOccursOnTopLevel 
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(e);
                }
                if (attrs.getBoolean("abstract", &bv)) {
                    elem->abstract_ = bv;
                    if (bv) {
                        elem->substitutionGroup_ =
                            elem->constCred()->expandedName();
                        XSD->insertSubstElement
                            (elem->substitutionGroup_, elem.pointer());
                    }
                }
                if (attrs.getBoolean("nullable", &bv)) {
                    elem->nullable_ = bv;
                }
                if (attrs.get("default", &value)) {
                    elem->default_ = value;
                }
                if (attrs.get("fixed", &value)) {
                    elem->fixed_ = value;
                }
                if ((COMMON_NS::String::null() != elem->default_) &&
                    (COMMON_NS::String::null() != elem->fixed_))
                    SCHEMA->mstream() << XsMessages::fixDefConflict 
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(e);
                
                if (attrs.get("substitutionGroup", &value)) {
                    GroveLib::QualifiedName qname;
                    qname.parse(value);
                    elem->substitutionGroup_.resolve(
                        CONST_ELEMENT_CAST(e->origin()), qname);
                    XSD->insertSubstElement(elem->substitutionGroup_,
                        elem.pointer());
                }
                if (attrs.get("form", &value)) {
                    SCHEMA->mstream() << XsMessages::noFormOnTopLevel
                              << COMMON_NS::Message::L_ERROR 
                              << XSP_ORIGIN(e);
                }
                if (attrs.get("block", &value)) {
                    if ("#all" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::ALL;
                    }
                    else if ("substitution" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::SUBSTITUTION;
                    }
                    else if ("extension" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::EXTENSION;
                    }
                    else if ("restriction" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::RESTRICTION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidBlock
                              << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(e);
                        elem->block_ = XS_NAMESPACE::XsElementImpl::ALL;
                    }
                }
                if (attrs.get("final", &value)) {
                    if ("#all" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::ALL;
                    }
                    else if ("extension" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::EXTENSION;
                    }
                    else if ("restriction" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::RESTRICTION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidFinal
                              << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(e);
                        elem->block_ = XS_NAMESPACE::XsElementImpl::ALL;
                    }
                }
                //if (espace.lookup(*elem->constCred())) {
                //    SCHEMA->mstream() << XsMessages::elementRedefinition
                //                      << COMMON_NS::Message::L_ERROR 
                //                      << elem->constCred()->name()
                //                      << XSP_ORIGIN(elem);
                //}
                //else 
                    espace.insert(elem.pointer());
                done:; 
            }
            (   
                ann = annotation
                {
                    elem->setAppinfo(ann);           
                }
            )? 
            ( 
                type = simpleType[elem.pointer()] 
                {
                    elem->isAnyType_ = false;
                    elem->baseType_ = type;
                }
            |   
                type = complexType[elem.pointer()]
                {
                    elem->isAnyType_ = false;
                    elem->baseType_ = type;
                }
            )? 
            (
                uni = unikey[elem.pointer()]
                {
                    elem->addIConstraint(uni);
                }
            )*
            (EndToken)?
        ) 
    ;



element[XS_NAMESPACE::Piece* parent]
    returns [COMMON_NS::RefCntPtr<XS_NAMESPACE::Particle> result]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsElementImpl> elem;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::XsTypeInst type;
    XS_NAMESPACE::XsIdentityConstraintInst uni;
}
    :
        #( e:Element 
            {
                XS_NAMESPACE::AttributeParser attrs(e->origin(), SCHEMA);
                XS_NAMESPACE::ElementParticle* elem_part =
                    new XS_NAMESPACE::ElementParticle(e->origin());
                uint occurs;
                COMMON_NS::String id, value, cns;
                Xs::NcnCred::Qualification form;
                bool bv;
                if (attrs.getUnsigned("minOccurs", &occurs)) {
                    elem_part->setMinOccur(SCHEMA, occurs);
                }
                if (attrs.get("maxOccurs", &value)) {
                    if ("unbounded" == value.lower()) {
                        elem_part->setMaxOccur(SCHEMA, ~0);
                    } else if (attrs.getUnsigned("maxOccurs", &occurs)) {
                        elem_part->setMaxOccur(SCHEMA, occurs);
                    }
                }
                result = elem_part;
                if (attrs.get("ref", &value)) {
                    if (e->origin() && e->origin()->firstChild()) {
                        GroveLib::Element* parent = e->element();
                        GroveLib::Node* node = parent->firstChild();
                        int count = 0;
                        while (node) {
                            if (GroveLib::Node::ELEMENT_NODE == node->nodeType() &&
                                ELEMENT_CAST(node)->localName() != "annotation")
                                    count++;
                            node = node->nextSibling();
                        }
                        if (0 < count) {
                            SCHEMA->mstream() << XsMessages::refElementHasContent
                                              << COMMON_NS::Message::L_ERROR 
                                              << XSP_ORIGIN(e);
                            throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(
                                  static_cast<ANTLR_USE_NAMESPACE(antlr)RefAST>(_t));
                        }
                    }

                    XSD->makeRefInst(elem_part->elem_, SCHEMA, e->element(),
                                     value, XS_NAMESPACE::Component::element);       
                    goto done;
                } else if (attrs.get("name", &value)) {
                    form = parse_form(SCHEMA, attrs, "form");
                    if (form == Xs::NcnCred::UNDEFINED)
                        form = XSD->elemForm_;
                    cns = XSD->targetNsUri();
                    if (form == Xs::NcnCred::UNQUALIFIED)
                        cns = GroveLib::XmlNs::defaultNs();
                    elem = new XS_NAMESPACE::XsElementImpl(e->origin(),
                        Xs::NcnCred(value, cns));
                    if (attrs.get("id", &id))
                        elem->putIntoTable(SCHEMA, id);

                    COMMON_NS::String xs_type;
                    if (attrs.get("type", &xs_type)) {
                        if ("anyType" != xs_type.right(7)) {
                             XSD->makeRefInst(elem->baseType_,
                                              SCHEMA,
                                              e->element(),
                                              xs_type,
                                              XS_NAMESPACE::Component::type);
                        }
                        else {
                             GROVE_NAMESPACE::QualifiedName anytype_name;
                             anytype_name.parse(xs_type);
                             GROVE_NAMESPACE::ExpandedName en;
                             en.resolve(e->element(), anytype_name);
                             COMMON_NS::String xns = e->element()->
                                getXmlNsByPrefix(anytype_name.prefix());        
                             if (en.uri() == xns)
                                 elem->isAnyType_ = true;
                        }
                    }
                    elem_part->setElement(elem.pointer());
                } else {
                    SCHEMA->mstream() << XsMessages::refRequired
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(e);
                    goto done;
                }

                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        elem->setCustomMessage(errmsg);
                }
                                
                if (attrs.getBoolean("abstract", &bv)) {
                    elem->abstract_ = bv;
                }
                if (attrs.getBoolean("nullable", &bv)) {
                    elem->nullable_ = bv;
                }
                if (attrs.get("default", &value)) {
                    elem->default_ = value;
//TODO: find this in Schema
//                    if (0 != result->minOccur()) {
//                        SCHEMA->mstream() << XsMessages::defaultAndMinOccurs
//                                          << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(e);
//                        elem_part->setMinOccur(SCHEMA, 0);
//                    }
                }
                if (attrs.get("fixed", &value)) {
                    elem->fixed_ = value;
                }
                if ((COMMON_NS::String::null() != elem->default_) &&
                    (COMMON_NS::String::null() != elem->fixed_))
                    SCHEMA->mstream() << XsMessages::fixDefConflict 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(e);

                if (attrs.get("substitutionGroup", &value)) {
                    SCHEMA->mstream() << XsMessages::substitutionGroupOnLocal
                              << COMMON_NS::Message::L_ERROR  
                              << XSP_ORIGIN(e);
                }
                if (attrs.get("block", &value)) {
                    if ("#all" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::ALL;
                    }
                    else if ("substitution" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::SUBSTITUTION;
                    }
                    else if ("extension" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::EXTENSION;
                    }
                    else if ("restriction" == value.lower()) {
                        elem->block_ = XS_NAMESPACE::XsElementImpl::RESTRICTION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidBlock
                              << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(e);
                        elem->block_ = XS_NAMESPACE::XsElementImpl::ALL;
                    }
                }
                if (attrs.get("final", &value)) {
                     SCHEMA->mstream() << XsMessages::finalOnLocal
                               << COMMON_NS::Message::L_ERROR  
                       << XSP_ORIGIN(e);
                }
              done:
                elem_part->setParentPiece(parent);
            }
            (   
                ann = annotation
                {
                    GroveLib::Element* parent = e->element();
                    GroveLib::Attr* ref = parent->attrs().getAttribute("ref");
                    if (0 == ref) 
                        elem->setAppinfo(ann);           
                }
            )? 
            ( 
                type = simpleType[elem.pointer()] 
                {
                    elem->baseType_ = type;
                }
            |   
                type = complexType[elem.pointer()] 
                {
                    elem->baseType_ = type;
                }
            )? 
            ( 
                uni = unikey[elem.pointer()]
                {
                    elem->addIConstraint(uni);
                }
            )*

            (EndToken)?
        ) 
    ;

unikey[XS_NAMESPACE::Piece* parent]
    returns [XS_NAMESPACE::XsIdentityConstraintInst result]
{
    XS_NAMESPACE::XsUniqueImpl* uniq;
    XS_NAMESPACE::XsKeyImpl*    k;
    XS_NAMESPACE::XsKeyRefImpl* kr;
}
    : k = key
        {
            k->setParentPiece(parent);
            result = k;
        }
    | kr = keyref
        {
            kr->setParentPiece(parent);
            result = kr;
        }    
    | uniq = unique
        {
            uniq->setParentPiece(parent);
            result = uniq;
        }   
    ;      

unique returns [XS_NAMESPACE::XsUniqueImpl* uniq]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::String s;
}
    :
        #( t:Unique
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                COMMON_NS::String id;
                if (attrs.get("name", &value)) {
            uniq = new XS_NAMESPACE::XsUniqueImpl(t->origin(),
            XSD->makeCred(value));
                    if (attrs.get("id", &id))
                        uniq->putIntoTable(SCHEMA, id);
                    
                    if (attrs.checkExt( "errmsg")) {
                        COMMON_NS::String errmsg;
                        if (attrs.get("errmsg", &errmsg)) 
                            uniq->setCustomMessage(errmsg);
                    }
                }
        else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(t);
                    uniq = 0;
                }
            }
            (   
                ann = annotation
                {
                    uniq->setAppinfo(ann);           
                }
            )? 
            s = selector
            {
                uniq->setSelector(s);
            }
            (
                s = field
                {
                    uniq->addField(s);           
                }
            )+
         )
    ;

key returns [XS_NAMESPACE::XsKeyImpl* k]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::String s;
}
    :
        #( t:Key
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                COMMON_NS::String id;
                if (attrs.get("name", &value)) {
            k = new XS_NAMESPACE::XsKeyImpl(t->origin(),
            XSD->makeCred(value));

                    if (attrs.get("id", &id))
                        k->putIntoTable(SCHEMA, id);
                    
                    if (attrs.checkExt( "errmsg")) {
                        COMMON_NS::String errmsg;
                        if (attrs.get("errmsg", &errmsg)) 
                            k->setCustomMessage(errmsg);
                    }

                    XS_NAMESPACE::XsData::IdentityConstraintSpace& ic_space =
                        XSD->identityConstraintSpace();
            //if (ic_space.lookup(*k->constCred())) {
            //            SCHEMA->mstream() << XsMessages::elementRedefinition
            //                  << COMMON_NS::Message::L_ERROR 
            //                  << k->constCred()->name()
            //                  << XSP_ORIGIN(k);
            //} else
                        ic_space.insert(k);                     
        } else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(t);
                    k = 0;
                }
            }
            (   
                ann = annotation
                {
                    k->setAppinfo(ann);           
                }
            )? 
            s = selector
            {
                k->setSelector(s);
            }
            (
                s = field
                {
                    k->addField(s);           
                }
            )+
         )
    ;

keyref returns [XS_NAMESPACE::XsKeyRefImpl* kr]
{
    XS_NAMESPACE::XsIdentityConstraintInst ic;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::String s;
}
    :
        #( t:Keyref
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                COMMON_NS::String id;
                COMMON_NS::String ref;
                if (attrs.get("name", &value)) {
                    kr = new  XS_NAMESPACE::XsKeyRefImpl(t->origin(),
            XSD->makeCred(value));
                    if (attrs.get("refer", &ref)) {
            XSD->makeRefInst(ic, SCHEMA, t->element(),
                ref, XS_NAMESPACE::Component::identityConstraint);
                        kr->setReference(ic); 
                    }  

                    if (attrs.get("id", &id))
                        kr->putIntoTable(SCHEMA, id);
                    
                    if (attrs.checkExt( "errmsg")) {
                        COMMON_NS::String errmsg;
                        if (attrs.get("errmsg", &errmsg)) 
                            kr->setCustomMessage(errmsg);
                    }
                }
        else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(t);
                    kr = 0;
                }
            }
            (   
                ann = annotation
                {
                    kr->setAppinfo(ann);           
                }
            )? 
            s = selector
            {
                kr->setSelector(s);
            }
            (
                s = field
                {
                    kr->addField(s);           
                }
            )+
         )
    ;

selector returns [COMMON_NS::String result]
    :
        #( t:Selector
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                COMMON_NS::String id;
                if (attrs.get("xpath", &value)) 
                   result = value;
                else 
                   result = "";
                attrs.get("id", &id);
            }   
//TODO           (annotation)?
         )
    ;

field returns [COMMON_NS::String result]
    :
       #( t:Field
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                COMMON_NS::String id;
                if (attrs.get("xpath", &value)) 
                   result = value;
                else 
                   result = "";
                attrs.get("id", &id);
            }
//TODO            (annotation)?
        )
    ;


topAttribute[bool redefine]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAttributeImpl> attr;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::XsTypeInst type;
}
    :
        #( a:Attribute 
            {
                XS_NAMESPACE::AttributeParser attrs(a->origin(), SCHEMA);
                COMMON_NS::String id;
                COMMON_NS::String value;
                COMMON_NS::String xs_type;
                COMMON_NS::String val;
                XS_NAMESPACE::XsData::AttributeSpace& aspace = XSD->attributeSpace();
                
                if (attrs.get("ref", &value)) {
                    SCHEMA->mstream() << XsMessages::refProhibited
                              << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(a);    
                    goto done;
                }
                else if (attrs.get("name", &value)) {
                    if ("xmlns" == value.lower() || 
                        "xmlns:" == value.left(6).lower()) {
                        SCHEMA->mstream() << XsMessages::attrInvalidStart
                                          << COMMON_NS::Message::L_ERROR
                                          << XSP_ORIGIN(a);
                        goto done;
                    }
                    attr = new XS_NAMESPACE::XsAttributeImpl(a->origin(),
                        XSD->makeCred(value, redefine));
                    if (attrs.get("id", &id))
                        attr->putIntoTable(SCHEMA, id);

                    if (attrs.get("type", &xs_type)) {
                        XSD->makeRefInst(type, SCHEMA, a->element(),
                                         xs_type, XS_NAMESPACE::Component::type);
                        attr->setBaseType(type); 
                    }
                }
                else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(a);
                    goto done;
                }
               
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        attr->setCustomMessage(errmsg);
                }

                if (attrs.get("default", &value)) {
                    attr->default_ = value;
                }
                if (attrs.get("fixed", &value)) {
                    attr->fixed_ = value;
                }
                if ((COMMON_NS::String::null() != attr->default_) &&
                    (COMMON_NS::String::null() != attr->fixed_))
                    SCHEMA->mstream() << XsMessages::fixDefConflict 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(a);

                if (attrs.get("use", &value))
                    if ((COMMON_NS::String::null() != attr->default_) &&
                        ("optional" != value.lower()))
                        SCHEMA->mstream() << XsMessages::noValueToUse 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(a);
                
                if (attrs.get("form", &value)) {
                    SCHEMA->mstream() << XsMessages::noFormOnTopLevel 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(a);
                }
                
        //if (aspace.lookup(*attr->constCred())) {
        //            SCHEMA->mstream() << XsMessages::attributeRedefinition 
        //                      << COMMON_NS::Message::L_ERROR 
        //                      << attr->constCred()->name()
         //                     << XSP_ORIGIN(attr);
         //       }
         //       else
                    aspace.insert(attr.pointer());
                done:;
            }
            (   
                ann = annotation
                {
                    attr->setAppinfo(ann);           
                }
            )? 
            ( 
                type = simpleType[attr.pointer()]   
                {
                    attr->setBaseType(type);
                }
            )?
            (EndToken)?      
        )
    ;              

attribute[XS_NAMESPACE::Piece* parent]
    returns [XS_NAMESPACE::XsAttributeInst result]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAttributeImpl> attr;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::XsTypeInst type;
}
    :
        #( a:Attribute 
            {
                XS_NAMESPACE::AttributeParser attrs(a->origin(), SCHEMA);
                Xs::NcnCred::Qualification form;
                COMMON_NS::String value, id, xs_type, cns;
                GroveLib::QualifiedName attr_qname;
                if (attrs.get("ref", &value) && !attrs.get("default")
                    && !attrs.get("fixed") && !attrs.get("use")) {
                        XSD->makeRefInst(result, SCHEMA, a->element(),
                            value, XS_NAMESPACE::Component::attribute);
                        goto done;
                }
                if (attrs.get("name", &value)) {
                    if ("xmlns" == value.lower() || 
                        "xmlns:" == value.left(6).lower()) {
                        SCHEMA->mstream() << XsMessages::attrInvalidStart
                                          << COMMON_NS::Message::L_ERROR
                                          << XSP_ORIGIN(a);
                    }
                }
                else 
                    attrs.get("ref", &value);
                if (value.isEmpty()) {
                    SCHEMA->mstream() << XsMessages::nameOrRefRequired
                        << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(a);
                    goto done;       
                }
                form = parse_form(SCHEMA, attrs, "form");
                if (form == Xs::NcnCred::UNDEFINED)
                    form = XSD->attrForm_;
                attr_qname.parse(value);
                if (attr_qname.isQualified())
                    cns = a->origin()->getXmlNsByPrefix(attr_qname.prefix());   
                else
                    cns = (form == Xs::NcnCred::UNQUALIFIED) ? 
                        GroveLib::XmlNs::defaultNs() :
                        XSD->targetNsUri();
                attr = new XS_NAMESPACE::XsAttributeImpl(a->origin(),
                    Xs::NcnCred(attr_qname.localName(), cns));
                if (attrs.get("id", &id))
                    attr->putIntoTable(SCHEMA, id);
                
                if (attrs.get("type", &xs_type)) {
                    XSD->makeRefInst(type, SCHEMA, a->element(),
                    xs_type, XS_NAMESPACE::Component::type);
                    attr->setBaseType(type);
                }
                if (attrs.get("form", &value)) {
                    if ("qualified" == value.lower()) {
                        attr->setForm(XS_NAMESPACE::XsAttributeImpl::QUALIFIED);
                    }
                    else if ("unqualified" == value.lower()) {
                        attr->setForm(XS_NAMESPACE::XsAttributeImpl::UNQUALIFIED);
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidForm
                                          << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(a);
                        attr->setForm(XS_NAMESPACE::XsAttributeImpl::UNQUALIFIED);
                    }
                }
                result = attr.pointer();
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        attr->setCustomMessage(errmsg);
                }
                attr->setUse(XS_NAMESPACE::XsAttributeImpl::A_OPTIONAL);
                if (attrs.get("default", &value)) {
                    attr->setUse(XS_NAMESPACE::XsAttributeImpl::A_DEFAULT); 
                    attr->default_ = value;
                }
                if (attrs.get("fixed", &value)) {
                    attr->fixed_ = value;
                    attr->setUse(XS_NAMESPACE::XsAttributeImpl::A_FIXED); 
                }
                if ((COMMON_NS::String::null() != attr->default_) &&
                    (COMMON_NS::String::null() != attr->fixed_)) {
                    SCHEMA->mstream() << XsMessages::fixDefConflict 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(a);
                    goto done;
                }
                
                if (attrs.get("use", &value)) {
                    if ((COMMON_NS::String::null() != attr->default_) &&
                        ("optional" != value.lower()))
                        SCHEMA->mstream() << XsMessages::noUseDefault 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(a);
                    if ("prohibited" == value.lower()) {
                        attr->setUse(XS_NAMESPACE::XsAttributeImpl::A_PROHIBITED);
                    }
                    else if ("optional" == value.lower()) {
                        attr->setUse(XS_NAMESPACE::XsAttributeImpl::A_OPTIONAL);
                    }
                    else if ("required" == value.lower()) {
                        attr->setUse(XS_NAMESPACE::XsAttributeImpl::A_REQUIRED);
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidUse 
                              << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(a);
                        attr->setUse(XS_NAMESPACE::XsAttributeImpl::A_OPTIONAL);
                    }
                } 
                if (attrs.get("ref", &value)) {
                    XS_NAMESPACE::XsAttributeInst inst;
                    XSD->makeRefInst(inst, SCHEMA, a->element(),
                        value, XS_NAMESPACE::Component::attribute);
                    attr->setRef(inst);
                }
                attr->setParentPiece(parent);  
                done: ;
            }
            (   
                ann = annotation
                {
                    attr->setAppinfo(ann);           
                }
            )? 
            ( 
                type = simpleType[attr.pointer()]   
                {
                    attr->setBaseType(type);           
                }
            )?
            (EndToken)?     
        )
    ;

topSimpleType[bool redefine]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsSimpleTypeImpl> stype; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::String name;
    XS_NAMESPACE::SimpleDerivedType::DerivationMethod final;
    COMMON_NS::String id;
}
    :
        #(t:SimpleType 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                if (!attrs.get("name", &name)) {
                    SCHEMA->mstream() << XsMessages::nameRequired 
                              << COMMON_NS::Message::L_ERROR  
                              << XSP_ORIGIN(t);
                }
                final = XS_NAMESPACE::SimpleDerivedType::NONE;
                COMMON_NS::String value;
                if (attrs.get("final", &value)) {
                    if ("#all" == value.lower()) {
                        final = XS_NAMESPACE::SimpleDerivedType::ALL;
                    }
                    else if ("list" == value.lower()) {
                        final = XS_NAMESPACE::SimpleDerivedType::LIST;
                    }
                    else if ("restriction" == value.lower()) {
                        final = XS_NAMESPACE::SimpleDerivedType::RESTRICTION;
                    }
                    else if ("union" == value.lower()) {
                        final = XS_NAMESPACE::SimpleDerivedType::UNION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidFinal
                              << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(t);
                    }
                }
                attrs.get("id", &id);
            }
            (  
                ann = annotation
            )?
            (
                stype = list
            | 
                stype = restriction
            | 
                stype = union_x
            )
            {
                if ((!id.isNull()) && (!id.isEmpty()))
                    stype->putIntoTable(SCHEMA, id);
                if (XS_NAMESPACE::SimpleDerivedType::NONE != final)
                    static_cast<XS_NAMESPACE::SimpleDerivedType*>(stype.pointer())->setFinal(final);
                stype->setAppinfo(ann);                
                stype->setCred(XSD->makeCred(name, redefine));
                XS_NAMESPACE::XsData::TypeSpace& tspace = XSD->typeSpace();
        //if (tspace.lookup(*stype->constCred())) {
        //            SCHEMA->mstream() << XsMessages::simpleTypeRedefinition 
        //                      << COMMON_NS::Message::L_ERROR 
        //                      << stype->constCred()->name()
        //                      << XSP_ORIGIN(stype);
        //        }
        //        else 
                    tspace.insert(stype.pointer());               
            }
            (EndToken)?     
        )
    ;


simpleType[XS_NAMESPACE::Piece* parent]
    returns [XS_NAMESPACE::XsTypeInst result]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsSimpleTypeImpl> stype; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::String name;
    COMMON_NS::String id;
}
    :
        #(t:SimpleType 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                if (attrs.get("name", &name)) {
                    SCHEMA->mstream() << XsMessages::nameProhibited 
                                      << COMMON_NS::Message::L_ERROR 
                                      << XSP_ORIGIN(t);
                }
                COMMON_NS::String value;
                if (attrs.get("final", &value)) {
                     SCHEMA->mstream() << XsMessages::finalOnLocal 
                                       << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);
                }
                attrs.get("id", &id);
            }
            (  
                ann = annotation
            )?
            (
                stype = list
            | 
                stype = restriction
            | 
                stype = union_x
            )
            {
//                stype->setAppinfo(ann);
                if ((!id.isNull()) && (!id.isEmpty()))
                    stype->putIntoTable(SCHEMA, id);
                stype->setParentPiece(parent);
                result = stype.pointer(); 
            }
            (EndToken)?     
        )
    ;

list returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsSimpleTypeImpl> stype]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::List> lst;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::XsTypeInst base;
}
    :
        #(t:List
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                lst = new XS_NAMESPACE::List(t->origin(),
                                             XS_NAMESPACE::NcnCred());
                stype = lst.pointer();

                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    lst->putIntoTable(SCHEMA, id);
               
                COMMON_NS::String xs_type;
                if (attrs.get("itemType", &xs_type)) {
                    XSD->makeRefInst(base, SCHEMA, t->element(),
                                     xs_type, XS_NAMESPACE::Component::type);
                    lst->setBaseType(base); 
                }
                COMMON_NS::String trim;
                if (attrs.checkExt( "trim")) {
                    if (attrs.get("trim", &trim)) {
                        COMMON_NS::String trimResult;
                        if (attrs.get("trimResult", &trimResult)) {
                            if (attrs.checkExt( "trimResult"))
                                lst->setTrim(trim, trimResult);
                        }
                        else {
                            SCHEMA->mstream() << XsMessages::trimOrResultAbsent
                                  << COMMON_NS::Message::L_ERROR 
                                              << XSP_ORIGIN(lst);    
                        }
                    }
                }
                else if (attrs.checkExt( "trimResult")) {
                    if(attrs.get("trimResult", &trim)) 
                        SCHEMA->mstream() << XsMessages::trimOrResultAbsent
                              << COMMON_NS::Message::L_ERROR 
                                          << XSP_ORIGIN(lst); 
                }
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        lst->setCustomMessage(errmsg);
                }
                
                if (attrs.checkExt( "pyPreFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyPreFunc", &fname)) 
                        lst->setPyPreFunc(fname);
                }
                if (attrs.checkExt( "pyPostFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyPostFunc", &fname)) 
                        lst->setPyPostFunc(fname);
                }      
            } 
            (
                ann = annotation
                {
                    lst->setAppinfo(ann);            
                }
            )?
            ( 
                base = simpleType[stype.pointer()]   
                {
                    lst->setBaseType(base);            
                }
            )?
            (EndToken)?    
        )
    ;


restriction returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsSimpleTypeImpl> stype]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Restriction> rst;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> fct; 
    XS_NAMESPACE::XsTypeInst base;
}
    :
        #(t:Restriction
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                rst = new XS_NAMESPACE::Restriction(t->origin(),
                                                    XS_NAMESPACE::NcnCred());
                stype = rst.pointer();

                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    rst->putIntoTable(SCHEMA, id);
                              
                COMMON_NS::String xs_type;
                if (attrs.get("base", &xs_type)) {
                    XSD->makeRefInst(base, SCHEMA, t->element(),
                                     xs_type, XS_NAMESPACE::Component::type);
                    rst->setBaseType(base); 
                }
                COMMON_NS::String trim;
                if (attrs.checkExt( "trim")) {
                    if (attrs.get("trim", &trim)) {
                        COMMON_NS::String trimResult;
                        if (attrs.get("trimResult", &trimResult)) {
                            if (attrs.checkExt( "trimResult"))
                                rst->setTrim(trim, trimResult);
                        }
                        else {
                            SCHEMA->mstream() << XsMessages::trimOrResultAbsent
                                  << COMMON_NS::Message::L_ERROR 
                                              << XSP_ORIGIN(rst);    
                        }
                    }
                }
                else if (attrs.checkExt( "trimResult")) {
                    if(attrs.get("trimResult", &trim)) 
                        SCHEMA->mstream() << XsMessages::trimOrResultAbsent
                              << COMMON_NS::Message::L_ERROR 
                                          << XSP_ORIGIN(rst); 
                }

                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        rst->setCustomMessage(errmsg);
                }
                
                if (attrs.checkExt( "pyPreFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyPreFunc", &fname)) 
                        rst->setPyPreFunc(fname);
                }
                if (attrs.checkExt( "pyPostFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyPostFunc", &fname)) 
                        rst->setPyPostFunc(fname);
                }  
            } 
            (
                ann = annotation
                {
                    rst->setAppinfo(ann);
                    stype->setAppinfo(ann);                                
                }
            )?
            ( 
                base = simpleType[stype.pointer()]
                {
                    rst->setBaseType(base);
                }
            )?
            (
                fct = facets[stype.pointer()]
                {
                    rst->addFacet(fct);         
                }
            )*
            (EndToken)?    
        )
    ;

union_x returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsSimpleTypeImpl> stype]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Union> unn;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::XsTypeInst base;
}
    :
        #(t:Union
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
        unn = new XS_NAMESPACE::Union(t->origin(),
            XS_NAMESPACE::NcnCred());
                stype = unn.pointer();

                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    unn->putIntoTable(SCHEMA, id);
                               
                COMMON_NS::Vector<COMMON_NS::String> xs_type;
                if (attrs.getTokenized("memberTypes", xs_type)) {
                    for(uint i = 0; i < xs_type.size(); i++) {
            XSD->makeRefInst(base, SCHEMA, t->element(),
                xs_type[i], XS_NAMESPACE::Component::type);
                        unn->addBaseType(base); 
                    }
                }
                COMMON_NS::String trim;
                if (attrs.checkExt( "trim")) {
                    if (attrs.get("trim", &trim)) {
                        COMMON_NS::String trimResult;
                        if (attrs.get("trimResult", &trimResult)) {
                            if (attrs.checkExt( "trimResult"))
                                unn->setTrim(trim, trimResult);
                        }
                        else {
                            SCHEMA->mstream() << XsMessages::trimOrResultAbsent
                                  << COMMON_NS::Message::L_ERROR 
                                              << XSP_ORIGIN(unn);    
                        }
                    }
                }
                else if (attrs.checkExt( "trimResult")) {
                    if(attrs.get("trimResult", &trim)) 
                        SCHEMA->mstream() << XsMessages::trimOrResultAbsent
                              << COMMON_NS::Message::L_ERROR 
                                          << XSP_ORIGIN(unn); 
                }
                
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        unn->setCustomMessage(errmsg);
                }
                
                if (attrs.checkExt( "pyPreFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyPreFunc", &fname)) 
                        unn->setPyPreFunc(fname);
                }
                if (attrs.checkExt( "pyPostFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyPostFunc", &fname)) 
                        unn->setPyPostFunc(fname);
                }  
            } 
            (
                ann = annotation
                {
                    unn->setAppinfo(ann);            
                }
            )?
            ( 
                base = simpleType[stype.pointer()]  
                {
                    unn->addBaseType(base);            
                }
            )*
            (EndToken)?     
        )
    ;

topComplexType[bool redefine]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsComplexTypeImpl> ctype; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content;
}
    :
        #(t:ComplexType 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String id;
                COMMON_NS::String value;
                bool bv;
                XS_NAMESPACE::XsData::TypeSpace& tspace = XSD->typeSpace();
                
                if (attrs.get("name", &value)) {
                    ctype = new XS_NAMESPACE::XsComplexTypeImpl(t->origin(),
                        XSD->makeCred(value, redefine));
                    if (attrs.get("id", &id))
                        ctype->putIntoTable(SCHEMA, id);
                }
                else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(t);
                    if (t->origin() && t->origin()->firstChild()) {
                        throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(
                                  static_cast<ANTLR_USE_NAMESPACE(antlr)RefAST>(_t));
                    }

                    goto done;
                }
                if (attrs.getBoolean("abstract", &bv)) {
                    ctype->abstract_ = bv;
                }
                if (attrs.getBoolean("mixed", &bv)) {
                    ctype->mixed_ = bv;
                }
                if (attrs.get("block", &value)) {
                    if ("#all" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                    else if ("extension" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::EXTENSION;
                    }
                    else if ("restriction" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::RESTRICTION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidBlock
                              << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(t);
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                }
                if (attrs.get("final", &value)) {
                    if ("#all" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                    else if ("extension" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::EXTENSION;
                    }
                    else if ("restriction" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::RESTRICTION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidFinal
                              << COMMON_NS::Message::L_ERROR 
                                         << value << XSP_ORIGIN(t);
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                }
                                
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        ctype->setCustomMessage(errmsg);
                }
                
                if (attrs.checkExt( "pyFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyFunc", &fname)) 
                        ctype->setPyFunc(fname);
                }
                
                //if (tspace.lookup(*ctype->constCred())) {
                //    SCHEMA->mstream() << XsMessages::elementRedefinition
                //              << COMMON_NS::Message::L_ERROR
                //              << ctype->constCred()->name() 
                //              << XSP_ORIGIN(ctype);
                //}
                //else
                    tspace.insert(ctype.pointer());
                done:;
            }
            (  
                ann = annotation
                {
                    ctype->setAppinfo(ann);          
                }
            )?
            (
                content = simpleContent[ctype.pointer()]
                {
                    ctype->setContent(content);     
                }
            | 
                content = complexContent[ctype.pointer()]
                {
                    ctype->setContent(content);     
                }
            | 
                ( (groupRef | all | choice | sequence)? 
                  (attribute | attributeGroupRef)* (anyAttribute)? )=>
                content = zeroContent[ctype.pointer()]
                {
                    ctype->setContent(content);     
                }       
            )
            (EndToken)?     
        )
    ;


complexType[XS_NAMESPACE::Piece* parent]
    returns [XS_NAMESPACE::XsTypeInst result]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsComplexTypeImpl> ctype; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content;
}
    :
        #(t:ComplexType 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String id;
                COMMON_NS::String value;
                if (attrs.get("name", &value)) {
                    SCHEMA->mstream() << XsMessages::nameProhibited
                              << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(t);
                    goto done;
                }
                else {
                    ctype = new XS_NAMESPACE::XsComplexTypeImpl
                        (t->origin(), XS_NAMESPACE::NcnCred());
                    if (attrs.get("id", &id))
                        ctype->putIntoTable(SCHEMA, id);            
                    ctype->setParentPiece(parent);
                }
                
                bool bv;
                if (attrs.getBoolean("abstract", &bv)) {
                    ctype->abstract_ = bv;
                }
                if (attrs.getBoolean("mixed", &bv)) {
                    ctype->mixed_ = bv;
                }
                if (attrs.get("block", &value)) {
                    if ("#all" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                    else if ("extension" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::EXTENSION;
                    }
                    else if ("restriction" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::RESTRICTION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidBlock 
                              << COMMON_NS::Message::L_ERROR 
                                          << value << XSP_ORIGIN(t);
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                }
                if (attrs.get("final", &value)) {
                    if ("#all" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                    else if ("extension" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::EXTENSION;
                    }
                    else if ("restriction" == value.lower()) {
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::RESTRICTION;
                    }
                    else {
                        SCHEMA->mstream() << XsMessages::noValidFinal 
                      << COMMON_NS::Message::L_ERROR 
                                  << value << XSP_ORIGIN(t);
                        ctype->block_ = XS_NAMESPACE::XsComplexTypeImpl::ALL;
                    }
                }
                                
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        ctype->setCustomMessage(errmsg);
                }

                if (attrs.checkExt( "pyFunc")) {
                    COMMON_NS::String fname;
                    if (attrs.get("pyFunc", &fname)) 
                        ctype->setPyFunc(fname);
                }
                
                result = ctype.pointer();
                done:;
            }
            (  
                ann = annotation
                {
                    ctype->setAppinfo(ann);          
                }
            )?
            (
                content = simpleContent[ctype.pointer()]
                {
                    ctype->setContent(content);     
                }
            | 
                content = complexContent[ctype.pointer()]
                {
                    ctype->setContent(content);     
                }
            | 
                ( (groupRef | all | choice | sequence)? 
                  (attribute | attributeGroupRef)* (anyAttribute)? )=>
                content = zeroContent[ctype.pointer()]
                {
                    ctype->setContent(content);     
                }       
            )
            (EndToken)?    
        )
    ;

zeroContent[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> con;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAnyAttribute> any_attr;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::ComplexContent> zcontent;

    XS_NAMESPACE::XsGroupInst          grp;
    XS_NAMESPACE::XsAttributeInst      attr;
    XS_NAMESPACE::XsAttributeGroupInst attr_grp;

    XS_NAMESPACE::Origin zero(0);

    zcontent = new XS_NAMESPACE::ComplexContent(
        XS_NAMESPACE::XsContent::NONE, zero);
    zcontent->setParentPiece(parent);
    content = zcontent.pointer();
    
    COMMON_NS::RefCntPtr<XS_NAMESPACE::AttributeSet> attr_set;
    
    attr_set = new XS_NAMESPACE::AttributeSet();
    zcontent->setAttributeSet(attr_set.pointer());
}
    :   
        ( con = groupRef[zcontent.pointer()]
            {   
                zcontent->setConnector(con);        
            }
        | con = all[zcontent.pointer()] 
            {
                zcontent->setConnector(con);        
            }
        | con = choice [zcontent.pointer()]
            {
                zcontent->setConnector(con);        
            }
        | con = sequence[zcontent.pointer()] 
            {
                zcontent->setConnector(con);        
            }
        )?  

        ( attr = attribute[zcontent.pointer()]
            {
                attr_set->addAttribute(attr);       
            }
        | attr_grp = attributeGroupRef
            {
                attr_set->addAttributeGroup(attr_grp);      
            }
        )* 

        ( any_attr = anyAttribute[zcontent.pointer()]
            {
                attr_set->setAnyAttribute(any_attr);        
            }
        )?
    ;

 
simpleContent[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
} 
    :
        #(t:SimpleContent 
            (
                ann = annotation
            )?
            (
        content = simpleRestriction
            [new XS_NAMESPACE::SimpleContent
            (XS_NAMESPACE::XsContent::RESTRICTION, t->origin())]
            | 
        content = simpleExtension
            [new XS_NAMESPACE::SimpleContent
            (XS_NAMESPACE::XsContent::EXTENSION, t->origin())]
            )
            {
                content->setParentPiece(parent);
                if (0 != ann.pointer())
                    content->setAppinfo(ann);           
            }
            (EndToken)?     
        )
    ;

simpleRestriction[ COMMON_NS::RefCntPtr<XS_NAMESPACE::SimpleContent> rst] 
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Restriction> simple_rst;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> fct;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::AttributeSet> attr_set;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAnyAttribute> any_attr;
    XS_NAMESPACE::XsTypeInst base;
    XS_NAMESPACE::XsAttributeInst attr;
    XS_NAMESPACE::XsAttributeGroupInst attr_grp;
} 
    :
        #( t:Restriction 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
        simple_rst = new XS_NAMESPACE::Restriction(t->origin(),
            XS_NAMESPACE::NcnCred());

                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    simple_rst->putIntoTable(SCHEMA, id);
                                   
                COMMON_NS::String xs_type;
                if (attrs.get("base", &xs_type)) {
                    XSD->makeRefInst(base, SCHEMA, t->element(),
                                     xs_type, XS_NAMESPACE::Component::type);
                    simple_rst->setBaseType(base); 
                }               
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        simple_rst->setCustomMessage(errmsg);
                }
                
                rst->setRestriction(simple_rst);
                
                attr_set = new XS_NAMESPACE::AttributeSet();
                rst->setAttributeSet(attr_set.pointer());
                content = rst.pointer();
            } 
            (
                ann = annotation
                {
                    simple_rst->setAppinfo(ann);            
                }
            )?
            ( 
                base = simpleType[simple_rst.pointer()]   
                {
                    simple_rst->setBaseType(base);   
                }
            )?
            (
                fct = facets[simple_rst.pointer()] 
                {
                    simple_rst->addFacet(fct);         
                }
            )* 
            
            ( attr = attribute[simple_rst.pointer()] 
                {
                    attr_set->addAttribute(attr);        
                }
            | attr_grp = attributeGroupRef
                {
                    attr_set->addAttributeGroup(attr_grp);       
                }
            )* 
            
            ( any_attr = anyAttribute[simple_rst.pointer()]
                {
                    attr_set->setAnyAttribute(any_attr);     
                }
            )?
            (EndToken)?     
        )
    ;

simpleExtension[ COMMON_NS::RefCntPtr<XS_NAMESPACE::SimpleContent> ext] 
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Restriction> simple_rst;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::AttributeSet> attr_set;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAnyAttribute> any_attr;
    XS_NAMESPACE::XsAttributeInst attr;
    XS_NAMESPACE::XsAttributeGroupInst attr_grp;
    XS_NAMESPACE::XsTypeInst base;
} 
    :
        #( t:Extension 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                simple_rst = new XS_NAMESPACE::Restriction(t->origin(),
                                                           XS_NAMESPACE::NcnCred());

                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    simple_rst->putIntoTable(SCHEMA, id);
                                                   
                COMMON_NS::String xs_type;
                if (attrs.get("base", &xs_type)) {
                    XSD->makeRefInst(base, SCHEMA, t->element(),
                                     xs_type, XS_NAMESPACE::Component::type);
                    simple_rst->setBaseType(base); 
                }               
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        simple_rst->setCustomMessage(errmsg);
                }
                ext->setRestriction(simple_rst);
                
                attr_set = new XS_NAMESPACE::AttributeSet(); 
                ext->setAttributeSet(attr_set.pointer());
                content = ext.pointer();
            } 
            (
                ann = annotation
                {
                    simple_rst->setAppinfo(ann);            
                }
            )?
            
            ( attr = attribute[ext.pointer()]
                {
                    attr_set->addAttribute(attr);        
                }
            | attr_grp = attributeGroupRef
                {
                    attr_set->addAttributeGroup(attr_grp);       
                }
            )* 
            
            ( any_attr = anyAttribute[ext.pointer()]
                {
                    attr_set->setAnyAttribute(any_attr);     
                }
            )?
            (EndToken)?     
        )

    ;

anyAttribute[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAnyAttribute> any_attr]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::Wildcard*     wc;
} 
    :
        #( t:AnyAttribute 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                XS_NAMESPACE::Wildcard::ProcessMode mode = XS_NAMESPACE::Wildcard::WC_STRICT;
                if (attrs.get("processContents", &value)) {
                    if ("skip" == value.lower())
                        mode = XS_NAMESPACE::Wildcard::SKIP;
                    else if ("lax" == value.lower())
                        mode = XS_NAMESPACE::Wildcard::LAX;
                    else
                        mode = XS_NAMESPACE::Wildcard::WC_STRICT;
                }
                if (attrs.get("namespace", &value))
                    wc = XS_NAMESPACE::Wildcard::make(SCHEMA, mode, value);
                else
                    wc = new XS_NAMESPACE::AnyNsWildcard(mode);
                any_attr = new XS_NAMESPACE::XsAnyAttribute(wc, t->origin());

                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    any_attr->putIntoTable(SCHEMA, id);                
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        any_attr->setCustomMessage(errmsg);
                }    
                any_attr->setParentPiece(parent);
            } 
            (
                ann = annotation
                {
                    any_attr->setAppinfo(ann);           
                }
            )?
            (EndToken)?    
        )
    ;

complexContent [XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    bool isMixed = false;
    XS_NAMESPACE::ComplexContent *cc;
} 
    :
        #( t:ComplexContent
        {
            XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
            attrs.getBoolean("mixed", &isMixed);
        }
            (
                ann = annotation
            )?
            (
                content = complexRestriction
                [cc = new XS_NAMESPACE::ComplexContent
                    (XS_NAMESPACE::XsContent::RESTRICTION, t->origin())]
                {
                    cc->setMixed(isMixed);
                }
                | 
                content = complexExtension
                [cc = new XS_NAMESPACE::ComplexContent
                    (XS_NAMESPACE::XsContent::EXTENSION, t->origin())]
                {
                    cc->setMixed(isMixed);
                }
            )?
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                attrs.getBoolean("mixed", &isMixed);
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    content->putIntoTable(SCHEMA, id);               
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        content->setCustomMessage(errmsg);
                }
                content->setParentPiece(parent);
                if (0 != ann.pointer())
                    content->setAppinfo(ann);           
            }
            (EndToken)?    
        )
    ;


complexRestriction[ COMMON_NS::RefCntPtr<XS_NAMESPACE::ComplexContent>  rst] 
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> con;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::AttributeSet> attr_set;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAnyAttribute> any_attr;
    XS_NAMESPACE::XsGroupInst grp;
    XS_NAMESPACE::XsAttributeInst attr;
    XS_NAMESPACE::XsAttributeGroupInst attr_grp;
    XS_NAMESPACE::XsTypeInst base;
} 
    :
        #( t:Restriction
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                                                   
                COMMON_NS::String xs_type;
                if (attrs.get("base", &xs_type)) {
                    if ("anyType" != xs_type.right(7)) {
                        XSD->makeRefInst(base, SCHEMA, t->element(),
                                         xs_type, XS_NAMESPACE::Component::type);
                        rst->setBaseType(base); 
                    }
            else
                rst->setAnyBase(true);
                }               
                attr_set = new XS_NAMESPACE::AttributeSet(); 
                rst->setAttributeSet(attr_set.pointer());
                content = rst.pointer();
            }
            (  
                ann = annotation
                {
//TODO              rst->setAppinfo(ann);            
                }
            )?
            ( con = groupRef[rst.pointer()]
                {
                    rst->setConnector(con);     
                }
            | con = all[rst.pointer()] 
                {
                    rst->setConnector(con);     
                }
            | con = choice[rst.pointer()] 
                {
                    rst->setConnector(con);     
                }
            | con = sequence[rst.pointer()] 
                {
                    rst->setConnector(con);     
                }
            )?  

            ( attr = attribute[rst.pointer()]
                {
                    attr_set->addAttribute(attr);        
                }
            | attr_grp = attributeGroupRef
                {
                    attr_set->addAttributeGroup(attr_grp);       
                }
            )* 

            ( any_attr = anyAttribute[rst.pointer()]
                {
                    attr_set->setAnyAttribute(any_attr);     
                }
            )?
            (EndToken)?
        )
    ;


complexExtension[ COMMON_NS::RefCntPtr<XS_NAMESPACE::ComplexContent> ext] 
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsContent> content]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> con;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::AttributeSet> attr_set;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAnyAttribute> any_attr;
    XS_NAMESPACE::XsGroupInst grp;
    XS_NAMESPACE::XsAttributeInst attr;
    XS_NAMESPACE::XsAttributeGroupInst attr_grp;
    XS_NAMESPACE::XsTypeInst base;
} 
    :
        #( t:Extension
            { 
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                                                   
                COMMON_NS::String xs_type;
                if (attrs.get("base", &xs_type)) {
                    XSD->makeRefInst(base, SCHEMA, t->element(),
                                     xs_type, XS_NAMESPACE::Component::type);
                    ext->setBaseType(base); 
                }               

                attr_set = new XS_NAMESPACE::AttributeSet(); 
                ext->setAttributeSet(attr_set.pointer());
                content = ext.pointer();
            }
            (  
                ann = annotation
                {
//TODO:                 ext->setAppinfo(ann);            
                }
            )?
            ( con = groupRef[ext.pointer()] 
                {
                    ext->setConnector(con);   
                }
            | con = all[ext.pointer()] 
                {
                    ext->setConnector(con);     
                }
            | con = choice[ext.pointer()]  
                {
                    ext->setConnector(con);     
                }
            | con = sequence[ext.pointer()]  
                {
                    ext->setConnector(con);     
                }
            )?    

            ( attr = attribute[ext.pointer()] 
                {
                    attr_set->addAttribute(attr);        
                }
            | attr_grp = attributeGroupRef
                {
                    attr_set->addAttributeGroup(attr_grp);       
                }
            )* 

            ( any_attr = anyAttribute[ext.pointer()]
                {
                    attr_set->setAnyAttribute(any_attr);     
                }
            )?
            (EndToken)?    
        )
    ;

topAttributeGroup[bool redefine]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAttributeGroupImpl> attr_grp;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::AttributeSet> attr_set;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsAnyAttribute> any_attr;
    XS_NAMESPACE::XsAttributeInst attr;
    XS_NAMESPACE::XsAttributeGroupInst agrp;
} 
    :
        #( t:AttributeGroup
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                XS_NAMESPACE::XsData::AttributeGroupSpace& attr_space =
                  XSD->attributeGroupSpace();
                
                if (attrs.get("name", &value)) {
                    attr_grp =
                        new XS_NAMESPACE::XsAttributeGroupImpl(t->origin(),
                            XSD->makeCred(value, redefine));
                    COMMON_NS::String id;
                    if (attrs.get("id", &id))
                        attr_grp->putIntoTable(SCHEMA, id);                    
                    if (attrs.checkExt( "errmsg")) {
                        COMMON_NS::String errmsg;
                        if (attrs.get("errmsg", &errmsg)) 
                            attr_grp->setCustomMessage(errmsg);
                    }
                }
                else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(t);
                    goto done;
                }
                attr_set = new XS_NAMESPACE::AttributeSet(); 
                attr_grp->setAttributeSet(attr_set.pointer());
                //if (attr_space.lookup(*attr_grp->constCred())) {
                //    SCHEMA->mstream() << XsMessages::elementRedefinition 
                //              << COMMON_NS::Message::L_ERROR 
                //              << attr_grp->constCred()->name()
                //              << XSP_ORIGIN(attr_grp);
                //}
                //else 
                    attr_space.insert(attr_grp.pointer());
                done:;
            } 
            (
                ann = annotation
                {
                    attr_grp->setAppinfo(ann);           
                }
            )?
            
            ( attr = attribute[attr_grp.pointer()]  
                {
                    attr_set->addAttribute(attr);       
                }
            | agrp = attributeGroupRef
                {
                    attr_set->addAttributeGroup(agrp);      
                }
            )* 
            
            ( any_attr = anyAttribute[attr_grp.pointer()] 
                {
                    attr_set->setAnyAttribute(any_attr);        
                }
            )?
            (EndToken)?     
        )
    ;

attributeGroupRef returns [ XS_NAMESPACE::XsAttributeGroupInst result]
{
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:AttributeGroup
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                if (attrs.get("ref", &value)) {
            XSD->makeRefInst(result, SCHEMA, t->element(),
            value, XS_NAMESPACE::Component::attributeGroup);
                }           
                else {
                    SCHEMA->mstream() << XsMessages::refRequired
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(t);
                }
            }
            (
                ann = annotation
                {
//                    result->setAppinfo(ann);           
                }
            )?
            (EndToken)?    
         )
    ;

topGroup[bool redefine]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsGroupImpl> grp;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo>  ann; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> child; 
}
    :
        #( t:Group
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                XS_NAMESPACE::XsData::GroupSpace& gspace = XSD->groupSpace();
                
                if (attrs.get("ref", &value)) {
                    SCHEMA->mstream() << XsMessages::refProhibited 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);  
                    goto done;
                }
                else if (attrs.get("name", &value)) {
                    grp = new XS_NAMESPACE::XsGroupImpl(t->origin(),
                        XSD->makeCred(value, redefine));
                    COMMON_NS::String id;
                    if (attrs.get("id", &id))
                        grp->putIntoTable(SCHEMA, id);
                }
                else {
                    SCHEMA->mstream() << XsMessages::nameRequired
                              << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(t);
                    goto done;
                }
                if ((attrs.has("minOccurs")) || (attrs.has("maxOccurs"))) {
                    SCHEMA->mstream() << XsMessages::noOccursOnTopLevel
                              << COMMON_NS::Message::L_ERROR  << XSP_ORIGIN(t);
                }                
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        grp->setCustomMessage(errmsg);
                }
        //if (gspace.lookup(*grp->constCred())) {
        //            SCHEMA->mstream() << XsMessages::elementRedefinition 
         //                     << COMMON_NS::Message::L_ERROR
         //                     << grp->constCred()->name() 
         //                     << XSP_ORIGIN(grp);
         //       }
         //       else 
                    gspace.insert(grp.pointer());
                
                done:;
            }
            (  
                ann = annotation
                {
                    grp->setAppinfo(ann);            
                }
            )?
            (
                child = all[grp.pointer()]
                {
                    child->setParent(0);
                    grp->setConnector(child);
                }
            |   child = choice[grp.pointer()]
                {
                    child->setParent(0);
                    grp->setConnector(child);
                }
            |   child = sequence[grp.pointer()] 
                {
                    child->setParent(0);
                    grp->setConnector(child);
                }
            )?
            (EndToken)?     
        )
    ;

groupRef[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> connector]
{
    XS_NAMESPACE::XsGroupInst grp_inst;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:Group
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                XS_NAMESPACE::GroupConnector* grp_con =
            new XS_NAMESPACE::GroupConnector
            (XS_NAMESPACE::Connector::GROUPREF, t->origin());
                grp_con->setParentPiece(parent);
                uint occurs;
                COMMON_NS::String value;
                if (attrs.getUnsigned("minOccurs", &occurs)) {
                    grp_con->setMinOccur(SCHEMA, occurs);
                }
                if (attrs.get("maxOccurs", &value)) {
                    if ("unbounded" == value.lower())
                        grp_con->setMaxOccur(SCHEMA, ~0);
                    else if (attrs.getUnsigned("maxOccurs", &occurs))
                        grp_con->setMaxOccur(SCHEMA, occurs);
                }
                                
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        grp_con->setCustomMessage(errmsg);
                }
                
                connector = grp_con;
                if (attrs.get("ref", &value)) {
            XSD->makeRefInst(grp_inst, SCHEMA, t->element(),
            value, XS_NAMESPACE::Component::group);
            grp_con->setGroupRef(grp_inst);
                }           
                else {
                    SCHEMA->mstream() << XsMessages::refRequired 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);
                }
            }
            (
                ann = annotation
                {
                    connector->setAppinfo(ann);           
                }
            )?
            (EndToken)?     
        )
    ;


all[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> connector]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::AllConnector> local_connector;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Particle> elem;  
}
    :
        #( t:All
            {
        local_connector = new XS_NAMESPACE::AllConnector
          (XS_NAMESPACE::Connector::ALL, t->origin());
                connector = local_connector.pointer();
                connector->setParentPiece(parent);
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint occurs;
                COMMON_NS::String value;
                if (attrs.getUnsigned("minOccurs", &occurs)) {
                    if (1 != occurs) {
                        SCHEMA->mstream() << XsMessages::minOccursOut 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);
                        occurs =1;
                    }
                    connector->setMinOccur(SCHEMA, occurs);
                }
                if (attrs.get("maxOccurs", &value)) {
                    if ("unbounded" == value.lower())
                        SCHEMA->mstream() << XsMessages::maxOccursOut 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);
                    else if (attrs.getUnsigned("maxOccurs", &occurs)) {
                        if (1 != occurs)
                SCHEMA->mstream() << XsMessages::maxOccursOut 
                                  << COMMON_NS::Message::L_ERROR 
                                              << XSP_ORIGIN(t);
                    }
                    connector->setMaxOccur(SCHEMA, 1);                    
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    connector->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        connector->setCustomMessage(errmsg);
                }
            }
            (  
                ann = annotation
                {
                    local_connector->setAppinfo(ann);          
                }
            )?
            (
                elem = element[connector.pointer()]
                {
                    local_connector->addChild(elem.pointer());
                }
            )*
            (EndToken)?
        )
    ;


choice[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> connector]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::ChoiceConnector> local_connector;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Particle> elem; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> child;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::WildcardParticle> any_grp;

}
    :
        #( t:Choice
            {
        local_connector = new XS_NAMESPACE::ChoiceConnector
            (XS_NAMESPACE::Connector::CHOICE, t->origin());
                connector = local_connector.pointer();
                connector->setParentPiece(parent);
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint occurs;
                COMMON_NS::String value;
                if (attrs.getUnsigned("minOccurs", &occurs))
                    connector->setMinOccur(SCHEMA, occurs);
                if (attrs.get("maxOccurs", &value)) {
                    if ("unbounded" == value.lower())
                        connector->setMaxOccur(SCHEMA, ~0);
                    else if (attrs.getUnsigned("maxOccurs", &occurs))
                        connector->setMaxOccur(SCHEMA, occurs);
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    connector->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        connector->setCustomMessage(errmsg);
                }
            }
            (  
                ann = annotation
                {
                    local_connector->setAppinfo(ann);          
                }
            )?
            (   elem = element[connector.pointer()] 
                {
                    local_connector->addChild(elem.pointer());
                }
            |   child = groupRef[connector.pointer()]
                {
                    child->setParent(connector.pointer());
                    local_connector->addChild(child.pointer());
                }
            |   child = choice[connector.pointer()]
                {
                    child->setParent(connector.pointer());
                    local_connector->addChild(child.pointer());
                }
            |   child = sequence[connector.pointer()] 
                {
                    child->setParent(connector.pointer());
                    local_connector->addChild(child.pointer());
                }
            |   any_grp = any[connector.pointer()]
                {
                    local_connector->addChild(any_grp.pointer());
                }
            )*
           (EndToken)?      
        )
    ;

sequence[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> connector]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::SequenceConnector> local_connector;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Particle> elem; 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Connector> child;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::WildcardParticle> any_grp;
}
    :
        #( t:Sequence
            {
                local_connector =
                    new XS_NAMESPACE::SequenceConnector
                    (XS_NAMESPACE::Connector::SEQUENCE, t->origin());
                connector = local_connector.pointer();
                connector->setParentPiece(parent);
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint occurs;
                COMMON_NS::String value;
                if (attrs.getUnsigned("minOccurs", &occurs))
                    connector->setMinOccur(SCHEMA, occurs);
                if (attrs.get("maxOccurs", &value)) {
                    if ("unbounded" == value.lower())
                        connector->setMaxOccur(SCHEMA, ~0);
                    else if (attrs.getUnsigned("maxOccurs", &occurs))
                        connector->setMaxOccur(SCHEMA, occurs);
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    connector->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        connector->setCustomMessage(errmsg);
                }
            }
            (  
                ann = annotation
                {
                    local_connector->setAppinfo(ann);          
                }
            )?
            (   elem = element[connector.pointer()] 
                {
                    local_connector->addChild(elem.pointer());
                }
            |   child = groupRef[connector.pointer()] 
                {
                    child->setParent(connector.pointer());
                    local_connector->addChild(child.pointer());
                }
            |   child = choice[connector.pointer()]
                {
                    child->setParent(connector.pointer());
                    local_connector->addChild(child.pointer());
                }
            |   child = sequence[connector.pointer()] 
                {
                    child->setParent(connector.pointer());
                    local_connector->addChild(child.pointer());
                }
            |   any_grp = any[connector.pointer()]
                {
                    local_connector->addChild(any_grp.pointer());
                }
            )*
         (EndToken)?        
        )
         {
         }
    ;

any[XS_NAMESPACE::Piece* parent] returns
    [ COMMON_NS::RefCntPtr<XS_NAMESPACE::WildcardParticle> any_grp]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::Wildcard*     wc;
}
    :
        #( t:Any
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
        XS_NAMESPACE::Wildcard::ProcessMode mode =
            XS_NAMESPACE::Wildcard::WC_STRICT;
                if (attrs.get("processContents", &value)) {
                    if ("skip" == value.lower())
                        mode = XS_NAMESPACE::Wildcard::SKIP;
                    else if ("lax" == value.lower())
                        mode = XS_NAMESPACE::Wildcard::LAX;
                    else
                        mode = XS_NAMESPACE::Wildcard::WC_STRICT;
                }
                if (attrs.get("namespace", &value))
                    wc = XS_NAMESPACE::Wildcard::make(SCHEMA, mode, value);
                else
                    wc = new XS_NAMESPACE::AnyNsWildcard(mode);
                any_grp = new XS_NAMESPACE::WildcardParticle(wc, t->origin());
                any_grp->setParentPiece(parent);
                uint occurs;
                if (attrs.getUnsigned("minOccurs", &occurs))
                    any_grp->setMinOccur(SCHEMA, occurs);
                if (attrs.get("maxOccurs", &value)) {
                    if ("unbounded" == value.lower())
                        any_grp->setMaxOccur(SCHEMA, ~0);
                    else if (attrs.getUnsigned("maxOccurs", &occurs))
                        any_grp->setMaxOccur(SCHEMA, occurs);
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    any_grp->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        any_grp->setCustomMessage(errmsg);
                }
            }
            (  
                ann = annotation
                {
                    any_grp->setAppinfo(ann);          
                }
            )?
         (EndToken)?        
        )
    ;

topNotation[bool redefine]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsNotationImpl> nt;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;

}
    :
        #( t:Notation 
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                XS_NAMESPACE::XsData::NotationSpace& nspace =
                    XSD->notationSpace();
                
                if (attrs.get("name", &value)) {
                nt = new XS_NAMESPACE::XsNotationImpl(t->origin(),
                    XSD->makeCred(value, redefine));
                    COMMON_NS::String id;
                    if (attrs.get("id", &id))
                        nt->putIntoTable(SCHEMA, id);
                    if (attrs.checkExt( "errmsg")) {
                        COMMON_NS::String errmsg;
                        if (attrs.get("errmsg", &errmsg)) 
                            nt->setCustomMessage(errmsg);
                    }
                }
                else {
                    SCHEMA->mstream() << XsMessages::nameRequired 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);
                    goto done;
                }                
                if (attrs.get("public", &value)) {
                    nt->public_ = value;
                }
                else {
                    SCHEMA->mstream() << XsMessages::refProhibited 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);    
                    goto done;
                }
                if (attrs.get("system", &value)) {
                    nt->uri_ = value;
                }
        //if (nspace.lookup(*nt->constCred())) {
        //            SCHEMA->mstream() << XsMessages::elementRedefinition 
        //                              << COMMON_NS::Message::L_ERROR 
        //                              << nt->constCred()->name()
        //                              << XSP_ORIGIN(nt);
        //        }
        //        else
                    nspace.insert(nt.pointer());
                done:;
            }
            (  
                ann = annotation
                {
                    nt->setAppinfo(ann);       
                }
            )?
            (EndToken)?     
        )
    ;  

facets[XS_NAMESPACE::Piece* parent]
    returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
  :
    facet = enumeration 
    | facet = length 
    | facet = maxExclusive 
    | facet = maxInclusive 
    | facet = maxLength 
    | facet = minExclusive 
    | facet = minInclusive 
    | facet = minLength 
    | facet = pattern 
    | facet = totalDigits
    | facet = fractionDigits
    | facet = whitespace
    {
    if (!facet.isNull())
            facet->setParentPiece(parent);
    }
    ;

enumeration returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
    XS_NAMESPACE::EnumerationFacet* enumFacet;
}
    :
        #( t:Enumeration
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.get("value", &value)) {
                    enumFacet =
                        new XS_NAMESPACE::EnumerationFacet(value, fixed);
                }
                else {
                    enumFacet = new XS_NAMESPACE::EnumerationFacet(fixed);
                         SCHEMA->mstream() << XsMessages::valueNotDefined 
                                           << COMMON_NS::Message::L_ERROR  
                                           << XSP_ORIGIN(t);  
                }
                facet = enumFacet;
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
                if (attrs.checkExt( "getEnum")) {
                    COMMON_NS::String tag;
                    if (attrs.get("getEnum", &tag))
                        enumFacet->setTag(tag);
                }
                if (attrs.checkExt( "title")) {
                    COMMON_NS::String title;
                    if (attrs.get("title", &title))
                        enumFacet->setTitle(title);
                }

            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)?
        )
    ;
 
length returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:Length
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.getUnsigned("value", &value)) {
                    facet = new  XS_NAMESPACE::LengthFacet(value, fixed);
                }
                else {
                    facet = new  XS_NAMESPACE::LengthFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)?
        )
    ;
  
maxExclusive returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:MaxExclusive
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.get("value", &value)) {
            facet = new XS_NAMESPACE::MaxExclusiveFacet(value, fixed);
                }
                else {
            facet = new XS_NAMESPACE::MaxExclusiveFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined
                              << COMMON_NS::Message::L_ERROR  
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                } 
            }
        ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)?
        )
    ;
 
maxInclusive returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:MaxInclusive
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.get("value", &value)) {
            facet = new XS_NAMESPACE::MaxInclusiveFacet(value, fixed);
                }
                else {
            facet = new XS_NAMESPACE::MaxInclusiveFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)? 
        )
    ;
 
maxLength returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:MaxLength
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.getUnsigned("value", &value)) {
            facet = new XS_NAMESPACE::MaxLengthFacet(value, fixed);
                }
                else {
            facet = new XS_NAMESPACE::MaxLengthFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)?
        )
    ;
 
minExclusive returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
 { 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
   :
        #( t:MinExclusive
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.get("value", &value)) {
            facet = new XS_NAMESPACE::MinExclusiveFacet(value, fixed);
                }
                else {
            facet = new XS_NAMESPACE::MinExclusiveFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)? 
        )
   ;

minInclusive returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:MinInclusive
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.get("value", &value)) {
            facet = new XS_NAMESPACE::MinInclusiveFacet(value, fixed);
        } else {
            facet = new XS_NAMESPACE::MinInclusiveFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)? 
        )
    ;
 
minLength returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:MinLength
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.getUnsigned("value", &value)) {
                    facet = new  XS_NAMESPACE::MinLengthFacet(value, fixed);
                }
                else {
                    facet = new  XS_NAMESPACE::MinLengthFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }   
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)? 
        )
    ;
 
pattern returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:Pattern
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.get("value", &value)) {
            facet = new XS_NAMESPACE::PatternFacet(value, fixed);
                }
                else {
            facet = new XS_NAMESPACE::PatternFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)?
        )
    ;

totalDigits returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:TotalDigits
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.getUnsigned("value", &value)) {
            facet = new XS_NAMESPACE::TotalDigitsFacet(value, fixed);
                }
                else {
            facet = new XS_NAMESPACE::TotalDigitsFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR << XSP_ORIGIN(t);
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)?
        )
    ;
 
fractionDigits returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
        #( t:FractionDigits
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                uint value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.getUnsigned("value", &value)) {
            facet =
            new XS_NAMESPACE::FractionDigitsFacet(value, fixed);
                }
                else {
            facet = new XS_NAMESPACE::FractionDigitsFacet(fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                }
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
            }
            ( ann = annotation
                {
                    facet->setAppinfo(ann);
                }
            )?
            (EndToken)?
        )
    ;

whitespace returns [ COMMON_NS::RefCntPtr<XS_NAMESPACE::XsFacet> facet]
{ 
    COMMON_NS::RefCntPtr<XS_NAMESPACE::Appinfo> ann;
}
    :
       #( t:Whitespace
            {
                XS_NAMESPACE::AttributeParser attrs(t->origin(), SCHEMA);
                COMMON_NS::String value;
                bool fixed;
                attrs.getBoolean("fixed", &fixed);
                if (attrs.get("value", &value)) {
                    XS_NAMESPACE::WhiteSpaceFacet::StripType strip;
                    if ("preserve" == value.lower())
                        strip = XS_NAMESPACE::WhiteSpaceFacet::PRESERVE;
                    else if ("replace" == value.lower())
                        strip = XS_NAMESPACE::WhiteSpaceFacet::REPLACE;
                    else if ("collapse" == value.lower())
                        strip = XS_NAMESPACE::WhiteSpaceFacet::COLLAPSE;
                    else {
                        SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);
                        strip = XS_NAMESPACE::WhiteSpaceFacet::PRESERVE;
                    }
                    
            facet = new XS_NAMESPACE::WhiteSpaceFacet(strip, fixed);
                }
                else {
            facet = new XS_NAMESPACE::WhiteSpaceFacet
                        (XS_NAMESPACE::WhiteSpaceFacet::PRESERVE, fixed);
                    SCHEMA->mstream() << XsMessages::valueNotDefined 
                              << COMMON_NS::Message::L_ERROR 
                      << XSP_ORIGIN(t);  
                } 
                COMMON_NS::String id;
                if (attrs.get("id", &id))
                    facet->putIntoTable(SCHEMA, id);
                if (attrs.checkExt( "errmsg")) {
                    COMMON_NS::String errmsg;
                    if (attrs.get("errmsg", &errmsg)) 
                        facet->setCustomMessage(errmsg);
                }
           }         
           ( ann = annotation
               {
                   facet->setAppinfo(ann);
               }
           )?
           (EndToken)?
        )
    ;
