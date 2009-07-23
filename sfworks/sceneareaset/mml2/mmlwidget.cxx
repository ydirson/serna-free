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
 /****************************************************************************
**
** Copyright (C) 2003-2005 Trolltech AS. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** Licensees holding valid Qt Solutions licenses may use this file in
** accordance with the Qt Solutions License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/products/solutions/index.html 
** or email sales@trolltech.com for information about Qt Solutions
** License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <qapplication.h>
#include <qstring.h>
#include <qmap.h>
#include <qdom.h>
#include <qdesktopwidget.h>
#include <QPainter>
#include <QPaintEvent>

#include "common/XTreeNode.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"

#include "grove/Nodes.h"

#include "qtmmlwidget.h"
#include "mml_utils.h"

#include <iostream>

// *******************************************************************
// Declarations
// *******************************************************************

#define ROUND(a) (int)((a)+.5)

using namespace MmlImpl;

static bool g_draw_frames = false;

typedef QMap<QString, QString> MmlAttributeMap;

class MmlNode;

typedef Common::RefCntPtr<MmlNode> MmlNodePtr;

class MmlDocument : public Mml
{
    public:
	MmlDocument();
	~MmlDocument();
	void clear();

	bool setContent(QString text, QString *errorMsg = 0,
			    int *errorLine = 0, int *errorColumn = 0);
        bool setContent(const GroveLib::Node*, QString* errorMsg = 0);
	void paint(QPainter *p, const QPoint &pos) const;
	void dump() const;
	QSize size();
	void layout();

	QString fontName(MmlRenderer::MmlFont type) const;
	void setFontName(MmlRenderer::MmlFont type, const QString &name);

	int baseFontPointSize() const
	    { return m_base_font_point_size; }
	void setBaseFontPointSize(int size)
	    { m_base_font_point_size = size; }
	QColor foregroundColor() const
	    { return m_foreground_color; }
	void setForegroundColor(const QColor &color)
	    { m_foreground_color = color; }
	QColor backgroundColor() const
	    { return m_background_color; }
	void setBackgroundColor(const QColor &color)
	    { m_background_color = color; }

        MmlNode* rootNode() const { return m_root_node.pointer(); }
        QPaintDevice* device() const { return device_; }
        void          setPaintDevice(QPaintDevice* dev) { device_ = dev; }

    private:
	void _dump(const MmlNode *node, QString &indent) const;
	bool insertChild(MmlNode *parent, MmlNode *new_node, QString *errorMsg);

	MmlNode *domToMml(const QDomNode &, bool *ok, QString *errorMsg);
	MmlNode* groveToMml(const GroveLib::Node*, 
                              bool *ok, QString *errorMsg);
	MmlNode *createNode(NodeType type, const MmlAttributeMap &mml_attr,
				const QString &mml_value, QString *errorMsg);
	MmlNode *createImplicitMrowNode(const QDomNode &dom_node, bool *ok,
				    QString *errorMsg);
	MmlNode* grove_createImplicitMrowNode(const GroveLib::Node*,
                                    bool* ok, QString* errorMsg);

	void insertOperator(MmlNode *node, const QString &text);

        MmlNodePtr m_root_node;

	QString m_normal_font_name;
	QString m_fraktur_font_name;
	QString m_sans_serif_font_name;
	QString m_script_font_name;
	QString m_monospace_font_name;
	QString m_doublestruck_font_name;
	int m_base_font_point_size;
	QColor m_foreground_color;
	QColor m_background_color;
        QPaintDevice* device_;
};

class MmlNode : public Common::RefCounted<>,
                public Common::XTreeNode<MmlNode, 
                       Common::XTreeNodeRefCounted<MmlNode> >,
                public Mml {
    friend class MmlDocument;

    public:
	MmlNode(NodeType type, MmlDocument *document, const MmlAttributeMap &attribute_map);
	virtual ~MmlNode();

	// Mml stuff
	NodeType nodeType() const
	    { return m_node_type; }

	virtual QString toStr() const;

	void setRelOrigin(const QPoint &rel_origin);
	QPoint relOrigin() const { return m_rel_origin; }
	void stretchTo(const QRect &rect);
	bool isStretched() const { return m_stretched; }
	QPoint devicePoint(const QPoint &p) const;

	QRect myRect() const { return m_my_rect; }
	QRect parentRect() const;
	virtual QRect deviceRect() const;
	void updateMyRect();
	virtual void setMyRect(const QRect &rect) { m_my_rect = rect; }

	virtual void stretch();
	virtual void layout();
	virtual void paint(QPainter *p);

	int basePos() const;
	int overlinePos() const;
	int underlinePos() const;
	int em() const;
	int ex() const;

	QString explicitAttribute(const QString &name, 
                                  const QString &def = QString::null) const;
	QString inheritAttributeFromMrow(const QString &name,
                                    const QString &def = QString::null) const;

	virtual QFont font() const;
	virtual QColor color() const;
	virtual QColor background() const;
	virtual int scriptlevel(const MmlNode *child = 0) const;


	// Node stuff
	MmlDocument *document() const { return m_document; }
	bool isLastSibling() const  { return !nextSibling(); }
	bool isFirstSibling() const { return !prevSibling(); }
	bool hasChildNodes() const  { return !!firstChild(); }

    protected:
	virtual void layoutSymbol();
	virtual void paintSymbol(QPainter *p) const;
	virtual QRect symbolRect() const
	    { return QRect(0, 0, 0, 0); }

	MmlNode *parentWithExplicitAttribute(const QString &name,
                                             NodeType type = NoNode);
	int interpretSpacing(const QString &value,
                             bool *ok) const;
    private:
	MmlAttributeMap m_attribute_map;
	bool m_stretched;
	QRect m_my_rect, m_parent_rect;
	QPoint m_rel_origin;

	NodeType m_node_type;
	MmlDocument *m_document;
};

class MmlTokenNode : public MmlNode
{
    public:
	MmlTokenNode(NodeType type, MmlDocument *document,
			const MmlAttributeMap &attribute_map)
	    : MmlNode(type, document, attribute_map) {}

	QString text() const;
};

class MmlMphantomNode : public MmlNode
{
    public:
	MmlMphantomNode(MmlDocument *document,
			    const MmlAttributeMap &attribute_map)
	    : MmlNode(MphantomNode, document, attribute_map) {}

	virtual void paint(QPainter *) {}
};

class MmlUnknownNode : public MmlNode
{
    public:
	MmlUnknownNode(MmlDocument *document,
			    const MmlAttributeMap &attribute_map)
	    : MmlNode(UnknownNode, document, attribute_map) {}
};

class MmlMfencedNode : public MmlNode
{
    public:
	MmlMfencedNode(MmlDocument *document,
			    const MmlAttributeMap &attribute_map)
	    : MmlNode(MfencedNode, document, attribute_map) {}
};

class MmlMalignMarkNode : public MmlNode
{
    public:
	MmlMalignMarkNode(MmlDocument *document)
	    : MmlNode(MalignMarkNode, document, MmlAttributeMap()) {}
};

class MmlMfracNode : public MmlNode
{
    public:
	MmlMfracNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
	    : MmlNode(MfracNode, document, attribute_map) {}

	MmlNode *numerator() const;
	MmlNode *denominator() const;

    protected:
	virtual void layoutSymbol();
	virtual void paintSymbol(QPainter *p) const;
	virtual QRect symbolRect() const;
};

class MmlMrowNode : public MmlNode
{
    public:
	MmlMrowNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		    : MmlNode(MrowNode, document, attribute_map) {}
};

class MmlRootBaseNode : public MmlNode
{
    public:
	MmlRootBaseNode(NodeType type, MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(type, document, attribute_map) {}

	MmlNode *base() const;
	MmlNode *index() const;

	virtual int scriptlevel(const MmlNode *child = 0) const;

    protected:
	virtual void layoutSymbol();
	virtual void paintSymbol(QPainter *p) const;
	virtual QRect symbolRect() const;
	int tailWidth() const;
};

class MmlMrootNode : public MmlRootBaseNode
{
    public:
	MmlMrootNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlRootBaseNode(MrootNode, document, attribute_map) {}
};

class MmlMsqrtNode : public MmlRootBaseNode
{
    public:
	MmlMsqrtNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlRootBaseNode(MsqrtNode, document, attribute_map) {}

};


class MmlTextNode : public MmlNode
{
    public:
	MmlTextNode(const QString &text, MmlDocument *document);

	virtual QString toStr() const;
	QString text() const
	    { return m_text; }

	// TextNodes are not xml elements, so they can't have attributes of
	// their own. Everything is taken from the parent.
	virtual QFont font() const
	    { return parent()->font(); }
	virtual int scriptlevel(const MmlNode* = 0) const
	    { return parent()->scriptlevel(this); }
	virtual QColor color() const
	    { return parent()->color(); }
	virtual QColor background() const
	    { return parent()->background(); }

     protected:
	virtual void paintSymbol(QPainter *p) const;
	virtual QRect symbolRect() const;

	QString m_text;
};

class MmlMiNode : public MmlTokenNode
{
    public:
	MmlMiNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlTokenNode(MiNode, document, attribute_map) {}
};

class MmlMnNode : public MmlTokenNode
{
    public:
	MmlMnNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlTokenNode(MnNode, document, attribute_map) {}
};

class MmlSubsupBaseNode : public MmlNode
{
    public:
	MmlSubsupBaseNode(NodeType type, MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(type, document, attribute_map) {}

	MmlNode *base() const;
	MmlNode *sscript() const;

	virtual int scriptlevel(const MmlNode *child = 0) const;
};

class MmlMsupNode : public MmlSubsupBaseNode
{
    public:
	MmlMsupNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlSubsupBaseNode(MsupNode, document, attribute_map) {}

    protected:
	virtual void layoutSymbol();
};

class MmlMsubNode : public MmlSubsupBaseNode
{
    public:
	MmlMsubNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlSubsupBaseNode(MsubNode, document, attribute_map) {}

    protected:
	virtual void layoutSymbol();
};

class MmlMsubsupNode : public MmlNode
{
    public:
	MmlMsubsupNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MsubsupNode, document, attribute_map) {}

	MmlNode *base() const;
	MmlNode *superscript() const;
	MmlNode *subscript() const;

	virtual int scriptlevel(const MmlNode *child = 0) const;

    protected:
	virtual void layoutSymbol();
};

class MmlMoNode : public MmlTokenNode
{
    public:
	MmlMoNode(MmlDocument *document, const MmlAttributeMap &attribute_map);

	QString dictionaryAttribute(const QString &name) const;
	virtual void stretch();
	virtual int lspace() const;
	virtual int rspace() const;

	virtual QString toStr() const;

    protected:
	virtual void layoutSymbol();
	virtual QRect symbolRect() const;

	virtual FormType form() const;

    private:
	const OperSpec *m_oper_spec;
};

class MmlMstyleNode : public MmlNode
{
    public:
	MmlMstyleNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MstyleNode, document, attribute_map) {}
};

class MmlTableBaseNode : public MmlNode
{
    public:
	MmlTableBaseNode(NodeType type, MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(type, document, attribute_map) {}
};

class MmlMtableNode : public MmlTableBaseNode
{
    public:
	MmlMtableNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlTableBaseNode(MtableNode, document, attribute_map) {}

	int rowspacing() const;
	int columnspacing() const;
	int framespacing_hor() const;
	int framespacing_ver() const;
	FrameType frame() const;
	FrameType columnlines(int idx) const;
	FrameType rowlines(int idx) const;

    protected:
	virtual void layoutSymbol();
	virtual QRect symbolRect() const;
	virtual void paintSymbol(QPainter *p) const;

    private:
	struct CellSizeData
	{
	    void init(const MmlNode *first_row);
	    QList<int> col_widths, row_heights;
	    int numCols() const { return col_widths.count(); }
	    int numRows() const { return row_heights.count(); }
	    uint colWidthSum() const;
	    uint rowHeightSum() const;
	};

	CellSizeData m_cell_size_data;
	int m_content_width, m_content_height;
};

class MmlMtrNode : public MmlTableBaseNode
{
    public:
	MmlMtrNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlTableBaseNode(MtrNode, document, attribute_map) {}
    void layoutCells(const QList<int> &col_widths, int col_spc);
};

class MmlMtdNode : public MmlTableBaseNode
{
    public:
	MmlMtdNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlTableBaseNode(MtdNode, document, attribute_map)
	    { m_scriptlevel_adjust = 0; }
	virtual void setMyRect(const QRect &rect);

	ColAlign columnalign();
	RowAlign rowalign();
	uint colNum();
	uint rowNum();
	virtual int scriptlevel(const MmlNode *child = 0) const;

    private:
	int m_scriptlevel_adjust; // added or subtracted to scriptlevel to
				  // make contents fit the cell
};

class MmlMoverNode : public MmlNode
{
    public:
	MmlMoverNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MoverNode, document, attribute_map) {}
	virtual int scriptlevel(const MmlNode *node = 0) const;

    protected:
	virtual void layoutSymbol();
};

class MmlMunderNode : public MmlNode
{
    public:
	MmlMunderNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MunderNode, document, attribute_map) {}
	virtual int scriptlevel(const MmlNode *node = 0) const;

    protected:
	virtual void layoutSymbol();
};

class MmlMunderoverNode : public MmlNode
{
    public:
	MmlMunderoverNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MunderoverNode, document, attribute_map) {}
	virtual int scriptlevel(const MmlNode *node = 0) const;

    protected:
	virtual void layoutSymbol();
};

class MmlMerrorNode : public MmlNode
{
    public:
	MmlMerrorNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MerrorNode, document, attribute_map) {}
};

class MmlMtextNode : public MmlNode
{
    public:
	MmlMtextNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MtextNode, document, attribute_map) {}
};

class MmlMpaddedNode : public MmlNode
{
    public:
	MmlMpaddedNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MpaddedNode, document, attribute_map) {}

    public:
	int lspace() const;
	int width() const;
	int height() const;
	int depth() const;

    protected:
	int interpretSpacing(QString value, int base_value, bool *ok) const;
	virtual void layoutSymbol();
	virtual QRect symbolRect() const;
};

class MmlMspaceNode : public MmlNode
{
    public:
	MmlMspaceNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
		: MmlNode(MspaceNode, document, attribute_map) {}
};

static const NodeSpec *mmlFindNodeSpec(Mml::NodeType type);
static const NodeSpec *mmlFindNodeSpec(const QString &tag);
static bool mmlCheckChildType(Mml::NodeType parent_type,
			Mml::NodeType child_type, QString *error_str);
static bool mmlCheckAttributes(Mml::NodeType child_type,
			const MmlAttributeMap &attr, QString *error_str);
static QString mmlDictAttribute(const QString &name, const OperSpec *spec);
static const OperSpec *mmlFindOperSpec(const QString &name, Mml::FormType form);
static int interpretSpacing(QString name, int em, int ex, bool *ok,
                            QPaintDevice*);
static int interpretPercentSpacing(QString value, int base, bool *ok);
static uint interpretMathVariant(const QString &value, bool *ok);
static Mml::FormType interpretForm(const QString &value, bool *ok);
static Mml::FrameType interpretFrameType(const QString &value_list, uint idx, bool *ok);
static Mml::FrameSpacing interpretFrameSpacing(const QString &value_list, int em, int ex, bool *ok, QPaintDevice*);
static Mml::ColAlign interpretColAlign(const QString &value_list, uint colnum, bool *ok);
static Mml::RowAlign interpretRowAlign(const QString &value_list, uint rownum, bool *ok);
static Mml::FrameType interpretFrameType(const QString &value_list, uint idx, bool *ok);
static QFont interpretDepreciatedFontAttr(const MmlAttributeMap &font_attr, QFont &fn, int em, int ex, QPaintDevice*);
static QFont interpretMathSize(QString value, QFont &fn, int em, int ex, bool *ok, QPaintDevice*);
static QString interpretListAttr(const QString &value_list, int idx, const QString &def);
static QString rectToStr(const QRect &rect);
static QString entityDeclarations();

// *******************************************************************
// MmlDocument
// *******************************************************************

QString MmlDocument::fontName(MmlRenderer::MmlFont type) const
{
    switch (type) {
	case MmlRenderer::NormalFont:
	    return m_normal_font_name;
	case MmlRenderer::FrakturFont:
	    return m_fraktur_font_name;
	case MmlRenderer::SansSerifFont:
	    return m_sans_serif_font_name;
	case MmlRenderer::ScriptFont:
	    return m_script_font_name;
	case MmlRenderer::MonospaceFont:
	    return m_monospace_font_name;
	case MmlRenderer::DoublestruckFont:
	    return m_doublestruck_font_name;
    };

    return QString::null;
}

void MmlDocument::setFontName(MmlRenderer::MmlFont type, const QString &name)
{
    switch (type) {
	case MmlRenderer::NormalFont:
	    m_normal_font_name = name;
	    break;
	case MmlRenderer::FrakturFont:
	    m_fraktur_font_name = name;
	    break;
	case MmlRenderer::SansSerifFont:
	    m_sans_serif_font_name = name;
	    break;
	case MmlRenderer::ScriptFont:
	    m_script_font_name = name;
	    break;
	case MmlRenderer::MonospaceFont:
	    m_monospace_font_name = name;
	    break;
	case MmlRenderer::DoublestruckFont:
	    m_doublestruck_font_name = name;
	    break;
    };
}

Mml::NodeType domToMmlNodeType(const QDomNode &dom_node)
{
    Mml::NodeType mml_type = Mml::NoNode;

    switch (dom_node.nodeType()) {
	case QDomNode::ElementNode: {
	    QString tag = dom_node.nodeName();
	    const NodeSpec *spec = mmlFindNodeSpec(tag);

	    // treat urecognised tags as mrow
	    if (spec == 0)
		mml_type = Mml::UnknownNode;
	    else
		mml_type = spec->type;

	    break;
	}
	case QDomNode::TextNode:
	    mml_type = Mml::TextNode;
	    break;

	case QDomNode::DocumentNode:
	    mml_type = Mml::MrowNode;
	    break;

	case QDomNode::EntityReferenceNode:
//	    qWarning("EntityReferenceNode: name=\"" + dom_node.nodeName() + "\" value=\"" + dom_node.nodeValue() + "\"");
	    break;

	case QDomNode::AttributeNode:
	case QDomNode::CDATASectionNode:
	case QDomNode::EntityNode:
	case QDomNode::ProcessingInstructionNode:
	case QDomNode::CommentNode:
	case QDomNode::DocumentTypeNode:
	case QDomNode::DocumentFragmentNode:
	case QDomNode::NotationNode:
	case QDomNode::BaseNode:
	case QDomNode::CharacterDataNode:
	    break;
    }

    return mml_type;
}

Mml::NodeType groveToMmlNodeType(const GroveLib::Node* node)
{
    using namespace GroveLib;
    Mml::NodeType mml_type = Mml::NoNode;

    switch (node->nodeType()) {
	case Node::ELEMENT_NODE: {
	    QString tag = CONST_ELEMENT_CAST(node)->localName();
	    const NodeSpec* spec = mmlFindNodeSpec(tag);

	    // treat urecognised tags as mrow
	    if (spec == 0) {
                if (tag == "math")
                    mml_type = Mml::MrowNode;
                else
		    mml_type = Mml::UnknownNode;
            }
	    else
		mml_type = spec->type;

	    break;
	}
        case Node::TEXT_NODE: 
	    mml_type = Mml::TextNode;
	    break;

        case Node::DOCUMENT_NODE:
	    mml_type = Mml::MrowNode;
	    break;
    
        default:
            break;
    }
    return mml_type;
}


MmlDocument::MmlDocument()
    : device_(qApp->desktop())
{
    // Some defaults which happen to work on my computer,
    // but probably won't work on other's
#if defined(Q_WS_WIN)
    m_normal_font_name = "Times New Roman";
#else
    m_normal_font_name = "Century Schoolbook L";
#endif
    m_fraktur_font_name = "Fraktur";
    m_sans_serif_font_name = "Luxi Sans";
    m_script_font_name = "Urw Chancery L";
    m_monospace_font_name = "Luxi Mono";
    m_doublestruck_font_name = "Doublestruck";
    m_base_font_point_size = 16;
    m_foreground_color = Qt::black;
    m_background_color = Qt::white;
}

MmlDocument::~MmlDocument()
{
    clear();
}

void MmlDocument::clear()
{
    m_root_node = 0;
}

void MmlDocument::dump() const
{
    if (m_root_node == 0)
    	return;

    QString indent;
    _dump(m_root_node.pointer(), indent);
}

void MmlDocument::_dump(const MmlNode *node, QString &indent) const
{
    if (node == 0) return;

    qWarning((indent + node->toStr()).local8Bit().data());

    indent += "  ";
    const MmlNode *child = node->firstChild();
    for (; child != 0; child = child->nextSibling())
	_dump(child, indent);
    indent.truncate(indent.length() - 2);
}

/////////////////////////////////////////////////////////////////////////

bool MmlDocument::setContent(const GroveLib::Node* node, QString* errorMsg)
{
    clear();
    bool ok;
    MmlNodePtr root_node = groveToMml(node, &ok, errorMsg);
    if (!ok)
	return false;
    if (!root_node) {
	if (errorMsg != 0)
	    *errorMsg = "empty document";
	return false;
    }
    insertChild(0, root_node.pointer(), 0);
    layout();
    return true;
}

/////////////////////////////////////////////////////////////////////////

bool MmlDocument::setContent(QString text, QString *errorMsg,
				    int *errorLine, int *errorColumn)
{
    clear();

    QString prefix = "<?xml version=\"2.0\"?>\n";
    prefix.append(entityDeclarations());

    uint prefix_lines = 0;
    for (int i = 0; i < prefix.length(); ++i) {
	if (prefix.at(i) == '\n')
	    ++prefix_lines;
    }

    QDomDocument dom;
    if (!dom.setContent(prefix + text, false, errorMsg, errorLine, errorColumn)) {
	if (errorLine != 0)
	    *errorLine -= prefix_lines;
	return false;
    }

    // we don't have access to line info from now on
    if (errorLine != 0) *errorLine = -1;
    if (errorColumn != 0) *errorColumn = -1;

    bool ok;
    MmlNode *root_node = domToMml(dom, &ok, errorMsg);
    if (!ok)
	return false;

    if (root_node == 0) {
	if (errorMsg != 0)
	    *errorMsg = "empty document";
	return false;
    }

    insertChild(0, root_node, 0);
    layout();

/*    QFile of("/tmp/dump.xml");
    of.open(IO_WriteOnly);
    QTextStream os(&of);
    os.setEncoding(QTextStream::UnicodeUTF8);
    os << dom.toString(); */

    return true;
}

