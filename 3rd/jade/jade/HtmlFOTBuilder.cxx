// Copyright (c) 1996, 1997 James Clark
// See the file copying.txt for copying permission.

#include "config.h"

#ifdef JADE_HTML

#include "HtmlFOTBuilder.h"
#include "FOTBuilder.h"
#include "Link.h"
#include "IList.h"
#include "IListIter.h"
#include "OutputCharStream.h"
#include "Ptr.h"
#include "Resource.h"
#include "macros.h"
#include "HtmlMessages.h"
#include "MessageArg.h"
#include "ErrnoMessageArg.h"
#include "StringResource.h"
#include "OwnerTable.h"
#include "Hash.h"
#include "OutputByteStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class DiscardOutputCharStream : public OutputCharStream {
public:
  DiscardOutputCharStream() { }
  void flush() { }
  void flushBuf(Char) { }
};

const char RE = '\r';

class HtmlFOTBuilder : public SerialFOTBuilder {
public:
  class OutputState;
  struct CharProps {
    CharProps();
    static unsigned long hash(const CharProps &) { return 17; }
    bool operator==(const CharProps &cp) const {
      return (fontWeight == cp.fontWeight
	      && fontStyle == cp.fontStyle
	      && fontSize == cp.fontSize
	      && fontFamily == cp.fontFamily
	      && color == cp.color);
    }
    bool operator!=(const CharProps &cp) const { return !(*this == cp); }
    char fontWeight;
    enum { styleNormal, styleItalic, styleOblique };
    char fontStyle;
    unsigned color;
    Length fontSize;
    StringC fontFamily;
  };
  struct InheritParaProps {
    InheritParaProps();
    enum {
      alignLeft,
      alignCenter,
      alignRight,
      alignJustify
    };
    Length leftMargin;
    Length rightMargin;
    Length lineHeight;
    Length textIndent;
    char align;
  };
  struct ParaProps : public InheritParaProps {
    ParaProps(const InheritParaProps &props)
      : InheritParaProps(props), topMargin(0) { }
    static unsigned long hash(const ParaProps &) { return 17; }
    bool operator==(const ParaProps &pp) const {
      return (leftMargin == pp.leftMargin
	      && rightMargin == pp.rightMargin
	      && lineHeight == pp.lineHeight
	      && textIndent == pp.textIndent
	      && topMargin == pp.topMargin
	      && align == pp.align);
    }
    bool operator!=(const ParaProps &pp) const { return !(*this == pp); }
    Length topMargin;
  };

  struct ClassPrefix {
    ClassPrefix(const StringC &s) : prefix(s), nCharClasses(0), nParaClasses(0) { }
    StringC prefix;
    unsigned nCharClasses;
    unsigned nParaClasses;
    static const StringC &key(const ClassPrefix &cp) { return cp.prefix; }
  };

  struct StyleClass : public Link {
    StyleClass(const StringC &, ClassPrefix *);
    void outputName(OutputCharStream &) const;
    StringC gi;
    const ClassPrefix *prefix;
    unsigned prefixIndex;
   };

  struct CharStyle;

  struct CharStyleClass : public StyleClass {
     CharStyleClass(const StringC &, ClassPrefix *, const CharStyle *);
     const CharStyle *style;
  };

  struct CharStyle : public CharProps {
    CharStyle(const CharProps &cp) : CharProps(cp) { }
    void output(OutputCharStream &) const;
    static const CharProps &key(const CharStyle &style) { return style; }
    IList<CharStyleClass> classes;
  };

  struct ParaStyle;

  struct ParaStyleClass : public StyleClass {
     ParaStyleClass(const StringC &, ClassPrefix *, const ParaStyle *);
     const ParaStyle *style;
  };

  struct ParaStyle : public ParaProps {
    ParaStyle(const ParaProps &pp) : ParaProps(pp) { }
    void output(OutputCharStream &) const;
    static const ParaProps &key(const ParaStyle &style) { return style; }
    IList<ParaStyleClass> classes;
  };

  struct FlowObjectInfo : public CharProps, public InheritParaProps {
    FlowObjectInfo();
    unsigned docIndex;
    ConstPtr<StringResource<Char> > scrollTitle;
    Length parentLeftMargin;
    Length parentRightMargin;
  };

  // An address that has been referenced.
  class AddressRef {
  public:
    virtual void outputRef(bool end, OutputCharStream &, OutputState &) const = 0;
    virtual ~AddressRef();
  };

