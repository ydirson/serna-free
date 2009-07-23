// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class UiEventExecutor {

%TypeHeaderCode
#include "sapi/app/DocumentPlugin.h"
using namespace SernaApi;
%End

public:
    UiEventExecutor(const UiAction&);
    UiAction uiAction() const;
    virtual void execute() = 0;
};

class DocumentPlugin : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/app/DocumentPlugin.h"
using namespace SernaApi;
%End

public:
    DocumentPlugin(long, long); 

    SernaDoc        sernaDoc() const;

    SString         pluginName() const;
    PropertyNode    pluginProperties() const;
    
    void            buildPluginExecutors(bool = false);
    void            buildPluginInterface();

    UiItem          makeUiItem(const SString&, const PropertyNode&) const;
    UiItem          buildUiItem(const SString&) const;
    void            registerExecutor(const SString&);
    
    PropertyNode    getPropTree(const SString&) const;
    PropertyNode    registerPropTree(const SString&,
                                     const SString&);
    virtual void    postInit();
    virtual bool    preClose();
    virtual void    aboutToSave();
    virtual void    newDocumentGrove();
    virtual void    beforeTransform();
    
    virtual void    executeUiEvent(const SString&, const UiAction&);
    
    bool            executeCommandEvent(const SString&) const;
    bool            executeCommandEvent(const SString&,
                                        const PropertyNode&) const;
    bool            executeCommandEvent(const SString&,
                                        const PropertyNode&,
                                        const PropertyNode&) const;

    static SString  sernaVersion();

    virtual ~DocumentPlugin();
};