void MmlDocument::layout()
{
    if (!m_root_node)
    	return;

    m_root_node->layout();
    m_root_node->stretch();
//    dump();
}

bool MmlDocument::insertChild(MmlNode *parent, MmlNode *new_node,
				QString *errorMsg)
{
    if (new_node == 0)
	return true;

    Q_ASSERT(new_node->parent() == 0);

    if (parent) {
	if (!mmlCheckChildType(parent->nodeType(), 
                               new_node->nodeType(), errorMsg))
	    return false;
    }
    if (!parent) {
	if (!m_root_node)
	    m_root_node = new_node;
	else 
            m_root_node->appendChild(new_node);
    }
    else 
        parent->appendChild(new_node);
    return true;
}

MmlNode *MmlDocument::createNode(NodeType type,
				    const MmlAttributeMap &mml_attr,
				    const QString &mml_value,
				    QString *errorMsg)
{
    Q_ASSERT(type != NoNode);

    MmlNode *mml_node = 0;

    if (!mmlCheckAttributes(type, mml_attr, errorMsg))
	return 0;

    switch (type) {
	case MiNode:
	    mml_node = new MmlMiNode(this, mml_attr);
	    break;
	case MnNode:
	    mml_node = new MmlMnNode(this, mml_attr);
	    break;
	case MfracNode:
	    mml_node = new MmlMfracNode(this, mml_attr);
	    break;
	case MrowNode:
	    mml_node = new MmlMrowNode(this, mml_attr);
	    break;
	case MsqrtNode:
	    mml_node = new MmlMsqrtNode(this, mml_attr);
	    break;
	case MrootNode:
	    mml_node = new MmlMrootNode(this, mml_attr);
	    break;
	case MsupNode:
	    mml_node = new MmlMsupNode(this, mml_attr);
	    break;
	case MsubNode:
	    mml_node = new MmlMsubNode(this, mml_attr);
	    break;
	case MsubsupNode:
	    mml_node = new MmlMsubsupNode(this, mml_attr);
	    break;
	case MoNode:
	    mml_node = new MmlMoNode(this, mml_attr);
	    break;
	case MstyleNode:
	    mml_node = new MmlMstyleNode(this, mml_attr);
	    break;
	case TextNode:
	    mml_node = new MmlTextNode(mml_value, this);
	    break;
	case MphantomNode:
	    mml_node = new MmlMphantomNode(this, mml_attr);
	    break;
	case MfencedNode:
	    mml_node = new MmlMfencedNode(this, mml_attr);
	    break;
	case MtableNode:
	    mml_node = new MmlMtableNode(this, mml_attr);
	    break;
	case MtrNode:
	    mml_node = new MmlMtrNode(this, mml_attr);
	    break;
	case MtdNode:
	    mml_node = new MmlMtdNode(this, mml_attr);
	    break;
	case MoverNode:
	    mml_node = new MmlMoverNode(this, mml_attr);
	    break;
	case MunderNode:
	    mml_node = new MmlMunderNode(this, mml_attr);
	    break;
	case MunderoverNode:
	    mml_node = new MmlMunderoverNode(this, mml_attr);
	    break;
	case MalignMarkNode:
	    mml_node = new MmlMalignMarkNode(this);
	    break;
	case MerrorNode:
	    mml_node = new MmlMerrorNode(this, mml_attr);
	    break;
	case MtextNode:
	    mml_node = new MmlMtextNode(this, mml_attr);
	    break;
	case MpaddedNode:
	    mml_node = new MmlMpaddedNode(this, mml_attr);
	    break;
	case MspaceNode:
	    mml_node = new MmlMspaceNode(this, mml_attr);
	    break;
	case UnknownNode:
	    mml_node = new MmlUnknownNode(this, mml_attr);
	    break;
	case NoNode:
	    mml_node = 0;
	    break;
    }

    return mml_node;
}

void MmlDocument::insertOperator(MmlNode *node, const QString &text)
{
    MmlNode *text_node = createNode(TextNode, MmlAttributeMap(), text, 0);
    MmlNode *mo_node = createNode(MoNode, MmlAttributeMap(), QString::null, 0);

    bool ok = insertChild(node, mo_node, 0);
    Q_ASSERT( ok );
    ok = insertChild(mo_node, text_node, 0);
    Q_ASSERT( ok );
}

static bool is_empty_text(const GroveLib::Node* node)
{
    return (node->nodeType() == GroveLib::Node::TEXT_NODE &&
        CONST_TEXT_CAST(node)->data().simplifyWhiteSpace().isEmpty());
}

static int child_count(const GroveLib::Node* node)
{
    int child_cnt = 0;
    const GroveLib::Node* child = node->firstChild();
    for (; child; child = child->nextSibling())
        if (!is_empty_text(child))
            child_cnt++;
    return child_cnt;
}

