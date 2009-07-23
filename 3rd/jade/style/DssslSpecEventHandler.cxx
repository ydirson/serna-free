// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "DssslSpecEventHandler.h"
#include "InterpreterMessages.h"
#include "InternalInputSource.h"
#include "FOTBuilder.h"
#include "macros.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

// FIXME Stop parsing spec when we know we don't we have everything we ever need

class TextInputSourceOrigin : public InputSourceOrigin {
public:
  TextInputSourceOrigin(Text &text);
  Boolean defLocation(Offset off, const Origin *&, Index &) const;
  const Text &text() const { return text_; }
  void noteCharRef(Index, const NamedCharRef &) {
    CANNOT_HAPPEN();
  }
  void setExternalInfo(ExternalInfo *) {
    CANNOT_HAPPEN();
  }
  InputSourceOrigin *copy() const { return new TextInputSourceOrigin(*this); }
  const Location &parent() const { return refLocation_; }
private:
  Text text_;
  Location refLocation_;
};

DssslSpecEventHandler::DssslSpecEventHandler(Messenger &mgr)
: mgr_(&mgr), gatheringBody_(0)
{
}

void DssslSpecEventHandler::load(SgmlParser &specParser,
				 const CharsetInfo &charset,
				 const StringC &id,
				 Vector<Part *> &parts)
{
  parser_ = &specParser;
  charset_ = &charset;
  Doc *doc = findDoc(StringC());
  // Load it now so that we can get the concrete syntax.
  doc->load(*this);
  Part *tem;
  if (id.size() == 0)
    tem = doc->resolveFirstPart(*this);
  else {
    StringC normId(id);
    ConstPtr<Syntax> syn = parser_->instanceSyntax();
    if (!syn.isNull())
      syn->generalSubstTable()->subst(normId);
    tem = doc->refPart(normId)->resolve(*this);
  }
  resolveParts(tem, parts);
}

DssslSpecEventHandler::Doc *DssslSpecEventHandler::findDoc(const StringC &sysid)
{
  for (IListIter<Doc> iter(docs_); !iter.done(); iter.next())
    if (sysid == iter.cur()->sysid())
      return iter.cur();
  Doc *doc = new Doc(sysid);
  docs_.insert(doc);
  return doc;
}

void DssslSpecEventHandler::resolveParts(Part *part,
					 Vector<Part *> &parts)
{
  if (!part)
    return;
  parts.push_back(part);
  if (part->setMark()) {
    mgr_->message(InterpreterMessages::useLoop);
    return;
  }
  const Vector<PartHeader *> &use = part->use();
  for (size_t i = 0; i < use.size(); i++) {
    Part *tem = use[i]->resolve(*this);
    resolveParts(tem, parts);
  }
  part->setMark(0);
}

void DssslSpecEventHandler::loadDoc(SgmlParser &parser, Doc &doc)
{
  currentDoc_ = &doc;
  gotArc_ = 0;
  ArcEngine::parseAll(parser, *mgr_, *this, cancelPtr());
  if (!gotArc_) {
    mgr_->message(InterpreterMessages::specNotArc);
    return;
  }
}


EventHandler *
DssslSpecEventHandler::arcEventHandler(const Notation *notation,
				       const Vector<StringC> &,
				       const SubstTable<Char> *)
{
  if (!notation)
    return 0;
  const StringC *pubid = notation->externalId().publicIdString();
  static const char dssslArc[]
    = "ISO/IEC 10179:1996//NOTATION DSSSL Architecture Definition Document//EN";
  if (pubid == 0 || pubid->size() != sizeof(dssslArc) - 1)
    return 0;
  for (int i = 0; dssslArc[i] != '\0'; i++) {
    if (dssslArc[i] != (*pubid)[i])
      return 0;
  }
  gotArc_ = 1;
  return this;
}

void DssslSpecEventHandler::message(MessageEvent *event)
{
  mgr_->dispatchMessage(event->message());
  delete event;
}

static struct {
  const char *gi;
  void (DssslSpecEventHandler::*start)(const StartElementEvent &);
  void (DssslSpecEventHandler::*end)(const EndElementEvent &);
} mappingTable[] = {
  { "STYLE-SPECIFICATION",
    &DssslSpecEventHandler::styleSpecificationStart,
    &DssslSpecEventHandler::styleSpecificationEnd },
  { "STYLE-SPECIFICATION-BODY",
    &DssslSpecEventHandler::styleSpecificationBodyStart,
    &DssslSpecEventHandler::styleSpecificationBodyEnd },
  { "EXTERNAL-SPECIFICATION",
    &DssslSpecEventHandler::externalSpecificationStart,
    &DssslSpecEventHandler::externalSpecificationEnd },
};

void DssslSpecEventHandler::endProlog(EndPrologEvent *event)
{
  currentDoc_->setLocation(event->location());
  delete event;
}

