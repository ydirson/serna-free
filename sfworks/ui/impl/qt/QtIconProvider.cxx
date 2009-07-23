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
#include "ui/IconProvider.h"
#include "common/PathName.h"
#include "common/Singleton.h"
#include "common/CompiledResourceReader.h"
#include "common/OwnerPtr.h"
#include "common/SubAllocator.h"
#include "common/Vector.h"
#include "dav/DavManager.h"
#include "dav/Session.h"
#include "dav/DavQIODevice.h"

#include <QDir>
#include <QIcon>
#include <QPixmapCache>
#include <QPixmap>

#include <map>
#include <iostream>

USING_COMMON_NS;

namespace {

class IconSource {
public:
    virtual QPixmap build() = 0;
    virtual ~IconSource() {}
};

class FileIconSource : public IconSource {
public:
    String          basePath;
    String          fileName;
    virtual QPixmap build()
    {
        return QPixmap(basePath + "/" + fileName);
    }
    USE_SUBALLOCATOR
};

class BuiltinIconSource : public IconSource {
public:
    uint        length;
    const unsigned char* data;
    virtual QPixmap build()
    {
        QPixmap pixmap;
        pixmap.loadFromData(data, length);
        return pixmap;
    }
    USE_SUBALLOCATOR
};

class IconCacheItem {
public:
    struct Item {
        QPixmap              pixmap;
        OwnerPtr<IconSource> source;
    } pixmaps[2];
    QIcon                 icon_set;
    QPixmap getPixmap(Sui::IconProvider::PixmapClass pc)
    {
        Item& item = pixmaps[pc];
        if (item.pixmap.isNull() && item.source)
            item.pixmap = item.source->build();
        return item.pixmap;
    }
    USE_SUBALLOCATOR
}; 

} // namespace

namespace Sui {

/////////////////////////////////////////////////////////////////////////

class IconProviderImpl : public IconProvider {
public:
    typedef std::map<String, IconCacheItem>   IconsCache;

    virtual QIcon    getIconSet(const String& name) const;
    virtual QPixmap     getPixmap(const String& name, 
                                  PixmapClass pc = ENABLED_PIXMAP) const;
    virtual void        getIconNames(Vector<String>& iconNames) const;

    virtual void        registerIconsFromDir(const String& dirpath,
                                             const String& prefix);
    virtual void        registerBuiltinIcons(const unsigned char* resource,
                                             const String& prefix);
    virtual void        registerIcon(const String& path,
                                     const String& prefix);
    virtual QPixmap     readPixmap(const Common::String& url) const;
    
private:
    void                setSource(const String& fn, const String& prefix,
                                  IconSource* source);
    mutable IconsCache  iconsCache_;
};

String parse_name(const String& fn, const String& prefix, bool& is_disabled)
{
    static const char  disabled_suffix[]   = "_disabled";
    static const int   disabled_suffix_len = sizeof(disabled_suffix) - 1;
    String result = PathName(fn).basename();
    if (result.right(disabled_suffix_len) == disabled_suffix) {
        is_disabled = true;
        result = result.left(result.length() - disabled_suffix_len);
    } else
        is_disabled = false;
    if (prefix.isEmpty() || result.left(1) == "_")
        return result;
    else
        return prefix + ":" + result;
}

void IconProviderImpl::setSource(const String& fn, 
                                 const String& prefix,
                                 IconSource* source)
{
    assert(source);
    bool is_disabled = false;
    String name = parse_name(fn, prefix, is_disabled);
    iconsCache_[name].pixmaps[is_disabled].source = source;
}

void IconProviderImpl::registerBuiltinIcons(const unsigned char* resource,
                                            const String& prefix)
{
    CompiledResourceReader icons_res(resource);
    String name;
    IconsCache::iterator it;
    for (;;) {
        OwnerPtr<BuiltinIconSource> source(new BuiltinIconSource);
        if (!icons_res.getNext(name, source->length, source->data))
            break;
        setSource(name, prefix, source.release());
    }
}
    
void IconProviderImpl::registerIcon(const String& path,
                                    const String& prefix)
{
    FileIconSource* source = new FileIconSource;
    source->basePath = PathName(path).dirname().name();
    source->fileName = PathName(path).filename();
    setSource(source->fileName, prefix, source);
}
    
void IconProviderImpl::registerIconsFromDir(const String& dirpath,
                                            const String& prefix)
{
    static const char* name_filter =
        "*.png;*.xpm;*.gif;*.pnm;*.xbm;*.jpg;*.bmp";
    QDir dir(dirpath);
    if (!dir.isReadable())
        return;
    String canonical_path = dir.canonicalPath();
    QStringList file_list = dir.entryList(name_filter,
        QDir::Files|QDir::Readable, QDir::Name);
    QStringList::iterator it = file_list.begin(); 
    for (; it != file_list.end(); ++it) {
        FileIconSource* source = new FileIconSource;
        source->basePath = canonical_path;
        source->fileName = *it;
        setSource(source->fileName, prefix, source);
    }
}

QPixmap IconProviderImpl::getPixmap(const String& name, PixmapClass pc) const
{
    if (name.isEmpty())
        return QPixmap();
    if (name.contains('/') || name.contains('\\')) {
        QPixmap* p = QPixmapCache::find(name);
        if (p)
            return *p;
        QPixmap new_pixmap(name);
        QPixmapCache::insert(name, new_pixmap);
        return new_pixmap;
    }
    IconsCache::iterator it = iconsCache_.find(name);
    if (it == iconsCache_.end()) 
        return QPixmap();
    return it->second.getPixmap(pc);
}

QIcon IconProviderImpl::getIconSet(const String& name) const
{
    if (name.isEmpty())
        return QIcon();
    if (name.contains('/') || name.contains('\\')) {
        QIcon icon_set = 
            QIcon(getPixmap(name, ENABLED_PIXMAP));
        icon_set.setPixmap(getPixmap(name, DISABLED_PIXMAP),
            QIcon::Automatic, QIcon::Disabled);
        return icon_set;             
    }
    IconsCache::iterator it = iconsCache_.find(name);
    if (it == iconsCache_.end())
        return QIcon(QPixmap());
    if (it->second.icon_set.isNull()) {
        it->second.icon_set = 
            QIcon(it->second.getPixmap(ENABLED_PIXMAP));
        it->second.icon_set.setPixmap(it->second.getPixmap(DISABLED_PIXMAP),
            QIcon::Automatic, QIcon::Disabled);
    }
    return it->second.icon_set;
}

void IconProviderImpl::getIconNames(Vector<String>& iconNames) const
{
    iconNames.reserve(256);
    IconsCache::const_iterator it = iconsCache_.begin();
    for (; it != iconsCache_.end(); ++it)
        iconNames.push_back(it->first);
}

QPixmap IconProviderImpl::readPixmap(const Common::String& url) const
{
    using namespace Dav;
    QPixmap pixmap;
    IoRequestHandle* dav_handle = 0;
    if (DavManager::instance().open(url, DAV_OPEN_READ, &dav_handle))
        return 0;
    DavQIODevice dev(dav_handle);
    QDataStream qds(&dev);
    qds.setVersion(4);
    qds >> pixmap;
    if (pixmap.isNull())
        return 0;
    return pixmap;
}

UI_EXPIMP IconProvider& icon_provider()
{
    return SingletonHolder<IconProviderImpl>::instance();
}
    
} // namespace Sui
