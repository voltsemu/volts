#include "alert.h"

#import <Cocoa/Cocoa.h>

namespace alert
{
    void alert(const char* title, const char* message)
    {
        NSString* msg = [
            NSString stringWithString:message
            encoding:[NSString defaultCStringEncoding]
        ];
        
        NSString* tl = [
            NSString stringWithString:title
            encoding:[NSString defaultCStringEncoding]
        ];
        
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText:tl];
        [alert setInformativeText:msg];
        [alert addButtonWithTitle:@"Ok"];
        [alert runModal];
    }
}