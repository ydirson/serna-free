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

#include "core/core_defs.h"
#include "docview/SernaDoc.h"
#include "common/String.h"
#include "common/PropertyTreeEventData.h"
#include "utils/Version.h"

#include "RegistrationProgressDialogBase.hpp"
#include "RegistrationDetailsWidgetBase.hpp"

#include <QtCore/QString>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>
#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>
#include <QtNetwork/QHttpRequestHeader>
#include <QTimer>


using namespace Common;

#define REGISTRATION_SERVER "reg.syntext.com"
#define REGISTRATION_CMD "/serna-free"

//! Connection timeout, seconds
#define CONNECTION_TIMEOUT 10
#define REQUEST_METHOD "POST"


enum REQUEST_STATE {
    REQUEST_PREPARE = 0,
    REQUEST_SEND = 50,
    REQUEST_DONE = 100
};


void set_label_italic_text(QLabel* label, const String& text)
{
    label->setText(QString("<i>%1</i>").arg(text));
}


/* Auxiliary widget to show sent information to the server */
class RegistrationDetailsWidget: public QWidget,
				 public Ui::RegistrationDetailsWidgetBase {
public:
    RegistrationDetailsWidget(QWidget* parent)
	: QWidget(parent)
    {
	setupUi(this);
    }
};


class RegistrationProgressDialogImpl : public QDialog,
				       public Ui::RegistrationProgressDialogBase {
    Q_OBJECT

public:
    RegistrationProgressDialogImpl(QWidget* parent, PropertyNode* userInfo,
				   bool showDetails = true);
    virtual int exec();

private slots:
    void processTerminated();
    void requestSent();
    void responseReceived(const QHttpResponseHeader& responseHeader);
    void requestTimeout();

private:
    PropertyNode* userInfo_;
    QHttp* httpConn_;
    QTimer* requestTimeoutTimer_;
    RegistrationDetailsWidget* registrationDetailsWidget_;
};



RegistrationProgressDialogImpl::RegistrationProgressDialogImpl(QWidget* parent,
							       PropertyNode* userInfo,
							       bool showDetails)
    : QDialog(parent), userInfo_(userInfo)
{
    setupUi(this);

    registrationDetailsWidget_ = new RegistrationDetailsWidget(this);

    QVBoxLayout* layout = static_cast<QVBoxLayout *>(this->layout());
    layout->insertWidget(1, registrationDetailsWidget_);

    if (!showDetails)
	registrationDetailsWidget_->hide();

    progressBar_->setRange(REQUEST_PREPARE, REQUEST_DONE);
    progressBar_->reset();

    //! Fill in information labels
    set_label_italic_text(registrationDetailsWidget_->urlLabel_,
			  String("http://" REGISTRATION_SERVER REGISTRATION_CMD));
    set_label_italic_text(registrationDetailsWidget_->methodLabel_,
			  String(REQUEST_METHOD));
    set_label_italic_text(registrationDetailsWidget_->firstnameLabel_,
			  userInfo->getSafeProperty("firstname")->getString());
    set_label_italic_text(registrationDetailsWidget_->surnameLabel_,
			  userInfo->getSafeProperty("surname")->getString());
    set_label_italic_text(registrationDetailsWidget_->emailLabel_,
			  userInfo->getSafeProperty("email")->getString());
    set_label_italic_text(registrationDetailsWidget_->companyLabel_,
			  userInfo_->getSafeProperty("company")->getString());

    bool subscribe_news = userInfo_->getSafeProperty("subscribeNews")->getBool();
    set_label_italic_text(registrationDetailsWidget_->subscribeNewsLabel_,
			  String(subscribe_news ? tr("yes") : tr("no")));

    httpConn_ = new QHttp(this);

    connect(cancelButton_, SIGNAL(clicked()), this, SLOT(processTerminated()));
    connect(httpConn_, SIGNAL(requestStarted(int)), this, SLOT(requestSent()));
    connect(httpConn_, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
	    this, SLOT(responseReceived(const QHttpResponseHeader&)));

    requestTimeoutTimer_ = new QTimer();
}


