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
/*! \file
    Simple test of messaging facility
 */

#include "common/common_defs.h"
#include "common/OwnerPtr.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/String.h"
#include "messages/TestMessages.h"

#include "common_test.h"
#include <iostream>

USING_COMMON_NAMESPACE
/* Custom messenger for testing
 */
class MyMessenger : public Messenger {
public:
    MyMessenger(std::ostream& os)
        : defaultMessenger_(new DefaultMessenger), dbg_os_(os) {}

    virtual void dispatch(RefCntPtr<Message>& m);
    virtual Messenger* copy() const { return 0; }

private:
    RefCntPtr<Messenger> defaultMessenger_;
    std::ostream& dbg_os_;
};

// custom message type
class MyType {
public:
    enum { messageArgType = MessageArgBase::USER_DEFINED + 1 };
    String foo() const { return String::number(n_) + "foo"; }
    MyType(int n) : n_(n) {}
private:
    int n_;
};

String MessageArg<MyType>::format() const
{
    return value_.foo();
}

/* custom-process the message (print my message and forward the
 * rest to default messenger)
 */
void MyMessenger::dispatch(RefCntPtr<Message>& m)
{
    ulong i;
    Vector<const MessageArgBase*> context;
    for (i = 0; i < m->nArgs(); ++i)
        if (m->getArg(i)->isContext())
            context.push_back(m->getArg(i));
    if (context.size()) {
        dbg_os_ << "CONTEXT: [";
        for (i = 0; i < context.size(); ++i)
            dbg_os_ << "TYPE " << context[i]->type()
                << ": " << context[i]->format();
        dbg_os_ << " ]; ";
    }
    dbg_os_ << "MESSAGE: ";
    defaultMessenger_->dispatch(m);
}

using namespace TEST_NS;
using namespace COMMON_TEST_NS;

class MsgTest : public CommonTest {
public:
    MsgTest(const TestContext& ctx) : CommonTest(ctx) {}
    virtual bool test();
};

bool
MsgTest::test()
{
#ifdef USE_TESTSYSTEM
    std::ostream& os_ref = getCtx().outStream_;
    if (!BuiltinMessageFetcher::loadMessageLibrary(getCtx().getParameter("msglib").c_str(), true)) {
#else
    std::ostream& os_ref = std::cerr;
    if (!BuiltinMessageFetcher::loadMessageLibrary("TestMessages", true)) {
#endif
        TEST_FAILED("Cannot open message library")
    }
    Vector<const char*> languages;
    BuiltinMessageFetcher::listLanguages(languages);
    if (0 == languages.size()) {
        TEST_FAILED("No available languages")
    }
    os_ref << "AVAILABLE LANGUAGES:";
    ulong i;
    for (i = 0; i < languages.size(); ++i)
        os_ref << ' ' << languages[i];
    os_ref << std::endl;

    MessageBlock ms(6, new MyMessenger(os_ref));   // 6 - facility of TestMessages
    ms.setMinSeverity(Message::L_ERROR);
    MyType myval(111);
    ms.setMaxMsgBuf(10);
    for (i = 0; i < 15; ++i)
        ms << TestMessages::referenceToUndefined << Message::L_INFO << i;
    ms << TestMessages::cannotObtainCredFromImp << Message::L_ERROR
       << "xslt" << myval << MessageArgFmt() << 3;
    ms.dispatch();
    std::cerr << "-- end\n";

    return true;
}

#ifdef USE_TESTSYSTEM
TESTCASE_REGISTER(MsgTest)
#endif

#ifndef USE_TESTSYSTEM
int main()
{
    return MsgTest(TEST_NS::TestContext()).test();
}
#endif
