// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class UiActionSet {

%TypeHeaderCode
#include "sapi/app/UiDocument.h"
using namespace SernaApi;
%End

public:
    UiAction   findAction(const SString&) const;
    void       removeAction(const UiAction&);
    
    UiAction   makeAction(const SString&, 
                            const SString& = SString(),
                            const SString& = SString(),
                            const SString& = SString(), 
                            const SString& = SString(),
                            const SString& = SString(), bool = false);
    UiAction   makeAction(const PropertyNode&);
};    

class UiItemMaker : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/app/UiDocument.h"
using namespace SernaApi;
%End
public:
    static UiItem   makeLiquidItem(const PropertyNode&,
                                   UiLiquidItemBase* /Transfer/);

    virtual UiItem makeItem(const UiAction&, 
                            const PropertyNode&) = 0;
    virtual ~UiItemMaker();

protected:
    UiItemMaker();
private:
    UiItemMaker(const UiItemMaker&);
};

class UiDocument : UiItem {

%TypeHeaderCode
#include "sapi/app/UiDocument.h"
using namespace SernaApi;
%End
public:
    PropertyNode addItemMaker(const SString&,
                              UiItemMaker* /Transfer/,
                              const SString& = SString());
    UiItem   makeItem(const SString&,
                      const PropertyNode&) const;
    void     makeAndShow(const SString&,
                         const PropertyNode&);

    UiActionSet  actionSet();
    bool operator==(const UiDocument&) const;
    bool operator!=(const UiDocument&) const;
};

