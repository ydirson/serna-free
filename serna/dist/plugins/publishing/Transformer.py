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

from utils  import *

__all__ = ['Transformer', 'TransformerCreator', 'TransformersFactory',
           'ChainedTransformer']

class Transformer(Value):
    def __init__(self, **kwargs):
        Value.__init__(self, **kwargs)
        self._runner = ScriptRunner(self)

    def transform(self, srcUri, dstUri):

        if dstUri is None:
            tmpExt = None
            if self.exts:
                tmpExt = self.exts[1]
            dstUri = get_tmpfile_name(srcUri, tmpExt)

            if self.tmpFiles is None :
                self.tmpFiles = []

            self.tmpFiles.append(dstUri)

        self.srcUri = srcUri
        self.dstUri = dstUri
        if hasattr(self, '_transform'):
            if self._transform(srcUri, dstUri):
                return

        if self.script:
            self._runner.run(self.script, args=self.args,
                             env=self.env, wd=self.wd)

    def cancel(self, kill=False):
        if self._runner:
            self._runner.stop(kill)

    def scriptFinished(self, exitCode, exitStatus):
        try:
            if hasattr(self, '_scriptFinished') and \
                    self._scriptFinished(exitCode, exitStatus):
                return
        except:
            pass

        if self.caller and not self._callerNotified:
            self.caller.transformComplete(exitCode, exitStatus)
            self.callerNotified = True

    def handleScriptOutput(self, data):
        if self.caller and len(data):
            self.caller.updateOutput(data)

class ChainedTransformer(Transformer):
    def __init__(self, transformers, **kwargs):
        Transformer.__init__(self, **kwargs)
        self._transformers = transformers

    def _transform(self, srcUri, dstUri):
        if self._iter is None:
            self._iter = iter(self._transformers)
            self._nextTrans = self._iter.next()
            self.dstUri = dstUri

        self._currentTrans = self._nextTrans

        try:
            self._nextTrans = self._iter.next()
            dstUri = None
        except:
            self._nextTrans = None

        if self._currentTrans is None:
            return

        self._currentTrans.caller = self
        self._currentTrans.transform(srcUri, dstUri)

    def cancel(self, kill=False):
        self._cancelling = True
        if self._currentTrans:
            self._currentTrans.cancel(kill)

    def transformComplete(self, exitCode, exitStatus):
        if 0 == exitCode and self._nextTrans and not self._cancelling:
            self._transform(self._currentTrans.dstUri, self.dstUri)
        else:
            if not ('_keepTmpFiles' in self and self._keepTmpFiles):
                for trans in self._transformers:
                    if trans.tmpFiles:
                        for tmpFile in trans.tmpFiles:
                            os.unlink(tmpFile)
            self._cancelling = self._iter = None
            self._currentTrans = self._nextTrans = None
            self.scriptFinished(exitCode, exitStatus)

    def updateOutput(self, data):
        self.handleScriptOutput(data)

class TransformerCreator(Creator):
    def __init__(self, name, key=None):
        Creator.__init__(self, name, _key=key)

    def _make(self, creatorClass, kwargs):
        for attr in ('exts', 'types'):
            if attr in self:
                kwargs[attr] = self[attr]
        return creatorClass(**kwargs)

class TransformersFactory(Factory):
    def __init__(self, props):
        Factory.__init__(self)
        pass

