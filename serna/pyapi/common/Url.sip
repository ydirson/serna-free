// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class Url {

%TypeHeaderCode
#include "sapi/common/Url.h"
using namespace SernaApi;
%End

public:
    Url(const SString&);
    Url(const Url&);

    enum UrlProperty {
        PROTOCOL, 
        USER, 
        PASSWORD, 
        HOST, 
        PORT, 
        PATH, 
        QUERY, 
        DIRPATH, 
        FILENAME, 
        COLLPATH, 
        ENCODED_PATH_AND_QUERY
    };

    SString get(const UrlProperty) const;
    bool    isValid() const;
    bool    isLocal() const;
    bool    exists() const;
    bool    isRelative() const;
    Url     absolute() const;   
    Url     combinePath2Path(const Url&) const;
    Url     combineDir2Path(const Url&) const;
    Url     relativePath(const Url&) const;
    Url     upperUrl() const;
    bool    operator==(const Url&) const;
    bool    operator!=(const Url&) const;
    SString asString() const; 
    
    static SString encode(const SString&);
    static SString decode(const SString&);
};

