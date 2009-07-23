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
#include "common/PathName.h"
#include "structeditor/StructEditor.h"
#include "structeditor/impl/debug_se.h"
#include "docview/SernaDoc.h"

#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/XmlNs.h"
#include "grove/StripInfo.h"
#include "xslt/ResultOrigin.h"

#include <QFileDialog>

using namespace Common;
using namespace GroveLib;

static void clean_serna_extensions(Node* n)
{
    NodeWithNamespace* node = n->asNodeWithNamespace();
    if (!node)
        return;
    if (NOTR("se") == node->xmlNsPrefix()) {
        node->remove();
        return;
    }
    if (Node::ELEMENT_NODE == node->nodeType()) {
        Element* elem = static_cast<Element*>(n);
        Attr* attr = elem->attrs().firstChild();
        Attr* next_attr = 0;
        while (attr) {
            next_attr = attr->nextSibling();
            clean_serna_extensions(attr);
            attr = next_attr;
        }
        Node* cur_node = elem->firstChild();
        Node* next_node = 0;
        while (cur_node) {
            next_node = cur_node->nextSibling();
            clean_serna_extensions(cur_node);
            cur_node = next_node;
        }
    }
}

static void add_serna_extensions(const Node* foNode, Node* node)
{
    if (node->nodeType() == GroveLib::Node::ELEMENT_NODE) {
        const Node* originNode = Xslt::resultOrigin(foNode);
        if (originNode) {
            Attr* attr = new Attr(NOTR("se:origin"));
            attr->setValue(originNode->nodeName());
            static_cast<Element*>(node)->attrs().appendChild(attr);
        }
    }
    foNode = foNode->firstChild();
    for (node = node->firstChild(); node; node = node->nextSibling()) {
        add_serna_extensions(foNode, node);
        foNode = foNode->nextSibling();
    }
}

class MyStripper : public StripInfo {
public:
    virtual bool    checkStrip(const Element*) const { return true; }
    virtual void    strip(Node*) const { return; }
};

SIMPLE_COMMAND_EVENT_IMPL(DumpFoTree,   StructEditor)

bool DumpFoTree::doExecute(StructEditor* se, EventData*)
{
    String fo_path = se->getDsi()->
                   getSafeProperty(DocSrcInfo::DUMP_FO_PATH)->getString();

    PathName path_name = PathName(se->grove()->topSysid());
    PathName sysid = fo_path.isEmpty() ? path_name.dirname() :
                                         PathName(fo_path);
    sysid.append(path_name.basename() + NOTR(".fo"));
    String path = sysid.absolute().name();
    String caption = tr("Dump FO Tree");
    String filter  = tr("FO files (*.fo);;XML files (*.xml);;All files (*)");
#if defined(__APPLE__) && 0
    QFileDialog file_dialog(path, filter,                                               se->widget(), NOTR("save_fo_tree"));
    file_dialog.setCaption(caption);
    file_dialog.setMode(QFileDialog::AnyFile);
    file_dialog.setSelection(path);
    if (file_dialog.exec() == QDialog::Accepted)
        path = file_dialog.selectedFile();
    else
        return false;
#else
    path = QFileDialog::getSaveFileName(se->widget(), caption, path, filter);
#endif
    if (path.isEmpty())
        return false;

    int res = se->sernaDoc()->showMessageBox(
        SernaDoc::MB_INFO, tr("Saving FO Tree"),
        tr("Document FO tree can have Serna-specific extension FO's.\n"
           "Do you want to omit Serna-specific extensions?"),
        tr("&Omit"), tr("&Don't omit")); 

    GrovePtr grove = new Grove;
    grove->document()->appendChild(se->fot()->document()->
                                   documentElement()->cloneNode(true));
    XmlNs::makePredefinedNamespaces(grove.pointer());
    int saveFlags = Grove::GS_SAVE_CONTENT|Grove::GS_EXPAND_ENTITIES|
        Grove::GS_INDENT|Grove::GS_FORCE_SAVE;
    if (res) {
        add_serna_extensions(se->fot()->document()->documentElement(),
                             grove->document()->documentElement());
        saveFlags |= Grove::GS_INDENT;
    } 
    else
        clean_serna_extensions(grove->document()->documentElement());
    grove->setTopSysid(path);
    MyStripper ms;
    if (!grove->saveAsXmlFile(saveFlags, &ms)) {
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_CRITICAL, tr("Save document FO tree"),
            tr("Document FO tree cannot be saved to:\n%1"
               "\nPlease check permission rights.").arg(path), tr("&Ok"));
        return false;
    }
    String dir = PathName(path).dirname().name();
    se->getDsi()->makeDescendant(DocSrcInfo::DUMP_FO_PATH)->setString(dir);
    return true;
}
