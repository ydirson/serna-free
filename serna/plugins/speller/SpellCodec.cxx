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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "SpellCodec.h"
#include "common/common_defs.h"
#include "common/FlexString.h"
#include "common/RefCounted.h"
#include "common/OwnerPtr.h"
#include "qtextcodec.h"
#include "qstring.h"
#include "SpellChecker.h"
#include "SpellCheckerImpl.h"

#include <QByteArray>

USING_COMMON_NS

struct SpellCodecError : SpellChecker::Error {
    SpellCodecError(const nstring& err) : SpellChecker::Error(err.c_str()) {}
};

class SpellCodec::Impl : public RefCounted<> {
public:
    Impl(const nstring& enc) : codec_(QTextCodec::codecForName(enc.c_str()))
    {
        if (0 == codec_) {
            nstring err(NOTR("Unsupported local encoding: '"));
            err.append(enc).append(1, '\'');
            throw SpellCodecError(err);
        }
    }
    QTextCodec* codec_;
};

SpellCodec::SpellCodec(const nstring& enc) : impl_(new Impl(enc)) {}
SpellCodec::~SpellCodec() {}

void SpellCodec::encode(const Char* w, unsigned l, nstring& dst)
{
    QByteArray result(impl_->codec_->fromUnicode(w, l));
    int rsz = result.size();
    if (0 < rsz && '\0' == result[rsz - 1])
        --rsz;
    dst.assign(result.data(), rsz);
}

void SpellCodec::decode(const char* w, unsigned l, ustring& dst)
{
    QString result(impl_->codec_->toUnicode(w, l));
    int rsz = result.length();
    if (0 < rsz && '\0' == result[rsz - 1])
        --rsz;
    dst.assign(result.unicode(), rsz);
}
