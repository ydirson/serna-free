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
from SimplePublisher import *
from publishing import PublishingPlugin

__all__ = ['register_creators']

class SyntextHtmlPublisher(SimpleHtmlPublisher):
    def __init__(self, **kwargs):
        SimpleHtmlPublisher.__init__(self, **kwargs)
        self.name = 'Syntext to HTML publisher'

class SyntextTextPublisher(XsltPublisher):
    def __init__(self, **kwargs):
        XsltPublisher.__init__(self, **kwargs)
        self.name = 'Syntext to text publisher'
        self.extension = 'txt'

    def hasAdvancedOptions(self):
        return True

    def fillAdvancedOptions(self, widget):
        get_simple_options(self, widget)

    def _fillAdvancedOptions(self, dlg):
        dlg.setWindowTitle('Publishing settings: %s to text' % self.doctype)

class SyntextFoPublisher(SimpleFoPublisher):
    def __init__(self, **kwargs):
        SimpleFoPublisher.__init__(self, **kwargs)
        self.name = 'Syntext to FO(PDF) publisher'

class SyntextPublisherCreator(SimplePublisherCreator):
    def __init__(self, **kwargs):
        SimplePublisherCreator.__init__(self, 'Syntext publisher creator',
                                        **kwargs)

    def _getTags(self, dsi):
        category = unicode(dsi.getProperty("category").getString())

        if not category in ('Syntext', 'Default'):
            return []

        name = unicode(dsi.getProperty("name").getString())

        if 'Timesheet' == name:
            tags = ['Text']
        else:
            tags = self._foTags.keys()

        return tags

    def _make(self, **kwargs):

        tag = kwargs.get('tag', None)
        if not tag:
            return None

        dsi = kwargs.get('dsi', None)
        if not dsi:
            return None

        name = unicode(dsi.getProperty("name").getString())

        if tag in self._foTags:
            args = Value()

            if 'Simple Letter' == name:
                args.stylesheet = os.path.normpath(self.pluginDir + \
                                                   '/simple_letter/letter.xsl')
            else:
                args.stylesheet = os.path.normpath(self.pluginDir + \
                                                   '/default/default.xsl')

            kwargs.update(args.attrs())

            return SyntextFoPublisher(fromFoTrans=self._foTags[tag],
                                      transFactory=self._transFactory,
                                      **kwargs)
        elif 'Text' == tag:
            if 'Timesheet' == name:
                xsl = os.path.normpath(self.pluginDir + \
                                       '/timesheet/timesheet-xsl-serna/' + \
                                       'timesheet2text.xsl')
                return SyntextTextPublisher(stylesheet=xsl, **kwargs)

        return None

def register_creators(pluginDir):
    registry = PublishingPlugin.PublishingPlugin.getPublishersRegistry()
    if registry:
        pc = SyntextPublisherCreator(pluginDir=pluginDir)
        registry.registerPublisherCreator(pc)
