## 
## Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
## Contact: info@syntext.com, http://www.syntext.com
## 
## This file is part of Syntext Serna XML Editor.
## 
## COMMERCIAL USAGE
## Licensees holding valid Syntext Serna commercial licenses may use this file
## in accordance with the Syntext Serna Commercial License Agreement provided
## with the software, or, alternatively, in accorance with the terms contained
## in a written agreement between you and Syntext, Inc.
## 
## GNU GENERAL PUBLIC LICENSE USAGE
## Alternatively, this file may be used under the terms of the GNU General 
## Public License versions 2.0 or 3.0 as published by the Free Software 
## Foundation and appearing in the file LICENSE.GPL included in the packaging 
## of this file. In addition, as a special exception, Syntext, Inc. gives you
## certain additional rights, which are described in the Syntext, Inc. GPL 
## Exception for Syntext Serna Free Edition, included in the file 
## GPL_EXCEPTION.txt in this package.
## 
## You should have received a copy of appropriate licenses along with this 
## package. If not, see <http://www.syntext.com/legal/>. If you are unsure
## which license is appropriate for your use, please contact the sales 
## department at sales@syntext.com.
## 
## This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
## WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
## 
#
# 
#

import xml.sax, xml.sax.xmlreader
import types, string

# Simple Stupid XML SAX Parser

subst = [('&', "&amp;"),('"', "&quot;"),("'", "&apos;"),('<', "&lt;"),('>', "&gt;")]

def get_up_to_delim( src, pos, delim):
    endpos = src[pos:].find( delim)
    res = src[pos:endpos+pos].strip()
    if endpos != -1: endpos = endpos + pos + len(delim)
    return ( endpos, res)

class SSXMLParser( xml.sax.xmlreader.XMLReader):
    def __init__( self):
        xml.sax.xmlreader.XMLReader.__init__(self)

    def process_lt( self, src):
        if src[self.pos_] == '/':
            self.pos_, elemname = get_up_to_delim( src, self.pos_ + 1, '>')
#            print "end of", elemname
            self._cont_handler.endElement( elemname)
            return
        elif src[self.pos_:self.pos_ + 3] == "!--":
            self.pos_, skip = get_up_to_delim( src, self.pos_, "-->")
            return
        elif src[self.pos_] == '?':
            self.pos_, skip = get_up_to_delim( src, self.pos_, "?>")
            return

        self.pos_, elem = get_up_to_delim( src, self.pos_, '>')
        pos, endpos = ( 0, len( elem))
        attrs = {}
        pos, elemname = get_up_to_delim( elem, pos, ' ')
#        print "start of", elemname
        while pos != -1:
            pos, attrname = get_up_to_delim( elem, pos, '=')
            if elem[pos] != '"': get_up_to_delim( elem, pos, '"')
            pos += 1
            pos, attrvalue = get_up_to_delim( elem, pos, '"')
            attrs[attrname] = attrvalue
#            print attrname, "->", attrvalue
            pos, skip = get_up_to_delim( elem, pos, ' ')
    
        self._cont_handler.startElement( elemname, xml.sax.xmlreader.AttributesImpl( attrs))

    def process_text( self, src):
        pos, text = get_up_to_delim( src, self.pos_, '<')
        if pos != -1: self.pos_ = pos - 1
        for sub, rep in subst:
            text = text.replace( rep, sub)

        self._cont_handler.characters( text)

    def parse( self, source):
        if type( source) is types.StringType:
            src = open( source, "r").read()
        elif type( source) is types.FileType:
            src = source.read()
        else:
        	raise TypeError

        self.eof_ = len( src)
        self.pos_ = 0

        self._cont_handler.startDocument()
        try:
            while self.pos_ != self.eof_:
                while src[self.pos_] in string.whitespace: self.pos_ += 1
                char = src[self.pos_]
                if char == '<':
                    self.pos_ += 1
                    self.process_lt( src)
                else:
                    self.process_text( src)

        except IndexError:
            pass
        self._cont_handler.endDocument()
        return

def create_parser():
	return SSXMLParser()
