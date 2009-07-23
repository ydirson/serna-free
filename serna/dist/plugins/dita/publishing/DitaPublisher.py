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
from Publisher      import *
from Transformer    import *
from PublisherUtils import *
from utils          import *

from SimplePublisher import SimplePublisherCreator
from dialogs.DitaOtOptionsDialog import get_dita_options

from publishing     import PublishingPlugin

try:
    from SernaApi import *
except:
    from XSernaApi.SernaConfig  import SernaConfig
    pass

import tempfile

__all__ = ['register_creators']

def get_win32_free_drive():
    drivelist = [ '%s:' % chr(x) for x in range(ord('d'), ord('z') + 1)]
    drivelist.reverse()
    for drive in drivelist:
        if not os.path.exists(drive + r'\.'):
            return drive
    return None

from subprocess import call

class DitaTempDir(Value):
    def __init__(self):
        Value.__init__(self)

        if 'win32' == sys.platform:
            tempDir = tempfile.mkdtemp()

            tempDrive = get_win32_free_drive()
            if tempDrive:
                tempDirParent, tempDirBase = os.path.split(tempDir)
                call('subst %s "%s"' % (tempDrive, tempDirParent),
                     creationflags=0x08000000)

                self._tempDir = '%s\\%s' % (tempDrive, tempDirBase)
                if not os.path.exists(self._tempDir):
                    self._tempDir = tempDir

                self._parentDir = tempDirParent
                self._tempDrive = tempDrive
            else:
                self._parentDir = os.path.dirname(tempDir)

            self._absTempDir = tempDir
        else:
            self._absTempDir = self._tempDir = tempfile.mkdtemp()
            self._parentDir = os.path.dirname(self._absTempDir)

    def get(self):
        return self._tempDir

    def parent(self):
        return self._parentDir

    def __del__(self):
        self.cleanup()
        pass

    def cleanup(self):
        from shutil import rmtree
        if self._tempDir and os.path.exists(self._tempDir) and \
                self._tempDir != os.path.dirname(self._tempDir):
            rmtree(self._tempDir, True)
        if 'win32' == sys.platform and self._tempDrive and \
                os.path.exists(self._tempDrive + r'\.'):
            call('subst /d %s' % self._tempDrive, creationflags=0x08000000)

class DitaOtTransformer(Transformer):
    def __init__(self, **kwargs):
        Transformer.__init__(self, **kwargs)

        if self._ditaOtDir is None:
            dotDir = SernaConfig.getProperty("vars/dita_ot_dir")
            self._ditaOtDir = unicode(dotDir)

    def _prepare(self, srcUri, dstUri):
        self._tempDir = DitaTempDir()

        ditaJars = "dost;resolver;fop;avalon-framework-cvs-20020806;batik;" \
                   "xalan;xercesImpl;xml-apis;icu4j".split(';')
        ditaLibDir = os.path.join(self._ditaOtDir, 'lib')

        classPath = [ os.path.join(ditaLibDir, '%s.jar' % x) for x in ditaJars ]
        classPath.append(ditaLibDir)
        self._env = { 'CLASSPATH': os.pathsep.join(classPath) }

        if 'win32' == sys.platform:
            ditaOtDirUrl = "file:///" + self._ditaOtDir
        else:
            ditaOtDirUrl = "file://" + self._ditaOtDir

        xmlCatalog = ditaOtDirUrl.replace('\\', '/') + "/catalog-dita.xml"
        self._catMgrProps = os.path.join(self._tempDir.get(),
                                         "CatalogManager.properties")
        open(self._catMgrProps, "w+").write("""
catalogs=%s
relative-catalogs=no
prefer=public
static-catalog=yes
        """.strip() % xmlCatalog)

        antArgsDict = {
            'args.target': 'init',
            'args.input': self.srcUri,
            'output.dir': os.path.dirname(self.dstUri),
            'dita.temp.dir': self._tempDir.get(),
            'args.logdir':  self._tempDir.get()
        }

        if self._antArgsDict:
            for k, v in self._antArgsDict.iteritems():
                if v.startswith('$'):
                    name = v[1:]
                    if name in self:
                        self._antArgsDict[k] = self[name]
                        
            antArgsDict.update(self._antArgsDict)

        self._antArgsDict = antArgsDict

    def _transform(self, srcUri, dstUri):
        self._prepare(srcUri, dstUri)

        args = []
        for name, value in self._antArgsDict.iteritems():
            args.append('-D%s=%s' % (name, value))

        if self._antOptions:
            opts = self._antOptions.split()
            if opts:
                args.extend(opts)

        if self._args:
            self._args.extend(args)
        else:
            self._args = args

        self._args.extend(['-f', self._antMakefile])
        self._args.append(self._antTarget)

        self._scriptRunner = ScriptRunner(self)
        antScript = find_script(unicode(self._antScript))
        wd = os.path.dirname(self.srcUri)
        self._scriptRunner.run(antScript, self._args, wd=wd, env=self._env)

        return True

    def _scriptFinished(self, exitCode, exitStatus):
        if self._tempDir:
            self._tempDir.cleanup()
        return False

