// Copyright (c) 1996, 1997 James Clark
// See the file COPYING for copying permission.

// FIXME location for SgmlDocument node.

#include "config.h"
#include "Boolean.h"
#include "Node.h"
#include "Resource.h"
#include "Ptr.h"
#include "xnew.h"
#include "Event.h"
#include "GroveBuilder.h"
#include "ErrorCountEventHandler.h"
#include "OutputCharStream.h"
#include "MessageFormatter.h"
#include "Dtd.h"
#include "Syntax.h"
#include "Attribute.h"
#include "Vector.h"
#include "LocNode.h"
#include "SdNode.h"
#include "threads.h"
#include "macros.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4250 ) // inherits via dominance
#endif

#include <stddef.h>
#include <string.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef GROVE_NAMESPACE
using namespace GROVE_NAMESPACE;
#endif

static bool blockingAccess = 1;

size_t initialBlockSize = 8192;
unsigned maxBlocksPerSize = 20;

struct Chunk;
struct ParentChunk;
class ElementChunk;
struct SgmlDocumentChunk;
class DataChunk;
class GroveImpl;
class BaseNode;
class ChunkNode;
class ElementNode;
class DataNode;
class CdataAttributeValueNode;
class AttributeValueTokenNode;
class AttributeAsgnNode;
class EntityNode;
class NotationNode;
class ExternalIdNode;
class DocumentTypeNode;
class SgmlConstantsNode;
class MessageNode;

struct Chunk {
  // second arg never null
  // Set ptr to a node pointing to first Node in this.
  virtual AccessResult setNodePtrFirst(NodePtr &ptr,
				       const BaseNode *) const = 0;
  virtual AccessResult setNodePtrFirst(NodePtr &ptr,
				       const ElementNode *node) const;
  virtual AccessResult setNodePtrFirst(NodePtr &ptr,
				       const DataNode *node) const;
  virtual const Chunk *after() const = 0;
  virtual AccessResult getFollowing(const GroveImpl *,
                                    const Chunk *&, unsigned long &nNodes)
    const;
  virtual AccessResult getFirstSibling(const GroveImpl *, const Chunk *&) const;
  virtual const StringC *id() const;
  virtual Boolean getLocOrigin(const Origin *&) const;
  ParentChunk *origin;
};

struct LocChunk : public Chunk {
  Index locIndex;
};

struct ParentChunk : public LocChunk {
  ParentChunk() : nextSibling(0) { }
  Chunk *nextSibling;
};

class ElementChunk : public ParentChunk {
public:
  virtual const AttributeValue *
    attributeValue(size_t attIndex, const GroveImpl &grove) const;
  virtual Boolean mustOmitEndTag() const;
  virtual Boolean included() const;
  const AttributeDefinitionList *attDefList() const;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
  AccessResult setNodePtrFirst(NodePtr &ptr, const DataNode *node) const;
  AccessResult setNodePtrFirst(NodePtr &ptr, const ElementNode *node) const;
  static const StringC &key(const ElementChunk &chunk) { return *chunk.id(); }
  const Chunk *after() const;
  AccessResult getFollowing(const GroveImpl *, const Chunk *&, unsigned long &nNodes)
    const;
private:
  friend class ElementNode;
  const ElementType *type;
public:
  unsigned long elementIndex;
};

inline
const AttributeDefinitionList *ElementChunk::attDefList() const
{
  return type->attributeDefTemp();
}

class LocOriginChunk : public Chunk {
public:
  LocOriginChunk(const Origin *lo) : locOrigin(lo) { }
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *) const;
  AccessResult setNodePtrFirst(NodePtr &ptr, const ElementNode *node) const;
  AccessResult setNodePtrFirst(NodePtr &ptr, const DataNode *node) const;
  const Chunk *after() const;
  AccessResult getFollowing(const GroveImpl *,
                            const Chunk *&, unsigned long &nNodes)
    const;
  Boolean getLocOrigin(const Origin *&) const;
private:
  const Origin *locOrigin;
};

class MessageItem {
public:
  MessageItem(Node::Severity severity, const StringC &text, const Location &loc)
    : severity_(severity), text_(text), loc_(loc), next_(0) { }
  Node::Severity severity() const { return severity_; }
  const Location &loc() const { return loc_; }
  const StringC &text() const { return text_; }
  const MessageItem *next() const { return next_; }
  MessageItem **nextP() { return &next_; }
private:
  Node::Severity severity_;
  StringC text_;
  Location loc_;
  MessageItem *next_;
};

// multiple threads using const interface.

class GroveImpl {
public:
  GroveImpl(unsigned groveIndex);

  // Const interface
  void addRef() const { ++(((GroveImpl *)this)->refCount_); }
  void release() const {
    if (!--(((GroveImpl *)this)->refCount_))
      delete (GroveImpl *)this;
  }
  unsigned groveIndex() const { return groveIndex_; }
  const SgmlDocumentChunk *root() const { return root_; }
  const AttributeValue *impliedAttributeValue() const {
    return impliedAttributeValue_.pointer();
  }
  // Return 0 if not yet available.
  Boolean getAppinfo(const StringC *&) const;
  const SubstTable<Char> *generalSubstTable() const {
    return instanceSyntax_.isNull() ? 0 : instanceSyntax_->generalSubstTable();
  }
  const SubstTable<Char> *entitySubstTable() const {
    return instanceSyntax_.isNull() ? 0 : instanceSyntax_->entitySubstTable();
  }
  // Be careful not to change ref counts while accessing DTD.
  const Dtd *governingDtd() const { return dtd_.pointer(); }
  // must not be called till grove is complete
  Dtd::ConstEntityIter defaultedEntityIter() const;
  const Entity *lookupDefaultedEntity(const StringC &) const;
  const ElementChunk *lookupElement(const StringC &) const;
  typedef PointerTableIter<ElementChunk *,StringC,Hash,ElementChunk> ElementIter;
  // must not be called till grove is complete
  ElementIter elementIter() const;
  Boolean complete() const { return complete_; }
  const void *completeLimit() const { return completeLimit_; }
  const void *completeLimitWithLocChunkAfter() const {
    return completeLimitWithLocChunkAfter_;
  }
  const Origin *currentLocOrigin() const { return currentLocOrigin_; }
  Boolean hasDefaultEntity() const { return hasDefaultEntity_; }
  Boolean maybeMoreSiblings(const ParentChunk *chunk) const;
  // return zero for timeout
  Boolean waitForMoreNodes() const;
  AccessResult proxifyLocation(const Location &, Location &) const;
  const MessageItem *messageList() const { return messageList_; }
  // must not be called till grove is complete
  void getSd(ConstPtr<Sd> &, ConstPtr<Syntax> &, ConstPtr<Syntax> &) const;
  // non-const interface
  void *allocChunk(size_t);
  void appendSibling(Chunk *);
  void appendSibling(DataChunk *);
  Boolean tryExtend(size_t n) {
    if (n <= nFree_) {
      nFree_ -= n;
      freePtr_ += n;
      return 1;
    }
    else
      return 0;
  }
  DataChunk *pendingData() { return pendingData_; }
  void push(ElementChunk *, Boolean hasId);
  void pop();
  void setAppinfo(const StringC &);
  void setDtd(const ConstPtr<Dtd> &dtd);
  void setSd(const ConstPtr<Sd> &, const ConstPtr<Syntax> &, const ConstPtr<Syntax> &);
  void storeAttributeValue(const ConstPtr<AttributeValue> &value) {
    values_.push_back(value);
  }
  void addDefaultedEntity(const ConstPtr<Entity> &);
  void setComplete();
  Boolean haveRootOrigin();
  void setLocOrigin(const ConstPtr<Origin> &);
  void appendMessage(MessageItem *);
private:
  GroveImpl(const GroveImpl &);
  void operator=(const GroveImpl &);
  ~GroveImpl();

  Boolean maybeMoreSiblings1(const ParentChunk *) const;
  void *allocFinish(size_t);
  void pulse();
  void maybePulse();
  void finishDocumentElement();
  void finishProlog();
  void addBarrier();
  void storeLocOrigin(const ConstPtr<Origin> &);

  struct BlockHeader {
    BlockHeader() : next(0) { }
    BlockHeader *next;
  };
  unsigned groveIndex_;
  SgmlDocumentChunk *root_;
  ParentChunk *origin_;
  DataChunk *pendingData_;
  Chunk **tailPtr_;
  ConstPtr<Dtd> dtd_;
  ConstPtr<Sd> sd_;
  ConstPtr<Syntax> prologSyntax_;
  ConstPtr<Syntax> instanceSyntax_;
  ConstPtr<AttributeValue> impliedAttributeValue_;
  Vector<ConstPtr<AttributeValue> > values_;
  Vector<ConstPtr<Origin> > origins_;
  NamedResourceTable<Entity> defaultedEntityTable_;
  PointerTable<ElementChunk *,StringC,Hash,ElementChunk> idTable_;
  Boolean hasDefaultEntity_;
  Boolean haveAppinfo_;
  StringC appinfo_;
  const Origin *currentLocOrigin_;

  Boolean complete_;
  const void *completeLimit_;
  const void *completeLimitWithLocChunkAfter_;
  // pointer to first free byte in current block
  char *freePtr_;
  // free bytes in current block
  // there's space for a forwarding chunk after this if freePtr_ != 0
  size_t nFree_;
  // the head of the list of blocks
  BlockHeader *blocks_;
  // where to store pointer to next block
  BlockHeader **blockTailPtr_;
  // current normal size for a block
  size_t blockAllocSize_;
  // number of blocks allocated at this size
  size_t nBlocksThisSizeAlloced_;
  RefCount refCount_;
  Condition moreNodesCondition_;
  Mutex mutex_;
  Mutex *mutexPtr_;
  unsigned pulseStep_;
  unsigned long nEvents_;
  unsigned long nElements_;
  enum { maxChunksWithoutLocOrigin = 100 };
  unsigned nChunksSinceLocOrigin_;
  MessageItem *messageList_;
  MessageItem **messageListTailP_;
};

class GroveImplPtr {
public:
  GroveImplPtr(const GroveImpl *grove) : grove_(grove) { grove_->addRef(); }
  ~GroveImplPtr() { grove_->release(); }
  const GroveImpl *operator->() const { return grove_; }
  operator const GroveImpl *() const { return grove_; }
private:
  GroveImplPtr(const GroveImplPtr &); // undefined
  void operator=(const GroveImplPtr &);	// undefined
  const GroveImpl *grove_;
};

class GroveImplProxyOrigin : public ProxyOrigin {
public:
  GroveImplProxyOrigin(const GroveImpl *grove, const Origin *origin)
    : grove_(grove), ProxyOrigin(origin) { }
private:
  GroveImplPtr grove_;
};

class GroveBuilderMessageEventHandler : public ErrorCountEventHandler {
public:
  GroveBuilderMessageEventHandler(unsigned groveIndex, Messenger *mgr, MessageFormatter *msgFmt_);
  ~GroveBuilderMessageEventHandler();
  void message(MessageEvent *);
  void sgmlDecl(SgmlDeclEvent *);
  void makeInitialRoot(NodePtr &);
  void setSd(const ConstPtr<Sd> &, const ConstPtr<Syntax> &, const ConstPtr<Syntax> &);
protected:
  GroveImpl *grove_;
private:
  Messenger *mgr_;
  MessageFormatter *msgFmt_;
};

class GroveBuilderEventHandler : public GroveBuilderMessageEventHandler {
public:
  GroveBuilderEventHandler(unsigned groveIndex, Messenger *mgr, MessageFormatter *msgFmt_);
  void appinfo(AppinfoEvent *);
  void startElement(StartElementEvent *);
  void endElement(EndElementEvent *);
  void data(DataEvent *);
  void sdataEntity(SdataEntityEvent *);
  void nonSgmlChar(NonSgmlCharEvent *);
  void externalDataEntity(ExternalDataEntityEvent *);
  void subdocEntity(SubdocEntityEvent *);
  void pi(PiEvent *);
  void endProlog(EndPrologEvent *);
  void entityDefaulted(EntityDefaultedEvent *);
};

inline
void setString(GroveString &to, const StringC &from)
{
  to.assign(from.data(), from.size());
}

inline
bool operator==(const StringC &str1, const GroveString &str2)
{
  return (str1.size() == str2.size()
          && memcmp(str1.data(), str2.data(), str1.size()*sizeof(Char)) == 0);
}

inline
bool operator!=(const StringC &str1, const GroveString &str2)
{
  return !(str1 == str2);
}

inline
size_t roundUp(size_t n)
{
  return (n + (sizeof(void *) - 1)) & ~(sizeof(void *) - 1);
}

// All nodes in this grove must be derived from BaseNode.

class BaseNode : public Node, public LocNode {
public:
  BaseNode(const GroveImpl *grove);
  virtual ~BaseNode();
  void addRef();
  void release();
  bool canReuse(NodePtr &ptr) const;
  unsigned groveIndex() const;
  bool operator==(const Node &node) const;
  // Implemented with double dispatching.
  virtual bool same(const BaseNode &) const = 0;
  // The second half of the dispatch.
  virtual bool same2(const ChunkNode *) const;
  virtual bool same2(const DataNode *) const;
  virtual bool same2(const AttributeAsgnNode *) const;
  virtual bool same2(const AttributeValueTokenNode *) const;
  virtual bool same2(const CdataAttributeValueNode *) const;
  virtual bool same2(const EntityNode *) const;
  virtual bool same2(const NotationNode *) const;
  virtual bool same2(const ExternalIdNode *) const;
  virtual bool same2(const DocumentTypeNode *) const;
  virtual bool same2(const SgmlConstantsNode *) const;
  virtual bool same2(const MessageNode *) const;
  const GroveImpl *grove() const { return grove_; }
  AccessResult nextSibling(NodePtr &ptr) const;
  AccessResult follow(NodeListPtr &ptr) const;
  AccessResult children(NodeListPtr &) const;
  AccessResult getOrigin(NodePtr &ptr) const;
  AccessResult getGroveRoot(NodePtr &ptr) const;
  AccessResult getLocation(Location &) const;
  bool queryInterface(IID, const void *&) const;
  bool chunkContains(const Node &) const;
  bool inChunk(const DataNode *node) const;
  bool inChunk(const CdataAttributeValueNode *) const;
protected:
  static unsigned long secondHash(unsigned long n) {
    return n * 1001;
  }
private:
  unsigned refCount_;
  GroveImplPtr grove_;
};

inline
BaseNode::BaseNode(const GroveImpl *grove)
: grove_(grove), refCount_(0)
{
}

inline
bool BaseNode::canReuse(NodePtr &ptr) const
{
  const Node *tem = &*ptr;
  return tem == this && refCount_ == 1;
}

struct ForwardingChunk : Chunk {
  ForwardingChunk(const Chunk *to, ParentChunk *p)
    : forwardTo(to) { origin = p; }
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
  AccessResult getFollowing(const GroveImpl *,
                            const Chunk *&, unsigned long &nNodes)
    const;
  const Chunk *after() const { return forwardTo; }
  const Chunk *forwardTo;
};

class ChunkNode : public BaseNode {
public:
  ChunkNode(const GroveImpl *grove, const LocChunk *chunk);
  const LocChunk *chunk() const { return chunk_; }
  bool same(const BaseNode &node) const;
  bool same2(const ChunkNode *node) const;
  unsigned long hash() const;
  AccessResult getParent(NodePtr &ptr) const;
  AccessResult getTreeRoot(NodePtr &ptr) const;
  AccessResult getOrigin(NodePtr &) const;
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &) const;
  AccessResult nextChunkSibling(NodePtr &) const;
  AccessResult nextChunkAfter(NodePtr &) const;
  AccessResult firstSibling(NodePtr &) const;
  AccessResult siblingsIndex(unsigned long &) const;
  AccessResult followSiblingRef(unsigned long, NodePtr &) const;
  AccessResult getLocation(Location &) const;
protected:
  const LocChunk *chunk_;		// never null
};

inline
ChunkNode::ChunkNode(const GroveImpl *grove, const LocChunk *chunk)
: BaseNode(grove), chunk_(chunk)
{
}

class SgmlDocumentNode;

struct SgmlDocumentChunk : public ParentChunk {
  SgmlDocumentChunk() : prolog(0), documentElement(0), epilog(0) { }
  Chunk *prolog;
  Chunk *documentElement;
  Chunk *epilog;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
  const Chunk *after() const { return this + 1; }
};

class SgmlDocumentNode : public ChunkNode, public SdNode {
public:
  SgmlDocumentNode(const GroveImpl *grove,
		   const SgmlDocumentChunk *chunk);
  void accept(NodeVisitor &visitor);
  const ClassDef &classDef() const { return ClassDef::sgmlDocument; }
  AccessResult getDocumentElement(NodePtr &ptr) const;
  AccessResult getElements(NamedNodeListPtr &ptr) const;
  AccessResult getEntities(NamedNodeListPtr &ptr) const;
  AccessResult getDefaultedEntities(NamedNodeListPtr &ptr) const;
  AccessResult getGoverningDoctype(NodePtr &ptr) const;
  AccessResult getDoctypesAndLinktypes(NamedNodeListPtr &ptr) const;
  AccessResult getProlog(NodeListPtr &ptr) const;
  AccessResult getEpilog(NodeListPtr &ptr) const;
  AccessResult getSgmlConstants(NodePtr &) const;
  AccessResult getApplicationInfo(GroveString &str) const;
  AccessResult getMessages(NodeListPtr &ptr) const;
  AccessResult nextChunkSibling(NodePtr &) const { return accessNotInClass; }
  AccessResult firstSibling(NodePtr &) const { return accessNotInClass; }
  AccessResult siblingsIndex(unsigned long &) const { return accessNotInClass; }
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &) const { return accessNull; }
  AccessResult getSd(ConstPtr<Sd> &sd,
		     ConstPtr<Syntax> &prologSyntax,
		     ConstPtr<Syntax> &instanceSyntax) const;
