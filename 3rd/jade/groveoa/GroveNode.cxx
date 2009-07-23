// GroveNode.cxx

#include "stdafx.h"
#include "groveoa.h"
#include "config.h"
#include "GroveNode.h"
#include "Boolean.h"
#include "Node.h"
#include "LocNode.h"
#include "PointerTable.h"
#include "NamedResource.h"
#include "Ptr.h"
#include "Owner.h"
#include "ExtendEntityManager.h"
#include "StorageManager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <new>

#ifdef SP_NO_STD_NAMESPACE
#define std /* as nothing */
#endif

#define TRY try {
#define CATCH  } catch (std::bad_alloc) { return E_OUTOFMEMORY; }

// A NULL BSTR is supposed to be equivalent to an empty BSTR
// but not everybody gets this right (perl build 306, for example)

inline
BSTR makeEmptyBSTR()
{
  return ::SysAllocStringLen(0, 0);
}

using GROVE_NAMESPACE::NodePtr;
using GROVE_NAMESPACE::NodeListPtr;
using GROVE_NAMESPACE::NamedNodeListPtr;
using GROVE_NAMESPACE::GroveString;
using GROVE_NAMESPACE::GroveChar;
using GROVE_NAMESPACE::accessOK;
using GROVE_NAMESPACE::accessNull;
using GROVE_NAMESPACE::accessNotInClass;
using GROVE_NAMESPACE::SdataMapper;

class NodeTable;
typedef SP_NAMESPACE::Ptr<NodeTable> NodeTablePtr;

class CNodeBase {
public:
    virtual ~CNodeBase();
    void init(const NodePtr &nd, const NodeTablePtr &);
    static HRESULT make(const NodePtr &, const NodeTablePtr &, Node **);
    static BSTR makeBSTR(const GroveString &);
    static const GROVE_NAMESPACE::Node &key(const CNodeBase &obj) {
      return *obj.nd_;
    }
    static unsigned long hash(const GROVE_NAMESPACE::Node &nd) {
      return nd.hash();
    }
    virtual Node *asNode() = 0;
    HRESULT internalGetStoragePos(StoragePos **);
protected:
    CNodeBase() { }
    NodePtr nd_;
    NodeTablePtr table_;
};

class NodeTable
: public SP_NAMESPACE::Resource,
  public SP_NAMESPACE::PointerTable<CNodeBase *, GROVE_NAMESPACE::Node, CNodeBase, CNodeBase> {
public:
  NodeTable(SP_NAMESPACE::Owner<ParserThread> &thd) {
    thd.swap(thread_);
  }
private:
  SP_NAMESPACE::Owner<ParserThread> thread_;
};

class ATL_NO_VTABLE CNodeList :
	public CComObjectRoot,
	public IDispatchImpl<NodeList, &IID_NodeList, &LIBID_GROVE> {
public:
    CNodeList() { }
BEGIN_COM_MAP(CNodeList)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(NodeList)
END_COM_MAP()
    STDMETHOD(get_Count)(long *);
    STDMETHOD(get_Item)(long, Node **);
    STDMETHOD(get__NewEnum)(IUnknown **);
    STDMETHOD(First)(Node **);
    STDMETHOD(Rest)(NodeList **);
    static HRESULT make(const NodeListPtr &, const NodeTablePtr &, NodeList **);
private:
    NodeListPtr nl_;
    NodeTablePtr table_;
};

class ATL_NO_VTABLE CChunkNodeList :
        public CComObjectRoot,
	public IDispatchImpl<NodeList, &IID_NodeList, &LIBID_GROVE> {
public:
  CChunkNodeList() { }
BEGIN_COM_MAP(CChunkNodeList)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(NodeList)
END_COM_MAP()
  STDMETHOD(get_Count)(long *);
  STDMETHOD(get_Item)(long, Node **);
  STDMETHOD(get__NewEnum)(IUnknown **);
  STDMETHOD(First)(Node **);
  STDMETHOD(Rest)(NodeList **);
  static HRESULT make(const NodeListPtr &, const NodeTablePtr &, NodeList **);
private:
    NodeListPtr nl_;
    NodeTablePtr table_;
};

class ATL_NO_VTABLE CNamedNodeList :
	public CComObjectRoot,
	public IDispatchImpl<NamedNodeList, &IID_NamedNodeList, &LIBID_GROVE> {
public:
    CNamedNodeList() { }
BEGIN_COM_MAP(CNamedNodeList)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(NamedNodeList)
END_COM_MAP()
    STDMETHOD(get_Count)(long *);
    STDMETHOD(get_Item)(BSTR, Node **);
    STDMETHOD(get__NewEnum)(IUnknown **);
    STDMETHOD(get_NodeList)(NodeList **);
    STDMETHOD(Normalize)(BSTR, BSTR *);
    STDMETHOD(NodeName)(Node *, BSTR *);
    static HRESULT make(const NamedNodeListPtr &, const NodeTablePtr &, NamedNodeList **);
private:
    NamedNodeListPtr nnl_;
    NodeTablePtr table_;
};

