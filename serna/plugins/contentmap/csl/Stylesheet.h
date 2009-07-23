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
#ifndef CSL_STYLESHEET_H_
#define CSL_STYLESHEET_H_

#include "common/RefCounted.h"

namespace GroveLib {
    class Element;
}

namespace Csl {

class Template;
class TemplateList;
                      
class Profile : public Common::XListItem<Profile> {
public:
    typedef Common::Vector<const Template*> TemplatesVector;

    const Common::String& name() const { return name_; }
    const Common::String& inscription() const { return inscription_; }
    const Common::String& icon() const { return icon_; }
    bool        showAttributes() const { return showAttributes_; }
    bool        showCursorBetween() const { return showCursorBetween_; }
    uint        cutoffLevel() const { return cutoffLevel_; }
    const TemplatesVector& templates() const { return templates_; }
    void        addTemplate(const Template* t) { templates_.push_back(t); }
    void        dump() const;
    
    Profile(const GroveLib::Element*);
    Profile();
    ~Profile();

private:
    TemplatesVector templates_;
    Common::String  name_;
    Common::String  inscription_;
    Common::String  icon_;
    bool            showAttributes_;
    bool            showCursorBetween_;
    uint            cutoffLevel_;
};

class ProfileList : public Common::OwnedXList<Profile> {};

class Stylesheet : public Common::RefCounted<> {
public:
    Stylesheet(const GroveLib::Element* topElem);
    ~Stylesheet();

    const ProfileList&  profiles() const { return profileList_; }   
    const TemplateList& templates() const { return *templateList_; }
    const Profile*      findProfile(const Common::String&) const;
    void                dump() const;
    GroveLib::DocumentFragment* processUse(const GroveLib::Element*) const;

private:
    class DefinesMap;
    Stylesheet(const Stylesheet&);
    Stylesheet& operator=(const Stylesheet&);

    ProfileList                    profileList_;
    Common::OwnerPtr<TemplateList> templateList_;
    Common::OwnerPtr<DefinesMap>   definesMap_;
};

} // namespace

#endif // CSL_STYLESHEET_H_
