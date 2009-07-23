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

class NitfHtmlPublisher(SimpleHtmlPublisher):
    def __init__(self, **kwargs):
        SimpleHtmlPublisher.__init__(self, **kwargs)
        self.name = 'NITF to HTML publisher'
        self.doctype = 'NITF'

class NitfFoPublisher(SimpleFoPublisher):
    def __init__(self, **kwargs):
        SimpleFoPublisher.__init__(self, **kwargs)
        self.name = 'NITF to FO(PDF) publisher'
        self.doctype = 'NITF'

class NitfPublisherCreator(SimplePublisherCreator):
    def __init__(self, **kwargs):
        SimplePublisherCreator.__init__(self, 'NITF publisher creator',
                                        FoPublisher=NitfFoPublisher,
                                        HtmlPublisher=NitfHtmlPublisher,
                                        **kwargs)

        xslDir = os.path.join(self.pluginDir, 'nitf-xsl-serna')
        self.xslFoScript = os.path.join(xslDir, 'nitf.xsl')
        self.singleXsl = self.multiXsl = ''

    def _matchDoctype(self, category, name, tags):
        if category == 'NITF':
            return [ tag for tag in tags if not 'HTML' in tag ]
        return []

def register_creators(pluginDir):
    registry = PublishingPlugin.PublishingPlugin.getPublishersRegistry()
    if registry:
        pc = NitfPublisherCreator(pluginDir=pluginDir)
        registry.registerPublisherCreator(pc)