class ATL_NO_VTABLE CStoragePos :
  public CComObjectRoot,
  public IDispatchImpl<StoragePos, &IID_StoragePos, &LIBID_GROVE> {
public:
  CStoragePos() { }
  BEGIN_COM_MAP(CStoragePos)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(StoragePos)
  END_COM_MAP()
  STDMETHOD(get_StorageManagerName)(BSTR *retval) {
    const char *s = loc_.storageObjectSpec->storageManager->type();
    BSTR tem = ::SysAllocStringLen(0, strlen(s));
    *retval = tem;
    if (!tem)
      return E_OUTOFMEMORY;
    while (*s)
      *tem++ = *s++;
    return NOERROR;
  }
  STDMETHOD(get_StorageObjectId)(BSTR *retval) {
    const SP_NAMESPACE::StringC &id = loc_.actualStorageId;
    CComBSTR tem(id.size(), id.data());
    *retval = tem.Detach();
    return *retval ? NOERROR : E_OUTOFMEMORY;
  }
  STDMETHOD(get_LineNumber)(long *retval) {
    *retval = loc_.lineNumber;
    if (*retval < 0)
      *retval = 0;
    return NOERROR;
  }
  STDMETHOD(get_ColumnNumber)(long *retval) {
    *retval = loc_.columnNumber;
    if (*retval < 0)
      *retval = 0;
    return NOERROR;
  }
  STDMETHOD(get_ByteIndex)(long *retval) {
    *retval = loc_.byteIndex + 1;
    return NOERROR;
  }
  STDMETHOD(get_CharacterIndex)(long *retval) {
    *retval = loc_.storageObjectOffset + 1;
    return NOERROR;
  }
  static HRESULT make(const SP_NAMESPACE::ExternalInfo *,
		      SP_NAMESPACE::Offset,
		      const NodePtr &,
		      StoragePos **);
private:
  SP_NAMESPACE::StorageObjectLocation loc_;
  NodePtr nd_;
};

#define NODE_PROP2(xfunc, ifunc) \
    STDMETHOD(xfunc)(Node **retval) { \
      TRY \
      *retval = 0; \
      NodePtr nd; \
      if (nd_->ifunc(nd) == accessOK) \
	return make(nd, table_, retval); \
      return NOERROR; \
      CATCH \
    }
#define NODE_PROP(Name) NODE_PROP2(get_##Name, get##Name)

