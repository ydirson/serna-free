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
#ifndef MML_UTILS_H_
#define MML_UTILS_H_

#include <QString>

namespace MmlImpl {

static const double   g_mfrac_spacing			= 0.1;
static const double   g_mroot_base_margin		= 0.1;
static const double   g_script_size_multiplier	    	= 0.7071; // sqrt(1/2)
static const int      g_min_font_point_size		= 8;
static const QChar    g_radical_char  	    	    	= QChar(0x1A, 0x22);
static const unsigned g_oper_spec_rows     	        = 9;

struct Mml
{
    enum NodeType {
	    NoNode = 0, MiNode, MnNode, MfracNode, MrowNode, MsqrtNode,
	    MrootNode, MsupNode, MsubNode, MsubsupNode, MoNode,
	    MstyleNode, TextNode, MphantomNode, MfencedNode,
	    MtableNode, MtrNode, MtdNode, MoverNode, MunderNode,
	    MunderoverNode, MerrorNode, MtextNode, MpaddedNode,
	    MspaceNode, MalignMarkNode, UnknownNode
    };

    enum MathVariant {
	    NormalMV	    = 0x0000,
	    BoldMV  	    = 0x0001,
	    ItalicMV	    = 0x0002,
	    DoubleStruckMV  = 0x0004,
	    ScriptMV        = 0x0008,
	    FrakturMV       = 0x0010,
	    SansSerifMV     = 0x0020,
	    MonospaceMV     = 0x0040
    };

    enum FormType { PrefixForm, InfixForm, PostfixForm };
    enum ColAlign { ColAlignLeft, ColAlignCenter, ColAlignRight };
    enum RowAlign { RowAlignTop, RowAlignCenter, RowAlignBottom,
		    RowAlignAxis, RowAlignBaseline };
    enum FrameType { FrameNone, FrameSolid, FrameDashed };

    struct FrameSpacing {
	FrameSpacing(int hor = 0, int ver = 0)
	    : m_hor(hor), m_ver(ver) {}
	int m_hor, m_ver;
    };
};

struct OperSpec {
    enum StretchDir { NoStretch, HStretch, VStretch, HVStretch };

    const char *name;
    Mml::FormType form;
    const char *attributes[g_oper_spec_rows];
    StretchDir stretch_dir;
};

struct NodeSpec
{
    Mml::NodeType type;
    const char *tag;
    const char *type_str;
    int child_spec;
    const char *child_types;
    const char *attributes;

    enum ChildSpec {
	    ChildAny     = -1, // any number of children allowed
	    ChildIgnore  = -2, // do not build subexpression of children
	    ImplicitMrow = -3  // if more than one child, build mrow
    };
};

struct EntitySpec
{
    const char *name;
    const char *value;
};


extern const EntitySpec g_xml_entity_data[];
extern const NodeSpec   g_node_spec_data[];
extern const char*      g_oper_spec_names[g_oper_spec_rows];
extern const uint       g_oper_spec_count;
extern const OperSpec   g_oper_spec_defaults;
extern const OperSpec   g_oper_spec_data[];

} // namespace MmlImpl

#endif // MML_UTILS_H_
