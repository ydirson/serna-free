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
#include "buildrev.hpp"
#include "docview/SernaDoc.h"
#include "ui/IconProvider.h"
#include "utils/Version.h"

#include "common/String.h"
#include "common/StringCvt.h"

#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QImageReader>
#include <QScrollBar>
#include <iostream>

#include "docview/AboutBase.hpp"
#include "docview/AboutSernaBase.hpp"

USING_COMMON_NS

class AboutSerna : public QDialog,
                   public Ui::AboutBase {
    Q_OBJECT
public:
    AboutSerna(QWidget* parent);
};

AboutSerna::AboutSerna(QWidget* parent)
    : QDialog(parent, 0, true)
{
    setupUi(this);
    setCaption(tr("About Serna"));

    QString version = QString(NOTR("<b>Syntext Serna v%1 - %2.%3</b><br/>"))
        .arg(Version::currentVersion().version())
        .arg(Version::currentVersion().build_date())
        .arg(SERNA_BUILDREV);

    QString copyright = tr("Copyright &copy; 2003 - 2009 Syntext, Inc. "
                           "All rights reserved.<br/>");
    QString uses(tr(
        "<br/><b>Syntext Core Team:</b><br/>"
        "Paul Antonov, Ilia Kuznetsov, Timofey Fouriaev."
        "<br/><br/>"
        "<b>Serna uses:</b><br/>"
        "Trolltech Qt Toolkit v%1 (supported image formats: %2)<br/><br/>"
        "James Clark SP Toolkit v1.3 &copy; 1994, 1995, 1996, 1997, 1998 "
        "James Clark, see SP-COPYING. <br/><br/>"
        "OpenSSL Toolkit (http://www.openssl.org) &copy; 1998-2003 "
        "The OpenSSL Project, "
        "cryptographic software written by Eric Young (eay@cryptsoft.com); "
        "software written by Tim Hudson (tjh@cryptsoft.com). <br/><br/>"
        "GNU Aspell library, "
        "coming with GNU LGPL license, see ASPELL-COPYING. <br/><br/>"
        "English wordlist for GNU Aspell, maintained by Kevin Atkinson, "
        "see ASPELL-EN-COPYING. WebDAV Neon client library coming with"
        "GNU LGPL license, see NEON-COPYING."
        "(See the COPYING files in the installation directory)."
        "<br/>"
    ));

    QString image_formats_str(NOTR("none"));
    QList<QByteArray> image_formats(QImageReader::supportedImageFormats());
    if (!image_formats.isEmpty()) {
        QList<QByteArray>::iterator i = image_formats.begin();
        QStringList str_list;
        for (; i != image_formats.end(); i++)
            str_list.append(*i);
        image_formats_str = str_list.join(QString(", "));
    }

    //! Setting composed text to the text browser
    infoBox_->setText(
        QString(NOTR("<qt style=\" font-size:8pt;color:white;\">%1</qt>")).
        arg(version + copyright + uses.arg(qVersion()).arg(image_formats_str)));

    //! Setting background image
    QPixmap logo_pix = Sui::icon_provider().getPixmap(NOTR("SernaAbout"));
    QPalette p = palette();
    p.setBrush(QPalette::Base, QBrush(logo_pix));
    p.setBrush(QPalette::Window, QBrush(logo_pix));
    setPalette(p);
    setFixedSize(logo_pix.size());
    
    const int BOFF = 3;
    const int XOFF = 22;
    QPixmap bg_pix = logo_pix.copy(XOFF, logo_pix.height() - BOFF, 
        logo_pix.width() - XOFF, BOFF);
    QPalette p2;
    p2.setBrush(QPalette::Base, QBrush(bg_pix));
    p2.setBrush(QPalette::Window, QBrush(bg_pix));
    infoBox_->setPalette(p2);
}
/////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(AboutDialog, SernaDoc)

bool AboutDialog::doExecute(SernaDoc* se, EventData*)
{
    return (QDialog::Accepted == AboutSerna(se->widget(0)).exec());
}

#include "moc/AboutSerna.moc"