#define DERIVED_NODE_PROP2(xfunc, ifunc, cls) \
    STDMETHOD(xfunc)(cls##Node **retval) { \
      TRY \
      *retval = 0; \
      NodePtr nd; \
      if (nd_->ifunc(nd) == accessOK) { \
        Node *base; \
	HRESULT hres = make(nd, table_, &base); \
	*retval = (cls##Node *)base; \
	return hres; \
      } \
      return NOERROR; \
      CATCH \
    }
#define DERIVED_NODE_PROP(Name, cls) DERIVED_NODE_PROP2(get_##Name, get##Name, cls)


#define NODE_LIST_PROP2(xfunc, ifunc) \
    STDMETHOD(xfunc)(NodeList **retval) { \
      TRY \
      *retval = 0; \
      NodeListPtr nl; \
      if (nd_->ifunc(nl) == accessOK) \
	return CNodeList::make(nl, table_, retval); \
      return NOERROR; \
      CATCH \
    }
#define NODE_LIST_PROP(Name) NODE_LIST_PROP2(get_##Name, get##Name)

#define CHUNK_NODE_LIST_PROP2(xfunc, ifunc) \
    STDMETHOD(xfunc)(NodeList **retval) { \
      TRY \
      *retval = 0; \
      NodeListPtr nl; \
      if (nd_->ifunc(nl) == accessOK) \
	return CChunkNodeList::make(nl, table_, retval); \
      return NOERROR; \
      CATCH \
    }

#define NAMED_NODE_LIST_PROP2(xfunc, ifunc) \
    STDMETHOD(xfunc)(NamedNodeList **retval) { \
      TRY \
      *retval = 0; \
      NamedNodeListPtr nnl; \
      if (nd_->ifunc(nnl) == accessOK) \
	return CNamedNodeList::make(nnl, table_, retval); \
      return NOERROR; \
      CATCH \
    }
#define NAMED_NODE_LIST_PROP(Name) \
 NAMED_NODE_LIST_PROP2(get_##Name, get##Name)

#define BOOL_PROP2(xfunc, ifunc) \
    STDMETHOD(xfunc)(VARIANT_BOOL *retval) { \
      TRY \
      bool b; \
      if (nd_->ifunc(b) == accessOK) \
        *retval = b ? -1 : 0; \
      else \
	*retval = 0; \
      return NOERROR; \
      CATCH \
    }
#define BOOL_PROP(Name) BOOL_PROP2(get_##Name, get##Name)

#define STRING_PROP2(xfunc, ifunc) \
    STDMETHOD(xfunc)(BSTR *retval) { \
      TRY \
      *retval = 0; \
      GroveString str; \
      if (nd_->ifunc(str) == accessOK) \
	*retval = makeBSTR(str); \
      else \
        *retval = makeEmptyBSTR(); \
      return NOERROR; \
      CATCH \
    }
#define STRING_PROP(Name) STRING_PROP2(get_##Name, get##Name)

#define NULL_STRING_PROP(Name) \
    STRING_PROP(Name) \
    STDMETHOD(get_##Name##Null)(VARIANT_BOOL *retval) { \
      TRY \
      *retval = 0; \
      GroveString str; \
      if (nd_->get##Name(str) == accessNull) \
	*retval = -1; \
      return NOERROR; \
      CATCH \
    }


template <class T, const IID *piid, const GUID *plibid, NodeClass nodeClass>
class INodeImpl : public IDispatchImpl<T, piid, plibid>, public CNodeBase {
public:
    NODE_PROP(Parent)
    NODE_PROP(Origin)
    NODE_PROP(GroveRoot)
    NODE_PROP(TreeRoot)
    NODE_LIST_PROP2(get_Children, children)
    CHUNK_NODE_LIST_PROP2(get_ChunkChildren, children)
    NODE_PROP2(FirstChild, firstChild)
    NODE_PROP2(NextSibling, nextSibling)
    NODE_PROP2(NextChunkSibling, nextChunkSibling)
    STDMETHOD(get_Class)(NodeClass *retval) {
      TRY
      *retval = nodeClass;
      return NOERROR;
      CATCH
    }
    STDMETHOD(get_SiblingIndex)(long *retval) {
      TRY
      unsigned long n;
      if (nd_->siblingsIndex(n) == accessOK)
	*retval = n + 1;
      else
	*retval = 0;
      return NOERROR;
      CATCH
    }
    Node *asNode() { return this; }
    STDMETHOD(get_StoragePos)(StoragePos **retval) {
      return internalGetStoragePos(retval);
    }
};

class ATL_NO_VTABLE CElementNode :
	public CComObjectRoot,
	public INodeImpl<ElementNode, &IID_ElementNode, &LIBID_GROVE, nodeClassElement> {
public:
    CElementNode() { }
BEGIN_COM_MAP(CElementNode)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(ElementNode)
   COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
   STRING_PROP(Gi)
   NAMED_NODE_LIST_PROP(Attributes)
   STRING_PROP(Id)
   NODE_LIST_PROP(Content)
   CHUNK_NODE_LIST_PROP2(get_ChunkContent, getContent)
   BOOL_PROP(Included)
   BOOL_PROP(MustOmitEndTag) 
};

class ATL_NO_VTABLE CAttributeAssignmentNode :
	public CComObjectRoot,
	public INodeImpl<AttributeAssignmentNode, &IID_AttributeAssignmentNode, &LIBID_GROVE, nodeClassAttributeAssignment> {
public:
    CAttributeAssignmentNode() { }
BEGIN_COM_MAP(CAttributeAssignmentNode)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(AttributeAssignmentNode)
   COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
   STRING_PROP(Name)
   BOOL_PROP(Implied)
   NODE_LIST_PROP(Value)
   CHUNK_NODE_LIST_PROP2(get_ChunkValue, getValue)
   STDMETHOD(get_TokenSep)(long *retval) {
     TRY
     GroveChar ch;
     if (nd_->getTokenSep(ch) == accessOK)
       *retval = ch;
     else
       *retval = -1;
     return NOERROR;
     CATCH
   }
   STDMETHOD(get_StringValue)(BSTR *retval) {
     TRY
     *retval = 0;
     bool implied;
     if (nd_->getImplied(implied) == accessOK && implied) {
       *retval = makeEmptyBSTR();
       return NOERROR;
     }
     GroveString tokens;
     if (nd_->tokens(tokens) == accessOK) {
       *retval = makeBSTR(tokens);
       return NOERROR;
     }
     NodePtr first;
     if (nd_->firstChild(first) == accessOK) {
       size_t len = 0;
       NodePtr tem(first);
       do {
	 GroveString chunk;
	 if (tem->charChunk(SdataMapper(), chunk) == accessOK)
	   len += chunk.size();
       } while (tem.assignNextChunkSibling() == accessOK);
       GroveChar *p = ::SysAllocStringLen(0, len);
       if (!p)
	 return E_OUTOFMEMORY;
       *retval = p;
       tem = first;
       do {
	 GroveString chunk;
	 if (tem->charChunk(SdataMapper(), chunk) == accessOK) {
	   memcpy(p, chunk.data(), chunk.size()*sizeof(*p));
	   p += chunk.size();
	 } 
       } while (tem.assignNextChunkSibling() == accessOK);
     }
     return NOERROR;
     CATCH
   }
};

class ATL_NO_VTABLE CAttributeValueTokenNode :
	public CComObjectRoot,
	public INodeImpl<AttributeValueTokenNode, &IID_AttributeValueTokenNode, &LIBID_GROVE, nodeClassAttributeValueToken> {
public:
    CAttributeValueTokenNode() { }
BEGIN_COM_MAP(CAttributeValueTokenNode)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(AttributeValueTokenNode)
   COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
   STRING_PROP(Token)
   DERIVED_NODE_PROP(Referent, Element)
   DERIVED_NODE_PROP(Entity, Entity)
   DERIVED_NODE_PROP(Notation, Notation)
};

class ATL_NO_VTABLE CSgmlDocumentNode :
	public CComObjectRoot,
	public INodeImpl<SgmlDocumentNode, &IID_SgmlDocumentNode, &LIBID_GROVE, nodeClassSgmlDocument> {
public:
    CSgmlDocumentNode() { }
BEGIN_COM_MAP(CSgmlDocumentNode)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(SgmlDocumentNode)
    COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
    DERIVED_NODE_PROP(DocumentElement, Element)
    DERIVED_NODE_PROP(SgmlConstants, SgmlConstants)
    NULL_STRING_PROP(ApplicationInfo)
    NODE_LIST_PROP(Prolog)
    NODE_LIST_PROP(Epilog)
    NAMED_NODE_LIST_PROP(Elements)
    NAMED_NODE_LIST_PROP(Entities)
    NAMED_NODE_LIST_PROP(DefaultedEntities)
    NAMED_NODE_LIST_PROP(DoctypesAndLinktypes)
    DERIVED_NODE_PROP(GoverningDoctype, DocumentType)
    NODE_LIST_PROP(Messages)
};

class ATL_NO_VTABLE CDataCharNode :
	public CComObjectRoot,
	public INodeImpl<DataCharNode, &IID_DataCharNode, &LIBID_GROVE, nodeClassDataChar> {
public:
    CDataCharNode() { }
BEGIN_COM_MAP(CDataCharNode)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(DataCharNode)
   COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
    STDMETHOD(get_Char)(long *);
    STDMETHOD(get_NonSgml)(long *);
    STDMETHOD(get_CharChunk)(BSTR *);
};

class ATL_NO_VTABLE CSgmlConstantsNode :
    public CComObjectRoot,
    public INodeImpl<SgmlConstantsNode, &IID_SgmlConstantsNode, &LIBID_GROVE, nodeClassSgmlConstants> {
public:
      CSgmlConstantsNode() { }
BEGIN_COM_MAP(CSgmlConstantsNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(SgmlConstantsNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
};

class ATL_NO_VTABLE CPiNode :
    public CComObjectRoot,
    public INodeImpl<PiNode, &IID_PiNode, &LIBID_GROVE, nodeClassPi> {
public:
      CPiNode() { }
BEGIN_COM_MAP(CPiNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(PiNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  STRING_PROP(SystemData)
  DERIVED_NODE_PROP(Entity, Entity)
  STRING_PROP(EntityName)
};

class ATL_NO_VTABLE CSdataNode :
    public CComObjectRoot,
    public INodeImpl<SdataNode, &IID_SdataNode, &LIBID_GROVE, nodeClassSdata> {
public:
      CSdataNode() { }
BEGIN_COM_MAP(CSdataNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(SdataNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  STRING_PROP(SystemData)
  DERIVED_NODE_PROP(Entity, Entity)
  STRING_PROP(EntityName)
};

class ATL_NO_VTABLE CDocumentTypeNode :
    public CComObjectRoot,
    public INodeImpl<DocumentTypeNode, &IID_DocumentTypeNode, &LIBID_GROVE, nodeClassDocumentType> {
public:
      CDocumentTypeNode() { }
BEGIN_COM_MAP(CDocumentTypeNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(DocumentTypeNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  STRING_PROP(Name)
  BOOL_PROP(Governing)
  NAMED_NODE_LIST_PROP(GeneralEntities)
  NAMED_NODE_LIST_PROP(Notations)
};

class ATL_NO_VTABLE CEntityNode :
    public CComObjectRoot,
    public INodeImpl<EntityNode, &IID_EntityNode, &LIBID_GROVE, nodeClassEntity> {
public:
      CEntityNode() { }
BEGIN_COM_MAP(CEntityNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(EntityNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  STRING_PROP(Name)
  NULL_STRING_PROP(Text)
  DERIVED_NODE_PROP(Notation, Notation)
  STRING_PROP(NotationName)
  BOOL_PROP(Defaulted)
  NAMED_NODE_LIST_PROP(Attributes)
  DERIVED_NODE_PROP(ExternalId, ExternalId)
  STDMETHOD(get_EntityType)(EntityType *retval) {
    GROVE_NAMESPACE::Node::EntityType type;
    if (nd_->getEntityType(type) == accessOK)
      *retval = EntityType(type);
    return NOERROR;
  }
};

class ATL_NO_VTABLE CNotationNode :
    public CComObjectRoot,
    public INodeImpl<NotationNode, &IID_NotationNode, &LIBID_GROVE, nodeClassNotation> {
public:
      CNotationNode() { }
BEGIN_COM_MAP(CNotationNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(NotationNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  STRING_PROP(Name)
  DERIVED_NODE_PROP(ExternalId, ExternalId)
};

class ATL_NO_VTABLE CExternalIdNode :
    public CComObjectRoot,
    public INodeImpl<ExternalIdNode, &IID_ExternalIdNode, &LIBID_GROVE, nodeClassExternalId> {
public:
      CExternalIdNode() { }
BEGIN_COM_MAP(CExternalIdNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(ExternalIdNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  NULL_STRING_PROP(PublicId)
  NULL_STRING_PROP(SystemId)
  STRING_PROP(GeneratedSystemId)
};

class ATL_NO_VTABLE CExternalDataNode :
    public CComObjectRoot,
    public INodeImpl<ExternalDataNode, &IID_ExternalDataNode, &LIBID_GROVE, nodeClassExternalData> {
public:
      CExternalDataNode() { }
BEGIN_COM_MAP(CExternalDataNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(ExternalDataNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  DERIVED_NODE_PROP(Entity, Entity)
  STRING_PROP(EntityName)
};

class ATL_NO_VTABLE CSubdocNode :
    public CComObjectRoot,
    public INodeImpl<SubdocNode, &IID_SubdocNode, &LIBID_GROVE, nodeClassSubdoc> {
public:
      CSubdocNode() { }
BEGIN_COM_MAP(CSubdocNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(SubdocNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  DERIVED_NODE_PROP(Entity, Entity)
  STRING_PROP(EntityName)
};

class ATL_NO_VTABLE CMessageNode :
    public CComObjectRoot,
    public INodeImpl<MessageNode, &IID_MessageNode, &LIBID_GROVE, nodeClassMessage> {
public:
  CMessageNode() { }
BEGIN_COM_MAP(CMessageNode)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(MessageNode)
  COM_INTERFACE_ENTRY(Node)
END_COM_MAP()
  STRING_PROP(Text)
  STDMETHOD(get_Severity)(Severity *retval) {
    TRY
    GROVE_NAMESPACE::Node::Severity type;
    if (nd_->getSeverity(type) == accessOK)
      *retval = Severity(type);
    return NOERROR;
    CATCH
  }
};

class ATL_NO_VTABLE CEnumNodeList :
	public CComObjectRoot,
	public IEnumVARIANT {
public:
    CEnumNodeList() { }
BEGIN_COM_MAP(CEnumNodeList)
   COM_INTERFACE_ENTRY(IEnumVARIANT)
END_COM_MAP()
  // IEnumVARIANT
  STDMETHOD(Next)(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched);
  STDMETHOD(Skip)(ULONG celt);
  STDMETHOD(Reset)();
  STDMETHOD(Clone)(IEnumVARIANT **ppEnum);
  static HRESULT make(const NodeListPtr &, const NodeTablePtr &table, IUnknown **);
private:
  NodeListPtr nl_;
  NodeListPtr cur_;
  NodeTablePtr table_;
};

class ATL_NO_VTABLE CEnumChunkNodeList :
	public CComObjectRoot,
	public IEnumVARIANT {
public:
    CEnumChunkNodeList() { }
BEGIN_COM_MAP(CEnumChunkNodeList)
   COM_INTERFACE_ENTRY(IEnumVARIANT)
END_COM_MAP()
  // IEnumVARIANT
  STDMETHOD(Next)(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched);
  STDMETHOD(Skip)(ULONG celt);
  STDMETHOD(Reset)();
  STDMETHOD(Clone)(IEnumVARIANT **ppEnum);
  static HRESULT make(const NodeListPtr &, const NodeTablePtr &table, IUnknown **);
private:
  NodeListPtr nl_;
  NodeListPtr cur_;
  NodeTablePtr table_;
};

struct BuildVisitor :  public GROVE_NAMESPACE::NodeVisitor {
public:
  BuildVisitor(const NodePtr &n, const NodeTablePtr &t)
    : nd(n), table(t), retval(0), hRes(NOERROR) { }
#define BUILD(mem, Cls) \
  void mem(GROVE_NAMESPACE::Node &) { \
    CComObject<Cls> *p; \
    hRes = CComObject<Cls>::CreateInstance(&p); \
    if (!FAILED(hRes)) { \
      p->init(nd, table); \
      retval = p; \
      (retval)->AddRef(); \
    } \
  }
  BUILD(element, CElementNode)
  BUILD(attributeAssignment, CAttributeAssignmentNode)
  BUILD(attributeValueToken, CAttributeValueTokenNode)
  BUILD(sgmlDocument, CSgmlDocumentNode)
  BUILD(dataChar, CDataCharNode)
  BUILD(sgmlConstants, CSgmlConstantsNode)
  BUILD(pi, CPiNode)
  BUILD(sdata, CSdataNode)
  BUILD(documentType, CDocumentTypeNode)
  BUILD(entity, CEntityNode)
  BUILD(notation, CNotationNode)
  BUILD(externalId, CExternalIdNode)
  BUILD(externalData, CExternalDataNode)
  BUILD(subdocument, CSubdocNode)
  BUILD(message, CMessageNode)
#undef BUILD
  NodePtr nd;
  HRESULT hRes;
  Node *retval;
  NodeTablePtr table;
};

HRESULT CNodeBase::make(const NodePtr &nd, const NodeTablePtr &table, Node **retval)
{
  CNodeBase *nb = table->lookup(*nd);
  if (nb) {
    *retval = nb->asNode();
    (*retval)->AddRef();
    return NOERROR;
  }
  else {
    BuildVisitor visitor(nd, table);
    nd->accept(visitor);
    *retval = visitor.retval;
    return visitor.hRes;
  }
}

CNodeBase::~CNodeBase()
{
  CNodeBase *tem = table_->remove(*nd_);
  if (tem != this)
    abort();
}

void CNodeBase::init(const NodePtr &nd, const NodeTablePtr &table)
{
  table_ = table;
  nd_ = nd;
  table_->insert(this);
}

BSTR CNodeBase::makeBSTR(const GroveString &str)
{
  CComBSTR bstr(str.size(), str.data());
  return bstr.Detach();
}

HRESULT CNodeBase::internalGetStoragePos(StoragePos **retval)
{
  TRY
  const SP_NAMESPACE::LocNode *lp = SP_NAMESPACE::LocNode::convert(nd_);
  if (lp) {
    SP_NAMESPACE::Location loc;
    if (lp->getLocation(loc) == accessOK) {
      const SP_NAMESPACE::Origin *origin = loc.origin().pointer();
      SP_NAMESPACE::Index index = loc.index();
      while (origin) {
	const SP_NAMESPACE::ExternalInfo *info
	  = origin->externalInfo();
	if (info)
	  return CStoragePos::make(info, origin->startOffset(index), nd_, retval);
	const SP_NAMESPACE::Location &loc = origin->parent();
	index = loc.index();
	origin = loc.origin().pointer();
      }
    }
  }
  *retval = 0;
  return NOERROR;
  CATCH
}

STDMETHODIMP CDataCharNode::get_CharChunk(BSTR *retval)
{
  TRY
  *retval = 0;
  GroveString str;
  if (nd_->charChunk(SdataMapper(), str) == accessOK)
    *retval = makeBSTR(str);
  else
    *retval = makeEmptyBSTR();
  return NOERROR;
  CATCH
}

STDMETHODIMP CDataCharNode::get_Char(long *retval)
{
  TRY
  GroveChar ch;
  if (nd_->getChar(SdataMapper(), ch) == accessOK)
    *retval = ch;
  else
    *retval = -1;
  return NOERROR;
  CATCH
}

STDMETHODIMP CDataCharNode::get_NonSgml(long *retval)
{
  TRY
  unsigned long n;
  if (nd_->getNonSgml(n) == accessOK)
    *retval = n;
  else
    *retval = -1;
  return NOERROR;
  CATCH
}

HRESULT CNodeList::make(const NodeListPtr &nl, const NodeTablePtr &table, NodeList **retval)
{
  CComObject<CNodeList> *p;
  HRESULT hRes = CComObject<CNodeList>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CNodeList *np = p;
  np->nl_ = nl;
  np->table_ = table;
  *retval = p;
  (*retval)->AddRef();
  return NOERROR;
}

STDMETHODIMP CNodeList::First(Node **retval)
{
  TRY
  *retval = 0;
  NodePtr nd;
  if (nl_->first(nd) == accessOK)
    return CNodeBase::make(nd, table_, retval);
  return NOERROR;
  CATCH
}

STDMETHODIMP CNodeList::Rest(NodeList **retval)
{
  TRY
  *retval = 0;
  NodeListPtr nl;
  if (nl_->rest(nl) == accessOK)
    return CNodeList::make(nl, table_, retval);
  return NOERROR;
  CATCH
}

STDMETHODIMP CNodeList::get_Count(long *retval)
{
  TRY
  long n = 0;
  NodeListPtr nl;
  if (nl_->rest(nl) == accessOK) {
    n++;
    while (nl.assignRest() == accessOK)
      n++;
  }
  *retval = n;
  return NOERROR;
  CATCH
}

STDMETHODIMP CNodeList::get_Item(long i, Node **retval)
{
  TRY
  *retval = 0;
  NodePtr nd;
  if (i <= 0)
    return E_INVALIDARG;
  if (nl_->ref(i - 1, nd) != accessOK)
    return E_INVALIDARG;
  return CNodeBase::make(nd, table_, retval);
  CATCH
}

STDMETHODIMP CNodeList::get__NewEnum(IUnknown **retval)
{
  TRY
  return CEnumNodeList::make(nl_, table_, retval);
  CATCH
}

HRESULT CChunkNodeList::make(const NodeListPtr &nl, const NodeTablePtr &table, NodeList **retval)
{
  CComObject<CChunkNodeList> *p;
  HRESULT hRes = CComObject<CChunkNodeList>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CChunkNodeList *np = p;
  np->nl_ = nl;
  np->table_ = table;
  *retval = p;
  (*retval)->AddRef();
  return NOERROR;
}

STDMETHODIMP CChunkNodeList::First(Node **retval)
{
  TRY
  *retval = 0;
  NodePtr nd;
  if (nl_->first(nd) == accessOK)
    return CNodeBase::make(nd, table_, retval);
  return NOERROR;
  CATCH
}

STDMETHODIMP CChunkNodeList::Rest(NodeList **retval)
{
  TRY
  *retval = 0;
  NodeListPtr nl;
  if (nl_->chunkRest(nl) == accessOK)
    return CNodeList::make(nl, table_, retval);
  return NOERROR;
  CATCH
}

STDMETHODIMP CChunkNodeList::get__NewEnum(IUnknown **retval)
{
  TRY
  return CEnumChunkNodeList::make(nl_, table_, retval);
  CATCH
}

STDMETHODIMP CChunkNodeList::get_Count(long *retval)
{
  TRY
  long n = 0;
  NodeListPtr nl;
  if (nl_->chunkRest(nl) == accessOK) {
    n++;
    while (nl.assignChunkRest() == accessOK)
      n++;
  }
  *retval = n;
  return NOERROR;
  CATCH
}

STDMETHODIMP CChunkNodeList::get_Item(long i, Node **retval)
{
  TRY
  *retval = 0;
  if (i <= 0)
    return E_INVALIDARG;
  if (i == 1)
    return First(retval);
  NodeListPtr nl(nl_);
  do {
    if (nl.assignChunkRest() != accessOK)
      return E_INVALIDARG;
  } while (--i > 1);
  NodePtr nd;
  if (nl->first(nd) != accessOK)
    return E_INVALIDARG;
  return CNodeBase::make(nd, table_, retval);
  CATCH
}

STDMETHODIMP CNamedNodeList::get_Count(long *retval)
{
  TRY
  NodeListPtr nl(nnl_->nodeListNoOrder());
  long n = 0;
  while (nl.assignRest() == accessOK)
    n++;
  *retval = n;
  return NOERROR;
  CATCH
}

STDMETHODIMP CNamedNodeList::get_Item(BSTR name, Node **retval)
{
  TRY
  *retval = 0;
  if (!name)
    return E_INVALIDARG;
  GroveString tem(name, ::SysStringLen(name));
  NodePtr nd;
  if (nnl_->namedNode(tem, nd) != accessOK)
    return E_INVALIDARG;
  return CNodeBase::make(nd, table_, retval);
  CATCH
}

STDMETHODIMP CNamedNodeList::get_NodeList(NodeList **retval)
{
  TRY
  return CNodeList::make(nnl_->nodeList(), table_, retval);
  CATCH
}

STDMETHODIMP CNamedNodeList::get__NewEnum(IUnknown **retval)
{
  TRY
  return CEnumNodeList::make(nnl_->nodeListNoOrder(), table_, retval);
  CATCH
}

STDMETHODIMP CNamedNodeList::Normalize(BSTR name, BSTR *retval)
{
  TRY
  *retval = 0;
  if (name) {
    size_t len = ::SysStringLen(name);
    BSTR tem = ::SysAllocStringLen(name, len);
    size_t newLen = nnl_->normalize(tem, len);
    if (newLen != len && !::SysReAllocStringLen(&tem, tem, newLen)) {
      ::SysFreeString(tem);
      return E_OUTOFMEMORY;
    }
    *retval = tem;
  }
  else
    *retval = makeEmptyBSTR();
  return NOERROR;
  CATCH
}

STDMETHODIMP CNamedNodeList::NodeName(Node *nd, BSTR *retval)
{
  TRY
  *retval = 0;
  switch (nnl_->type()) {
#define CASE(T, I, P) \
  case GROVE_NAMESPACE::NamedNodeList::T: \
    { \
      I *tem; \
      if (SUCCEEDED(nd->QueryInterface(IID_##I, (void **)&tem))) { \
	HRESULT hRes = tem->get_##P(retval); \
	tem->Release(); \
	return hRes; \
      } \
      return NOERROR; \
    }
    CASE(elements,ElementNode,Id)
    CASE(entities,EntityNode,Name)
    CASE(notations,NotationNode,Name)
    CASE(attributes,AttributeAssignmentNode,Name)
    CASE(doctypesAndLinktypes,DocumentTypeNode,Name)
  }
  return NOERROR;
  CATCH
}

HRESULT CNamedNodeList::make(const NamedNodeListPtr &nnl,
			     const NodeTablePtr &table,
			     NamedNodeList **retval)
{
  CComObject<CNamedNodeList> *p;
  HRESULT hRes = CComObject<CNamedNodeList>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CNamedNodeList *np = p;
  np->nnl_ = nnl;
  np->table_ = table;
  *retval = p;
  (*retval)->AddRef();
  return NOERROR;
}

HRESULT CEnumNodeList::make(const NodeListPtr &nl, const NodeTablePtr &table,
			    IUnknown **retval)
{
  CComObject<CEnumNodeList> *p;
  HRESULT hRes = CComObject<CEnumNodeList>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CEnumNodeList *np = p;
  np->nl_ = nl;
  np->cur_ = nl;
  np->table_ = table;
  return p->QueryInterface(IID_IUnknown, (void **)retval);
}

STDMETHODIMP CEnumNodeList::Next(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched)
{
  TRY
  ULONG n = 0;
  for (; celt > 0; celt--, rgVar++, n++) {
    NodePtr nd;
    if (cur_->first(nd) != accessOK)
      break;
    if (cur_.assignRest() != accessOK)
      goto error;
    Node *ip;
    if (FAILED(CNodeBase::make(nd, table_, &ip)))
      goto error;
    rgVar->vt = VT_DISPATCH;
    rgVar->pdispVal = ip;
  }
  if (pCeltFetched)
    *pCeltFetched = n;
  return celt ? S_FALSE : NOERROR;
error:
  while (n > 0) {
    --n ;
    --rgVar;
    VariantClear(rgVar);
  }
  if (pCeltFetched)
    *pCeltFetched = 0;
  return S_FALSE;
  CATCH
}

STDMETHODIMP CEnumNodeList::Skip(ULONG celt)
{
  TRY
  for (; celt > 0; celt--) {
    if (cur_.assignRest() != accessOK)
      return S_FALSE;
  }
  return NOERROR;
  CATCH
}

STDMETHODIMP CEnumNodeList::Reset()
{
  TRY
  cur_ = nl_;
  return NOERROR;
  CATCH
}

STDMETHODIMP CEnumNodeList::Clone(IEnumVARIANT **ppEnum)
{
  TRY
  CComObject<CEnumNodeList> *p;
  HRESULT hRes = CComObject<CEnumNodeList>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CEnumNodeList *np = p;
  np->nl_ = nl_;
  np->cur_ = cur_;
  np->table_ = table_;
  *ppEnum = p;
  (*ppEnum)->AddRef();
  return NOERROR;
  CATCH
}

HRESULT CEnumChunkNodeList::make(const NodeListPtr &nl, const NodeTablePtr &table,
			    IUnknown **retval)
{
  CComObject<CEnumChunkNodeList> *p;
  HRESULT hRes = CComObject<CEnumChunkNodeList>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CEnumChunkNodeList *np = p;
  np->nl_ = nl;
  np->cur_ = nl;
  np->table_ = table;
  return p->QueryInterface(IID_IUnknown, (void **)retval);
}

STDMETHODIMP CEnumChunkNodeList::Next(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched)
{
  TRY
  ULONG n = 0;
  for (; celt > 0; celt--, rgVar++, n++) {
    NodePtr nd;
    if (cur_->first(nd) != accessOK)
      break;
    if (cur_.assignChunkRest() != accessOK)
      goto error;
    Node *ip;
    if (FAILED(CNodeBase::make(nd, table_, &ip)))
      goto error;
    rgVar->vt = VT_DISPATCH;
    rgVar->pdispVal = ip;
  }
  if (pCeltFetched)
    *pCeltFetched = n;
  return celt ? S_FALSE : NOERROR;
error:
  while (n > 0) {
    --n ;
    --rgVar;
    VariantClear(rgVar);
  }
  if (pCeltFetched)
    *pCeltFetched = 0;
  return S_FALSE;
  CATCH
}

STDMETHODIMP CEnumChunkNodeList::Skip(ULONG celt)
{
  TRY
  for (; celt > 0; celt--) {
    if (cur_.assignChunkRest() != accessOK)
      return S_FALSE;
  }
  return NOERROR;
  CATCH
}

STDMETHODIMP CEnumChunkNodeList::Reset()
{
  TRY
  cur_ = nl_;
  return NOERROR;
  CATCH
}

STDMETHODIMP CEnumChunkNodeList::Clone(IEnumVARIANT **ppEnum)
{
  TRY
  CComObject<CEnumChunkNodeList> *p;
  HRESULT hRes = CComObject<CEnumChunkNodeList>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CEnumChunkNodeList *np = p;
  np->nl_ = nl_;
  np->cur_ = cur_;
  np->table_ = table_;
  *ppEnum = p;
  (*ppEnum)->AddRef();
  return NOERROR;
  CATCH
}

HRESULT CStoragePos::make(const SP_NAMESPACE::ExternalInfo *info,
			  SP_NAMESPACE::Offset offset,
			  const NodePtr &nd,
			  StoragePos **retval)
{
  *retval = 0;
  CComObject<CStoragePos> *p;
  HRESULT hRes = CComObject<CStoragePos>::CreateInstance(&p);
  if (FAILED(hRes))
    return hRes;
  CStoragePos *tem = p;
  if (!SP_NAMESPACE::ExtendEntityManager::externalize(info, offset, tem->loc_)) {
    delete p;
    return NOERROR;
  }
  tem->nd_ = nd;
  *retval = p;
  (*retval)->AddRef();
  return NOERROR;
}

HRESULT makeRootNode(const GROVE_NAMESPACE::NodePtr &root,
		     SP_NAMESPACE::Owner<ParserThread> &parser,
		     SgmlDocumentNode **retval)
{
  TRY
  NodeTablePtr table(new NodeTable(parser));
  Node *tem;
  HRESULT hRes = CNodeBase::make(root, table, &tem);
  *retval = (SgmlDocumentNode *)tem;
  return hRes;
  CATCH
}
