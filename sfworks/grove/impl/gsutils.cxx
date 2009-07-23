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
#include "grove/impl/gsutils.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "grove/SectionNodes.h"
#include "common/Encodings.h"
#include "common/Url.h"
#include "dav/DavManager.h"

using namespace Common;

namespace GroveLib {

static const int INDENT_STEP = 2;

GroveSaverContext::GroveSaverContext(const StripInfo* si,
                                     Grove* grove,
                                     int flags)
    : stripInfo_(si), flags_(flags), grove_(grove),
      cdataLevel_(0), indentDepth_(0), indentStep_(INDENT_STEP),
      localRoot_(0)
{
}

bool GroveSaverContext::setOutputUrl(const String& saveAs, int encoding)
{
    Url url((saveAs.isEmpty()) ? grove_->topSysid() : saveAs);
    if (Dav::DavManager::instance().open(url,
        Dav::DAV_OPEN_WRITE|Dav::DAV_OPEN_MKPATH, os_))
            return false;
    os_.setAddCR(Grove::GS_CRLF_LINE_BREAKS & flags_);
    os_.encodeCharacterEntities(true);
    if (encoding < 0)
        os_.setEncoding(grove_->groveBuilder()->getEncoding());
    else
        os_.setEncoding(encoding);
    if (flags_ & Grove::GS_GENERATE_UNICODE_BOM)
        os_.setGenerateByteOrderMark(true);
    return true;
}

void GroveSaverContext::setOutputString()
{
    saveTo_.reserve(1024);
    os_ = Dav::IoStream(saveTo_);
}

Common::String GroveSaverContext::getSavedString() const
{
    return saveTo_;
}

void GroveSaverContext::printIndent(int offset)
{
    int nSpaces = (indentDepth_ + offset) * indentStep_;
    for (int i = 0; i < nSpaces; ++i)
        os() << ' ';
}

void GroveSaverContext::adjustIndent(int offset)
{
    indentDepth_ += offset;
}

void GroveSaverContext::xmlHeader(const EntityDecl* ed)
{
    const ExternalEntityDecl* eed = ed->asConstExternalEntityDecl();
    if (0 == eed)
        return;
    Encodings::Encoding enc = eed->encoding();
    if (ed->declType() == EntityDecl::document)
        enc = grove_->groveBuilder()->getEncoding();
    if (enc == Encodings::XML)
        enc = Encodings::UTF_8;
    os() << "<?xml version=\'1.0\'";
    if (enc != Encodings::UTF_8 || !(flags_ & Grove::GS_DONTSAVEDEFENC))
        os() << " encoding=\'" << Encodings::encodingName(enc) << '\'';
    os() << "?>";
    if (eed->declType() == EntityDecl::document)
        os() << CR_ENDL;
}

GroveSaverContext::~GroveSaverContext()
{
    os_.close();
}

} // namespace GroveLib
