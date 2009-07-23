// Copyright (c) 1997 James Clark
// See the file COPYING for copying permission.

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "XmlOutputEventHandler.h"
#include "XmlOutputMessages.h"
#include "Message.h"
#include "MessageArg.h"
#include "macros.h"
#if 0
#include "UTF8CodingSystem.h"
#endif
#include "InputSource.h"
#include "StorageManager.h"
#include <string.h>

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

inline
void operator+=(StringC &str, const char *s)
{
  while (*s)
    str += *s++;
}

inline
void XmlOutputEventHandler::closeCdataSection()
{
  useCdata_ = 0;
  if (inCdata_) {
    os() << "]]>";
    inCdata_ = 0;
    nCdataEndMatched_ = 0;
  }
}

const char RE = '\r';

XmlOutputEventHandler::Options::Options()
{
  memset(this, 0, sizeof(*this));
}

static
void escape(OutputCharStream &s, Char c)
{
  s << "&#" << (unsigned long)c << ";";
}

XmlOutputEventHandler::XmlOutputEventHandler(const Options &options,
					     OutputCharStream *os,
					     const StringC &encodingName,
					     const Ptr<ExtendEntityManager> &entityManager,
					     const CharsetInfo &systemCharset,
					     Messenger *mgr)
: options_(options),
  os_(os),
  entityManager_(entityManager),
  systemCharset_(&systemCharset),
  mgr_(mgr),
  inDtd_(0),
  inCdata_(0),
  useCdata_(0),
  nCdataEndMatched_(0),
  namecaseGeneral_(0)
{
  os_->setEscaper(escape);
  *os_ << "<?xml version=\"1.0\"";
  if (encodingName.size())
    *os_ << " encoding=\"" << encodingName << '"';
  *os_ << "?>" << RE;
}

XmlOutputEventHandler::~XmlOutputEventHandler()
{
  os() << RE;
  delete os_;
}

void XmlOutputEventHandler::message(MessageEvent *event)
{
  mgr_->dispatchMessage(event->message());
  ErrorCountEventHandler::message(event);
}

void XmlOutputEventHandler::sgmlDecl(SgmlDeclEvent *event)
{
  const Syntax &syntax = event->instanceSyntax();
  if (syntax.namecaseGeneral()) {
    namecaseGeneral_ = 1;
    syntax.generalSubstTable()->inverseTable(lowerSubst_);
  }
  delete event;
}

void XmlOutputEventHandler::data(DataEvent *event)
{
  if (useCdata_)
    outputCdata(event->data(), event->dataLength());
  else
    outputData(event->data(), event->dataLength(), 0);
  delete event;
}

void XmlOutputEventHandler::startElement(StartElementEvent *event)
{
  // This shouldn't happen (although useCdata_ can be true).
  if (inCdata_) {
    inCdata_ = 0;
    nCdataEndMatched_ = 0;
    os() << "]]>";
  }
  os() << '<' << generalName(event->name(), nameBuf_);
  size_t nAttributes = event->attributes().size();
  for (size_t i = 0; i < nAttributes; i++)
    outputAttribute(event->attributes(), i);
  if (options_.nlInTag)
    os() << RE;
  if (options_.cdata
      && event->elementType()->definition()->declaredContent() == ElementDefinition::cdata)
    useCdata_ = 1;
  if (options_.empty
      && event->elementType()->definition()->declaredContent() == ElementDefinition::empty)
    os() << "/>";
  else
    os() << '>';
  delete event;
}

void XmlOutputEventHandler::outputAttribute(const AttributeList &attributes, size_t i)
{
  const AttributeValue *value = attributes.value(i);
  if (!value)
    return;
  const Text *text;
  const StringC *string;
  AttributeValue::Type type = value->info(text, string);
  if (type == AttributeValue::implied)
    return;
  if (options_.nlInTag)
    os() << RE;
  else
    os() << ' ';
  os() << generalName(attributes.name(i), nameBuf_) << "=\"";
  if (type == AttributeValue::cdata) {
    TextIter iter(*text);
    TextItem::Type type;
    const Char *p;
    size_t length;
    const Location *loc;
    while (iter.next(type, p, length, loc)) {
      switch (type) {
      case TextItem::data:
      case TextItem::cdata:
	outputData(p, length, 1);
	break;
      case TextItem::sdata:
	{
	  mgr_->setNextLocation(loc->origin()->parent());
	  const Entity *entity = loc->origin()->asEntityOrigin()->entity();
	  mgr_->message(XmlOutputMessages::sdataEntityReference,
			StringMessageArg(entity->name()));
	  os() << '&' << entity->name() << ';';
	}
	break;
      case TextItem::nonSgml:
	// FIXME
  	break;
      default:
	break;
      }
    }
  }
  else if (attributes.def()->def(i)->isEntity())
    os() << *string;
  else
    os() << generalName(*string, nameBuf_);
  os() << '"';
}

