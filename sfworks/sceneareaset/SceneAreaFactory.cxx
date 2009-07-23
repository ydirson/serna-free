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
#include "sceneareaset/debug.h"
#include "sceneareaset/SceneAreaFactory.h"
#include "sceneareaset/TextAreaView.h"
#include "sceneareaset/GraphicAreaView.h"
#include "sceneareaset/PageAreaView.h"
#include "sceneareaset/InlineObject.h"
#include "formatter/impl/Areas.h"
#include "common/safecast.h"
#include "common/MessageUtils.h"

using namespace Common;
using namespace Formatter;

SceneAreaFactory::SceneAreaFactory()
    : QGraphicsScene(0, 0, 800, 600),
      rootViewMade_(false),
      pstream_(0),
      isPaginated_(true)
{
}

void SceneAreaFactory::setProgressStream(MessageStream* pstream)
{
    pstream_ = pstream;
}

AreaView* SceneAreaFactory::makeAreaView(const Area* area) const
{
    SceneAreaFactory* scene = const_cast<SceneAreaFactory*>(this);
    switch (area->type()) {
        case ROOT_AREA:
            //std::cerr << "makeAreaView scene->setSceneRect\n";
            scene->setSceneRect(0, 0, area->allcW(), area->allcH());
            rootViewMade_ = true;
            break;
        case PAGE_AREA:
            {
                if (pstream_) {
                    const PageArea* page = SAFE_CAST(const PageArea*, area);
                    String s = NOTR("Formatting page ") +
                        String::number(page->pageNum());
                    *pstream_ << s;
                }
                CType bottom = area->absAllcPoint().y_ + area->allcH();
                DBG(GV.TEST)
                    << "PageArea view made:" << area
                    << " bottom:" << bottom
                    << " scene_height:" << scene->height()
                    << " rootViewMade:" << rootViewMade_
                    << std::endl;
                 QRectF rect(scene->sceneRect());
                 if (rootViewMade_ && bottom > rect.height()) {
                    rect.setHeight(bottom);
                    scene->setSceneRect(rect);
                 }
            }
            return new PageAreaView(area, scene, isPaginated_);
        case REGION_AREA:
        case LINE_AREA:
            return new ContentAreaView(area, scene);
        case GRAPHIC_AREA:
            return new GraphicAreaView(area, scene);
        case INLINE_AREA:
        case BLOCK_AREA:
            return new TaggedAreaView(area, scene);
        case TEXT_AREA:
            return new TextAreaView(area, scene);
        case SECTION_CORNER_AREA:
            return new SectionCornerView(area, scene);
        case CHOICE_AREA:
            return new ChoiceAreaView(area, scene);
        case FOLD_AREA:
            return new FoldAreaView(area, scene);
        case COMBO_BOX_AREA:
            return new ComboBoxAreaView(area, scene);
        case LINE_EDIT_AREA:
            return new LineEditAreaView(area, scene);
        case INLINE_OBJECT_AREA: 
            return new MmlAreaView(area, scene);
        case COMMENT_AREA:
        case PI_AREA: 
        case UNKNOWN_AREA:
            return new SelectableAreaView(area, scene);
        default:
            break;
    }
    return 0;
}
