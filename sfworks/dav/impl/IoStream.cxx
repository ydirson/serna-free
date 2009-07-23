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

#include "dav/IoStream.h"
#include "dav/Session.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"
#include "common/Encodings.h"

#include <QTextCodec>

static const uint IOSTREAM_STRBUFSIZE = 8192;

using namespace Common;

static inline String to_charent(const Char c)
{
    return "&#" + String::number(uint(c.unicode())) + ';';
}

static void init_conv_state(QTextCodec::ConverterState* state, bool ignoreHdr)
{
    state->flags = QTextCodec::DefaultConversion;
    state->remainingChars = state->invalidChars =
        state->state_data[0] = state->state_data[1] = state->state_data[2] = 0;
    if (state->d) 
        qFree(state->d);
    state->d = 0; 
    if (ignoreHdr)
        state->flags |= QTextCodec::IgnoreHeader;
}

namespace Dav {

class IoStream::Impl : public RefCounted<> {
public:
    Impl(IoRequestHandle* handle);
    Impl(String& s);
    ~Impl() {}

    void            setEncoding(int);
    void            write_string(const String& s);
    void            write_char(const Char);
    String          read_string();
    bool            read_char(Char&);
    void            check_out_buffer(uint charsNeeded);
    void            flush_out_buffer();
    bool            fill_buffer();

private:
    friend class IoStream;
    int          check_unicode_header(const char* buf, int bytesRead);
    
