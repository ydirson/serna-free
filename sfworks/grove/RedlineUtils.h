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
#ifndef GROVE_REDLINE_UTILS_H_
#define GROVE_REDLINE_UTILS_H_

#include "grove/Node.h"
#include "grove/SectionNodes.h"
#include "common/XTreeNode.h"
#include "common/OwnerPtr.h"

namespace GroveLib {

enum RedlineMaskShift {
    REDLINE_FONT_COLOR = 0,         // 4 bits
    REDLINE_HIGHLIGHT_COLOR = 4,    // 4 bits
    REDLINE_STRIKE_THRU = 8,        // 1 bit
    REDLINE_UNDERLINE = 9           // 1 bit
};

class RedlineSection;
typedef Common::CDList<RedlineSectionStart> RedlineSectionList;

class GROVE_EXPIMP RedlineData : public Common::RefCounted<> {
public:
    GROVE_OALLOC(RedlineData);
    
    RedlineData(uint redlineMask, 
                const Common::String& annotation)
        : redlineMask_(redlineMask),
          annotation_(annotation) {}
    ~RedlineData();
    
    uint                    redlineMask() const { return redlineMask_; }
    const Common::String&   annotation() const { return annotation_; }
    RedlineSectionList&     replicas() { return replicas_; }

    void        setRedlineMask(uint v);
    void        setAnnotation(const Common::String& s);
        
    Common::String      asPiString() const;
    void                dump() const;
    RedlineData*        copy() const 
    { 
        return new RedlineData(redlineMask_, annotation_);
    }
    void                notifyChanged();
    static void*        redlineNotifyId();

private:
    RedlineData(const RedlineData&);
    uint                redlineMask_;
    Common::String      annotation_;
    RedlineSectionList  replicas_;
};

typedef Common::RefCntPtr<RedlineData> RedlineDataPtr;

//////////////////////////////////////////////////////////////////////

// Utility class which is used for redline PI's parsing in grovebuilder
class GROVE_EXPIMP RedlineParserTool {
public:
    RedlineParserTool(RedlineSectionStart*);

    void            processPi(ProcessingInstruction* pi);
    void            processElementEnd(Node* n);
    void            finalize();

private:
    RedlineSectionStart* rss_; 
};

GROVE_EXPIMP RedlineSectionStart* get_redline_section(const Node*);

} // namespace GroveLib

#endif // GROVE_REDLINE_UTILS_H_
