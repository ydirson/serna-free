// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class IconProvider {

%TypeHeaderCode
#include "sapi/app/IconProvider.h"
%End

public:
    enum PixmapClass { ENABLED_PIXMAP, DISABLED_PIXMAP };
    static QPixmap    getPixmap(const SString&,
                                PixmapClass = ENABLED_PIXMAP);
    static QIcon      getIcon(const SString&);
    
    /// Read the pixmap from specified URL. Pixmap is not cached.
    static QPixmap    readPixmap(const SString&);
};
