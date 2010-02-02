// Copyright (c) 2006 by Syntext, Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifdef __APPLE__

#include <Carbon/Carbon.h>
#include <common/Vector.h>

AEEventHandlerUPP AEHandlerUPP;
extern QString cfstring2qstring(CFStringRef); //qglobal.cpp

static pascal OSErr open_doc_event_handler(const AppleEvent* theEvent,
                                           AppleEvent*, long)
{
    FSSpec fss;
    AEDescList the_list;
    AEKeyword ae_keyword = keyDirectObject;
    long item_count = 0;
    long i = 0;
    DescType actual_type;
    Size actual_size;
    DescType event_id;
    OSStatus error;

    OSErr err = AEGetAttributePtr((AppleEvent*)theEvent, keyEventIDAttr,
                                  typeType,&actual_type, (Ptr)&event_id,
                                  sizeof(event_id), &actual_size);
    if(err || kAEOpenDocuments != event_id)
        return err;
    // get event description
    error = AEGetParamDesc(theEvent, keyDirectObject,typeAEList,&the_list);
    if (error != noErr)
        return error;
    DescType returnedType;
    //Size  actual_size;
    err = AEGetAttributePtr(theEvent, keyMissedKeywordAttr, typeWildCard,
                            &returnedType, NULL, 0, &actual_size);
    if (err != errAEDescNotFound)
        return err;
    error = AECountItems(&the_list, &item_count);
    if(error != noErr)
        return error;

    // open all items
    for (i = 1; i <= item_count; i++ ) {
        error = AEGetNthPtr(&the_list, i, typeFSS, &ae_keyword, &actual_type,
                            (Ptr) &fss, sizeof(FSSpec), &actual_size);
        if (error == noErr) {
            FSRef fsRef;
            error = FSpMakeFSRef(&fss, &fsRef);
            if (error != noErr)
                return error;
            CFURLRef url = CFURLCreateFromFSRef(kCFAllocatorDefault, &fsRef);
            CFStringRef cfString = NULL;
            if (url != NULL) {
                cfString = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
                CFRelease(url);
            }
            int cfLen = CFStringGetLength(cfString);
            if (cfLen > 0) {
                Common::Vector<UniChar> buf(cfLen);
                CFStringGetCharacters(cfString, 
                    CFRangeMake(0, cfLen), buf.begin());
                QString file((QChar*)buf.begin(), cfLen);
                static_cast<QtSerna*>(qApp)->handleParams(file);
            }
        }
    }
    return noErr;
}

static void install_apple_doc_handler()
{
    AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, 
        NewAEEventHandlerUPP(open_doc_event_handler), 0, false);
}

#include <qmacstyle_mac.h>

class SernaMacStyle : public QMacStyle {
public:
    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption* opt,
        const QSize& sz, const QWidget* w) const
    {
        return (ct == CT_ToolButton && w->parentWidget() &&
		w->parentWidget()->inherits("QToolBar"))
            ? sz : QMacStyle::sizeFromContents(ct, opt, sz, w);
    }
};

#else // __APPLE__

static void install_apple_doc_handler() {}

#endif // __APPLE__