  class OutputState {
  public:
    OutputState(const String<CmdLineApp::AppChar> *outputFilename,
		const StringC &styleSheetFilename,
                CmdLineApp *app)
     : outputFilename_(outputFilename),
       styleSheetFilename_(styleSheetFilename),
       app_(app),
       curAref_(0),
       specAref_(0),
       curCharStyleClass_(0),
       outputDocIndex_(unsigned(-1)) { }
    unsigned setOutputDocIndex(unsigned i) {
      unsigned tem = outputDocIndex_;
      outputDocIndex_ = i;
      return tem;
    }
    const AddressRef *setAddress(const AddressRef *aref) {
      const AddressRef *tem = specAref_;
      specAref_ = aref;
      return tem;
    }
    void syncChar(const CharStyleClass *, OutputCharStream &);
    unsigned outputDocIndex() const { return outputDocIndex_; }
    String<CmdLineApp::AppChar> outputFilename(unsigned i) const;
    const String<CmdLineApp::AppChar> &outputFilename() const {
      return *outputFilename_;
    }
    CmdLineApp &app() { return *app_; }
    const StringC &styleSheetFilename() const { return styleSheetFilename_; }
    const OutputCodingSystem *codingSystem() const { return app_->outputCodingSystem(); }
    Messenger &messenger() { return *app_; }
  private:
    const String<CmdLineApp::AppChar> *outputFilename_;

    CmdLineApp *app_;
    unsigned outputDocIndex_;
    const AddressRef *curAref_;
    const AddressRef *specAref_;
    const CharStyleClass *curCharStyleClass_;
    StringC styleSheetFilename_;
  };
  class Item : public Link {
  public:
    Item() { }
    virtual ~Item();
    virtual void output(OutputCharStream &, OutputState &) { }
  private:
    void operator=(const Item &);
    Item(const Item &);
  };
  // Corresponds to <A NAME="..."></A>
  class Addressable : public Item, public AddressRef {
  public:
    Addressable(size_t g, size_t e)
      : groveIndex_(g), elementIndex_(e), referenced_(0), docIndex_(unsigned(-1)) { }
    void outputRef(bool end, OutputCharStream &, OutputState &) const;
    void output(OutputCharStream &, OutputState &);
    bool defined() const { return docIndex_ != unsigned(-1); }
    bool referenced() const { return referenced_; }
    void setDefined(unsigned docIndex, bool wholeDocument = 0) {
      docIndex_ = docIndex;
      if (wholeDocument)
	elementIndex_ = (unsigned)-1;
    }
    void setReferenced() { referenced_ = 1; }
  private:
    size_t groveIndex_;
    // -1 if the whole document
    size_t elementIndex_;
    // unsigned(-1) if not defined
    unsigned docIndex_;
    bool referenced_;
  };
  class Markup : public Item {
  public:
    Markup(const StringC &str) : str_(str) { }
    void output(OutputCharStream &, OutputState &);
  private:
    StringC str_;
  };
  class Pcdata : public Item {
  public:
    Pcdata(const CharStyleClass *styleClass) : styleClass_(styleClass) { }
    void output(OutputCharStream &, OutputState &);
  private:
    const CharStyleClass *styleClass_;
    virtual const Char *data(size_t &) const = 0;
  };
  class ImmediatePcdata : public Pcdata {
  public:
    ImmediatePcdata(const Char *s, size_t n, const CharStyleClass *styleClass)
      : Pcdata(styleClass), str_(s, n) { }
  private:
    const Char *data(size_t &n) const { n = str_.size(); return str_.data(); }
    StringC str_;
  };
  class NodePcdata : public Pcdata {
  public:
    NodePcdata(const NodePtr &node, const Char *s, size_t n, const CharStyleClass *styleClass)
      : Pcdata(styleClass), node_(node), s_(s), n_(n) { }
  private:
    const Char *data(size_t &n) const { n = n_; return s_; }
    NodePtr node_;
    const Char *s_;
    size_t n_;
  };
  class Container : public Item {
  public:
    Container() { }
    IList<Item> *contentPtr() { return &content_; }
    void output(OutputCharStream &, OutputState &);
  protected:
    void reverse();
    // In reverse order while being built.
    IList<Item> content_;
  private:
    Container(const Container &);
    void operator=(const Container &);
  };
  class Ref : public Container {
  public:
    Ref(AddressRef *aref) : aref_(aref) { }
    void output(OutputCharStream &, OutputState &);
  private:
    AddressRef *aref_;
  };
  class Block : public Container {
  public:
    Block(const ParaStyleClass *styleClass) : styleClass_(styleClass) { }
    void output(OutputCharStream &, OutputState &);
  private:
    const ParaStyleClass *styleClass_;
  };
  class Document : public Container {
  public:
    Document(unsigned index, const ConstPtr<StringResource<Char> > &title)
      : index_(index), title_(title) { }
    ~Document();
    void output(OutputCharStream &, OutputState &);
  private:
    ConstPtr<StringResource<Char> > title_;
    unsigned index_;
  };
  HtmlFOTBuilder(const String<CmdLineApp::AppChar> &, CmdLineApp *);
  ~HtmlFOTBuilder();
  void setFontWeight(Symbol);
  void setFontPosture(Symbol);
  void setFontFamilyName(const StringC &);
  void setFontSize(Length);
  void setColor(const DeviceRGBColor &);
  void setQuadding(Symbol);
  void setLineSpacing(const LengthSpec &);
  void setFirstLineStartIndent(const LengthSpec &);
  void setStartIndent(const LengthSpec &);
  void setEndIndent(const LengthSpec &);
  void setScrollTitle(const StringC &);
  void formattingInstruction(const StringC &);
  void start();
  void end();
  void atomic();
  void characters(const Char *s, size_t n);
  void charactersFromNode(const NodePtr &, const Char *s, size_t n);
  void startParagraph(const ParagraphNIC &nic);
  void endParagraph();
  void startLink(const Address &);
  void endLink();
  void startScroll();
  void endScroll();
  void startNode(const NodePtr &, const StringC &);
  void endNode();
  void flushPendingAddresses();
  static void outputCdata(const Char *, size_t, OutputCharStream &);
  static void outputCdata(const StringC &s, OutputCharStream &os) {
    outputCdata(s.data(), s.size(), os);
  }
  static void outputLength(FOTBuilder::Length n, OutputCharStream &);
private:
  HtmlFOTBuilder(const HtmlFOTBuilder &);
  void operator=(const HtmlFOTBuilder &);
  
