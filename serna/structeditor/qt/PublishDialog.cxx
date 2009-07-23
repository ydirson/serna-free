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

#include "common/String.h"
#include "common/PathName.h"
#include "common/PropertyTree.h"
#include "common/Url.h"

#include "structeditor/impl/debug_se.h"
#include "structeditor/StructEditor.h"

#include "grove/Grove.h"
#include "grove/Nodes.h"

#include "utils/Config.h"
#include "utils/config_defaults.h"
#include "utils/SernaCatMgr.h"
#include "utils/file_utils.h"
#include "common/PropertyTreeEventData.h"
#include "utils/HelpAssistant.h"
#include "utils/Properties.h"
#include "utils/DocSrcInfo.h"
#include "docutils/doctags.h"

#include <QShortcut>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QTimer>
#include <QDir>
#include <QPixmap>
#include <QTextCodec>
#include <QTemporaryFile>
#include <QDateTime>

#include "structeditor/GenInfoDialogBase.hpp"
#include "structeditor/PublishDialogBase.hpp"

#include <iostream>
#include <map>

////////////////////////////////////////////////////////////////////////////

#if defined(__APPLE__)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

static const char TMPFILE[] = NOTR("publishTempXXXXXX");

#ifdef _WIN32
# define ENVIRON _environ
static const char SCRIPT_EXT[] = NOTR(".bat");
static const Qt::CaseSensitivity PATH_CASE_SENSITIVITY = Qt::CaseInsensitive;
#else
static const Qt::CaseSensitivity PATH_CASE_SENSITIVITY = Qt::CaseSensitive;
# if defined(__sun__)
extern char** _environ;
#  define ENVIRON _environ
# else
#  define ENVIRON environ
# endif
# include <unistd.h>
static const char SCRIPT_EXT[] = NOTR(".sh");
#endif

////////////////////////////////////////////////////////////////////////////
using namespace FileUtils;
using namespace Common;
using namespace GroveLib;

// START_IGNORE_LITERALS
static const char* PUBLISH_INSCRIPTION = "inscription";
static const char* PUBLISH_SCRIPT      = "script";
static const char* PUBLISH_NEEDS_PROLOG= "needs-prolog";
static const char* PUBLISH_STYLE       = "stylesheet";
static const char* PUBLISH_EXT         = "extension";
// STOP_IGNORE_LITERALS

static QTextCodec* get_codec_for_locale()
{
#if defined(_WIN32)
    QTextCodec* codec = 0;
    std::string localeInfo;
    int ret = 64;

    do {
        localeInfo.resize(ret);
        int ret = GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTCODEPAGE,
                                 &localeInfo[0], localeInfo.size());
    } while (ret && localeInfo.size() < ret);
    if (ret) {
        std::string locale(localeInfo, 0, ret - 1);
        std::string name(NOTR("CP") + locale);
        codec = QTextCodec::codecForName(name.c_str());
        if (0 == codec) {
            name.assign(NOTR("IBM")).append(locale);
            codec = QTextCodec::codecForName(name.c_str());
            if (0 == codec)
                codec = QTextCodec::codecForLocale();
        }
    }
    return codec;
#else
    return QTextCodec::codecForLocale();
#endif
}

////////////////////////////////////////////////////////////////////////////

class GenInfoDialog : public QDialog,
                      protected Ui::GenInfoDialogBase {
    Q_OBJECT
public:
    GenInfoDialog(QWidget* parent, const String& caption,
                  const QStringList& argList, const String& dir,
                  QStringList* env, QString* log);
protected slots:
    void    errorSignaled();
    void    readFromStdOut() {
        if (proc_)
            showData(proc_->readAllStandardOutput());
    }
    void    readFromStdErr() {
        if (proc_)
            showData(proc_->readAllStandardError());
    }
    void    launchFinished(int, QProcess::ExitStatus);
    void    reject();
    void    accept();

protected:
    void    showData(QByteArray data);

protected:
    QStringList         argList_;
    QProcess*           proc_;
    COMMON_NS::String   curDir_;
    QTextCodec*         codec_;
    QString*            log_;
};