void DssslSpecEventHandler::startElement(StartElementEvent *event)
{
  for (size_t i = 0; i < SIZEOF(mappingTable); i++)
    if (event->name() == mappingTable[i].gi) {
      (this->*(mappingTable[i].start))(*event);
      break;
    }
  delete event;
}

void DssslSpecEventHandler::endElement(EndElementEvent *event)
{
  for (size_t i = 0; i < SIZEOF(mappingTable); i++)
    if (event->name() == mappingTable[i].gi) {
      (this->*(mappingTable[i].end))(*event);
      break;
    }
  delete event;
}

void DssslSpecEventHandler::data(DataEvent *event)
{
  if (gatheringBody_)
    currentBody_.addChars(event->data(), event->dataLength(), event->location());
  delete event;
}

const Text *
DssslSpecEventHandler::attributeText(const StartElementEvent &event,
				     const char *attName)
{
  const AttributeList &atts = event.attributes();
  StringC attNameString;
  for (const char *p = attName; *p; p++)
    attNameString += Char(*p);
  unsigned index;
  if (atts.attributeIndex(attNameString, index)) {
    const AttributeValue *val = atts.value(index);
    if (val)
      return val->text();
  }
  return 0;
}

const StringC *
DssslSpecEventHandler::attributeString(const StartElementEvent &event,
				       const char *attName)
{
  const Text *text = attributeText(event, attName);
  if (text)
    return &text->string();
  else
    return 0;
}

ConstPtr<Entity>
DssslSpecEventHandler::attributeEntity(const StartElementEvent &event,
				       const char *attName)
{
  const AttributeList &atts = event.attributes();
  StringC attNameString;
  for (const char *p = attName; *p; p++)
    attNameString += Char(*p);
  unsigned index;
  if (!atts.attributeIndex(attNameString, index))
    return 0;
  const AttributeSemantics *sem = atts.semantics(index);
  if (!sem || sem->nEntities() != 1)
    return 0;
  return sem->entity(0);
}

void DssslSpecEventHandler::externalSpecificationStart(const StartElementEvent &event)
{
  StringC empty;
  const StringC *idP = attributeString(event, "ID");
  if (!idP)
    idP = &empty;
  PartHeader *header = currentDoc_->refPart(*idP);
  const Entity *ent = attributeEntity(event, "DOCUMENT").pointer();
  if (!ent)
    return;
  const ExternalEntity *ext = ent->asExternalEntity();
  if (!ext)
    return;
  const StringC &sysid = ext->externalId().effectiveSystemId();
  if (sysid.size()) {
    Doc *doc = findDoc(sysid);
    const StringC *specidP = attributeString(event, "SPECID");
    if (!specidP)
      header->setPart(new ExternalFirstPart(doc));
    else
      header->setPart(new ExternalPart(doc->refPart(*specidP, event.location())));
  }
}

void DssslSpecEventHandler::externalSpecificationEnd(const EndElementEvent &)
{
  // nothing to do
}

void DssslSpecEventHandler::styleSpecificationStart(const StartElementEvent &event)
{
  StringC empty;
  const StringC *idP = attributeString(event, "ID");
  if (!idP)
    idP = &empty;
  PartHeader *header = currentDoc_->refPart(*idP);
  // FIXME give an error (or ignore) if header has part already
  const Text *useP = attributeText(event, "USE");
  header->setPart(currentPart_ = new Part);
  if (useP) {
    const StringC &use = useP->string();
    size_t i = 0;
    for (;;) {
      size_t j;
      for (j = i; j < use.size() && use[j] != ' '; j++)
	;
      if (j > i)
	currentPart_->addUse(currentDoc_->refPart(StringC(use.data() + i,
							  j - i),
						  useP->charLocation(i)));
      if (j >= use.size())
	break;
      i = j + 1;
    }
  }
  // FIXME Give warning if selected part is incomplete
}

void DssslSpecEventHandler::styleSpecificationEnd(const EndElementEvent &event)
{
  currentPart_ = 0;
}

void DssslSpecEventHandler::styleSpecificationBodyStart(const StartElementEvent &event)
{
  if (currentPart_) {
    currentBody_.clear();
    ConstPtr<Entity> entity = attributeEntity(event, "CONTENT");
    if (entity.isNull())
      gatheringBody_ = 1;
    else
      currentPart_->append(new EntityBodyElement(entity));
  }
}

void DssslSpecEventHandler::styleSpecificationBodyEnd(const EndElementEvent &event)
{
  if (gatheringBody_) {
    if (currentPart_)
      currentPart_->append(new ImmediateBodyElement(currentBody_));
    gatheringBody_ = 0;
  }
}

TextInputSourceOrigin::TextInputSourceOrigin(Text &text)
{
  text_.swap(text);
}

Boolean TextInputSourceOrigin::defLocation(Offset off, const Origin *&origin, Index &index) const
{
  return text_.charLocation(off, origin, index);
}

DssslSpecEventHandler::Doc::Doc()
: loaded_(0)
{
}