  struct DestInfo : public Link {
    DestInfo(IList<Item> *p) : list(p) { }
    IList<Item> *list;
  };

  void insertAddr(size_t, size_t);
  void outputStyleSheet(StringC &);
  void startDisplay(const DisplayNIC &);
  void endDisplay();
  Addressable *elementAddress(size_t g, size_t e);

  const CharStyleClass *makeCharStyleClass();
  const ParaStyleClass *makeParaStyleClass();
  ClassPrefix *makeClassPrefix(const StringC &);

  OwnerTable<CharStyle, CharProps, CharProps, CharStyle> charStyleTable_;
  OwnerTable<ParaStyle, ParaProps, ParaProps, ParaStyle> paraStyleTable_;
  OwnerTable<ClassPrefix, StringC, Hash, ClassPrefix> prefixTable_;
  Vector<FlowObjectInfo> flowObjectStack_;
  FlowObjectInfo nextFlowObject_;
  IList<DestInfo> destStack_;
  IList<Item> *dest_;
  Container root_;
  // Number of HTML documents
  unsigned nDocuments_;
  Vector<Vector<Addressable *> > elements_;
  Vector<Vector<size_t> > pendingAddr_;
  String<CmdLineApp::AppChar> outputFilename_;
  CmdLineApp *app_;
  Vector<StringC> giStack_;
  Length topMargin_;
  Vector<Length> spaceAfterStack_;
};

FOTBuilder *makeHtmlFOTBuilder(const String<CmdLineApp::AppChar> &outputFilename,
			       CmdLineApp *app,
			       const FOTBuilder::Extension *&ext)
{
  static const FOTBuilder::Extension extensions[] = {
    { "UNREGISTERED::James Clark//Characteristic::scroll-title", 0,
      (void (FOTBuilder::*)(const StringC &))&HtmlFOTBuilder::setScrollTitle },
    { 0, 0, 0}
  };
  ext = extensions;
  return new HtmlFOTBuilder(outputFilename, app);
}

static
void reverse(IList<HtmlFOTBuilder::Item> &list)
{
  IList<HtmlFOTBuilder::Item> tem;
  while (!list.empty())
    tem.insert(list.get());
  tem.swap(list);
}

HtmlFOTBuilder::HtmlFOTBuilder(const String<CmdLineApp::AppChar> &outputFilename,
			       CmdLineApp *app)
: outputFilename_(outputFilename),
  app_(app),
  nDocuments_(0),
  giStack_(1),
  topMargin_(0)
{
  flowObjectStack_.push_back(nextFlowObject_);
  dest_ = root_.contentPtr();
  giStack_[0] += 'S';
}

HtmlFOTBuilder::~HtmlFOTBuilder()
{
  reverse(*dest_);
  while (!destStack_.empty()) {
    reverse(*destStack_.head()->list);
    delete destStack_.get();
  }
  StringC styleSheetFilename;
  outputStyleSheet(styleSheetFilename);
  OutputState state(&outputFilename_, styleSheetFilename, app_);
  DiscardOutputCharStream os;
  root_.output(os, state);
  for (size_t i = 0; i < elements_.size(); i++) {
    for (size_t j = 0; j < elements_[i].size(); j++)
      if (elements_[i][j] && !elements_[i][j]->defined())
	delete elements_[i][j];
  }
}