GenInfoDialog::GenInfoDialog(QWidget* parent, const String& caption,
                             const QStringList& argList, const String& dir,
                             QStringList* env, QString* log)
 :  QDialog(parent),
    argList_(argList),
    proc_(0),
    codec_(get_codec_for_locale()),
    log_(log)
{
    setupUi(this);

    setWindowTitle(caption);
    okButton_->setEnabled(false);
    icon_->setPixmap(
        Sui::icon_provider().getPixmap(NOTR("modified_attribute")));
    warning_->setText(tr("Generating output, please wait..."));

    curDir_ = QDir::currentPath();

    QDir workdir(dir);
    if (!workdir.exists())
        workdir.setPath(DocSrcInfo::myDocumentsPath());
    QDir::setCurrent(workdir.absolutePath());

    proc_ = new QProcess(this);
    proc_->setEnvironment(*env);

    proc_->setWorkingDirectory(workdir.absolutePath());

    connect(proc_, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readFromStdOut()));
    connect(proc_, SIGNAL(readyReadStandardError()),
            this, SLOT(readFromStdErr()));
    connect(proc_, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(launchFinished(int, QProcess::ExitStatus)));

    text_->append(tr("SCRIPT BEGINS:\n"));

    QString cmd = argList_.front();
    argList_.pop_front();
    text_->append(cmd + ' ' + '\'' + argList_.join(NOTR("' '")) + '\'');
    proc_->start(cmd, argList_);
}

void GenInfoDialog::errorSignaled()
{
    warning_->setText(tr("Error..."));
    QDir::setCurrent(curDir_);
    QMessageBox::critical(
        this, tr("Error"), tr("Error launching command:\n\"%1"
                              "\"\nPlease check paths and parameters.").
        arg(argList_.join(QChar(' '))));
    text_->append(argList_.join(QChar(' ')));
    readFromStdOut();
    readFromStdErr();
    QDir::setCurrent(curDir_);
    QDialog::reject();
}

void GenInfoDialog::reject()
{
    warning_->setText(tr("Aborted"));
    QDir::setCurrent(curDir_);
    proc_->terminate();
    QTimer::singleShot(3000, proc_, SLOT(kill()));
    QDialog::reject();
}

void GenInfoDialog::accept()
{
    if (log_)
        *log_ = text_->text();
    QDialog::accept();
}

void GenInfoDialog::showData(QByteArray data)
{
    if (0 == data.size())
        return;
    QString text = codec_->toUnicode(data);
    if (isVisible())
        text_->append(text);
    else
        std::cout << text.local8Bit().data();
}

void GenInfoDialog::launchFinished(int /*exitCode*/,
                                   QProcess::ExitStatus /*status*/)
{
    warning_->setText(tr("Finished."));
    text_->append(tr("SCRIPT ENDS:\n"));
    okButton_->setEnabled(true);
    abortButton_->setEnabled(false);
    QDir::setCurrent(curDir_);
}
////////////////////////////////////////////////////////////////////////////

class PublishDialogImpl : public QDialog,
                          protected Ui::PublishDialogBase {
    Q_OBJECT
public:
    PublishDialogImpl(StructEditor* se, PropertyNode* pinfo);
    virtual ~PublishDialogImpl() {}

protected slots:
    void    on_browseToButton__clicked();
    void    on_browseScriptButton__clicked();
    void    on_browseStyleButton__clicked();
    void    on_generateButton__clicked();
    void    on_viewButton__clicked();

    void    on_outputTypeCombo__currentIndexChanged();
    void    on_fileLineEdit__textChanged();
    void    on_scriptLineEdit__textChanged();

    void    on_styleLineEdit__textChanged()
    {
        methodProperty(PUBLISH_STYLE)->setString(styleLineEdit_->text());
    }

    void    on_helpButton__clicked()
    {
        helpAssistant().show(DOCTAG(UG_PUBLISH_D));
    }

private:
    PropertyNode*   methodProperty(const String& name)
    {
        return currentMethod_->makeDescendant(name);
    }

    String          resolveStylePath(const String&) const;
    void            adjustExtension(const QString& file);
    QString         getScriptFullPath(const QString& path);
    void            checkGenerateButtonState();

private:
    StructEditor*   se_;
    PropertyNode*   publishInfo_;
    PropertyNode*   currentMethod_;
    String          src_;
    String          instPath_;
    bool            scriptChanged_;
    bool            tempSaved_;
    QString         outFile_;
    QTemporaryFile  tempFile_;
};

