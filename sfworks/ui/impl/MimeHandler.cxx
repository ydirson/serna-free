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
#include "ui/MimeHandler.h"
#include "ui/impl/ui_debug.h"

#include "common/Vector.h"
#include "common/asserts.h"
#include "common/Singleton.h"

#include <QVariant>
#include <QObject>
#include <QMimeData>   
#include <QUuid>
#include <QPointer>
#include <QDrag>
#include <QDropEvent>
#include <QStringList>
#include <QWidget>

#include <set>
#include <iostream>

using namespace Common;

namespace Sui {

class MimeHandlerImpl : public MimeHandler {
public:
    typedef Common::XList<MimeCallback> CallbacksList;
    typedef Common::Vector<Common::String> MimeTypesVec;
    
    virtual void            registerCallback(MimeCallback*, 
                                             double priority = 0);
    virtual MimeCallback*   getCallback(const CC,
                                        const Common::String&) const;
    virtual void            getMimeTypes(const CC, MimeTypesVec&) const;
    virtual MimeCallback*   getHighestPriorityCallback(const CC cb_class,
                                                const QMimeData* md) const;
    virtual MimeCallback*   getFirstCallback(const CC) const;

    virtual bool            callback(const CC, QDropEvent*);

    void                    dragEnter(QDragEnterEvent* event);

    virtual void            addObject(SernaApiBase*, const Common::String&);
    virtual SernaApiBase*   getObject(const Common::String&, bool) const;
    void                    removeObject(SernaApiBase*);
    void                    clearObjects();

