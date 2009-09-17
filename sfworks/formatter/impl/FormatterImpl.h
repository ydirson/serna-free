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

#ifndef FORMATTER_IMPL_H
#define FORMATTER_IMPL_H

#include <list>

#include "formatter/Formatter.h"
#include "formatter/impl/FoImpl.h"
#include "formatter/impl/Branch.h"
#include "common/MessageUtils.h"

namespace Common
{
    class MessageStream;
}

namespace Formatter
{

/*! \brief Formatter
 */
class FormatterImpl : public Formatter,
                      public ModificationRegistry {
public:

    FORMATTER_OALLOC(FormatterImpl);

    typedef COMMON_NS::XList<Branch,
            COMMON_NS::XTreeNodeRefCounted<Branch> > BranchList;

    FormatterImpl(COMMON_NS::Messenger* messenger,
                  const MediaInfo& mediaInfo, FormattedView* view,
                  FontMgr* fontMgr, GroveLib::GrovePtr& fot,
                  const AreaViewFactory* viewFactory,
                  TagMetrixTable& tagMetrixTable );
    virtual ~FormatterImpl();

    bool                    isFormatting() const { return isFormatting_; }
    //! Returns true when the whole document is formatted
    bool                    isFinished() const;
    //! Returns true if given fo node was already formatted
    bool                    isFormatted(const GroveLib::Node* foNode) const;
    //!
    bool                    isModified() const { return isModified_; }
    //!
    void                    setNotModified() { isModified_ = false; }
    //!
    void                    format(bool isPostponable);
    //!
    const Area*             lastAreaMade() const;
    //!
    Area*                   rootArea() const;
    //! Detaches areas from their Fo`s (nondestructive)
    void                    detachAreaTree();
    //!
    virtual void            detachFromFormatter(Fo* fo, const 
                                                FoController* preserveBranch);
    virtual FoModList&      textFoModList() { return foData_.textFoModList_; }

protected:
    //!
    virtual bool            add(Fo* fo);
private:
    bool                    isModified_;
    bool                    isFormatting_;
    
    //! Bunch of data used in fo tree
    Common::MessengerPtr        messenger_;
    COMMON_NS::MessageStream    mstream_;
    MediaInfo                   mediaInfo_;
    FontMgr* const              fontMgr_;

    FoData                  foData_;
    FoPtr                   rootFo_;
    //! Data used in branch management
    FormattedView* const    view_;
    GroveLib::GrovePtr      foGrove_;
    const AreaViewFactory*  viewFactory_;

    FoControllerMap         foMap_;
    BranchList              branches_;
    Common::OwnerPtr<GroveLib::GroveVisitor> formatterGroveVisitor_;
};

}

#endif // FORMATTER_IMPL_H