static const char pub_method_prop[] = NOTR("#publish-output-method");

PublishDialogImpl::PublishDialogImpl(StructEditor* se, PropertyNode* pinfo)
 :  QDialog(se->widget(0)),
    se_(se),
    publishInfo_(pinfo),
    currentMethod_(0),
    src_(se->grove()->topSysid()),
    instPath_(QDir::toNativeSeparators(config().getDataDir())),
    scriptChanged_(false),
    tempSaved_(false)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
            SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));

    DBG_IF(SE.TEST) publishInfo_->dump();

    String last_method =
        se->getDsi()->getSafeProperty(pub_method_prop)->getString();
    PropertyNode* method = publishInfo_->firstChild();
    int last_method_index = -1;
    for (int i = 0; method; i++, method = method->nextSibling()) {
        if (NOTR("method") != method->name())
            continue;
        if (method->getSafeProperty(PUBLISH_INSCRIPTION)->getString() ==
            last_method)
                last_method_index = i;
        outputTypeCombo_->insertItem(
            method->makeDescendant(PUBLISH_INSCRIPTION, "unknown", false)->
            getString());
    }
    if (last_method_index >= 0)
        outputTypeCombo_->setCurrentItem(last_method_index);

    Url srcUrl(src_);
    QDir tmpDir;
    QFileInfo srcFi(src_);
    if (srcUrl.isLocal()) {
        tmpDir.setPath(srcFi.absolutePath());
        QTemporaryFile tmpFile(tmpDir.absoluteFilePath(NOTR("probeXXX")));
        if (!tmpFile.open())
            tmpDir = QDir::tempPath();
    }
    else
        tmpDir = QDir::tempPath();

    QString baseName(TMPFILE);
    baseName.append('.').append(srcFi.suffix());
    tempFile_.setFileTemplate(tmpDir.absoluteFilePath(baseName));
    tempFile_.open();

    on_outputTypeCombo__currentIndexChanged();  //to set Currentmethod_
    adjustExtension(to_string<QString>(srcUrl[Url::FILENAME]));
    setFixedHeight(height());
}

void PublishDialogImpl::on_outputTypeCombo__currentIndexChanged()
{
    currentMethod_ = 0;
    PropertyNode* method = publishInfo_->firstChild();
    for (; method; method = method->nextSibling()) {
        if (NOTR("method") != method->name())
            continue;
        if (outputTypeCombo_->currentText() ==
            method->makeDescendant(PUBLISH_INSCRIPTION)->getString()) {
            currentMethod_ = method;
            break;
        }
    }

    if (0 == currentMethod_)
        currentMethod_ = publishInfo_->makeDescendant(NOTR("method"));
    String script = methodProperty(PUBLISH_SCRIPT)->getString();
    if (scriptLineEdit_->text().isEmpty() || !scriptChanged_) {
        if (!script.isEmpty())
            scriptLineEdit_->setText(script + SCRIPT_EXT);
        scriptChanged_ = false;
    }
    String style = methodProperty(PUBLISH_STYLE)->getString();
    if (!style.isEmpty())
        styleLineEdit_->setText(style);
    adjustExtension(fileLineEdit_->text());
    se_->getDsi()->makeDescendant(pub_method_prop,
        outputTypeCombo_->currentText(), true);
}

void PublishDialogImpl::adjustExtension(const QString& file)
{
    if (file.isEmpty())
        return;
    // Append appropriate suffix to the output file
    QString outPath(file);
    QString ext(methodProperty(PUBLISH_EXT)->getString());
    if (ext.isEmpty())
        ext = NOTR("out");

    QFileInfo fi(file);
    QString srcExt(fi.suffix().toLower());
    if (srcExt.isEmpty() || ext.toLower() != srcExt)
        outPath = fi.completeBaseName() + QChar('.') + ext;

    fileLineEdit_->setText(outPath);
}

