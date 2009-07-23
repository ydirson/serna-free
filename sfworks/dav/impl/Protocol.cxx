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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "dav/Protocol.h"
#include "dav/DavManager.h"
#include "dav/Session.h"
#include "dav/impl/FileProtocol.h"
#include "common/OwnerPtr.h"
#include "common/PropertyTree.h"
#include "common/file_utils.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define W_OK 2
# define R_OK 4
#endif // _WIN32

namespace Dav {

static bool access(const QFileInfo& fi, int test)
{
    QString path(fi.absFilePath());
#if defined(WIN32)
    const wchar_t* wpath = reinterpret_cast<const wchar_t*>(path.ucs2());
    const DWORD attrs = GetFileAttributes(reinterpret_cast<LPCSTR>(wpath));
    if (INVALID_FILE_ATTRIBUTES == attrs) {
        DWORD mask = 0;
        mask |= ((test & R_OK) ?  GENERIC_READ : 0);
        mask |= ((test & W_OK) ?  GENERIC_WRITE : 0);
        mask |= (mask ? 0 : FILE_ALL_ACCESS);
        DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        DWORD attrs = fi.isDir() ? FILE_FLAG_BACKUP_SEMANTICS : 0;
        HANDLE h = CreateFileW(wpath, mask, share, 0, OPEN_EXISTING, attrs, 0);
        if (INVALID_HANDLE_VALUE == h)
            return false;
        CloseHandle(h);
        return true;
    }
    return 0 == ::_waccess(wpath, test);
#else
    return 0 == ::access(QFile::encodeName(path), test);
#endif
}

static bool is_readable(const QFileInfo& fi)
{
    return access(fi, R_OK);
}

static bool is_readonly_file(const QFileInfo& fi)
{
    return !access(fi, W_OK);
}

Protocol::~Protocol()
{
    DavManager::instance().deregisterProtocol(this);
}

///////////////////////////////////////////////////////////////////

IoRequestHandle::IoRequestHandle(Session* session)
    : session_(session)
{
}

IoRequestHandle::~IoRequestHandle()
{
}

///////////////////////////////////////////////////////////////////

static void dav_error(const String& errmsg)
{
    DavManager::instance().setLastError(errmsg);
}

class FileIoHandle : public IoRequestHandle {
public:
    FileIoHandle(Session* session, QFile* file)
        : IoRequestHandle(session), file_(file) {}