    QDrag*                  makeDrag(QWidget*) const;
    QMimeData*              mimeData() const { return mimeData_; }
    void                    processFocusOut();

private:
    CallbacksList           callbacks_[MimeCallback::MAX_CALLBACK_ID];
    QPointer<QMimeData>     mimeData_;
};

typedef std::set<SernaApiBase*> ObjectSet;

static ObjectSet& mh_objects()
{
    return SingletonHolder<ObjectSet>::instance();
}

#ifndef _NDEBUG
static const char* callback_classname(uint n)
{
    static const char* callback_names[] = {
        "CREATE_CONTENT", "CREATE_FRAGMENT", "TRANSFORM",
        "WRAP_CONTENT", "PASTE_CONTENT", "VALIDATE_CONTENT",
        "DROP_CONTENT", "DROP_COPY_CONTENT", "DROP_MOVE_CONTENT", 
        "DROP_LINK_CONTENT"
    };
    return (n >= MimeCallback::MAX_CALLBACK_ID) ? NOTR("bad-callback-id")
        : callback_names[n];
}
#endif // _NDEBUG

////////////////////////////////////////////////////////////

void MimeHandlerImpl::addObject(SernaApiBase* obj, const String& name)
{
    removeObject(obj);
    mh_objects().insert(obj);
    String mtype = sernaObjectMimetype(name);
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream.writeRawData((const char*)(&obj), sizeof(SernaApiBase*));
    DBG(UI.DRAG) << "add object: mtype = " << mtype << std::endl;
    mimeData()->setData(mtype, array);
}

SernaApiBase* MimeHandlerImpl::getObject(const Common::String& name, 
                                         bool usePrefix) const
{
    String mtype = sernaObjectMimetype(name);
    if (usePrefix) {
        QStringList res = mimeData()->formats();
        QStringList::const_iterator it = res.begin();
        for (; it != res.end(); ++it)
            if (it->startsWith(mtype)) {
                mtype = *it;
                break;
            }
    } 
    QDataStream stream(mimeData()->data(mtype));
    SernaApiBase* sdd = 0;
    if (stream.readRawData((char*)(&sdd), sizeof(SernaApiBase*)) !=
        sizeof(SernaApiBase*))
            return 0;
    return (mh_objects().find(sdd) != mh_objects().end()) ? sdd : 0;
}

void MimeHandlerImpl::removeObject(SernaApiBase* obj)
{
    ObjectSet::iterator it;
    if (!obj) {
        for (it = mh_objects().begin(); it != mh_objects().end(); ++it)
            delete *it;
        mh_objects().clear();
    }
    it = mh_objects().find(obj);
    if (it != mh_objects().end()) {
        delete *it;
        mh_objects().erase(it);
    }
}

void MimeHandlerImpl::clearObjects()
{
    removeObject(0);
    String mtype = sernaObjectMimetype("");
    QStringList qsl(mimeData()->formats());
    QStringList::const_iterator it = qsl.begin();
    for (; it != qsl.end(); ++it) 
        if (it->startsWith(mtype))
            mimeData()->removeFormat(*it);
}

////////////////////////////////////////////////////////////

static void copy_qmimedata(const QMimeData* from, QMimeData* to)
{
    QStringList formats(from->formats());
    QStringList::iterator it = formats.begin();
    for (; it != formats.end(); ++it)
        to->setData(*it, from->data(*it));
}

void MimeHandlerImpl::dragEnter(QDragEnterEvent* event)
{
    if (event->mimeData() == mimeData())
        return;
    delete mimeData_;
    mimeData_ = new QMimeData;
    copy_qmimedata(event->mimeData(), mimeData_);
}

QDrag* MimeHandlerImpl::makeDrag(QWidget* dragSource) const
{
    DBG(UI.DRAG) << "MimeHandlerImpl::makeDrag, source="
        << String(dragSource->objectName());
    MimeHandlerImpl& mhi = const_cast<MimeHandlerImpl&>(*this);
    mhi.removeObject(0);
    delete mhi.mimeData_;
    QDrag* drag = new QDrag(dragSource);
    mhi.mimeData_ = new QMimeData;
    drag->setMimeData(mimeData_);
    return drag;
}

void MimeHandlerImpl::registerCallback(MimeCallback* new_cb, double priority)
{
    RT_ASSERT(new_cb->callbackClass() < MimeCallback::MAX_CALLBACK_ID);
    CallbacksList& clist = callbacks_[new_cb->callbackClass()];
    MimeCallback* cb = clist.firstChild();
    while (cb && cb->priority() >= priority)
        cb = cb->nextSibling();
    DBG(UI.DRAG) << "MimeHandler: registering "
        << callback_classname(new_cb->callbackClass()) << " callback for <"
        << new_cb->mimeType() << ">, prio=" << priority << std::endl;
    if (cb)
        cb->insertBefore(new_cb);
    else
        clist.appendChild(new_cb);
}

MimeCallback* MimeHandlerImpl::getCallback(const CC cb_class,
                                           const String& mime_type) const
{
    RT_ASSERT(cb_class < MimeCallback::MAX_CALLBACK_ID);
    const CallbacksList& clist = callbacks_[cb_class];
    MimeCallback* cb = clist.firstChild();
    while (cb && cb->mimeType() != mime_type)
        cb = cb->nextSibling();
    return cb;
}

void MimeHandlerImpl::getMimeTypes(const CC cb_class, MimeTypesVec& mv) const
{
    RT_ASSERT(cb_class < MimeCallback::MAX_CALLBACK_ID);
    typedef std::set<String> StrSet;
    StrSet sset;
    const CallbacksList& clist = callbacks_[cb_class];
    MimeCallback* cb = clist.firstChild();
    for (; cb; cb = cb->nextSibling())
        sset.insert(cb->mimeType());
    mv.reserve(16);
    for (StrSet::const_iterator it = sset.begin(); it != sset.end(); ++it)
        mv.push_back(*it);
}

MimeCallback* MimeHandlerImpl::getFirstCallback(const CC cb_class) const
{
    RT_ASSERT(cb_class < MimeCallback::MAX_CALLBACK_ID);
    return callbacks_[cb_class].firstChild();
}

MimeCallback* 
MimeHandlerImpl::getHighestPriorityCallback(const CC cb_class,
                                            const QMimeData* md) const
{
    QStringList qsl(md->formats());
    QStringList::const_iterator it = qsl.begin();
    MimeCallback* cb;
    for (; it != qsl.end(); ++it) {
        if ((cb = getCallback(cb_class, *it)))
            return cb;
    }
    return 0;
}

static bool drop_callback(MimeHandlerImpl* mh, 
                          const MimeCallback::CallbackClass cb_class,
                          QDropEvent* event)
{
    MimeCallback* cb = mh->getFirstCallback(cb_class);
    for (; cb; cb = cb->nextSibling()) {
        MimeCallback* validate_cb = 
            mh->getCallback(MimeCallback::VALIDATE_CONTENT, cb->mimeType());
        if (!validate_cb || !validate_cb->execute(*mh, event))
            continue;
        if (cb->execute(*mh, event))
            return true;
    }
    return false;
}

bool MimeHandlerImpl::callback(const CC cb_class, QDropEvent* event)
{
    RT_ASSERT(cb_class < MimeCallback::MAX_CALLBACK_ID);
    if (!mimeData())
        return false;
    switch (cb_class) {
        case MimeCallback::VALIDATE_CONTENT: {
            MimeCallback* cb = getFirstCallback(MimeCallback::VALIDATE_CONTENT);
            for (; cb; cb = cb->nextSibling()) 
                if (cb->execute(*this, event)) 
                    break;
            if (cb) {
                event->acceptProposedAction();
                event->accept();
                return true;
            } else {
                event->setDropAction(Qt::IgnoreAction);
                event->ignore();
                return false;
            }
        }
        case MimeCallback::DROP_CONTENT: {
            // DROP_* is called for every valid mimetype. if no DROP_*
            // exists for some mimetype, try 1) copy instead of move|link
            // and 2) generic drop
            MimeCallback::CallbackClass cl = MimeCallback::MAX_CALLBACK_ID;
            switch (event->dropAction()) {
                case Qt::CopyAction: 
                    cl = MimeCallback::DROP_COPY_CONTENT; break;
                case Qt::LinkAction:
                    cl = MimeCallback::DROP_LINK_CONTENT; break;
                case Qt::MoveAction:
                    cl = MimeCallback::DROP_MOVE_CONTENT; break;
                default: 
                    return clearObjects(), false;
            }
            if (drop_callback(this, cl, event))
                return clearObjects(), true;
            // we are still here - check alternative actions
            if (MimeCallback::DROP_LINK_CONTENT == cl ||
                MimeCallback::DROP_MOVE_CONTENT == cl) {
                    if (drop_callback(this, 
                        MimeCallback::DROP_COPY_CONTENT, event))
                            return clearObjects(), true;
            }
            // no specific callback - try generic one
            return clearObjects(),
                drop_callback(this, MimeCallback::DROP_CONTENT, event);
        }
        case MimeCallback::CREATE_FRAGMENT: {
            // try all CREATE callbacks for each mime type, until success
            MimeCallback* cb = getFirstCallback(cb_class);
            for (; cb; cb = cb->nextSibling()) 
                if (mimeData()->hasFormat(cb->mimeType()) &&
                    cb->execute(*this, event))
                        return true;
            break;
        }
        case MimeCallback::TRANSFORM: {
            MimeCallback* cb = getFirstCallback(cb_class);
            for (; cb; cb = cb->nextSibling())
                if (cb->execute(*this, event))
                    return true;
            break;
        }
        case MimeCallback::CREATE_CONTENT: {
            MimeCallback* cb = getFirstCallback(cb_class);
            for (; cb; cb = cb->nextSibling())
                cb->execute(*this, 0);
            return true;
        }
        default:
            break;
    }
    return false;
}

//////////////////////////////////////////////////////////////////

String MimeHandler::sernaObjectMimetype(const String& name)
{
    static String uuid_string;
    if (uuid_string.isEmpty())
        uuid_string = QUuid::createUuid().toString();
    return NOTR("application/syntext-serna") + uuid_string + "-" + name;
}

MimeHandler* MimeHandler::make()
{
    return new MimeHandlerImpl;
}

} // namespace Sui