void HtmlFOTBuilder::outputStyleSheet(StringC &styleSheetFilename)
{
  String<CmdLineApp::AppChar> filename(outputFilename_);
  for (size_t j = 0; j < 5; j++) {
    if (filename.size() < j + 1)
      break;
    Char c = filename[filename.size() - j - 1];
    if (c == '/' || c == '\\')
      break;
    if (c == '.') {
      filename.resize(filename.size() - j - 1);
      break;
    }
  }
  filename += '.';
  filename += 'c';
  filename += 's';
  filename += 's';
  filename += 0;
  FileOutputByteStream file;
  if (!file.open(filename.data())) {
    app_->message(HtmlMessages::cannotOpenOutputError,
		  StringMessageArg(app_->convertInput(filename.data())),
		  ErrnoMessageArg(errno));
    return;
  }
  styleSheetFilename = app_->convertInput(filename.data());
  RecordOutputCharStream os(new EncodeOutputCharStream(&file,
						       app_->outputCodingSystem()));

  {
    OwnerTableIter<CharStyle, CharProps, CharProps, CharStyle> iter(charStyleTable_);
    for (;;) {
      const CharStyle *style = iter.next();
      if (!style)
	break;
      style->output(os);
    }
  }
  os << "DIV { margin-top: 0pt; margin-bottom: 0pt; margin-left: 0pt; margin-right: 0pt }"
     << RE;
  {
    OwnerTableIter<ParaStyle, ParaProps, ParaProps, ParaStyle> iter(paraStyleTable_);
    for (;;) {
      const ParaStyle *style = iter.next();
      if (!style)
	break;
      style->output(os);
    }
  }
}

void HtmlFOTBuilder::CharStyle::output(OutputCharStream &os) const
{
  bool first = 1;
  for (IListIter<CharStyleClass> iter(classes); !iter.done(); iter.next()) {
    if (first)
      first = 0;
    else
      os << ", ";
    os << "SPAN.";
    iter.cur()->outputName(os);
  }
  if (!first) {
    os << " {" << RE;
    static const char indent[] = "  ";
    // Netscape Communicator preview release 2 can't handle quoted font family names.
    os << indent << "font-family: " << fontFamily << ';' << RE;
    os << indent << "font-weight: " << int(fontWeight) << "00;" << RE;
    static const char *styleNames[3] = {
      "normal", "italic", "oblique"
    };
    os << indent << "font-style: " << styleNames[fontStyle] << ';' << RE;
    os << indent << "font-size: ";
    outputLength(fontSize, os);
    os << ';' << RE;
    os << indent << "color: #";
    for (int i = 20; i >= 0; i -= 4)
      os << "0123456789abcdef"[(color >> i) & 0xf];
    os << ';' << RE;
    os << '}' << RE;
  }
}

void HtmlFOTBuilder::ParaStyle::output(OutputCharStream &os) const
{
  bool first = 1;
  for (IListIter<ParaStyleClass> iter(classes); !iter.done(); iter.next()) {
    if (first)
      first = 0;
    else
      os << ", ";
    os << "DIV.";
    iter.cur()->outputName(os);
  }
  if (!first) {
    os << " {" << RE;
    static const char indent[] = "  ";
    if (leftMargin) {
      os << indent << "margin-left: ";
      outputLength(leftMargin, os);
      os << ';' << RE;
    }
    if (rightMargin) {
      os << indent << "margin-right: ";
      outputLength(rightMargin, os);
      os << ';' << RE;
    }
    if (topMargin) {
      os << indent << "margin-top: ";
      outputLength(topMargin, os);
      os << ';' << RE;
    }
    static const char *alignNames[4] = {
      "left", "center", "right", "justify"
    };
    os << indent << "text-align: " << alignNames[align] << ';' << RE;
    os << indent << "line-height: ";
    outputLength(lineHeight, os);
    os << ';' << RE;
    os << indent << "text-indent: ";
    outputLength(textIndent, os);
    os << ';' << RE;
    os << '}' << RE;
  }
}

void HtmlFOTBuilder::outputLength(FOTBuilder::Length n, OutputCharStream &os)
{
  char buf[32];
  sprintf(buf, "%04ld", long(n));
  char *end = strchr(buf, '\0');
  for (int i = 0; i < 4; i++)
    end[1 - i] = end[-i];
  end[-3] = '.';
  while (*end == '0')
    *end-- = '\0';
  if (*end == '.')
    *end = '\0';
  strcat(buf, "pt");
  os << buf;
}

void HtmlFOTBuilder::start()
{
  flowObjectStack_.push_back(nextFlowObject_);
}

void HtmlFOTBuilder::end()
{
  flowObjectStack_.resize(flowObjectStack_.size() - 1);
  nextFlowObject_ = flowObjectStack_.back();
}

void HtmlFOTBuilder::atomic()
{
  nextFlowObject_ = flowObjectStack_.back();
}

void HtmlFOTBuilder::formattingInstruction(const StringC &s)
{
  dest_->insert(new Markup(s));
  atomic();
}