void XmlOutputEventHandler::endElement(EndElementEvent *event)
{
  closeCdataSection();
  if (options_.empty
      && event->elementType()->definition()->declaredContent() == ElementDefinition::empty)
    ;
  else {
    os() << "</" << generalName(event->name(), nameBuf_);
#if 0
    if (options_.nlInTag)
      os() << RE;
#endif
    os() << '>';
  }
  delete event;
}

static Boolean isXmlS(Char c)
{
  switch (c) {
  case ' ':
  case '\r':
  case '\n':
  case '\t':
    return 1;
  }
  return 0;
}

static Boolean containsQuestionLt(const Char *s, size_t n)
{
  for (; n > 1; n--, s++)
    if (*s == '?' && s[1] == '>')
      return 1;
  return 0;
}

// These do not handle Unicode chars properly.
// They treat all Unicode chars >= 170 as name start characters

static Boolean isXmlNameStartChar(Char c)
{
  switch (c) {
  case ':':
  case '.':
  case '_':
  case '-':
    return 1;
  }
  if (c >= 170)
    return 1;
  if ('a' <= c && c <= 'z')
    return 1;
  if ('A' <= c && c <= 'Z')
    return 1;
  return 0;
}

static Boolean isXmlNameChar(Char c)
{
  if (isXmlNameStartChar(c))
    return 1;
  if ('0' <= c && c <= '9')
    return 1;
  return 0;
}

static Boolean startsWithXmlName(const Char *s, size_t n)
{
  if (n == 0)
    return 0;
  if (!isXmlNameStartChar(*s))
    return 0;
  for (s++, n--; n > 0; n--, s++) {
    if (isXmlS(*s))
      return 1;
    if (!isXmlNameChar(*s))
      return 0;
  }
  return 1;
}

void XmlOutputEventHandler::pi(PiEvent *event)
{
  const Char *s = event->data();
  size_t n = event->dataLength();

  if (n >= 3 && s[0] == 'x' && s[1] == 'm' && s[2] == 'l'
           && (n == 3 || isXmlS(s[3])))
    ; // Probably came from an encoding PI.
  else if (!startsWithXmlName(s, n)) {
    mgr_->setNextLocation(event->location());
    mgr_->message(XmlOutputMessages::piNoName);
  }
  else if (options_.piEscape) {
    os() << "<?";
    outputData(s, n, 0);
    os() << "?>";
  }
  else if (containsQuestionLt(s, n)) {
    mgr_->setNextLocation(event->location());
    mgr_->message(XmlOutputMessages::piQuestionLt);
  }
  else {
    os() << "<?";
    os().write(s, n);
    os() << "?>";
  }
  delete event;
}

void XmlOutputEventHandler::sdataEntity(SdataEntityEvent *event)
{
  mgr_->setNextLocation(event->location().origin()->parent());
  mgr_->message(XmlOutputMessages::sdataEntityReference,
                StringMessageArg(event->entity()->name()));
  os() << '&' << event->entity()->name() << ';';
  delete event;
}

void XmlOutputEventHandler::externalDataEntity(ExternalDataEntityEvent *event)
{
  mgr_->setNextLocation(event->location().origin()->parent());
  mgr_->message(XmlOutputMessages::externalDataEntityReference,
                StringMessageArg(event->entity()->name()));
  os() << (options_.lower ? "<entity name=\"" : "<ENTITY NAME=\"")
       << event->entity()->name() << "\"/>";
  delete event;
}

