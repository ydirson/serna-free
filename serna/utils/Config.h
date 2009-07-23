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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifndef SERNA_CONFIG_H_
#define SERNA_CONFIG_H_

#include "utils/utils_defs.h"
#include "common/String.h"

namespace Common {
    class PropertyNode;
    class PropertyTreeEventFactory;
    class DynamicEventFactory;
}

class UTILS_EXPIMP Config {
public:
    typedef COMMON_NS::String String;
    /// Initialize config manager
    virtual void    init(int argc = 0, const char* const* argv = 0,
                         const String& cfgTemplate = String::null()) = 0;

    virtual void    merge(const String& cfgTemplate) = 0;

    /// Get value of a named property. If property not found, property
    /// with null value is returned (not the null pointer).
    virtual const COMMON_NS:: PropertyNode*
                    getProperty(const String& propPath) const = 0;

    /// Accessor for property updates event factory
    virtual COMMON_NS::PropertyTreeEventFactory& eventFactory() = 0;

    /// Accessor for update transaction finish event factory
    virtual COMMON_NS::DynamicEventFactory& updateFinishFactory() = 0;

    /// Direct access to the property tree
    virtual COMMON_NS::PropertyNode*    root() const = 0;

    /// Get home directory for the current user
    virtual String  getHomeDir() const = 0;

    /// Get Serna configuration directory
    virtual String  getConfigDir() const = 0;

    /// Get Serna installation directory
    virtual String  getDataDir() const = 0;

    /// Returns version of configuration. Note that this value is compiled
    /// in and NOT taken from config file.
    virtual String  configVersion() const = 0;

    /// Save changed config data to disk
    virtual void    save() = 0;

    /// Dispatch update events
    virtual void    update() = 0;

    virtual String  resolveResource(const String& defaultPropName,
        const String& src, const String& baseUri,
        const Common::PropertyNode* localVars = 0) const = 0;
    virtual ~Config() {}
};

/// Accessor to config singleton. With this, access to properties
/// is like that: String s = config().getProperty("blah")->get<String>();
UTILS_EXPIMP Config& config();
UTILS_EXPIMP void set_trace_tags();
UTILS_EXPIMP void set_ldlibpath();

#endif // SERNA_CONFIG_H
