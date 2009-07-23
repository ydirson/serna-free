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
#include "ui/PixmapCache.h"
#include "ui/impl/ui_debug.h"
#include "common/Singleton.h"

#include "common/PropertyTree.h"
#include "dav/DavManager.h"
#include "dav/Session.h"
#include "dav/DavQIODevice.h"

#include <QPixmap>
#include <QImage>
#include <QImageReader>
#include <QDataStream>
#include <QFile>
#include <QTimer>
#include <QTimerEvent>

#include <map>

#ifdef _WIN32
static const int PIXMAP_CACHE_LIMIT = 500;
#else
static const int PIXMAP_CACHE_LIMIT = 1500;
#endif // _WIN32

static const int HOLD_LIST_SIZE_LIMIT = 30;
static const int ITEM_HOLDER_CLEANUP_INTERVAL = 60000; // 60 secs 

using namespace Dav;
using namespace Common;
using namespace Sui;

#define IMDBG DBG(UI.IMAGE)

namespace Sui {

class ItemHolderCleanupTimer;

class PixmapCacheImpl : public PixmapCache {
public:
    class ItemHolder : public Common::RefCounted<>,
                       public Common::RefCntPtr<PixmapItem>,
                       public Common::XListItem<ItemHolder,
                            Common::XTreeNodeRefCounted<ItemHolder> > {
    public:
        ItemHolder(PixmapItem* item) : Common::RefCntPtr<PixmapItem>(item) {}
    };
    typedef Common::XList<ItemHolder,
        Common::XTreeNodeRefCounted<ItemHolder> > ItemHolderList;
    typedef std::multimap<String, PixmapItem*> PixmapMap;
    typedef std::pair<PixmapMap::iterator, PixmapMap::iterator> PixmapIterPair;
    
    virtual PixmapItem* getScaled(const Common::String& url,
                                  const QSizeF& size);
    virtual PixmapItem* getOriginal(const Common::String& url);
    virtual void        advanceSeq() { ++seq_; }
    virtual uint        seq() const { return seq_; }

    void    itemHolderTick();
    void    removeItem(PixmapItem*);

    static PixmapCacheImpl& instance() 
    { 
        return static_cast<PixmapCacheImpl&>(PixmapCache::instance());
    }
    PixmapCacheImpl();
    
private:
    friend class PixmapItem;
    typedef Common::XList<PixmapItem> PixmapList;
    
    PixmapCacheImpl(const PixmapCacheImpl&);
    PixmapCacheImpl& operator=(const PixmapCacheImpl&);
    PixmapItem*                 findItemWithImage(const Common::String& url);
    PixmapItem*                 add_to_cache(PixmapItem*);