private:
  const SgmlDocumentChunk *chunk() const {
    return (const SgmlDocumentChunk *)ChunkNode::chunk();
  }
};

inline
SgmlDocumentNode::SgmlDocumentNode(const GroveImpl *grove,
				   const SgmlDocumentChunk *chunk)
: ChunkNode(grove, chunk)
{
}

// array of pointers to attribute values stored after chunk

class AttElementChunk : private ElementChunk {
protected:
  AttElementChunk(size_t n) : nAtts(n) { }
  friend class ElementNode;
private:
  const AttributeValue *
    attributeValue(size_t attIndex, const GroveImpl &) const;
  Boolean mustOmitEndTag() const;
  const Chunk *after() const;
  const StringC *id() const;
  size_t nAtts;
};

class IncludedElementChunk : public ElementChunk {
  friend class ElementNode;
  Boolean included() const;
};

class IncludedAttElementChunk : public AttElementChunk {
  IncludedAttElementChunk(size_t n) : AttElementChunk(n) { }
  friend class ElementNode;
  Boolean included() const;
};

class ElementNode : public ChunkNode {
public:
  friend class ElementChunk;
  ElementNode(const GroveImpl *grove, const ElementChunk *chunk)
    : ChunkNode(grove, chunk) { }
  AccessResult attributeRef(unsigned long i, NodePtr &ptr) const;
  AccessResult nextChunkSibling(NodePtr &ptr) const;
  AccessResult nextChunkAfter(NodePtr &) const;
  AccessResult firstChild(NodePtr &ptr) const;
  AccessResult getAttributes(NamedNodeListPtr &ptr) const;
  AccessResult getGi(GroveString &str) const;
  bool hasGi(GroveString) const;
  AccessResult getId(GroveString &str) const;
  AccessResult getContent(NodeListPtr &ptr) const;
  AccessResult getMustOmitEndTag(bool &) const;
  AccessResult getIncluded(bool &) const;
  AccessResult elementIndex(unsigned long &) const;
  void accept(NodeVisitor &visitor);
  const ClassDef &classDef() const { return ClassDef::element; }
  static void add(GroveImpl &grove, const StartElementEvent &event);
private:
  static
    ElementChunk *makeAttElementChunk(GroveImpl &grove,
				      const StartElementEvent &,
				      Boolean &hasId);
  const ElementChunk *chunk() const {
    return (const ElementChunk *)ChunkNode::chunk();
  }
  void reuseFor(const ElementChunk *chunk) { chunk_ = chunk; }
};

class CharsChunk : public LocChunk {
public:
  const Chunk *after() const {
    return (const Chunk *)((char *)this + allocSize(size));
  }
  const Char *data() const { return (const Char *)(this + 1); }
  size_t size;
  static size_t allocSize(size_t nChars) {
    return roundUp(sizeof(CharsChunk) + nChars*sizeof(Char));
  }
};
// The characters immediately follow the chunk

class DataChunk : public CharsChunk {
private:
  friend class DataNode;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
  AccessResult setNodePtrFirst(NodePtr &ptr, const ElementNode *node) const;
  AccessResult setNodePtrFirst(NodePtr &ptr, const DataNode *node) const;
  AccessResult getFollowing(const GroveImpl *, const Chunk *&, unsigned long &) const;
};

class DataNode : public ChunkNode {
public:
  friend class DataChunk;
  DataNode(const GroveImpl *, const DataChunk *chunk, size_t index);
  bool same(const BaseNode &node) const;
  bool same2(const DataNode *node) const;
  AccessResult nextSibling(NodePtr &ptr) const;
  AccessResult nextChunkSibling(NodePtr &ptr) const;
  AccessResult nextChunkAfter(NodePtr &) const;
  AccessResult siblingsIndex(unsigned long &) const;
  AccessResult followSiblingRef(unsigned long, NodePtr &) const;
  AccessResult charChunk(const SdataMapper &, GroveString &) const;
  bool chunkContains(const Node &) const;
  bool inChunk(const DataNode *node) const;
  AccessResult getNonSgml(unsigned long &) const;
  AccessResult getLocation(Location &) const;
  void accept(NodeVisitor &visitor);
  const ClassDef &classDef() const { return ClassDef::dataChar; }
  unsigned long hash() const;
  static void add(GroveImpl &grove, const DataEvent &event);
private:
  const DataChunk *chunk() const {
    return (const DataChunk *)ChunkNode::chunk();
  }
  void reuseFor(const DataChunk *chunk, size_t index);
  size_t index_;
};

inline
DataNode::DataNode(const GroveImpl *grove,
		   const DataChunk *chunk, size_t index)
: ChunkNode(grove, chunk), index_(index)
{
}

class PiChunk : private CharsChunk {
  friend class PiNode;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
};

class PrologPiChunk : public PiChunk {
  AccessResult getFirstSibling(const GroveImpl *, const Chunk *&) const;
};

class EpilogPiChunk : public PiChunk {
  AccessResult getFirstSibling(const GroveImpl *, const Chunk *&) const;
};

class PiNode : public ChunkNode {
public:
  PiNode(const GroveImpl *grove, const PiChunk *chunk)
    : ChunkNode(grove, chunk) {}
  AccessResult getSystemData(GroveString &) const;
  AccessResult getEntityName(GroveString &) const{ return accessNull; }
  AccessResult getEntity(NodePtr &) const { return accessNull; }
  void accept(NodeVisitor &visitor) { visitor.pi(*this); }
  const ClassDef &classDef() const { return ClassDef::pi; }
  static void add(GroveImpl &grove, const PiEvent &);
private:
  const PiChunk *chunk() const {
    return (const PiChunk *)ChunkNode::chunk();
  }
};

class EntityRefChunk : public LocChunk {
public:
  const Entity *entity;
  const Chunk *after() const { return this + 1; }
};

class EntityRefNode : public ChunkNode {
public:
  EntityRefNode(const GroveImpl *grove, const EntityRefChunk *chunk)
    : ChunkNode(grove, chunk) { }
  AccessResult getEntity(NodePtr &) const;
  AccessResult getEntityName(GroveString &) const;
protected:
  const EntityRefChunk *chunk() const {
    return (const EntityRefChunk *)ChunkNode::chunk();
  }
};

class SdataNode;

class SdataChunk : private EntityRefChunk {
  friend class SdataNode;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
};

class SdataNode : public EntityRefNode {
public:
  SdataNode(const GroveImpl *grove, const SdataChunk *chunk)
    : EntityRefNode(grove, chunk) { }
  AccessResult charChunk(const SdataMapper &, GroveString &) const;
  AccessResult getSystemData(GroveString &str) const;
  void accept(NodeVisitor &visitor) { visitor.sdata(*this); }
  const ClassDef &classDef() const { return ClassDef::sdata; }
  static void add(GroveImpl &grove, const SdataEntityEvent &event);
private:
  Char c_;
};

class NonSgmlNode;

class NonSgmlChunk : public LocChunk {
public:
  Char c;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
  const Chunk *after() const { return this + 1; }
};

class NonSgmlNode : public ChunkNode {
public:
  NonSgmlNode(const GroveImpl *grove, const NonSgmlChunk *chunk)
    : ChunkNode(grove, chunk) { }
  AccessResult charChunk(const SdataMapper &, GroveString &) const;
  AccessResult getNonSgml(unsigned long &) const;
  void accept(NodeVisitor &visitor) { visitor.nonSgml(*this); }
  const ClassDef &classDef() const { return ClassDef::nonSgml; }
  static void add(GroveImpl &grove, const NonSgmlCharEvent &event);
protected:
  const NonSgmlChunk *chunk() const {
    return (const NonSgmlChunk *)ChunkNode::chunk();
  }
};

class ExternalDataNode;

class ExternalDataChunk : private EntityRefChunk {
  friend class ExternalDataNode;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
};

class ExternalDataNode : public EntityRefNode {
public:
  ExternalDataNode(const GroveImpl *grove, const ExternalDataChunk *chunk)
    : EntityRefNode(grove, chunk) { }
  void accept(NodeVisitor &visitor) { visitor.externalData(*this); }
  const ClassDef &classDef() const { return ClassDef::externalData; }
  static void add(GroveImpl &grove, const ExternalDataEntityEvent &event);
};

class SubdocChunk : private EntityRefChunk {
  friend class SubdocNode;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
};

class SubdocNode : public EntityRefNode {
public:
  SubdocNode(const GroveImpl *grove, const SubdocChunk *chunk)
    : EntityRefNode(grove, chunk) { }
  void accept(NodeVisitor &visitor) { visitor.subdocument(*this); }
  const ClassDef &classDef() const { return ClassDef::subdocument; }
  static void add(GroveImpl &grove, const SubdocEntityEvent &event);
};

class PiEntityChunk : private EntityRefChunk {
  friend class PiEntityNode;
  AccessResult setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const;
};

class PiEntityNode : public EntityRefNode {
public:
  PiEntityNode(const GroveImpl *grove, const PiEntityChunk *chunk)
    : EntityRefNode(grove, chunk) { }
  AccessResult getSystemData(GroveString &) const;
  void accept(NodeVisitor &visitor) { visitor.pi(*this); }
  const ClassDef &classDef() const { return ClassDef::pi; }
  static void add(GroveImpl &grove,  const Entity *, const Location &);
};

struct AttributeOrigin {
  virtual const AttributeDefinitionList *attDefList() const = 0;
  virtual const AttributeValue *
    attributeValue(size_t attIndex, const GroveImpl &grove) const = 0;
  virtual AccessResult
    setNodePtrAttributeOrigin(NodePtr &, const BaseNode *) const = 0;
  virtual Node *makeCdataAttributeValueNode(const GroveImpl *grove,
					    const AttributeValue *value,
					    size_t attIndex,
					    const TextIter &iter,
					    size_t charIndex = 0) const = 0;
  virtual Node *makeAttributeValueTokenNode(const GroveImpl *grove,
					    const TokenizedAttributeValue *value,
					    size_t attIndex,
					    size_t tokenIndex) const = 0;
  virtual Node *makeAttributeAsgnNode(const GroveImpl *grove,
				      size_t attIndex) const = 0;
  virtual const void *attributeOriginId() const = 0;
};

class ElementAttributeOrigin : public virtual AttributeOrigin {
public:
  ElementAttributeOrigin(const ElementChunk *);
  const AttributeDefinitionList *attDefList() const;
  const AttributeValue *
    attributeValue(size_t attIndex, const GroveImpl &grove) const;
  AccessResult setNodePtrAttributeOrigin(NodePtr &, const BaseNode *) const;
  Node *makeCdataAttributeValueNode(const GroveImpl *grove,
				    const AttributeValue *value,
				    size_t attIndex,
				    const TextIter &iter,
				    size_t charIndex) const;
  Node *makeAttributeValueTokenNode(const GroveImpl *grove,
				    const TokenizedAttributeValue *value,
				    size_t attIndex,
				    size_t tokenIndex) const;
  Node *makeAttributeAsgnNode(const GroveImpl *grove,
			      size_t attIndex) const;
  const void *attributeOriginId() const;
private:
  const ElementChunk *chunk_;
};

class EntityAttributeOrigin : public virtual AttributeOrigin {
public:
  EntityAttributeOrigin(const ExternalDataEntity *);
  const AttributeDefinitionList *attDefList() const;
  const AttributeValue *
    attributeValue(size_t attIndex, const GroveImpl &grove) const;
  AccessResult setNodePtrAttributeOrigin(NodePtr &, const BaseNode *) const;
  Node *makeCdataAttributeValueNode(const GroveImpl *grove,
				    const AttributeValue *value,
				    size_t attIndex,
				    const TextIter &iter,
				    size_t charIndex) const;
  Node *makeAttributeValueTokenNode(const GroveImpl *grove,
				    const TokenizedAttributeValue *value,
				    size_t attIndex,
				    size_t tokenIndex) const;
  Node *makeAttributeAsgnNode(const GroveImpl *grove,
			      size_t attIndex) const;
  const void *attributeOriginId() const;
private:
  const ExternalDataEntity *entity_;
};


class AttributeAsgnNode : public BaseNode, public virtual AttributeOrigin {
public:
  AttributeAsgnNode(const GroveImpl *grove, size_t attIndex);
  AccessResult getOrigin(NodePtr &ptr) const;
  AccessResult getName(GroveString &str) const;
  AccessResult getImplied(bool &implied) const;
  AccessResult getValue(NodeListPtr &ptr) const;
  AccessResult children(NodeListPtr &ptr) const;
  AccessResult firstChild(NodePtr &ptr) const;
  AccessResult nextChunkSibling(NodePtr &ptr) const;
  AccessResult followSiblingRef(unsigned long, NodePtr &) const;
  AccessResult firstSibling(NodePtr &) const;
  AccessResult siblingsIndex(unsigned long &) const;
  AccessResult getTokenSep(Char &) const;
  AccessResult tokens(GroveString &) const;
  void accept(NodeVisitor &visitor);
  const ClassDef &classDef() const { return ClassDef::attributeAssignment; }
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::idAttributes;
    return accessOK;
  }
  bool same(const BaseNode &node) const;
  bool same2(const AttributeAsgnNode *node) const;
  unsigned long hash() const;
private:
  size_t attIndex_;
};

class ElementAttributeAsgnNode
: public AttributeAsgnNode, public ElementAttributeOrigin {
public:
  ElementAttributeAsgnNode(const GroveImpl *grove, size_t attIndex,
			   const ElementChunk *);
};

class EntityAttributeAsgnNode
: public AttributeAsgnNode, public EntityAttributeOrigin {
public:
  EntityAttributeAsgnNode(const GroveImpl *grove, size_t attIndex,
			   const ExternalDataEntity *);
};

class AttributeValueTokenNode
: public BaseNode, public virtual AttributeOrigin {
public:
  AttributeValueTokenNode(const GroveImpl *grove,
			  const TokenizedAttributeValue *value,
			  size_t attIndex, size_t tokenIndex);
  AccessResult getParent(NodePtr &ptr) const;
  AccessResult nextChunkSibling(NodePtr &ptr) const;
  AccessResult followSiblingRef(unsigned long, NodePtr &ptr) const;
  AccessResult firstSibling(NodePtr &) const;
  AccessResult siblingsIndex(unsigned long &) const;
  AccessResult getToken(GroveString &str) const;
  AccessResult getEntity(NodePtr &ptr) const;
  AccessResult getNotation(NodePtr &ptr) const;
  AccessResult getReferent(NodePtr &ptr) const;
  AccessResult getLocation(Location &) const;
  void accept(NodeVisitor &visitor);
  const ClassDef &classDef() const { return ClassDef::attributeValueToken; }
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::idValue;
    return accessOK;
  }
  bool same(const BaseNode &node) const;
  bool same2(const AttributeValueTokenNode *node) const;
  unsigned long hash() const;
private:
  const TokenizedAttributeValue *value_;
  size_t attIndex_;
  size_t tokenIndex_;
};

class ElementAttributeValueTokenNode
: public AttributeValueTokenNode, public ElementAttributeOrigin {
public:
  ElementAttributeValueTokenNode(const GroveImpl *grove,
				 const TokenizedAttributeValue *value,
				 size_t attIndex,
				 size_t tokenIndex,
				 const ElementChunk *);
};

class EntityAttributeValueTokenNode
: public AttributeValueTokenNode, public EntityAttributeOrigin {
public:
  EntityAttributeValueTokenNode(const GroveImpl *grove,
				const TokenizedAttributeValue *value,
				size_t attIndex,
				size_t tokenIndex,
				const ExternalDataEntity *);
};

class CdataAttributeValueNode
: public BaseNode, public virtual AttributeOrigin {
public:
  static bool skipBoring(TextIter &iter);
  CdataAttributeValueNode(const GroveImpl *grove,
			  const AttributeValue *value,
			  size_t attIndex,
			  const TextIter &iter,
			  size_t charIndex);
  AccessResult getParent(NodePtr &ptr) const;
  AccessResult charChunk(const SdataMapper &, GroveString &) const;
  bool chunkContains(const Node &) const;
  bool inChunk(const CdataAttributeValueNode *) const;
  AccessResult getEntity(NodePtr &) const;
  AccessResult getEntityName(GroveString &) const;
  AccessResult getSystemData(GroveString &str) const;
  AccessResult nextSibling(NodePtr &ptr) const;
  AccessResult nextChunkSibling(NodePtr &ptr) const;
  AccessResult firstSibling(NodePtr &) const;
  AccessResult siblingsIndex(unsigned long &) const;
  AccessResult getLocation(Location &) const;
  void accept(NodeVisitor &visitor);
  const ClassDef &classDef() const;
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::idValue;
    return accessOK;
  }
  bool same(const BaseNode &node) const;
  bool same2(const CdataAttributeValueNode *node) const;
  unsigned long hash() const;
private:
  const AttributeValue *value_;
  size_t attIndex_;
  TextIter iter_; // must be valid
  size_t charIndex_;
  Char c_;
};

class ElementCdataAttributeValueNode
: public CdataAttributeValueNode, public ElementAttributeOrigin {
public:
  ElementCdataAttributeValueNode(const GroveImpl *grove,
				 const AttributeValue *value,
				 size_t attIndex,
				 const TextIter &iter,
				 size_t charIndex,
				 const ElementChunk *);
};

