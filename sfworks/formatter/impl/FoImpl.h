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
/*! \file
 */

#ifndef FORMATTER_FO_IMPL_H
#define FORMATTER_FO_IMPL_H

#include "formatter/FontMgr.h"
#include "formatter/MediaInfo.h"
#include "formatter/impl/Fo.h"
#include "common/ModRegistry.h"
#include "common/CDList.h"

namespace Formatter {

class ModificationRegistry : public COMMON_NS::ModRegistry<Fo> {
public:
    virtual void    detachFromFormatter(Fo*, const FoController*) = 0;
    virtual bool    add(Fo*) = 0;

    virtual ~ModificationRegistry() {}
};

class TagMetrixTable;
class TextFo;

//! Following pointers are kept together to minimize Fo`s memory consumption
struct FoData {
    FoData(COMMON_NS::MessageStream& mstream,
           MediaInfo& mediaInfo, FontMgr* fontMgr,
           TagMetrixTable& tagMetrixTable,
           ModificationRegistry& foRegistry)
        : mstream_(mstream),
          mediaInfo_(mediaInfo),
          fontMgr_(fontMgr),
          tagMetrixTable_(tagMetrixTable),
          foRegistry_(foRegistry) {}

    COMMON_NS::MessageStream&   mstream_;
    const MediaInfo&            mediaInfo_;
    FontMgr* const              fontMgr_;
    TagMetrixTable&             tagMetrixTable_;
    ModificationRegistry&       foRegistry_;
    Common::CDList<TextFo>      textFoModList_;
};

/*! \brief
*/
class FORMATTER_EXPIMP FoImpl : public Fo {
public:
    FORMATTER_OALLOC(FoImpl);

    FoImpl(PropertySet* parentSet, bool hasParentOrigin, FoData& data);
    virtual ~FoImpl();

    //! If was not registered then register
    void            registerModification(ModificationType mType);
    //! If was registered then deregister
    void            deregisterModification();
    //!
    bool            isModified() const { return isRegistered_; }

    //! Returns true if given area has tail decoration
    bool            isChainFinishedAt(const AreaPtr& area) const;
    //! TODO: reimplement in TableFo, etc.
    PDirection      progression() const { return VERTICAL; }
    //! TODO: all necessary data for AreaViews
    const FoData&   data() const { return data_; }
    //!
    const MediaInfo& mediaInfo() const { return data_.mediaInfo_; }
    //!
    FontMgr*        fontMgr() const { return data_.fontMgr_; }
    //!
    TagMetrixTable& tagMetrixTable() const { return data_.tagMetrixTable_; }

protected:
    //!
    virtual void    detachFromFormatter(const FoController* preserveBranch);
    //! Returns true if area should contain decoration
    virtual bool    needsDecoration(Decoration type) const = 0;
    //!
    void            layoutChildrenOnBaseline(Area* area, const CType& accender,
                                             const CType& descender) const;

private:
    //! Reimplemented from PropertySet
    double          dpi() const { return data_.mediaInfo_.dpi_; }

protected:
    FoData&         data_;
private:
    bool            isRegistered_;
};

class FoInit {
public:
    FoInit(PropertySet* parentset, const GroveLib::Node* foNode,
          FoData& data, uint physicalLevel, uint logicalLevel,
          bool hasParentOrigin = false)
        : parentset_(parentset), foNode_(foNode), data_(data),
          physicalLevel_(physicalLevel), logicalLevel_(logicalLevel),
          hasParentOrigin_(hasParentOrigin) {}
    PropertySet*            parentset_;
    const GroveLib::Node*   foNode_;
    FoData&                 data_;
    uint                    physicalLevel_;
    uint                    logicalLevel_;
    bool                    hasParentOrigin_;
};

} // namespace Formatter

#endif // FORMATTER_FO_IMPL_H
