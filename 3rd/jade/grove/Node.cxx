// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "Boolean.h"
#include "Node.h"
#include "macros.h"
#include <string.h>

#ifdef GROVE_NAMESPACE
namespace GROVE_NAMESPACE {
#endif

bool Node::queryInterface(IID, const void *&) const
{
  return 0;
}

AccessResult Node::getMessages(NodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getSeverity(Severity &) const
{
  return accessNotInClass;
}

AccessResult Node::getOrigin(NodePtr &) const
{
  return accessNull;
}

AccessResult Node::getParent(NodePtr &) const
{
  return accessNull;
}

AccessResult Node::getGroveRoot(NodePtr &) const
{
  return accessNull;
}

AccessResult Node::getTreeRoot(NodePtr &nd) const
{
  // FIXME avoid cast -- need copy method
  nd.assign((Node *)this);
  for (;;) {
    AccessResult res = nd->getParent(nd);
    if (res != accessOK) {
      if (res == accessTimeout)
	return res;
      break;
    }
  }
  return accessOK;
}

AccessResult Node::getAttributes(NamedNodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getName(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getSystemData(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getEntity(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getEntityName(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getExternalId(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getNotation(NodePtr &) const
{
  return accessNotInClass;
}
AccessResult Node::getText(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getNotationName(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getEntityType(EntityType &) const
{
  return accessNotInClass;
}

AccessResult Node::getDefaulted(bool &) const
{
  return accessNotInClass;
}

AccessResult Node::getPublicId(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getSystemId(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getGeneratedSystemId(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getValue(NodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getTokenSep(GroveChar &) const
{
  return accessNotInClass;
}

AccessResult Node::getImplied(bool &) const
{
  return accessNotInClass;
}

AccessResult Node::getGi(GroveString &) const
{
  return accessNotInClass;
}

bool Node::hasGi(GroveString) const
{
  return 0;
}

AccessResult Node::getId(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getContent(NodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getIncluded(bool &) const
{
  return accessNotInClass;
}

AccessResult Node::getMustOmitEndTag(bool &) const
{
  return accessNotInClass;
}

AccessResult Node::getToken(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getReferent(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getGoverning(bool &) const
{
  return accessNotInClass;
}

AccessResult Node::getGeneralEntities(NamedNodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getNotations(NamedNodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getSgmlConstants(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getApplicationInfo(GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::getProlog(NodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getEpilog(NodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getDocumentElement(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getElements(NamedNodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getEntities(NamedNodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getDefaultedEntities(NamedNodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getGoverningDoctype(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getDoctypesAndLinktypes(NamedNodeListPtr &) const
{
  return accessNotInClass;
}

AccessResult Node::getNonSgml(unsigned long &) const
{
  return accessNotInClass;
}

AccessResult Node::nextSibling(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::nextChunkAfter(NodePtr &nd) const
{
  AccessResult ret = firstChild(nd);
  switch (ret) {
  case accessOK:
  case accessTimeout:
    return ret;
  default:
    break;
  }
  for (;;) {
    ret = nextChunkSibling(nd);
    switch (ret) {
    case accessOK:
    case accessTimeout:
      return ret;
    default:
      break;
    }
    ret = getParent(nd);
    if (ret != accessOK)
      break;
  }
  return ret;
}

AccessResult Node::nextChunkSibling(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::firstChild(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::charChunk(const SdataMapper &, GroveString &) const
{
  return accessNotInClass;
}

AccessResult Node::firstSibling(NodePtr &) const
{
  return accessNotInClass;
}

AccessResult Node::siblingsIndex(unsigned long &) const
{
  return accessNotInClass;
}

AccessResult Node::tokens(GroveString &) const
{
  return accessNotInClass;
}

unsigned long Node::hash() const
{
  // Hopefully implementations will do better.
  return 0;
}

AccessResult Node::elementIndex(unsigned long &) const
{
  return accessNotInClass;
}

bool Node::chunkContains(const Node &nd) const
{
  return *this == nd;
}

AccessResult Node::attributeRef(unsigned long n, NodePtr &ptr) const
{
  NamedNodeListPtr atts;
  AccessResult ret = getAttributes(atts);
  if (ret != accessOK)
    return ret;
  return atts->nodeList()->ref(n, ptr);
}

AccessResult Node::followSiblingRef(unsigned long n, NodePtr &ptr) const
{
  if (n == 0)
    return nextSibling(ptr);
  NodePtr tem;
  AccessResult ret = nextSibling(tem);
  if (ret != accessOK)
    return ret;
  while (--n > 0) {
    ret = tem.assignNextSibling();
    if (ret != accessOK)
      return ret;
  }
  return tem->nextSibling(ptr);
}

AccessResult Node::property(ComponentName::Id id,
			    const SdataMapper &mapper,
			    PropertyValue &value) const
{
  AccessResult ret;
  switch (id) {
  default:
    ret = accessNotInClass;
    break;
  case ComponentName::idEntityType:
    {
      EntityType type;
      ret = getEntityType(type);
      if (ret == accessOK) {
	switch (type) {
#define ENUM(e, E) case e: value.set(ComponentName::id##E); break;
	ENUM(text, Text)
	ENUM(cdata, Cdata)
	ENUM(sdata, Sdata)
	ENUM(ndata, Ndata)
	ENUM(pi, Pi)
	ENUM(subdocument, Subdocument)
	default:
	  ret = accessNotInClass;
	}
      }
    }
  case ComponentName::idChar:
    {
      GroveChar tem;
      ret = getChar(mapper, tem);
      if (ret == accessOK)
	value.set(tem);
      break;
    }
#define PROP(Name, Type) \
  case ComponentName::id##Name: \
    { \
      Type tem; \
      ret = get##Name(tem); \
      if (ret == accessOK) \
	value.set(tem); \
      break; \
    }
#define PROP_BOOLEAN(name) PROP(name, bool)
#define PROP_CHAR(name) PROP(name, GroveChar)
#define PROP_COMPNAME(name) PROP(name, ComponentName::Id)
#define PROP_CNMLIST(name) PROP(name, const ComponentName::Id *)
#define PROP_STRING(name) PROP(name, GroveString)
#define PROP_NODE(name) PROP(name, NodePtr)
#define PROP_NODELIST(name) PROP(name, NodeListPtr)
#define PROP_NMNDLIST(name) PROP(name, NamedNodeListPtr)
  PROP_COMPNAME(ClassName)
  PROP_COMPNAME(ChildrenPropertyName)
  PROP_COMPNAME(DataPropertyName)
  PROP_COMPNAME(DataSepPropertyName)
  PROP_COMPNAME(OriginToSubnodeRelPropertyName)
  PROP_CNMLIST(SubnodePropertyNames)
  PROP_CNMLIST(AllPropertyNames)
  PROP_BOOLEAN(Defaulted)
  PROP_BOOLEAN(Governing)
  PROP_BOOLEAN(Implied)
  PROP_BOOLEAN(Included)
  PROP_BOOLEAN(MustOmitEndTag)
  PROP_CHAR(TokenSep)
  PROP_NMNDLIST(Attributes)
  PROP_NMNDLIST(DefaultedEntities)
  PROP_NMNDLIST(DoctypesAndLinktypes)
  PROP_NMNDLIST(Elements)
  PROP_NMNDLIST(Entities)
  PROP_NMNDLIST(GeneralEntities)
  PROP_NMNDLIST(Notations)
  PROP_NODE(DocumentElement)
  PROP_NODE(Entity)
  PROP_NODE(ExternalId)
  PROP_NODE(GoverningDoctype)
  PROP_NODE(GroveRoot)
  PROP_NODE(Notation)
  PROP_NODE(Origin)
  PROP_NODE(Parent)
  PROP_NODE(Referent)
  PROP_NODE(SgmlConstants)
  PROP_NODE(TreeRoot)
  PROP_NODELIST(Content)
  PROP_NODELIST(Epilog)
  PROP_NODELIST(Prolog)
  PROP_NODELIST(Value)
  PROP_STRING(ApplicationInfo)
  PROP_STRING(EntityName)
  PROP_STRING(GeneratedSystemId)
  PROP_STRING(Gi)
  PROP_STRING(Id)
  PROP_STRING(Name)
  PROP_STRING(NotationName)
  PROP_STRING(PublicId)
  PROP_STRING(SystemData)
  PROP_STRING(SystemId)
  PROP_STRING(Text)
  PROP_STRING(Token)
  }
#undef PROP
#undef PROP_BOOLEAN
#undef PROP_CHAR
#undef PROP_NMNDLIST
#undef PROP_NODE
#undef PROP_NODELIST
#undef PROP_STRING
  return ret;
}

AccessResult NodeList::ref(unsigned long n, NodePtr &ptr) const
{
  if (n == 0)
    return first(ptr);
  NodeListPtr tem;
  AccessResult ret = rest(tem);
  if (ret != accessOK)
    return ret;
  while (--n > 0) {
    ret = tem.assignRest();
    if (ret != accessOK)
      return ret;
  }
  return tem->first(ptr);
}

NodeListPtr NamedNodeList::nodeListNoOrder() const
{
  return nodeList();
}

struct NodeNameNodeVisitor : public NodeVisitor {
  NodeNameNodeVisitor(NamedNodeList::Type t, GroveString &name)
    : ret(accessNotInClass), type(t), nameP(&name) { }
  AccessResult ret;
  GroveString *nameP;
  NamedNodeList::Type type;
  void element(Node &nd) {
    if (type == NamedNodeList::elements)
      ret = nd.getId(*nameP);
  }
  void attributeAssignment(Node &nd) {
    if (type == NamedNodeList::attributes)
      ret = nd.getName(*nameP);
  }
  void documentType(Node &nd) {
    if (type == NamedNodeList::doctypesAndLinktypes)
      ret = nd.getName(*nameP);
  }
  void entity(Node &nd) {
    if (type == NamedNodeList::entities)
      ret = nd.getName(*nameP);
  }
  void notation(Node &nd) {
    if (type == NamedNodeList::notations)
      ret = nd.getName(*nameP);
  }
};

AccessResult NamedNodeList::nodeName(const NodePtr &node, GroveString &name) const
{
  NodeNameNodeVisitor v(type(), name);
  node->accept(v);
  return v.ret;
}
  
void NodeVisitor::sgmlDocument(Node &)
{
}

void NodeVisitor::sgmlConstants(Node &)
{
}

void NodeVisitor::dataChar(Node &)
{
}

void NodeVisitor::element(Node &)
{
}

void NodeVisitor::attributeAssignment(Node &)
{
}

void NodeVisitor::attributeValueToken(Node &)
{
}

void NodeVisitor::pi(Node &)
{
}

void NodeVisitor::sdata(Node &)
{
}

void NodeVisitor::documentType(Node &)
{
}

void NodeVisitor::entity(Node &)
{
}

void NodeVisitor::notation(Node &)
{
}

void NodeVisitor::externalId(Node &)
{
}

void NodeVisitor::externalData(Node &)
{
}

void NodeVisitor::subdocument(Node &)
{
}

void NodeVisitor::nonSgml(Node &)
{
}

void NodeVisitor::message(Node &)
{
}

bool GroveString::operator==(const GroveString &str) const
{
  if (size() != str.size())
    return 0;
  if (size() == 0)
    return 1;
  if (memcmp(data(), str.data(), size() * sizeof(GroveChar)) == 0)
    return 1;
  return 0;
}

SdataMapper::~SdataMapper()
{
}

bool SdataMapper::sdataMap(GroveString name, GroveString text, GroveChar &) const
{
  return 0;
}

const char *ComponentName::rcsName(Id id)
{
  static const char *const names[] = {
    "allpns",
    "appinfo",
    "attasgn",
    "atts",
    "attvaltk",
    "cdata",
    "char",
    "childpn",
    "classnm",
    "content",
    "datachar",
    "datapn",
    "dseppn",
    "dflted",
    "dfltents",
    "docelem",
    "doctype",
    "dtlts",
    "element",
    "elements",
    "entities",
    "entity",
    "entname",
    "enttype",
    "epilog",
    "extdata",
    "extid",
    "genents",
    "gensysid",
    "gi",
    "govdt",
    "govrning",
    "grovroot",
    "id",
    "implied",
    "included",
    "momitend",
    "name",
    "ndata",
    "notation",
    "notname",
    "nots",
    "origin",
    "otsrelpn",
    "parent",
    "pi",
    "prolog",
    "pubid",
    "referent",
    "sdata",
    "sgmlcsts",
    "sgmldoc",
    "subdoc",
    "subpns",
    "sysdata",
    "sysid",
    "text",
    "token",
    "tokensep",
    "treeroot",
    "value",
  };
  if (id < 0 || id >= SIZEOF(names))
    return 0;
  return names[id];
}

const char *ComponentName::sdqlName(Id id)
{
  static const char *const names[] = {
    "all-property-names",
    "application-info",
    "attribute-assignment",
    "attributes",
    "attribute-value-token",
    "cdata",
    "char",
    "children-property-name",
    "class-name",
    "content",
    "data-char",
    "data-property-name",
    "data-sep-property-name",
    "defaulted?",
    "defaulted-entities",
    "document-element",
    "document-type",
    "doctypes-and-linktypes",
    "element",
    "elements",
    "entities",
    "entity",
    "entity-name",
    "entity-type",
    "epilog",
    "external-data",
    "external-id",
    "general-entities",
    "generated-system-id",
    "gi",
    "governing-doctype",
    "governing?",
    "grove-root",
    "id",
    "implied?",
    "included?",
    "must-omit-end-tag?",
    "name",
    "ndata",
    "notation",
    "notation-name",
    "notations",
    "origin",
    "origin-to-subnode-rel-property-name",
    "parent",
    "pi",
    "prolog",
    "public-id",
    "referent",
    "sdata",
    "sgml-constants",
    "sgml-document",
    "subdocument",
    "subnode-property-names",
    "system-data",
    "system-id",
    "text",
    "token",
    "token-sep",
    "tree-root",
    "value",
  };
  if (id < 0 || id >= SIZEOF(names))
    return 0;
  return names[id];
}

#define INTRINSIC_PROPS \
  ComponentName::idClassName, \
  ComponentName::idGroveRoot, \
  ComponentName::idSubnodePropertyNames, \
  ComponentName::idAllPropertyNames, \
  ComponentName::idChildrenPropertyName, \
  ComponentName::idDataPropertyName, \
  ComponentName::idDataSepPropertyName, \
  ComponentName::idParent, \
  ComponentName::idTreeRoot, \
  ComponentName::idOrigin, \
  ComponentName::idOriginToSubnodeRelPropertyName

  static const ComponentName::Id noProps[] = { ComponentName::noId };

static const ComponentName::Id allProps_externalId[] = {
  INTRINSIC_PROPS,
  ComponentName::idPublicId,
  ComponentName::idSystemId,
  ComponentName::idGeneratedSystemId,
  ComponentName::noId
};
const ClassDef ClassDef::externalId = {
  ComponentName::idExternalId,
  allProps_externalId,
  noProps,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_documentType[] = {
  INTRINSIC_PROPS,
  ComponentName::idName,
  ComponentName::idGoverning,
  ComponentName::idGeneralEntities,
  ComponentName::idNotations,
  ComponentName::noId
};
static const ComponentName::Id subnodeProps_documentType[] = {
  ComponentName::idGeneralEntities,
  ComponentName::idNotations,
  ComponentName::noId
};
const ClassDef ClassDef::documentType = {
  ComponentName::idDocumentType,
  allProps_documentType,
  subnodeProps_documentType,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_attributeValueToken[] = {
  INTRINSIC_PROPS,
  ComponentName::idToken,
  ComponentName::idEntity,
  ComponentName::idNotation,
  ComponentName::idReferent,
  ComponentName::noId
};
const ClassDef ClassDef::attributeValueToken = {
  ComponentName::idAttributeValueToken,
  allProps_attributeValueToken,
  noProps,
  ComponentName::noId,
  ComponentName::idToken,
  ComponentName::noId
};
static const ComponentName::Id allProps_sgmlDocument[] = {
  INTRINSIC_PROPS,
  ComponentName::idSgmlConstants,
  ComponentName::idApplicationInfo,
  ComponentName::idProlog,
  ComponentName::idEpilog,
  ComponentName::idGoverningDoctype,
  ComponentName::idDoctypesAndLinktypes,
  ComponentName::idDocumentElement,
  ComponentName::idElements,
  ComponentName::idEntities,
  ComponentName::idDefaultedEntities,
  ComponentName::noId
};
static const ComponentName::Id subnodeProps_sgmlDocument[] = {
  ComponentName::idSgmlConstants,
  ComponentName::idProlog,
  ComponentName::idEpilog,
  ComponentName::idDoctypesAndLinktypes,
  ComponentName::idDocumentElement,
  ComponentName::idDefaultedEntities,
  ComponentName::noId
};
const ClassDef ClassDef::sgmlDocument = {
  ComponentName::idSgmlDocument,
  allProps_sgmlDocument,
  subnodeProps_sgmlDocument,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_dataChar[] = {
  INTRINSIC_PROPS,
  ComponentName::idChar,
  ComponentName::noId
};
const ClassDef ClassDef::dataChar = {
  ComponentName::idDataChar,
  allProps_dataChar,
  noProps,
  ComponentName::noId,
  ComponentName::idChar,
  ComponentName::noId
};
static const ComponentName::Id allProps_subdocument[] = {
  INTRINSIC_PROPS,
  ComponentName::idEntityName,
  ComponentName::idEntity,
  ComponentName::noId
};
const ClassDef ClassDef::subdocument = {
  ComponentName::idSubdocument,
  allProps_subdocument,
  noProps,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_pi[] = {
  INTRINSIC_PROPS,
  ComponentName::idSystemData,
  ComponentName::idEntityName,
  ComponentName::idEntity,
  ComponentName::noId
};
const ClassDef ClassDef::pi = {
  ComponentName::idPi,
  allProps_pi,
  noProps,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_element[] = {
  INTRINSIC_PROPS,
  ComponentName::idGi,
  ComponentName::idId,
  ComponentName::idAttributes,
  ComponentName::idContent,
  ComponentName::idIncluded,
  ComponentName::idMustOmitEndTag,
  ComponentName::noId
};
static const ComponentName::Id subnodeProps_element[] = {
  ComponentName::idAttributes,
  ComponentName::idContent,
  ComponentName::noId
};
const ClassDef ClassDef::element = {
  ComponentName::idElement,
  allProps_element,
  subnodeProps_element,
  ComponentName::idContent,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_notation[] = {
  INTRINSIC_PROPS,
  ComponentName::idName,
  ComponentName::idExternalId,
  ComponentName::noId
};
static const ComponentName::Id subnodeProps_notation[] = {
  ComponentName::idExternalId,
  ComponentName::noId
};
const ClassDef ClassDef::notation = {
  ComponentName::idNotation,
  allProps_notation,
  subnodeProps_notation,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_externalData[] = {
  INTRINSIC_PROPS,
  ComponentName::idEntityName,
  ComponentName::idEntity,
  ComponentName::noId
};
const ClassDef ClassDef::externalData = {
  ComponentName::idExternalData,
  allProps_externalData,
  noProps,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
static const ComponentName::Id allProps_attributeAssignment[] = {
  INTRINSIC_PROPS,
  ComponentName::idValue,
  ComponentName::idName,
  ComponentName::idImplied,
  ComponentName::idTokenSep,
  ComponentName::noId
};
static const ComponentName::Id subnodeProps_attributeAssignment[] = {
  ComponentName::idValue,
  ComponentName::noId
};
const ClassDef ClassDef::attributeAssignment = {
  ComponentName::idAttributeAssignment,
  allProps_attributeAssignment,
  subnodeProps_attributeAssignment,
  ComponentName::idValue,
  ComponentName::noId,
  ComponentName::idTokenSep
};
static const ComponentName::Id allProps_sdata[] = {
  INTRINSIC_PROPS,
  ComponentName::idSystemData,
  ComponentName::idChar,
  ComponentName::idEntityName,
  ComponentName::idEntity,
  ComponentName::noId
};
const ClassDef ClassDef::sdata = {
  ComponentName::idSdata,
  allProps_sdata,
  noProps,
  ComponentName::noId,
  ComponentName::idChar,
  ComponentName::noId
};
static const ComponentName::Id allProps_entity[] = {
  INTRINSIC_PROPS,
  ComponentName::idName,
  ComponentName::idEntityType,
  ComponentName::idText,
  ComponentName::idExternalId,
  ComponentName::idAttributes,
  ComponentName::idNotationName,
  ComponentName::idNotation,
  ComponentName::idDefaulted,
  ComponentName::noId
};
static const ComponentName::Id subnodeProps_entity[] = {
  ComponentName::idExternalId,
  ComponentName::idAttributes,
  ComponentName::noId
};
const ClassDef ClassDef::entity = {
  ComponentName::idEntity,
  allProps_entity,
  subnodeProps_entity,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
const ClassDef ClassDef::sgmlConstants = {
  ComponentName::idSgmlConstants,
  noProps,
  noProps,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
// FIXME
const ClassDef ClassDef::nonSgml = {
  ComponentName::noId,
  noProps,
  noProps,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};
// FIXME
const ClassDef ClassDef::message = {
  ComponentName::noId,
  noProps,
  noProps,
  ComponentName::noId,
  ComponentName::noId,
  ComponentName::noId
};

#ifdef GROVE_NAMESPACE
}
#endif
