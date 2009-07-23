// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class UiAction : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/app/UiAction.h"
using namespace SernaApi;
%End

public:
    SString         name() const;
    PropertyNode    property(const SString&) const;
    PropertyNode    properties() const;

    SString         get(const SString&) const;
    SString         getTranslated(const SString&) const;
    bool            getBool(const SString&) const;
    int             getInt(const SString&) const;
    double          getDouble(const SString&) const;

    void            set(const SString&, const SString&);
    void            setBool(const SString&, bool);
    void            setInt(const SString&, int);
    void            setDouble(const SString&, double);

    bool            isEnabled() const;
    void            setEnabled(bool);
    
    bool            isToggleable() const;
    bool            isToggledOn() const;
    void            setToggled(bool);

    void            dispatch() const;
    void            dump() const;

    UiAction();
    ~UiAction();
};
