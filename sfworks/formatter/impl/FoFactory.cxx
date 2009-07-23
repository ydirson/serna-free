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
#ifdef _MSC_VER
# pragma warning( disable : 4786 )
#endif

#include "common/Singleton.h"
#include "common/Factory.h"
#include "grove/Node.h"

#include "formatter/XslFoExt.h"
#include "formatter/impl/FoFactory.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/TableFo.h"
#include "formatter/impl/TerminalFos.h"
#include "formatter/impl/SimpleFormFos.h"
#include "formatter/impl/PageMaster.h"
#include "formatter/XslMessages.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE
/*!
 */
namespace Formatter {

    template<class T> class Maker {
    public:
        static Fo* make(PropertySet* parentSet, bool hasParentOrigin,
                        const Node* foNode, FoData& data,
                        uint physicalLevel, uint logicalLevel)
            {
                return new T(FoInit(parentSet, foNode, data,
                             physicalLevel, logicalLevel, hasParentOrigin));
            }
    };

    typedef Fo* (*FoMaker) (PropertySet* parentSet, bool hasParentOrigin,
                            const Node* foNode, FoData& data,
                            uint physicalLevel, uint logicalLevel);
/*!
 */
class FoFactoryImpl : public FoFactory {
public:
    FORMATTER_OALLOC(FoFactoryImpl);
    FoFactoryImpl() {
        for (int name = FIRST_FO; name < LAST_FO; name++) {
            foMaker_[name] = Maker<UnknownFo>::make;
            tagFoMaker_[name] = 0;
        }

        foMaker_[BLOCK] = Maker<BlockFo>::make;
        foMaker_[INLINE] = Maker<InlineFo>::make;
        foMaker_[EXTERNAL_GRAPHIC] = Maker<GraphicFo>::make;
        foMaker_[TEXT] = Maker<TextFo>::make;

        //! In tagless mode treat lists as is
        foMaker_[LIST_BLOCK] = Maker<ListBlockFo>::make;
        foMaker_[LIST_ITEM] = Maker<ListItemFo>::make;
        foMaker_[LIST_ITEM_LABEL] = Maker<ListItemLabelFo>::make;
        foMaker_[LIST_ITEM_BODY] = Maker<ListItemBodyFo>::make;
        //! In tag mode treat them as blocks
        tagFoMaker_[LIST_BLOCK] = Maker<BlockFo>::make;
        tagFoMaker_[LIST_ITEM] = Maker<BlockFo>::make;
        tagFoMaker_[LIST_ITEM_LABEL] = Maker<BlockFo>::make;
        tagFoMaker_[LIST_ITEM_BODY] = Maker<BlockFo>::make;

        //! In tagless mode treat tables as is
        foMaker_[TABLE_AND_CAPTION] = Maker<BlockFo>::make;
        foMaker_[TABLE] = Maker<TableFo>::make;
        foMaker_[TABLE_COLUMN] = Maker<TableColumnFo>::make;
        foMaker_[TABLE_CAPTION] = Maker<UnknownFo>::make;
        foMaker_[TABLE_HEADER] = Maker<TableBodyFo>::make;
        foMaker_[TABLE_FOOTER] = Maker<TableBodyFo>::make;
        foMaker_[TABLE_BODY] = Maker<TableBodyFo>::make;
        foMaker_[TABLE_ROW] = Maker<TableRowFo>::make;
        foMaker_[TABLE_CELL] = Maker<TableCellFo>::make;
        //! In tag mode treat them as blocks
        tagFoMaker_[TABLE_AND_CAPTION] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE_COLUMN] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE_CAPTION] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE_HEADER] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE_FOOTER] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE_BODY] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE_ROW] = Maker<BlockFo>::make;
        tagFoMaker_[TABLE_CELL] = Maker<BlockFo>::make;

        foMaker_[SECTION_CORNER] = Maker<SectionCornerFo>::make;
        foMaker_[CHOICE] = Maker<ChoiceFo>::make;
        foMaker_[COMMENT] = Maker<CommentFo>::make;
        foMaker_[PI] = Maker<PiFo>::make;
        foMaker_[FOLD] = Maker<FoldFo>::make;
        foMaker_[COMBO_BOX] = Maker<ComboBoxFo>::make;
        foMaker_[LINE_EDIT] = Maker<LineEditFo>::make;
        foMaker_[INLINE_OBJECT] = Maker<InlineObjectFo>::make;
        foMaker_[UNKNOWN] = Maker<UnknownFo>::make;
    }
    //!
    Fo* makeFo(PropertySet* parentSet, const Node* foNode,
               FoData& data, uint physicalLevel, uint logicalLevel) const {

        bool has_parent_origin = false;
        const Node* origin = XslFoExt::origin(foNode);
        const Node* parent_fo = foNode->parent();

        if (parent_fo && FLOW != Fo::foName(parent_fo)) {
            //! TODO: take parents origin from parameters
            const Node* parent_origin = XslFoExt::origin(parent_fo);
            if (parent_origin)
                has_parent_origin = (parent_origin == origin);
        }
        FoName name = Fo::foName(foNode);
        if (data.mediaInfo_.isShowTags_ && TABLE_COLUMN == name &&
            (!origin || origin->nodeName() !=  foNode->nodeName()))
            return 0;

        FoMaker maker_funk = (data.mediaInfo_.isShowTags_ && tagFoMaker_[name])
            ? tagFoMaker_[name] : foMaker_[name];
        Fo* fo = maker_funk(parentSet, has_parent_origin,
                            foNode, data, physicalLevel, logicalLevel);
        if (0 == fo)
            data.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                           << foNode->nodeName() << parent_fo->nodeName();
        return fo;
    };
private:
    FoMaker foMaker_[LAST_FO];
    FoMaker tagFoMaker_[LAST_FO];
};

} // namespace Formatter

namespace Common {

template<> Formatter::FoFactory* Factory<Formatter::FoFactory>::instance()
{
    return &SingletonHolder<Formatter::FoFactoryImpl>::instance();
}

} // namespace Common