void XmlOutputEventHandler::subdocEntity(SubdocEntityEvent *event)
{
  mgr_->setNextLocation(event->location().origin()->parent());
  mgr_->message(XmlOutputMessages::subdocEntityReference,
                StringMessageArg(event->entity()->name()));
  os() << (options_.lower ? "<entity name=\"" : "<ENTITY NAME=\"")
       << event->entity()->name() << "\"/>";
  delete event;
}

void XmlOutputEventHandler::startDtd(StartDtdEvent *event)
{
  inDtd_ = 1;
  delete event;
}

void XmlOutputEventHandler::endDtd(EndDtdEvent *event)
{
  inDtd_ = 0;
  delete event;
}

void XmlOutputEventHandler::maybeStartDoctype(Boolean &doctypeStarted, const Dtd &dtd)
{
  if (doctypeStarted)
    return;
  doctypeStarted = 1;
  os() << "<!DOCTYPE " << dtd.documentElementType()->name() << " [" << RE;
}

void XmlOutputEventHandler::endProlog(EndPrologEvent *event)
{
  const Dtd &dtd = event->dtd();
  Boolean doctypeStarted = 0;
  if (options_.notation) {
    Dtd::ConstNotationIter iter(dtd.notationIter());
    for (;;) {
      const Notation *notation = iter.nextTemp();
      if (!notation)
	break;
      maybeStartDoctype(doctypeStarted, dtd);
      os() << "<!NOTATION " << generalName(notation->name(), nameBuf_);
      outputExternalId(*notation);
      os() << ">" << RE;
      if (notation->attributeDefTemp()) {
	mgr_->setNextLocation(notation->defLocation());
	mgr_->message(XmlOutputMessages::notationAttributes,
	              StringMessageArg(notation->name()));
      }
    }
  }
  if (options_.ndata) {
    Dtd::ConstEntityIter iter(dtd.generalEntityIter());
    for (;;) {
      const Entity *entity = iter.nextTemp();
      if (!entity)
	break;
      const ExternalDataEntity *extDataEntity = entity->asExternalDataEntity();
      if (extDataEntity) {
	maybeStartDoctype(doctypeStarted, dtd);
	os() << "<!ENTITY " << entity->name();
	outputExternalId(*entity);
	if (extDataEntity->dataType() != EntityDecl::ndata) {
	  mgr_->setNextLocation(entity->defLocation());
	  mgr_->message(XmlOutputMessages::externalDataNdata,
	                StringMessageArg(entity->name()));
	}
	os() << " NDATA " << generalName(extDataEntity->notation()->name(), nameBuf_) << ">" << RE;
      }	
    }
  }
  if (options_.id || options_.attlist) {
    Dtd::ConstElementTypeIter iter(dtd.elementTypeIter());
    for (;;) {
      const ElementType *elementType = iter.next();
      if (!elementType)
	break;
      const AttributeDefinitionList *adl = elementType->attributeDefTemp();
      if (adl) {
	if (options_.attlist) {
	    os() << "<!ATTLIST " << generalName(elementType->name(), nameBuf_);
	    for (size_t i = 0; i < adl->size(); i++) {
	      const AttributeDefinition *def = adl->def(i);
	      os() << RE << generalName(def->name(), nameBuf_);
	      AttributeDefinitionDesc desc;
	      def->getDesc(desc);
	      switch (desc.declaredValue) {
	      case AttributeDefinitionDesc::cdata:
		os() << " CDATA #IMPLIED";
		break;
	      case AttributeDefinitionDesc::name:
	      case AttributeDefinitionDesc::number:
	      case AttributeDefinitionDesc::nmtoken:
	      case AttributeDefinitionDesc::nutoken:
		os() << " NMTOKEN #IMPLIED";
		break;
	      case AttributeDefinitionDesc::entity:
		os() << " ENTITY #IMPLIED";
		break;
	      case AttributeDefinitionDesc::idref:
		os() << " IDREF #IMPLIED";
		break;
	      case AttributeDefinitionDesc::names:
	      case AttributeDefinitionDesc::numbers:
	      case AttributeDefinitionDesc::nmtokens:
	      case AttributeDefinitionDesc::nutokens:
		os() << " NMTOKENS #IMPLIED";
		break;
	      case AttributeDefinitionDesc::entities:
		os() << " ENTITIES #IMPLIED";
		break;
	      case AttributeDefinitionDesc::idrefs:
		os() << " IDREFS #IMPLIED";
		break;
	      case AttributeDefinitionDesc::id:
		os() << " ID #IMPLIED";
		break;
	      case AttributeDefinitionDesc::notation:
		os() << " NOTATION";
		// fall through
	      case AttributeDefinitionDesc::nameTokenGroup:
		{
		  os() << " (";
		  for (size_t j = 0; j < desc.allowedValues.size(); j++) {
		    if (j > 0)
		      os() << '|';
		    os() << desc.allowedValues[j];
		  }
		  os() << ") #IMPLIED";
		}
		break;
	      default:
		CANNOT_HAPPEN();
	      }
	    }
	    os() << '>' << RE;
	}
	else {
	  size_t idIndex = adl->idIndex();
	  if (idIndex != size_t(-1)) {
	    maybeStartDoctype(doctypeStarted, dtd);
	    os() << "<!ATTLIST " << generalName(elementType->name(), nameBuf_);
	    os() << ' ' << generalName(adl->def(idIndex)->name(), nameBuf_)
	        << " ID #IMPLIED>" << RE;
	  }
	}
      }
    }
  }
  if (doctypeStarted)
    os() << "]>" << RE;
  delete event;
}