class EntityCdataAttributeValueNode
: public CdataAttributeValueNode, public EntityAttributeOrigin {
public:
  EntityCdataAttributeValueNode(const GroveImpl *grove,
				const AttributeValue *value,
				size_t attIndex,
				const TextIter &iter,
				size_t charIndex,
				const ExternalDataEntity *);
};

class EntityNode : public BaseNode {
public:
  EntityNode(const GroveImpl *grove, const Entity *entity);
  AccessResult getOrigin(NodePtr &ptr) const;
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &) const;
  AccessResult getName(GroveString &str) const;
  AccessResult getExternalId(NodePtr &ptr) const;
  AccessResult getNotation(NodePtr &) const;
  AccessResult getNotationName(GroveString &) const;
  AccessResult getText(GroveString &) const;
  AccessResult getEntityType(EntityType &) const;
  AccessResult getDefaulted(bool &) const;
  AccessResult getAttributes(NamedNodeListPtr &) const;
  AccessResult attributeRef(unsigned long i, NodePtr &ptr) const;
  AccessResult getLocation(Location &) const;
  bool same(const BaseNode &) const;
  bool same2(const EntityNode *) const;
  void accept(NodeVisitor &);
  const ClassDef &classDef() const { return ClassDef::entity; }
  unsigned long hash() const;
private:
  const Entity *entity_;
};

class NotationNode : public BaseNode {
public:
  NotationNode(const GroveImpl *grove, const Notation *notation);
  AccessResult getOrigin(NodePtr &ptr) const;
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::idNotations;
    return accessOK;
  }
  AccessResult getName(GroveString &str) const;
  AccessResult getExternalId(NodePtr &ptr) const;
  bool same(const BaseNode &) const;
  bool same2(const NotationNode *) const;
  AccessResult getLocation(Location &) const;
  void accept(NodeVisitor &);
  const ClassDef &classDef() const { return ClassDef::notation; }
  unsigned long hash() const;
private:
  const Notation *notation_;
};

class ExternalIdNode : public BaseNode {
public:
  ExternalIdNode(const GroveImpl *grove);
  virtual const ExternalId &externalId() const = 0;
  AccessResult getPublicId(GroveString &) const;
  AccessResult getSystemId(GroveString &) const;
  AccessResult getGeneratedSystemId(GroveString &) const;
  void accept(NodeVisitor &);
  const ClassDef &classDef() const { return ClassDef::externalId; }
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::idExternalId;
    return accessOK;
  }
  bool same(const BaseNode &) const;
  bool same2(const ExternalIdNode *) const;
};

class EntityExternalIdNode : public ExternalIdNode {
public:
  EntityExternalIdNode(const GroveImpl *grove,
		       const ExternalEntity *entity);
  const ExternalId &externalId() const;
  AccessResult getOrigin(NodePtr &ptr) const;
  unsigned long hash() const;
private:
  const ExternalEntity *entity_;
};

class NotationExternalIdNode : public ExternalIdNode {
public:
  NotationExternalIdNode(const GroveImpl *grove,
			 const Notation *notation);
  const ExternalId &externalId() const;
  AccessResult getOrigin(NodePtr &ptr) const;
  unsigned long hash() const;
private:
  const Notation *notation_;
};

class DocumentTypeNode : public BaseNode {
public:
  DocumentTypeNode(const GroveImpl *grove, const Dtd *);
  AccessResult getName(GroveString &) const;
  AccessResult getGoverning(bool &) const;
  AccessResult getGeneralEntities(NamedNodeListPtr &) const;
  AccessResult getNotations(NamedNodeListPtr &) const;
  AccessResult getOrigin(NodePtr &) const;
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::idDoctypesAndLinktypes;
    return accessOK;
  }
  AccessResult nextChunkSibling(NodePtr &) const;
  void accept(NodeVisitor &);
  const ClassDef &classDef() const { return ClassDef::documentType; }
  bool same(const BaseNode &) const;
  bool same2(const DocumentTypeNode *) const;
private:
  const Dtd *dtd_;
};

class SgmlConstantsNode : public BaseNode {
public:
  SgmlConstantsNode(const GroveImpl *);
  AccessResult getOrigin(NodePtr &) const;
  void accept(NodeVisitor &);
  const ClassDef &classDef() const { return ClassDef::sgmlConstants; }
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::idSgmlConstants;
    return accessOK;
  }
  bool same(const BaseNode &) const;
  bool same2(const SgmlConstantsNode *) const;
};

class MessageNode : public BaseNode {
public:
  MessageNode(const GroveImpl *, const MessageItem *);
  AccessResult getOrigin(NodePtr &) const;
  AccessResult getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const {
    name = ComponentName::noId;
    return accessOK;
  }
  AccessResult nextChunkSibling(NodePtr &) const;
  AccessResult firstSibling(NodePtr &) const;
  AccessResult siblingsIndex(unsigned long &) const;
  void accept(NodeVisitor &);
  const ClassDef &classDef() const { return ClassDef::message; }
  bool same(const BaseNode &) const;
  bool same2(const MessageNode *) const;
  AccessResult getLocation(Location &) const;
  AccessResult getText(GroveString &) const;
  AccessResult getSeverity(Severity &) const;
private:
  const MessageItem *item_;
};

class BaseNodeList : public NodeList {
public:
  BaseNodeList() : refCount_(0) { }
  virtual ~BaseNodeList() { }
  void addRef() { ++refCount_; }
  bool canReuse(NodeListPtr &ptr) const {
    const NodeList *tem = &*ptr;
    return tem == this && refCount_ == 1;
  }
  void release() {
    ASSERT(refCount_ != 0);
    if (--refCount_ == 0) delete this;
  }
  AccessResult first(NodePtr &) const { return accessNull; }
  AccessResult rest(NodeListPtr &ptr) const { return chunkRest(ptr); }
  AccessResult chunkRest(NodeListPtr &) const { return accessNull; }
private:
  unsigned refCount_;
};

class SiblingNodeList : public BaseNodeList {
public:
  SiblingNodeList(const NodePtr &first) : first_(first) { }
  AccessResult first(NodePtr &ptr) const {
    ptr = first_;
    return accessOK;
  }
  AccessResult rest(NodeListPtr &ptr) const {
    AccessResult ret;
    if (canReuse(ptr)) {
      ret = ((SiblingNodeList *)this)->first_.assignNextSibling();
      if (ret == accessOK)
	return ret;
    }
    else {
      NodePtr next;
      ret = first_->nextSibling(next);
      if (ret == accessOK) {
	ptr.assign(new SiblingNodeList(next));
	return ret;
      }
    }
    if (ret == accessNull) {
      ptr.assign(new BaseNodeList);
      return accessOK;
    }
    return ret;
  }
  AccessResult chunkRest(NodeListPtr &ptr) const {
    AccessResult ret;
    if (canReuse(ptr)) {
      ret = ((SiblingNodeList *)this)->first_.assignNextChunkSibling();
      if (ret == accessOK)
	return ret;
    }
    else {
      NodePtr next;
      ret = first_->nextChunkSibling(next);
      if (ret == accessOK) {
	ptr.assign(new SiblingNodeList(next));
	return ret;
      }
    }
    if (ret == accessNull) {
      ptr.assign(new BaseNodeList);
      return accessOK;
    }
    return ret;
  }
  AccessResult ref(unsigned long i, NodePtr &ptr) const {
    if (i == 0) {
      ptr = first_;
      return accessOK;
    }
    return first_->followSiblingRef(i - 1, ptr);
  }
private:
  NodePtr first_; // never null
};

class BaseNamedNodeList : public NamedNodeList {
public:
  BaseNamedNodeList(const GroveImpl *grove,
		    const SubstTable<Char> *substTable)
  : grove_(grove), substTable_(substTable), refCount_(0) { }
  virtual ~BaseNamedNodeList() { }
  void addRef() { ++refCount_; }
  bool canReuse(NamedNodeListPtr &ptr) const {
    const NamedNodeList *tem = &*ptr;
    return tem == this && refCount_ == 1;
  }
  void release() {
    ASSERT(refCount_ != 0);
    if (--refCount_ == 0) delete this;
  }
  size_t normalize(Char *s, size_t n) const {
    if (substTable_) {
      for (size_t i = 0; i < n; i++)
	substTable_->subst(s[i]);
    }
    return n;
  }
  const GroveImpl *grove() const { return grove_; }
  AccessResult namedNode(GroveString str, NodePtr &node) const {
    StringC tem(str.data(), str.size());
    normalize(&tem[0], tem.size());
    return namedNodeU(tem, node);
  }
  virtual AccessResult namedNodeU(const StringC &, NodePtr &) const = 0;
private:
  GroveImplPtr grove_;
  const SubstTable<Char> *substTable_;
  unsigned refCount_;
};

class AttributesNamedNodeList
: public BaseNamedNodeList, public virtual AttributeOrigin {
public:
  AttributesNamedNodeList(const GroveImpl *grove)
   : BaseNamedNodeList(grove, grove->generalSubstTable()) { }
  NodeListPtr nodeList() const;
  AccessResult namedNodeU(const StringC &, NodePtr &) const;
  Type type() const { return attributes; }
};

class ElementAttributesNamedNodeList
: public AttributesNamedNodeList, public ElementAttributeOrigin {
public:
  ElementAttributesNamedNodeList(const GroveImpl *grove,
				 const ElementChunk *chunk)
   : AttributesNamedNodeList(grove), ElementAttributeOrigin(chunk) { }
};

class EntityAttributesNamedNodeList
: public AttributesNamedNodeList, public EntityAttributeOrigin {
public:
  EntityAttributesNamedNodeList(const GroveImpl *grove,
				const ExternalDataEntity *entity)
   : AttributesNamedNodeList(grove), EntityAttributeOrigin(entity) { }
};

class ElementsNamedNodeList : public BaseNamedNodeList {
public:
  ElementsNamedNodeList(const GroveImpl *grove)
    : BaseNamedNodeList(grove, grove->generalSubstTable()) { }
  NodeListPtr nodeList() const;
  AccessResult namedNodeU(const StringC &, NodePtr &) const;
  Type type() const { return elements; }
};

class DocEntitiesNamedNodeList : public BaseNamedNodeList {
public:
  DocEntitiesNamedNodeList(const GroveImpl *grove)
   : BaseNamedNodeList(grove, grove->entitySubstTable()) { }
  NodeListPtr nodeList() const;
  AccessResult namedNodeU(const StringC &, NodePtr &) const;
  Type type() const { return entities; }
};

class DefaultedEntitiesNamedNodeList : public BaseNamedNodeList {
public:
  DefaultedEntitiesNamedNodeList(const GroveImpl *grove)
   : BaseNamedNodeList(grove, grove->entitySubstTable()) { }
  NodeListPtr nodeList() const;
  AccessResult namedNodeU(const StringC &, NodePtr &) const;
  Type type() const { return entities; }
};

class GeneralEntitiesNamedNodeList : public BaseNamedNodeList {
public:
  GeneralEntitiesNamedNodeList(const GroveImpl *, const Dtd *);
  NodeListPtr nodeList() const;
  AccessResult namedNodeU(const StringC &, NodePtr &) const;
  Type type() const { return entities; }
private:
  const Dtd *dtd_;
};

class NotationsNamedNodeList : public BaseNamedNodeList {
public:
  NotationsNamedNodeList(const GroveImpl *, const Dtd *);
  NodeListPtr nodeList() const;
  AccessResult namedNodeU(const StringC &, NodePtr &) const;
  Type type() const { return notations; }
private:
  const Dtd *dtd_;
};

class DoctypesAndLinktypesNamedNodeList : public BaseNamedNodeList {
public:
  DoctypesAndLinktypesNamedNodeList(const GroveImpl *);
  NodeListPtr nodeList() const;
  AccessResult namedNodeU(const StringC &, NodePtr &) const;
  Type type() const { return doctypesAndLinktypes; }
};

class ElementsNodeList : public BaseNodeList {
public:
  ElementsNodeList(const GroveImpl *grove,
                   const Chunk *head);
  AccessResult first(NodePtr &) const;
  AccessResult chunkRest(NodeListPtr &) const;
public:
  GroveImplPtr grove_;
  const Chunk *first_;
};

class EntitiesNodeList : public BaseNodeList {
public:
  EntitiesNodeList(const GroveImpl *grove,
		   const Dtd::ConstEntityIter &iter);
  AccessResult first(NodePtr &) const;
  AccessResult chunkRest(NodeListPtr &) const;
protected:
  const GroveImpl *grove() const { return grove_; }
public:
  GroveImplPtr grove_;
  Dtd::ConstEntityIter iter_;
};

class DocEntitiesNodeList : public EntitiesNodeList {
public:
  DocEntitiesNodeList(const GroveImpl *grove);
  AccessResult first(NodePtr &) const;
  AccessResult chunkRest(NodeListPtr &) const;
};

class NotationsNodeList : public BaseNodeList {
public:
  NotationsNodeList(const GroveImpl *grove,
                    const Dtd::ConstNotationIter &iter);
  AccessResult first(NodePtr &) const;
  AccessResult chunkRest(NodeListPtr &) const;
public:
  GroveImplPtr grove_;
  Dtd::ConstNotationIter iter_;
};

inline
Boolean GroveImpl::waitForMoreNodes() const
{
  if (blockingAccess)
    return moreNodesCondition_.wait();
  else
    return 0;
}

inline
void GroveImpl::pulse()
{
  moreNodesCondition_.pulse();
}

inline
void GroveImpl::maybePulse()
{
  // Once we've had (2^n)*(2^10) events, only pulse every (2^n)th event.
  // Up to a limit of n == 8.
  // This reduces the overhead of pulsing to negligible levels on WinNT.
  if ((++nEvents_ & ~(~unsigned(0) << pulseStep_)) == 0) {
    pulse();
    if (pulseStep_ < 8 && nEvents_ > (1 << (pulseStep_ + 10)))
      pulseStep_++;
  }
}

inline
void GroveImpl::appendSibling(Chunk *chunk)
{
  if (pendingData_) {
    if (tailPtr_) {
      // Must set completeLimit_ before setting tailPtr_.
      completeLimit_ = pendingData_->after();
      *tailPtr_ = pendingData_;
      tailPtr_ = 0;
    }
    pendingData_ = 0;
  }
  // Must set origin before advancing completeLimit_.
  chunk->origin = origin_;
  // Must advance completeLimit_ before setting tailPtr_.
  completeLimit_ = freePtr_;
  if (tailPtr_) {
    *tailPtr_ = chunk;
    tailPtr_ = 0;
  }
  pendingData_ = 0;
  maybePulse();
}

inline
void GroveImpl::appendSibling(DataChunk *chunk)
{
  // Since we might extend this DataChunk, it's
  // not safe to set completeLimit_ to after this chunk yet.
  // This means we can't yet set tailPtr_.
  if (pendingData_) {
    // Must set completeLimit_ before setting tailPtr_.
    completeLimit_ = pendingData_->after();
    if (tailPtr_) {
      *tailPtr_ = pendingData_;
      tailPtr_ = 0;
    }
  }
  chunk->origin = origin_;
  pendingData_ = chunk;
  maybePulse();
}

inline
void GroveImpl::push(ElementChunk *chunk, Boolean hasId)
{
  if (pendingData_) {
    if (tailPtr_) {
      // Must set completeLimit_ before setting tailPtr_.
      completeLimit_ = pendingData_->after();
      *tailPtr_ = pendingData_;
      tailPtr_ = 0;
    }
    pendingData_ = 0;
  }
  chunk->elementIndex = nElements_++;
  chunk->origin = origin_;
  // Must set origin_ to chunk before advancing completeLimit_
  // otherwise thread would look at element and
  // maybeMoreSiblings() would return false.
  // Must advance completeLimit_ before setting tailPtr_,
  // otherwise tailPtr_ would be beyond completeLimit_.
  origin_ = chunk;
  completeLimit_ = freePtr_;
  // Allow for the possibility of invalid documents with elements
  // after the document element.
  if ((const Chunk *)chunk->origin == root_ && root_->documentElement == 0)
    root_->documentElement = chunk;
  else if (tailPtr_) {
    *tailPtr_ = chunk;
    tailPtr_ = 0;
  }
  if (hasId) {
    Mutex::Lock lock(mutexPtr_);
    idTable_.insert(chunk);
  }
  maybePulse();
}

inline
void GroveImpl::pop()
{
  if (pendingData_) {
    // Must set completeLimit_ before setting tailPtr_.
    completeLimit_ = pendingData_->after();
    if (tailPtr_) {
      *tailPtr_ = pendingData_;
      tailPtr_ = 0;
    }
    pendingData_ = 0;
  }
  tailPtr_ = &origin_->nextSibling;
  origin_ = origin_->origin;
  if ((const Chunk *)origin_ == root_)
    finishDocumentElement();
  maybePulse();
}

inline
Boolean GroveImpl::haveRootOrigin()
{
  return (const Chunk *)origin_ == root_;
}

inline
void GroveImpl::setDtd(const ConstPtr<Dtd> &dtd)
{
  dtd_ = dtd;
  hasDefaultEntity_ = !dtd_->defaultEntity().isNull();
  finishProlog();
  pulse();
}

inline
const ElementChunk *GroveImpl::lookupElement(const StringC &id) const
{
  Mutex::Lock lock(mutexPtr_);
  return idTable_.lookup(id);
}

inline
GroveImpl::ElementIter GroveImpl::elementIter() const
{
  ASSERT(complete());
  return ElementIter(idTable_);
}