class DitaPublisher(Publisher):
    def __init__(self, **kwargs):
        Publisher.__init__(self, **kwargs)

    def hasAdvancedOptions(self):
        return True

    def fillAdvancedOptions(self, widget):
        accepted, dlg = get_dita_options(self, widget)
        if accepted:
            for opt in ('_antScript', '_antMakefile', '_antOptions'):
                prop = dlg.getProperty(opt[1:])
                if prop:
                    self[opt] = prop

class HtmlDitaOtPublisher(DitaPublisher):
    def __init__(self, **kwargs):
        Publisher.__init__(self, **kwargs)
        self.extension = 'html'

    def _fillAdvancedOptions(self, dlg):
        dlg.setLabel('antMakefile', 'DITA to XHTML Ant makefile')
        title = 'Publishing settings: DITA to XHTML with DITA Open Toolkit'
        dlg.setWindowTitle(title)

    def _publish(self, dsi, dstUri):

        base, ext = os.path.splitext(os.path.basename(self.srcUri))
        args = ['-Dargs.transtype=xhtml', '-Dargs.xhtml.toc=%s' % base]

        kwargs = {
            '_ditaOtDir': self._ditaOtDir,
            '_antTarget': 'dita2xhtml',
            '_args': args,
            'exts': [ext, '.html'],
            'caller': self
        }
        for name, value in self.attrs().iteritems():
            if name.startswith('_ant'):
                kwargs[name] = value

        self._dotHtmlTrans = DitaOtTransformer(**kwargs)

        self._dotHtmlTrans.transform(self.srcUri, dstUri)

class FoDitaOtPublisher(DitaPublisher):
    def __init__(self, **kwargs):
        Publisher.__init__(self, **kwargs)

        self._fromFoTrans = self.transFactory.make(self.fromFoTrans)
        self.script = self._fromFoTrans.script
        self.extension = self._fromFoTrans.exts[1]

    def _fillAdvancedOptions(self, dlg):
        dlg.setLabel('antMakefile', 'DITA to XSL-FO Ant makefile')
        title = 'Publishing settings: DITA to PDF with DITA Open Toolkit and %s' % self._fromFoTrans.name
        dlg.setWindowTitle(title)

    def _publish(self, dsi, dstUri):

        base, ext = os.path.splitext(os.path.basename(self.srcUri))
        kwargs = {
            '_ditaOtDir': self._ditaOtDir,
            '_antTarget': 'dita2fo',
            '_defineOutput': 0, '_defineInput': 0,
            'exts': [ext, '.fo'],
            'caller': self
        }
        for name, value in self.attrs().iteritems():
            if name.startswith('_ant'):
                kwargs[name] = value

        toFoTrans = DitaOtTransformer(**kwargs)
        toFoTrans._antArgsDict = { 'output.fo': '$dstUri' }

        self._trans = ChainedTransformer([toFoTrans, self._fromFoTrans],
                                         name='Dita2%s' % self.fromFoTrans,
                                         caller=self)

        self._trans.transform(self.srcUri, self.dstUri)

    def _cancel(self, kill):
        if self._trans:
            self._trans.cancel(kill)

class DitaOTPublisherCreator(SimplePublisherCreator):
    def __init__(self, **kwargs):
        SimplePublisherCreator.__init__(self, 'Dita', **kwargs)

        params = Value()

        self._ditaOtDir = unicode(SernaConfig.getProperty("vars/dita_ot_dir"))
        params._ditaOtDir = self._ditaOtDir
        params._antHome = os.path.join(params._ditaOtDir, 'tools', 'ant')
        params._antScript = os.path.join(params._antHome, 'bin', 'ant')

        for k in params.attrs().keys():
            params[k] = os.path.normpath(params[k])

        self._ditaParams = params


    def _make(self, **kwargs):
        tag = kwargs.get('tag', None)
        if tag:
            kwargs.update(self._ditaParams.attrs())
            if tag in self._foTags:
                antMakefile = os.path.join(self._ditaOtDir, "build_with_fo.xml")
                return FoDitaOtPublisher(fromFoTrans=self._foTags[tag],
                                         transFactory=self._transFactory,
                                         _antMakefile=antMakefile,
                                         **kwargs)
            elif tag.startswith('HTML'):
                antMakefile = os.path.join(self._ditaOtDir, "build.xml")
                return HtmlDitaOtPublisher(_antMakefile=antMakefile, **kwargs)
        return None

    def _getTags(self, dsi):
        category = unicode(dsi.getProperty("category").getString())
        if not 'DITA' in category:
            return []
        tags = ['HTML']
        foTransformers = self._foTags.keys()
        tags.extend(foTransformers)
        return tags

def register_creators(pluginDir):
    registry = PublishingPlugin.PublishingPlugin.getPublishersRegistry()
    if registry:
        pc = DitaOTPublisherCreator(pluginDir=pluginDir)
        registry.registerPublisherCreator(pc)

