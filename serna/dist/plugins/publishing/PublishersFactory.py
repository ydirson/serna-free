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
from utils import Factory

class PublishersFactory(Factory):
    def __init__(self, props=None):
        Factory.__init__(self)
        self._tags = []

    def getPublishersTags(self):
        return self._tags

    def addPublisherCreator(self, publisherCreator, dsi):
        pcTags = publisherCreator.getTags(dsi)
        if pcTags:
            self._tags.extend(pcTags)
            for tag in pcTags:
                self.addCreator(publisherCreator, tag)

class PublishersRegistry(object):
    def __init__(self, props):
        self._props = props
        self._publishersDict = {}

    def registerPublisherCreator(self, publisherCreator):
        self._publishersDict[publisherCreator.getName()] = publisherCreator

    def findPublisherCreator(self, name):
        return self._publishersDict.get(name, None)

    def getFactoryForDsi(self, dsi):
        pcFactory = PublishersFactory()
        for pc in self._publishersDict.values():
            pcFactory.addPublisherCreator(pc, dsi)
        return pcFactory

    def dump(self):
        print "PublishersRegistry:"
        for k, v in self._publishersDict.iteritems():
            v.dump(2)

