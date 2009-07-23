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
#include "sceneareaset/PixmapImage.h"
#include "sceneareaset/debug.h"
#include "ui/PixmapCache.h"

#include <QPainter>
#include "ui/common_xpms.i"

///////////////////////////////////////////////////////////////////////////

using Formatter::CRange;
using Formatter::CType;
using namespace Common;
using namespace Sui;

static QPixmap* broken_pixmap_inst = 0;

QPixmap& ScenePixmapImage::brokenPixmap()
{
    if (broken_pixmap_inst)
        return *broken_pixmap_inst;
    broken_pixmap_inst = new QPixmap(broken_image_xpm);
    return *broken_pixmap_inst;
}

void ScenePixmapImage::setSize(qreal w, qreal h)
{
    QRectF new_rect(rect().x(), rect().y(), w, h);
    if (rect() != new_rect)
        setRect(new_rect);
}

void ScenePixmapImage::paint(QPainter* painter, 
                             const QStyleOptionGraphicsItem*,
                             QWidget*) 
{
    const QPixmap* p = pixmap();
    setSize(p->width(), p->height());
    painter->drawPixmap(QPointF(rect().x(), rect().y()), *p);
}

///////////////////////////////////////////////////////////////////////////
    
DirectPixmapImage::DirectPixmapImage(const QPixmap& pixmap, 
                                     QGraphicsScene* scene)
    : ScenePixmapImage(scene), pixmap_(pixmap)
{
    setSize(pixmap.width(), pixmap_.height());
}

CRange DirectPixmapImage::size(bool* isFound) const
{
    if (isFound)
        *isFound = true;
    return CRange(CType(pixmap_.width()), CType(pixmap_.height()));
}

bool DirectPixmapImage::isEqual(const Formatter::Image* other) const
{
    return pixmap() == static_cast<const ScenePixmapImage*>(other)->pixmap();
}

///////////////////////////////////////////////////////////////////////////

CachedPixmapImage::CachedPixmapImage(const String& url,
                                     const CRange& new_size,
                                     QGraphicsScene* scene)
    : ScenePixmapImage(scene)
{
    if (new_size.w_) {
        pixmapItem_ = PixmapCache::instance().getScaled(url, 
            QSizeF(new_size.w_, new_size.h_));
        setSize((int)new_size.w_, (int)new_size.h_);
    } else {
        pixmapItem_ = PixmapCache::instance().getOriginal(url);
        const CRange my_size(size(0));
        setSize((int)my_size.w_, (int)my_size.h_);
    }
    DBG(GV.IMAGE) << "CachedPixmapImage cons: " << this << std::endl;
}

const QPixmap* CachedPixmapImage::pixmap() const
{
    if (pixmapItem_ && pixmapItem_->getPixmap())
        return pixmapItem_->getPixmap();
    return &brokenPixmap();
}

CRange CachedPixmapImage::size(bool* isFound) const
{
    if (isFound)
        *isFound = !!pixmapItem_;
    if (pixmapItem_) {
        const QSizeF& origSize = pixmapItem_->originalSize();
        return CRange(origSize.width(), origSize.height());
    }
    return CRange(brokenPixmap().width(), brokenPixmap().height()); 
}
    
void CachedPixmapImage::resize(const Formatter::CRange& new_size) 
{
    DBG(GV.IMAGE) << "CachedPixmapImage: " << this << ", resize to h="
        << new_size.h_ << ", w=" << new_size.w_ << std::endl;
    QSizeF new_qsize(new_size.w_, new_size.h_);
    if (pixmapItem_ && new_qsize != pixmapItem_->scaledSize()) {
        DBG(GV.IMAGE) << "ScenePixmapImage: updating pixmapItem to size\n";
        pixmapItem_ = PixmapCache::instance().
            getScaled(pixmapItem_->url(), new_qsize);
    }
}

bool CachedPixmapImage::isEqual(const Formatter::Image* other) const
{
    const CachedPixmapImage* img = 
        static_cast<const CachedPixmapImage*>(other);
    return pixmapItem_ && img->pixmapItem_ && 
        pixmapItem_->scaledSize() == img->pixmapItem_->scaledSize() &&
        pixmapItem_->url() == img->pixmapItem_->url();
}

CachedPixmapImage::~CachedPixmapImage()
{
    DBG(GV.IMAGE) << "~CachedPixmapImage: " << this << ", URL = "
        << (pixmapItem_ ? pixmapItem_->url() : String("NONE")) << std::endl;
}

///////////////////////////////////////////////////////////////////////////

Formatter::Image* 
SceneImageProvider::makeExternalImage(const String& url,
                                       const CRange& size) const
{
    return new CachedPixmapImage(url, size, scene_);
}

Formatter::Image*  
SceneImageProvider::makeBuiltinImage(BuiltinImageType type) const
{
    static QPixmap comment_pixmap(comment_xpm),
                   pi_pixmap(pi_xpm),
                   unknown_pixmap(unknown_xpm);
    switch (type) {
        case COMMENT_ICON:
            return new DirectPixmapImage(comment_pixmap, scene_);
        case PI_ICON:
            return new DirectPixmapImage(pi_pixmap, scene_);
        case UNKNOWN_ICON:
        default:
            return new DirectPixmapImage(unknown_pixmap, scene_);
    }
}

///////////////////////////////////////////////////////////////////////////

