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
#ifndef _PROPERTY_TREE_SAVER_H_
#define _PROPERTY_TREE_SAVER_H_

#include "common/String.h"
#include "common/RefCntPtr.h"
#include "grove/Decls.h"
#include "proputils/proputils_exports.h"

namespace Common {
    class PropertyNode;
} // namespace

namespace GroveLib {
    class Element;
}

namespace PropUtils {

class PROPUTILS_EXPIMP PropertyTreeSaver {
public:
    PropertyTreeSaver(Common::PropertyNode*,
                      const Common::String& rootTag = "propertySet");
    ~PropertyTreeSaver();

    /// Return TRUE on success, FALSE on errors (in this case error message
    /// can be fetched with errmsg())
    bool readPropertyTree(const Common::String& fn = Common::String());
    bool savePropertyTree(const Common::String& fn = Common::String());
    bool mergePropertyTree(const Common::String& filename);

    /// Error message (if any)
    const Common::String& errmsg() const { return errmsg_; }
    const Common::String& rootTag() const { return rootTag_; }

private:
    bool error(const Common::String&);
    bool set_filename(const Common::String& filename);
    bool prepare_read(const Common::String& filename,
                      Common::RefCntPtr<GroveLib::Element>&);

    Common::String   rootTag_;
    Common::String   version_;
    Common::String   filename_;
    Common::String   errmsg_;
    Common::RefCntPtr<Common::PropertyNode> root_;
};

} // namespace PropUtils

#endif // PROPERTY_TREE_SAVER_H_
