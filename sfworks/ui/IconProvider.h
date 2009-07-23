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
#ifndef UI_ICON_PROVIDER_H_
#define UI_ICON_PROVIDER_H_

#include "ui/ui_defs.h"
#include "common/String.h"
//Added by qt3to4:
#include <QPixmap>

class QIcon;
class QPixmap;

namespace Common {
    template <class T> class Vector;
}

namespace Sui {

/*! This singleton class provides access to icons.
 */
class UI_EXPIMP IconProvider {
public:
    enum PixmapClass { ENABLED_PIXMAP, DISABLED_PIXMAP };
    /// register all icons from given directory
    virtual void        registerIconsFromDir(const Common::String& dirpath,
                                             const Common::String& prefix) = 0;
    /// Register built-in icons
    virtual void        registerBuiltinIcons(const unsigned char* resource,
                                             const Common::String& prefix) = 0;
    /// Register icon from specified path 
    virtual void        registerIcon(const Common::String& path,
                                     const Common::String& prefix) = 0;

    virtual QPixmap     getPixmap(const Common::String& name,
                                  PixmapClass pc = ENABLED_PIXMAP) const = 0;
    virtual QIcon    getIconSet(const Common::String& name) const = 0;
    virtual void        getIconNames(Common::Vector<Common::String>&) const = 0;

    virtual QPixmap     readPixmap(const Common::String& url) const = 0;

    virtual ~IconProvider() {}
};

UI_EXPIMP IconProvider& icon_provider(); 

} // namespace Sui

#endif // UI_ICON_PROVIDER_H_
