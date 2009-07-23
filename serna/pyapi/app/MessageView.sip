// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class MessageView : UiItem {

%TypeHeaderCode
#include "sapi/grove/Grove.h"
#include "sapi/app/MessageView.h"
using namespace SernaApi;
%End
public:
    enum MessageFacility {
        ALL_MSGS         = -1,
        GROVE_MSGS       = 1, 
        FORMATTER_MSGS   = 3, 
        VALIDATOR_MSGS   = 6, 
        SERNA_MSGS       = 10 
    };
    void            emitMessage(const SString&, const GroveNode* = 0);
    void            clearMessages(int = SERNA_MSGS);
    
    MessageView(const MessageView&);
};
