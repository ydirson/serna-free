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

#ifndef FORMATTER_H
#define FORMATTER_H

#include "grove/Grove.h"

#include "formatter/formatter_defs.h"
#include "formatter/Area.h"
#include "formatter/FontMgr.h"
#include "formatter/MediaInfo.h"
#include "formatter/TagMetrixTable.h"

namespace Common
{
    class Messenger;
}

namespace Formatter
{

/*! \brief FormattedView accepts formatted document from Formatter and assists
  postponed formatting.
 */
class FORMATTER_EXPIMP FormattedView {
public:
    virtual ~FormattedView() {}
    //!
    virtual bool        isToPostpone(const Area* lastAreaMade) = 0;
    //! 
    virtual void        updateSize(const Area* area) = 0;
};

class AreaViewFactory;
/*! \brief Formatter
 */
class FORMATTER_EXPIMP Formatter {
public:
    FORMATTER_OALLOC(Formatter);
    virtual ~Formatter() {}

    //! Returns true when the document is being formatted
    virtual bool        isFormatting() const = 0;
    //! Returns true when the whole document is formatted
    virtual bool        isFinished() const = 0;
    //! Returns true if given fo node was already formatted
    virtual bool        isFormatted(const GroveLib::Node* foNode) const = 0;
    //!
    virtual bool        isModified() const = 0;
    //!
    virtual void        setNotModified() = 0;
    //! Returns true if no actual formatting has occured
    virtual void        format(bool isPostponable) = 0;
    //!
    virtual const Area* lastAreaMade() const = 0;
    //!
    virtual Area*       rootArea() const = 0;
    //! Detaches areas from their Fo`s (nondestructive)
    virtual void        detachAreaTree() = 0;
};

FORMATTER_EXPIMP Formatter*  makeFormatter(COMMON_NS::Messenger* messenger,
                                           const MediaInfo& mediaInfo,
                                           FormattedView* view,
                                           FontMgr* fontMgr,
                                           GroveLib::GrovePtr& fot,
                                           const AreaViewFactory* viewFactory,
                                           TagMetrixTable& tagMetrixTable);
}

#endif // FORMATTER_H