MmlNode* MmlDocument::groveToMml(const GroveLib::Node* node, 
                                 bool* ok, QString* errorMsg)
{
    Q_ASSERT(ok);
    NodeType mml_type = groveToMmlNodeType(node);
    if (mml_type == NoNode) {
        *ok = true;
        return 0;
    }
    MmlAttributeMap mml_attr;
    if (node->nodeType() == GroveLib::Node::ELEMENT_NODE) {
        const GroveLib::Element* elem = CONST_ELEMENT_CAST(node);
        const GroveLib::Attr* a = elem->attrs().firstChild();
        for (; a; a = a->nextSibling()) 
            mml_attr[a->localName()] = a->value();
    }
    QString mml_value;
    if (mml_type == TextNode) 
        mml_value = CONST_TEXT_CAST(node)->data();
    Common::OwnerPtr<MmlNode> mml_node(createNode(mml_type, 
        mml_attr, mml_value, errorMsg));
    if (!mml_node) {
	*ok = false;
	return 0;
    }
    // create the node's children according to the child_spec

    const NodeSpec* spec = mmlFindNodeSpec(mml_type);
    const GroveLib::Node* child = node->firstChild();
    int child_cnt = child_count(node);
    MmlNodePtr mml_child;

    QString separator_list;
    if (mml_type == MfencedNode)
	separator_list = mml_node->explicitAttribute("separators", ",");

    switch (spec->child_spec) {
	case NodeSpec::ChildIgnore:
	    break;

	case NodeSpec::ImplicitMrow:

	    if (child_cnt) {
		mml_child = grove_createImplicitMrowNode(node, ok, errorMsg);
		if (!*ok) 
		    return 0;
		if (!insertChild(mml_node.pointer(), 
                                 mml_child.pointer(), errorMsg)) {
		    *ok = false;
		    return 0;
		}
	    }
	    break;

	default:
	    // exact ammount of children specified - check...
	    if (spec->child_spec != child_cnt &&
                !(!child_cnt && mml_type == MmlNode::MtextNode)) {
		if (errorMsg != 0)
		    *errorMsg = QString("element ")
			+ spec->tag
			+ " requires exactly "
			+ QString::number(spec->child_spec)
			+ " arguments, got "
			+ QString::number(child_cnt);
		*ok = false;
		return 0;
	}
	// ...and continue just as in ChildAny

	case NodeSpec::ChildAny:
	    if (mml_type == MfencedNode)
		insertOperator(mml_node.pointer(),
                               mml_node->explicitAttribute("open", "("));
            int i = 0;
            for (; child; child = child->nextSibling()) {
                if (is_empty_text(child))                            
                    continue;
		mml_child = groveToMml(child, ok, errorMsg);
		if (!*ok)
                    return 0;
		if (mml_type == MtableNode && 
                    mml_child->nodeType() != MtrNode) {
		    MmlNodePtr mtr_node = createNode(MtrNode,
                        MmlAttributeMap(), QString::null, 0);
		    insertChild(mml_node.pointer(), mtr_node.pointer(), 0);
		    if (!insertChild(mtr_node.pointer(),
                                     mml_child.pointer(), errorMsg)) {
			*ok = false;
			return 0;
		    }
		}
		else if (mml_type == MtrNode &&
                         mml_child->nodeType() != MtdNode) {
		    MmlNodePtr mtd_node = createNode(MtdNode, 
                        MmlAttributeMap(), QString::null, 0);
		    insertChild(mml_node.pointer(), mtd_node.pointer(), 0);
		    if (!insertChild(mtd_node.pointer(), 
                                     mml_child.pointer(), errorMsg)) {
			*ok = false;
			return 0;
		    }
		}
		else {
		    if (!insertChild(mml_node.pointer(),
                            mml_child.pointer(), errorMsg)) {
			*ok = false;
			return 0;
		    }
		}
		if (i < child_cnt - 1 && 
                    mml_type == MfencedNode && !separator_list.isEmpty()) {
		    QChar separator;
		    if (i >= (int)separator_list.length())
			separator = 
                            separator_list.at(separator_list.length() - 1);
		    else
			separator = separator_list[i];
		    insertOperator(mml_node.pointer(), QString(separator));
		}
                i++;
	    }
	    if (mml_type == MfencedNode)
		insertOperator(mml_node.pointer(),
                               mml_node->explicitAttribute("close", ")"));
	    break;
    }
    *ok = true;
    return mml_node.release();
}

MmlNode* MmlDocument::grove_createImplicitMrowNode(const GroveLib::Node* node,
                                                     bool* ok,
	                                             QString* errorMsg)
{
    const GroveLib::Node* child = node->firstChild();
    int child_cnt = child_count(node);

    if (child_cnt == 0) {
	*ok = true;
	return 0;
    }
    Common::OwnerPtr<MmlNode> mml_node;
    if (child_cnt == 1) {
        while (child && is_empty_text(child))
            child = child->nextSibling();
        if (0 == child) {
            *ok = true;
            return 0;
        }
	return groveToMml(child, ok, errorMsg);
    }
    mml_node = createNode(MrowNode, MmlAttributeMap(),
			  QString::null, errorMsg);
    for (; child; child = child->nextSibling()) {
        if (is_empty_text(child))
            continue;
	MmlNodePtr mml_child = groveToMml(child, ok, errorMsg);
	if (!*ok) 
	    return 0;
	if (!insertChild(mml_node.pointer(), mml_child.pointer(), errorMsg)) {
	    *ok = false;
	    return 0;
	}
    }
    return mml_node.release();
}

MmlNode *MmlDocument::domToMml(const QDomNode &dom_node, bool *ok, QString *errorMsg)
{
    // create the node

    Q_ASSERT(ok != 0);

    NodeType mml_type = domToMmlNodeType(dom_node);

    if (mml_type == NoNode) {
	*ok = true;
	return 0;
    }

    QDomNamedNodeMap dom_attr = dom_node.attributes();
    MmlAttributeMap mml_attr;
    for (int i = 0; i < dom_attr.count(); ++i) {
	QDomNode attr_node = dom_attr.item(i);
	Q_ASSERT(!attr_node.nodeName().isNull());
	Q_ASSERT(!attr_node.nodeValue().isNull());
	mml_attr[attr_node.nodeName()] = attr_node.nodeValue();
    }

    QString mml_value;
    if (mml_type == TextNode)
	mml_value = dom_node.nodeValue();
    MmlNode *mml_node = createNode(mml_type, mml_attr, mml_value, errorMsg);
    if (mml_node == 0) {
	*ok = false;
	return 0;
    }

    // create the node's children according to the child_spec

    const NodeSpec *spec = mmlFindNodeSpec(mml_type);
    QDomNodeList dom_child_list = dom_node.childNodes();
    int child_cnt = dom_child_list.count();
    MmlNode *mml_child = 0;

    QString separator_list;
    if (mml_type == MfencedNode)
	separator_list = mml_node->explicitAttribute("separators", ",");

    switch (spec->child_spec) {
	case NodeSpec::ChildIgnore:
	    break;

	case NodeSpec::ImplicitMrow:

	    if (child_cnt > 0) {
		mml_child = createImplicitMrowNode(dom_node, ok, errorMsg);
		if (!*ok) {
		    delete mml_node;
		    return 0;
		}

		if (!insertChild(mml_node, mml_child, errorMsg)) {
		    delete mml_node;
		    delete mml_child;
		    *ok = false;
		    return 0;
		}
	    }

	    break;

	default:
	    // exact ammount of children specified - check...
	    if (spec->child_spec != child_cnt) {
		if (errorMsg != 0)
		    *errorMsg = QString("element ")
			+ spec->tag
			+ " requires exactly "
			+ QString::number(spec->child_spec)
			+ " arguments, got "
			+ QString::number(child_cnt);
		delete mml_node;
		*ok = false;
		return 0;
	}

	// ...and continue just as in ChildAny

	case NodeSpec::ChildAny:
	    if (mml_type == MfencedNode)
		insertOperator(mml_node, mml_node->explicitAttribute("open", "("));

	    for (int i = 0; i < child_cnt; ++i) {
		QDomNode dom_child = dom_child_list.item(i);

		MmlNode *mml_child = domToMml(dom_child, ok, errorMsg);
		if (!*ok) {
		    delete mml_node;
		    return 0;
		}

		if (mml_type == MtableNode && mml_child->nodeType() != MtrNode) {
		    MmlNode *mtr_node = createNode(MtrNode, MmlAttributeMap(), QString::null, 0);
		    insertChild(mml_node, mtr_node, 0);
		    if (!insertChild(mtr_node, mml_child, errorMsg)) {
			delete mml_node;
			delete mml_child;
			*ok = false;
			return 0;
		    }
		}
		else if (mml_type == MtrNode && mml_child->nodeType() != MtdNode) {
		    MmlNode *mtd_node = createNode(MtdNode, MmlAttributeMap(), QString::null, 0);
		    insertChild(mml_node, mtd_node, 0);
		    if (!insertChild(mtd_node, mml_child, errorMsg)) {
			delete mml_node;
			delete mml_child;
			*ok = false;
			return 0;
		    }
		}
		else {
		    if (!insertChild(mml_node, mml_child, errorMsg)) {
			delete mml_node;
			delete mml_child;
			*ok = false;
			return 0;
		    }
		}

		if (i < child_cnt - 1 && mml_type == MfencedNode && !separator_list.isEmpty()) {
		    QChar separator;
		    if (i >= (int)separator_list.length())
			separator = separator_list.at(separator_list.length() - 1);
		    else
			separator = separator_list[i];
		    insertOperator(mml_node, QString(separator));
		}
	    }

	    if (mml_type == MfencedNode)
		insertOperator(mml_node, mml_node->explicitAttribute("close", ")"));

	    break;
    }

    *ok = true;
    return mml_node;
}

MmlNode *MmlDocument::createImplicitMrowNode(const QDomNode &dom_node, bool *ok,
						QString *errorMsg)
{
    QDomNodeList dom_child_list = dom_node.childNodes();
    int child_cnt = dom_child_list.count();

    if (child_cnt == 0) {
	*ok = true;
	return 0;
    }

    if (child_cnt == 1)
	return domToMml(dom_child_list.item(0), ok, errorMsg);

    MmlNode *mml_node = createNode(MrowNode, MmlAttributeMap(),
				    QString::null, errorMsg);
    Q_ASSERT(mml_node != 0); // there is no reason in heaven or hell for this to fail

    for (int i = 0; i < child_cnt; ++i) {
	QDomNode dom_child = dom_child_list.item(i);

	MmlNode *mml_child = domToMml(dom_child, ok, errorMsg);
	if (!*ok) {
	    delete mml_node;
	    return 0;
	}

	if (!insertChild(mml_node, mml_child, errorMsg)) {
	    delete mml_node;
	    delete mml_child;
	    *ok = false;
	    return 0;
	}
    }

    return mml_node;
}

void MmlDocument::paint(QPainter *p, const QPoint &pos) const
{
    if (m_root_node == 0)
    	return;

/*    p->save();
    p->setPen(Qt::blue);
    p->drawLine(pos.x() - 5, pos.y(), pos.x() + 5, pos.y());
    p->drawLine(pos.x(), pos.y() - 5, pos.x(), pos.y() + 5);
    p->restore(); */

    QRect mr = m_root_node->myRect();
    m_root_node->setRelOrigin(pos - mr.topLeft());
    m_root_node->paint(p);
}

QSize MmlDocument::size()
{
    if (m_root_node == 0)
    	return QSize(0, 0);
    return m_root_node->deviceRect().size();
}

// *******************************************************************
// MmlNode
// *******************************************************************


MmlNode::MmlNode(NodeType type, MmlDocument *document, const MmlAttributeMap &attribute_map)
{
    m_node_type = type;
    m_document = document;
    m_attribute_map = attribute_map;

    m_my_rect = m_parent_rect = QRect(0, 0, 0, 0);
    m_rel_origin = QPoint(0, 0);
    m_stretched = false;
}

MmlNode::~MmlNode()
{
   // MmlNode *n = firstChild();
   // while (n != 0) {
   //     MmlNode *tmp = n->nextSibling();
   //     delete n;
   //     n = tmp;
   // }
}

static QString rectToStr(const QRect &rect)
{
    return QString("[(%1, %2), %3x%4]")
		.arg(rect.x())
		.arg(rect.y())
		.arg(rect.width())
		.arg(rect.height());
}

QString MmlNode::toStr() const
{
    const NodeSpec *spec = mmlFindNodeSpec(nodeType());
    Q_ASSERT(spec != 0);

    return QString("%1 %2 mr=%3 pr=%4 dr=%5 ro=(%7, %8) str=%9")
		.arg(spec->type_str)
		.arg((unsigned long)this, 0, 16)
		.arg(rectToStr(myRect()))
		.arg(rectToStr(parentRect()))
		.arg(rectToStr(deviceRect()))
		.arg(m_rel_origin.x())
		.arg(m_rel_origin.y())
		.arg((int)isStretched());
}

int MmlNode::interpretSpacing(const QString &value, bool *ok) const
{
    return ::interpretSpacing(value, em(), ex(), ok, document()->device());
}

int MmlNode::basePos() const
{
    QFontMetrics fm(font());
    return fm.strikeOutPos();
}

int MmlNode::underlinePos() const
{
    QFontMetrics fm(font());
    return basePos() + fm.underlinePos();
}
int MmlNode::overlinePos() const
{
    QFontMetrics fm(font());
    return basePos() - fm.overlinePos();
}

int MmlNode::em() const
{
    return QFontMetrics(font()).boundingRect('m').width();
}

int MmlNode::ex() const
{
    return QFontMetrics(font()).boundingRect('x').height();
}

int MmlNode::scriptlevel(const MmlNode *) const
{
    int parent_sl;
    const MmlNode *p = parent();
    if (p == 0)
	parent_sl = 0;
    else
	parent_sl = p->scriptlevel(this);

    QString expl_sl_str = explicitAttribute("scriptlevel");
    if (expl_sl_str.isNull())
	return parent_sl;

    if (expl_sl_str.startsWith("+") || expl_sl_str.startsWith("-")) {
	bool ok;
	int expl_sl = expl_sl_str.toInt(&ok);
	if (ok) {
	    return parent_sl + expl_sl;
	}
	else {
	    qWarning(("MmlNode::scriptlevel(): bad value " + expl_sl_str).toLatin1().data());
	    return parent_sl;
	}
    }

    bool ok;
    int expl_sl = expl_sl_str.toInt(&ok);
    if (ok)
	return expl_sl;


    if (expl_sl_str == "+")
	return parent_sl + 1;
    else if (expl_sl_str == "-")
	return parent_sl - 1;
    else {
	qWarning(("MmlNode::scriptlevel(): could not parse value: \"" + expl_sl_str + "\"").toLatin1().data());
	return parent_sl;
    }
}

QPoint MmlNode::devicePoint(const QPoint &p) const
{
    QRect mr = myRect();
    QRect dr = deviceRect();

    if (isStretched())
	return dr.topLeft() + QPoint((p.x() - mr.left())*dr.width()/mr.width(),
					(p.y() - mr.top())*dr.height()/mr.height());
    else
	return dr.topLeft() + p - mr.topLeft();
}

QString MmlNode::inheritAttributeFromMrow(const QString &name,
					    const QString &def) const
{
    const MmlNode *p = this;
    for (; p != 0; p = p->parent()) {
	if (p == this || p->nodeType() == MstyleNode) {
	    QString value = p->explicitAttribute(name);
	    if (!value.isNull())
		return value;
	}
    }

    return def;
}