    virtual OpStatus    readRaw(uint n,  char* buf, uint& readBytes)
    {
        long nread = file_->readBlock(buf, n);
        if (nread < 0) {
            readBytes = 0;
            dav_error("read error");
            return DAV_RESULT_IO_ERROR;
        }
        readBytes = nread;
        return DAV_RESULT_OK;
    }
    virtual OpStatus    writeRaw(uint n, const char* buf, uint& writtenBytes)
    {
        long nwrite = file_->writeBlock(buf, n);
        if (nwrite < 0) {
            writtenBytes = 0;
            dav_error("write error");
            return DAV_RESULT_IO_ERROR;
        }
        writtenBytes = nwrite;
        return DAV_RESULT_OK;
    }
    virtual OpStatus    close()
    {
        file_->close();
        return DAV_RESULT_OK;
    }
    virtual uint        position() const
    {
        return file_->at();
    }
    virtual OpStatus    setPosition(uint n)
    {
        if (file_->at(n))
            return DAV_RESULT_OK;
        dav_error("cannot set file position");
        return DAV_RESULT_IO_ERROR;
    }
    virtual uint        size() const
    {
        return file_->size();
    }
    ~FileIoHandle()
    {
        close();
    }

private:
    OwnerPtr<QFile> file_;
};

class FileSession : public Session {
public:
    OpStatus    open(UrlRef url, int mode, IoRequestHandle**);
    OpStatus    listCollection(UrlRef url,
                               Common::PropertyNode* to);
    OpStatus    makeCollection(UrlRef url);
    OpStatus    getResourceInfo(UrlRef url, Common::PropertyNode* to);
    OpStatus    lock(UrlRef url, LockOp op, int key);
};

#define CHECK_FILE_PROTOCOL \
    if (url[Url::PROTOCOL] != "file") return DAV_RESULT_BAD_PROTOCOL;

OpStatus FileSession::open(UrlRef url, int mode, IoRequestHandle** handle)
{
    CHECK_FILE_PROTOCOL;
    OwnerPtr<QFile> file(new QFile(url[Url::PATH]));
    int qmode = 0;
    if (mode & DAV_OPEN_READ)
        qmode |= QIODevice::ReadOnly;
    if (mode & DAV_OPEN_WRITE)
        qmode |= QIODevice::WriteOnly|QIODevice::Truncate;
    if (mode & DAV_OPEN_MKPATH) {
        QFileInfo file_info(url[Url::PATH]);
        QDir dir(file_info.dirPath(true));
        if (!dir.exists() && !mkdirp(dir.absPath())) {
            dav_error("cannot create path");
            return DAV_RESULT_CANNOT_OPEN;
        }
    }
    if (!file->open((QIODevice::OpenModeFlag)qmode)) {
        dav_error("cannot open for " +
            String((mode & DAV_OPEN_WRITE) ? "writing" : "reading"));
        return DAV_RESULT_CANNOT_OPEN;
    }
    *handle = new FileIoHandle(this, file.release());
    return DAV_RESULT_OK;
}

OpStatus FileSession::listCollection(UrlRef url,
                                     Common::PropertyNode* to)
{
    CHECK_FILE_PROTOCOL;
    QDir dir(url[Url::PATH]);
    if (!dir.exists() || !is_readable(dir.absPath())) {
        dav_error("directory does not exist or unreadable");
        return DAV_RESULT_CANNOT_OPEN;
    }
    QFileInfoList info_list = dir.entryInfoList();
    if (info_list.isEmpty())
        return DAV_RESULT_OK;
    
    QFileInfoList::iterator fi = info_list.begin();
    for (; fi != info_list.end(); ++fi) {
        if (fi->isDir() && (fi->fileName() == "." || fi->fileName() == ".."))
            continue;
        PropertyNode* pn = new PropertyNode(fi->fileName());
        pn->appendChild(new PropertyNode("size", 
            String::number(ulong(fi->size()))));
        if (fi->isDir())
            pn->appendChild(new PropertyNode("is_collection"));
        if (is_readonly_file(*fi))
            pn->appendChild(new PropertyNode("is_read_only", "true"));
        int ttime = fi->lastModified().toTime_t();
        pn->appendChild(new PropertyNode("modtime", ttime));
        to->appendChild(pn);
    }
    return DAV_RESULT_OK;
}

OpStatus FileSession::makeCollection(UrlRef url)
{
    CHECK_FILE_PROTOCOL;
    QDir d;
    if (!mkdirp(url[Url::PATH]))
        return DAV_RESULT_CANNOT_OPEN;
    return DAV_RESULT_OK;
}

OpStatus FileSession::getResourceInfo(UrlRef url,
                                      Common::PropertyNode* to)
{
    CHECK_FILE_PROTOCOL;
    QFileInfo fi(url[Url::PATH]);
    if (!fi.exists() || !is_readable(fi))
        return DAV_RESULT_CANNOT_OPEN;
    to->setName(fi.absFilePath());
    if (fi.isDir())
        to->makeDescendant("is_collection");
    else
        to->makeDescendant("size")->setInt(fi.size());
    unsigned long t = fi.lastModified().toTime_t();
    to->makeDescendant("modtime")->setString(String::number(t));
    if (is_readonly_file(fi))
        to->makeDescendant("is_read_only")->setBool(true);
    return DAV_RESULT_OK;
}

OpStatus FileSession::lock(UrlRef url, LockOp op, int)
{
    Common::PropertyTree pt;
    OpStatus status = getResourceInfo(url, pt.root());
    if (status != DAV_RESULT_OK)
        return status;
    if (pt.root()->getSafeProperty("is_read_only")->getBool())
        return (op == DAV_CHECK_LOCK) ?
            DAV_RESULT_LOCKED : DAV_RESULT_CANNOT_OPEN;
    return DAV_RESULT_OK;
}

OpStatus FileProtocol::makeSession(UrlRef, SessionPtr& session) const
{
    session = new FileSession;
    return DAV_RESULT_OK;
}

} // namespace Dav
