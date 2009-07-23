// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef DssslSpecEventHandler_INCLUDED
#define DssslSpecEventHandler_INCLUDED 1

#include "ErrorCountEventHandler.h"
#include "ArcEngine.h"
#include "Message.h"
#include "Text.h"
#include "Owner.h"
#include "Boolean.h"
#include "InputSource.h"
#include "Vector.h"
#include "NCVector.h"
#include "IList.h"
#include "IListIter.h"
#include "StringC.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class DssslSpecEventHandler
: private ErrorCountEventHandler, private ArcDirector {
public:
  class BodyElement : public Link {
  public:
    virtual ~BodyElement();
    virtual void makeInputSource(DssslSpecEventHandler &, Owner<InputSource> &) = 0;
  };

  class ImmediateBodyElement : public BodyElement {
  public:
    ImmediateBodyElement(Text &);
    void makeInputSource(DssslSpecEventHandler &, Owner<InputSource> &);
  private:
    Text text_;
  };

  class EntityBodyElement : public BodyElement {
  public:
    EntityBodyElement(const ConstPtr<Entity> &);
    void makeInputSource(DssslSpecEventHandler &, Owner<InputSource> &);
  private:
    ConstPtr<Entity> entity_;
  };

  class SpecPart;
  class Doc;
  class Part;

  class PartHeader : public Link {
  public:
    PartHeader(Doc *, const StringC &id);
    const StringC &id() const;
    Part *resolve(DssslSpecEventHandler &);
    void setPart(SpecPart *);
    void setRefLoc(const Location &);
  private:
    Doc *doc_;
    StringC id_;
    // Location of first reference
    Location refLoc_;
    Owner<SpecPart> specPart_;
  };


  class SpecPart {
  public:
    virtual ~SpecPart();
    virtual Part *resolve(DssslSpecEventHandler &) = 0;
  };

  class ExternalPart : public SpecPart {
  public:
    ExternalPart(PartHeader *);
    Part *resolve(DssslSpecEventHandler &);
  private:
    PartHeader *header_;
  };

  class ExternalFirstPart : public SpecPart {
  public:
    ExternalFirstPart(Doc *);
    Part *resolve(DssslSpecEventHandler &);
  private:
    Doc *doc_;
  };

  class Part : public SpecPart {
  public:
    typedef IListIter<BodyElement> Iter;
    Part();
    Iter iter();
    const Vector<PartHeader *> &use() const;
    void addUse(PartHeader *);
    void append(BodyElement *);
    bool setMark(bool = 1);
  private:
    Part *resolve(DssslSpecEventHandler &);
    IList<BodyElement> bodyElements_;
    Vector<PartHeader *> use_;
    bool mark_;
  };

  class Doc : public Link {
  public:
    Doc();
    Doc(const StringC &sysid);
    void setLocation(const Location &);
    PartHeader *refPart(const StringC &);
    PartHeader *refPart(const StringC &, const Location &refLoc);
    Part *resolveFirstPart(DssslSpecEventHandler &);
    void load(DssslSpecEventHandler &);
    const StringC &sysid() const;
  private:
    bool loaded_;
    StringC sysid_;
    IList<PartHeader> headers_;
    Location loc_;
  };
  DssslSpecEventHandler(Messenger &);
  void load(SgmlParser &specParser, const CharsetInfo &, const StringC &id,
	    Vector<Part *> &parts);
  void externalSpecificationStart(const StartElementEvent &);
  void externalSpecificationEnd(const EndElementEvent &);
  void styleSpecificationStart(const StartElementEvent &);
  void styleSpecificationEnd(const EndElementEvent &);
  void styleSpecificationBodyStart(const StartElementEvent &);
  void styleSpecificationBodyEnd(const EndElementEvent &);
private:
  Vector<Part *> &parts();
  void startElement(StartElementEvent *);
  void endElement(EndElementEvent *);
  void data(DataEvent *);
  void message(MessageEvent *);
  void endProlog(EndPrologEvent *);
  EventHandler *arcEventHandler(const Notation *,
				const Vector<StringC> &,
				const SubstTable<Char> *);
  void loadDoc(SgmlParser &, Doc &);
  void resolveParts(Part *, Vector<Part *> &);
  Doc *findDoc(const StringC &);
  const StringC *attributeString(const StartElementEvent &, const char *);
  const Text *attributeText(const StartElementEvent &, const char *);
  ConstPtr<Entity> attributeEntity(const StartElementEvent &, const char *);

  Messenger *mgr_;
  bool gotArc_;
  bool gatheringBody_;
  Part *currentPart_;
  Text currentBody_;
  Doc *currentDoc_;
  IList<Doc> docs_;
  SgmlParser *parser_;
  const CharsetInfo *charset_;
  friend class Doc;
  friend class EntityBodyElement;
  friend class PartHeader;
};

inline
const StringC &DssslSpecEventHandler::PartHeader::id() const
{
  return id_;
}

inline
void DssslSpecEventHandler::PartHeader::setPart(SpecPart *part)
{
  specPart_ = part;
}

inline
void DssslSpecEventHandler::Part::addUse(PartHeader *header)
{
  use_.push_back(header);
}
    
inline
const Vector<DssslSpecEventHandler::PartHeader *> &
DssslSpecEventHandler::Part::use() const
{
  return use_;
}

inline
const StringC &DssslSpecEventHandler::Doc::sysid() const
{
  return sysid_;
}

inline
DssslSpecEventHandler::Part::Iter DssslSpecEventHandler::Part::iter()
{
  return Iter(bodyElements_);
}

inline
bool DssslSpecEventHandler::Part::setMark(bool b)
{
  bool tem = mark_;
  mark_ = b;
  return tem;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not DssslSpecEventHandler_INCLUDED */