QColor MmlNode::color() const
{
    // If we are child of <merror> return red
    const MmlNode *p = this;
    for (; p != 0; p = p->parent()) {
	if (p->nodeType() == MerrorNode)
	    return QColor("red");
    }

    QString value_str = inheritAttributeFromMrow("mathcolor");
    if (value_str.isNull())
	value_str = inheritAttributeFromMrow("color");
    if (value_str.isNull())
	return QColor();

    return QColor(value_str);
}

QColor MmlNode::background() const
{
    QString value_str = inheritAttributeFromMrow("mathbackground");
    if (value_str.isNull())
	value_str = inheritAttributeFromMrow("background");
    if (value_str.isNull())
	return QColor();

    return QColor(value_str);
}

static void updateFontAttr(MmlAttributeMap &font_attr, const MmlNode *n,
				const QString &name, const QString &preferred_name = QString::null)
{
    if (font_attr.contains(preferred_name) || font_attr.contains(name))
	return;
    QString value = n->explicitAttribute(name);
    if (!value.isNull())
	font_attr[name] = value;
}

static MmlAttributeMap collectFontAttributes(const MmlNode *node)
{
    MmlAttributeMap font_attr;

    for (const MmlNode *n = node; n != 0; n = n->parent()) {
	if (n == node || n->nodeType() == Mml::MstyleNode) {
	    updateFontAttr(font_attr, n, "mathvariant");
	    updateFontAttr(font_attr, n, "mathsize");

	    // depreciated attributes
	    updateFontAttr(font_attr, n, "fontsize", "mathsize");
	    updateFontAttr(font_attr, n, "fontweight", "mathvariant");
	    updateFontAttr(font_attr, n, "fontstyle", "mathvariant");
	    updateFontAttr(font_attr, n, "fontfamily", "mathvariant");
	}
    }

    return font_attr;
}

QFont MmlNode::font() const
{
    QFont fn(document()->fontName(MmlRenderer::NormalFont), 
             document()->device());
    int ps = document()->baseFontPointSize();
    int sl = scriptlevel();
    if (sl >= 0) {
	for (int i = 0; i < sl; ++i)
	    ps = (int)(ps*g_script_size_multiplier);
    }
    else {
	for (int i = 0; i > sl; --i)
	    ps = (int)(ps/g_script_size_multiplier);
    }
    if (ps < g_min_font_point_size)
	ps = g_min_font_point_size;
    fn.setPointSize(ps);

    int em = QFontMetrics(fn).boundingRect('m').width();
    int ex = QFontMetrics(fn).boundingRect('x').height();

    MmlAttributeMap font_attr = collectFontAttributes(this);

    if (font_attr.contains("mathvariant")) {
	QString value = font_attr["mathvariant"];

	bool ok;
	uint mv = interpretMathVariant(value, &ok);

	if (ok) {
	    if (mv & ScriptMV)
		fn.setFamily(document()->fontName(MmlRenderer::ScriptFont));

	    if (mv & FrakturMV)
		fn.setFamily(document()->fontName(MmlRenderer::FrakturFont));

	    if (mv & SansSerifMV)
		fn.setFamily(document()->fontName(MmlRenderer::SansSerifFont));

	    if (mv & MonospaceMV)
		fn.setFamily(document()->fontName(MmlRenderer::MonospaceFont));

	    if (mv & DoubleStruckMV)
		fn.setFamily(document()->fontName(MmlRenderer::DoublestruckFont));

	    if (mv & BoldMV)
		fn.setBold(true);

	    if (mv & ItalicMV)
		fn.setItalic(true);
	}
    }

    if (font_attr.contains("mathsize")) {
	QString value = font_attr["mathsize"];
	fn = interpretMathSize(value, fn, em, ex, 0, document()->device());
    }

    fn = interpretDepreciatedFontAttr(font_attr, fn, em, ex, 
        document()->device());

    if (nodeType() == MiNode
	    && !font_attr.contains("mathvariant")
	    && !font_attr.contains("fontstyle")) {
	const MmlMiNode *mi_node = (const MmlMiNode*) this;
	if (mi_node->text().length() == 1)
	    fn.setItalic(true);
    }

    if (nodeType() == MoNode) {
	fn.setItalic(false);
	fn.setBold(false);
    }

    return fn;
}

QString MmlNode::explicitAttribute(const QString &name, const QString &def) const
{
    MmlAttributeMap::const_iterator it = m_attribute_map.find(name);
    if (it != m_attribute_map.end())
	return *it;
    return def;
}


QRect MmlNode::parentRect() const
{
    if (isStretched())
	return m_parent_rect;

    QRect mr = myRect();
    QPoint ro = relOrigin();

    return QRect(ro + mr.topLeft(), mr.size());
}


void MmlNode::stretchTo(const QRect &rect)
{
    m_parent_rect = rect;
    m_stretched = true;
}

void MmlNode::setRelOrigin(const QPoint &rel_origin)
{
    m_rel_origin = rel_origin + QPoint(-myRect().left(), 0);
    m_stretched = false;
}

void MmlNode::updateMyRect()
{
    m_my_rect = symbolRect();
    MmlNode *child = firstChild();
    for (; child != 0; child = child->nextSibling())
	m_my_rect |= child->parentRect();
}

void MmlNode::layout()
{
    m_parent_rect = QRect(0, 0, 0, 0);
    m_stretched = false;
    m_rel_origin = QPoint(0, 0);

    MmlNode *child = firstChild();
    for (; child != 0; child = child->nextSibling())
	child->layout();

    layoutSymbol();

    updateMyRect();

    if (parent() == 0)
	m_rel_origin = QPoint(0, 0);
}


QRect MmlNode::deviceRect() const
{
    if (parent() == 0)
	return QRect(relOrigin() + myRect().topLeft(), myRect().size());

/*    if (!isStretched()) {
	QRect pdr = parent()->deviceRect();
	QRect pmr = parent()->myRect();
	QRect pr = parentRect();
	QRect mr = myRect();
	return QRect(pdr.left() + pr.left() - pmr.left(),
			pdr.top()  + pr.top() - pmr.top(),
			mr.width(), mr.height());
    }
*/
    QRect pdr = parent()->deviceRect();
    QRect pr = parentRect();
    QRect pmr = parent()->myRect();

    float scale_w = 0;
    if (pmr.width() != 0)
	scale_w = (float)pdr.width()/pmr.width();
    float scale_h = 0;
    if (pmr.height() != 0)
	scale_h = (float)pdr.height()/pmr.height();

    return QRect(pdr.left() + ROUND((pr.left() - pmr.left())*scale_w),
		     pdr.top()  + ROUND((pr.top() - pmr.top())*scale_h),
		     ROUND((pr.width()*scale_w)),
		     ROUND((pr.height()*scale_h)));
}

void MmlNode::layoutSymbol()
{
    // default behaves like an mrow

    // now lay them out in a neat row, aligning their origins to my origin
    int w = 0;
    MmlNode *child = firstChild();
    for (; child != 0; child = child->nextSibling()) {
	child->setRelOrigin(QPoint(w, 0));
	w += child->parentRect().width() + 1;
    }
}

void MmlNode::paint(QPainter *p)
{
    p->save();
    QRect dr(p->xForm(deviceRect()));
    QRect mr(p->xForm(myRect()));
    p->setViewTransformEnabled(false);
    p->setViewport(dr);
    p->setWindow(mr);
    QColor fg = color();
    QColor bg = background();
    if (bg.isValid())
	p->fillRect(myRect(), bg);
    if (fg.isValid())
	p->setPen(color());

    MmlNode *child = firstChild();
    for (; child != 0; child = child->nextSibling())
	child->paint(p);

    paintSymbol(p);

    p->restore();
}

void MmlNode::paintSymbol(QPainter *p) const
{
    if (g_draw_frames && myRect().isValid()) {
	p->save();
	p->setPen(Qt::red);
	p->drawRect(m_my_rect);
	QPen pen = p->pen();
	pen.setStyle(Qt::DotLine);
	p->setPen(pen);
	p->drawLine(myRect().left(), 0, myRect().right(), 0);
	p->restore();
    }
}

void MmlNode::stretch()
{
    MmlNode *child = firstChild();
    for (; child != 0; child = child->nextSibling())
	child->stretch();
}

QString MmlTokenNode::text() const
{
    QString result;

    const MmlNode *child = firstChild();
    for (; child != 0; child = child->nextSibling()) {
	if (child->nodeType() != TextNode) continue;
	if (!result.isEmpty())
	    result += ' ';
	result += ((MmlTextNode*)child)->text();
    }

    return result;
}

MmlNode *MmlMfracNode::numerator() const
{
    MmlNode *node = firstChild();
    Q_ASSERT(node != 0);
    return node;
}

MmlNode *MmlMfracNode::denominator() const
{
    MmlNode *node = numerator()->nextSibling();
    Q_ASSERT(node != 0);
    return node;
}

QRect MmlMfracNode::symbolRect() const
{
    int num_width = numerator()->myRect().width();
    int denom_width = denominator()->myRect().width();
    int my_width = qMax(num_width, denom_width) + 4;

    return QRect(-my_width/2, 0, my_width, 1);
}

void MmlMfracNode::layoutSymbol()
{
    MmlNode *num = numerator();
    MmlNode *denom = denominator();

    QRect num_rect = num->myRect();
    QRect denom_rect = denom->myRect();

    int spacing = (int)(g_mfrac_spacing*(num_rect.height() + denom_rect.height()));

    num->setRelOrigin(QPoint(-num_rect.width()/2, - spacing - num_rect.bottom()));
    denom->setRelOrigin(QPoint(-denom_rect.width()/2, spacing - denom_rect.top()));
}

static bool zeroLineThickness(const QString &s)
{
    if (s.length() == 0 || !s[0].isDigit())
	return false;

    for (int i = 0; i < s.length(); ++i) {
	QChar c = s.at(i);
	if (c.isDigit() && c != '0')
	    return false;
    }
    return true;
}

void MmlMfracNode::paintSymbol(QPainter *p) const
{
    QString linethickness_str = inheritAttributeFromMrow("linethickness", "1");

    /* InterpretSpacing returns an int, which might be 0 even if the thickness
       is > 0, though very very small. That's ok, because the painter then paints
       a line of thickness 1. However, we have to run this check if the line
       thickness really is zero */
    if (!zeroLineThickness(linethickness_str)) {
	bool ok;
	int linethickness = interpretSpacing(linethickness_str, &ok);
	if (!ok)
	    linethickness = 1;

	p->save();
	QPen pen = p->pen();
	pen.setWidth(linethickness);
	p->setPen(pen);
	QSize s = myRect().size();
	p->drawLine(-s.width()/2, 0, s.width()/2, 0);
	p->restore();
    }
}

MmlNode *MmlRootBaseNode::base() const
{
    MmlNode *node = firstChild();
//    Q_ASSERT(node != 0);
    return node;
}

MmlNode *MmlRootBaseNode::index() const
{
    MmlNode *b = base();
    if (b == 0)
	return 0;
    return b->nextSibling();
}

int MmlRootBaseNode::scriptlevel(const MmlNode *child) const
{
    int sl = MmlNode::scriptlevel();

    MmlNode *i = index();
    if (child != 0 && child == i)
	return sl + 1;
    else
	return sl;
}


QRect MmlRootBaseNode::symbolRect() const
{
    MmlNode *b = base();
    QRect base_rect;
    if (b == 0)
	base_rect = QRect(0, 0, 1, 1);
    else
	base_rect = base()->myRect();

    int margin = (int)(g_mroot_base_margin*base_rect.height());
    int tw = tailWidth();

    return QRect(-tw, base_rect.top() - margin, tw,
		    base_rect.height() + 2*margin);
}

int MmlRootBaseNode::tailWidth() const
{
    QFontMetrics fm(font());
    return fm.boundingRect(g_radical_char).width();
}

void MmlRootBaseNode::layoutSymbol()
{
    MmlNode *b = base();
    QSize base_size;
    if (b != 0) {
	b->setRelOrigin(QPoint(0, 0));
	base_size = base()->myRect().size();
    } else
	base_size = QSize(1, 1);

    MmlNode *i = index();
    if (i != 0) {
	int tw = tailWidth();

	QRect i_rect = i->myRect();
	i->setRelOrigin(QPoint(-tw/2 - i_rect.width(),
				-i_rect.bottom() - 4));
    }
}

void MmlRootBaseNode::paintSymbol(QPainter *p) const
{
    QFont fn = font();

    p->save();

    QRectF sr(symbolRect());
    QRectF mr(myRect());
    p->setFont(fn);
    QFontMetricsF fm(fn);
    QRectF br = fm.boundingRect(g_radical_char);
    qreal scale_x = qreal(sr.width()) / br.width();
    qreal scale_y = qreal(sr.height()) / br.height();
    p->scale(scale_x, scale_y);
    qreal ypos = myRect().y() + myRect().height() - br.height()/2;
    p->drawText(QPointF(myRect().x(), ypos), QString(g_radical_char));
    p->restore();

    p->save();
    QPen pen2(p->pen());
    pen2.setWidth(1);
    p->setPen(pen2);
    p->drawLine(QPointF(sr.right() - 2, sr.top() + 1),
                QPointF(myRect().right(), sr.top() + 1));
    p->restore();
}

MmlTextNode::MmlTextNode(const QString &text, MmlDocument *document)
    : MmlNode(TextNode, document, MmlAttributeMap())
{
    m_text = text.trimmed();
    if (m_text == QString(QChar(0x62, 0x20))	     // &InvisibleTimes;
	    || m_text == QString(QChar(0x63, 0x20))  // &InvisibleComma;
	    || m_text == QString(QChar(0x61, 0x20))) // &ApplyFunction;
	m_text = "";
}

QString MmlTextNode::toStr() const
{
    return MmlNode::toStr() + ", text=\"" + m_text + "\"";
}

void MmlTextNode::paintSymbol(QPainter *p) const
{
    MmlNode::paintSymbol(p);

    QFont fn = font();

    QFontInfo fi(fn);
//    qWarning("MmlTextNode::paintSymbol(): requested: %s, used: %s, size=%d, italic=%d, bold=%d, text=\"%s\" sl=%d",
//    	    	fn.family().latin1(), fi.family().latin1(), fi.pointSize(), (int)fi.italic(), (int)fi.bold(), m_text.latin1(), scriptlevel());

    QFontMetrics fm(fn);

    p->save();
    p->setFont(fn);

    p->drawText(0, fm.strikeOutPos(), m_text);
    p->restore();
}

