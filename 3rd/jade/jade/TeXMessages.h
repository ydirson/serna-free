// This file was automatically generated from jade/TeXMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct TeXMessages {
  // 5000
  static const MessageType0 unsupportedPageNumberNonElement;
  // 5001
  static const MessageType0 unsupportedLinkNonElement;
  // 5002
  static const MessageType0 unsupportedLinkEntity;
  // 5003
  static const MessageType0 unsupportedLinkSgmlDoc;
  // 5004
  static const MessageType0 unsupportedLinkHyTime;
  // 5005
  static const MessageType0 unsupportedLinkTei;
  // 5006
  static const MessageType0 unsupportedLinkHtml;
  // 5007
  static const MessageType0 unsupportedGlyphSubstTable;
};
const MessageType0 TeXMessages::unsupportedPageNumberNonElement(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5000
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support references to page numbers of nodes other than elements"
#endif
);
const MessageType0 TeXMessages::unsupportedLinkNonElement(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5001
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support links to nodes other than elements"
#endif
);
const MessageType0 TeXMessages::unsupportedLinkEntity(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5002
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support links to entities"
#endif
);
const MessageType0 TeXMessages::unsupportedLinkSgmlDoc(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5003
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support links to other SGML documents"
#endif
);
const MessageType0 TeXMessages::unsupportedLinkHyTime(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5004
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support HyTime linkends"
#endif
);
const MessageType0 TeXMessages::unsupportedLinkTei(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5005
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support TEI links"
#endif
);
const MessageType0 TeXMessages::unsupportedLinkHtml(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5006
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support HTML links"
#endif
);
const MessageType0 TeXMessages::unsupportedGlyphSubstTable(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5007
#ifndef SP_NO_MESSAGE_TEXT
,"TeX backend does not currently support glyph substitution tables"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
