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
// Copyright (c) 2006 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "utils/utils_defs.h"
#include "utils/utils_debug.h"
#include "utils/i18n_utils.h"
#include "utils/Config.h"
#include "utils/Properties.h"

#include "common/String.h"
#include "common/StringCvt.h"
#include "common/PathName.h"
#include "common/PropertyTree.h"

#include <QTranslator>
#include <QApplication>
#include <QTextCodec>

using namespace Common;

Common::String get_translations_dir()
{
    String trPropName(NOTR("app/"));
    trPropName.append(App::TRANSLATIONS_PATH);
    PathName trDir(config().getProperty(trPropName)->getString());
    if (!trDir.exists()) {
        trDir = config().getDataDir();
        trDir.append(NOTR("translations"));
        if (!trDir.exists())
            return String();
    }
    return trDir.name();
}

class Translator {
public:
    Translator(const char* prefix) : prefix_(prefix), trans_(0) {}
    QTranslator* load(const String& trans, const String& transDir);
    void install(QTranslator* newTrans)
    {
        DDBG << "Translator::install() '" << newTrans << '\'' << std::endl;
        if (trans_) {
            DDBG << "removing translator '" << trans_ << '\'' << std::endl;
            qApp->removeTranslator(trans_);
            delete trans_;
        }
        trans_ = newTrans;
        if (trans_) {
            DDBG << "installing translator '" << trans_ << '\'' << std::endl;
            qApp->installTranslator(trans_);
        }
    }
private:
    nstring      prefix_;
    QTranslator* trans_;
};

QTranslator* Translator::load(const String& trans, const String& transDir)
{
    String transBase(prefix_.begin(), prefix_.end());
    transBase.append(1, '_').append(trans);
    DDBG << "translation dir = '" << transDir << '\'' << std::endl
         << "translation basename = '" << transBase << '\'' << std::endl;

    std::auto_ptr<QTranslator> newTrans(new QTranslator(0));
    if (!newTrans->load(to_string<QString>(transBase),
                        to_string<QString>(transDir), QChar('_')))
        return 0;
    DDBG << "translation loaded, translator = " << &*newTrans << std::endl;

    return newTrans.release();
}

void load_translation(const String& trans)
{
    static Translator sernaTr(NOTR("serna")),
                      qtTr(NOTR("qt")),
                      qscTr(NOTR("qscintilla"));

    DDBG << "loading translation '" << trans << '\'' << std::endl;

    Translator* translators[] = { &sernaTr, &qtTr, &qscTr };
    const int numTrans = sizeof(translators)/sizeof(translators[0]);
    std::auto_ptr<QTranslator> newTranses[numTrans];

    const bool isDefaultLang = trans.empty() || NOTR("default") == trans;
    String langId(isDefaultLang ? QTextCodec::locale() : trans);
    if (!langId.empty()) {
        PathName transDir(get_translations_dir());
        for (int i = 0; i < numTrans; ++i)
            newTranses[i].reset(translators[i]->load(langId, transDir.name()));
    }
    for (int i = 0; i < numTrans; ++i)
        translators[i]->install(newTranses[i].release());
}
