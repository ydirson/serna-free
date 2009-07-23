#ifndef TmpOutputByteStream_INCLUDED
#define TmpOutputByteStream_INCLUDED 1

#include "OutputByteStream.h"
#include "Boolean.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

#ifdef SP_NAMESPACE
using namespace SP_NAMESPACE;
#endif

class TmpOutputByteStream : public OutputByteStream {
public:
  enum { bufSize = 1024 };
  struct Block {
    Block *next;
    char buf[bufSize];
  };
  class Iter {
  public:
    Iter(const TmpOutputByteStream &sb) : block_(sb.head_), lastBlockUsed_(sb.lastBlockUsed()) { }
    bool next(const char *&p, size_t &n) {
      if (block_) {
	p = block_->buf;
	n = block_->next ? TmpOutputByteStream::bufSize : lastBlockUsed_;
	block_ = block_->next;
	return 1;
      }
      else
	return 0;
    }
  private:
    Block *block_;
    size_t lastBlockUsed_;
  };
  TmpOutputByteStream();
  ~TmpOutputByteStream();
  bool isEmpty() { return head_ == 0; }
  void flush();
  void flushBuf(char ch);
private:
  friend class Iter;
  size_t lastBlockUsed() const {
    return last_ ? (ptr_ - last_->buf) : 0;
  }
  unsigned nFullBlocks_;
  Block *head_;
  Block *last_;
};

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not OutputByteStream_INCLUDED */
