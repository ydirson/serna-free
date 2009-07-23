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
#include "ui/IconProvider.h"

#include "structeditor/impl/debug_se.h"
#include "structeditor/StructEditor.h"
#include "structeditor/StructDocument.h"

#include "docview/SernaDoc.h"
#include "editableview/EditableView.h"
#include "genui/StructDocumentActions.hpp"

#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"

#include "common/PropertyTreeEventData.h"
#include "utils/Config.h"
#include "utils/DocSrcInfo.h"

#include "formatter/impl/Areas.h"

#include <QWidget>

using namespace Common;

static const int MIN_ZOOM = 10;
static const int MAX_ZOOM = 1000;
static const double ZOOM_STEP = 1.15;

static const char * const FIT_PAGE_WIDTH    = NOTR("pageWidth");
static const char * const FIT_PAGE_HEIGHT   = NOTR("pageHeight");

/////////////////////////////////////////////////////////////////////////

static int page_fit_scale(EditableView& view, bool fitWidth)
{
    const Formatter::Area* area = view.getInitialCursor().area();
    while (area && Formatter::PAGE_AREA != area->type())
        area = area->parent();
    if (area) {
        const Formatter::PageArea* page =
            static_cast<const Formatter::PageArea*>(area);
        QWidget* view_widget = view.widget();
        if (fitWidth)
            return int((view_widget->width() - 20) * 100.0 / page->allcW());
        else
            return int((view_widget->height() - 20) * 100.0 / page->allcH());
    }
    return 100;
}

static void enable_zoom_actions(StructEditor* se, double scale)
{
    se->uiActions().zoomIn()->setBool(Sui::IS_ENABLED, 
        (scale * ZOOM_STEP) <= 1000);
    se->uiActions().zoomOut()->setBool(Sui::IS_ENABLED, 
        (scale * ZOOM_STEP) >= 10);
}

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(SetZoom, StructEditor)

bool SetZoom::doExecute(StructEditor* se, EventData*)
{
    using namespace Sui;

    Action* current_zoom = se->uiActions().zoom();
    Action* sub_zoom = current_zoom->activeSubAction();
    String zoom_value = current_zoom->get(INSCRIPTION);
    String zoom_name(sub_zoom ? sub_zoom->get(NAME) : String());

    if (zoom_name == NOTR("actualSize"))
        zoom_value = NOTR("100");
    const bool is_to_fit = (FIT_PAGE_WIDTH == zoom_name || 
                            FIT_PAGE_HEIGHT == zoom_name);
    if (!is_to_fit && !zoom_value.contains('%')) {
        zoom_value += "%";
        current_zoom->set(INSCRIPTION, zoom_value);
    }
    double scale = 100;
    if (is_to_fit) 
        scale = page_fit_scale(se->editableView(),
                               FIT_PAGE_WIDTH == zoom_name);
    else {
        // if name if valid, use name; otherwise, use (custom) value
        bool ok = false;
        scale = String(zoom_name).replace("%", "").toDouble(&ok);
        if (!ok) {
            scale = String(zoom_value).replace("%", "").toDouble(&ok);
            if (!ok)
                return false;
        }
    }
    if (scale < MIN_ZOOM)
        scale = MIN_ZOOM;
    else
        if (scale > MAX_ZOOM)
            scale = MAX_ZOOM;
    enable_zoom_actions(se, scale);
    scale /= 100.;
    if (scale != se->editableView().getZoom()) 
        se->editableView().setZoom(scale);
    return true;
}

/////////////////////////////////////////////////////////////////////////

static bool set_custom_zoom(StructEditor* se, double scale)
{
    se->editableView().setZoom(scale / 100.);
    Sui::Action* zoom = se->uiActions().zoom();
    String str = String::number(scale, 'd', 1).replace(".0", "") + "%";
    zoom->setActiveSubAction(NOTR("custom"));
    zoom->set(Sui::INSCRIPTION, str);
    zoom->dispatch();
    return true;
}

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ZoomIn, StructEditor)

bool ZoomIn::doExecute(StructEditor* se, EventData*)
{
    double scale = 100. * se->editableView().getZoom() * ZOOM_STEP;
    if (scale < MAX_ZOOM) {
        bool ok = set_custom_zoom(se, scale);
        enable_zoom_actions(se, scale);
        return ok;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ZoomOut, StructEditor)

bool ZoomOut::doExecute(StructEditor* se, EventData*)
{
    double scale = 100. * se->editableView().getZoom() / ZOOM_STEP;
    if (scale < MAX_ZOOM) {
        bool ok = set_custom_zoom(se, scale);
        enable_zoom_actions(se, scale);
        return ok;
    }
    return false;
}

