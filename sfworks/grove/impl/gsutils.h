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
#ifndef GROVE_GSUTILS_H_
#define GROVE_GSUTILS_H_

#include "grove/StripInfo.h"
#include "grove/Grove.h"
#include "common/String.h"
#include "dav/IoStream.h"

namespace GroveLib {

class EntityDecl;
class Element;

class GroveSaverContext {
public:
    enum InternalFlags {
        MIXED_CONTENT = 001,
        HAS_CONTENT   = 002,
        STRIP_ALLOWED = 004,
        ESC_ATTVAL    = 010,
        SAVE_ECONTENT = 020,
        HAS_EREFS     = 040,
        INT_ENTITY    = 0100 // saving internal entity
    };
    bool    setOutputUrl(const Common::String& saveAs, int encoding = -1);
    void    setOutputString();
    bool    close() { return os_.close() == Dav::DAV_RESULT_OK; }

    GroveSaverContext(const StripInfo* si,
                      Grove* grove,
                      int flags);
    ~GroveSaverContext();

    Dav::IoStream& os() { return os_; }
  
    void                printIndent(int offset = 0);
    void                adjustIndent(int offset = 0);
    Grove*              grove() const { return grove_; }
    void                xmlHeader(const EntityDecl*);
    const StripInfo*    stripInfo() const { return stripInfo_; }
    Common::String      getSavedString() const;
    int                 cdataLevel(int offs = 0) 
    { 
        cdataLevel_ += offs; return cdataLevel_;
    }
    int                 flags() const { return flags_; }
    void                setLocalRoot(const Element* e) { localRoot_ = e; }
    const Element*      localRoot() const { return localRoot_; }

private:
    GroveSaverContext(const GroveSaverContext&);
    GroveSaverContext& operator=(const GroveSaverContext&);

    const StripInfo*    stripInfo_;
    int                 flags_;
    Grove*              grove_;
    int                 cdataLevel_;
    int                 indentDepth_;
    int                 indentStep_;
    const Element*      localRoot_;
    
    Common::String      saveTo_;
    Dav::IoStream       os_;
};

#define CR_ENDL "\n"

} // namespace GroveLib

#endif // GROVE_GSUTILS_H_
