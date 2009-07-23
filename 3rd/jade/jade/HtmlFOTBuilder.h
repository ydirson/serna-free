// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef HtmlFOTBuilder_INCLUDED
#define HtmlFOTBuilder_INCLUDED 1

#include "FOTBuilder.h"
#include "CmdLineApp.h"
#include "Message.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

FOTBuilder *makeHtmlFOTBuilder(const String<CmdLineApp::AppChar> &basename,
			       CmdLineApp *app,
			       const FOTBuilder::Extension *&);

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not HtmlFOTBuilder_INCLUDED */