QString PublishDialogImpl::getScriptFullPath(const QString& path)
{
    QString scriptPath = QDir::convertSeparators(path);
    if (!scriptPath.isEmpty() && 0 > scriptPath.find(QDir::separator()))
        scriptPath.insert(0, QString(instPath_) + NOTR("/utils/publishing/"));
    else
        scriptPath = resolveStylePath(scriptPath);
    return scriptPath;
}

void PublishDialogImpl::checkGenerateButtonState()
{
    QString script = scriptLineEdit_->text();
    const bool isEnabled = !fileLineEdit_->text().isEmpty() &&
                           !script.isEmpty() &&
                           QFile::exists(getScriptFullPath(script));
    generateButton_->setEnabled(isEnabled);
}


void PublishDialogImpl::on_scriptLineEdit__textChanged()
{
    scriptChanged_ = true;
    QString text = scriptLineEdit_->text();
    int extension_start = text.findRev('.');
    if (-1 != extension_start) {
        text = text.left(extension_start);
    }
    methodProperty(PUBLISH_SCRIPT)->setString(text);
    checkGenerateButtonState();
}

void PublishDialogImpl::on_fileLineEdit__textChanged()
{
    methodProperty(PUBLISH_EXT)->setString(
        QFileInfo(fileLineEdit_->text()).suffix());
    checkGenerateButtonState();
}

static String fix_output_file(const QFileInfo& outFi, const String& tmpFileName,
                              const QString& log, const String& pubMethod)
{
// START_IGNORE_LITERALS
    QString outDir(outFi.absolutePath());
    if (!outDir.endsWith(QChar('\\')) && !outDir.endsWith(QChar('/')))
        outDir += '/';
    outDir = QDir::toNativeSeparators(outDir);
    QString outExt(outFi.suffix());

    QFileInfo tmpFi(tmpFileName);
    QString tmpExt(tmpFi.suffix());
    QString tmpBase(tmpFi.completeBaseName());

    QString ditaReStr(QString::fromLatin1("\\s+\\[(?:fop|xslt)\\]"
                                          "[^\\n\\r]*\\ "));
    ditaReStr += QString::fromLatin1("(?:%1)?[^\\n\\r\\>]* (?:->|to) ");
    ditaReStr += QString::fromLatin1("(.*\\%2+%3\\.(?:%4\\.)?%5)");
    ditaReStr = ditaReStr.arg(QRegExp::escape(outDir));
    ditaReStr = ditaReStr.arg(PathName::DIR_SEP).arg(tmpBase);
    if ("pdf" == pubMethod)
        outExt = "(" + outExt + "|fo)";

    ditaReStr = ditaReStr.arg(tmpExt).arg(outExt);
    QRegExp ditaRe(ditaReStr);
    ditaRe.setMinimal(true);

#if defined(_WIN32) || defined(__APPLE__)
    ditaRe.setCaseSensitivity(Qt::CaseInsensitive);
#else
    ditaRe.setCaseSensitivity(Qt::CaseSensitive);
#endif

    if (-1 != ditaRe.searchRev(log)) {
        tmpFi.setFile(ditaRe.cap(1));
        if (tmpFi.fileName() == outFi.fileName())
            return tmpFi.absoluteFilePath();

        QString tmpName(tmpFi.completeBaseName());
        tmpName.append('.').append(outFi.suffix());
        tmpFi.setFile(tmpFi.dir(true), tmpName);

        if (tmpFi.exists()) {
            QDir dstDir(tmpFi.dir(true));
            QString outFile(outFi.fileName());
            if (dstDir.exists(outFile));
                dstDir.remove(outFile);
            if (dstDir.rename(tmpFi.fileName(), outFile))
                return dstDir.absFilePath(outFile);
        }
    }
    return String();
// STOP_IGNORE_LITERALS
}