const HtmlFOTBuilder::CharStyleClass *HtmlFOTBuilder::makeCharStyleClass()
{
  CharStyle *style = charStyleTable_.lookup(nextFlowObject_);
  if (!style) {
    style = new CharStyle(nextFlowObject_);
    charStyleTable_.insert(style);
  }
  for (IListIter<CharStyleClass> iter(style->classes); !iter.done(); iter.next())
    if (iter.cur()->gi == giStack_.back())
      return iter.cur();
  ClassPrefix *prefix = makeClassPrefix(giStack_.back());
  CharStyleClass *sc = new CharStyleClass(giStack_.back(), prefix, style);
  style->classes.insert(sc);
  return sc;
}

const HtmlFOTBuilder::ParaStyleClass *HtmlFOTBuilder::makeParaStyleClass()
{
  ParaProps props(nextFlowObject_);
  props.topMargin = topMargin_;
  topMargin_ = 0;
  ParaStyle *style = paraStyleTable_.lookup(props);
  if (!style) {
    style = new ParaStyle(props);
    paraStyleTable_.insert(style);
  }
  for (IListIter<ParaStyleClass> iter(style->classes); !iter.done(); iter.next())
    if (iter.cur()->gi == giStack_.back())
      return iter.cur();
  ClassPrefix *prefix = makeClassPrefix(giStack_.back());
  ParaStyleClass *sc = new ParaStyleClass(giStack_.back(), prefix, style);
  style->classes.insert(sc);
  return sc;
}

HtmlFOTBuilder::StyleClass::StyleClass(const StringC &g, ClassPrefix *pfx)
: gi(g), prefix(pfx), prefixIndex(pfx->nCharClasses + pfx->nParaClasses + 1)
{

  
}

HtmlFOTBuilder::CharStyleClass::CharStyleClass(const StringC &g, ClassPrefix *pfx,
					       const CharStyle *s)
: StyleClass(g, pfx), style(s)
{
  pfx->nCharClasses += 1;
}

HtmlFOTBuilder::ParaStyleClass::ParaStyleClass(const StringC &g, ClassPrefix *pfx,
					       const ParaStyle *s)
: StyleClass(g, pfx), style(s)
{
  pfx->nParaClasses += 1;
}

HtmlFOTBuilder::ClassPrefix *HtmlFOTBuilder::makeClassPrefix(const StringC &gi)
{
  // FIXME strip illegal characters
  ClassPrefix *prefix = prefixTable_.lookup(gi);
  if (!prefix) {
    prefix = new ClassPrefix(gi);
    prefixTable_.insert(prefix);
  }
  return prefix;
}

void HtmlFOTBuilder::charactersFromNode(const NodePtr &node, const Char *s, size_t n)
{
  flushPendingAddresses();
  dest_->insert(new NodePcdata(node, s, n, makeCharStyleClass()));
}

void HtmlFOTBuilder::characters(const Char *s, size_t n)
{
  flushPendingAddresses();
  dest_->insert(new ImmediatePcdata(s, n, makeCharStyleClass()));
}

void HtmlFOTBuilder::startParagraph(const ParagraphNIC &nic)
{
  startDisplay(nic);
  Block *block = new Block(makeParaStyleClass());
  nextFlowObject_.parentLeftMargin += nextFlowObject_.leftMargin;
  nextFlowObject_.parentRightMargin += nextFlowObject_.rightMargin;
  nextFlowObject_.leftMargin = 0;
  nextFlowObject_.rightMargin = 0;
  dest_->insert(block);
  destStack_.insert(new DestInfo(dest_));
  dest_ = block->contentPtr();
  start();
}

void HtmlFOTBuilder::endParagraph()
{
  reverse(*dest_);
  dest_ = destStack_.head()->list;
  delete destStack_.get();
  end();
  endDisplay();
}

void HtmlFOTBuilder::startDisplay(const DisplayNIC &nic)
{
  Length spaceBefore = nic.spaceBefore.nominal.length;
  if (spaceBefore > topMargin_)
    topMargin_ = spaceBefore;
  spaceAfterStack_.push_back(nic.spaceAfter.nominal.length);
}

void HtmlFOTBuilder::endDisplay()
{
  const Length &spaceAfter = spaceAfterStack_.back();
  if (spaceAfter > topMargin_)
    topMargin_ = spaceAfter;
  spaceAfterStack_.resize(spaceAfterStack_.size() - 1);
}

void HtmlFOTBuilder::startScroll()
{
  nextFlowObject_.docIndex = nDocuments_++;
  start();
  Document *doc = new Document(nextFlowObject_.docIndex,
			       nextFlowObject_.scrollTitle);
  dest_->insert(doc);
  destStack_.insert(new DestInfo(dest_));
  dest_ = doc->contentPtr();
  if (pendingAddr_.size()) {
    Vector<size_t> &v = pendingAddr_.back();
    for (size_t i = 0; i < v.size(); i += 2) {
      Addressable *tem = elementAddress(v[i], v[i + 1]);
      if (!tem->defined()) {
        dest_->insert(tem);
	tem->setDefined(nextFlowObject_.docIndex, 1);
      }
    }
    v.resize(0);
  }
}

