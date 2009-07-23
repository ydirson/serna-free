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
from Transformer    import *
from Publisher      import Publisher
from PublisherUtils import *
from utils          import *

import os, sys

__all__ = ['XsltTransformer', 'XsltPublisher']

class SimpleTransformer(Transformer):
    def __init__(self, **kwargs):
        Transformer.__init__(self, **kwargs)

    def _transform(self, srcUri, dstUri):

        wd = os.path.dirname(self.srcUri)
        args, env = self._getTransformArgsAndEnv()
        self._runner.run(self.script, args, env=env, wd=wd)

class SimplePdfTransformer(SimpleTransformer):
    def __init__(self, **kwargs):
        SimpleTransformer.__init__(self, **kwargs)

    def _getTransformArgsAndEnv(self):
        return ['-fo', self.srcUri, '-pdf', self.dstUri], None

class _PdfTransformerCreator(TransformerCreator):
    def __init__(self, name, key=None):
        TransformerCreator.__init__(self, name, key)
        self.exts = ['.fo', '.pdf']
        self.types = ['XSL-FO', 'PDF']

def find_dirs(pattern):
    from glob import glob
    return [ x for x in glob(pattern) if os.path.isdir(x) ]

def find_pdf_script(base, pathsPattern, exts):
    paths = find_dirs(pathsPattern)
    syspath = os.environ['PATH']
    if syspath:
        paths = syspath.split(os.pathsep) + paths
    return find_path_with_ext(base, paths, exts)

class XepTransformerCreator(_PdfTransformerCreator):
    def __init__(self):
        _PdfTransformerCreator.__init__(self, 'XEP')

    def make(self, **kwargs):
        if 'win32' == sys.platform:
            tryDir = r'C:\Program Files\RenderX\XEP'
            exts = ['.bat', '.cmd', '.exe']
        else:
            tryDir = '/usr/local/RenderX/XEP'
            exts = ['']

        script = find_pdf_script('xep', tryDir, exts)
        if not script:
            script = 'xep'

        kwargs.update({'script': script, 'name': 'XEP'})
        return self._make(SimplePdfTransformer, kwargs)
#        return SimplePdfTransformer(**kwargs)

class FopTransformerCreator(_PdfTransformerCreator):
    def __init__(self):
        _PdfTransformerCreator.__init__(self, 'FOP')

    def make(self, **kwargs):
        if 'win32' == sys.platform:
            tryDir = r'C:\Program Files\FOP\fop-*'
            exts = ['.bat', '.cmd', '.exe']
        else:
            tryDir = '/usr/local/fop-*'
            exts = ['']

        script = find_pdf_script('fop', tryDir, exts)
        if not script:
            script = 'fop'

        kwargs.update({'script': script, 'name': 'FOP'})
        return self._make(SimplePdfTransformer, kwargs)
#        return SimplePdfTransformer(**kwargs)


class XsltTransformer(Transformer):
    def __init__(self, **kwargs):
        Transformer.__init__(self, **kwargs)

        if not self.name:
            self.name = 'xsltproc'
        if not self.script:
            self.script = get_xsltproc()

    def _transform(self, srcUri, dstUri):
        args = []

        if self.args:
            if isSequence(self.args):
                args.extend(list(self.args))
            elif isString(self.args):
                args.extend(self.args.split())

        it = None
        if isDict(self.params):
            it = self.params.iteritems()
        elif isSequence(self.params):
            it = iter(self.params)

        if it:
            for name, value in it:
                args.extend(['--stringparam', name, value])

        if 'win32' != sys.platform:
            dstUri = dstUri.replace(' ', '%20')
        args.extend(['-o', dstUri, self.stylesheet, srcUri])

        env = {}
        if self.catalogs:
            if isString(self.catalogs):
                env['XML_CATALOG_FILES'] = self.catalogs
            elif isSequence(self.catalogs):
                env['XML_CATALOG_FILES'] = ' '.join(self.catalogs)

        wd = os.path.dirname(self.srcUri)
        self._runner.run(self.script, args, env=env, wd=wd)

class XsltPublisher(Publisher):
    def __init__(self, **kwargs):
        Publisher.__init__(self, **kwargs)
        self._xslt = XsltTransformer(caller=self)
        if not self.script:
            self.script = self._xslt.script

    def _publish(self, dsi, dstUri):

        catalogs = dsi.getProperty("xml-catalogs")
        self._xslt.catalogs = unicode(catalogs.getString())
        self._xslt.args = ['--xinclude']

        self._xslt.stylesheet = self.stylesheet
        self._xslt.script = self.script

        self._xslt.transform(self.srcUri, self.dstUri)

    def _cancel(self, kill=False):
        self._xslt.cancel(kill)

__trMap = {
    'fop':  FopTransformerCreator,
    'xep':  XepTransformerCreator
}

from PublishingPlugin import PublishingPlugin

if __name__ != '__main__':
    factory = PublishingPlugin.getTransformersFactory()
    if factory:
        for key, creatorClass in __trMap.iteritems():
            factory.addCreator(creatorClass())

