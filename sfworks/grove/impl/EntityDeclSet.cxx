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
#include "grove/grove_defs.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityDecl.h"
#include "grove/PrologNodes.h"
#include "grove/grove_trace.h"

using namespace Common;

GROVE_NAMESPACE_BEGIN

void EntityDeclSet::insertDecl(EntityDecl* d, bool replace)
{
    if (replace) {
        iterator it = table_.find(d->name());
        if (it != table_.end()) {
            if ((*it)->entityDeclNode()) {
                (*it)->entityDeclNode()->setDecl(d);
                d->setEntityDeclNode((*it)->entityDeclNode());
            }
            table_.erase(it);
        }
    }
    table_.insert(d);
}

void EntityDeclSet::removeDecl(const String& name)
{
    iterator it = table_.find(name);
    if (it == end())
        return;
    EntityDeclNode* edn = (*it)->entityDeclNode();
    if (edn) {
        Node* n = edn->nextSibling();
        // beautification: removing extra newline after decl
        if (n && n->nodeType() == Node::SSEP_NODE) {
            SSepNode* sn = static_cast<SSepNode*>(n);
            const String& snd = sn->data();
            const unsigned sz = snd.length();
            if (0 < sz && '\n' == snd[0]) {
                if (1 == sz)
                    sn->remove();
                else
                    sn->setData(snd.substr(1, sz - 1));
            }
        }
        edn->remove();
    }
    table_.erase(it);
}

void EntityDeclSet::rebuildInternalEntityContents(Grove* g)
{
    for (iterator it = table_.begin(); it != table_.end(); ++it) {
        InternalEntityDecl* ie = (*it)->asInternalEntityDecl();
        if (0 == ie)
            continue;
        if (!ie->isContentModified())
            continue;
        ie->rebuildContent(g);
    }
}

#ifdef GROVE_DEBUG
void EntityDeclSet::dump() const
{
    DDINDENT;
    DDBG << "EntityDeclSet:";
    for (iterator it = table_.begin(); it != table_.end(); ++it)
        (*it)->dump();
}
#else
void EntityDeclSet::dump() const
{
}
#endif // GROVE_DEBUG

GROVE_NAMESPACE_END
