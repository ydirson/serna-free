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

class IDEAllianceHtmlPublisher(SimpleHtmlPublisher):
    def __init__(self, **kwargs):
        SimpleHtmlPublisher.__init__(self, **kwargs)
        self.name = 'IDE Alliance to HTML publisher'
        self.doctype = 'IDEAlliance'

class IDEAllianceFoPublisher(SimpleFoPublisher):
    def __init__(self, **kwargs):
        SimpleFoPublisher.__init__(self, **kwargs)
        self.name = 'IDE Alliance to FO(PDF) publisher'
        self.doctype = 'IDEAlliance'

class IDEAlliancePublisherCreator(SimplePublisherCreator):
    def __init__(self, **kwargs):
        SimplePublisherCreator.__init__(self, 'IDE Alliance publisher creator',
                                        **kwargs)

    def _getTags(self, dsi):
        category = unicode(dsi.getProperty("category").getString())

        if 'IDEAlliance' != category:
            return []

        name = unicode(dsi.getProperty("name").getString())

        if 'GCA Proceedings' == name:
            tags = self._foTags.keys()
        else:
            tags = ['HTML (Single chunk)', 'HTML (Multiple chunks)']
            foTransformers = self._foTags.keys()
            tags.extend(foTransformers)

        return tags

    def _make(self, **kwargs):

        tag = kwargs.get('tag', None)
        if tag:
            dsi = kwargs.get('dsi', None)

            if not dsi:
                return None

            name = unicode(dsi.getProperty("name").getString())

            args = Value()

            if 'GCA Proceedings' == name:
                args.stylesheet = os.path.normpath(self.pluginDir + \
                                                   '/gca/gca-xsl-serna/gca.xsl')
            else:
                xslPropName = r'$SERNA_STYLESHEETS'
                dbDir = unicode(SernaConfig.resolveResource('', xslPropName, ''))
                if not os.path.exists(dbDir):
                    dataDir = os.environ.get(r'SERNA_DATA_DIR', None)
                    if dataDir:
                        dbDir = os.path.join(dbDir, 'xml', 'stylesheets')

                dbDir = os.path.join(dbDir, 'docbook-xsl-1.68.1')

                args.stylesheet = os.path.normpath(dbDir + '/fo/docbook.xsl')
                args.singleXsl = os.path.normpath(dbDir + '/html/docbook.xsl')
                args.multiXsl = os.path.normpath(dbDir + '/html/chunk.xsl')

            kwargs.update(args.attrs())

            if tag in self._foTags:
                return IDEAllianceFoPublisher(fromFoTrans=self._foTags[tag],
                                              transFactory=self._transFactory,
                                              **kwargs)
            elif tag.startswith('HTML'):
                return IDEAllianceHtmlPublisher(multi=('Multiple' in tag),
                                                **kwargs)

        return None

def register_creators(pluginDir):
    registry = PublishingPlugin.PublishingPlugin.getPublishersRegistry()
    if registry:
        pc = IDEAlliancePublisherCreator(pluginDir=pluginDir)
        registry.registerPublisherCreator(pc)

