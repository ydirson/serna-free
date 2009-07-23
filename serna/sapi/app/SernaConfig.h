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

#ifndef SAPI_UTILS_SERNA_CONFIG_H_
#define SAPI_UTILS_SERNA_CONFIG_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/PropertyNode.h"

namespace SernaApi {
/// The access to the values read from the Serna configuration
class SAPI_EXPIMP SernaConfig {
public:
    /// Get root of the Serna configuration
    static PropertyNode root();

    /// Shortcut to root()->getProperty(prop)
    static SString      getProperty(const SString&);

    /// Get user home directory path
    static SString      getHomeDir();

    /// Get Serna installation path
    static SString      getDataDir();

    /// Get Serna configuration directory path
    static SString      getConfigDir();

    /// Resolve file resource using config "vars" section.
    static SString      resolveResource(const SString& defaultPropName,
                            const SString& src,
                            const SString& baseUri,
                            const PropertyNode& localVars = PropertyNode());
};

} // namespace SernaApi

#endif // SAPI_UTILS_SERNA_CONFIG_H