static void modify_links(GroveLib::Node* node, const QDir& dstDir,
                         const String& prefixDir)
{
    GroveLib::Element* elem = dynamic_cast<GroveLib::Element*>(node);
    if (0 == elem)
        return;
    GroveLib::Attr* href = elem->attrs().getAttribute(NOTR("href"));
    if (href && !QFileInfo(dstDir, href->value()).exists())
        href->setValue(prefixDir + href->value());
    for(GroveLib::Node* n = elem->firstChild(); n; n = n->nextSibling())
        modify_links(n, dstDir, prefixDir);
}

static QString get_fs_path(const String& path)
{
    Url url(path);
    if (url.isLocal())
        return QDir::convertSeparators(path);
    return String(url);
}

typedef QMap<QString, QString> Env;

static void add_to_env(Env& env, const char* name, const QString& value)
{
    env.insert(QString::fromLocal8Bit(name).trimmed(),
               get_fs_path(value).trimmed());
}

static void prepare_env(Env& envMap, const Vector<String>& xmlCats)
{
    // START_IGNORE_LITERALS
    const PropertyNode* varProp =
        config().root()->getSafeProperty("vars")->firstChild();
    QString varName, varValue;
    for (; varProp; varProp = varProp->nextSibling()) {
        static const QString serna_pfx(QString::fromLatin1("SERNA_"));
        varName = varProp->name();
        varName = varName.toUpper().trimmed();
        varValue = varProp->getString();
        envMap.insert(varName.prepend(serna_pfx), varValue.trimmed());
    }

    for (const char* const* p = ENVIRON; 0 != *p; ++p) {
        QString es = QString::fromLocal8Bit(*p);
        int eqPos = es.find('=');
        if (0 >= eqPos)
            continue;
        varValue = es.mid(eqPos + 1).trimmed();
        if (es.startsWith("PATH=", PATH_CASE_SENSITIVITY)) {
            QFileInfo fi(argv0());
            QString binPath(get_fs_path(fi.absolutePath()));
            if (!varValue.isEmpty())
                binPath += QChar(PathName::PATH_SEP) + varValue;
            varValue = binPath;
        }
        else {
            if (varValue.isEmpty())
                continue;
        }
        varName = es.left(eqPos).trimmed();
        envMap.insert(varName, varValue);
    }

    if (0 < xmlCats.size()) {
        QStringList cats;
        const QString space(QChar(' '));
        Vector<String>::const_iterator it = xmlCats.begin();
        for (QString cat; xmlCats.end() != it; cat = *it, ++it) {
            cat = cat.stripWhiteSpace();
            if (cat.isEmpty())
                continue;
            cat.replace(space, "%20");
#if defined(WIN32)
            cat.replace(QChar('\\'), QChar('/'));
            cat.replace(QRegExp("file:///([A-Za-z]):"), "file:///\\1%3A");
            cat.replace(QRegExp("^([A-Za-z]):"), "file:///\\1%3A");
#endif
            cats.push_back(cat);
        }
        envMap.insert(QString::fromLatin1("XML_CATALOG_FILES"),
                      cats.join(space));
    }
    // STOP_IGNORE_LITERALS
}

