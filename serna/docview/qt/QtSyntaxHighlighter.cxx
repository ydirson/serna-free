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
#include "QtSyntaxHighlighter.h"
#include <QApplication>

#define NOTR(x) x

QtSyntaxHighlighter::QtSyntaxHighlighter(QTextDocument* textEdit)
    : QSyntaxHighlighter(textEdit)
{
    QFont font(qApp->font());

    body_.font_ = font;
    body_.color_ = Qt::black;

    tag_.font_ = font;
    tag_.color_ = Qt::blue;

    tagContent_.font_ = font;
    tagContent_.color_ = Qt::blue; // QColor(158,0,0);

    comment_.font_ = font;
    comment_.color_ = Qt::gray;

    pi_.font_ = font;
    pi_.color_ = Qt::blue;

    prolog_.font_ = font;
    prolog_.color_ = QColor(85,0,127);

    cur_ = &body_;
}

#define RET(v) return setCurrentBlockState(v);

char* PROLOG_CLOSE_SYMBOL = "]>";

void QtSyntaxHighlighter::set_format(int offs, int cnt, 
                                     const FormatSettings& fs)
{
    setFormat(offs, cnt, fs.font_);
    setFormat(offs, cnt, fs.color_);
}

void QtSyntaxHighlighter::highlightBlock(const QString& text)
{
    const int endStateOfLastPara = previousBlockState();
    uint from = 0;
    if (1 == endStateOfLastPara)
        cur_ = &tagContent_;
    else if (2 == endStateOfLastPara) {
        int comment_end = text.find("-->", 0);
        if (0 > comment_end) {
            set_format(0, text.length(), comment_);
            RET(2);
        }
        set_format(0, comment_end + 3, comment_);
        cur_ = &body_;
        from = comment_end + 3;
    }
    else if (3 == endStateOfLastPara) {
        int pi_end = text.find("?>", 0);
        if (0 > pi_end) {
            set_format(0, text.length(), pi_);
            RET(3);
        }
        set_format(0, pi_end + 2, pi_);
        cur_ = &body_;
        from = pi_end + 2;
    }
    else if (4 == endStateOfLastPara) {
        int prolog_end = text.find(PROLOG_CLOSE_SYMBOL, 0);
        if (0 > prolog_end) {
            set_format(0, text.length(), prolog_);
            RET(4);
        }
        set_format(0, prolog_end + 2, prolog_);
        cur_ = &body_;
        from = prolog_end + 2;
    }
    else
        cur_ = &body_;
    for (int i = from; i < (int)text.length(); ++i) {

        if ('<' == text[i]) {
            int length = text.length();
            cur_ = &tagContent_;
            set_format(i, 1, tag_);
            if (i + 1 < length && '/' == text[i + 1]) {
                i++;
                set_format(i, 1, tag_);
            }
            else if (i + 3 < length && "<!--" == text.mid(i,4)) {
                cur_ = &comment_;
                int comment_end = text.find("-->", i);
                if (0 > comment_end) {
                    set_format(i, text.length(), comment_);
                    RET(2);
                }
                comment_end += 3;
                set_format(i, comment_end - i, comment_);
                cur_ = &body_;
                i += comment_end - i - 1;
            }
            else if (i + 1 < length && "<?" == text.mid(i,2)) {
                cur_ = &pi_;
                int pi_end = text.find("?>", i);
                if (0 > pi_end) {
                    set_format(i, text.length(), pi_);
                    RET(3);
                }
                pi_end += 2;
                set_format(i, pi_end - i, pi_);
                cur_ = &body_;
                i += pi_end - i - 1;
            }
            else if (i + 8 < length && NOTR("<!DOCTYPE") == text.mid(i,9)) {
                cur_ = &prolog_;
                if (0 <= text.find("[", i))
                    PROLOG_CLOSE_SYMBOL = "]>";
                else
                    PROLOG_CLOSE_SYMBOL = ">";
                int prolog_end = text.find(PROLOG_CLOSE_SYMBOL, i);
                if (0 > prolog_end) {
                    set_format(i, text.length(), prolog_);
                    RET(4);
                }
                prolog_end += 2;
                set_format(i, prolog_end - i, prolog_);
                cur_ = &body_;
                i += prolog_end - i - 1;
            }
            continue;
        }
        else if ('>' == text[i]) {
            cur_ =  &body_;
            set_format(i, 1, tag_);
            if (i - 1 > 0 && '/' == text[i - 1])
                set_format(i - 1, 1, tag_);
            continue;
        }
        //TODO:ATTRIBUTES
//        if (cur_ == &tagContent_ && ' ' == text[i]) {
//        }

        set_format(i, 1, *cur_);
    }
    if (cur_ == &tagContent_)
        RET(1);
    RET(0);
}