QRect MmlTextNode::symbolRect() const
{
    QFontMetrics fm(font());

    QRect br = fm.boundingRect(m_text);
    br.translate(0, fm.strikeOutPos());

    return br;
}

MmlNode *MmlSubsupBaseNode::base() const
{
    MmlNode *b = firstChild();
    Q_ASSERT(b != 0);
    return b;
}

MmlNode *MmlSubsupBaseNode::sscript() const
{
    MmlNode *s = base()->nextSibling();
    Q_ASSERT(s != 0);
    return s;
}

int MmlSubsupBaseNode::scriptlevel(const MmlNode *child) const
{
    int sl = MmlNode::scriptlevel();

    MmlNode *s = sscript();
    if (child != 0 && child == s)
	return sl + 1;
    else
	return sl;
}

void MmlMsupNode::layoutSymbol()
{
    MmlNode *b = base();
    MmlNode *s = sscript();

    b->setRelOrigin(QPoint(-b->myRect().width(), 0));
    s->setRelOrigin(QPoint(0, b->myRect().top()));
}

void MmlMsubNode::layoutSymbol()
{
    MmlNode *b = base();
    MmlNode *s = sscript();

    b->setRelOrigin(QPoint(-b->myRect().width(), 0));
    s->setRelOrigin(QPoint(0, b->myRect().bottom()));
}

MmlNode *MmlMsubsupNode::base() const
{
    MmlNode *b = firstChild();
    Q_ASSERT(b != 0);
    return b;
}

MmlNode *MmlMsubsupNode::subscript() const
{
    MmlNode *sub = base()->nextSibling();
    Q_ASSERT(sub != 0);
    return sub;
}

MmlNode *MmlMsubsupNode::superscript() const
{
    MmlNode *sup = subscript()->nextSibling();
    Q_ASSERT(sup != 0);
    return sup;
}

void MmlMsubsupNode::layoutSymbol()
{
    MmlNode *b = base();
    MmlNode *sub = subscript();
    MmlNode *sup = superscript();

    b->setRelOrigin(QPoint(-b->myRect().width(), 0));
    sub->setRelOrigin(QPoint(0, b->myRect().bottom()));
    sup->setRelOrigin(QPoint(0, b->myRect().top()));
}

int MmlMsubsupNode::scriptlevel(const MmlNode *child) const
{
    int sl = MmlNode::scriptlevel();

    MmlNode *sub = subscript();
    MmlNode *sup = superscript();

    if (child != 0 && (child == sup || child == sub))
	return sl + 1;
    else
	return sl;
}

QString MmlMoNode::toStr() const
{
    return MmlNode::toStr() + QString(" form=%1").arg((int)form());
}

void MmlMoNode::layoutSymbol()
{
    MmlNode *child = firstChild();
    if (child == 0)
	return;

    child->setRelOrigin(QPoint(0, 0));

    if (m_oper_spec == 0)
	m_oper_spec = mmlFindOperSpec(text(), form());
}

MmlMoNode::MmlMoNode(MmlDocument *document, const MmlAttributeMap &attribute_map)
    : MmlTokenNode(MoNode, document, attribute_map)
{
    m_oper_spec = 0;
}

QString MmlMoNode::dictionaryAttribute(const QString &name) const
{
    const MmlNode *p = this;
    for (; p != 0; p = p->parent()) {
	if (p == this || p->nodeType() == MstyleNode) {
	    QString expl_attr = p->explicitAttribute(name);
	    if (!expl_attr.isNull())
		return expl_attr;
	}
    }

    return mmlDictAttribute(name, m_oper_spec);
}

Mml::FormType MmlMoNode::form() const
{
    QString value_str = inheritAttributeFromMrow("form");
    if (!value_str.isNull()) {
	bool ok;
	FormType value = interpretForm(value_str, &ok);
	if (ok)
	    return value;
	else
	    qWarning("Could not convert %s to form", value_str.toLatin1().data());

    }

    // Default heuristic.
    if (firstChild() == (MmlNode*)this && lastChild() != (MmlNode*)this)
	return PrefixForm;
    else if (lastChild() == (MmlNode*)this && firstChild() != (MmlNode*)this)
	return PostfixForm;
    else return InfixForm;

}

void MmlMoNode::stretch()
{
    if (parent() == 0)
	return;

    if (m_oper_spec == 0)
	return;

    if (m_oper_spec->stretch_dir == OperSpec::HStretch
    	    && parent()->nodeType() == MrowNode
	    && (nextSibling() != 0 || prevSibling() != 0))
	return;

    QRect pmr = parent()->myRect();
    QRect pr = parentRect();

    switch (m_oper_spec->stretch_dir) {
	case OperSpec::VStretch:
	    stretchTo(QRect(pr.left(), pmr.top(), pr.width(), pmr.height()));
	    break;
	case OperSpec::HStretch:
	    stretchTo(QRect(pmr.left(), pr.top(), pmr.width(), pr.height()));
	    break;
	case OperSpec::HVStretch:
	    stretchTo(pmr);
	    break;
	case OperSpec::NoStretch:
	    break;
    }
}

int MmlMoNode::lspace() const
{
    Q_ASSERT(m_oper_spec != 0);
    if (parent() == 0
	    || (parent()->nodeType() != MrowNode  
	    	    && parent()->nodeType() != MfencedNode
		    && parent()->nodeType() != UnknownNode)
	    || (prevSibling() == 0 && nextSibling() == 0))
	return 0;
    else
	return interpretSpacing(dictionaryAttribute("lspace"), 0);
}

int MmlMoNode::rspace() const
{
    Q_ASSERT(m_oper_spec != 0);
    if (parent() == 0
	    || (parent()->nodeType() != MrowNode 
	    	    && parent()->nodeType() != MfencedNode
		    && parent()->nodeType() != UnknownNode)
	    || (prevSibling() == 0 && nextSibling() == 0))
	return 0;
    else
	return interpretSpacing(dictionaryAttribute("rspace"), 0);
}

QRect MmlMoNode::symbolRect() const
{
    const MmlNode *child = firstChild();

    if (child == 0)
	return QRect(0, 0, 0, 0);

    QRect cmr = child->myRect();

    return QRect(-lspace(), cmr.top(),
		   cmr.width() + lspace() + rspace(), cmr.height());
}

int MmlMtableNode::rowspacing() const
{
    QString value = explicitAttribute("rowspacing");
    if (value.isNull())
	return ex();
    bool ok;
    int r = interpretSpacing(value, &ok);

    if (ok)
	return r;
    else
	return ex();
}

int MmlMtableNode::columnspacing() const
{
    QString value = explicitAttribute("columnspacing");
    if (value.isNull())
	return (int)(0.8*em());
    bool ok;
    int r = interpretSpacing(value, &ok);

    if (ok)
	return r;
    else
	return (int)(0.8*em());
}

uint MmlMtableNode::CellSizeData::colWidthSum() const
{
    uint w = 0;
    for (int i = 0; i < col_widths.count(); ++i)
	w += col_widths[i];
    return w;
}

uint MmlMtableNode::CellSizeData::rowHeightSum() const
{
    uint h = 0;
    for (int i = 0; i < row_heights.count(); ++i)
	h += row_heights[i];
    return h;
}

void MmlMtableNode::CellSizeData::init(const MmlNode *first_row)
{
    col_widths.clear();
    row_heights.clear();

    const MmlNode *mtr = first_row;
    for (; mtr != 0; mtr = mtr->nextSibling()) {

	Q_ASSERT(mtr->nodeType() == MtrNode);

	int col_cnt = 0;
	const MmlNode *mtd = mtr->firstChild();
	for (; mtd != 0; mtd = mtd->nextSibling(), ++col_cnt) {

	    Q_ASSERT(mtd->nodeType() == MtdNode);

	    QRect mtdmr = mtd->myRect();

	    if (col_cnt == col_widths.count())
		col_widths.append(mtdmr.width());
	    else
		col_widths[col_cnt] = qMax(col_widths[col_cnt], mtdmr.width());
	}

	row_heights.append(mtr->myRect().height());
    }
}

void MmlMtableNode::layoutSymbol()
{
    // Obtain natural widths of columns
    m_cell_size_data.init(firstChild());

    int col_spc = columnspacing();
    int row_spc = rowspacing();
    int frame_spc_hor = framespacing_hor();
    QString columnwidth_attr = explicitAttribute("columnwidth", "auto");

    // Is table width set by user? If so, set col_width_sum and never ever change it.
    int col_width_sum = m_cell_size_data.colWidthSum();
    bool width_set_by_user = false;
    QString width_str = explicitAttribute("width", "auto");
    if (width_str != "auto") {
	bool ok;

	int w = interpretSpacing(width_str, &ok);
	if (ok) {
	    col_width_sum = w
			    - col_spc*(m_cell_size_data.numCols() - 1)
			    - frame_spc_hor*2;
	    width_set_by_user = true;
	}
    }

    // Find out what kind of columns we are dealing with and set the widths of
    // statically sized columns.
    int fixed_width_sum = 0;	      // sum of widths of statically sized set columns
    int auto_width_sum = 0; 	      // sum of natural widths of auto sized columns
    int relative_width_sum = 0;       // sum of natural widths of relatively sized columns
    double relative_fraction_sum = 0; // total fraction of width taken by relatively
				      // sized columns
    int i;
    for (i = 0; i < m_cell_size_data.numCols(); ++i) {
	QString value = interpretListAttr(columnwidth_attr, i, "auto");

	// Is it an auto sized column?
	if (value == "auto" || value == "fit") {
	    auto_width_sum += m_cell_size_data.col_widths[i];
	    continue;
	}

	// Is it a statically sized column?
	bool ok;
	int w = interpretSpacing(value, &ok);
	if (ok) {
	    // Yup, sets its width to the user specified value
	    m_cell_size_data.col_widths[i] = w;
	    fixed_width_sum += w;
	    continue;
	}

	// Is it a relatively sized column?
	if (value.endsWith("%")) {
	    value.truncate(value.length() - 1);
	    double factor = value.toFloat(&ok);
	    if (ok && !value.isEmpty()) {
		factor /= 100.0;
		relative_width_sum += m_cell_size_data.col_widths[i];
		relative_fraction_sum += factor;
		if (!width_set_by_user) {
		    // If the table width was not set by the user, we are free to increase
		    // it so that the width of this column will be >= than its natural width
		    int min_col_width_sum = ROUND(m_cell_size_data.col_widths[i]/factor);
		    if (min_col_width_sum > col_width_sum)
			col_width_sum = min_col_width_sum;
		}
		continue;
	    }
	    else
		qWarning("MmlMtableNode::layoutSymbol(): could not parse value %s%%", value.toLatin1().data());
	}

	// Relatively sized column, but we failed to parse the factor. Treat is like an auto
	// column.
	auto_width_sum += m_cell_size_data.col_widths[i];
    }

    // Work out how much space remains for the auto olumns, after allocating
    // the statically sized and the relatively sized columns.
    int required_auto_width_sum = col_width_sum
				    - ROUND(relative_fraction_sum*col_width_sum)
				    - fixed_width_sum;

    if (!width_set_by_user && required_auto_width_sum < auto_width_sum) {
	if (relative_fraction_sum < 1)
	    col_width_sum = ROUND((fixed_width_sum + auto_width_sum)/(1 - relative_fraction_sum));
	else
	    col_width_sum = fixed_width_sum + auto_width_sum + relative_width_sum;
	required_auto_width_sum = auto_width_sum;
    }

    // Ratio by which we have to shring/grow all auto sized columns to make it all fit
    double auto_width_scale = 1;
    if (auto_width_sum > 0)
	auto_width_scale = (float)required_auto_width_sum/auto_width_sum;

    // Set correct sizes for the auto sized and the relatively sized columns.
    for (i = 0; i < m_cell_size_data.numCols(); ++i) {
	QString value = interpretListAttr(columnwidth_attr, i, "auto");

	// Is it a relatively sized column?
	if (value.endsWith("%")) {
	    bool ok;
	    int w = interpretPercentSpacing(value, col_width_sum, &ok);
	    if (ok)
		m_cell_size_data.col_widths[i] = w;
	    else
		// We're treating parsing errors here as auto sized columns
		m_cell_size_data.col_widths[i]
			= ROUND(auto_width_scale*m_cell_size_data.col_widths[i]);
	}
	// Is it an auto sized column?
	else if (value == "auto") {
	    m_cell_size_data.col_widths[i]
		    = ROUND(auto_width_scale*m_cell_size_data.col_widths[i]);
	}
    }

    QString s;
    QList<int> &col_widths = m_cell_size_data.col_widths;
    for (i = 0; i < col_widths.count(); ++i) {
	s += QString("[w=%1 %2%%]")
		.arg(col_widths[i])
		.arg(100*col_widths[i]/m_cell_size_data.colWidthSum());
    }
//    qWarning(s);

    m_content_width = m_cell_size_data.colWidthSum()
		    + col_spc*(m_cell_size_data.numCols() - 1);
    m_content_height = m_cell_size_data.rowHeightSum()
		    + row_spc*(m_cell_size_data.numRows() - 1);

    int bottom = -m_content_height/2;
    MmlNode *child = firstChild();
    for (; child != 0; child = child->nextSibling()) {
	Q_ASSERT(child->nodeType() == MtrNode);
	MmlMtrNode *row = (MmlMtrNode*) child;

	row->layoutCells(m_cell_size_data.col_widths, col_spc);
	QRect rmr = row->myRect();
	row->setRelOrigin(QPoint(0, bottom - rmr.top()));
	bottom += rmr.height() + row_spc;
    }
}

QRect MmlMtableNode::symbolRect() const
{
    int frame_spc_hor = framespacing_hor();
    int frame_spc_ver = framespacing_ver();

    return QRect(-frame_spc_hor,
		    -m_content_height/2 - frame_spc_ver,
		    m_content_width + 2*frame_spc_hor,
		    m_content_height + 2*frame_spc_ver);
}

