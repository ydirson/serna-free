// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
 #include "sp/config.h"
#include "sp/Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct DAVStorageMessages {
  // 2300
  static const MessageType1 emptyHost;
  // 2301
  static const MessageType1 badRelative;
  // 2302
  static const MessageType1 emptyPort;
  // 2303
  static const MessageType1 invalidPort;
  // 2304
  static const MessageType1 hostNotFound;
  // 2305
  static const MessageType1 hostTryAgain;
  // 2306
  static const MessageType1 hostNoRecovery;
  // 2307
  static const MessageType1 hostNoData;
  // 2308
  static const MessageType2 hostOtherError;
  // 2309
  static const MessageType1 hostUnknownError;
  // 2310
  static const MessageType1 cannotCreateSocket;
  // 2311
  static const MessageType2 cannotConnect;
  // 2312
  static const MessageType2 writeError;
  // 2313
  static const MessageType2 readError;
  // 2314
  static const MessageType2 closeError;
  // 2315
  static const MessageType1 invalidHostNumber;
  // 2316
  static const MessageType3 getFailed;
  // 2317
  static const MessageType0 notSupported;
  // 2318
  static const MessageType0 onlyHTTP;
  // 2319
  static const MessageType1 winsockInitialize;
  // 2320
  static const MessageType0 winsockVersion;
  // 2321
  static const MessageFragment winsockErrorNumber;
};
const MessageType1 DAVStorageMessages::emptyHost(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2300
#ifndef SP_NO_MESSAGE_TEXT
,"empty host in HTTP URL %1"
#endif
);
const MessageType1 DAVStorageMessages::badRelative(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2301
#ifndef SP_NO_MESSAGE_TEXT
,"uncompletable relative HTTP URL %1"
#endif
);
const MessageType1 DAVStorageMessages::emptyPort(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2302
#ifndef SP_NO_MESSAGE_TEXT
,"empty port number in HTTP URL %1"
#endif
);
const MessageType1 DAVStorageMessages::invalidPort(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2303
#ifndef SP_NO_MESSAGE_TEXT
,"invalid port number in HTTP URL %1"
#endif
);
const MessageType1 DAVStorageMessages::hostNotFound(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2304
#ifndef SP_NO_MESSAGE_TEXT
,"host %1 not found"
#endif
);
const MessageType1 DAVStorageMessages::hostTryAgain(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2305
#ifndef SP_NO_MESSAGE_TEXT
,"could not resolve host %1 (try again later)"
#endif
);
const MessageType1 DAVStorageMessages::hostNoRecovery(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2306
#ifndef SP_NO_MESSAGE_TEXT
,"could not resolve host %1 (unrecoverable error)"
#endif
);
const MessageType1 DAVStorageMessages::hostNoData(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2307
#ifndef SP_NO_MESSAGE_TEXT
,"no address record for host name %1"
#endif
);
const MessageType2 DAVStorageMessages::hostOtherError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2308
#ifndef SP_NO_MESSAGE_TEXT
,"could not resolve host %1 (%2)"
#endif
);
const MessageType1 DAVStorageMessages::hostUnknownError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2309
#ifndef SP_NO_MESSAGE_TEXT
,"could not resolve host %1 (unknown error)"
#endif
);
const MessageType1 DAVStorageMessages::cannotCreateSocket(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2310
#ifndef SP_NO_MESSAGE_TEXT
,"cannot create socket (%1)"
#endif
);
const MessageType2 DAVStorageMessages::cannotConnect(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2311
#ifndef SP_NO_MESSAGE_TEXT
,"error connecting to %1 (%2)"
#endif
);
const MessageType2 DAVStorageMessages::writeError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2312
#ifndef SP_NO_MESSAGE_TEXT
,"error sending request to %1 (%2)"
#endif
);
const MessageType2 DAVStorageMessages::readError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2313
#ifndef SP_NO_MESSAGE_TEXT
,"error receiving from host %1 (%2)"
#endif
);
const MessageType2 DAVStorageMessages::closeError(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2314
#ifndef SP_NO_MESSAGE_TEXT
,"error closing connection to host %1 (%2)"
#endif
);
const MessageType1 DAVStorageMessages::invalidHostNumber(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2315
#ifndef SP_NO_MESSAGE_TEXT
,"invalid host number %1"
#endif
);
const MessageType3 DAVStorageMessages::getFailed(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2316
#ifndef SP_NO_MESSAGE_TEXT
,"could not get %2 from %1 (reason given was %3)"
#endif
);
const MessageType0 DAVStorageMessages::notSupported(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2317
#ifndef SP_NO_MESSAGE_TEXT
,"URL not supported by this version"
#endif
);
const MessageType0 DAVStorageMessages::onlyHTTP(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2318
#ifndef SP_NO_MESSAGE_TEXT
,"only HTTP scheme supported"
#endif
);
const MessageType1 DAVStorageMessages::winsockInitialize(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2319
#ifndef SP_NO_MESSAGE_TEXT
,"could not initialize Windows Sockets (%1)"
#endif
);
const MessageType0 DAVStorageMessages::winsockVersion(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2320
#ifndef SP_NO_MESSAGE_TEXT
,"incompatible Windows Sockets version"
#endif
);
const MessageFragment DAVStorageMessages::winsockErrorNumber(
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2321
#ifndef SP_NO_MESSAGE_TEXT
,"error number "
#endif
);
#ifdef SP_NAMESPACE
}
#endif
