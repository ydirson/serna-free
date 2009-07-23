// This file was automatically generated from .\MifMessages.msg by ..\msggen.pl.
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct MifMessages {
  // 7000
  static const MessageType2 cannotOpenOutputError;
  // 7001
  static const MessageType1 systemIdNotFilename;
  // 7002
  static const MessageType0 missingTableColumnFlowObject;
};
const MessageType2 MifMessages::cannotOpenOutputError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
7000
#ifndef SP_NO_MESSAGE_TEXT
,"MIF: cannot open output file %1 (%2)"
#endif
);
const MessageType1 MifMessages::systemIdNotFilename(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
7001
#ifndef SP_NO_MESSAGE_TEXT
,"MIF: could not convert system identifier %1 to a single filename"
#endif
);
const MessageType0 MifMessages::missingTableColumnFlowObject(
MessageType::warning,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
7002
#ifndef SP_NO_MESSAGE_TEXT
,"MIF: missing table column flow object (table-auto-width feature not fully supported)"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