void PublishDialogImpl::on_generateButton__clicked()
{
    QString scriptPath(getScriptFullPath(scriptLineEdit_->text()));

    if (!QFile::exists(scriptPath)) {
        QMessageBox::critical(this, tr("Error"),
                     tr("Script file \"%1\" does not exist.").arg(scriptPath));
        return;
    }
    QFileInfo tmpFi(tempFile_);
    QString tempFileName(tmpFi.absFilePath());
    if (!tempSaved_) {
        int saveFlags = Grove::GS_SAVE_CONTENT |
                        Grove::GS_EXPAND_ENTITIES |
                        Grove::GS_SAVE_DEFATTRS |
                        Grove::GS_INDENT |
                        Grove::GS_FORCE_SAVE;

        if (methodProperty(PUBLISH_NEEDS_PROLOG)->getBool())
            saveFlags |= Grove::GS_SAVE_PROLOG;

        GrovePtr grove(se_->grove());
        if (src_.left(5).lower() == NOTR("http:")) { //Webdav
            GroveBuilder* gb = se_->grove()->groveBuilder()->copy();
            grove = gb->buildGroveFromFile(tempFileName);
            Node* node = grove->document()->documentElement();
            modify_links(node, QDir(QFileInfo(tempFile_).absoluteDir()),
                         src_.left(src_.qstring().findRev("/") + 1));
        }
        tempSaved_ = grove->saveAsXmlFile(saveFlags, se_->stripInfo(),
                                          tempFileName);
    }
    if (!tempFile_.exists()) {
        QMessageBox::critical(this, tr("Error"), tr("Temporary xml file \"%1"
                               "\" could not be created.").arg(tempFileName));
        tempSaved_ = false;
        return;
    }
// START_IGNORE_LITERALS
    QStringList cmd_list;
    //! 0. shell/bat script path
    cmd_list.push_back(get_fs_path(scriptPath));

    Env envMap;
    prepare_env(envMap, se_->uriMapper()->getCatalogsList());

    //! 1. input file absolute path (temporarily saved original)
    add_to_env(envMap, "SERNA_XML_SRCFULLPATH", tmpFi.absoluteFilePath());
    add_to_env(envMap, "SERNA_XML_SRCFILENAME", tmpFi.fileName());
    add_to_env(envMap, "SERNA_XML_TOPSYSID",    se_->grove()->topSysid());

    //! 2. stylesheet path
    String style = styleLineEdit_->text();
    add_to_env(envMap, "SERNA_XSL_STYLESHEET", resolveStylePath(style));
    //! Output filename
    QFileInfo outFi(fileLineEdit_->text());
    if (outFi.isRelative()) {
        QDir srcDir(QFileInfo(src_).absolutePath());
        outFi.setFile(srcDir, outFi.filePath());
    }
    //! 3. output file absolute path
    add_to_env(envMap, "SERNA_OUTPUT_FILE", outFi.absFilePath());
    add_to_env(envMap, "SERNA_OUTPUT_DIR", outFi.dirPath(true));
    //! 4. SERNA_DATA_DIR
    if (0 == getenv("SERNA_DATA_DIR"))
        add_to_env(envMap, "SERNA_DATA_DIR", instPath_);
    //! 5. output file extension
    add_to_env(envMap, "SERNA_OUTPUT_EXT",
               methodProperty(PUBLISH_EXT)->getString());

    OwnerPtr<QString> log;
    String category(se_->getDsi()->getSafeProperty("category")->getString());
    const bool isDita = starts_with(category, "DITA ");
    if (isDita)
        log.reset(new QString);
// STOP_IGNORE_LITERALS

    QStringList env;
    for (Env::const_iterator it = envMap.begin(); it != envMap.end(); ++it)
        env.push_back(it.key() + QChar('=') + it.data());

    GenInfoDialog gi(this, tr("Generating %1 ...").
                           arg(outputTypeCombo_->currentText()),
                     cmd_list, QFileInfo(tempFile_).absolutePath(),
                     &env, &*log);
    gi.exec();
    if (isDita) {
        outFile_ = fix_output_file(outFi, tmpFi.fileName(), *log,
                                   methodProperty(NOTR("name"))->getString());
        if (!outFile_.isEmpty())
            fileLineEdit_->setText(outFile_);
    }
    else {
        outFi.setFile(fileLineEdit_->text());
        if (outFi.isRelative()) {
            QDir outDir(tmpFi.absolutePath());
            outFi.setFile(outDir, outFi.fileName());
        }
        outFile_ = outFi.absoluteFilePath();
    }
}

