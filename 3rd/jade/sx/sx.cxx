// Copyright (c) 1997 James Clark
// See the file COPYING for copying permission.

#include "config.h"
#include "OutputCharStream.h"
#include "ParserApp.h"
#include "macros.h"
#include "XmlOutputEventHandler.h"
#include "SxMessages.h"

#include "sptchar.h"

#include <stdlib.h>

#ifdef SP_NAMESPACE
using namespace SP_NAMESPACE;
#endif

static
struct {
  const ParserApp::AppChar *name;
  PackedBoolean XmlOutputEventHandler::Options::*ptr;
} outputOptionsTable[] = {
  { SP_T("nl-in-tag"), &XmlOutputEventHandler::Options::nlInTag },
  { SP_T("id"), &XmlOutputEventHandler::Options::id },
  { SP_T("notation"), &XmlOutputEventHandler::Options::notation },
  { SP_T("ndata"), &XmlOutputEventHandler::Options::ndata },
  { SP_T("cdata"), &XmlOutputEventHandler::Options::cdata },
  { SP_T("comment"), &XmlOutputEventHandler::Options::comment },
  { SP_T("lower"), &XmlOutputEventHandler::Options::lower },
  { SP_T("pi-escape"), &XmlOutputEventHandler::Options::piEscape },
  { SP_T("empty"), &XmlOutputEventHandler::Options::empty },
  { SP_T("attlist"), &XmlOutputEventHandler::Options::attlist },
};

class SxApp : public ParserApp {
public:
  SxApp();
  ErrorCountEventHandler *makeEventHandler();
  void processOption(AppChar opt, const AppChar *arg);
  int processSysid(const StringC &);
private:
  XmlOutputEventHandler::Options outputOptions_;
  StringC encodingName_;
};

SP_DEFINE_APP(SxApp)

SxApp::SxApp()
: ParserApp("unicode")
{
  outputCodingSystem_ = codingSystemKit_->makeCodingSystem("UTF-8", 0);
  registerOption('x', SP_T("xml_output_option"));
  outputOptions_.nlInTag = 1;
}

int SxApp::processSysid(const StringC &sysid)
{
  if (outputOptions_.comment)
    options_.eventsWanted.addCommentDecls();
  if (outputOptions_.notation)
    options_.warnNotationSystemId = 1;
  if (outputOptions_.cdata)
    options_.eventsWanted.addMarkedSections();
  return ParserApp::processSysid(sysid);
}

void SxApp::processOption(AppChar opt, const AppChar *arg)
{
  switch (opt) {
  case 'x':
    {
      PackedBoolean value;
      if (arg[0] == 'n' && arg[1] == 'o' && arg[2] == '-') {
	value = 0;
	arg += 3;
      }
      else
	value = 1;
      Boolean found = 0;
      for (size_t i = 0; i < SIZEOF(outputOptionsTable); i++) {
	if (tcscmp(outputOptionsTable[i].name, arg) == 0) {
	  found = 1;
	  outputOptions_.*(outputOptionsTable[i].ptr) = value;
 	  break;
	}
      }
      if (!found)
	message(SxMessages::unknownOutputOption, StringMessageArg(convertInput(arg)));
      break;
    }
  case 'b':
    {
      // Need to generate appropriate encoding declaration.
      ParserApp::processOption(opt, arg);
      if (outputCodingSystem_)
	encodingName_ = convertInput(arg);
      else
	encodingName_.resize(0);
      break;
    }
  default:
    ParserApp::processOption(opt, arg);
    break;
  }
}

ErrorCountEventHandler *SxApp::makeEventHandler()
{
  return new XmlOutputEventHandler(outputOptions_,
				   new RecordOutputCharStream(makeStdOut()),
				   encodingName_,
                                   entityManager(),
				   systemCharset(),
				   this);
}