inline
Boolean GroveImpl::maybeMoreSiblings(const ParentChunk *chunk) const
{
  return (complete_
          ? chunk->nextSibling != 0
	  : (origin_ == chunk 
	      || &chunk->nextSibling == tailPtr_
	      || maybeMoreSiblings1(chunk)));
} 

inline
void *GroveImpl::allocChunk(size_t n)
{
  nChunksSinceLocOrigin_++;
  if (n <= nFree_) {
    void *p = freePtr_;
    freePtr_ += n;
    nFree_ -= n;
    return p;
  }
  else
    return allocFinish(n);
}

inline
void GroveImpl::setLocOrigin(const ConstPtr<Origin> &locOrigin)
{
  if (locOrigin.pointer() != currentLocOrigin_
      || nChunksSinceLocOrigin_ >= maxChunksWithoutLocOrigin)
    storeLocOrigin(locOrigin);
}

inline
void GroveImpl::appendMessage(MessageItem *item)
{
  *messageListTailP_ = item;
  messageListTailP_ = item->nextP();
  pulse();
}

inline
void ElementNode::add(GroveImpl &grove, const StartElementEvent &event)
{
  grove.setLocOrigin(event.location().origin());
  ElementChunk *chunk;
  const AttributeList &atts = event.attributes();
  Boolean hasId;
  if (atts.nSpec() == 0 && !atts.anyCurrent()) {
    void *mem = grove.allocChunk(sizeof(ElementChunk));
    if (event.included())
      chunk = new (mem) IncludedElementChunk;
    else
      chunk = new (mem) ElementChunk;
    hasId = 0;
  }
  else
    chunk = makeAttElementChunk(grove, event, hasId);
  chunk->type = event.elementType();
  chunk->locIndex = event.location().index();
  grove.push(chunk, hasId);
}

// We duplicate ChunkNode::nextChunkSibling to take advantage
// of Node reuse (via setNodePtrFirst(NodePtr &, const DataNode *).
inline
AccessResult DataNode::nextChunkSibling(NodePtr &ptr) const
{
  // The forwarding chunk has origin = 0, so it will stop
  // the iteration before after() can return 0.
  const Chunk *p = chunk_->after();
  while (p == grove()->completeLimit())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  if (p->origin != chunk_->origin)
    return accessNull;
  return p->setNodePtrFirst(ptr, this);
}

inline
void DataNode::reuseFor(const DataChunk *chunk, size_t index)
{
  chunk_ = chunk;
  index_ = index;
}

inline
void DataNode::add(GroveImpl &grove, const DataEvent &event)
{
  size_t dataLen = event.dataLength();
  if (dataLen) {
   DataChunk *chunk = grove.pendingData();
   if (chunk
       && event.location().origin().pointer() == grove.currentLocOrigin()
       && event.location().index() == chunk->locIndex + chunk->size
       && grove.tryExtend(CharsChunk::allocSize(chunk->size + dataLen)
                          - CharsChunk::allocSize(chunk->size))) {
     memcpy((Char *)(chunk + 1) + chunk->size,
	    event.data(),
	    dataLen * sizeof(Char));
     chunk->size += dataLen;
   }
   else {
     grove.setLocOrigin(event.location().origin());
     chunk = new (grove.allocChunk(CharsChunk::allocSize(dataLen))) DataChunk;
     chunk->size = dataLen;
     chunk->locIndex = event.location().index();
     memcpy(chunk + 1, event.data(), dataLen * sizeof(Char));
     grove.appendSibling(chunk);
   }
 }
}

GroveBuilderMessageEventHandler::GroveBuilderMessageEventHandler(unsigned groveIndex,
								 Messenger *mgr,
								 MessageFormatter *msgFmt)
: mgr_(mgr), grove_(new GroveImpl(groveIndex)), msgFmt_(msgFmt)
{
  grove_->addRef();
}

GroveBuilderMessageEventHandler::~GroveBuilderMessageEventHandler()
{
  grove_->setComplete();
  grove_->release();
}

void GroveBuilderMessageEventHandler::makeInitialRoot(NodePtr &root)
{
  root.assign(new SgmlDocumentNode(grove_, grove_->root()));
}

void GroveBuilderMessageEventHandler::message(MessageEvent *event)
{
  mgr_->dispatchMessage(event->message());
  const Message &msg = event->message();
  StrOutputCharStream os;
  msgFmt_->formatMessage(*msg.type, msg.args, os);
  StringC tem;
  os.extractString(tem);
  Node::Severity severity;
  switch (msg.type->severity()) {
  case MessageType::info:
    severity = Node::info;
    break;
  case MessageType::warning:
    severity = Node::warning;
    break;
  default:
    severity = Node::error;
    break;
  }
  grove_->appendMessage(new MessageItem(severity, tem, msg.loc));
  if (!msg.auxLoc.origin().isNull()) {
    msgFmt_->formatMessage(msg.type->auxFragment(), msg.args, os);
    os.extractString(tem);
    grove_->appendMessage(new MessageItem(Node::info, tem, msg.auxLoc));
  }
  ErrorCountEventHandler::message(event);
}

void GroveBuilderMessageEventHandler::sgmlDecl(SgmlDeclEvent *event)
{
  grove_->setSd(event->sdPointer(), event->prologSyntaxPointer(), event->instanceSyntaxPointer());
  delete event;
}

void GroveBuilderMessageEventHandler::setSd(const ConstPtr<Sd> &sd, const ConstPtr<Syntax> &prologSyntax, const ConstPtr<Syntax> &instanceSyntax)
{
  grove_->setSd(sd, prologSyntax, instanceSyntax);
}

GroveBuilderEventHandler::GroveBuilderEventHandler(unsigned groveIndex,
						   Messenger *mgr,
						   MessageFormatter *msgFmt)
: GroveBuilderMessageEventHandler(groveIndex, mgr, msgFmt)
{
}

void GroveBuilderEventHandler::appinfo(AppinfoEvent *event)
{
  const StringC *appinfo;
  if (event->literal(appinfo))
    grove_->setAppinfo(*appinfo);
  delete event;
}

void GroveBuilderEventHandler::endProlog(EndPrologEvent *event)
{
  grove_->setDtd(event->dtdPointer());
  delete event;
}

void GroveBuilderEventHandler::startElement(StartElementEvent *event)
{
  ElementNode::add(*grove_, *event);
  delete event;
}

void GroveBuilderEventHandler::endElement(EndElementEvent *event)
{
  grove_->pop();
  delete event;
}

void GroveBuilderEventHandler::data(DataEvent *event)
{
  DataNode::add(*grove_, *event);
  delete event;
}

void GroveBuilderEventHandler::sdataEntity(SdataEntityEvent *event)
{
  SdataNode::add(*grove_, *event);
  delete event;
}

void GroveBuilderEventHandler::nonSgmlChar(NonSgmlCharEvent *event)
{
  NonSgmlNode::add(*grove_, *event);
  delete event;
}

void GroveBuilderEventHandler::externalDataEntity(ExternalDataEntityEvent *event)
{
  ExternalDataNode::add(*grove_, *event);
  delete event;
}

void GroveBuilderEventHandler::subdocEntity(SubdocEntityEvent *event)
{
  SubdocNode::add(*grove_, *event);
  delete event;
}

void GroveBuilderEventHandler::pi(PiEvent *event)
{
  PiNode::add(*grove_, *event);
  delete event;
}

void GroveBuilderEventHandler::entityDefaulted(EntityDefaultedEvent *event)
{
  grove_->addDefaultedEntity(event->entityPointer());
  delete event;
}

ErrorCountEventHandler *GroveBuilder::make(unsigned index,
					   Messenger *mgr,
					   MessageFormatter *msgFmt,
					   bool validateOnly,
					   NodePtr &root)
{
  GroveBuilderMessageEventHandler *eh;
  if (validateOnly)
    eh = new GroveBuilderMessageEventHandler(index, mgr, msgFmt);
  else
    eh = new GroveBuilderEventHandler(index, mgr, msgFmt);
  eh->makeInitialRoot(root);
  return eh;
}

ErrorCountEventHandler *GroveBuilder::make(unsigned index,
					   Messenger *mgr,
					   MessageFormatter *msgFmt,
					   bool validateOnly,
					   const ConstPtr<Sd> &sd,
					   const ConstPtr<Syntax> &prologSyntax,
					   const ConstPtr<Syntax> &instanceSyntax,
					   NodePtr &root)
{
  GroveBuilderMessageEventHandler *eh;
  if (validateOnly)
    eh = new GroveBuilderMessageEventHandler(index, mgr, msgFmt);
  else
    eh = new GroveBuilderEventHandler(index, mgr, msgFmt);
  eh->makeInitialRoot(root);
  eh->setSd(sd, prologSyntax, instanceSyntax);
  return eh;
}

bool GroveBuilder::setBlocking(bool b)
{
  bool prev = blockingAccess;
  blockingAccess = b;
  return prev;
}

GroveImpl::GroveImpl(unsigned groveIndex)
: groveIndex_(groveIndex),
  root_(0),
  impliedAttributeValue_(new ImpliedAttributeValue),
  tailPtr_(0),
  freePtr_(0),
  nFree_(0),
  blocks_(0),
  blockTailPtr_(&blocks_),
  blockAllocSize_(initialBlockSize),
  nBlocksThisSizeAlloced_(0),
  complete_(0),
  mutexPtr_(&mutex_),
  pulseStep_(0),
  nEvents_(0),
  haveAppinfo_(0),
  pendingData_(0),
  nElements_(0),
  currentLocOrigin_(0),
  completeLimitWithLocChunkAfter_(0),
  nChunksSinceLocOrigin_(0),
  messageList_(0),
  messageListTailP_(&messageList_)
{
  root_ = new (allocChunk(sizeof(SgmlDocumentChunk))) SgmlDocumentChunk;
  root_->origin = 0;
  root_->locIndex = 0;
  completeLimit_ = freePtr_;
  origin_ = root_;
  tailPtr_ = &root_->prolog;
}

GroveImpl::~GroveImpl()
{
  while (blocks_) {
    BlockHeader *tem = blocks_;
    blocks_ = blocks_->next;
    ::operator delete(tem);
  }
  while (messageList_) {
    MessageItem *tem = messageList_;
    messageList_ = *messageList_->nextP();
    delete tem;
  }
}

void GroveImpl::setAppinfo(const StringC &appinfo)
{
  appinfo_ = appinfo;
  haveAppinfo_ = 1;
}

Boolean GroveImpl::getAppinfo(const StringC *&appinfo) const
{
  if (!haveAppinfo_) {
    if (!complete_ && sd_.isNull())
      return 0; // not available yet
    appinfo = 0;
  }
  else
    appinfo = &appinfo_;
  return 1;
}

void GroveImpl::setSd(const ConstPtr<Sd> &sd, const ConstPtr<Syntax> &prologSyntax, const ConstPtr<Syntax> &instanceSyntax)
{
  instanceSyntax_ = instanceSyntax;
  prologSyntax_ = prologSyntax;
  sd_ = sd;
}

void GroveImpl::getSd(ConstPtr<Sd> &sd, ConstPtr<Syntax> &prologSyntax, ConstPtr<Syntax> &instanceSyntax) const
{
  instanceSyntax = instanceSyntax_;
  prologSyntax = prologSyntax_;
  sd = sd_;
}

void GroveImpl::finishProlog()
{
  if (root_->prolog)
    addBarrier();
  tailPtr_ = 0;
}

void GroveImpl::finishDocumentElement()
{
  // Be robust in the case of erroneous documents.
  if (root_->epilog == 0) {
    addBarrier();
    tailPtr_ = &root_->epilog;
  }
}

void GroveImpl::addBarrier()
{
  if (freePtr_) {
    (void) new (freePtr_) ForwardingChunk(0, 0);
    if (nFree_ <= sizeof(ForwardingChunk)) {
      nFree_ = 0;
      freePtr_ = 0;
    }
    else {
      nFree_ -= sizeof(ForwardingChunk);
      freePtr_ += sizeof(ForwardingChunk);
    }
  }
}

void GroveImpl::setComplete()
{
  addBarrier();
  mutexPtr_ = 0;
  completeLimit_ = 0;
  completeLimitWithLocChunkAfter_ = 0;
  if (pendingData_ && tailPtr_)
    *tailPtr_ = pendingData_;
  tailPtr_ = 0; 
  pendingData_ = 0;
  complete_ = 1;
  moreNodesCondition_.set();
}

void GroveImpl::addDefaultedEntity(const ConstPtr<Entity> &entity)
{
  Mutex::Lock lock(mutexPtr_);
  // We need a table of ConstPtr<Entity> but we don't have one.
  defaultedEntityTable_.insert((Entity *)entity.pointer());
}

const Entity *GroveImpl::lookupDefaultedEntity(const StringC &name) const
{
  Mutex::Lock lock(mutexPtr_);
  return defaultedEntityTable_.lookupTemp(name);
}

Dtd::ConstEntityIter GroveImpl::defaultedEntityIter() const
{
  ASSERT(complete());
  return Dtd::ConstEntityIter(defaultedEntityTable_);
}

Boolean GroveImpl::maybeMoreSiblings1(const ParentChunk *chunk) const
{
  for (const ParentChunk *open = origin_; open; open = open->origin)
    if (open == chunk)
      return 1;
  // for multi-thread case
  return tailPtr_ == &chunk->nextSibling || chunk->nextSibling != 0;
}

void *GroveImpl::allocFinish(size_t n)
{
  if (++nBlocksThisSizeAlloced_ >= maxBlocksPerSize) {
     blockAllocSize_ *= 2;
     nBlocksThisSizeAlloced_ = 0;
  }
  size_t allocSize = n + (sizeof(ForwardingChunk) + sizeof(BlockHeader));
  if (allocSize < blockAllocSize_) {
    nFree_ = blockAllocSize_ - allocSize;
    allocSize = blockAllocSize_;
  }
  else
    nFree_ = 0;
  *blockTailPtr_ = new (::operator new(allocSize)) BlockHeader;
  char *chunkStart = (char *)(*blockTailPtr_ + 1);
  blockTailPtr_ = &(*blockTailPtr_)->next;
  if (freePtr_)
    (void)new (freePtr_) ForwardingChunk((const Chunk *)chunkStart, origin_);
  freePtr_ = chunkStart + n;
  return chunkStart;
}

AccessResult ChunkNode::getLocation(Location &loc) const
{
  const Origin *origin = grove()->currentLocOrigin();
  for (const Chunk *p = chunk_->after(); p; p = p->after()) {
    if (p == grove()->completeLimitWithLocChunkAfter()) {
      while (!p->getLocOrigin(origin)) {
	p = p->after();
	ASSERT(p != 0);
      }
      break;
    }
    if (p == grove()->completeLimit() || p->getLocOrigin(origin))
      break;
  }
  if (!origin)
    return accessNull;
  loc = Location(new GroveImplProxyOrigin(grove(), origin), chunk_->locIndex);
  return accessOK;
}

void GroveImpl::storeLocOrigin(const ConstPtr<Origin> &locOrigin)
{
  LocOriginChunk *chunk
    = new (allocChunk(sizeof(LocOriginChunk)))
	  LocOriginChunk(currentLocOrigin_);
  chunk->origin = origin_;
  completeLimitWithLocChunkAfter_ = completeLimit_;
  nChunksSinceLocOrigin_ = 0;
  if (locOrigin.pointer() == currentLocOrigin_)
    return;
  if (currentLocOrigin_
      && locOrigin == currentLocOrigin_->parent().origin()) {
    // Don't need to store it.
    currentLocOrigin_ = locOrigin.pointer();
    return;
  }
  currentLocOrigin_ = locOrigin.pointer();
  if (locOrigin.isNull())
    return;
  origins_.push_back(locOrigin);
}

AccessResult GroveImpl::proxifyLocation(const Location &loc, Location &ret) const
{
  if (loc.origin().isNull())
    return accessNull;
  ret = Location(new GroveImplProxyOrigin(this, loc.origin().pointer()),
		 loc.index());
  return accessOK;
}

NodeListPtr AttributesNamedNodeList::nodeList() const
{
  const AttributeDefinitionList *defList = attDefList();
  if (!defList || defList->size() == 0)
    return new BaseNodeList;
  else
    return new SiblingNodeList(makeAttributeAsgnNode(grove(), 0));
}

AccessResult
AttributesNamedNodeList::namedNodeU(const StringC &str, NodePtr &ptr) const
{
  const AttributeDefinitionList *defList = attDefList();
  if (defList) {
    for (size_t i = 0; i < defList->size(); i++)
      if (defList->def(i)->name() == str) {
        ptr.assign(makeAttributeAsgnNode(grove(), i));
	return accessOK;
      }
  }
  return accessNull;
}

void SgmlDocumentNode::accept(NodeVisitor &visitor)
{
  visitor.sgmlDocument(*this);
}

AccessResult SgmlDocumentNode::getSgmlConstants(NodePtr &ptr) const
{
  ptr.assign(new SgmlConstantsNode(grove()));
  return accessOK;
}

AccessResult SgmlDocumentNode::getApplicationInfo(GroveString &str) const
{
  const StringC *appinfo;
  while (!grove()->getAppinfo(appinfo))
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  if (!appinfo)
    return accessNull;
  setString(str, *appinfo);
  return accessOK;
}