Mml::FrameType MmlMtableNode::frame() const
{
    QString value = explicitAttribute("frame", "none");
    return interpretFrameType(value, 0, 0);
}

Mml::FrameType MmlMtableNode::columnlines(int idx) const
{
    QString value = explicitAttribute("columnlines", "none");
    return interpretFrameType(value, idx, 0);
}

Mml::FrameType MmlMtableNode::rowlines(int idx) const
{
    QString value = explicitAttribute("rowlines", "none");
    return interpretFrameType(value, idx, 0);
}

void MmlMtableNode::paintSymbol(QPainter *p) const
{
    FrameType f = frame();
    if (f != FrameNone) {
	p->save();

	QPen pen = p->pen();
	if (f == FrameDashed)
	    pen.setStyle(Qt::DashLine);
	else
	    pen.setStyle(Qt::SolidLine);
	p->setPen(pen);
	p->drawRect(myRect());

	p->restore();
    }

    p->save();

    int col_spc = columnspacing();
    int row_spc = rowspacing();

    QPen pen = p->pen();
    int col_offset = 0;
    int i;
    for (i = 0; i < m_cell_size_data.numCols() - 1; ++i) {
	FrameType f = columnlines(i);
	col_offset += m_cell_size_data.col_widths[i];

	if (f != FrameNone) {
	    if (f == FrameDashed)
		pen.setStyle(Qt::DashLine);
	    else if (f == FrameSolid)
		pen.setStyle(Qt::SolidLine);

	    p->setPen(pen);
	    int x = col_offset + col_spc/2;
	    p->drawLine(x, -m_content_height/2, x, m_content_height/2 );
	}
	col_offset += col_spc;
    }

    int row_offset = 0;
    for (i = 0; i < m_cell_size_data.numRows() - 1; ++i) {
	FrameType f = rowlines(i);
	row_offset += m_cell_size_data.row_heights[i];

	if (f != FrameNone) {
	    if (f == FrameDashed)
		pen.setStyle(Qt::DashLine);
	    else if (f == FrameSolid)
		pen.setStyle(Qt::SolidLine);

	    p->setPen(pen);
	    int y = row_offset + row_spc/2 - m_content_height/2;
	    p->drawLine(0, y, m_content_width, y);
	}
	row_offset += row_spc;
    }

    p->restore();
}

int MmlMtableNode::framespacing_ver() const
{
    if (frame() == FrameNone)
	return (int)(0.2*em());

    QString value = explicitAttribute("framespacing", "0.4em 0.5ex");

    bool ok;
    FrameSpacing fs = interpretFrameSpacing(value, em(), ex(), &ok,
        document()->device());
    if (ok)
	return fs.m_ver;
    else
	return (int)(0.5*ex());
}

int MmlMtableNode::framespacing_hor() const
{
    if (frame() == FrameNone)
	return (int)(0.2*em());

    QString value = explicitAttribute("framespacing", "0.4em 0.5ex");

    bool ok;
    FrameSpacing fs = interpretFrameSpacing(value, em(), ex(), &ok,
        document()->device());
    if (ok)
	return fs.m_hor;
    else
	return (int)(0.4*em());
}

void MmlMtrNode::layoutCells(const QList<int> &col_widths,
				int col_spc)
{
    QRect mr = myRect();

    MmlNode *child = firstChild();
    int col_offset = 0;
    uint colnum = 0;
    for (; child != 0; child = child->nextSibling(), ++colnum) {
	Q_ASSERT(child->nodeType() == MtdNode);
	MmlMtdNode *mtd = (MmlMtdNode*) child;

	QRect r = QRect(0, mr.top(), col_widths[colnum], mr.height());
	mtd->setMyRect(r);
	mtd->setRelOrigin(QPoint(col_offset, 0));
	col_offset += col_widths[colnum] + col_spc;
    }

    updateMyRect();
}

int MmlMtdNode::scriptlevel(const MmlNode *child) const
{
    int sl = MmlNode::scriptlevel();
    if (child != 0 && child == firstChild())
	return sl + m_scriptlevel_adjust;
    else
	return sl;
}

void MmlMtdNode::setMyRect(const QRect &rect)
{
    MmlNode::setMyRect(rect);
    MmlNode *child = firstChild();
    if (child == 0)
	return;

    if (rect.width() < child->myRect().width()) {
	while (rect.width() < child->myRect().width()
		    && child->font().pointSize() > g_min_font_point_size) {

//    	    qWarning("MmlMtdNode::setMyRect(): rect.width()=%d, child()->myRect().width=%d sl=%d",
//	    		rect.width(), child->myRect().width(), m_scriptlevel_adjust);

	    ++m_scriptlevel_adjust;
	    child->layout();
	}

//    	qWarning("MmlMtdNode::setMyRect(): rect.width()=%d, child()->myRect().width=%d sl=%d",
//	    	    rect.width(), child->myRect().width(), m_scriptlevel_adjust);
    }

    QRect mr = myRect();
    QRect cmr = child->myRect();

    QPoint child_rel_origin;

    switch (columnalign()) {
	case ColAlignLeft:
	    child_rel_origin.setX(0);
	    break;
	case ColAlignCenter:
	    child_rel_origin.setX(mr.left() + (mr.width() - cmr.width())/2);
	    break;
	case ColAlignRight:
	    child_rel_origin.setX(mr.right() - cmr.width());
	    break;
    }

    switch (rowalign()) {
	case RowAlignTop:
	    child_rel_origin.setY(mr.top() - cmr.top());
	    break;
	case RowAlignCenter:
	case RowAlignBaseline:
	    child_rel_origin.setY(mr.top() -cmr.top() + (mr.height() - cmr.height())/2);
	    break;
	case RowAlignBottom:
	    child_rel_origin.setY(mr.bottom() - cmr.bottom());
	    break;
	case RowAlignAxis:
	    child_rel_origin.setY(0);
	    break;
    }

    child->setRelOrigin(child_rel_origin);
}

uint MmlMtdNode::colNum()
{
    MmlNode *syb = prevSibling();

    uint i = 0;
    for (; syb != 0; syb = syb->prevSibling())
	++i;

    return i;
}

uint MmlMtdNode::rowNum()
{
    MmlNode *row = parent()->prevSibling();

    uint i = 0;
    for (; row != 0; row = row->prevSibling())
	++i;

    return i;
}

MmlMtdNode::ColAlign MmlMtdNode::columnalign()
{
    QString val = explicitAttribute("columnalign");
    if (!val.isNull())
	return interpretColAlign(val, 0, 0);

    MmlNode *node = parent(); // <mtr>
    if (node == 0)
	return ColAlignCenter;

    uint colnum = colNum();
    val = node->explicitAttribute("columnalign");
    if (!val.isNull())
	return interpretColAlign(val, colnum, 0);

    node = node->parent(); // <mtable>
    if (node == 0)
	return ColAlignCenter;

    val = node->explicitAttribute("columnalign");
    if (!val.isNull())
	return interpretColAlign(val, colnum, 0);

    return ColAlignCenter;
}

MmlMtdNode::RowAlign MmlMtdNode::rowalign()
{
    QString val = explicitAttribute("rowalign");
    if (!val.isNull())
	return interpretRowAlign(val, 0, 0);

    MmlNode *node = parent(); // <mtr>
    if (node == 0)
	return RowAlignAxis;

    uint rownum = rowNum();
    val = node->explicitAttribute("rowalign");
    if (!val.isNull())
	return interpretRowAlign(val, rownum, 0);

    node = node->parent(); // <mtable>
    if (node == 0)
	return RowAlignAxis;

    val = node->explicitAttribute("rowalign");
    if (!val.isNull())
	return interpretRowAlign(val, rownum, 0);

    return RowAlignAxis;
}

void MmlMoverNode::layoutSymbol()
{
    MmlNode *base = firstChild();
    Q_ASSERT(base != 0);
    MmlNode *over = base->nextSibling();
    Q_ASSERT(over != 0);

    QRect base_rect = base->myRect();
    QRect over_rect = over->myRect();

    int spacing = (int)(g_mfrac_spacing*(over_rect.height()
					    + base_rect.height()));

    base->setRelOrigin(QPoint(-base_rect.width()/2, 0));
    over->setRelOrigin(QPoint(-over_rect.width()/2,
				base_rect.top() - spacing - over_rect.bottom()));
}

int MmlMoverNode::scriptlevel(const MmlNode *node) const
{
    MmlNode *base = firstChild();
    Q_ASSERT(base != 0);
    MmlNode *over = base->nextSibling();
    Q_ASSERT(over != 0);

    int sl = MmlNode::scriptlevel();
    if (node != 0 && node == over)
	return sl + 1;
    else
	return sl;
}

void MmlMunderNode::layoutSymbol()
{
    MmlNode *base = firstChild();
    Q_ASSERT(base != 0);
    MmlNode *under = base->nextSibling();
    Q_ASSERT(under != 0);

    QRect base_rect = base->myRect();
    QRect under_rect = under->myRect();

    int spacing = (int)(g_mfrac_spacing*(under_rect.height() + base_rect.height()));

    base->setRelOrigin(QPoint(-base_rect.width()/2, 0));
    under->setRelOrigin(QPoint(-under_rect.width()/2, base_rect.bottom() + spacing - under_rect.top()));
}

int MmlMunderNode::scriptlevel(const MmlNode *node) const
{
    MmlNode *base = firstChild();
    Q_ASSERT(base != 0);
    MmlNode *under = base->nextSibling();
    Q_ASSERT(under != 0);

    int sl = MmlNode::scriptlevel();
    if (node != 0 && node == under)
	return sl + 1;
    else
	return sl;
}

void MmlMunderoverNode::layoutSymbol()
{
    MmlNode *base = firstChild();
    Q_ASSERT(base != 0);
    MmlNode *under = base->nextSibling();
    Q_ASSERT(under != 0);
    MmlNode *over = under->nextSibling();
    Q_ASSERT(over != 0);

    QRect base_rect = base->myRect();
    QRect under_rect = under->myRect();
    QRect over_rect = over->myRect();

    int spacing = (int)(g_mfrac_spacing*(   base_rect.height()
						+ under_rect.height()
						+ over_rect.height())	);

    base->setRelOrigin(QPoint(-base_rect.width()/2, 0));
    under->setRelOrigin(QPoint(-under_rect.width()/2, base_rect.bottom() + spacing - under_rect.top()));
    over->setRelOrigin(QPoint(-over_rect.width()/2, base_rect.top() - spacing - under_rect.bottom()));
}

int MmlMunderoverNode::scriptlevel(const MmlNode *node) const
{
    MmlNode *base = firstChild();
    Q_ASSERT(base != 0);
    MmlNode *under = base->nextSibling();
    Q_ASSERT(under != 0);
    MmlNode *over = under->nextSibling();
    Q_ASSERT(over != 0);

    int sl = MmlNode::scriptlevel();
    if (node != 0 && (node == under || node == over))
	return sl + 1;
    else
	return sl;
}

int MmlMpaddedNode::interpretSpacing(QString value, int base_value, bool *ok) const
{
    if (ok != 0)
	*ok = false;

    value.replace(' ', "");

    QString sign, factor_str, pseudo_unit;
    bool percent = false;

    // extract the sign
    int idx = 0;
    if (idx < value.length() && (value.at(idx) == '+' || value.at(idx) == '-'))
	sign = value.at(idx++);

    // extract the factor
    while (idx < value.length() && (value.at(idx).isDigit() || value.at(idx) == '.'))
	factor_str.append(value.at(idx++));

    // extract the % sign
    if (idx < value.length() && value.at(idx) == '%') {
	percent = true;
	++idx;
    }

    // extract the pseudo-unit
    pseudo_unit = value.mid(idx);

    bool float_ok;
    double factor = factor_str.toFloat(&float_ok);
    if (!float_ok || factor < 0) {
	qWarning("MmlMpaddedNode::interpretSpacing(): could not parse \"%s\"", value.toLatin1().data());
	return 0;
    }

    if (percent)
	factor /= 100.0;

    QRect cr;
    if (firstChild() == 0)
	cr = QRect(0, 0, 0, 0);
    else
	cr = firstChild()->myRect();

    int unit_size;

    if (pseudo_unit.isEmpty())
	unit_size = base_value;
    else if (pseudo_unit == "width")
	unit_size = cr.width();
    else if (pseudo_unit == "height")
	unit_size = -cr.top();
    else if (pseudo_unit == "depth")
	unit_size = cr.bottom();
    else {
	bool unit_ok;
	unit_size = MmlNode::interpretSpacing("1" + pseudo_unit, &unit_ok);
	if (!unit_ok) {
	    qWarning("MmlMpaddedNode::interpretSpacing(): could not parse \"%s\"", value.toLatin1().data());
	    return 0;
	}
    }

    if (ok != 0)
	*ok = true;

    if (sign.isNull())
	return (int)(factor*unit_size);
    else if (sign == "+")
	return base_value + (int)(factor*unit_size);
    else // sign == "-"
	return base_value - (int)(factor*unit_size);
}

int MmlMpaddedNode::lspace() const
{
    QString value = explicitAttribute("lspace");

    if (value.isNull())
	return 0;

    bool ok;
    int lspace = interpretSpacing(value, 0, &ok);

    if (ok)
	return lspace;

    return 0;
}

int MmlMpaddedNode::width() const
{
    int child_width = 0;
    if (firstChild() != 0)
	child_width = firstChild()->myRect().width();

    QString value = explicitAttribute("width");
    if (value.isNull())
	return child_width;

    bool ok;
    int w = interpretSpacing(value, child_width, &ok);
    if (ok)
	return w;

    return child_width;
}

int MmlMpaddedNode::height() const
{
    QRect cr;
    if (firstChild() == 0)
	cr = QRect(0, 0, 0, 0);
    else
	cr = firstChild()->myRect();

    QString value = explicitAttribute("height");
    if (value.isNull())
	return -cr.top();

    bool ok;
    int h = interpretSpacing(value, -cr.top(), &ok);
    if (ok)
	return h;

    return -cr.top();
}

int MmlMpaddedNode::depth() const
{
    QRect cr;
    if (firstChild() == 0)
	cr = QRect(0, 0, 0, 0);
    else
	cr = firstChild()->myRect();

    QString value = explicitAttribute("depth");
    if (value.isNull())
	return cr.bottom();

    bool ok;
    int h = interpretSpacing(value, cr.bottom(), &ok);
    if (ok)
	return h;

    return cr.bottom();
}