    OwnerPtr<IoRequestHandle>   iohandle_;
    String                      outBufferStr_;
    String&                     outBuffer_;
    String                      inBuffer_;
    OpStatus                    status_;
    QTextCodec*                 codec_;
    uint                        inBufferPos_;
    QTextCodec::ConverterState  readConverterState_;
    QTextCodec::ConverterState  writeConverterState_;
    bool                        addCr_;
    bool                        atEof_;
    bool                        encodeCharEnt_;
    bool                        rawUnicode_;
    bool                        check_unicode_;
    bool                        outBufferIsSet_;
};

////////////////////////////////////////////////////////////////////

IoStream::Impl::Impl(IoRequestHandle* handle)
        : iohandle_(handle),
          outBuffer_(outBufferStr_),
          status_(DAV_RESULT_OK),
          inBufferPos_(0),
          addCr_(false), atEof_(false),
          encodeCharEnt_(false), 
          rawUnicode_(false),
          check_unicode_(false),
          outBufferIsSet_(false)
{
    setEncoding(Encodings::UTF_8);
}

IoStream::Impl::Impl(String& os)
        : outBuffer_(os),
          status_(DAV_RESULT_OK),
          inBufferPos_(0),
          addCr_(false), atEof_(false),
          encodeCharEnt_(false), 
          rawUnicode_(false),
          check_unicode_(false),
          outBufferIsSet_(true)
{
}
    
void IoStream::Impl::write_string(const String& s)
{
    if (addCr_ || encodeCharEnt_) {
        check_out_buffer(s.length() * 2);
        const bool test_enc = encodeCharEnt_ && !rawUnicode_;
        const Char* cp = s.unicode();
        const Char* ce = cp + s.length();
        for (; cp < ce; ++cp) {
            if (*cp == '\n' && addCr_)
                outBuffer_ += '\r';
            if (test_enc && !codec_->canEncode(*cp)) 
                outBuffer_ += to_charent(*cp);
            else
                outBuffer_ += *cp;
        }
    } else {
        check_out_buffer(s.length());
        outBuffer_ += s;
    }
}

inline void IoStream::Impl::write_char(const Char c)
{
    if (addCr_ || encodeCharEnt_) {
        check_out_buffer(2);
        const bool test_enc = encodeCharEnt_ && !rawUnicode_;
        if (c == '\n' && addCr_) {
            check_out_buffer(2);
            outBuffer_ += NOTR("\r\n");
            return;
        }
        if (test_enc && !codec_->canEncode(c)) {
            check_out_buffer(4);
            outBuffer_ += to_charent(c);
            return;
        }
    }
    check_out_buffer(1);
    outBuffer_ += c;
}

inline void IoStream::Impl::check_out_buffer(uint needChars)
{
    if (outBuffer_.length() + needChars >= IOSTREAM_STRBUFSIZE) 
        flush_out_buffer();
}

void IoStream::Impl::flush_out_buffer()
{
    if (outBufferIsSet_ || !outBuffer_.length())
        return;
    if (iohandle_.isNull()) {
        status_ = DAV_RESULT_IO_ERROR;
        return;
    }
    QByteArray byte_string(codec_->fromUnicode((QChar*)outBuffer_.unicode(),
        outBuffer_.length(), &writeConverterState_));
    uint nwrite;
    status_ = iohandle_->writeRaw(byte_string.size(), 
        byte_string.constData(), nwrite); 
    outBuffer_.resize(0);
}

bool IoStream::Impl::fill_buffer()
{
    char buf[IOSTREAM_STRBUFSIZE * 4]; // to accomodate even UTF32
    char* buf_start = buf;
    uint bytes_read = 0;
    if (iohandle_.isNull())
        return false;
    inBuffer_.reserve(IOSTREAM_STRBUFSIZE);
    status_ = iohandle_->readRaw(sizeof(buf), buf, bytes_read);
    if (!bytes_read || status_ != DAV_RESULT_OK) {
        atEof_ = true;
        return false;
    }
    if (check_unicode_) {
        check_unicode_ = false;
        int hdr_size = check_unicode_header(buf, bytes_read);
        if (hdr_size) {
            buf_start  += hdr_size;
            bytes_read -= hdr_size;
        }
    }
    if (bytes_read <= 0)
        return false;
    inBuffer_ += codec_->toUnicode(buf, bytes_read, &readConverterState_);
    return true;
}

inline String IoStream::Impl::read_string()
{
    String result;
    do {
        if (inBufferPos_ < inBuffer_.length()) {
            result += inBufferPos_ ? inBuffer_.mid(inBufferPos_) : inBuffer_;
            inBuffer_.resize(0);
            inBufferPos_ = 0;
        }
    } while (fill_buffer());
    atEof_ = true;
    return result;
}

inline bool IoStream::Impl::read_char(Char& c)
{
    if (inBufferPos_ < inBuffer_.length()) {
        c = inBuffer_[inBufferPos_++];
        return true;
    }
    inBuffer_.resize(0);
    inBufferPos_ = 0;
    if (!fill_buffer()) {
        atEof_ = true;
        return false;
    }
    if (inBuffer_.length()) {
        c = inBuffer_[0];
        ++inBufferPos_;
        return true;
    }
    atEof_ = true;
    return false;
}

void IoStream::Impl::setEncoding(int encoding)
{
    rawUnicode_ = check_unicode_ = false;
    Encodings::Encoding enc = (Encodings::Encoding) encoding;
    if (Encodings::XML == enc)
        enc = Encodings::UTF_8;
    if (enc == Encodings::UTF_8 || enc == Encodings::UTF_16) 
        rawUnicode_ = check_unicode_ = true;
    init_conv_state(&readConverterState_, false);
    init_conv_state(&writeConverterState_, enc != Encodings::UTF_16);
    codec_ = QTextCodec::codecForName
        (Encodings::qtEncodingName(enc).latin1().c_str());
    if (!codec_)
        codec_ = QTextCodec::codecForName("utf8");
}

void IoStream::setGenerateByteOrderMark(bool v)
{
    if (v)
        impl_->writeConverterState_.flags &= ~QTextCodec::IgnoreHeader;
    else
        impl_->writeConverterState_.flags |= QTextCodec::IgnoreHeader;
}

bool IoStream::generateByteOrderMark() const
{
    return (!impl_->writeConverterState_.flags & QTextCodec::IgnoreHeader);
}

int IoStream::Impl::check_unicode_header(const char* buf, int bytesRead)
{
    QTextCodec* codec = 0;
    int header_len = 0;
    if (bytesRead >= 4 && ((uchar(buf[0]) == 0xff && uchar(buf[1]) == 0xfe &&
        uchar(buf[2]) == 0 && uchar(buf[3]) == 0) ||
        (uchar(buf[0]) == 0 && uchar(buf[1]) == 0 && uchar(buf[2]) == 0xfe &&
        uchar(buf[3]) == 0xff))) {
            codec = QTextCodec::codecForName("UTF-32");
            header_len = 4;
    } else if (bytesRead >= 3 && uchar(buf[0]) == 0xef &&
        uchar(buf[1]) == 0xbb && uchar(buf[2]) == 0xbf) {
        // UTF-8 with header
        writeConverterState_.flags &= ~QTextCodec::IgnoreHeader;
    } else if (bytesRead >= 2 && ((uchar(buf[0]) == 0xff && 
        uchar(buf[1]) == 0xfe) ||
        (uchar(buf[0]) == 0xfe && uchar(buf[1]) == 0xff))) {
            codec = QTextCodec::codecForName("UTF-16");
            header_len = 2;
    } 
    if (codec) {  // reset codecs
        codec_ = codec;
        writeConverterState_.flags &= ~QTextCodec::IgnoreHeader;
    }
    return header_len;
}

//////////////////////////////////////////////////////////////

int IoStream::readRaw(int n,  char* buf)
{    
    if (impl_->iohandle_.isNull()) {
        impl_->status_ = DAV_RESULT_IO_ERROR;
        return -1;
    }
    uint nread = 0;
    impl_->status_ = impl_->iohandle_->readRaw(n, buf, nread); 
    return nread;
}

int IoStream::writeRaw(int n, const char* buf)
{
    if (impl_->iohandle_.isNull()) {
        impl_->status_ = DAV_RESULT_IO_ERROR;
        return -1;
    }
    uint nwrite = 0;
    impl_->status_ = impl_->iohandle_->writeRaw(n, buf, nwrite); 
    return nwrite;
}


OpStatus IoStream::status() const
{
    return impl_->status_;
}

OpStatus IoStream::close() 
{
    if (!impl_)
        return DAV_RESULT_OK;
    if (impl_->iohandle_.isNull()) {
        impl_->status_ = DAV_RESULT_OK;
        return impl_->status_;
    }
    impl_->flush_out_buffer();
    impl_->status_ = impl_->iohandle_->close();
    impl_->iohandle_ = 0;

    return impl_->status_;
}

OpStatus IoStream::rewind()
{
    if (impl_->iohandle_)
        impl_->status_ = impl_->iohandle_->setPosition(0);
    impl_->inBuffer_.resize(0);
    impl_->inBufferPos_ = 0;
    init_conv_state(&impl_->readConverterState_, false);
    return impl_->status_;
}

bool IoStream::atEof() const
{
    return impl_->atEof_;
}

void IoStream::setEncoding(int encoding)
{
    impl_->setEncoding(encoding);
}


void IoStream::setAddCR(bool v)
{
    impl_->addCr_ = v;
}

void IoStream::encodeCharacterEntities(bool v)
{
    impl_->encodeCharEnt_ = v;
}

IoStream& IoStream::operator<<(const Common::String& s)
{
    impl_->write_string(s);
    return *this;
}

IoStream& IoStream::operator<<(const Common::Char c)
{
    impl_->write_char(c);
    return *this;
}

IoStream& IoStream::operator>>(Common::Char& c)
{
    impl_->read_char(c);
    return *this;
}

IoStream& IoStream::operator>>(Common::String& s)
{
    s = impl_->read_string();
    return *this;
}

IoStream::IoStream(IoRequestHandle* handle)
{
    if (handle)
        impl_ = new Impl(handle);
}

void IoStream::init(IoRequestHandle* handle)
{
    impl_ = new Impl(handle);
}

IoStream::IoStream(String& ostr)
    : impl_(new Impl(ostr))
{
}

IoStream::~IoStream()
{   
    close();
}
    
IoStream::IoStream(const IoStream& other)
    : impl_(other.impl_)
{
}

IoStream& IoStream::operator=(const IoStream& other)
{
    impl_ = other.impl_;
    return *this;
}

} // namespace Dav
