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
#include "grove/NodeExt.h"
#include "grove/Nodes.h"
#include "grove/EntityDecl.h"
#include "grove/Origin.h"
#include "grove/grove_trace.h"

USING_COMMON_NS

GROVE_NAMESPACE_BEGIN

LineLocExt::LineLocExt(const OriginBase& loc)
    : line_(loc.line()), column_(loc.column()), udata_(0), readOnly_(0)
{
}

LineLocExt::LineLocExt(long line, long column)
    :  line_(line), column_(column), udata_(0), readOnly_(0)
{
}

NodeExt* LineLocExt::copy() const
{
    LineLocExt* le = new LineLocExt(line_, column_);
    le->udata_ = udata_;
    le->readOnly_ = readOnly_;
    le->setErs(getErs());
    return le;
}

void LineLocExt::dump() const
{
#ifdef GROVE_DEBUG
    DDBG << "NodeExt<Line/Col>: " << line_ << '/' << column_ << std::endl;
    EntityNodeExt::dump();
#endif // GROVE_DEBUG
}

LineLocExt::~LineLocExt()
{
}

//////////////////////////////////////////////////////////////////

NodeExt* EntityNodeExt::copy() const
{
    EntityNodeExt* en = new EntityNodeExt;
    en->setErs(ers_);
    return en;
}

void EntityNodeExt::dump() const
{
#ifdef GROVE_DEBUG
    if (ers_) {
        DDBG << "NodeExt<Entity>: " << ers_ << (ers_->entityDecl() ?
            String(" (" + ers_->entityDecl()->name() + ")") : String(""));
        DDBG << std::endl;
    }
#endif // GROVE_DEBUG
}

//////////////////////////////////////////////////////////////////

Common::String NodeOrigin::fileName() const
{
    if (node_.isNull() || !node_->grove())
        return String();
    return node_->grove()->topSysid();
}

int NodeOrigin::line() const
{
    if (node_.isNull() || !node_->nodeExt())
        return -1;
    const LineLocExt* le = node_->nodeExt()->asConstLineLocExt();
    return le->line();
}

int NodeOrigin::column() const
{
    if (node_.isNull() || !node_->nodeExt())
        return -1;
    const LineLocExt* le = node_->nodeExt()->asConstLineLocExt();
    return le->column();
}

String NodeOrigin::asString() const
{
    if (node())
        return nodePosToString(node());
    return String();
}

String PlainOrigin::asString() const
{
    String res = fileName();
    if (line() > 0)
        res += ":" + String::number(line());
    if (column() > 0)
        res += ":" + String::number(column());
    return res;
}

GROVE_EXPIMP String nodePosToString(const Node* n, int textIdx)
{
    Vector<const Node*> rpath;
    rpath.reserve(256);
    const Node* np;
    COMMON_NS::String res;
    uint cnt;

    if (n && n->nodeType() == Node::ATTRIBUTE_NODE)
        n = static_cast<const Attr*>(n)->element();

    for (; n && n->parent(); n = n->parent())
        rpath.push_back(n);
    for (int i = rpath.size() - 1; i >= 0; --i) {
        n = rpath[i];
        cnt = 1;
        np = n->parent()->firstChild();
        for (; np && np != n; np = np->nextSibling()) {
            if (np->nodeName() == n->nodeName())
                ++cnt;
        }
        res += n->nodeName();
        if (cnt > 1) {
            res += "[" + String::number(cnt) + "]";
        }
        if (i)
            res += '/';
    }
    if (textIdx >= 0)
        res += "[" + String::number(textIdx) + "]";
    return res;
}

GROVE_EXPIMP
const CompositeOrigin* getCompositeOrigin(const Common::Message* m)
{
    for (ulong i = 0; i < m->nArgs(); ++i) {
        const MessageArgBase* ma = m->getArg(i);
        if (ma->type() == MessageArgBase::USER_DEFINED + 1)
            return &static_cast<const MessageArg
                <CompositeOrigin>*>(ma)->value();
    }
    return 0;
}

//////////////////////////////////////////////////////////////////

PRTTI_BASE_STUB(NodeExt, LineLocExt)
PRTTI_BASE_STUB(NodeExt, XsNodeExt)
PRTTI_BASE_STUB(NodeExt, EntityNodeExt)
PRTTI_IMPL(LineLocExt)
PRTTI_IMPL(EntityNodeExt)

GROVE_NAMESPACE_END

namespace Common {

template<> GROVE_EXPIMP
String MessageArg<GroveLib::CompositeOrigin>::format() const
{
    if (!value_.docOrigin())
        return String();
    return value_.docOrigin()->asString();
}

} // namespace Common

