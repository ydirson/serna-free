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
#include "utils/Config.h"
#include "utils/Properties.h"
#include "ui/UiPropertySyncher.h"

#include "RegistrationDialogBase.hpp"
#include <QValidator>

using namespace Common;

class RegistrationProgressDialog;

#define EMAIL_REGEXP "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}$"


class RegistrationDialogImpl : public QDialog,
			       public Ui::RegistrationDialogBase {
    Q_OBJECT

public:
    RegistrationDialogImpl(QWidget* parent, SernaDoc* se);

private:
    SernaDoc* doc_;
    QRegExp emailRegExp_;
    Sui::PropertySyncherPtr dontShowSyncher_;

private slots:
    virtual void requiredFieldChanged();
    virtual void registerLater();
    virtual void registerImmediately();

};


RegistrationDialogImpl::RegistrationDialogImpl(QWidget* parent, SernaDoc* se)
    : QDialog(parent), doc_(se), emailRegExp_(EMAIL_REGEXP)
{
    setupUi(this);

    sendButton_->setEnabled(false);

    //! validate input email on-fly using regular expression
    emailLineEdit_->setValidator(new QRegExpValidator(emailRegExp_, this));

    connect(laterButton_, SIGNAL(clicked()), this, SLOT(registerLater()));
    connect(sendButton_, SIGNAL(clicked()), this, SLOT(registerImmediately()));
    connect(firstnameLineEdit_, SIGNAL(textChanged(const QString&)),
	    this, SLOT(requiredFieldChanged()));
    connect(surnameLineEdit_, SIGNAL(textEdited(const QString&)),
	    this, SLOT(requiredFieldChanged()));
    connect(emailLineEdit_, SIGNAL(textEdited(const QString&)),
	    this, SLOT(requiredFieldChanged()));

    PropertyNode* pinfo = config().root()->
	makeDescendant(Registration::REGISTRATION);
    dontShowSyncher_ = new Sui::ButtonSyncher(pinfo->makeDescendant(
        Registration::DONT_SHOW_ON_START), notShowDialogCheckBox_);
}

void RegistrationDialogImpl::requiredFieldChanged()
{
    QString firstname = firstnameLineEdit_->text().simplified();
    QString surname = surnameLineEdit_->text().simplified();
    QString email = emailLineEdit_->text().simplified();

    bool are_all_filled = !firstname.isEmpty() &&
  	                  !surname.isEmpty() && !email.isEmpty();

    sendButton_->setEnabled(are_all_filled && emailRegExp_.exactMatch(email));
}

void RegistrationDialogImpl::registerLater()
{
    reject();
}

void RegistrationDialogImpl::registerImmediately()
{
    PropertyTreeEventData ptree;
    PropertyNode* user_info = new PropertyNode("user-info");
    ptree.root()->appendChild(user_info);

    user_info->makeDescendant("firstname",
			      firstnameLineEdit_->text());
    user_info->makeDescendant("surname",
			      surnameLineEdit_->text());
    user_info->makeDescendant("email",
			      emailLineEdit_->text());
    user_info->makeDescendant("company",
			      companyLineEdit_->text().simplified());
    user_info->makeDescendant("subscribeNews", "")->
	setBool(subscribeNewsCheckBox_->isChecked());

    ptree.root()->makeDescendant("showDetails", "")->
	setBool(showSentCheckBox_->isChecked());

     if (makeCommand<RegistrationProgressDialog>(&ptree)->execute(doc_))
	 accept();
}

/////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(RegistrationDialog, SernaDoc)

bool RegistrationDialog::doExecute(SernaDoc* se, EventData*)
{
    return (QDialog::Accepted == 
        RegistrationDialogImpl(se->widget(0), se).exec());
}

#include "moc/RegistrationDialog.moc"
