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
#include "docview/SernaDoc.h"
#include "utils/file_utils.h"
#include "utils/DocSrcInfo.h"
#include "utils/Properties.h"
#include "utils/Config.h"
#include "common/Url.h"
#include "common/PropertyTreeEventData.h"
#include <QApplication>

using namespace Common;

PROPTREE_EVENT_IMPL(LaunchBrowser, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(LaunchBrowser, "PropertyTree", "-")

bool LaunchBrowser::doExecute(SernaDoc* sd, EventData*)
{
    String urlString = ed_->getSafeProperty(NOTR("url"))->getString();
    String basepath = ed_->getSafeProperty(NOTR("basepath"))->getString();

    Url url(urlString);
    if (!basepath.isEmpty() && url.isRelative())
        url = Url(basepath).combinePath2Path(url);
    QString errMsg;

    if (url.isLocal() && !url.exists()) {
        errMsg = tr("File '%0' does not exist").arg(urlString);
    } else {
        using namespace FileUtils;
        LaunchCode code = launch_file_handler(url);
        switch (code) {
            case HANDLER_OK:
                return true;
            case HANDLER_UNDEF:
                errMsg = tr("Browser application for URLs like \n'%0'\n"
                            "is not defined.\n\n"
                            "Please check Tools->Preferences->Applications");
                errMsg = errMsg.arg(urlString);
                break;
            default:
                errMsg = tr("Unable to launch browser application for\n"
                            "'%0'").arg(urlString);
                break;
        }
    }
    sd->showMessageBox(SernaDoc::MB_CRITICAL, tr("Error launching browser"),
        errMsg, tr("&Ok"));
    return false;
}
