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
#ifndef UI_IMPL_PIXMAP_CACHE_H_
#define UI_IMPL_PIXMAP_CACHE_H_

#include "ui/ui_defs.h"
#include "common/String.h"
#include "common/RefCounted.h"
#include "common/OwnerPtr.h"
#include "common/XList.h"

#include <QPixmap>
#include <QSizeF>

class QPixmap;
class QImage;

namespace Sui {

class UI_EXPIMP PixmapItem : public Common::RefCounted<>,
                             public Common::XListItem<PixmapItem> {
public:
    PixmapItem(const Common::String& url, 
               const QSizeF& size,
               uint seq);
    ~PixmapItem();
    
    bool                        isExpensive() { return false; }
    const QPixmap*              getPixmap();
    QImage*                     getImage();
    void                        releasePixmap();
    const Common::String&       url() const { return url_; }
    const QSizeF&               scaledSize() const { return scaledSize_; }
    void  setScaledSize(const QSizeF& size) { scaledSize_ = size; }
    const QSizeF&               originalSize() const { return origSize_; }
    QImage*                     image() const { return image_.pointer(); }
    bool                        isOnHold() const { return isOnHold_; }
    void                        setOnHold(bool v) { isOnHold_ = v; }
    uint                        seq() const { return seq_; }
    bool                        modtimeChanged() const;

private:
    PixmapItem(const PixmapItem&);
    PixmapItem& operator=(const PixmapItem&);

    uint                        seq_;       // cache seq at creation time
    uint                        modtime_;
    Common::OwnerPtr<QPixmap>   pixmap_; 
    Common::OwnerPtr<QImage>    image_;     // 100% image
    const Common::String        url_;
    QSizeF                      scaledSize_;
    QSizeF                      origSize_;
    bool                        isOnHold_;
};        

class UI_EXPIMP PixmapCache {
public:
    virtual PixmapItem* getScaled(const Common::String& url,
                                  const QSizeF& size) = 0;
    virtual PixmapItem* getOriginal(const Common::String& url) = 0;

    // cache sequence
    virtual void        advanceSeq() = 0;
    virtual uint        seq() const = 0;

    static PixmapCache& instance();
    
    virtual ~PixmapCache() {}
};

} // namespace Sui

#endif // UI_IMPL_PIXMAP_CACHE_H_