DssslSpecEventHandler::Doc::Doc(const StringC &sysid)
: sysid_(sysid), loaded_(0)
{
}

void DssslSpecEventHandler::Doc::setLocation(const Location &loc)
{
  loc_ = loc;
}

DssslSpecEventHandler::Part *
DssslSpecEventHandler::Doc::resolveFirstPart(DssslSpecEventHandler &eh)
{
  load(eh);
  PartHeader *header = 0;
  for (IListIter<PartHeader> iter(headers_); !iter.done(); iter.next())
    header = iter.cur();
  if (!header) {
    if (!loc_.origin().isNull()) {
      eh.mgr_->setNextLocation(loc_);
      eh.mgr_->message(InterpreterMessages::noParts);
    }
    return 0;
  }
  return header->resolve(eh);
}

void DssslSpecEventHandler::Doc::load(DssslSpecEventHandler &eh)
{
  if (loaded_)
    return;
  loaded_ = 1;
  if (sysid_.size() > 0) {
    SgmlParser::Params params;
    params.parent = eh.parser_;
    params.sysid = sysid_;
    SgmlParser specParser(params);
    eh.loadDoc(specParser, *this);
  }
  else
    eh.loadDoc(*eh.parser_, *this);
}

DssslSpecEventHandler::PartHeader *
DssslSpecEventHandler::Doc::refPart(const StringC &id)
{
  for (IListIter<PartHeader> iter(headers_); !iter.done(); iter.next())
    if (iter.cur()->id() == id)
      return iter.cur();
  PartHeader *header = new PartHeader(this, id);
  headers_.insert(header);
  return header;
}

DssslSpecEventHandler::PartHeader *
DssslSpecEventHandler::Doc::refPart(const StringC &id, const Location &refLoc)
{
  PartHeader *header = refPart(id);
  header->setRefLoc(refLoc);
  return header;
}

DssslSpecEventHandler::BodyElement::~BodyElement()
{
}

DssslSpecEventHandler::ImmediateBodyElement::ImmediateBodyElement(Text &text)
{
  text_.swap(text);
}

void DssslSpecEventHandler
::ImmediateBodyElement::makeInputSource(DssslSpecEventHandler &, Owner<InputSource> &in)
{
  TextInputSourceOrigin *origin = new TextInputSourceOrigin(text_);
  in = new InternalInputSource(origin->text().string(), origin);
}

DssslSpecEventHandler
::EntityBodyElement::EntityBodyElement(const ConstPtr<Entity> &entity)
: entity_(entity)
{
}

void DssslSpecEventHandler
::EntityBodyElement::makeInputSource(DssslSpecEventHandler &eh,
				     Owner<InputSource> &in)
{
  const InternalEntity *internal = entity_->asInternalEntity();
  if (internal) {
    in = new InternalInputSource(internal->string(),
			         EntityOrigin::make(entity_, Location()));
    return;
  }
  const StringC &sysid
    = entity_->asExternalEntity()->externalId().effectiveSystemId();
  if (sysid.size())
    in = eh.parser_->entityManager().open(sysid, *eh.charset_, InputSourceOrigin::make(),
					  0, *eh.mgr_);
}

DssslSpecEventHandler::PartHeader::PartHeader(Doc *doc, const StringC &id)
: doc_(doc), id_(id)
{
}

void DssslSpecEventHandler::PartHeader::setRefLoc(const Location &loc)
{
  if (refLoc_.origin().isNull())
    refLoc_ = loc;
}

DssslSpecEventHandler::Part *
DssslSpecEventHandler::PartHeader::resolve(DssslSpecEventHandler &eh)
{
  doc_->load(eh);
  if (!specPart_) {
    eh.mgr_->setNextLocation(refLoc_);
    eh.mgr_->message(InterpreterMessages::missingPart, StringMessageArg(id_));
    return 0;
  }
  return specPart_->resolve(eh);
}

DssslSpecEventHandler::SpecPart::~SpecPart()
{
}

DssslSpecEventHandler::ExternalPart::ExternalPart(PartHeader *header)
: header_(header)
{
}

DssslSpecEventHandler::Part *
DssslSpecEventHandler::ExternalPart::resolve(DssslSpecEventHandler &eh)
{
  return header_->resolve(eh);
}

DssslSpecEventHandler::ExternalFirstPart::ExternalFirstPart(Doc *doc)
: doc_(doc)
{
}

DssslSpecEventHandler::Part *
DssslSpecEventHandler::ExternalFirstPart::resolve(DssslSpecEventHandler &eh)
{
  return doc_->resolveFirstPart(eh);
}

DssslSpecEventHandler::Part::Part()
: mark_(0)
{
}

DssslSpecEventHandler::Part *
DssslSpecEventHandler::Part::resolve(DssslSpecEventHandler &)
{
  return this;
}

void DssslSpecEventHandler::Part::append(BodyElement *element)
{
  bodyElements_.append(element);
}

#ifdef DSSSL_NAMESPACE
}
#endif