void XmlOutputEventHandler::outputExternalId(const EntityDecl &decl)

{
  const StringC *pubIdP = decl.publicIdPointer();
  const StringC *sysIdP = decl.effectiveSystemIdPointer();
  if (pubIdP) {
    os() << " PUBLIC \"" << *pubIdP << "\"";
    if (decl.declType() == EntityDecl::notation && !sysIdP)
      return;
    os() << " \"";
  }
  else
    os() << " SYSTEM \"";
 if (sysIdP) {
   StringC url;
   switch(fsiToUrl(*sysIdP, decl.defLocation(), url)) {
   case 1:
     os() << url;
     break;
   case 0:
     break;
   default:
     mgr_->setNextLocation(decl.defLocation());
     mgr_->message(XmlOutputMessages::cannotConvertFsiToUrl,
                   StringMessageArg(*sysIdP));
     break;
   }
 }
 os() << "\"";
}

// Return 1 if OK; return -1 to generate generic error; return 0 if error already generated.

int XmlOutputEventHandler::fsiToUrl(const StringC &fsi, const Location &loc, StringC &url)
{
  ParsedSystemId parsedBuf;
  if (!entityManager_->parseSystemId(fsi, *systemCharset_, 0, 0, *mgr_, parsedBuf))
    return 0;
  if (parsedBuf.size() != 1)
    return -1;
  if (strcmp(parsedBuf[0].storageManager->type(), "URL") == 0) {
    url = parsedBuf[0].specId;
    return 1;
  }
  Owner<InputSource> in(entityManager_->open(fsi,
					     *systemCharset_,
					     InputSourceOrigin::make(),
					     0,
					     *mgr_));
  if (!in)
    return 0;
  Xchar c = in->get(*mgr_);
  StorageObjectLocation soLoc;
  if (c == InputSource::eE && in->accessError()) {
    if (parsedBuf[0].baseId.size())
      return 0;
    soLoc.storageObjectSpec = &parsedBuf[0];
    soLoc.actualStorageId = parsedBuf[0].specId;
  }
  else {
    const Location &loc = in->currentLocation();
    if (loc.origin().isNull())
      return -1;
    const InputSourceOrigin *tem = loc.origin()->asInputSourceOrigin();
    if (!tem)
      return -1;
    const ParsedSystemId *psi
      = ExtendEntityManager::externalInfoParsedSystemId(tem->externalInfo());
    if (!psi || psi->size() != 1)
      return -1;
    if (!ExtendEntityManager::externalize(tem->externalInfo(), 0, soLoc))
      return -1;
  }
  if (strcmp(soLoc.storageObjectSpec->storageManager->type(), "OSFILE") != 0)
    return -1;
  return filenameToUrl(soLoc.actualStorageId, loc, url);
}

static
StringC hexEncode(unsigned char c)
{
  static const char hexDigits[] = "0123456789ABCDEF";
  StringC result;
  result.resize(2);
  result[0] = hexDigits[c >> 4];
  result[1] = hexDigits[c & 0xF];
  return result;
}

