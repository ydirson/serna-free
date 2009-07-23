// This file was automatically generated from .\SxMessages.msg by ..\msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct SxMessages {
  // 0
  static const MessageType1 unknownOutputOption;
};
const MessageType1 SxMessages::unknownOutputOption(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
0
#ifndef SP_NO_MESSAGE_TEXT
,"unknown output option %1"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