void MmlMpaddedNode::layoutSymbol()
{
    MmlNode *child = firstChild();
    if (child == 0)
	return;

    child->setRelOrigin(QPoint(0, 0));
}


QRect MmlMpaddedNode::symbolRect() const
{
    return QRect(-lspace(), -height(), lspace() + width(), height() + depth());
}

// *******************************************************************
// MmlRenderer
// *******************************************************************

/*!
    \class MmlRenderer

    \brief The MmlRenderer class renders mathematical formulas written in MathML 2.0.

    MmlRenderer implements the Presentation Markup subset of the
    MathML 2.0 specification, with a few \link overview.html exceptions\endlink.

    \code
    MmlRenderer *mmlWidget = new MmlRenderer(this);
    QString errorMsg;
    int errorLine;
    int errorColumn;
    bool ok = mmlWidget->setContent(mmlText, &errorMsg, &errorLine, &errorColumn);
    if (!ok) {
	qWarning("MathML error: %s, Line: %d, Column: %d",
		 errorMsg.latin1(), errorLine, errorColumn);
    }
    \endcode

*/


/*!
    \enum MmlRenderer::MmlFont

    This ennumerated type is used in fontName() and setFontName() to
    specify a font type.

    \value NormalFont	The default font type, used to render
    expressions for which no mathvariant or fontfamily is specified,
    or for which the "normal" mathvariant is specified.

    \value FrakturFont	The font type used to render expressions for
    which the "fraktur" mathvariant is specified.

    \value SansSerifFont    The font type used to render expressions
    for which the "sans-serif" mathvariant is specified.

    \value ScriptFont	    The font type used to render expressions
    for which the "script" mathvariant is specified.

    \value MonospaceFont    The font type used to render expressions
    for which the "monospace" mathvariant is specified.

    \value DoublestruckFont The font type used to render expressions
    for which the "doublestruck" mathvariant is specified.

    \sa setFontName() fontName() setBaseFontPointSize() baseFontPointSize()
*/

/*!
    Constructs a MmlRenderer object. The \a parent
    parameter is passed to QFrame's constructor.
*/

MmlRenderer::MmlRenderer()
{
    m_doc = new MmlDocument;
}

/*!
    Destructs a MmlRenderer object.
*/

MmlRenderer::~MmlRenderer()
{
    delete m_doc;
}

/*!
    Returns the name of the font used to render the font \a type.

    \sa setFontName()  setBaseFontPointSize() baseFontPointSize() MmlRenderer::MmlFont
*/

QString MmlRenderer::fontName(MmlFont type) const
{
    return m_doc->fontName(type);
}

/*!
    Sets the name of the font used to render the font \a type to \a name.

    \sa fontName() setBaseFontPointSize() baseFontPointSize() MmlRenderer::MmlFont
*/

void MmlRenderer::setFontName(MmlFont type, const QString &name)
{
    m_doc->setFontName(type, name);
    m_doc->layout();
    update_view();
}

/*!
    If \a b is true, draws a red bounding rectangle around each
    expression; if \a b is false, no such rectangle is drawn.
    This is mostly useful for debugging MathML expressions.

    \sa drawFrames()
*/

void MmlRenderer::setDrawFrames(bool b)
{
    g_draw_frames = b;
    update_view();
}

/*!
    Returns true if each expression should be drawn with a red
    bounding rectangle; otherwise returns false.
    This is mostly useful for debugging MathML expressions.

    \sa setDrawFrames()
*/

bool MmlRenderer::drawFrames() const
{
    return g_draw_frames;
}

/*!
    Clears the contents of this widget.
*/
void MmlRenderer::clear()
{
    m_doc->clear();
}

/*!
    Returns the point size of the font used to render expressions
    whose scriptlevel is 0.

    \sa setBaseFontPointSize() fontName() setFontName()
*/

int MmlRenderer::baseFontPointSize() const
{
    return m_doc->baseFontPointSize();
}

/*!
    Sets the point \a size of the font used to render expressions
    whose scriptlevel is 0.

    \sa baseFontPointSize() fontName() setFontName()
*/

void MmlRenderer::setBaseFontPointSize(int size)
{
    if (size < g_min_font_point_size)
	return;

    m_doc->setBaseFontPointSize(size);
    m_doc->layout();
    update_view();
}

/*!
    Returns the size of the formula in pixels.
*/

QSize MmlRenderer::mmlSizeHint() const
{
    QSize size = m_doc->size();
    if (size == QSize(0, 0))
    	return QSize(100, 50);
    return m_doc->size();
}

void MmlRenderer::stretchTo(const QRect& rect)
{
    if (m_doc->rootNode())
        m_doc->rootNode()->stretchTo(rect);
}

/*!
    Sets the MathML expression to be rendered. The expression is given
    in the string \a text. If the expression is sucesfully parsed,
    this method returns true; otherwise it returns false. If an error
    occured \a errorMsg is set to a diagnostic message, while \a
    errorLine and \a errorColumn contain the location of the error.
    Any of \a errorMsg, \a errorLine and \a errorColumn may be 0,
    in which case they are not set.
    
    \a text should contain MathML 2.0 presentation markup elements enclosed 
    in a <math> element.
*/

bool MmlRenderer::setContent(const QString &text, QString *errorMsg,
				int *errorLine, int *errorColumn)
{
    bool result = m_doc->setContent(text, errorMsg, errorLine, errorColumn);
    if (result)
	update_view();
    return result;
}

bool MmlRenderer::setContent(const GroveLib::Node* node, QString* errorMsg)
{
    bool result = m_doc->setContent(node, errorMsg);
    if (result)
	update_view();
    return result; 
}

void MmlRenderer::paintMml(QPainter* p, const QPoint& point)
{
    m_doc->paint(p, point);
}

void MmlRenderer::layoutMml()
{
    m_doc->layout();
}

void MmlRenderer::setPaintDevice(QPaintDevice* dev)
{
    m_doc->setPaintDevice(dev);
}

/*! \internal */

void QtMmlWidget::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    QPainter p(this);
    if (e->rect().intersects(contentsRect()))
        p.setClipRegion(e->region().intersect(contentsRect()));
    QSize s = m_doc->size();
    int x = (width() - s.width())/2;
    int y = (height() - s.height())/2;
    paintMml(&p, QPoint(x, y));
}

/*! \internal */

void MmlRenderer::dump() const
{
    m_doc->dump();
}

// *******************************************************************
// Static helper functions
// *******************************************************************

static QString entityDeclarations()
{
    QString result = "<!DOCTYPE math [\n";

    const EntitySpec *ent = g_xml_entity_data;
    for (; ent->name != 0; ++ent) {
	result += "\t<!ENTITY " + QString(ent->name) + " \"" + ent->value + "\">\n";
    }

    result += "]>\n";

    return result;
}

static int interpretSpacing(QString value, int em, int ex, bool *ok,
                            QPaintDevice* device)
{
    if (ok != 0)
	*ok = true;

    if (value == "thin")
	return 1;

    if (value == "medium")
	return 2;

    if (value == "thick")
	return 3;

    struct HSpacingValue {
	const char *name;
	float factor;
    };

    static const HSpacingValue g_h_spacing_data[] =
    {
	{ "veryverythinmathspace",	(float) 0.0555556   },
	{ "verythinmathspace",  	(float) 0.111111    },
	{ "thinmathspace",          	(float) 0.166667    },
	{ "mediummathspace",    	(float) 0.222222    },
	{ "thickmathspace",     	(float) 0.277778    },
	{ "verythickmathspace", 	(float) 0.333333    },
	{ "veryverythickmathspace", 	(float) 0.388889    },
	{ 0,    	    	    	(float) 0   	    }
    };

    const HSpacingValue *v = g_h_spacing_data;
    for (; v->name != 0; ++v) {
	if (value == v->name)
	    return (int)(em*v->factor);
    }

    if (value.endsWith("em")) {
	value.truncate(value.length() - 2);
	bool float_ok;
	float factor = value.toFloat(&float_ok);
	if (float_ok && factor >= 0)
	    return (int)(em*factor);
	else {
	    qWarning("interpretSpacing(): could not parse \"%sem\"", value.toLatin1().data());
	    if (ok != 0)
		*ok = false;
	    return 0;
	}
    }

    if (value.endsWith("ex")) {
	value.truncate(value.length() - 2);
	bool float_ok;
	float factor = value.toFloat(&float_ok);
	if (float_ok && factor >= 0)
	    return (int)(ex*factor);
	else {
	    qWarning("interpretSpacing(): could not parse \"%sex\"", value.toLatin1().data());
	    if (ok != 0)
		*ok = false;
	    return 0;
	}
    }

   if (value.endsWith("cm")) {
	value.truncate(value.length() - 2);
	bool float_ok;
	float factor = value.toFloat(&float_ok);
	if (float_ok && factor >= 0) {
	    Q_ASSERT(device);
	    Q_ASSERT(device->width() != 0);
	    Q_ASSERT(device->widthMM() != 0);
	    return (int)(factor*10*device->width()/device->widthMM());
	}
	else {
	    qWarning("interpretSpacing(): could not parse \"%scm\"", value.toLatin1().data());
	    if (ok != 0)
		*ok = false;
	    return 0;
	}
    }

    if (value.endsWith("mm")) {
	value.truncate(value.length() - 2);
	bool float_ok;
	float factor = value.toFloat(&float_ok);
	if (float_ok && factor >= 0) {
	    Q_ASSERT(device);
	    Q_ASSERT(device->width() != 0);
	    Q_ASSERT(device->widthMM() != 0);
	    return (int)(factor*device->width()/device->widthMM());
	}
	else {
	    qWarning("interpretSpacing(): could not parse \"%smm\"", value.toLatin1().data());
	    if (ok != 0)
		*ok = false;
	    return 0;
	}
    }

    if (value.endsWith("in")) {
	value.truncate(value.length() - 2);
	bool float_ok;
	float factor = value.toFloat(&float_ok);
	if (float_ok && factor >= 0) {
	    Q_ASSERT(device);
	    Q_ASSERT(device->width() != 0);
	    Q_ASSERT(device->widthMM() != 0);
	    return (int)(factor*10*device->width()/(2.54*device->widthMM()));
	}
	else {
	    qWarning("interpretSpacing(): could not parse \"%sin\"", value.toLatin1().data());
	    if (ok != 0)
		*ok = false;
	    return 0;
	}
    }

    if (value.endsWith("px")) {
	value.truncate(value.length() - 2);
	bool float_ok;
	int i = (int) value.toFloat(&float_ok);
	if (float_ok && i >= 0)
	    return i;
	else {
	    qWarning("interpretSpacing(): could not parse \"%spx\"", value.toLatin1().data());
	    if (ok != 0)
		*ok = false;
	    return 0;
	}
    }

    bool float_ok;
    int i = (int)value.toFloat(&float_ok);
    if (float_ok && i >= 0)
	return i;

    qWarning("interpretSpacing(): could not parse \"%s\"", value.toLatin1().data());
    if (ok != 0)
	*ok = false;
    return 0;
}

static int interpretPercentSpacing(QString value, int base, bool *ok)
{
    if (!value.endsWith("%")) {
	if (ok != 0)
	    *ok = false;
	return 0;
    }

    value.truncate(value.length() - 1);
    bool float_ok;
    float factor = value.toFloat(&float_ok);
    if (float_ok && factor >= 0) {
	if (ok != 0)
	    *ok = true;
	return (int)(base*factor/100.0);
    }

    qWarning("interpretPercentSpacing(): could not parse \"%s%%\"", value.toLatin1().data());
    if (ok != 0)
	*ok = false;
    return 0;
}

static int interpretPointSize(QString value, bool *ok)
{
    if (!value.endsWith("pt")) {
	if (ok != 0)
	    *ok = false;
	return 0;
    }

    value.truncate(value.length() - 2);
    bool float_ok;
    int pt_size = (int) value.toFloat(&float_ok);
    if (float_ok && pt_size > 0) {
	if (ok != 0)
	    *ok = true;
	return pt_size;
    }

    qWarning("interpretPointSize(): could not parse \"%spt\"", value.toLatin1().data());
    if (ok != 0)
	*ok = false;
    return 0;
}

static const NodeSpec *mmlFindNodeSpec(Mml::NodeType type)
{
    const NodeSpec *spec = g_node_spec_data;
    for (; spec->type != Mml::NoNode; ++spec) {
	if (type == spec->type) return spec;
    }
    return 0;
}

static const NodeSpec *mmlFindNodeSpec(const QString &tag)
{
    const NodeSpec *spec = g_node_spec_data;
    for (; spec->type != Mml::NoNode; ++spec) {
	if (tag == spec->tag) return spec;
    }
    return 0;
}

static bool mmlCheckChildType(Mml::NodeType parent_type, Mml::NodeType child_type,
			    QString *error_str)
{
    if (parent_type == Mml::UnknownNode || child_type == Mml::UnknownNode)
	return true;

    const NodeSpec *child_spec = mmlFindNodeSpec(child_type);
    const NodeSpec *parent_spec = mmlFindNodeSpec(parent_type);

    Q_ASSERT(parent_spec != 0);
    Q_ASSERT(child_spec != 0);

    QString allowed_child_types(parent_spec->child_types);
    // null list means any child type is valid
    if (allowed_child_types.isNull())
	return true;

    QString child_type_str = QString(" ") + child_spec->type_str + " ";
    if (!allowed_child_types.contains(child_type_str)) {
	if (error_str != 0)
	    *error_str = QString("illegal child ")
			  + child_spec->type_str
			  + " for parent "
			  + parent_spec->type_str;
	return false;
    }

    return true;
}

static bool mmlCheckAttributes(Mml::NodeType child_type, const MmlAttributeMap &attr,
			    QString *error_str)
{
    const NodeSpec *spec = mmlFindNodeSpec(child_type);
    Q_ASSERT(spec != 0);

    QString allowed_attr(spec->attributes);
    // empty list means any attr is valid
    if (allowed_attr.isEmpty())
	return true;

    MmlAttributeMap::const_iterator it = attr.begin(), end = attr.end();
    for (; it != end; ++it) {
	QString name = it.key();

	if (name.indexOf(':') != -1)
	    continue;

	QString padded_name = " " + name + " ";
	if (!allowed_attr.contains(padded_name)) {
	    if (error_str != 0)
		*error_str = QString("illegal attribute ")
				 + name
				 + " in "
				 + spec->type_str;
	    return false;
	}
    }

    return true;
}

