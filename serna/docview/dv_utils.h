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
#ifndef DV_UTILS_H
#define DV_UTILS_H

#include "docview/dv_defs.h"
#include "docview/SernaDoc.h"
#include "common/String.h"
#include "common/PropertyTree.h"
#include "ui/UiItem.h"

namespace Sui {
    class Document;
    class Item;
}

DOCVIEW_EXPIMP void save_ui_item(Sui::Item* item, Common::PropertyNode* node,
                                 bool isTopLevel);
DOCVIEW_EXPIMP Sui::Item*    restore_ui_item(Sui::Document* doc,
                                            const Common::PropertyNode* node,
                                            Sui::Item* nodeItem);
DOCVIEW_EXPIMP void install_ui_item(Sui::Document* doc,
                                    const Sui::ItemPtr& item);
DOCVIEW_EXPIMP void install_ui_items(Sui::Document* doc,
                                     const Common::PropertyNode* node);

/////////////////////////////////////////////////////////////////////////////

DOCVIEW_EXPIMP void save_view_on_close(SernaDoc* doc);
DOCVIEW_EXPIMP void toggle_save_view(SernaDoc* doc, bool dontSave);

/////////////////////////////////////////////////////////////////////////////

class DOCVIEW_EXPIMP PropertyMaker {
public:
    virtual Common::PropertyNode* makeProperty() = 0;
    virtual ~PropertyMaker() {}
};

DOCVIEW_EXPIMP bool save_specific_file(Common::PropertyNode* prop,
                                       const Common::String& path,
                                       const Common::String& extension);
DOCVIEW_EXPIMP bool load_most_specific_file(SernaDoc* doc,
                                            Common::PropertyNode* prop,
                                            const Common::String& extension,
                                            SernaDoc::Level& level);

DOCVIEW_EXPIMP void restore_specific_file(const Common::PropertyNode* fromList,
                                          const Common::String& suffix);
DOCVIEW_EXPIMP bool is_original_level(const SernaDoc::Level);

/////////////////////////////////////////////////////////////////////////////

DOCVIEW_EXPIMP void save_recent_urls(const Common::PropertyNode* prop,
                                        const Common::String& protocol);

DOCVIEW_EXPIMP void make_browse_url_list(const Common::String& baseUrl,
                                         Common::PropertyNode* protocol,
                                         Common::PropertyNode* urlListParent,
                                         Common::PropertyNode* savedProtocol);

DOCVIEW_EXPIMP Common::String removed_item_path(Sui::Item* item);

#endif // DV_UTILS_H