AccessResult SgmlDocumentNode::getDocumentElement(NodePtr &ptr) const
{
  while (chunk()->documentElement == 0) {
    if (grove()->complete()) {
      // Just in case another thread crept 
      if (chunk()->documentElement)
	break;
      return accessNull;
    }
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  return chunk()->documentElement->setNodePtrFirst(ptr, this);
}

AccessResult SgmlDocumentNode::getProlog(NodeListPtr &ptr) const
{
  while (chunk()->prolog == 0) {
    if (chunk()->documentElement || grove()->complete())
      break;
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  if (chunk()->prolog == 0)
    ptr.assign(new BaseNodeList);
  else {
    NodePtr tem;
    chunk()->prolog->setNodePtrFirst(tem, this);
    ptr.assign(new SiblingNodeList(tem));
  }
  return accessOK;
}

AccessResult SgmlDocumentNode::getEpilog(NodeListPtr &ptr) const
{
  while (chunk()->epilog == 0) {
    if (grove()->complete())
      break;
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  if (chunk()->epilog == 0)
    ptr.assign(new BaseNodeList);
  else {
    NodePtr tem;
    chunk()->epilog->setNodePtrFirst(tem, this);
    ptr.assign(new SiblingNodeList(tem));
  }
  return accessOK;
}

AccessResult SgmlDocumentNode::getElements(NamedNodeListPtr &ptr) const
{
  while (!grove()->root()->documentElement) {
    if (grove()->complete()) {
      if (grove()->root()->documentElement)
	break;
      return accessNull;
    }
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  if (!grove()->generalSubstTable())
    return accessNull;
  ptr.assign(new ElementsNamedNodeList(grove()));
  return accessOK;
}

AccessResult SgmlDocumentNode::getEntities(NamedNodeListPtr &ptr) const
{
  while (!grove()->governingDtd()) {
    if (grove()->complete()) {
      if (grove()->governingDtd())
	break;
      return accessNull;
    }
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  ptr.assign(new DocEntitiesNamedNodeList(grove()));
  return accessOK;
}

AccessResult SgmlDocumentNode::getDefaultedEntities(NamedNodeListPtr &ptr) const
{
  while (!grove()->complete())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  ptr.assign(new DefaultedEntitiesNamedNodeList(grove()));
  return accessOK;
}

AccessResult SgmlDocumentNode::getGoverningDoctype(NodePtr &ptr) const
{
  while (!grove()->governingDtd()) {
    if (grove()->complete()) {
      if (grove()->governingDtd())
	break;
      return accessNull;
    }
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  ptr.assign(new DocumentTypeNode(grove(), grove()->governingDtd()));
  return accessOK;
}

AccessResult SgmlDocumentNode::getDoctypesAndLinktypes(NamedNodeListPtr &ptr) const
{
  while (!grove()->governingDtd()) {
    if (grove()->complete()) {
      if (grove()->governingDtd())
	break;
      return accessNull;
    }
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  ptr.assign(new DoctypesAndLinktypesNamedNodeList(grove()));
  return accessOK;
}

AccessResult SgmlDocumentNode::getMessages(NodeListPtr &ptr) const
{
  while (grove()->messageList() == 0) {
    if (grove()->complete())
      break;
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  if (grove()->messageList()) {
    NodePtr tem(new MessageNode(grove(), grove()->messageList()));
    ptr.assign(new SiblingNodeList(tem));
  }
  else
    ptr.assign(new BaseNodeList);
  return accessOK;
}

AccessResult SgmlDocumentNode::getSd(ConstPtr<Sd> &sd,
				     ConstPtr<Syntax> &prologSyntax,
				     ConstPtr<Syntax> &instanceSyntax) const
{
  while (!grove()->complete()) {
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  grove()->getSd(sd, prologSyntax, instanceSyntax);
  if (!sd.isNull() && !prologSyntax.isNull() && !instanceSyntax.isNull())
    return accessOK;
  return accessNull;
}

AccessResult
SgmlDocumentChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const
{
  ptr.assign(new SgmlDocumentNode(node->grove(), this));
  return accessOK;
}
 
DocumentTypeNode::DocumentTypeNode(const GroveImpl *grove, const Dtd *dtd)
: BaseNode(grove), dtd_(dtd)
{
}

AccessResult DocumentTypeNode::nextChunkSibling(NodePtr &) const
{
  return accessNull;
}

AccessResult DocumentTypeNode::getName(GroveString &str) const
{
  setString(str, dtd_->name());
  return accessOK;
}

AccessResult DocumentTypeNode::getGoverning(bool &governing) const
{
  governing = dtd_->isBase();
  return accessOK;
}

AccessResult DocumentTypeNode::getGeneralEntities(NamedNodeListPtr &ptr) const
{
  ptr.assign(new GeneralEntitiesNamedNodeList(grove(), dtd_));
  return accessOK;
}

AccessResult DocumentTypeNode::getNotations(NamedNodeListPtr &ptr) const
{
  ptr.assign(new NotationsNamedNodeList(grove(), dtd_));
  return accessOK;
}

AccessResult DocumentTypeNode::getOrigin(NodePtr &ptr) const
{
  ptr.assign(new SgmlDocumentNode(grove(), grove()->root()));
  return accessOK;
}

void DocumentTypeNode::accept(NodeVisitor &visitor)
{
  visitor.documentType(*this);
}

bool DocumentTypeNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool DocumentTypeNode::same2(const DocumentTypeNode *node) const
{
  return dtd_ == node->dtd_;
}

SgmlConstantsNode::SgmlConstantsNode(const GroveImpl *grove)
: BaseNode(grove)
{
}

AccessResult SgmlConstantsNode::getOrigin(NodePtr &ptr) const
{
  ptr.assign(new SgmlDocumentNode(grove(), grove()->root()));
  return accessOK;
}

void SgmlConstantsNode::accept(NodeVisitor &visitor)
{
  visitor.sgmlConstants(*this);
}

bool SgmlConstantsNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool SgmlConstantsNode::same2(const SgmlConstantsNode *) const
{
  return 1;
}

MessageNode::MessageNode(const GroveImpl *grove, const MessageItem *item)
: BaseNode(grove), item_(item)
{
}

AccessResult MessageNode::getOrigin(NodePtr &ptr) const
{
  ptr.assign(new SgmlDocumentNode(grove(), grove()->root()));
  return accessOK;
}

AccessResult MessageNode::nextChunkSibling(NodePtr &ptr) const
{
  while (!item_->next()) {
    if (grove()->complete()) {
      if (item_->next())
	break;
      return accessNull;
    }
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  const MessageItem *p = item_->next();
  if (!p)
    return accessNull;
  ptr.assign(new MessageNode(grove(), p));
  return accessOK;
}

AccessResult MessageNode::firstSibling(NodePtr &ptr) const
{
  ptr.assign(new MessageNode(grove(), grove()->messageList()));
  return accessOK;
}

AccessResult MessageNode::siblingsIndex(unsigned long &n) const
{
  n = 0;
  for (const MessageItem *p = grove()->messageList(); p != item_; p = p->next())
    n++;
  return accessOK;
}

void MessageNode::accept(NodeVisitor &visitor)
{
  visitor.message(*this);
}

bool MessageNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool MessageNode::same2(const MessageNode *node) const
{
  return item_ == node->item_;
}

AccessResult MessageNode::getLocation(Location &loc) const
{
  return grove()->proxifyLocation(item_->loc(), loc);
}

AccessResult MessageNode::getText(GroveString &str) const
{
  setString(str, item_->text());
  return accessOK;
}

AccessResult MessageNode::getSeverity(Severity &severity) const
{
  severity = item_->severity();
  return accessOK;
}

AccessResult ElementNode::nextChunkSibling(NodePtr &ptr) const
{
  while (!chunk()->nextSibling) {
    if (!grove()->maybeMoreSiblings(chunk())) {
      // Allow for the possibility of invalid documents with elements in the epilog.
      if ((const Chunk *)chunk() == grove()->root()->documentElement)
	return accessNotInClass;
      else
        return accessNull;
    }
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  return chunk()->nextSibling->setNodePtrFirst(ptr, this);
}

// This is a duplicate of ChunkNode::nextChunkAfter
// to take advantage of overloaded setNodePtrFirst.

AccessResult ElementNode::nextChunkAfter(NodePtr &nd) const
{
  const Chunk *p = chunk_->after();
  while (p == grove()->completeLimit())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  return p->setNodePtrFirst(nd, this);
}

AccessResult ElementChunk::getFollowing(const GroveImpl *grove,
					const Chunk *&f,
					unsigned long &n) const
{
  while (!nextSibling) {
    if (!grove->maybeMoreSiblings(this)) {
      if ((const Chunk *)origin == grove->root())
	return accessNotInClass;
      else
        return accessNull;
    }
    if (!grove->waitForMoreNodes())
      return accessTimeout;
  }
  f = nextSibling;
  n = 1;
  return accessOK;
}

AccessResult ElementNode::firstChild(NodePtr &ptr) const
{
  const Chunk *p = chunk()->after();
  while (p == grove()->completeLimit()) {
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  if ((const Chunk *)(p->origin) == chunk())
    return p->setNodePtrFirst(ptr, this);
  return accessNull;
}

AccessResult ElementNode::attributeRef(unsigned long n, NodePtr &ptr) const
{
  const AttributeDefinitionList *defList = chunk()->attDefList();
  if (!defList || n >= defList->size())
    return accessNull;
  ptr.assign(new ElementAttributeAsgnNode(grove(), size_t(n), chunk()));
  return accessOK;
}

AccessResult ElementNode::getAttributes(NamedNodeListPtr &ptr) const
{
  ptr.assign(new ElementAttributesNamedNodeList(grove(), chunk()));
  return accessOK;
}

AccessResult ElementNode::getGi(GroveString &str) const
{
  setString(str, chunk()->type->name());
  return accessOK;
}

bool ElementNode::hasGi(GroveString str) const
{
  const StringC &gi = chunk()->type->name();
  size_t len = gi.size();
  if (len != str.size())
    return 0;
  const SubstTable<Char> *subst = grove()->generalSubstTable();
  if (!subst)
    return 0;
  for (size_t i = 0; i < len; i++)
    if ((*subst)[str[i]] != gi[i])
      return 0;
  return 1;
}

AccessResult ElementNode::getId(GroveString &str) const
{
  const StringC *id = chunk()->id();
  if (!id)
    return accessNull;
  setString(str, *id);
  return accessOK;
}

AccessResult ElementNode::elementIndex(unsigned long &i) const
{
  i = chunk()->elementIndex;
  return accessOK;
}

AccessResult ElementNode::getContent(NodeListPtr &ptr) const
{
  return children(ptr);
}

AccessResult ElementNode::getMustOmitEndTag(bool &b) const
{
  b = chunk()->mustOmitEndTag();
  return accessOK;
}

AccessResult ElementNode::getIncluded(bool &b) const
{
  b = chunk()->included();
  return accessOK;
}

void ElementNode::accept(NodeVisitor &visitor)
{
  visitor.element(*this);
}

ElementChunk *
ElementNode::makeAttElementChunk(GroveImpl &grove,
				 const StartElementEvent &event,
				 Boolean &hasId)
{
  const AttributeList &atts = event.attributes();
  size_t nAtts = atts.size();
  while (nAtts > 0 && !atts.specified(nAtts - 1) && !atts.current(nAtts - 1))
    nAtts--;
  ElementChunk *chunk;
  void *mem
    = grove.allocChunk(sizeof(AttElementChunk) + nAtts * sizeof(AttributeValue *));
  if (event.included()) {
    // Workaround VC++ 4.1 bug.
    AttElementChunk *tem = new (mem) IncludedAttElementChunk(nAtts);
    chunk = tem;
  }
  else
    chunk = new (mem) AttElementChunk(nAtts);
  const AttributeValue **values
    = (const AttributeValue **)(((AttElementChunk *)chunk) + 1);
  const AttributeDefinitionList *defList
    = event.elementType()->attributeDef().pointer();
  unsigned idIndex;
  if (atts.idIndex(idIndex) && atts.specified(idIndex) && atts.value(idIndex))
    hasId = 1;
  else
    hasId = 0;
  for (size_t i = 0; i < nAtts; i++) {
    if (atts.specified(i) || atts.current(i)) {
      grove.storeAttributeValue(atts.valuePointer(i));
      values[i] = atts.value(i);
    }
    else {
      // If we stored a reference to the implied attribute value in the
      // Dtd then it would be safe just to use atts.value(i) here.
      // But that would mean we couldn't conveniently tell whether it
      // was specified or implied.
      values[i] = defList->def(i)->defaultValue(grove.impliedAttributeValue());
    }
  }
  return chunk;
}

const Chunk *AttElementChunk::after() const
{
  return (const Chunk *)((char *)(this + 1)
			 + (sizeof(const AttributeValue *) * nAtts));
}

const AttributeValue *
AttElementChunk::attributeValue(size_t attIndex, const GroveImpl &grove)
     const
{
  if (attIndex < nAtts)
    return ((const AttributeValue **)(this + 1))[attIndex];
  else
    return ElementChunk::attributeValue(attIndex, grove);
}

const StringC *AttElementChunk::id() const
{
  size_t i = ElementChunk::attDefList()->idIndex();
  if (i == size_t(-1) || i >= nAtts)
    return 0;
  const AttributeValue *av = ((const AttributeValue **)(this + 1))[i];
  if (!av)
    return 0;
  const Text *t = av->text();
  if (!t)
    return 0;
  return &t->string();
}

Boolean AttElementChunk::mustOmitEndTag() const
{
  if (ElementChunk::mustOmitEndTag())
    return 1;
  const AttributeDefinitionList *adl = ElementChunk::attDefList();
  size_t nAtts = adl->size();
  const AttributeValue **atts = (const AttributeValue **)(this + 1);
  for (size_t i = 0; i < nAtts; i++)
    if (adl->def(i)->isConref() && atts[i] && atts[i]->text())
      return 1;
  return 0;
}

const Chunk *ElementChunk::after() const
{
  return this + 1;
}

const AttributeValue *
ElementChunk::attributeValue(size_t attIndex, const GroveImpl &grove) const
{
  return attDefList()->def(attIndex)->defaultValue(grove.impliedAttributeValue());
}

Boolean ElementChunk::mustOmitEndTag() const
{
  return type->definition()->declaredContent() == ElementDefinition::empty;
}

Boolean IncludedElementChunk::included() const
{
  return 1;
}

Boolean IncludedAttElementChunk::included() const
{
  return 1;
}

Boolean ElementChunk::included() const
{
  return 0;
}

AccessResult
ElementChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const
{
  ptr.assign(new ElementNode(node->grove(), this));
  return accessOK;
}

AccessResult
ElementChunk::setNodePtrFirst(NodePtr &ptr, const ElementNode *node) const
{
  if (node->canReuse(ptr))
    ((ElementNode *)node)->reuseFor(this);
  else
    ptr.assign(new ElementNode(node->grove(), this));
  return accessOK;
}

AccessResult
ElementChunk::setNodePtrFirst(NodePtr &ptr, const DataNode *node) const
{
  ptr.assign(new ElementNode(node->grove(), this));
  return accessOK;
}

ElementAttributeOrigin::ElementAttributeOrigin(const ElementChunk *chunk)
: chunk_(chunk)
{
}

const AttributeDefinitionList *ElementAttributeOrigin::attDefList() const
{
  return chunk_->attDefList();
}


const AttributeValue *
ElementAttributeOrigin::attributeValue(size_t attIndex, const GroveImpl &grove) const
{
  return chunk_->attributeValue(attIndex, grove);
}


AccessResult
ElementAttributeOrigin::setNodePtrAttributeOrigin(NodePtr &ptr,
						  const BaseNode *node) const
{
  return chunk_->setNodePtrFirst(ptr, node);
}
  

Node *ElementAttributeOrigin
::makeCdataAttributeValueNode(const GroveImpl *grove,
			      const AttributeValue *value,
			      size_t attIndex,
			      const TextIter &iter,
			      size_t charIndex) const
{
  return new ElementCdataAttributeValueNode(grove, value, attIndex, iter,
					    charIndex, chunk_);
}


Node *ElementAttributeOrigin
::makeAttributeValueTokenNode(const GroveImpl *grove,
			      const TokenizedAttributeValue *value,
			      size_t attIndex,
			      size_t tokenIndex) const
{
  return new ElementAttributeValueTokenNode(grove, value, attIndex,
					    tokenIndex, chunk_);
}

Node *ElementAttributeOrigin
::makeAttributeAsgnNode(const GroveImpl *grove,
			size_t attIndex) const
{
  return new ElementAttributeAsgnNode(grove, attIndex, chunk_);
}

const void *ElementAttributeOrigin::attributeOriginId() const
{
  return chunk_;
}

bool DataNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool DataNode::same2(const DataNode *node) const
{
  return chunk_ == node->chunk_ && index_ == node->index_;
}

bool DataNode::chunkContains(const Node &node) const
{
  if (!sameGrove(node))
    return 0;
  return ((const BaseNode &)node).inChunk(this);
}

bool DataNode::inChunk(const DataNode *node) const
{
  return chunk_ == node->chunk_ && index_ >= node->index_;
}

AccessResult DataNode::charChunk(const SdataMapper &, GroveString &str) const
{
  str.assign(chunk()->data() + index_, chunk()->size - index_);
  return accessOK;
}

void DataNode::accept(NodeVisitor &visitor)
{
  visitor.dataChar(*this);
}

unsigned long DataNode::hash() const
{
  return secondHash(ChunkNode::hash() + index_);
}

AccessResult DataNode::getNonSgml(unsigned long &) const
{
  return accessNull;
}

AccessResult DataNode::nextSibling(NodePtr &ptr) const
{
  if (index_ + 1 < chunk()->size) {
    if (canReuse(ptr))
      ((DataNode *)this)->index_ += 1;
    else
      ptr.assign(new DataNode(grove(), chunk(), index_ + 1));
    return accessOK;
  }
  return DataNode::nextChunkSibling(ptr);
}

AccessResult DataNode::nextChunkAfter(NodePtr &nd) const
{
  const Chunk *p = chunk_->after();
  while (p == grove()->completeLimit())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  return p->setNodePtrFirst(nd, this);
}

AccessResult DataNode::followSiblingRef(unsigned long i, NodePtr &ptr) const
{
  if (i < chunk()->size - index_ - 1) {
    if (canReuse(ptr))
      ((DataNode *)this)->index_ += 1 + size_t(i);
    else
      ptr.assign(new DataNode(grove(), chunk(), index_ + size_t(i) + 1));
    return accessOK;
  }
  return ChunkNode::followSiblingRef(i - (chunk()->size - index_ - 1), ptr);
}

AccessResult DataNode::siblingsIndex(unsigned long &i) const
{
  AccessResult ret = ChunkNode::siblingsIndex(i);
  if (ret == accessOK)
    i += index_;
  return ret;
}

AccessResult DataNode::getLocation(Location &loc) const
{
  AccessResult ret = ChunkNode::getLocation(loc);
  if (ret == accessOK)
    loc += index_;
  return ret;
}

AccessResult DataChunk::getFollowing(const GroveImpl *grove,
				     const Chunk *&f,
				     unsigned long &i) const
{
  // We could call Chunk::getFollowing to do most of
  // the work, but that would cost us a couple of extra
  // virtual function calls.
  const Chunk *p = CharsChunk::after();
  while (p == grove->completeLimit())
    if (!grove->waitForMoreNodes())
      return accessTimeout;
  if (p->origin != origin)
    return accessNull;
  i = size;
  f = p;
  return accessOK;
}

AccessResult DataChunk::setNodePtrFirst(NodePtr &ptr,
					const BaseNode *node) const
{
  ptr.assign(new DataNode(node->grove(), this, 0));
  return accessOK;
}

// This just saves us a virtual function call in a common case
AccessResult DataChunk::setNodePtrFirst(NodePtr &ptr,
					const ElementNode *node) const
{
  ptr.assign(new DataNode(node->grove(), this, 0));
  return accessOK;
}

AccessResult DataChunk::setNodePtrFirst(NodePtr &ptr, const DataNode *node)
     const
{
  if (node->canReuse(ptr))
    ((DataNode *)node)->reuseFor(this, 0);
  else
    ptr.assign(new DataNode(node->grove(), this, 0));
  return accessOK;
}

AccessResult PiNode::getSystemData(GroveString &str) const
{
  str.assign(chunk()->data(), chunk()->size);
  return accessOK;
}

void PiNode::add(GroveImpl &grove, const PiEvent &event)
{
  const Entity *entity = event.entity();
  if (entity)
    PiEntityNode::add(grove, entity, event.location());
  else {
    grove.setLocOrigin(event.location().origin());
    size_t dataLen = event.dataLength();
    void *mem = grove.allocChunk(CharsChunk::allocSize(dataLen));
    PiChunk *chunk;
    if (grove.haveRootOrigin()) {
      if (grove.root()->documentElement)
	chunk = new (mem) EpilogPiChunk;
      else
	chunk = new (mem) PrologPiChunk;
    }
    else
      chunk = new (mem) PiChunk;
    chunk->size = dataLen;
    chunk->locIndex = event.location().index();
    memcpy(chunk + 1, event.data(), dataLen * sizeof(Char));
    grove.appendSibling(chunk);
  }
}

AccessResult PiChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const
{
  ptr.assign(new PiNode(node->grove(), this));
  return accessOK;
}

AccessResult PrologPiChunk::getFirstSibling(const GroveImpl *grove, const Chunk *&p) const
{
  p = grove->root()->prolog;
  return accessOK;
}

AccessResult EpilogPiChunk::getFirstSibling(const GroveImpl *grove, const Chunk *&p) const
{
  p = grove->root()->epilog;
  return accessOK;
}

AccessResult SdataNode::charChunk(const SdataMapper &mapper, GroveString &str) const
{
  const StringC &name = chunk()->entity->name();
  const StringC &text = chunk()->entity->asInternalEntity()->string();
  Char *cp = (Char *)&c_;
  if (mapper.sdataMap(GroveString(name.data(), name.size()), GroveString(text.data(), text.size()), *cp)) {
    str.assign(&c_, 1);
    return accessOK;
  }
  else
    return accessNull;
}

AccessResult SdataNode::getSystemData(GroveString &str) const
{
  setString(str, chunk()->entity->asInternalEntity()->string());
  return accessOK;
}

void SdataNode::add(GroveImpl &grove, const SdataEntityEvent &event)
{
  const Location &loc = event.location().origin()->parent();
  grove.setLocOrigin(loc.origin());
  SdataChunk *chunk = new (grove.allocChunk(sizeof(SdataChunk))) SdataChunk;
  chunk->entity = event.entity();
  chunk->locIndex = loc.index();
  grove.appendSibling(chunk);
}

AccessResult SdataChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node)
     const
{
  ptr.assign(new SdataNode(node->grove(), this));
  return accessOK;
}

AccessResult NonSgmlChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node)
     const
{
  ptr.assign(new NonSgmlNode(node->grove(), this));
  return accessOK;
}

AccessResult NonSgmlNode::getNonSgml(unsigned long &n) const
{
  n = chunk()->c;
  return accessOK;
}

AccessResult NonSgmlNode::charChunk(const SdataMapper &, GroveString &) const
{
  return accessNull;
}

void NonSgmlNode::add(GroveImpl &grove, const NonSgmlCharEvent &event)
{
  grove.setLocOrigin(event.location().origin());
  NonSgmlChunk *chunk = new (grove.allocChunk(sizeof(NonSgmlChunk))) NonSgmlChunk;
  chunk->c = event.character();
  chunk->locIndex = event.location().index();
  grove.appendSibling(chunk);
}

void ExternalDataNode::add(GroveImpl &grove, const ExternalDataEntityEvent &event)
{
  grove.setLocOrigin(event.location().origin());
  ExternalDataChunk *chunk = new (grove.allocChunk(sizeof(ExternalDataChunk))) ExternalDataChunk;
  chunk->entity = event.entity();
  chunk->locIndex = event.location().index();
  grove.appendSibling(chunk);
}

AccessResult ExternalDataChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node)
     const
{
  ptr.assign(new ExternalDataNode(node->grove(), this));
  return accessOK;
}

void SubdocNode::add(GroveImpl &grove, const SubdocEntityEvent &event)
{
  grove.setLocOrigin(event.location().origin());
  SubdocChunk *chunk = new (grove.allocChunk(sizeof(SubdocChunk))) SubdocChunk;
  chunk->entity = event.entity();
  chunk->locIndex = event.location().index();
  grove.appendSibling(chunk);
}

AccessResult SubdocChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node)
     const
{
  ptr.assign(new SubdocNode(node->grove(), this));
  return accessOK;
}

AccessResult PiEntityNode::getSystemData(GroveString &str) const
{
  setString(str, chunk()->entity->asInternalEntity()->string());
  return accessOK;
}

void PiEntityNode::add(GroveImpl &grove, const Entity *entity,
		       const Location &loc)
{
  // FIXME use parent?
  grove.setLocOrigin(loc.origin());
  PiEntityChunk *chunk = new (grove.allocChunk(sizeof(PiEntityChunk))) PiEntityChunk;
  chunk->entity = entity;
  chunk->locIndex = loc.index();
  grove.appendSibling(chunk);
}

AccessResult PiEntityChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node)
 const
{
  ptr.assign(new PiEntityNode(node->grove(), this));
  return accessOK;
}

AccessResult EntityRefNode::getEntity(NodePtr &ptr) const
{
  ptr.assign(new EntityNode(grove(), chunk()->entity));
  return accessOK;
}

AccessResult EntityRefNode::getEntityName(GroveString &str) const
{
  setString(str, chunk()->entity->name());
  return accessOK;
}

AttributeAsgnNode::AttributeAsgnNode(const GroveImpl *grove,
				     size_t attIndex)
: BaseNode(grove),
  attIndex_(attIndex)
{
}

AccessResult ChunkNode::nextChunkSibling(NodePtr &ptr) const
{
  // The forwarding chunk has origin = 0, so it will stop
  // the iteration before after() can return 0.
  const Chunk *p = chunk_->after();
  while (p == grove()->completeLimit())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  if (p->origin != chunk_->origin)
    return accessNull;
  return p->setNodePtrFirst(ptr, this);
}

AccessResult ChunkNode::nextChunkAfter(NodePtr &nd) const
{
  const Chunk *p = chunk_->after();
  while (p == grove()->completeLimit())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  return p->setNodePtrFirst(nd, this);
}

AccessResult ChunkNode::firstSibling(NodePtr &ptr) const
{
  const Chunk *first;
  AccessResult ret = chunk_->getFirstSibling(grove(), first);
  if (ret != accessOK)
    return ret;
  return first->setNodePtrFirst(ptr, this);
}

AccessResult ChunkNode::siblingsIndex(unsigned long &i) const
{
  const Chunk *p;
  AccessResult ret = chunk_->getFirstSibling(grove(), p);
  if (ret != accessOK)
    return ret;
  i = 0;
  while (p != chunk_) {
    unsigned long tem;
    if (p->getFollowing(grove(), p, tem) != accessOK)
      CANNOT_HAPPEN();
    i += tem;
  }
  return accessOK;
}

AccessResult ChunkNode::followSiblingRef(unsigned long i, NodePtr &ptr) const
{
  const Chunk *p;
  unsigned long count;
  AccessResult ret = chunk()->getFollowing(grove(), p, count);
  if (ret != accessOK)
    return ret;
  while (i > 0) {
    const Chunk *lastP = p;
    ret = p->getFollowing(grove(), p, count);
    if (ret == accessOK && count <= i)
      i -= count;
    else if (ret == accessOK || ret == accessNull) {
      lastP->setNodePtrFirst(ptr, this);
      return ptr->followSiblingRef(i - 1, ptr);
    }
    else
      return ret;
  }
  return p->setNodePtrFirst(ptr, this);
}

AccessResult AttributeAsgnNode::getOrigin(NodePtr &ptr) const
{
  return setNodePtrAttributeOrigin(ptr, this);
}

AccessResult AttributeAsgnNode::getName(GroveString &str) const
{
  setString(str, attDefList()->def(attIndex_)->name());
  return accessOK;
}

AccessResult AttributeAsgnNode::getImplied(bool &implied) const
{
  const AttributeValue *value = attributeValue(attIndex_, *grove());
  implied = (value != 0 && value->text() == 0);
  return accessOK;
}

AccessResult AttributeAsgnNode::getValue(NodeListPtr &ptr) const
{
  return children(ptr);
}

AccessResult AttributeAsgnNode::nextChunkSibling(NodePtr &ptr) const
{
  return followSiblingRef(0, ptr);
}
 
AccessResult AttributeAsgnNode::followSiblingRef(unsigned long i, NodePtr &ptr) const
{
  // Do it like this to avoid overflow.
  if (i >= attDefList()->size() - attIndex_ - 1)
    return accessNull;
  if (canReuse(ptr))
    ((AttributeAsgnNode *)this)->attIndex_ += size_t(i) + 1;
  else
    ptr.assign(makeAttributeAsgnNode(grove(), attIndex_ + 1 + size_t(i)));
  return accessOK;
}

AccessResult AttributeAsgnNode::firstSibling(NodePtr &ptr) const
{
  if (canReuse(ptr))
    ((AttributeAsgnNode *)this)->attIndex_ = 0;
  else
    ptr.assign(makeAttributeAsgnNode(grove(), 0));
  return accessOK;
}

AccessResult AttributeAsgnNode::siblingsIndex(unsigned long &i) const
{
  i = attIndex_;
  return accessOK;
}
 
void AttributeAsgnNode::accept(NodeVisitor &visitor)
{
  visitor.attributeAssignment(*this);
}

AccessResult AttributeAsgnNode::firstChild(NodePtr &ptr) const
{
  const AttributeValue *value = attributeValue(attIndex_, *grove());
  if (value) {
    const Text *text;
    const StringC *str;
    switch (value->info(text, str)) {
    case AttributeValue::tokenized:
      ptr.assign(makeAttributeValueTokenNode(grove(),
					     (const TokenizedAttributeValue *)value,
	                                     attIndex_, 0));
      return accessOK;
    case AttributeValue::cdata:
      {
	TextIter iter(*text);
	if (!CdataAttributeValueNode::skipBoring(iter))
	  break;
	ptr.assign(makeCdataAttributeValueNode(grove(), value,
					       attIndex_, iter));
	return accessOK;
      }
    default:
      break;
    }
  }
  return accessNull;
}

AccessResult AttributeAsgnNode::children(NodeListPtr &ptr) const
{
  const AttributeValue *value = attributeValue(attIndex_, *grove());
  if (value) {
    const Text *text;
    const StringC *str;
    switch (value->info(text, str)) {
    case AttributeValue::tokenized:
      ptr.assign(new SiblingNodeList(makeAttributeValueTokenNode(grove(),
				     (const TokenizedAttributeValue *)value,
				     attIndex_, 0)));
      return accessOK;
    case AttributeValue::cdata:
      {
	TextIter iter(*text);
	if (!CdataAttributeValueNode::skipBoring(iter))
          ptr.assign(new BaseNodeList);
	else
	  ptr.assign(new SiblingNodeList(makeCdataAttributeValueNode(grove(), value,
								     attIndex_, iter)));
	return accessOK;
      }
    default:
      break;
    }
  }
  return accessNull;
}

AccessResult AttributeAsgnNode::getTokenSep(Char &ch) const
{
  const AttributeValue *value = attributeValue(attIndex_, *grove());
  if (!value)
    return accessNull;
  const Text *text;
  const StringC *str;
  if (value->info(text, str) != AttributeValue::tokenized)
    return accessNull;
  const TokenizedAttributeValue *tValue =
    (const TokenizedAttributeValue *)value;
  if (tValue->nTokens() <= 1)
    return accessNull;
  const Char *ptr;
  size_t len;
  tValue->token(0, ptr, len);
  // the character following the token is a space
  ch = ptr[len];
  return accessOK;
}

AccessResult AttributeAsgnNode::tokens(GroveString &s) const
{
  const AttributeValue *value = attributeValue(attIndex_, *grove());
  if (!value)
    return accessNull;
  const Text *text;
  const StringC *str;
  if (value->info(text, str) != AttributeValue::tokenized)
    return accessNull;
  setString(s, *str);
  return accessOK;
}

bool AttributeAsgnNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool AttributeAsgnNode::same2(const AttributeAsgnNode *node)
 const
{
  return (attributeOriginId() == node->attributeOriginId()
	  && attIndex_ == node->attIndex_);
}

unsigned long AttributeAsgnNode::hash() const
{
  unsigned long n = (unsigned long)attributeOriginId();
  return secondHash(n + attIndex_);
}

ElementAttributeAsgnNode
::ElementAttributeAsgnNode(const GroveImpl *grove, size_t attIndex,
			   const ElementChunk *chunk)
: AttributeAsgnNode(grove, attIndex), ElementAttributeOrigin(chunk)
{
}

EntityAttributeAsgnNode
::EntityAttributeAsgnNode(const GroveImpl *grove, size_t attIndex,
			  const ExternalDataEntity *entity)
: AttributeAsgnNode(grove, attIndex), EntityAttributeOrigin(entity)
{
}

CdataAttributeValueNode
::CdataAttributeValueNode(const GroveImpl *grove,
			  const AttributeValue *value,
			  size_t attIndex,
			  const TextIter &iter,
			  size_t charIndex)
: BaseNode(grove),
  value_(value),
  attIndex_(attIndex),
  iter_(iter),
  charIndex_(charIndex)
{
}

bool CdataAttributeValueNode::skipBoring(TextIter &iter)
{
  while (iter.valid()) {
    switch (iter.type()) {
    case TextItem::data:
    case TextItem::cdata:
    case TextItem::sdata:
      {
	size_t length;
	iter.chars(length);
	if (length > 0)
	  return 1;
      }
      // fall through
    default:
      iter.advance();
      break;
    }
  }
  return 0;
}

AccessResult CdataAttributeValueNode::getParent(NodePtr &ptr) const
{
  ptr.assign(makeAttributeAsgnNode(grove(), attIndex_));
  return accessOK;
}

AccessResult CdataAttributeValueNode::charChunk(const SdataMapper &mapper, GroveString &str) const
{
  if (iter_.type() == TextItem::sdata) {
    const Entity *entity = iter_.location().origin()->asEntityOrigin()->entity();
    const StringC &name = entity->name();
    const StringC &text = entity->asInternalEntity()->string();
    Char *cp = (Char *)&c_;
    if (mapper.sdataMap(GroveString(name.data(), name.size()), GroveString(text.data(), text.size()), *cp)) {
      str.assign(&c_, 1);
      return accessOK;
    }
    else
      return accessNull;
  }
  size_t len;
  const Char *s = iter_.chars(len);
  str.assign(s + charIndex_, len - charIndex_);
  return accessOK;
}

AccessResult CdataAttributeValueNode::siblingsIndex(unsigned long &n) const
{
  TextIter copy(iter_);
  size_t tem;
  const Char *iterChars = iter_.chars(tem);
  copy.rewind();
  skipBoring(copy);
  n = 0;
  while (copy.chars(tem) != iterChars) {
    if (copy.type() == TextItem::sdata)
      n += 1;
    else
      n += tem;
    copy.advance();
    skipBoring(copy);
  }
  n += charIndex_;
  return accessOK;
}

AccessResult CdataAttributeValueNode::getEntity(NodePtr &ptr) const
{
  if (iter_.type() != TextItem::sdata)
    return accessNotInClass;
  const Entity *entity
    = iter_.location().origin()->asEntityOrigin()->entity();
  ptr.assign(new EntityNode(grove(), entity));
  return accessOK;
}

AccessResult CdataAttributeValueNode::getEntityName(GroveString &str) const
{
  if (iter_.type() != TextItem::sdata)
    return accessNotInClass;
  const Entity *entity
    = iter_.location().origin()->asEntityOrigin()->entity();
  setString(str, entity->name());
  return accessOK;
}

AccessResult CdataAttributeValueNode::getSystemData(GroveString &str) const
{
  if (iter_.type() != TextItem::sdata)
    return accessNotInClass;
  size_t len;
  const Char *ptr = iter_.chars(len);
  str.assign(ptr, len);
  return accessOK;
}

AccessResult CdataAttributeValueNode::firstSibling(NodePtr &ptr) const
{
  TextIter copy(iter_);
  copy.rewind();
  skipBoring(copy);
  if (canReuse(ptr)) {
    CdataAttributeValueNode *node = (CdataAttributeValueNode *)this;
    node->iter_ = copy;
    node->charIndex_ = 0;
  }
  else
    ptr.assign(makeCdataAttributeValueNode(grove(), value_, attIndex_, copy));
  return accessOK;
}

AccessResult CdataAttributeValueNode::nextChunkSibling(NodePtr &ptr) const
{
  TextIter copy(iter_);
  copy.advance();
  if (!skipBoring(copy))
    return accessNull;
  if (canReuse(ptr)) {
    CdataAttributeValueNode *node = (CdataAttributeValueNode *)this;
    node->iter_ = copy;
    node->charIndex_ = 0;
  }
  else
    ptr.assign(makeCdataAttributeValueNode(grove(), value_, attIndex_, copy));
  return accessOK;
}

AccessResult CdataAttributeValueNode::nextSibling(NodePtr &ptr) const
{
  if (iter_.type() != TextItem::sdata) {
    size_t length;
    iter_.chars(length);
    if (charIndex_ + 1 < length) {
      if (canReuse(ptr))
	((CdataAttributeValueNode *)this)->charIndex_ += 1;
      else
	ptr.assign(makeCdataAttributeValueNode(grove(), value_,
					       attIndex_, iter_,
					       charIndex_ + 1));
      return accessOK;
    }
  }
  return CdataAttributeValueNode::nextChunkSibling(ptr);
}

AccessResult CdataAttributeValueNode::getLocation(Location &loc) const
{
  if (iter_.type() == TextItem::sdata)
    return grove()->proxifyLocation(iter_.location().origin()->parent(), loc);
  else
    return grove()->proxifyLocation(iter_.location(), loc);
}

void CdataAttributeValueNode::accept(NodeVisitor &visitor)
{
  if (iter_.type() == TextItem::sdata)
    visitor.sdata(*this);
  else
    visitor.dataChar(*this);
}

unsigned long CdataAttributeValueNode::hash() const
{
  unsigned long n;
  CdataAttributeValueNode::siblingsIndex(n);
  return secondHash(secondHash((unsigned long)attributeOriginId() + attIndex_) + n);
}

const ClassDef &CdataAttributeValueNode::classDef() const
{
  if (iter_.type() == TextItem::sdata)
    return ClassDef::sdata;
  else
    return ClassDef::dataChar;
}

bool CdataAttributeValueNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool CdataAttributeValueNode::same2(const CdataAttributeValueNode *node)
     const
{
  size_t tem;
  return (attributeOriginId() == node->attributeOriginId()
	  && attIndex_ == node->attIndex_
	  && charIndex_ == node->charIndex_
	  && iter_.chars(tem) == node->iter_.chars(tem));
}

bool CdataAttributeValueNode::chunkContains(const Node &node) const
{
  if (!sameGrove(node))
    return 0;
  return ((const BaseNode &)node).inChunk(this);
}

bool CdataAttributeValueNode::inChunk(const CdataAttributeValueNode *node) const
{
  size_t tem;
  return (attributeOriginId() == node->attributeOriginId()
          && attIndex_ == node->attIndex_
	  && iter_.chars(tem) == node->iter_.chars(tem)
	  && charIndex_ >= node->charIndex_);
}

ElementCdataAttributeValueNode
::ElementCdataAttributeValueNode(const GroveImpl *grove,
				 const AttributeValue *value,
				 size_t attIndex,
				 const TextIter &iter,
				 size_t charIndex,
				 const ElementChunk *chunk)
: CdataAttributeValueNode(grove, value, attIndex, iter, charIndex),
  ElementAttributeOrigin(chunk)
{
}

EntityCdataAttributeValueNode
::EntityCdataAttributeValueNode(const GroveImpl *grove,
				const AttributeValue *value,
				size_t attIndex,
				const TextIter &iter,
				size_t charIndex,
				const ExternalDataEntity *entity)
: CdataAttributeValueNode(grove, value, attIndex, iter, charIndex),
  EntityAttributeOrigin(entity)
{
}

AttributeValueTokenNode
::AttributeValueTokenNode(const GroveImpl *grove,
			  const TokenizedAttributeValue *value,
			  size_t attIndex, size_t tokenIndex)
: BaseNode(grove),
  value_(value),
  attIndex_(attIndex),
  tokenIndex_(tokenIndex)
{
}

AccessResult AttributeValueTokenNode::getParent(NodePtr &ptr) const
{
  ptr.assign(makeAttributeAsgnNode(grove(), attIndex_));
  return accessOK;
}

AccessResult AttributeValueTokenNode::nextChunkSibling(NodePtr &ptr) const
{
  return followSiblingRef(0, ptr);
}

AccessResult AttributeValueTokenNode::followSiblingRef(unsigned long i, NodePtr &ptr) const
{
  // Do it like this to avoid possibility of overflow
  if (i >= value_->nTokens() - tokenIndex_ - 1)
    return accessNull;
  if (canReuse(ptr)) {
    AttributeValueTokenNode *node = (AttributeValueTokenNode *)this;
    node->tokenIndex_ += size_t(i) + 1;
  }
  else
    ptr.assign(makeAttributeValueTokenNode(grove(), value_, attIndex_,
					   tokenIndex_ + size_t(i) + 1));
  return accessOK;
}

AccessResult AttributeValueTokenNode::firstSibling(NodePtr &ptr) const
{
  if (canReuse(ptr))
    ((AttributeValueTokenNode *)this)->tokenIndex_ = 0;
  else
    ptr.assign(makeAttributeValueTokenNode(grove(), value_, attIndex_, 0));
  return accessOK;
}

AccessResult AttributeValueTokenNode::siblingsIndex(unsigned long &i) const
{
  i = tokenIndex_;
  return accessOK;
}

AccessResult AttributeValueTokenNode::getToken(GroveString &str) const
{
  const Char *ptr;
  size_t len;
  value_->token(tokenIndex_, ptr, len);
  str.assign(ptr, len);
  return accessOK;
}

AccessResult AttributeValueTokenNode::getEntity(NodePtr &ptr) const
{
   if (!attDefList()->def(attIndex_)->isEntity())
    return accessNull;
  StringC token(value_->token(tokenIndex_));
  const Entity *entity = grove()->governingDtd()->lookupEntityTemp(0, token);
  if (!entity) {
    entity = grove()->lookupDefaultedEntity(token);
    if (!entity)
      return accessNull;
  }
  ptr.assign(new EntityNode(grove(), entity));
  return accessOK;
}

AccessResult AttributeValueTokenNode::getNotation(NodePtr &ptr) const
{
  if (!attDefList()->def(attIndex_)->isNotation())
    return accessNull;
  StringC token(value_->token(tokenIndex_));
  const Notation *notation = grove()->governingDtd()->lookupNotationTemp(token);
  if (!notation)
    return accessNull;
  ptr.assign(new NotationNode(grove(), notation));
  return accessOK;
}

AccessResult AttributeValueTokenNode::getReferent(NodePtr &ptr) const
{
  if (!attDefList()->def(attIndex_)->isIdref())
    return accessNull;
  StringC token(value_->token(tokenIndex_));
  for (;;) {
    Boolean complete = grove()->complete();
    const ElementChunk *element = grove()->lookupElement(token);
    if (element) {
      ptr.assign(new ElementNode(grove(), element));
      break;
    }
    if (complete)
      return accessNull;
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  return accessOK;
}

void AttributeValueTokenNode::accept(NodeVisitor &visitor)
{
  visitor.attributeValueToken(*this);
}

unsigned long AttributeValueTokenNode::hash() const
{
  return secondHash(secondHash((unsigned long)attributeOriginId() + attIndex_) + tokenIndex_);
}

bool AttributeValueTokenNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool AttributeValueTokenNode::same2(const AttributeValueTokenNode *node) const
{
  return (attributeOriginId() == node->attributeOriginId()
	  && attIndex_ == node->attIndex_
	  && tokenIndex_ == node->tokenIndex_);
}

AccessResult AttributeValueTokenNode::getLocation(Location &loc) const
{
  const ConstPtr<Origin> *originP;
  Index index;
  if (!value_->tokenLocation(tokenIndex_, originP, index)
      && originP->pointer()) {
    loc = Location(new GroveImplProxyOrigin(grove(), originP->pointer()), index);
    return accessOK;
  }
  return accessNull;
}

ElementAttributeValueTokenNode
::ElementAttributeValueTokenNode(const GroveImpl *grove,
				 const TokenizedAttributeValue *value,
				 size_t attIndex,
				 size_t tokenIndex,
				 const ElementChunk *chunk)
: AttributeValueTokenNode(grove, value, attIndex, tokenIndex),
  ElementAttributeOrigin(chunk)
{
}

EntityAttributeValueTokenNode
::EntityAttributeValueTokenNode(const GroveImpl *grove,
				const TokenizedAttributeValue *value,
				size_t attIndex,
				size_t tokenIndex,
				const ExternalDataEntity *entity)
: AttributeValueTokenNode(grove, value, attIndex, tokenIndex),
  EntityAttributeOrigin(entity)
{
}

EntityNode::EntityNode(const GroveImpl *grove, const Entity *entity)
: BaseNode(grove), entity_(entity)
{
}

AccessResult EntityNode::getOrigin(NodePtr &ptr) const
{
  if (entity_->defaulted() && grove()->lookupDefaultedEntity(entity_->name()))
    ptr.assign(new SgmlDocumentNode(grove(), grove()->root()));
  else
    ptr.assign(new DocumentTypeNode(grove(), grove()->governingDtd()));
  return accessOK;
}

AccessResult EntityNode::getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const
{
  if (entity_->defaulted() && grove()->lookupDefaultedEntity(entity_->name()))
    name = ComponentName::idDefaultedEntities;
  else
    name = ComponentName::idGeneralEntities;
  return accessOK;
}

AccessResult EntityNode::getName(GroveString &str) const
{
  setString(str, entity_->name());
  return accessOK;
}

AccessResult EntityNode::getExternalId(NodePtr &ptr) const
{
  const ExternalEntity *x = entity_->asExternalEntity();
  if (!x)
    return accessNull;
  ptr.assign(new EntityExternalIdNode(grove(), x));
  return accessOK;
}

AccessResult EntityNode::getNotation(NodePtr &ptr) const
{
  const ExternalDataEntity *x = entity_->asExternalDataEntity();
  if (!x || !x->notation())
    return accessNull;
  ptr.assign(new NotationNode(grove(), x->notation()));
  return accessOK;
}

AccessResult EntityNode::getNotationName(GroveString &str) const
{
  const ExternalDataEntity *x = entity_->asExternalDataEntity();
  if (!x || !x->notation())
    return accessNull;
  setString(str, x->notation()->name());
  return accessOK;
}

AccessResult EntityNode::getText(GroveString &str) const
{
  const InternalEntity *i = entity_->asInternalEntity();
  if (!i)
    return accessNull;
  setString(str, i->string());
  return accessOK;
}

AccessResult EntityNode::getEntityType(EntityType &entityType) const
{
  switch (entity_->dataType()) {
  case EntityDecl::sgmlText:
    entityType = text;
    break;
  case EntityDecl::pi:
    entityType = pi;
    break;
  case EntityDecl::cdata:
    entityType = cdata;
    break;
  case EntityDecl::sdata:
    entityType = sdata;
    break;
  case EntityDecl::ndata:
    entityType = ndata;
    break;
  case EntityDecl::subdoc:
    entityType = subdocument;
    break;
  default:
    CANNOT_HAPPEN();
  }
  return accessOK;
}

AccessResult EntityNode::getDefaulted(bool &dflted) const
{
  dflted = entity_->defaulted();
  return accessOK;
}

AccessResult EntityNode::getAttributes(NamedNodeListPtr &ptr) const
{
  const ExternalDataEntity *x = entity_->asExternalDataEntity();
  if (!x)
    return accessNull;
  ptr.assign(new EntityAttributesNamedNodeList(grove(), x));
  return accessOK;
}

AccessResult EntityNode::attributeRef(unsigned long i, NodePtr &ptr) const
{
  const ExternalDataEntity *x = entity_->asExternalDataEntity();
  if (!x || i >= x->attributes().size())
    return accessNull;
  ptr.assign(new EntityAttributeAsgnNode(grove(), size_t(i), x));
  return accessOK;
}

AccessResult EntityNode::getLocation(Location &loc) const
{
  return grove()->proxifyLocation(entity_->defLocation(), loc);
}

bool EntityNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool EntityNode::same2(const EntityNode *node) const
{
  return entity_ == node->entity_;
}

void EntityNode::accept(NodeVisitor &visitor)
{
  visitor.entity(*this);
}

unsigned long EntityNode::hash() const
{
  return (unsigned long)entity_;
}

EntityAttributeOrigin
::EntityAttributeOrigin(const ExternalDataEntity *entity)
: entity_(entity)
{
}

const AttributeDefinitionList *
EntityAttributeOrigin::attDefList() const
{
  return entity_->notation()->attributeDefTemp();
}


const AttributeValue *
EntityAttributeOrigin::attributeValue(size_t attIndex, const GroveImpl &) const
{
  return entity_->attributes().value(attIndex);
}

AccessResult
EntityAttributeOrigin::setNodePtrAttributeOrigin(NodePtr &ptr,
						 const BaseNode *node) const
{
  ptr.assign(new EntityNode(node->grove(), entity_));
  return accessOK;
}

Node *EntityAttributeOrigin
::makeCdataAttributeValueNode(const GroveImpl *grove,
			      const AttributeValue *value,
			      size_t attIndex,
			      const TextIter &iter,
			      size_t charIndex) const
{
  return new EntityCdataAttributeValueNode(grove, value, attIndex, iter,
					   charIndex, entity_);
}


Node *EntityAttributeOrigin
::makeAttributeValueTokenNode(const GroveImpl *grove,
			      const TokenizedAttributeValue *value,
			      size_t attIndex,
			      size_t tokenIndex) const
{
  return new EntityAttributeValueTokenNode(grove, value, attIndex,
					   tokenIndex, entity_);
}

Node *EntityAttributeOrigin
::makeAttributeAsgnNode(const GroveImpl *grove,
			size_t attIndex) const
{
  return new EntityAttributeAsgnNode(grove, attIndex, entity_);
}

const void *EntityAttributeOrigin::attributeOriginId() const
{
  return entity_;
}

DoctypesAndLinktypesNamedNodeList
::DoctypesAndLinktypesNamedNodeList(const GroveImpl *grove)
: BaseNamedNodeList(grove, grove->generalSubstTable())
{
}

NodeListPtr DoctypesAndLinktypesNamedNodeList::nodeList() const
{
  NodePtr tem(new DocumentTypeNode(grove(), grove()->governingDtd()));
  return new SiblingNodeList(tem);
}

AccessResult
DoctypesAndLinktypesNamedNodeList
::namedNodeU(const StringC &str, NodePtr &ptr) const
{
  if (grove()->governingDtd()->name() != str)
    return accessNull;
  ptr.assign(new DocumentTypeNode(grove(), grove()->governingDtd()));
  return accessOK;
}

GeneralEntitiesNamedNodeList
::GeneralEntitiesNamedNodeList(const GroveImpl *grove, const Dtd *dtd)
: BaseNamedNodeList(grove, grove->entitySubstTable()), dtd_(dtd)
{
}

NodeListPtr GeneralEntitiesNamedNodeList::nodeList() const
{
  return new EntitiesNodeList(grove(),
			      dtd_->generalEntityIter());
}

AccessResult
GeneralEntitiesNamedNodeList::namedNodeU(const StringC &str, NodePtr &ptr) const
{
  const Entity *entity
   = dtd_->lookupEntityTemp(0, str);
  if (!entity)
    return accessNull;
  ptr.assign(new EntityNode(grove(), entity));
  return accessOK;
}

AccessResult
DefaultedEntitiesNamedNodeList::namedNodeU(const StringC &str, NodePtr &ptr) const
{
  const Entity *entity
   = grove()->lookupDefaultedEntity(str);
  if (!entity)
    return accessNull;
  ptr.assign(new EntityNode(grove(), entity));
  return accessOK;
}

NodeListPtr
DefaultedEntitiesNamedNodeList::nodeList() const
{
  return new EntitiesNodeList(grove(), grove()->defaultedEntityIter());
}

NodeListPtr DocEntitiesNamedNodeList::nodeList() const
{
  return new DocEntitiesNodeList(grove());
}

AccessResult
DocEntitiesNamedNodeList::namedNodeU(const StringC &str, NodePtr &ptr) const
{
  const Entity *entity
   = grove()->governingDtd()->lookupEntityTemp(0, str);
  // How I hate the default entity.
  while (!entity) {
    if (!grove()->hasDefaultEntity())
      return accessNull;
    // Make sure that the value of complete
    // we look at is that before we looked up
    // the entity.
    Boolean complete = grove()->complete();
    entity = grove()->lookupDefaultedEntity(str);
    if (entity)
      break;
    if (complete)
      return accessNull;
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  ptr.assign(new EntityNode(grove(), entity));
  return accessOK;
}

AccessResult
ElementsNamedNodeList::namedNodeU(const StringC &str, NodePtr &ptr) const
{
  for (;;) {
    Boolean complete = grove()->complete();
    const ElementChunk *element = grove()->lookupElement(str);
    if (element) {
      ptr.assign(new ElementNode(grove(), element));
      break;
    }
    if (complete)
      return accessNull;
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  }
  return accessOK;
}

NodeListPtr ElementsNamedNodeList::nodeList() const
{
  return new ElementsNodeList(grove(), grove()->root()->documentElement);
}

ElementsNodeList::ElementsNodeList(const GroveImpl *grove,
				   const Chunk *first)
: grove_(grove), first_(first)
{
}

AccessResult ElementsNodeList::first(NodePtr &ptr) const
{
  const Chunk *p = first_;
  while (p) {
    while (p == grove_->completeLimit()) {
      if (!grove_->waitForMoreNodes())
	return accessTimeout;
    }
    if (p->id()) {
      ((ElementsNodeList *)this)->first_ = p;
      ptr.assign(new ElementNode(grove_, (const ElementChunk *)p));
      return accessOK;
    }
    p = p->after();
  }
  return accessNull;
}

AccessResult ElementsNodeList::chunkRest(NodeListPtr &ptr) const
{
  const Chunk *p = first_;
  while (p) {
    while (p == grove_->completeLimit()) {
      if (!grove_->waitForMoreNodes())
	return accessTimeout;
    }
    if (p->id()) {
      if (canReuse(ptr))
	((ElementsNodeList *)this)->first_ = p->after();
      else
	ptr.assign(new ElementsNodeList(grove_, p->after()));
      return accessOK;
    }
    p = p->after();
 }
 return accessNull;
}

// iter.next() gives first member of list that this represents

EntitiesNodeList::EntitiesNodeList(const GroveImpl *grove,
				   const Dtd::ConstEntityIter &iter)
: grove_(grove), iter_(iter)
{
}

AccessResult EntitiesNodeList::first(NodePtr &ptr) const
{
  Dtd::ConstEntityIter tem(iter_);
  const Entity *entity = tem.nextTemp();
  if (!entity)
    return accessNull;
  ptr.assign(new EntityNode(grove_, entity));
  return accessOK;
}

AccessResult EntitiesNodeList::chunkRest(NodeListPtr &ptr) const
{
  if (canReuse(ptr)) {
    EntitiesNodeList *list = (EntitiesNodeList *)this;
    if (list->iter_.nextTemp() == 0)
      return accessNull;
    return accessOK;
  }
  Dtd::ConstEntityIter tem(iter_);
  if (tem.nextTemp() == 0)
    return accessNull;
  ptr.assign(new EntitiesNodeList(grove_, tem));
  return accessOK;
}

DocEntitiesNodeList::DocEntitiesNodeList(const GroveImpl *grove)
: EntitiesNodeList(grove, grove->governingDtd()->generalEntityIter())
{
}

AccessResult DocEntitiesNodeList::first(NodePtr &ptr) const
{
  AccessResult ret = EntitiesNodeList::first(ptr);
  if (ret != accessNull || !grove()->hasDefaultEntity())
    return ret;
  while (!grove()->complete())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  Dtd::ConstEntityIter tem(grove()->defaultedEntityIter());
  const Entity *entity = tem.nextTemp();
  if (!entity)
    return accessNull;
  ptr.assign(new EntityNode(grove(), entity));
  return accessOK;
}

AccessResult DocEntitiesNodeList::chunkRest(NodeListPtr &ptr) const
{
  AccessResult ret = EntitiesNodeList::chunkRest(ptr);
  if (ret != accessNull || !grove()->hasDefaultEntity())
    return ret;
  while (!grove()->complete())
    if (!grove()->waitForMoreNodes())
      return accessTimeout;
  Dtd::ConstEntityIter tem(grove()->defaultedEntityIter());
  const Entity *entity = tem.nextTemp();
  if (!entity)
    return accessNull;
  ptr.assign(new EntitiesNodeList(grove(), tem));
  return accessOK;
}

NotationsNamedNodeList
::NotationsNamedNodeList(const GroveImpl *grove, const Dtd *dtd)
: BaseNamedNodeList(grove, grove->generalSubstTable()), dtd_(dtd)
{
}

NodeListPtr NotationsNamedNodeList::nodeList() const
{
  return new NotationsNodeList(grove(),
			       dtd_->notationIter());
}

AccessResult
NotationsNamedNodeList::namedNodeU(const StringC &str, NodePtr &ptr) const
{
  const Notation *notation
   = dtd_->lookupNotationTemp(str);
  if (!notation)
    return accessNull;
  ptr.assign(new NotationNode(grove(), notation));
  return accessOK;
}


// iter.next() gives first member of list that this represents

NotationsNodeList::NotationsNodeList(const GroveImpl *grove,
				     const Dtd::ConstNotationIter &iter)
: grove_(grove), iter_(iter)
{
}

AccessResult NotationsNodeList::first(NodePtr &ptr) const
{
  Dtd::ConstNotationIter tem(iter_);
  const Notation *notation = tem.nextTemp();
  if (!notation)
    return accessNull;
  ptr.assign(new NotationNode(grove_, notation));
  return accessOK;
}

AccessResult NotationsNodeList::chunkRest(NodeListPtr &ptr) const
{
  if (canReuse(ptr)) {
    NotationsNodeList *list = (NotationsNodeList *)this;
    if (list->iter_.next().isNull())
      return accessNull;
    return accessOK;
  }
  Dtd::ConstNotationIter tem(iter_);
  if (tem.nextTemp() == 0)
    return accessNull;
  ptr.assign(new NotationsNodeList(grove_, tem));
  return accessOK;
}

NotationNode::NotationNode(const GroveImpl *grove,
			   const Notation *notation)
: BaseNode(grove), notation_(notation)
{
}

AccessResult NotationNode::getOrigin(NodePtr &ptr) const
{
  ptr.assign(new DocumentTypeNode(grove(), grove()->governingDtd()));
  return accessOK;
}

AccessResult NotationNode::getName(GroveString &str) const
{
  setString(str, notation_->name());
  return accessOK;
}

AccessResult NotationNode::getExternalId(NodePtr &ptr) const
{
  ptr.assign(new NotationExternalIdNode(grove(), notation_));
  return accessOK;
}

AccessResult NotationNode::getLocation(Location &loc) const
{
  return grove()->proxifyLocation(notation_->defLocation(), loc);
}

bool NotationNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool NotationNode::same2(const NotationNode *node) const
{
  return notation_ == node->notation_;
}

void NotationNode::accept(NodeVisitor &visitor)
{
  visitor.notation(*this);
}

unsigned long NotationNode::hash() const
{
  return (unsigned long)notation_;
}

ExternalIdNode::ExternalIdNode(const GroveImpl *grove)
: BaseNode(grove)
{
}

AccessResult ExternalIdNode::getPublicId(GroveString &str) const
{
  const StringC *s = externalId().publicIdString();
  if (!s)
    return accessNull;
  setString(str, *s);
  return accessOK;
}

AccessResult ExternalIdNode::getSystemId(GroveString &str) const
{
  const StringC *s = externalId().systemIdString();
  if (!s)
    return accessNull;
  setString(str, *s);
  return accessOK;
}

AccessResult ExternalIdNode::getGeneratedSystemId(GroveString &str) const
{
  const StringC &s = externalId().effectiveSystemId();
  if (!s.size())
    return accessNull;
  setString(str, s);
  return accessOK;
}

void ExternalIdNode::accept(NodeVisitor &visitor)
{
  visitor.externalId(*this);
}

bool ExternalIdNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool ExternalIdNode::same2(const ExternalIdNode *node) const
{
  return &externalId() == &node->externalId();
}

EntityExternalIdNode::EntityExternalIdNode(const GroveImpl *grove,
					   const ExternalEntity *entity)
: ExternalIdNode(grove), entity_(entity)
{
}

const ExternalId &EntityExternalIdNode::externalId() const
{
  return entity_->externalId();
}

AccessResult EntityExternalIdNode::getOrigin(NodePtr &ptr) const
{
  ptr.assign(new EntityNode(grove(), entity_));
  return accessOK;
}

unsigned long EntityExternalIdNode::hash() const
{
  return secondHash((unsigned long)entity_);
}

NotationExternalIdNode::NotationExternalIdNode(const GroveImpl *grove,
					       const Notation *notation)
: ExternalIdNode(grove), notation_(notation)
{
}

const ExternalId &NotationExternalIdNode::externalId() const
{
  return notation_->externalId();
}

AccessResult NotationExternalIdNode::getOrigin(NodePtr &ptr) const
{
  ptr.assign(new NotationNode(grove(), notation_));
  return accessOK;
}

unsigned long NotationExternalIdNode::hash() const
{
  return secondHash((unsigned long)notation_);
}

AccessResult ChunkNode::getParent(NodePtr &ptr) const
{
  if (!chunk_->origin)
    return accessNull;
  // This is needed because PiNodes in the prolog and
  // epilog don't have a parent but do have an origin.
  // Also for the document element.
  if ((const Chunk *)chunk()->origin == grove()->root())
    return accessNull;
  chunk_->origin->setNodePtrFirst(ptr, this);
  return accessOK;
}

AccessResult ChunkNode::getTreeRoot(NodePtr &ptr) const
{
  if (chunk()->origin
      && (const Chunk *)chunk()->origin != grove()->root()
      // With invalid documents we might have elements in the epilog
      && !grove()->root()->epilog
      && grove()->root()->documentElement)
    return grove()->root()->documentElement->setNodePtrFirst(ptr, this);
  return Node::getTreeRoot(ptr);
}

AccessResult ChunkNode::getOrigin(NodePtr &ptr) const
{
  if (!chunk_->origin)
    return accessNull;
  chunk_->origin->setNodePtrFirst(ptr, this);
  return accessOK;
}

AccessResult ChunkNode::getOriginToSubnodeRelPropertyName(ComponentName::Id &name) const
{
  if ((const Chunk *)chunk()->origin != grove()->root())
    name = ComponentName::idContent;
  else if ((const Chunk *)chunk() == grove()->root()->documentElement)
    name = ComponentName::idDocumentElement;
  else {
    const Chunk *tem;
    if (chunk()->getFirstSibling(grove(), tem) == accessOK && tem == grove()->root()->prolog)
      name = ComponentName::idProlog;
    else
      name = ComponentName::idEpilog;
  }
  return accessOK;
}

unsigned long ChunkNode::hash() const
{
  return (unsigned long)chunk_;
}

bool ChunkNode::same(const BaseNode &node) const
{
  return node.same2(this);
}

bool ChunkNode::same2(const ChunkNode *node) const
{
  return chunk_ == node->chunk_;
}

BaseNode::~BaseNode()
{
}

void BaseNode::addRef()
{
  ++refCount_;
}

void BaseNode::release()
{
  ASSERT(refCount_ != 0);
  if (--refCount_ == 0)
    delete this;
}

unsigned BaseNode::groveIndex() const
{
  return grove_->groveIndex();
}

bool BaseNode::operator==(const Node &node) const
{
  if (!sameGrove(node))
    return 0;
  return same((const BaseNode &)node);
}

bool BaseNode::chunkContains(const Node &node) const
{
  if (!sameGrove(node))
    return 0;
  return same((const BaseNode &)node);
}

bool BaseNode::inChunk(const DataNode *) const
{
  return 0;
}

bool BaseNode::inChunk(const CdataAttributeValueNode *) const
{
  return 0;
}

bool BaseNode::same2(const ChunkNode *) const
{
  return 0;
}

bool BaseNode::same2(const DataNode *) const
{
  return 0;
}

bool BaseNode::same2(const AttributeAsgnNode *) const
{
  return 0;
}

bool BaseNode::same2(const AttributeValueTokenNode *) const
{
  return 0;
}

bool BaseNode::same2(const CdataAttributeValueNode *) const
{
  return 0;
}

bool BaseNode::same2(const EntityNode *) const
{
  return 0;
}

bool BaseNode::same2(const NotationNode *) const
{
  return 0;
}

bool BaseNode::same2(const ExternalIdNode *) const
{
  return 0;
}

bool BaseNode::same2(const DocumentTypeNode *) const
{
  return 0;
}

bool BaseNode::same2(const SgmlConstantsNode *) const
{
  return 0;
}

bool BaseNode::same2(const MessageNode *) const
{
  return 0;
}

AccessResult BaseNode::nextSibling(NodePtr &ptr) const
{
  return nextChunkSibling(ptr);
}

AccessResult BaseNode::follow(NodeListPtr &ptr) const
{
  NodePtr nd;
  AccessResult ret = nextSibling(nd);
  switch (ret) {
  case accessOK:
    ptr.assign(new SiblingNodeList(nd));
    break;
  case accessNull:
    ptr.assign(new BaseNodeList);
    ret = accessOK;
    break;
  default:
    break;
  }
  return ret;
}

AccessResult BaseNode::children(NodeListPtr &ptr) const
{
  NodePtr head;
  AccessResult ret = firstChild(head);
  switch (ret) {
  case accessOK:
    ptr.assign(new SiblingNodeList(head));
    break;
  case accessNull:
    ptr.assign(new BaseNodeList);
    ret = accessOK;
    break;
  default:
    break;
  }
  return ret;
}

AccessResult BaseNode::getOrigin(NodePtr &ptr) const
{
  return getParent(ptr);
}

AccessResult BaseNode::getGroveRoot(NodePtr &ptr) const
{
  ptr.assign(new SgmlDocumentNode(grove(), grove()->root()));
  return accessOK;
}

AccessResult BaseNode::getLocation(Location &) const
{
  return accessNull;
}

bool BaseNode::queryInterface(IID iid, const void *&p) const
{
  if (iid == LocNode::iid) {
    const LocNode *ip = this;
    p = ip;
    return 1;
  }
  return 0;
}

AccessResult
ForwardingChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const
{
  if (forwardTo == 0)
    return accessNull;
  ASSERT(origin == forwardTo->origin);
  return forwardTo->setNodePtrFirst(ptr, node);
}

AccessResult
ForwardingChunk::getFollowing(const GroveImpl *grove,
                              const Chunk *&p, unsigned long &nNodes)
    const
{
  AccessResult ret = Chunk::getFollowing(grove, p, nNodes);
  if (ret == accessOK)
    nNodes = 0;
  return ret;
}

AccessResult
LocOriginChunk::getFollowing(const GroveImpl *grove,
                             const Chunk *&p, unsigned long &nNodes)
    const
{
  AccessResult ret = Chunk::getFollowing(grove, p, nNodes);
  if (ret == accessOK)
    nNodes = 0;
  return ret;
}

AccessResult LocOriginChunk::setNodePtrFirst(NodePtr &ptr, const BaseNode *node) const
{
  return ((const Chunk *)(this + 1))->setNodePtrFirst(ptr, node);
}

AccessResult LocOriginChunk::setNodePtrFirst(NodePtr &ptr, const ElementNode *node) const
{
  return ((const Chunk *)(this + 1))->setNodePtrFirst(ptr, node);
}

AccessResult LocOriginChunk::setNodePtrFirst(NodePtr &ptr, const DataNode *node) const
{
  return ((const Chunk *)(this + 1))->setNodePtrFirst(ptr, node);
}

const Chunk *LocOriginChunk::after() const
{
  return this + 1;
}

Boolean LocOriginChunk::getLocOrigin(const Origin *&ret) const
{
  ret = locOrigin;
  return 1;
}

AccessResult
Chunk::setNodePtrFirst(NodePtr &ptr, const ElementNode *node) const
{
  return setNodePtrFirst(ptr, (const BaseNode *)node);
}

AccessResult
Chunk::setNodePtrFirst(NodePtr &ptr, const DataNode *node) const
{
  return setNodePtrFirst(ptr, (const BaseNode *)node);
}

const StringC *Chunk::id() const
{
  return 0;
}

AccessResult Chunk::getFollowing(const GroveImpl *grove,
			         const Chunk *&f, unsigned long &n) const
{
  const Chunk *p = after();
  while (p == grove->completeLimit())
    if (!grove->waitForMoreNodes())
      return accessTimeout;
  if (p->origin != origin)
    return accessNull;
  n = 1;
  f = p;
  return accessOK;
}

AccessResult Chunk::getFirstSibling(const GroveImpl *grove,
				    const Chunk *&p) const
{
  if ((const Chunk *)origin == grove->root())
    return accessNotInClass;
  p = origin->after();
  return accessOK;
}

Boolean Chunk::getLocOrigin(const Origin *&) const
{
  return 0;
}

#ifdef SP_NAMESPACE
}
#endif

#include "grove_inst.cxx"