    uint                        seq_;
    PixmapMap                   pixmapMap_;
    PixmapList                  pixmapList_;
    ItemHolderList              itemHolderList_;
    int                         cacheSize_;
    int                         holdSize_;
    OwnerPtr<ItemHolderCleanupTimer> holderTimer_;
}; 

class ItemHolderCleanupTimer : public QObject {
public:
    ItemHolderCleanupTimer() 
        : timerId_(0) {}
    void restartTimer()
    {
        if (timerId_)
            killTimer(timerId_);
        timerId_ = startTimer(ITEM_HOLDER_CLEANUP_INTERVAL);
    }
    virtual void timerEvent(QTimerEvent*)
    {
        PixmapCacheImpl::instance().itemHolderTick();
    }
    ~ItemHolderCleanupTimer() 
    {
        if (timerId_)
            killTimer(timerId_);
    }
public:
    int timerId_;
};

PixmapItem::PixmapItem(const Common::String& url, 
                       const QSizeF& size,
                       uint seq)
    : seq_(seq), modtime_(0), url_(url), scaledSize_(size), isOnHold_(false)
{
}

static uint get_modtime(const String& url)
{
    PropertyTree pt;
    if (DavManager::instance().getResourceInfo(url, pt.root()) != 
        Dav::DAV_RESULT_OK)
            return 0;
    bool ok = false;
    uint modtime = pt.root()->getString("modtime").toUInt(&ok);
    return ok ? modtime : 0;
}

bool PixmapItem::modtimeChanged() const
{
    return get_modtime(url_) > modtime_;
}

QImage* PixmapItem::getImage()
{
    if (image_) {
        IMDBG << "Found ready image for URL=" << url_ << std::endl;
        return image_.pointer();
    }
    IMDBG << "PixmapItem: loading QImage for URL=" << url_ << std::endl;
    modtime_ = get_modtime(url_);
    IoRequestHandle* dav_handle = 0;
    if (DavManager::instance().open(url_, DAV_OPEN_READ, &dav_handle))
        return 0;
    QByteArray format;
    // svg hack
    Dav::Url url(url_);
    if (url[Dav::Url::FILENAME].right(4).lower() == NOTR(".svg"))
        format = NOTR("svg");
    DavQIODevice dev(dav_handle);
    QImageReader img_io(&dev, format);
    image_ = new QImage(img_io.read());
    if (!image_->width() || !image_->height()) {
        image_ = 0;
        return 0;
    }
    origSize_ = QSizeF(image_->width(), image_->height());
    return image_.pointer();
}

const QPixmap* PixmapItem::getPixmap()
{
    if (pixmap_) {
        //IMDBG << "Return ready pixmap for URL=" << url_ << std::endl;
        return pixmap_.pointer();
    }
    PixmapItem* image_item = static_cast<PixmapCacheImpl&>
        (PixmapCache::instance()).findItemWithImage(url_);
    if (!image_item)
        image_item = this;
    QImage* image = image_item->getImage();
    if (0 == image)
        return 0;
    origSize_ = QSizeF(image->width(), image->height());
    //! Scale image if necessary
    if (scaledSize_.height() && scaledSize_.width() && 
        scaledSize_ !=  origSize_) {
           IMDBG << "Scaling image from original URL=" << url_ << std::endl;
           QImage scaled_image = 
             image->smoothScale(int(scaledSize_.width()),
                                int(scaledSize_.height()));
           pixmap_ = new QPixmap(scaled_image);
    } else {
        scaledSize_ = origSize_;
        pixmap_ = new QPixmap(*image);
    }
    if (pixmap_->isNull())
        pixmap_.clear();
    image_item->image_ = 0; // release 100% image after scaling
    IMDBG << "Returning built pixmap\n";
    return pixmap_.pointer();
}

void PixmapItem::releasePixmap()
{
    IMDBG << "Releasing pixmap data for URL=" << url_ << std::endl;
    pixmap_ = 0;
    image_  = 0;
}

PixmapItem::~PixmapItem()
{
    PixmapCacheImpl::instance().removeItem(this);
    DBG(UI.IMAGE) << "PixmapCache:~Item: " << url_ << std::endl;
}

void PixmapCacheImpl::removeItem(PixmapItem* item)
{
    PixmapIterPair matches(pixmapMap_.equal_range(item->url()));
    PixmapMap::iterator it = matches.first;
    for (; it != matches.second; ++it) { 
        if (it->second == item) {
            pixmapMap_.erase(it);
            return;
        }
    }
}
    
PixmapItem* PixmapCacheImpl::findItemWithImage(const Common::String& url)
{
    PixmapIterPair matches(pixmapMap_.equal_range(url));
    PixmapMap::iterator it = matches.first;
    for (; it != matches.second; ++it) {
        if (it->second->image()) {
            IMDBG << "Found QImage item for URL=" << url << std::endl;
            return it->second;
        }
        if (!it->second->scaledSize().width()) {
            it->second->getImage();
            return it->second;
        }
    }
    IMDBG << "QImage item NOT FOUND for URL=" << url << std::endl;
    return 0;
}

PixmapItem* PixmapCacheImpl::getOriginal(const Common::String& url)
{
    PixmapIterPair matches(pixmapMap_.equal_range(url));
    PixmapMap::iterator it = matches.first;
    PixmapItem* item = 0;
    for (; it != matches.second; ++it) {
        IMDBG << "traverse original: SS=" << it->second->scaledSize().width()
            << NOTR("/") << it->second->scaledSize().height() << std::endl;
        if (it->second->image() || 
            it->second->scaledSize() == it->second->originalSize() ||
            !it->second->scaledSize().width()) {
                item = it->second;
                if (seq() == item->seq()) {
                    IMDBG << "getOriginal: equal seq " << url << std::endl;
                    return add_to_cache(item);
                }
                if (item->modtimeChanged()) {
                    IMDBG << "getOriginal: modtime changed, remaking="
                        << url << std::endl;
                    pixmapMap_.erase(it);
                    break;
                }
                IMDBG << "getOriginal: returning existing item\n";
                return add_to_cache(item);
        }
    }
    IMDBG << "Original was not found\n";
    item = new PixmapItem(url, QSizeF(), seq());
    if (!item->getImage()) {
        delete item;
        return 0;
    }
    IMDBG << "PixmapCache:: adding to cache (original)\n";
    return add_to_cache(item);
}
    
PixmapItem* PixmapCacheImpl::getScaled(const Common::String& url,
                                       const QSizeF& size)
{
    IMDBG << "PixmapCache::getScaled<" << url
        << ", size=" << size.width() << "/" << size.height() << "\n";
    PixmapIterPair matches(pixmapMap_.equal_range(url));
    PixmapMap::iterator it = matches.first;
    PixmapItem* item = 0;
    for (; it != matches.second; ++it) {
        const QSizeF& ssize = it->second->scaledSize();
        IMDBG << "traversing(scaled): OS="
            << it->second->originalSize().width() 
            << NOTR("/") << it->second->originalSize().height()
            << NOTR(", SS=") << ssize.width() << NOTR("/")
            << ssize.height() << std::endl;
        if (ssize == size || 
           (!ssize.width() && it->second->originalSize() == size)) {
                item = it->second;
                if (seq() == item->seq()) {
                    IMDBG << "getScaled: equal seq " << url << std::endl;
                    return add_to_cache(item);
                }
                if (item->modtimeChanged()) {
                    IMDBG << "getScaled: modtime changed, remaking="
                        << url << std::endl;
                    pixmapMap_.erase(it);
                    break;
                }
                IMDBG << "getScaled: returning existing item\n";
                return add_to_cache(item);
        }
    }
    IMDBG << "PixmapCache::ready scaled pixmap not found for "
        << url << std::endl;
    item = new PixmapItem(url, size, seq());
    if (!item->getPixmap()) {
        delete item;
        return 0;
    }
    IMDBG << "PixmapCache:: adding to cache (Scaled)\n";
    return add_to_cache(item);
}

PixmapItem* PixmapCacheImpl::add_to_cache(PixmapItem* item)
{
    holderTimer_->restartTimer();
    if (!item->isOnHold()) {
        if (++holdSize_ > HOLD_LIST_SIZE_LIMIT) {
            --holdSize_;
            itemHolderList_.firstChild()->pointer()->setOnHold(false);
            itemHolderList_.firstChild()->remove();
        }
        item->setOnHold(true);
        itemHolderList_.appendChild(new ItemHolder(item));
    }
    IMDBG << "add_to_cache: hold size=" << holdSize_
        << ", refcnt=" << item->getRefCnt() << std::endl;
    if (item->parent()) { // already in cache - just move around
        RefCntPtr<PixmapItem> item_holder(item);
        item->remove();
        pixmapList_.appendChild(item);
        return item;
    }
    pixmapMap_.insert(PixmapMap::value_type(item->url(), item));
    pixmapList_.appendChild(item);
    if (++cacheSize_ > PIXMAP_CACHE_LIMIT) {
        IMDBG << "PixmapCache:: Cache limit exceeded\n";
        PixmapItem* to_remove = pixmapList_.firstChild();
        removeItem(to_remove);
        to_remove->releasePixmap();
        to_remove->remove();
        --cacheSize_;
    }
    return item;
}

void PixmapCacheImpl::itemHolderTick()
{
    IMDBG << "PixmapItemHolder::Tick\n";
    if (itemHolderList_.firstChild()) {
        itemHolderList_.firstChild()->pointer()->setOnHold(false);
        itemHolderList_.firstChild()->remove();
        --holdSize_;
    }
}
    
PixmapCache& PixmapCache::instance()
{
    return SingletonHolder<PixmapCacheImpl>::instance();
}

PixmapCacheImpl::PixmapCacheImpl()
    : seq_(0),
      cacheSize_(0), holdSize_(0), holderTimer_(new ItemHolderCleanupTimer)
{
}

} // namespace Sui
