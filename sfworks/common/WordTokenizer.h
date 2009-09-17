// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifndef COMMON_WORD_TOKENIZER_H_
#define COMMON_WORD_TOKENIZER_H_

#include "common/common_defs.h"
#include "common/RangeString.h"

namespace Common {

class WordTokenizer {
public:
    WordTokenizer(RangeString& rs)
        : cp_(rs.begin()), ce_(rs.end()) {}
    WordTokenizer(const Char* begin, const Char* end)
        : cp_(begin), ce_(end) {}
    
    // retrieve next word in range
    bool next(RangeString& result);
    // checks wheter this char belongs to the sole token
    static bool tokenChar(const Char& c);
private:
    const Char* cp_, *ce_;
}; 

inline bool WordTokenizer::tokenChar(const Char& c)
{
    return c.isLetter() || c.isDigit() || c == '\'';   
}

inline bool WordTokenizer::next(RangeString& result)
{
    const Char* cs;
    while (cp_ < ce_) {
        while (cp_ < ce_ && cp_->isSpace())
            ++cp_;
        cs = cp_;
        bool ok = true;
        while (cs < ce_) {
            bool tmp = cs->isDigit();
            if (tmp)
                ok = false;
            if (!cs->isLetter() && *cs != '\'' && !tmp)
                break;
            ++cs;
        }
        result = RangeString(cp_, cs);
        cp_ = ++cs;
        if (ok && result.length() > 1 && result.begin()->isLetter())
            return true;
    }
    return false;
}

} // namespace

#endif // COMMON_WORD_TOKENIZER_H_
