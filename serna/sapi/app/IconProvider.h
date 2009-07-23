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
#ifndef SAPI_ICON_PROVIDER_H_
#define SAPI_ICON_PROVIDER_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"

#include <QPixmap>
#include <QIcon>

namespace SernaApi {

/// The singleton providing access to the Serna icons.

class SAPI_EXPIMP IconProvider {
public:
    /// Pixmap type, when pixmap is used as enabled/disabled icon.
    enum PixmapClass { ENABLED_PIXMAP, DISABLED_PIXMAP };
    
    /// Get the pixmap (icon) by name. Note that for accessing icons
    /// defined in plug-ins name must be of the form <plugin-name>:<icon-name>.
    /// If the name specified without prefix, then it is taken from the
    /// central Serna icons repository.
    static QPixmap     getPixmap(const SString& name,
                                 PixmapClass pc = ENABLED_PIXMAP);
                                 
    /// Get the icon set by name. Naming rules are the same as with getPixmap().
    static QIcon       getIcon(const SString& name);
                     
    /// Read the pixmap from specified URL. Pixmap is not cached.
    static QPixmap     readPixmap(const SString& url);
};

} // namespace SernaApi

#endif // SAPI_ICON_PROVIDER_H_
