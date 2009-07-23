// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
 #ifndef IDLE_HANDLER_IMPL_H_
#define IDLE_HANDLER_IMPL_H_

#include "utils/IdleHandler.h"
#include "common/oalloc.h"

#include <QObject>

class QTimer;

class IdleProcessManagerImpl : public QObject {
    Q_OBJECT
public:
    IdleProcessManagerImpl();

public slots:
    void    holdoffTimerEvent();
    void    reprocessTimerEvent();

private:
    friend class IdleHandler;

    typedef COMMON_NS::CDList<IdleHandler> IdleHandlerList;
    typedef IdleHandlerList::iterator IdleHandlerListIter;

    void                startHoldoffTimer(int);

    IdleHandlerList     hlist_;
    bool                reprocessTimerActive_;
    bool                needReprocessing_;
    QTimer*             holdoffTimer_;
    QTimer*             reprocessTimer_;
};

class SingleShotTimer : public QObject,
                        public Common::CDListItem<SingleShotTimer> {
    Q_OBJECT

public:
    USE_OALLOC(SingleShotTimer);
    
    SingleShotTimer(QObject* receiver, const char* m, int interval = 0);
    SingleShotTimer(__delayed_sst_funcp funcp, void* funcarg, int interval = 0);
    ~SingleShotTimer();

    virtual bool event(QEvent *e);
    void stop();

signals:
    void    timeout();

private:
    int         id_;
    __delayed_sst_funcp funcp_;
    void*       funcarg_;
};    

typedef Common::CDList<SingleShotTimer> SS_List;

#endif // IDLE_HANDLER_IMPL_H_
