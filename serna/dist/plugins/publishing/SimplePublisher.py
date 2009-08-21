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
from Publisher              import *
from PublisherUtils         import *
from utils                  import *
from BuiltinTransformers    import *
from Transformer            import *
from PublishingPlugin       import PublishingPlugin

class SimpleHtmlPublisher(XsltPublisher):
    def __init__(self, **kwargs):
        XsltPublisher.__init__(self, **kwargs)
        self.extension = 'html'
        if self.multi:
            self.stylesheet = self.multiXsl
        else:
            self.stylesheet = self.singleXsl

    def hasAdvancedOptions(self):
        return True

    def fillAdvancedOptions(self, widget):
        get_simple_options(self, widget)

    def _fillAdvancedOptions(self, dlg):
        dlg.setWindowTitle('Publishing settings: %s to HTML' % self.doctype)

    def _publish(self, dsi, dstUri):
        if self.multi:
            basename, ext = os.path.splitext(os.path.basename(dstUri))
            self._xslt.params = { 'root.filename': basename,
                                  'ext': ext }

        XsltPublisher._publish(self, dsi, dstUri)

class SimpleFoPublisher(Publisher):
    def __init__(self, **kwargs):
        Publisher.__init__(self, **kwargs)

        self._fromFoTrans = self.transFactory.make(self.fromFoTrans)
        self.script = self._fromFoTrans.script
        self.extension = self._fromFoTrans.exts[1]

    def _publish(self, dsi, dstUri):

        catalogs = dsi.getProperty("xml-catalogs")
        srcBase, srcExt = os.path.splitext(os.path.basename(self.srcUri))
        toFoTrans = XsltTransformer(catalogs=catalogs,
                                    args=['--xinclude'],
                                    name='Xml2Fo',
                                    exts=[srcExt, self._fromFoTrans.exts[0]],
                                    stylesheet=self.stylesheet)

        self._fromFoTrans.script = self.script
        name = 'Xml2%s' % self._fromFoTrans.types[1]
        self._trans = ChainedTransformer([toFoTrans, self._fromFoTrans],
                                         name=name,
                                         _keepTmpFiles=self._keepTmpFiles,
                                         caller=self)
        self._trans.transform(self.srcUri, self.dstUri)


    def hasAdvancedOptions(self):
        return True

    def fillAdvancedOptions(self, widget):
        get_simple_options(self, widget)

    def _fillAdvancedOptions(self, dlg):
        oType = self._fromFoTrans.types[1]
        oName = self._fromFoTrans.name
        dlg.setWindowTitle('Publishing settings: %s to %s with %s' \
                           % (self.doctype, oType, oName))

    def _cancel(self, kill):
        if self._trans:
            self._trans.cancel(kill)

class SimplePublisherCreator(PublisherCreator):
    def __init__(self, name, **kwargs):
        PublisherCreator.__init__(self, name, **kwargs)

        self._transFactory = PublishingPlugin.getTransformersFactory()

        self._foTags = {}
        for trans in self._transFactory:
            if 'XSL-FO' == trans.types[0]:
                transName = trans.getName()
                tag = 'PDF (with %s)' % transName
                self._foTags[tag] = transName

    def _make(self, **kwargs):
        tag = kwargs.get('tag', None)
        if tag:
            if tag in self._foTags:
                return self.FoPublisher(fromFoTrans=self._foTags[tag],
                                        stylesheet=self.xslFoScript,
                                        transFactory=self._transFactory,
                                        **kwargs)
            elif tag.startswith('HTML'):
                return self.HtmlPublisher(multi=('Multiple' in tag),
                                          singleXsl=self.singleXsl,
                                          multiXsl=self.multiXsl,
                                          **kwargs)
        return None

    def _getTags(self, dsi):
        category = unicode(dsi.getProperty("category").getString())
        name = unicode(dsi.getProperty("name").getString())

        tags = ['HTML (Single chunk)', 'HTML (Multiple chunks)']
        foTransformers = self._foTags.keys()
        tags.extend(foTransformers)

        if self._matchDoctype:
            return self._matchDoctype(category, name, tags)

        return []