static int attributeIndex(const QString &name)
{
    for (unsigned i = 0; i < g_oper_spec_rows; ++i) {
	if (name == g_oper_spec_names[i])
	    return i;
    }
    return -1;
}

static QString decodeEntityValue(QString literal)
{
    QString result;

    while (!literal.isEmpty()) {

	if (!literal.startsWith("&#")) {
	    qWarning(("decodeEntityValue(): bad entity literal: \"" + literal + "\"").toLatin1().data());
	    return QString::null;
	}

	literal = literal.right(literal.length() - 2);

	int i = literal.indexOf(';');
	if (i == -1) {
	    qWarning(("decodeEntityValue(): bad entity literal: \"" + literal + "\"").toLatin1().data());
	    return QString::null;
	}

	QString char_code = literal.left(i);
	literal = literal.right(literal.length() - i - 1);

	if (char_code.isEmpty()) {
	    qWarning(("decodeEntityValue(): bad entity literal: \"" + literal + "\"").toLatin1().data());
	    return QString::null;
	}

	if (char_code.at(0) == 'x') {
	    char_code = char_code.right(char_code.length() - 1);
	    bool ok;
	    unsigned c = char_code.toUInt(&ok, 16);
	    if (!ok) {
		qWarning(("decodeEntityValue(): bad entity literal: \"" + literal + "\"").toLatin1().data());
		return QString::null;
	    }
	    result += QChar(c);
	}
	else {
	    bool ok;
	    unsigned c = char_code.toUInt(&ok, 10);
	    if (!ok) {
		qWarning(("decodeEntityValue(): bad entity literal: \"" + literal + "\"").toLatin1().data());
		return QString::null;
	    }
	    result += QChar(c);
	}
    }

    return result;
}

static const EntitySpec *searchEntitySpecData(const QString &value, const EntitySpec *from = 0)
{
    const EntitySpec *ent = from;
    if (ent == 0)
	ent = g_xml_entity_data;
    for (; ent->name != 0; ++ent) {
	QString ent_value = decodeEntityValue(ent->value);
	if (value == ent_value)
	    return ent;
    }
    return 0;
}

struct OperSpecSearchResult
{
    OperSpecSearchResult() { prefix_form = infix_form = postfix_form = 0; }

    const OperSpec *prefix_form,
		   *infix_form,
		   *postfix_form;

    const OperSpec *&getForm(Mml::FormType f);
    bool haveForm(Mml::FormType f)
	{ return getForm(f) != 0; }
    void addForm(const OperSpec *spec)
	{ getForm(spec->form) = spec; }
};

const OperSpec *&OperSpecSearchResult::getForm(Mml::FormType f)
{
    switch (f) {
	case Mml::PrefixForm:
	    return prefix_form;
	case Mml::InfixForm:
	    return infix_form;
	case Mml::PostfixForm:
	    return postfix_form;
    }
    return postfix_form; // just to avoid warning
}

/*
    Searches g_oper_spec_data and returns any instance of operator name. There may
    be more instances, but since the list is sorted, they will be next to each other.
*/
static const OperSpec *searchOperSpecData(const QString &name)
{
    // binary search
    // establish invariant g_oper_spec_data[begin].name < name < g_oper_spec_data[end].name

    int cmp = name.compare(g_oper_spec_data[0].name);
    if (cmp < 0)
    	return 0;

    if (cmp == 0)
    	return g_oper_spec_data;

    uint begin = 0;
    uint end = g_oper_spec_count;

    // invariant holds
    while (end - begin > 1) {
    	uint mid = (begin + end)/2;

    	const OperSpec *spec = g_oper_spec_data + mid;
	int cmp = name.compare(spec->name);
    	if (cmp < 0)
	    end = mid;
	else if (cmp > 0)
	    begin = mid;
	else
	    return spec;
    }

    return 0;
}

/*
    This searches g_oper_spec_data until at least one name in name_list is found with FormType form,
    or until name_list is exhausted. The idea here is that if we don't find the operator in the
    specified form, we still want to use some other available form of that operator.
*/
static OperSpecSearchResult _mmlFindOperSpec(const QStringList &name_list, Mml::FormType form)
{
    OperSpecSearchResult result;

    QStringList::const_iterator it = name_list.begin();
    for (; it != name_list.end(); ++it) {
    	const QString &name = *it;

    	const OperSpec *spec = searchOperSpecData(name);
	
	if (spec == 0)
	    continue;
    
    	// backtrack to the first instance of name
	while (spec > g_oper_spec_data && QString((spec - 1)->name) == name)
    	    --spec;

    	// iterate over instances of name until the instances are exhausted or until we
	// find an instance in the specified form.
	do {
	    result.addForm(spec++);
	    if (result.haveForm(form))
	    	break;
	} while (name == spec->name);
	
	if (result.haveForm(form))
	    break;
    }
    
    return result;
}

/*
    text is a string between <mo> and </mo>. It can be a character ('+'), an
    entity reference ("&infin;") or a character reference ("&#x0221E"). Our
    job is to find an operator spec in the operator dictionary (g_oper_spec_data)
    that matches text. Things are further complicated by the fact, that many 
    operators come in several forms (prefix, infix, postfix).

    If available, this function returns an operator spec matching text in the specified
    form. If such operator is not available, returns an operator spec that matches
    text, but of some other form in the preference order specified by the MathML spec. 
    If that's not available either, returns the default operator spec.
*/
static const OperSpec *mmlFindOperSpec(const QString &text, Mml::FormType form)
{
    QStringList name_list;
    name_list.append(text);

    // First, just try to find text in the operator dictionary.
    OperSpecSearchResult result = _mmlFindOperSpec(name_list, form);

    if (!result.haveForm(form)) {
    	// Try to find other names for the operator represented by text.

	const EntitySpec *ent = 0;
	for (;;) {
	    ent = searchEntitySpecData(text, ent);
	    if (ent == 0)
		break;
	    name_list.append('&' + QString(ent->name) + ';');
	    ++ent;
	}

	result = _mmlFindOperSpec(name_list, form);
    }

    const OperSpec *spec = result.getForm(form);
    if (spec != 0)
	return spec;

    spec = result.getForm(Mml::InfixForm);
    if (spec != 0)
	return spec;

    spec = result.getForm(Mml::PostfixForm);
    if (spec != 0)
	return spec;

    spec = result.getForm(Mml::PrefixForm);
    if (spec != 0)
	return spec;

    return &g_oper_spec_defaults;
}

static QString mmlDictAttribute(const QString &name, const OperSpec *spec)
{
    int i = attributeIndex(name);
    if (i == -1)
	return QString::null;
    else
	return spec->attributes[i];
}

static uint interpretMathVariant(const QString &value, bool *ok)
{
    struct MathVariantValue {
	const char *value;
	uint mv;
    };

    static const MathVariantValue g_mv_data[] =
    {
	{ "normal",     	    	    Mml::NormalMV },
	{ "bold",   	    	    	    Mml::BoldMV },
	{ "italic",   	    	    	    Mml::ItalicMV },
	{ "bold-italic",   	    	    Mml::BoldMV | Mml::ItalicMV },
	{ "double-struck",   	    	    Mml::DoubleStruckMV },
	{ "bold-fraktur",   	    	    Mml::BoldMV | Mml::FrakturMV },
	{ "script",   	    	    	    Mml::ScriptMV },
	{ "bold-script",	    	    Mml::BoldMV | Mml::ScriptMV },
	{ "fraktur",   	    	    	    Mml::FrakturMV },
	{ "sans-serif",   	    	    Mml::SansSerifMV },
	{ "bold-sans-serif",   	    	    Mml::BoldMV | Mml::SansSerifMV },
	{ "sans-serif-italic",   	    Mml::SansSerifMV | Mml::ItalicMV },
	{ "sans-serif-bold-italic",         Mml::SansSerifMV | Mml::ItalicMV | Mml::BoldMV },
	{ "monospace",   	    	    Mml::MonospaceMV },
	{ 0,    	    	    	    0 }
    };

    const MathVariantValue *v = g_mv_data;
    for (; v->value != 0; ++v) {
	if (value == v->value) {
	    if (ok != 0)
		*ok = true;
	    return v->mv;
	}
    }

    if (ok != 0)
	*ok = false;

   qWarning("interpretMathVariant(): could not parse value: \"%s\"", value.toLatin1().data());

    return Mml::NormalMV;
}

static Mml::FormType interpretForm(const QString &value, bool *ok)
{
    if (ok != 0)
	*ok = true;

    if (value == "prefix")
	return Mml::PrefixForm;
    if (value == "infix")
	return Mml::InfixForm;
    if (value == "postfix")
	return Mml::PostfixForm;

    if (ok != 0)
	*ok = false;

    qWarning("interpretForm(): could not parse value \"%s\"", value.toLatin1().data());
    return Mml::InfixForm;
}

static Mml::ColAlign interpretColAlign(const QString &value_list, uint colnum, bool *ok)
{
    QString value = interpretListAttr(value_list, colnum, "center");

    if (ok != 0)
	*ok = true;

    if (value == "left")
	return Mml::ColAlignLeft;
    if (value == "right")
	return Mml::ColAlignRight;
    if (value == "center")
	return Mml::ColAlignCenter;

    if (ok != 0)
	*ok = false;

    qWarning("interpretColAlign(): could not parse value \"%s\"", value.toLatin1().data());
    return Mml::ColAlignCenter;
}

static Mml::RowAlign interpretRowAlign(const QString &value_list, uint rownum, bool *ok)
{
    QString value = interpretListAttr(value_list, rownum, "axis");

    if (ok != 0)
	*ok = true;

    if (value == "top")
	return Mml::RowAlignTop;
    if (value == "center")
	return Mml::RowAlignCenter;
    if (value == "bottom")
	return Mml::RowAlignBottom;
    if (value == "baseline")
	return Mml::RowAlignBaseline;
    if (value == "axis")
	return Mml::RowAlignAxis;

    if (ok != 0)
	*ok = false;

    qWarning("interpretRowAlign(): could not parse value \"%s\"", value.toLatin1().data());
    return Mml::RowAlignAxis;
}

static QString interpretListAttr(const QString &value_list, int idx, const QString &def)
{
    QStringList l = value_list.split(' ');

    if (l.count() == 0)
	return def;

    if (l.count() <= idx)
	return l[l.count() - 1];
    else
	return l[idx];
}

static Mml::FrameType interpretFrameType(const QString &value_list, uint idx, bool *ok)
{
    if (ok != 0)
	*ok = true;

    QString value = interpretListAttr(value_list, idx, "none");

    if (value == "none")
	return Mml::FrameNone;
    if (value == "solid")
	return Mml::FrameSolid;
    if (value == "dashed")
	return Mml::FrameDashed;

    if (ok != 0)
	*ok = false;

    qWarning("interpretFrameType(): could not parse value \"%s\"", value.toLatin1().data());
    return Mml::FrameNone;
}


static Mml::FrameSpacing interpretFrameSpacing(const QString &value_list, int em, int ex, bool *ok, QPaintDevice* device)
{
    Mml::FrameSpacing fs;

    QStringList l = value_list.split(' ');
    if (l.count() != 2) {
	qWarning("interpretFrameSpacing: could not parse value \"%s\"", value_list.toLatin1().data());
	if (ok != 0)
	    *ok = false;
	return Mml::FrameSpacing((int)(0.4*em), (int)(0.5*ex));
    }

    bool hor_ok, ver_ok;
    fs.m_hor = interpretSpacing(l[0], em, ex, &hor_ok, device);
    fs.m_ver = interpretSpacing(l[1], em, ex, &ver_ok, device);

    if (ok != 0)
	*ok = hor_ok && ver_ok;

    return fs;
}

static QFont interpretDepreciatedFontAttr(const MmlAttributeMap &font_attr, QFont &fn, int em, int ex, QPaintDevice* device)
{
    if (font_attr.contains("fontsize")) {
	QString value = font_attr["fontsize"];

	for (;;) {

	    bool ok;
	    int ptsize = interpretPointSize(value, &ok);
	    if (ok) {
		fn.setPointSize(ptsize);
		break;
	    }

	    ptsize = interpretPercentSpacing(value, fn.pointSize(), &ok);
	    if (ok) {
		fn.setPointSize(ptsize);
		break;
	    }

	    int size = interpretSpacing(value, em, ex, &ok, device);
	    if (ok) {
		fn.setPixelSize(size);
		break;
	    }

	    break;
	}
    }

    if (font_attr.contains("fontweight")) {
	QString value = font_attr["fontweight"];
	if (value == "normal")
	    fn.setBold(false);
	else if (value == "bold")
	    fn.setBold(true);
	else
	    qWarning("interpretDepreciatedFontAttr(): could not parse fontweight \"%s\"", value.toLatin1().data());
    }

    if (font_attr.contains("fontstyle")) {
	QString value = font_attr["fontstyle"];
	if (value == "normal")
	    fn.setItalic(false);
	else if (value == "italic")
	    fn.setItalic(true);
	else
	    qWarning("interpretDepreciatedFontAttr(): could not parse fontstyle \"%s\"", value.toLatin1().data());
    }

    if (font_attr.contains("fontfamily")) {
	QString value = font_attr["fontfamily"];
	fn.setFamily(value);
    }

    return fn;
}

static QFont interpretMathSize(QString value, QFont &fn, int em, int ex,
                               bool *ok, QPaintDevice* device)
{
    if (ok != 0)
	*ok = true;

    if (value == "small") {
	fn.setPointSize((int)(fn.pointSize()*0.7));
	return fn;
    }

    if (value == "normal")
	return fn;

    if (value == "big") {
	fn.setPointSize((int)(fn.pointSize()*1.5));
	return fn;
    }

    bool size_ok;

    int ptsize = interpretPointSize(value, &size_ok);
    if (size_ok) {
	fn.setPointSize(ptsize);
	return fn;
    }

    int size = interpretSpacing(value, em, ex, &size_ok, device);
    if (size_ok) {
	fn.setPixelSize(size);
	return fn;
    }

    if (ok != 0)
	*ok = false;
    qWarning("interpretMathSize(): could not parse mathsize \"%s\"", value.toLatin1().data());
    return fn;
}

