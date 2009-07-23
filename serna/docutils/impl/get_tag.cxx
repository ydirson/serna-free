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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "docutils/doctags.h"
#include "DocTag.h"

#include "common/String.h"
#include "common/StringCvt.h"

#include <string.h>

static const doctags::DocTag* find_doctag(const char* tag)
{
    using namespace doctags;
    for (const DocTag* pdt = &DOCTAGS[0]; 
         pdt != &DOCTAGS[DOCTAGS_SIZE]; ++pdt) {
        if (0 != pdt && 0 != pdt->tag_ && 0 == strcmp(tag, pdt->tag_))
            return pdt;
    }
    return 0;
}

namespace doctags {

Common::String get_tag(const Common::String& tag)
{
    Common::nstring ntag(Common::latin1(tag));
    return get_tag(ntag.c_str());
}

Common::String get_tag(const char* p)
{
    const DocTag* pdt = find_doctag(p);
    
    if (0 == pdt)
        pdt = find_doctag("INDEX");
        
    if (0 == pdt || 0 == pdt->url_)
        return Common::from_latin1("index.html");
        
    return Common::from_latin1(pdt->url_);
}

}