void HtmlFOTBuilder::endScroll()
{
  reverse(*dest_);
  dest_ = destStack_.head()->list;
  delete destStack_.get();
  end();
}

void HtmlFOTBuilder::startLink(const Address &addr)
{
  start();
  AddressRef *aref = 0;
  switch (addr.type) {
  case Address::resolvedNode:
    {
      unsigned long n;
      if (addr.node->elementIndex(n) == accessOK) {
	Addressable *tem = elementAddress(addr.node->groveIndex(), n);
	tem->setReferenced();
	aref = tem;
      }
      break;
    }
  case Address::idref:
    {
      const StringC &id = addr.params[0];
      size_t i;
      for (i = 0; i < id.size(); i++)
	if (id[i] == ' ')
	  break;
      NodePtr node;
      NamedNodeListPtr elements;
      unsigned long n;
      if (addr.node->getGroveRoot(node) == accessOK
	  && node->getElements(elements) == accessOK
	  && elements->namedNode(GroveString(id.data(), i), node) == accessOK
	  && node->elementIndex(n) == accessOK) {
	Addressable *tem = elementAddress(addr.node->groveIndex(), n);
	tem->setReferenced();
	aref = tem;
      }
      break;
    }
  default:
    break;
  }
  Ref *ref = new Ref(aref);
  dest_->insert(ref);
  destStack_.insert(new DestInfo(dest_));
  dest_ = ref->contentPtr();
}

void HtmlFOTBuilder::endLink()
{
  reverse(*dest_);
  dest_ = destStack_.head()->list;
  delete destStack_.get();
  end();
}

HtmlFOTBuilder::Addressable *HtmlFOTBuilder::elementAddress(size_t g, size_t e)
{
  if (g >= elements_.size())
    elements_.resize(g + 1);
  Vector<Addressable *> &v = elements_[g];
  for (size_t i = v.size(); i <= e; i++)
    v.push_back((Addressable *)0);
  if (!v[e])
    v[e] = new Addressable(g, e);
  return v[e];
}

void HtmlFOTBuilder::startNode(const NodePtr &node, const StringC &mode)
{
  pendingAddr_.resize(pendingAddr_.size() + 1);
  if (mode.size() == 0) {
    if (pendingAddr_.size() > 1)
      pendingAddr_.back() = pendingAddr_[pendingAddr_.size() - 2];
    unsigned long n;
    if (node->elementIndex(n) == accessOK) {
      pendingAddr_.back().push_back(node->groveIndex());
      pendingAddr_.back().push_back(size_t(n));
    }
  }
  giStack_.resize(giStack_.size() + 1);
  GroveString str;
  if (node->getGi(str) == accessOK)
    giStack_.back().assign(str.data(), str.size());
  else if (giStack_.size() >= 2)
    giStack_.back() = giStack_[giStack_.size() - 2];
}

void HtmlFOTBuilder::endNode()
{
  pendingAddr_.resize(pendingAddr_.size() - 1);
  giStack_.resize(giStack_.size() - 1);
}

void HtmlFOTBuilder::insertAddr(size_t g, size_t e)
{
  Addressable *tem = elementAddress(g, e);
  if (!tem->defined() && nextFlowObject_.docIndex != (unsigned)-1) {
    dest_->insert(tem);
    tem->setDefined(nextFlowObject_.docIndex);
  }
}

void HtmlFOTBuilder::setLineSpacing(const LengthSpec &val)
{
  nextFlowObject_.lineHeight = val.length;
}

void HtmlFOTBuilder::setQuadding(Symbol sym)
{
  switch (sym) {
  case symbolStart:
    nextFlowObject_.align = ParaProps::alignLeft;
    break;
  case symbolEnd:
    nextFlowObject_.align = ParaProps::alignRight;
    break;
  case symbolCenter:
    nextFlowObject_.align = ParaProps::alignCenter;
    break;
  case symbolJustify:
    nextFlowObject_.align = ParaProps::alignJustify;
    break;
  default:
    break;
  }
}

void HtmlFOTBuilder::setFirstLineStartIndent(const LengthSpec &ls)
{
  nextFlowObject_.textIndent = ls.length;
}

void HtmlFOTBuilder::setStartIndent(const LengthSpec &ls)
{
  nextFlowObject_.leftMargin = ls.length - nextFlowObject_.parentLeftMargin;
}

void HtmlFOTBuilder::setEndIndent(const LengthSpec &ls)
{
  nextFlowObject_.rightMargin = ls.length - nextFlowObject_.parentRightMargin;
}

void HtmlFOTBuilder::setFontFamilyName(const StringC &name)
{
  nextFlowObject_.fontFamily = name;
}

