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
#ifndef SCENEAREASET_PIXMAP_IMAGE_H_
#define SCENEAREASET_PIXMAP_IMAGE_H_

#include "sceneareaset/defs.h"
#include "sceneareaset/SceneAreaView.h"
#include "editableview/TooltipView.h"
#include "formatter/Image.h"

#include <QPixmap>

namespace Sui {

class PixmapItem;

} // namespace Sui

class ScenePixmapImage : public Formatter::Image,
                          public QGraphicsRectItem,
                          public TooltipView {
public:
    ScenePixmapImage(QGraphicsScene* scene)
        : QGraphicsRectItem(0, scene), origin_(0) {}

    virtual const QPixmap* pixmap() const = 0;
    virtual ~ScenePixmapImage() {}

    virtual const Formatter::Area* tooltipViewArea() const { return origin_; }
    virtual RegionType regionType() const { return CONTENT_AREA_REGION; }
    virtual int type() const { return AREAVIEW_RTTI; } 
    void    setOrigin(const Formatter::Area* area) { origin_ = area; }
    void    setSize(qreal, qreal);

    static QPixmap& brokenPixmap();

private:
    virtual void        paint(QPainter*, const QStyleOptionGraphicsItem*,
                              QWidget*); 
    const Formatter::Area* origin_;
};

// DirectPixmapImage doesn't own the pixmaps - it is used with static
// pixmaps mostly
class DirectPixmapImage : public ScenePixmapImage {
public:
    DirectPixmapImage(const QPixmap& pixmap, QGraphicsScene* scene);
    virtual ~DirectPixmapImage() {}
    
    virtual const QPixmap*      pixmap() const { return &pixmap_; }
    virtual void  resize(const Formatter::CRange&) {}
    virtual Formatter::CRange   size(bool* isFound) const;
    virtual bool isEqual(const Formatter::Image* other) const;

private:
    const QPixmap&  pixmap_;
};

class CachedPixmapImage: public ScenePixmapImage {
public:
    CachedPixmapImage(const Common::String& url,
                      const Formatter::CRange& size,
                      QGraphicsScene* scene);
    virtual ~CachedPixmapImage();
    
    virtual const QPixmap*      pixmap() const;
    virtual Formatter::CRange   size(bool* isFound) const;
    virtual void  resize(const Formatter::CRange&);
    virtual bool  isEqual(const Formatter::Image* other) const;

private:
    CachedPixmapImage(const CachedPixmapImage&);
    CachedPixmapImage& operator=(const CachedPixmapImage&);
    
    Common::RefCntPtr<Sui::PixmapItem> pixmapItem_;
};

///////////////////////////////////////////////////////////////////////

class SceneImageProvider : public Formatter::ImageProvider {
public:
     SceneImageProvider(QGraphicsScene* scene)
        : scene_(scene) {}
        
     virtual Formatter::Image* 
        makeExternalImage(const Common::String& url,
                          const Formatter::CRange& size) const;
     virtual Formatter::Image* makeBuiltinImage(BuiltinImageType type) const;

private:
    QGraphicsScene* scene_;
};

#endif // SCENEAREASET_PIXMAP_IMAGE_H_
