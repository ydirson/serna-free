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
#!/usr/bin/python

import sys, os

xp, adp_info_xsl, db_adp, dita_adp = sys.argv[1:]

xsl = open(adp_info_xsl, "r").read()

def get_adp_info (adp):
    info_cmd = "%s %s %s" % (xp, adp_info_xsl, adp)
    xslin, xslout = os.popen4(info_cmd)
    xslin.write(xsl)
    xslin.close()
    adp_dict = {}

    for line in xslout:
        line = line.strip()
        ref, title = line.split(' ', 1)

        htmldir, htmllink = ref.split('/', 1)
        html, fragid = split_fragid(htmllink)
        adp_dict.setdefault(htmldir, {}).setdefault(html, {})[fragid] = 1

    return adp_dict

def split_fragid(href):
    hlist = href.split('#', 1)
    if len(hlist) > 1:
        return tuple(hlist)
    return href, ''

def check_link(html, fragid):
    if os.path.exists(html):
        if fragid:
            contents = open(html, "r").read()
            if -1 == contents.find('<a name="%s">' % fragid):
                print >> sys.stderr, "Broken link '%s#%s'" % (html, fragid),
                print >> sys.stderr, "no '%s' fragment identifier" % fragid
                return 1
    else:
        print >> sys.stderr, "Broken link '%s%s'" % (html, '#' + fragid)
        print >> sys.stderr, "'%s' doesn't exist" % html
        return 1
    return 0

def check_links(htmldir, htmldict, add=''):
    if add:
        add += '/'
    broken_links = 0
    for html, fragdict in htmldict.iteritems():
        for fragid in fragdict.keys():
            broken_links += check_link(add + htmldir + '/' + html, fragid)
    return broken_links

def count_text(htmldir, htmldict, add='', xp_arg=''):
    htmllist = [ "%s/%s" % (htmldir, x) for x in htmldict.keys() if x != 'index.html']
    dbg = open("%s-%s.count.txt" % (htmldir, add), "w")
    if add:
        add += '/'
    htmls = add + (' %s' % add).join(htmllist)
    cmd = "%s %s - %s" % (xp, xp_arg, htmls)

    xslin, xslout = os.popen4(cmd)
    xslin.write("""<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.1'>

<xsl:output method="text" encoding="UTF-8"/>
<xsl:strip-space elements="*"/>

<xsl:template match="text()">
  <xsl:value-of select="concat(normalize-space(.), '&#10;')"/>
</xsl:template>

</xsl:stylesheet>""")
    xslin.close()
    txtlen = 0
    for line in xslout:
        line = ' '.join(line.replace("\xc2\xa0", ' ').split())
        line_len = len(line.strip())
        if line_len > 0:
            print >> dbg, line
            txtlen += line_len
    return txtlen

db_dict = get_adp_info(db_adp)
dita_dict = get_adp_info(dita_adp)

broken_links = 0

for htmldir, htmldict in db_dict.iteritems():
    broken_links += check_links(htmldir, dita_dict[htmldir], 'dita')

sys.exit(broken_links)

if 0:
    db_len = count_text(htmldir, htmldict, '', '--html')
    dita_len = count_text(htmldir, dita_dict[htmldir], 'dita')

    diff = 100*float(abs(dita_len - db_len))/max(dita_len, db_len)
    if diff > 1:
        print >> sys.stderr, "Difference in '%s' is %d" % (htmldir, diff)