void PublishDialogImpl::on_viewButton__clicked()
{
    QFileInfo file_info(outFile_);
    if (outFile_.isEmpty() || !file_info.exists()) {
        QMessageBox::critical(
            this, tr("Error"),
            tr("Output file does not exist :\n\"%1\"").arg(
                QString(file_info.absFilePath())));
        return;
    }

    QString errMsg;
    using namespace FileUtils;

    String extension(file_info.suffix());
    QString path(QDir::toNativeSeparators(outFile_));
    LaunchCode code = launch_file_handler(path, extension);
    switch (code) {
        case HANDLER_OK:
            return;
        case HANDLER_UNDEF:
            errMsg = tr("Viewer application is not defined for extension \"%1"
               "\".\nPlease check the Preferences->Applications tab.");
            errMsg = errMsg.arg(extension);
            break;
        default:
            errMsg = tr("Viewer application for \n\"%1\""
                        "\ncould not be started\n"
                        "Please check the corresponding viewer path in "
                        "Preferences->Applications tab.").arg(path);
            break;
    }
    QMessageBox::critical(this, tr("Error"), errMsg);
}

void PublishDialogImpl::on_browseToButton__clicked()
{
    QString path = fileLineEdit_->text();
    QFileInfo fi(path);
    if (path.isEmpty() || !fi.dir(true).exists()) {
        path = get_default_doc_dir();
//TODO: get PI or set to My documents or /home for linux
        path = QDir::homePath();
    }
    else if (0 > path.find(QDir::separator()))
        path = QFileInfo(src_).dirPath(true) + QDir::separator() + path;
    QString filter = tr("%1 (*.%2);;All files (*)").
        arg(methodProperty(PUBLISH_INSCRIPTION)->getString()).
        arg(methodProperty(PUBLISH_EXT)->getString().lower());
    QString input = QFileDialog::getSaveFileName(path, filter, this,
                                                 tr("publish to file"),
                                                 tr("Choose output file"));
    if (input.isEmpty())
        return;
    fileLineEdit_->setText(QDir::convertSeparators(input));
    fileLineEdit_->setFocus();
}

void PublishDialogImpl::on_browseScriptButton__clicked()
{
    QString path = getScriptFullPath(scriptLineEdit_->text());
    if (path.isEmpty())
        path = QDir::homePath();
    path = QDir::convertSeparators(path);

    QString filter;
#ifdef WIN32
    filter = tr("Bat files (*.bat;*.cmd)");
#else
    filter = tr("Shell files (*.sh)");
#endif
    filter += tr(";;All files (*)");

    QString input = QFileDialog::getOpenFileName(path, filter, this,
                                                 NOTR("open file dialog"),
                                                 tr("Choose script"));
    if (!input.isEmpty()) {
        scriptLineEdit_->setText(QDir::convertSeparators(input));
        scriptLineEdit_->setFocus();
    }
}

void PublishDialogImpl::on_browseStyleButton__clicked()
{
    String path = styleLineEdit_->text();
    if (path.isEmpty())
        path =  QDir::homePath();
    else
        path = resolveStylePath(path);
    QString filter = tr("XSL files (*.xsl);;All files (*)");
    QString input = QFileDialog::getOpenFileName(path, filter, this,
                                                 NOTR("open file dialog"),
                                                 tr("Choose stylesheet"));
    if (!input.isEmpty()) {
        styleLineEdit_->setText(QDir::convertSeparators(input));
        styleLineEdit_->setFocus();
    }
}

String PublishDialogImpl::resolveStylePath(const String& path) const
{
    if (path.empty())
        return path;
    String tmplpath =
        se_->getDsi()->getSafeProperty(DocSrcInfo::TEMPLATE_PATH)->getString();
    PropertyTree localVars;
    if (!tmplpath.isEmpty())
        localVars.root()->makeDescendant(DocSrcInfo::TEMPLATE_DIR,
            Url(tmplpath)[Url::DIRPATH], true);
    String script = config().resolveResource(NOTR("doc-defaults/stylesheet"),
                                             path, src_, localVars.root());
    return script;
}

////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(PublishDialog, StructEditor)

bool PublishDialog::doExecute(StructEditor* se, EventData*)
{
    if (!ed_ || 0 == ed_->getProperty("method")) {
        QMessageBox::warning(qApp->mainWidget(), tr("Warning"),
                             tr("Document template has no publishing hints.\n"
                                "Please fill template's publish section."));
        ed_->makeDescendant("method");
    }

    PublishDialogImpl(se, ed_).exec();
    return true;
}

////////////////////////////////////////////////////////////////////////

#include "moc/PublishDialog.moc"
