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
#ifndef GROVE_BUILDER_H_
#define GROVE_BUILDER_H_

#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "common/Encodings.h"
#include "common/String.h"
#include "common/RefCounted.h"
#include "common/Vector.h"
#include "common/MessageUtils.h"
#include "common/OwnerPtr.h"

namespace CatMgr {
    class CatalogManager;
}

namespace Common {
    class PropertyNode;
}

GROVE_NAMESPACE_BEGIN

class XmlValidatorProvider;
class MatchPatternFactory;

/*! This class provides abstract interface to grove builders, and also
 *  holds grove builder parameters.
 */
class GROVE_EXPIMP GroveBuilder {
public:
    enum Flags {
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
        lineInfo = 01000,        //! build line information for each node
        processRedlines = 02000, //! enable redline processing
        processXinclude = 04000, //! process XInclude(s)
        disableXincludeBackrefs = 010000,
        checkLocks =   020000,   //! check read-only status of entities
        doLocks    =   040000    //! attempt to lock all resources
    };
    /// Get current grove builder flags
    Flags   flags() const { return flags_; }

    /// Set new grove builder flags
    void    setFlags(int f) { flags_ = (Flags)f; }

    /// Build grove from file(s) and return result
    virtual GrovePtr buildGrove(const Common::Vector<Common::String>& files,
                                const Common::String& ssysid = Common::String(),
                                bool dtdValidate = false);

    /// Build grove from string and return result
    virtual GrovePtr buildGrove(const COMMON_NS::String& s,
                                const Common::String& ssysid = Common::String(),
                                bool dtdValidate = false);

    /// Build grove from single file (shortcut)
    GrovePtr        buildGroveFromFile(const COMMON_NS::String& fn,
                                       bool dtdValidate = false);

    void            setValidatorProvider(const XmlValidatorProvider* vp);

    const XmlValidatorProvider* getValidatorProvider() const 
        { return validatorProvider_.pointer(); }

    void            setCatalogManager(CatMgr::CatalogManager* catmgr);
    CatMgr::CatalogManager* getCatalogManager() const;
    
    /// Set/get DTD/external entities search path
    void            setSearchPath(const COMMON_NS::String& p) { spath_ = p; }
    const COMMON_NS::String& searchPath() const { return spath_; }

    /// Set encoding
    void            setEncoding(Common::Encodings::Encoding e) 
        { encoding_ = e; }

    /// Get encoding
    Common::Encodings::Encoding getEncoding() const { return encoding_; }

    /// Set messenger
    void            setMessenger(COMMON_NS::Messenger* m) { messenger_ = m; }

    /// Get messenger
    COMMON_NS::Messenger* getMessenger() { return messenger_.pointer(); }

    /// Returns number of elements in the built grove
    uint            numberOfElements() const { return numberOfElements_; }
    void            setNumberOfElements(uint n) { numberOfElements_ = n; }

    /// Set/get context (root) grove, used for Xinclude processing.
    void            setRootGrove(Grove* g) { rootGrove_ = g; }
    Grove*          rootGrove() const { return rootGrove_; }

    void            setIdManagerParams(Common::PropertyNode* scopeDesc,
                                       Common::PropertyNode* nsMap,
                                       MatchPatternFactory* factory);
    Common::PropertyNode* scopeDefs() const { return scopeDefs_.pointer(); }
    Common::PropertyNode* nsMap() const { return nsMap_.pointer(); }
    MatchPatternFactory* matchPatternFactory() const 
        { return matchPatternFactory_.pointer(); }

    /// Copy grove builder
    virtual GroveBuilder* copy() const;

    GroveBuilder(int flags = 0);
    virtual ~GroveBuilder();

    static void setDefaultCatalogManager(CatMgr::CatalogManager* catmgr);

    // for debug purposes
    Common::String  flagsAsString() const;

protected:
    GroveBuilder(const GroveBuilder&);
    GroveBuilder& operator=(const GroveBuilder&);
    
    COMMON_NS::RefCntPtr<COMMON_NS::Messenger>  messenger_;
    COMMON_NS::String                           spath_;
    Common::Encodings::Encoding                 encoding_;
    Flags                                       flags_;
    uint                                        numberOfElements_;
    CatMgr::CatalogManager*                     catMgr_;
    Common::RefCntPtr<Common::PropertyNode>     scopeDefs_;
    Common::RefCntPtr<Common::PropertyNode>     nsMap_;
    Common::OwnerPtr<MatchPatternFactory>       matchPatternFactory_;

private:
    Grove*                                      rootGrove_;
    Common::RefCntPtr<XmlValidatorProvider>     validatorProvider_;
};

GROVE_NAMESPACE_END

#endif // GROVE_BUILDER_H_
