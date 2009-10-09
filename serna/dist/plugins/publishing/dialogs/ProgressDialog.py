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
import os, sys, re

from ui             import build_ui_widgets
from PyQt4          import QtCore
from PyQt4.QtCore   import Qt, QProcess, QTimer, QTimeLine
from PyQt4.QtGui    import QGroupBox, QTextEdit, QVBoxLayout, QHBoxLayout, \
                           QGridLayout, QSpacerItem, QSizePolicy
from utils          import dump_ptree, PublishException
from SernaApi       import XpathExpr, Grove

Ui_ProgressDialog, DialogBase = build_ui_widgets("ProgressDialog")

class ProgressDialog(Ui_ProgressDialog, DialogBase):
    def __init__(self, publisher, plugin, parentWidget=None):
        DialogBase.__init__(self, parentWidget)
        self.setupUi(self)
        self.setObjectName("ProgressDialog")
        self.viewButton_.setEnabled(False)

        self._publisher = publisher
        self._plugin = plugin
	self._parent = parentWidget
        self._cancelled = False
        self._timeline = QTimeLine(1000*60, self)
        self._timeline.setFrameRange(0, 2*60)
        self._timeline.setLoopCount(0)
        self.progressBar_.setRange(0, 60)
        self.connect(self._timeline, QtCore.SIGNAL("frameChanged(int)"),
                     self.updateProgressBar)

        self.outputGroupBox_ = QGroupBox("Script output", None)

        self.outputTextEdit_ = QTextEdit()
        self.outputTextEdit_.setTextInteractionFlags(Qt.TextSelectableByKeyboard
                                                     | Qt.TextSelectableByMouse)
        self.outputTextEdit_.setReadOnly(True)
        self.outputTextEdit_.setTabChangesFocus(True)
        self.outputTextEdit_.setAcceptRichText(False)

        groupBoxLayout = QVBoxLayout()
        groupBoxLayout.setObjectName("groupBoxLayout")
        groupBoxLayout.setMargin(0)
        groupBoxLayout.addWidget(self.outputTextEdit_)
        self.outputGroupBox_.setLayout(groupBoxLayout)

        gridLayout = QGridLayout()
        gridLayout.setSizeConstraint(gridLayout.SetFixedSize)
        gridLayout.addWidget(self.progressLabel_, 0, 0, 1, 4)
        gridLayout.addWidget(self.progressBar_, 1, 0, 1, 4)
        gridLayout.addWidget(self.detailsCheckBox_, 2, 0)
        hSpacer = QSpacerItem(250, 10, QSizePolicy.Expanding)
        gridLayout.addItem(hSpacer, 2, 1)

        gridLayout.addWidget(self.viewButton_, 2, 2)
        gridLayout.addWidget(self.cancelButton_, 2, 3)
        gridLayout.addWidget(self.outputGroupBox_, 3, 0, 1, 4)

        self.setLayout(gridLayout)

        self.outputGroupBox_.setVisible(False)

    def updateProgressBar(self, frame):
        self.progressBar_.setValue(self.progressBar_.value() + 1)

    def on_detailsCheckBox__stateChanged(self, state):
        self.outputGroupBox_.setVisible(Qt.Checked == state)
        gridLayout = self.layout()
        if Qt.Checked == state:
            gridLayout.setSizeConstraint(gridLayout.SetMaximumSize)
            self.setSizeGripEnabled(True)
        else:
            gridLayout.setSizeConstraint(gridLayout.SetFixedSize)
            self.setSizeGripEnabled(False)

    def on_cancelButton__clicked(self, released=True):
        if not released:
            return

        if self._cancelled:
            self.reject()
            return

        self.cancelButton_.setEnabled(False)
        self.progressLabel_.setText("Cancelling...")
        self._publisher.cancel()
        self._cancelled = True
        QTimer.singleShot(5*1000, self, QtCore.SLOT("_kill()"))

    @QtCore.pyqtSignature("_kill()")
    def _cancel(self):
        self._parent.update()
        self._publisher.cancel(True)
        self.reject()

    def updatePublisherOutput(self, data):
        self.outputTextEdit_.append(data)

    def publishComplete(self, exitCode, exitStatus):
        self.progressBar_.setValue(self.progressBar_.maximum())
        self._timeline.stop()
        if self._cancelled:
            self.reject()
        self._cancelled = True
        publishSuccess = (0 == exitCode and QProcess.NormalExit == exitStatus)
        output_exists = self.__findOutput()
        self.viewButton_.setEnabled(publishSuccess and \
                                    output_exists)
        if not publishSuccess:
            self.progressLabel_.setText("Publishing failed, see script output"
                                        " for more details")
        else:
            self.progressLabel_.setText("Publishing completed")

    def __findOutput(self):
        output_exists = os.path.exists(unicode(self._outFile))
        if not output_exists:
            output_exists = self.__findInSubdir()
        if not(output_exists) and ('Dita' in self._publisher.__str__()) :
            output_exists = self.__findInLog()
        if not(output_exists) and ('Docbook' in self._publisher.__str__()) :
            output_exists = self.__findInPI()
        return output_exists

    def __findInLog(self):
        log = self.outputTextEdit_.toPlainText()
        src_filename = os.path.basename(self._publisher.attrs()['srcUri'])
        dst_filename = src_filename.split('.')[0] + "." + self._publisher.attrs()['extension']
        re_str = '\[xslt\] Processing.*?' + src_filename  + ' to (?P<outputFilename>.*?' + dst_filename + ')'
        output_re = re.compile(re_str)
        output_filename = ''
        if None != output_re.search(log):
            output_filename = output_re.search(log).group("outputFilename")
        if not output_filename:
            return False
        real_dst_dir = os.path.dirname(unicode(output_filename))
        dst_filename = os.path.join(real_dst_dir, os.path.basename(self._outFile))
        os.rename(output_filename, dst_filename)
        output_exists = os.path.exists(dst_filename)
        if output_exists:
            self._outFile = dst_filename
            self._parent.setOutputFilePath(self._outFile)
            return True
        return False

    def __findInPI(self):
        src_uri = self._publisher.attrs()['srcUri']
        grove = Grove.buildGroveFromFile(src_uri)
        xpath_value = XpathExpr("//self::processing-instruction('dbhtml')").eval(grove.document())
        dbhtml_pi = xpath_value.getNodeSet().firstNode()
        str_ = unicode(dbhtml_pi.asGrovePi().data())
        filename_re = re.compile('filename="(?P<filename>.*?\n?.*?)"')
        dir_re = re.compile('dir="(?P<dir>.*?\n?.*?)"')
        if None != filename_re.search(str_):
            filename_ = filename_re.search(str_).group("filename")
        if None != dir_re.search(str_):
            dir_ = dir_re.search(str_).group("dir")
        out_dir = os.path.dirname(self._outFile)
        combined_output_filename = os.path.join(out_dir, dir_, filename_)
        output_exists = os.path.exists(combined_output_filename)
        if output_exists:
            self._outFile = combined_output_filename
            self._parent.setOutputFilePath(self._outFile)
            return True
        return False

    def __findInSubdir(self):
        output_filename = unicode(self._outFile)
	filename_ = os.path.basename(output_filename)
        dir_ = os.path.dirname(output_filename)
	folder_name = os.path.basename(dir_)
	output_filename = os.path.join(dir_, folder_name, filename_)
	output_exists = os.path.exists(output_filename)
	if output_exists:
	    self._outFile = output_filename
            self._parent.setOutputFilePath(self._outFile)
            return True
        return False

    def on_viewButton__clicked(self, released=True):
        if not released:
            return
        self._plugin.launchViewer(os.path.abspath(self._outFile))

    def publish(self, dsi, outFile):
        if not self._publisher:
            self.updatePublisherOutput("Script is not found")
            self.publishComplete(1, QProcess.Crashed)
            return self.exec_()
        self._outFile = outFile
        self.show()
        try:
            self.progressBar_.setValue(self.progressBar_.minimum() + 1)
            self._publisher.publish(self, dsi, outFile)
            self._timeline.start()
        except PublishException, pe:
            self.updatePublisherOutput(pe.getErrorString())
        return self.exec_()
