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
#ifndef SAPI_GROVE_H_
#define SAPI_GROVE_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/WrappedObject.h"
#include "sapi/common/RefCntPtr.h"
#include "sapi/common/SString.h"
#include "sapi/grove/GroveDecls.h"
#include "sapi/grove/GroveIdManager.h"
#include "sapi/grove/GroveStripInfo.h"
#include "sapi/grove/CatalogManager.h"

namespace SernaApi {

class GroveIdManager;
class GroveEntityDeclSet;

/// XML document grove instance
class SAPI_EXPIMP Grove : public RefCountedWrappedObject {
public:
    Grove(SernaApiBase* = 0);
    Grove(const SString& rootElementName);
    virtual ~Grove();

    /// Returns Document node of a document. This is a top-level
    /// node for a document (except prolog).
    GroveDocument       document() const;

    /// Returns associated #ID manager.
    GroveIdManager      idManager();

    /// Returns DOCTYPE name string.
    SString             doctypeName() const;

    /// Returns SYSID of the document.
    SString             topSysid() const;

    /// Returns a set of general entity declarations.
    GroveEntityDeclSet  entityDecls() const;

    /// Returns a set of parameter entity declarations.
    GroveEntityDeclSet  parameterEntityDecls() const;

    /// Returns a set of notations
    GroveEntityDeclSet  notations() const;

    //////////////////////////////////////////////////////////

    enum GroveBuilderFlags {
        SGML = 0001,             //! Input is SGML file (not XML)
        noIdProcessing = 0002,   //! don't process #ID's
        noSSep = 0004,           //! don't preserve space seps in instances
        noProlog = 0010,         //! don't build document prolog
        noMarkedSections = 0020, //! don't preserve marked sections info
        noEntitySections = 0040, //! don't preserve entity sections  info
        noSections = (noMarkedSections|noEntitySections),
        noComments = 0100,       //! no comment nodes
        pureXmlData = (noProlog|noSSep|noSections|noComments),
        noAttrGroups = 0200,     //! don't attempt to group attribute decls
        noExtSubsetDecls = 0400, //! don't include ext. subset declarations
        lineInfo = 01000         //! build line information for each node
    };
    /// Build grove from string and return result
    static Grove buildGroveFromString(const SString& s,
                                      GroveBuilderFlags flags = pureXmlData,
                                      bool dtdValidate = false,
                                      const CatalogManager& = CatalogManager());
    /// Build grove from file
    static Grove buildGroveFromFile(const SString& fn,
                                    GroveBuilderFlags flags = pureXmlData,
                                    bool dtdValidate = false,
                                    const CatalogManager& = CatalogManager());
    /// Grove saving flags
    enum SaveFlags {
        GS_SAVE_PROLOG      = 0001, //! Save prolog
        GS_SAVE_CONTENT     = 0002, //! Save content
        GS_SAVE_ENTITIES    = 0004, //! Save external entities
        GS_EXPAND_ENTITIES  = 0010, //! Expand entities
        GS_SAVE_DEFATTRS    = 0020, //! Save with default attribute values
        GS_FORCE_SAVE       = 0040, //! Save even unmodified entities
        GS_RECURSIVE        = 0100, //! Save all sub-groves, if any
        GS_INDENT           = 0200, //! Indent output
        GS_DONTSAVEDEFENC   = 0400, //! don't always save default encoding
        GS_DEF_FILEFLAGS = (GS_SAVE_PROLOG|GS_SAVE_CONTENT|GS_SAVE_ENTITIES|
                            GS_RECURSIVE),
        GS_DEF_STRFLAGS  = (GS_SAVE_PROLOG|GS_SAVE_CONTENT)
    };
    /// Save grove to file or URL
    bool    saveAsXmlFile(int flags = GS_DEF_FILEFLAGS, 
                          const GroveStripInfo& = GroveStripInfo(),
                          const SString& saveAs = SString());
    /// Save grove to string
    bool    saveAsXmlString(SString& saveTo,
                            int flags = GS_DEF_STRFLAGS, 
                            const GroveStripInfo& = GroveStripInfo());
    
};

} // namespace SernaApi

#endif // SAPI_GROVE_H_
