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
#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "common/String.h"
#include "common/Url.h"
#include "common/Encodings.h"

#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>

using namespace Common;
typedef Common::Encodings ENC;

#include <iostream>

/////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(SaveAsDialog, SernaDoc)
REGISTER_COMMAND_EVENT_MAKER(SaveAsDialog, "PropertyTree", "PropertyTree")

bool SaveAsDialog::doExecute(SernaDoc* sernaDoc, EventData* ed)
{
    QString filter = qApp->translate("SaveAsDialogImpl", "XML files (*.xml)");
    filter += NOTR(";;");
    filter += qApp->translate("SaveAsDialogImpl", "All files (*)");
    
    String fname(ed_->getSafeProperty(DocSrcInfo::DOC_PATH)->getString());
    if (fname.isEmpty())
        fname = ed_->getSafeProperty(DocSrcInfo::BROWSE_DIR)->getString();
    String result;
    for (;;) {
        result = QFileDialog::getSaveFileName(sernaDoc->widget(0), 
            qApp->translate("SaveAsDialogImpl", "Save As..."), 
            fname, filter, 0, QFileDialog::DontConfirmOverwrite);
        if (result.isEmpty())
            return false;
        if (0 > result.find('.'))
            result += NOTR(".xml");
        if (!QFileInfo(result).exists())
            break;
        if (QMessageBox::No != QMessageBox::warning(sernaDoc->widget(0), 
            tr("Warning"),
            tr("File \"%1\" already exists.\nOverwrite?").arg(result),
            QMessageBox::Yes|QMessageBox::No, QMessageBox::No))
                break;
    }
    static_cast<PropertyTreeEventData*>(ed)->root()->
        makeDescendant(DocSrcInfo::DOC_PATH, result, true);
    return true;
}
