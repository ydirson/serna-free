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
 */

#include "SpUtils.h"
#include "grove/Origin.h"

#include "sp/Message.h"
#include "sp/ExtendEntityManager.h"
#include "sp/StorageManager.h"
#include "sp/MessageFormatter.h"
#include "sp/MessageBuilder.h"
#include "sp/rtti.h"
#include "sp/SearchResultMessageArg.h"
#include "sp/ErrnoMessageArg.h"

#include <string>
#include <iostream>
#include <iomanip>

using COMMON_NS::String;

namespace {

class MsgBuilder : public Sp::MessageBuilder {
public:
    MsgBuilder(String& str, bool complete_message)
        : str_(str), complete_message_(complete_message) {}
    virtual ~MsgBuilder() {}

    virtual void appendNumber(unsigned long);
    virtual void appendOrdinal(unsigned long);
    virtual void appendChars(const Sp::Char *, size_t);
    virtual void appendOther(const Sp::OtherMessageArg *);
    virtual void appendFragment(const Sp::MessageFragment &);

private:
    String& str_;
    bool complete_message_;
};

bool getMsgText(const Sp::MessageFragment &frag, std::string& str);

}

GROVE_NAMESPACE_BEGIN

PlainOrigin SpUtils::extractSourceLocation(const Sp::Location& loc)
{
    const Sp::Origin *origin = loc.origin().pointer();
    const Sp::InputSourceOrigin *inputSourceOrigin = 0;
    const Sp::ExternalInfo *info = 0;
    Sp::Index index = loc.index();

    while (origin) {
        inputSourceOrigin = origin->asInputSourceOrigin();
        if (inputSourceOrigin) {
            info = inputSourceOrigin->externalInfo();
            if (info)
                break;
        }
        const Sp::Location &loc = origin->parent();
        index = loc.index();
        origin = loc.origin().pointer();
    }
    if (0 == origin)
        return PlainOrigin("<unknown>");
            // TODO handle locationless MessageEvent somehow

    Sp::Offset off = inputSourceOrigin->startOffset(index);
    Sp::StorageObjectLocation soLoc;
    if (!Sp::ExtendEntityManager::externalize(info, off, soLoc))
        return PlainOrigin("<unknown>");

    String src_id;
    if (0 != strcmp(soLoc.storageObjectSpec->storageManager->type(),
                    "OSFILE")) {
        const char* stype = soLoc.storageObjectSpec->storageManager->type();
        unsigned slen = strlen(stype);
        src_id.reserve(2 + slen);
        src_id.assign(1, '<').append(stype, stype + slen).append(1, '>');
    }
    else {
        const Sp::StringC& asi = soLoc.actualStorageId;
        assignString(src_id, asi.data(), asi.size());
    }

    return PlainOrigin(src_id, soLoc.lineNumber, soLoc.columnNumber + 1);
}

String SpUtils::extractFormattedMessage(const Sp::Message& msg)
{
    using namespace Sp;

    std::string text;
    if (!getMsgText(*msg.type, text)) {
//        formatFragment(MessageFormatterMessages::invalidMessage, os);
        return COMMON_NS::String();
    }
    COMMON_NS::String str;
    MsgBuilder builder(str, 2 == text.length());
    const Sp::Vector<CopyOwner<Sp::MessageArg> >& args = msg.args;
    bool esc_char = false;
    const size_t args_sz = args.size();
    for (size_t i = 0; i < text.size(); ++i) {
        if (esc_char) {
            esc_char = false;
            if (text[i] >= '1' && text[i] <= '9' &&
                (size_t(text[i] - '1') < args_sz)) {
                args[text[i] - '1']->append(builder);
                continue;
            }
            else
                str += '%';
        }
        else
            if ('%' == text[i]) {
                esc_char = true;
                continue;
            }
        str += COMMON_NS::Char(text[i]);
    }
    return str;
}

GROVE_NAMESPACE_END

namespace {

using namespace Sp;

const char* ord_sfx[] = { "st", "nd", "rd", "th" };

void
MsgBuilder::appendNumber(unsigned long n)
{
    str_ += COMMON_NS::String::number(n);
}

void
MsgBuilder::appendOrdinal(unsigned long n)
{
    str_ += COMMON_NS::String::number(n);
    int i = (n % 10) - 1;
    if (i > 3 || i < 0)
        i = 3;
    str_ += ord_sfx[i];
}

void
MsgBuilder::appendChars(const Sp::Char* p, size_t n)
{
    if (complete_message_)
        str_ += '"';
    for (size_t i = 0; i < n; ++i)
        str_.append(COMMON_NS::Char(p[i]));
    if (complete_message_)
        str_ += '"';
}

void
MsgBuilder::appendFragment(const MessageFragment &frag)
{
    std::string text;
    if (!getMsgText(frag, text))
        return;
    str_ += text.c_str();
}

void
MsgBuilder::appendOther(const OtherMessageArg *p)
{
    const ErrnoMessageArg *ea = DYNAMIC_CAST_CONST_PTR(ErrnoMessageArg, p);

    if (ea) {
        str_ += strerror(ea->errnum());
        return;
    }

    const SearchResultMessageArg *sr
    = DYNAMIC_CAST_CONST_PTR(SearchResultMessageArg, p);
    if (sr) {
        for (size_t i = 0; i < sr->nTried(); i++) {
            if (i > 0)
                str_ += ", ";
            const StringC &f = sr->filename(i);
            appendChars(f.data(), f.size());
            str_ += " (";
            str_ += strerror(sr->errnum(i));
            str_ += ')';
        }
        return;
    }
//  TODO handle this
//  appendFragment(MessageFormatterMessages::invalidArgumentType);
}

bool
getMsgText(const Sp::MessageFragment &frag, std::string& str)
{
    if (const char *p = frag.text()) {
        str.assign(p);
        return true;
    }
    return false;
}

} // end of anonymous namespace
