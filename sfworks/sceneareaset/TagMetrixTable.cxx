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
#include "formatter/FontMgr.h"
#include "sceneareaset/TagMetrixTable.h"
#include <math.h>

using namespace Common;
using namespace Formatter;

const uint BORDER_WIDTH = 2;

CRange SceneTagMetrix::size(TagMetrix::Type type)
{
    switch (type) {
        case TagMetrix::START :
        case TagMetrix::END :
            return CRange(start_[2].x() + 3,
                          start_[4].y() + 2);
        case TagMetrix::EMPTY :
            return CRange(empty_[4].x() + 3,
                          empty_[7].y() + 2);
        case TagMetrix::FOLD :
            return CRange(fold_[2].x() + 3,
                          fold_[3].y() + 2);
        case TagMetrix::CHOICE :
            return CRange(choice_[2].x() + 3,
                          choice_[3].y() + 2);
        case TagMetrix::INTERNAL_SECTION :
            return CRange(internalErs_[1].x() + 4,
                          internalErs_[2].y() + 2);
        case TagMetrix::EXTERNAL_SECTION :
            return CRange(externalErs_[1].x() + 4,
                          externalErs_[5].y() + 2);
        case TagMetrix::XINCLUDE_SECTION :
            return CRange(xincludeEnd_[2].x() + 4,
                          xincludeEnd_[3].y() + 2);
        case TagMetrix::REDLINE_SECTION :
            return CRange(redlineStart_[1].x() + 4,
                          redlineStart_[3].y() + 2);
        case TagMetrix::MARKED_SECTION :
            return CRange(markedSection_[1].x() + 4,
                          markedSection_[2].y() + 2);
        default:
            return CRange(0,0);
   }
}

TagMetrix* SceneTagMetrixTable::getMetrix(const String& name,
                                          const FontPtr& font)
{
    TagMetrixMap::const_iterator iter = metrixMap_.find(name);
    if (metrixMap_.end() != iter)
        return &metrixMap_[name];

    SceneTagMetrix metrix;
    CRange nm(font->width(name), font->descender() + font->accender());

    const qreal border = ceil((font->descender() + font->accender()) / 8.0);
    const qreal hh = (nm.h_ / 2) + border;
    const qreal h = 2 * hh;

    /////////////////////////////////////////////////////////////////////

    metrix.startName_ = QPointF(border, border);
    metrix.start_ << QPointF(0,                     0)
                  << QPointF(nm.w_ + 2*border,      0)
                  << QPointF(nm.w_ + 2*border + hh, hh)
                  << QPointF(nm.w_ + 2*border,      h)
                  << QPointF(0,                     h);

    metrix.endName_ = QPointF(hh + border, border);
    metrix.end_ << QPointF(0,        hh)
                << QPointF(hh,       0)
                << QPointF(nm.w_ + 2*border + hh,   0)
                << QPointF(nm.w_ + 2*border + hh,   h)
                << QPointF(hh,       h);

    /////////////////////////////////////////////////////////////////////

    metrix.emptyName_ = QPointF(border, border);
    metrix.empty_ << QPointF(0,                         0)
                  << QPointF(nm.w_ + 2*border,          0)
                  << QPointF(nm.w_ + 2*border + hh,     hh)
                  << QPointF(nm.w_ + 2*border + 2*hh,   0)
                  << QPointF(nm.w_ + 2*border + 2*hh,   h)
                  << QPointF(nm.w_ + 2*border + hh,     hh)
                  << QPointF(nm.w_ + 2*border,          h)
                  << QPointF(0,                         h);

    /////////////////////////////////////////////////////////////////////

    qreal w = font->width(name) + border + 3*h/4;
    metrix.choiceName_ = QPointF(border, border);
    metrix.choice_ << QPointF(0, 0)
                   << QPointF(w, 0)
                   << QPointF(w, h)
                   << QPointF(0, h);

    w = h + font->width(name) + border;
    metrix.foldName_ = QPointF(h, border);
    metrix.fold_ << QPointF(0, 0)
                 << QPointF(w, 0)
                 << QPointF(w, h)
                 << QPointF(0, h);

    /////////////////////////////////////////////////////////////////////

    const qreal unit = ceil(h / 6 + 0.5);
    qreal b = unit * 3;
    qreal c = unit;
    metrix.externalErs_ << QPointF(0,    0)
                        << QPointF(b,    0)
                        << QPointF(b,    c)
                        << QPointF(c,    c)
                        << QPointF(c,    b)
                        << QPointF(0,    b);

    metrix.externalEre_ << QPointF(b,    b)
                        << QPointF(0,    b)
                        << QPointF(0,    2*c)
                        << QPointF(2*c,  2*c)
                        << QPointF(2*c,  0)
                        << QPointF(b,    0);

    /////////////////////////////////////////////////////////////////////

    const qreal d = b / 2;
    metrix.internalErs_ << QPointF(0,    0)
                        << QPointF(d,    d)
                        << QPointF(0,    2*d);

    metrix.internalEre_ << QPointF(d,    0)
                        << QPointF(d,    2*d)
                        << QPointF(0,    d);

    /////////////////////////////////////////////////////////////////////
        
    qreal i = 3.0 * unit/4.0;
    metrix.xincludeEnd_ << QPointF(0,      i)
                        << QPointF(i,      0)
                        << QPointF(3*i,    2*i)
                        << QPointF(i,      4*i)
                        << QPointF(0,      3*i)
                        << QPointF(i,      2*i);

    metrix.xincludeStart_ << QPointF(0,      2*i)
                          << QPointF(2*i,    0)
                          << QPointF(3*i,    i)
                          << QPointF(2*i,    2*i)
                          << QPointF(3*i,    3*i)
                          << QPointF(2*i,    4*i);

    /////////////////////////////////////////////////////////////////////

    const qreal rh = h - 2 * border;
    metrix.redlineStart_ << QPointF(0,   0)
                         << QPointF(c/2, 0)
                         << QPointF(0,   0)
                         << QPointF(0,   rh)
                         << QPointF(c/2, rh)
                         << QPointF(0,   rh);

    metrix.redlineEnd_ << QPointF(0,   0)
                       << QPointF(c/2, 0)
                       << QPointF(c/2, rh)
                       << QPointF(0,   rh)
                       << QPointF(c/2, rh)
                       << QPointF(c/2, 0);

    /////////////////////////////////////////////////////////////////////

    metrix.markedSection_ << QPointF(0,  0)
                          << QPointF(c,  0)
                          << QPointF(c,  c)
                          << QPointF(0,  c);

    /////////////////////////////////////////////////////////////////////

    metrix.font_ = font;

    //! add to table
    metrixMap_[name] = metrix;

    return &metrixMap_[name];
}
