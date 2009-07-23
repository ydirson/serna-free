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

try:
    from SernaApi import *
except:
    from XSernaApi.SernaConfig  import SernaConfig
    pass

__all__ = ['register_creators']

class DocbookHtmlPublisher(SimpleHtmlPublisher):
    def __init__(self, **kwargs):
        SimpleHtmlPublisher.__init__(self, **kwargs)
        self.name = 'Docbook to HTML publisher'
        self.doctype = 'Docbook'

class DocbookFoPublisher(SimpleFoPublisher):
    def __init__(self, **kwargs):
        SimpleFoPublisher.__init__(self, **kwargs)
        self.name = 'Docbook to FO(PDF) publisher'
        self.doctype = 'Docbook'

class DocbookPublisherCreator(SimplePublisherCreator):
    def __init__(self, **kwargs):
        SimplePublisherCreator.__init__(self, 'Docbook publisher creator',
                                        FoPublisher=DocbookFoPublisher,
                                        HtmlPublisher=DocbookHtmlPublisher,
                                        **kwargs)

        xslPropName = r'$SERNA_STYLESHEETS'
        xslDir = unicode(SernaConfig.resolveResource('', xslPropName, ''))
        if not os.path.exists(xslDir):
            dataDir = os.environ.get(r'SERNA_DATA_DIR', None)
            if dataDir:
                xslDir = os.path.join(dataDir, 'xml', 'stylesheets')

        xslDir = os.path.join(xslDir, 'docbook-xsl-1.68.1')

        self.xslFoScript = os.path.join(xslDir, 'fo', 'docbook.xsl')
        self.singleXsl = os.path.join(xslDir, 'html', 'docbook.xsl')
        self.multiXsl = os.path.join(xslDir, 'html', 'chunk.xsl')

    def _matchDoctype(self, category, name, tags):
        if category.startswith('Docbook'):
            return tags
        return []

def register_creators(pluginDir):
    registry = PublishingPlugin.PublishingPlugin.getPublishersRegistry()
    if registry:
        pc = DocbookPublisherCreator(pluginDir=pluginDir)
        registry.registerPublisherCreator(pc)