int RegistrationProgressDialogImpl::exec()
{
    httpConn_->setHost(REGISTRATION_SERVER);
    QHttpRequestHeader header(REQUEST_METHOD, REGISTRATION_CMD);

    header.setValue("Host", REGISTRATION_SERVER);
    header.setValue("User-Agent", QString("Syntext Serna %1").
 		                            arg(Version::version()));

    header.setContentType("application/x-www-form-urlencoded");

    QString args("firstname=%1&surname=%2&company=%3&"
		 "email=%4&subscribe-to-news=%5");
    args = args.arg(registrationDetailsWidget_->firstnameLabel_->text(),
		    registrationDetailsWidget_->surnameLabel_->text(),
		    registrationDetailsWidget_->companyLabel_->text(),
		    registrationDetailsWidget_->emailLabel_->text(),
		    registrationDetailsWidget_->subscribeNewsLabel_->text());

    httpConn_->request(header, args.toUtf8());

    requestTimeoutTimer_->start(CONNECTION_TIMEOUT * 1000);
    connect(requestTimeoutTimer_, SIGNAL(timeout()),
	    this, SLOT(requestTimeout()));

    return QDialog::exec();
}


void RegistrationProgressDialogImpl::processTerminated()
{
    QMessageBox::information(this, tr("Registration terminated!"),
			     tr("Registration process aborted. "
				"You can repeat it any time later."));
    reject();
}


void RegistrationProgressDialogImpl::requestSent()
{
    progressBar_->setValue(REQUEST_SEND);
}

void RegistrationProgressDialogImpl::requestTimeout()
{
    QMessageBox::information(this, tr("Registration server timeout!"),
			     tr("Sorry, cannot establish connection "
				"with the registration server! "
				"Please check your internet connection."));
    requestTimeoutTimer_->stop();
    reject();
}

void RegistrationProgressDialogImpl::responseReceived(const QHttpResponseHeader&
						      responseHeader)
{
    requestTimeoutTimer_->stop();

    progressBar_->setValue(REQUEST_DONE);

    int code = responseHeader.statusCode() / 100;
    bool is_critical_response = false;
    QString msg_title;
    QString msg_body;

    switch (code) {
    case 5:
	msg_title = tr("Registration Server Error!");
	msg_body = tr("Registration server error. Can you please try later or "
		      "ask "
		      "<a href=\"mailto:support@syntext.com\">support@syntext.com</a>"
		      " why server is down?");
	is_critical_response = true;
	break;

    case 4:
	msg_title = tr("Incorrect Registration Request!");
	msg_body = tr("Incorrect registration request to the server. Can you "
		      "please try later or ask "
		      "<a href=\"mailto:support@syntext.com\">support@syntext.com</a>"
		      " why server is down?");
	is_critical_response = true;
	break;

    case 2:
	msg_title = tr("Registration Finished!");
	msg_body = tr("Serna XML Editor has been successfully registered. "
		      "Thank you!");
	break;

    default:
	msg_title = tr("Unexpected Error!");
	msg_body = tr("Unexpected registration error. Can you please ask "
		      "<a href=\"mailto:support@syntext.com\">support@syntext.com</a>"
		      " why it happened?");
	is_critical_response = true;
	break;
    }

    if (!is_critical_response) {
	QMessageBox::information(this, msg_title, msg_body);
	accept();
	return;
    }

    QMessageBox::warning(this, msg_title, msg_body);
    reject();
}


/////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(RegistrationProgressDialog, SernaDoc)

bool RegistrationProgressDialog::doExecute(SernaDoc* se, EventData* result)
{
    bool show_details = ed_->getSafeProperty("showDetails")->getBool();

    int rc = RegistrationProgressDialogImpl(se->widget(0),
					    ed_->getProperty("user-info"),
					    show_details).exec();

    return (QDialog::Accepted == rc);
}



#include "moc/RegistrationProgressDialog.moc"
