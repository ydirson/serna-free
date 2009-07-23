// Copyright (c) 1997 James Clark
// See the file COPYING for copying permission.

#ifndef XmlOutputEventHandler_INCLUDED
#define XmlOutputEventHandler_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif

#include "ErrorCountEventHandler.h"
#include "Message.h"
#include "OutputCharStream.h"
#include "Boolean.h"
#include "CharsetInfo.h"
#include "ExtendEntityManager.h"
#include "Ptr.h"
#include "SubstTable.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class CharsetInfo;

class XmlOutputEventHandler : public ErrorCountEventHandler {
public:
  struct Options {
    Options();
    PackedBoolean nlInTag;
    PackedBoolean id;
    PackedBoolean notation;
    PackedBoolean ndata;
    PackedBoolean comment;
    PackedBoolean cdata;
    PackedBoolean lower;
    PackedBoolean piEscape;
    PackedBoolean empty;
    PackedBoolean attlist;
  };
  XmlOutputEventHandler(const Options &,
			OutputCharStream *,
			const StringC &encodingName,
		        const Ptr<ExtendEntityManager> &,
	  		const CharsetInfo &,
			Messenger *);
  ~XmlOutputEventHandler();
  void data(DataEvent *);
  void startElement(StartElementEvent *);
  void endElement(EndElementEvent *);
  void pi(PiEvent *);
  void startDtd(StartDtdEvent *);
  void endDtd(EndDtdEvent *);
  void endProlog(EndPrologEvent *event);
  void sdataEntity(SdataEntityEvent *);
  void externalDataEntity(ExternalDataEntityEvent *);
  void subdocEntity(SubdocEntityEvent *);
  void commentDecl(CommentDeclEvent *);
  void markedSectionStart(MarkedSectionStartEvent *);
  void markedSectionEnd(MarkedSectionEndEvent *);
  void message(MessageEvent *);
  void sgmlDecl(SgmlDeclEvent *);
private:
  XmlOutputEventHandler(const XmlOutputEventHandler &); // undefined
  void operator=(const XmlOutputEventHandler &); // undefined
  OutputCharStream &os();
  void outputData(const Char *s, size_t n, Boolean inLit);
  void outputCdata(const Char *s, size_t n);
  void outputExternalId(const EntityDecl &decl);
  void outputAttribute(const AttributeList &attributes, size_t i);
  int fsiToUrl(const StringC &fsi, const Location &loc, StringC &url);
  int filenameToUrl(const StringC &filename, const Location &loc, StringC &url);
  void maybeStartDoctype(Boolean &doctypeStarted, const Dtd &dtd);
  void closeCdataSection();
  const StringC &generalName(const StringC &name, StringC &buf);

  Messenger *mgr_;
  Ptr<ExtendEntityManager> entityManager_;
  const CharsetInfo *systemCharset_;
  OutputCharStream *os_;
  Boolean inDtd_;
  Boolean useCdata_;
  Boolean inCdata_;
  int nCdataEndMatched_;
  Options options_;
  Boolean namecaseGeneral_;
  SubstTable<Char> lowerSubst_;
  StringC nameBuf_;
};

inline
OutputCharStream &XmlOutputEventHandler::os()
{
  return *os_;
}

#ifdef SP_NAMESPACE
}
#endif

#endif /* not XmlOutputEventHandler_INCLUDED */
