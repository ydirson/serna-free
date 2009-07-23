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

#ifndef FORMATTER_BRANCH_H
#define FORMATTER_BRANCH_H

#include "common/XList.h"

#include "formatter/formatter_defs.h"
#include "formatter/Formatter.h"
#include "formatter/impl/FoController.h"

namespace Formatter {

/*! \brief Branch controls the list of FoControllers to process the Fo tree
 */

enum BranchState {
    POSTPONED,
    FINISHED
};

class Branch : public COMMON_NS::RefCounted<>,
               public COMMON_NS::XListItem<Branch,
                      COMMON_NS::XTreeNodeRefCounted<Branch> > {
public:
    FORMATTER_OALLOC(Branch);

    typedef COMMON_NS::RefCntPtr<FoController> FoControllerPtr;

    Branch(const FoPtr& fo, FormattedView* view,
           const AreaViewFactory* viewFactory, FoControllerMap* foMap,  
           bool isPaginated);
    //!
    BranchState     process(bool isPostponable);
    //!
    const Area*     lastAreaMade() const { return lastAreaMade_; }
    //!
    bool            isFinished() const { return root_.isNull(); }
    //! Returns true if this branch will process given Fo
    bool            willProcess(const FoPtr& fo) const;
    //! Returns true if this branch will process given Fo
    bool            willProcess(const GroveLib::Node* foNode) const;
    bool            isProcesses(const GroveLib::Node* foNode) const;
    //!
    bool            isProcesses(Fo* fo, 
                                const FoController* preserveBranch) const;
    //!
    bool            operator==(const Branch& branch) const;

protected:
    FoController*   lastChild() const;

private:
    FormattedView*  view_;
    FoControllerPtr root_;
    FoController*   current_;
    const Area*     lastAreaMade_;
    uint            splitCount_;
    const bool      isPaginated_;
};

} // namespace Formatter

#endif // FORMATTER_BRANCH_H
