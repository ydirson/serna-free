// This file was automatically generated from spam\SpamMessages.msg by msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct SpamMessages {
  // 0
  static const MessageType1 invalidMarkupArgument;
  // 1
  static const MessageType0 tagInSpecialMarkedSection;
};
const MessageType1 SpamMessages::invalidMarkupArgument(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
0
#ifndef SP_NO_MESSAGE_TEXT
,"invalid argument for -m option"
#endif
);
const MessageType0 SpamMessages::tagInSpecialMarkedSection(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
1
#ifndef SP_NO_MESSAGE_TEXT
,"omitted start or end tag implied in CDATA or RCDATA marked section; not normalized"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