void HtmlFOTBuilder::setFontWeight(Symbol weight)
{
  switch (weight) {
  case symbolUltraLight:
    nextFlowObject_.fontWeight = 1;
    break;
  case symbolExtraLight:
    nextFlowObject_.fontWeight = 2;
    break;
  case symbolLight:
    nextFlowObject_.fontWeight = 3;
    break;
  case symbolSemiLight:
    nextFlowObject_.fontWeight = 4;
    break;
  case symbolMedium:
    nextFlowObject_.fontWeight = 5;
    break;
  case symbolSemiBold:
    nextFlowObject_.fontWeight = 6;
    break;
  case symbolBold:
    nextFlowObject_.fontWeight = 7;
    break;
  case symbolExtraBold:
    nextFlowObject_.fontWeight = 8;
    break;
  case symbolUltraBold:
    nextFlowObject_.fontWeight = 9;
    break;
  default:
    break;
  }
}

void HtmlFOTBuilder::setFontPosture(Symbol posture)
{
  switch (posture) {
  case symbolOblique:
    nextFlowObject_.fontStyle = CharProps::styleOblique;
    break;
  case symbolItalic:
    nextFlowObject_.fontStyle = CharProps::styleItalic;
    break;
  default:
    nextFlowObject_.fontStyle = CharProps::styleNormal;
    break;
  }
}

void HtmlFOTBuilder::setFontSize(Length n)
{
  nextFlowObject_.fontSize = n;
}

void HtmlFOTBuilder::setColor(const DeviceRGBColor &color)
{
  nextFlowObject_.color = (color.red << 16) | (color.green << 8) | color.blue;
}

void HtmlFOTBuilder::setScrollTitle(const StringC &s)
{
  nextFlowObject_.scrollTitle = new StringResource<Char>(s);
}

void HtmlFOTBuilder::flushPendingAddresses()
{
  if (pendingAddr_.size()) {
    for (size_t i = 0; i < pendingAddr_.back().size(); i += 2)
      insertAddr(pendingAddr_.back()[i], pendingAddr_.back()[i + 1]);
    pendingAddr_.back().resize(0);
  }
}

void HtmlFOTBuilder::outputCdata(const Char *s, size_t n, OutputCharStream &os)
{
  for (; n > 0; n--, s++) {
    switch (*s) {
    case '\n':
      break;
    case '&':
      os << "&amp;";
      break;
    case '<':
      os << "&lt;";
      break;
    case '>':
      os << "&gt;";
      break;
    case '\r':
      os << RE;
      break;
    default:
      if (*s >= 255)
	os << "&#" << (unsigned long)*s << ';';
      else
	os.put(*s);
      break;
    }
  }
}

void HtmlFOTBuilder::Document::output(OutputCharStream &, OutputState &oldState)
{
  OutputState state(&oldState.outputFilename(),
                    oldState.styleSheetFilename(),
		    &oldState.app());
  FileOutputByteStream file;
  String<CmdLineApp::AppChar> filename(state.outputFilename(index_));
  filename += 0;
  if (!file.open(filename.data())) {
    state.messenger()
      .message(HtmlMessages::cannotOpenOutputError,
               StringMessageArg(state.app().convertInput(filename.data())),
	       ErrnoMessageArg(errno));
    return;
  }
  RecordOutputCharStream os(new EncodeOutputCharStream(&file,
						    state.app().outputCodingSystem()));
  // FIXME setEscaper
  unsigned oldDocIndex = state.setOutputDocIndex(index_);
  os << "<HTML>" << RE;
  const StringC &styleSheetFilename = state.styleSheetFilename();
  if (styleSheetFilename.size()) {
    os << "<LINK REL=STYLESHEET TYPE=\"text/css\" HREF=\"";
    size_t i = styleSheetFilename.size();
    for (; i > 0; i--)
      if (styleSheetFilename[i - 1] == '\\' || styleSheetFilename[i - 1] == '/')
    	break;
    os.write(styleSheetFilename.data() + i, styleSheetFilename.size() - i);
    os << "\">" << RE;
  }
  if (!title_.isNull()) {
    os << "<TITLE>";
    HtmlFOTBuilder::outputCdata(*title_, os);
    os << "</TITLE>";
  }
  os << "<BODY>" << RE;
  Container::output(os, state);
  os << "</BODY>" << RE;
  os << "</HTML>" << RE;
  state.setOutputDocIndex(oldDocIndex);
}

void HtmlFOTBuilder::Container::output(OutputCharStream &os, OutputState &state)
{
  for (IListIter<Item> iter(content_); !iter.done(); iter.next())
    iter.cur()->output(os, state);
}

void HtmlFOTBuilder::Block::output(OutputCharStream &os, OutputState &state)
{
  const AddressRef *addr = state.setAddress(0);
  state.syncChar(0, os);
  os << "<DIV CLASS=";
  styleClass_->outputName(os);
  os << RE <<'>';
  state.setAddress(addr);
  Container::output(os, state);
  state.setAddress(0);
  state.syncChar(0, os);
  os << "</DIV>";
  state.setAddress(addr);
}