static
StringC urlEncode(Char c)
{
  if (c >= 0x80) {
    StringC tem;
    tem += c;
#if 0
    // This is what the URN spec says.
    UTF8CodingSystem utf8;
    String<char> bytes(utf8.convertOut(tem));
    tem.resize(0);
    for (size_t i = 0; i < bytes.size(); i++)
      tem += hexEncode(bytes[i]);
#endif
    return tem;
  }
  if (strchr("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ$-_.+!*'(),",
             char(c))) {
    StringC tem;
    tem += c;
    return tem;
  }
  return hexEncode((unsigned char)c);
}

int XmlOutputEventHandler::filenameToUrl(const StringC &filename,
					 const Location &loc,
					 StringC &url)
{
#ifdef SP_MSDOS_FILENAMES
  if (filename.size() >= 3 && filename[1] == ':'
      && (('a' <= filename[0] && filename[0] <= 'z')
	  || ('A' <= filename[0] && filename[0] <= 'Z'))
      && (filename[2] == '/' || filename[2] == '\\')) {
    url += "file://";
    url += filename;
    return 1;
  }
#endif
  for (size_t i = 0; i < filename.size(); i++) {
    if (filename[i] == '/'
#ifdef SP_MSDOS_FILENAMES
        || filename[i] == '\\'
#endif
	) {
      url += '/';
    }
    else
      url += urlEncode(filename[i]);
  }
  return 1;
}

void XmlOutputEventHandler::commentDecl(CommentDeclEvent *event)
{
  if (!inDtd_ && options_.comment) {
    Boolean started = 0;
    for (MarkupIter iter(event->markup()); iter.valid(); iter.advance()) {
      if (iter.type() == Markup::comment) {
	if (!started) {
	  os() << "<!--";
	  started = 1;
	}
	os().write(iter.charsPointer(), iter.charsLength());
      }
    }
    if (started)
      os() << "-->";
  }
  delete event;
}

void XmlOutputEventHandler::markedSectionStart(MarkedSectionStartEvent *event)
{
  if (options_.cdata && event->status() == MarkedSectionEvent::cdata)
    useCdata_ = 1;
  delete event;
}

void XmlOutputEventHandler::markedSectionEnd(MarkedSectionEndEvent *event)
{
  if (event->status() == MarkedSectionEvent::cdata)
    closeCdataSection();
  delete event;
}

void XmlOutputEventHandler::outputCdata(const Char *s, size_t n)
{
  static const char cdataEnd[] = "]]>";
  for (; n > 0; s++, n--) {
    Char c = *s;
    if (!inCdata_) {
      os() << "<![CDATA[";
      inCdata_ = 1;
    }
    if (c == cdataEnd[nCdataEndMatched_]) {
      if (nCdataEndMatched_ == 2) {
	os() << "]]><![CDATA[";
	nCdataEndMatched_ = 0;
      }
      else
	nCdataEndMatched_++;
    }
    os().put(c);
  }
}

void XmlOutputEventHandler::outputData(const Char *s, size_t n, Boolean inLit)
{
  for (; n > 0; s++, n--) {
    Char c = *s;
    switch (c) {
    case '&':
      os() << "&amp;";
      break;
    case '<':
      os() << "&lt;";
      break;
    case '>':
      os() << "&gt;";
      break;
    case 9:
    case 10:
    case 13:
      if (inLit)
	os() << "&#" << int(c) << ';';
      else
	os().put(c);
      break;
    case '"':
      if (inLit) {
	os() << "&quot;";
	break;
      }
      // fall through
    default:
      os().put(c);
      break;
    }
  }
}

const StringC &XmlOutputEventHandler::generalName(const StringC &name,
					          StringC &buf)
{
  if (options_.lower && namecaseGeneral_) {
    for (size_t i = 0; i < name.size(); i++) {
      Char c = lowerSubst_[name[i]];
      if (c != name[i]) {
	buf = name;
	buf[i] = c;
	for (i++; i < name.size(); i++)
	  lowerSubst_.subst(buf[i]);
	return buf;
      }
    }
  }
  return name;
}

#ifdef SP_NAMESPACE
}
#endif
