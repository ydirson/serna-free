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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "core/EULADialogBase.hpp"
#include "common/StringCvt.h"
#include "common/Url.h"
#include "ui/IconProvider.h"
#include "utils/Config.h"

#include <QPixmap>
#include <QString>
#include <QFile>
#include <QTextStream>

using namespace Common;

static QString eula_text();

namespace {

class EULADialog : public QDialog, public Ui::EULADialogBase {
public:
    EULADialog()
        : QDialog(qApp->activeWindow())
    {
        setupUi(this);
        QPixmap serna_pix(Sui::icon_provider().getPixmap(NOTR("serna_app")));
        if (!serna_pix.isNull())
            setIcon(serna_pix);
        eulaEdit_->setText(eula_text());
    }
};

} // namespace

bool eula_dialog()
{
    return QDialog::Accepted == EULADialog().exec();
}

//////////////////////////////////////////////////////////////////////////

static QString get_file(const QString& fn)
{
    QString ostring;
    Url path(config().getDataDir());
    QFile file(String(path.combineDir2Path(Url(fn))));
    if (!file.open(IO_ReadOnly)) {
        file.setName(String(path.upperUrl().upperUrl()));
        if (!file.open(IO_ReadOnly))
            return ostring;
    }
    QTextStream stream(&file);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    stream >> ostring;
    file.close();
    return ostring;
}

QString eula_text()
{
    QString ostring;
    ostring += get_file(NOTR("LICENSE.GPL"));
    if (ostring.isEmpty()) 
        return NOTR("** MISSING LICENSE FILE. PLEASE REMOVE THIS COPY OF THE SOFTWARE FROM YOUR COMPUTER. **");
    ostring += NOTR("\n-------------------------------------------------\n");
    ostring += get_file(NOTR("GPL_EXCEPTION.txt"));
    return ostring;
}

