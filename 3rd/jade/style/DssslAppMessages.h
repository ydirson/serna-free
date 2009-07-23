// This file was automatically generated from .\DssslAppMessages.msg by ..\msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct DssslAppMessages {
  // 2000
  static const MessageType1 versionInfo;
  // 2001
  static const MessageType0 noSpec;
};
const MessageType1 DssslAppMessages::versionInfo(
MessageType::info,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2000
#ifndef SP_NO_MESSAGE_TEXT
,"Jade version %1"
#endif
);
const MessageType0 DssslAppMessages::noSpec(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2001
#ifndef SP_NO_MESSAGE_TEXT
,"no DSSSL specification: use -d to specify"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