void HtmlFOTBuilder::Ref::output(OutputCharStream &os, OutputState &state)
{
  const AddressRef *oldAref = state.setAddress(aref_);
  Container::output(os, state);
  state.setAddress(oldAref);
}

void HtmlFOTBuilder::Pcdata::output(OutputCharStream &os, OutputState &state)
{
  size_t n;
  const Char *s = data(n);
  state.syncChar(styleClass_, os);
  HtmlFOTBuilder::outputCdata(s, n, os);
}

void HtmlFOTBuilder::Markup::output(OutputCharStream &os, OutputState &state)
{
  state.syncChar(0, os);
  os << str_;
}

void HtmlFOTBuilder::Addressable::output(OutputCharStream &os, OutputState &)
{
  ASSERT(defined());
  if (referenced_ && elementIndex_ != (unsigned)-1) {
    os << "<A NAME=";
    if (groveIndex_)
      os << (unsigned long)groveIndex_ << '.';
    os << (unsigned long)elementIndex_ << "></A>";
  }
}

void HtmlFOTBuilder::Addressable::outputRef(bool end, OutputCharStream &os,
					    OutputState &state) const
{
  ASSERT(referenced_);
  if (defined()) {
    if (end)
      os << "</A>";
    else {
      os << "<A HREF=\"";
      if (state.outputDocIndex() != docIndex_ || elementIndex_ == (unsigned)-1) {
	const String<CmdLineApp::AppChar> &outputFilename = state.outputFilename(docIndex_);
	size_t i = outputFilename.size();
	for (; i > 0; i--)
	  if (outputFilename[i - 1] == '\\' || outputFilename[i - 1] == '/')
	     break;
	for (; i < outputFilename.size(); i++)
	  os.put(outputFilename[i]);
      }
      if (elementIndex_ != (unsigned)-1) {
	os << "#";
	if (groveIndex_)
	  os << (unsigned long)groveIndex_ << '.';
	os << (unsigned long)elementIndex_;
      }
      os << "\">";
    }
  }
}

String<CmdLineApp::AppChar> HtmlFOTBuilder::OutputState::outputFilename(unsigned index) const
{
  if (index == 0)
    return *outputFilename_;
  String<CmdLineApp::AppChar> base(*outputFilename_);
  String<CmdLineApp::AppChar> ext;
  for (size_t j = 0; j < 5; j++) {
    if (base.size() < j + 1)
      break;
    Char c = base[base.size() - j - 1];
    if (c == '/' || c == '\\')
      break;
    if (c == '.') {
      ext.assign(base.data() + (base.size() - j - 1), j + 1);
      base.resize(base.size() - j - 1);
      break;
    }
  }
  char buf[32];
  sprintf(buf, "%u", index);
  for (const char *p = buf; *p; p++)
    base += CmdLineApp::AppChar(*p);
  base += ext;
  return base;
}

void HtmlFOTBuilder::OutputState::syncChar(const CharStyleClass *styleClass,
					   OutputCharStream &os)
{
  // <A><SPAN>...</SPAN></A>
  if (curAref_ != specAref_) {
    if (curCharStyleClass_) {
      os << "</SPAN>";
      curCharStyleClass_ = 0;
    }
    if (curAref_)
      curAref_->outputRef(1, os, *this);
    if (specAref_)
      specAref_->outputRef(0, os, *this);
    curAref_ = specAref_;
  }
  if (curCharStyleClass_ != styleClass) {
    if (curCharStyleClass_)
      os << "</SPAN>";
    if (styleClass) {
      os << "<SPAN CLASS=";
      styleClass->outputName(os);
      os << '>';
    }
    curCharStyleClass_ = styleClass;
  }
}

HtmlFOTBuilder::AddressRef::~AddressRef()
{
}

HtmlFOTBuilder::Item::~Item()
{
}

HtmlFOTBuilder::Document::~Document()
{
}

HtmlFOTBuilder::CharProps::CharProps()
: fontWeight(5), color(0), fontStyle(styleNormal), fontSize(10*1000)
{
  for (const char *p = "Times New Roman,serif"; *p; p++)
    fontFamily += *p;
}

HtmlFOTBuilder::InheritParaProps::InheritParaProps()
: leftMargin(0), rightMargin(0), align(alignLeft), lineHeight(12*1000),
  textIndent(0)
{
}

HtmlFOTBuilder::FlowObjectInfo::FlowObjectInfo()
: docIndex(unsigned(-1)), parentLeftMargin(0), parentRightMargin(0)
{
}

void HtmlFOTBuilder::StyleClass::outputName(OutputCharStream &os) const
{
  os << prefix->prefix;
  if (prefix->nCharClasses > 1 || prefix->nParaClasses > 1)
    os << (unsigned long)prefixIndex;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#include "HtmlFOTBuilder_inst.cxx"

#endif /* JADE_HTML */
