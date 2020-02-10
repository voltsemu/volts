#include "alert.h"

#import <Cocoa/Cocoa.h>

namespace alert
{
    void alert(const std::wstring& title, const std::wstring& message)
    {
        NSString* msg = [
            [NSString alloc] 
            initWithBytes:message.data()
            length:message.size() * sizeof(wchar_t)
            encoding:NSUTF32LittleEndianStringEncoding
        ];
        
        NSString* tl = [
            [NSString alloc] 
            initWithBytes:title.data()
            length:title.size() * sizeof(wchar_t)
            encoding:NSUTF32LittleEndianStringEncoding
        ];
        
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText:tl];
        [alert setInformativeTest:msg];
        [alert addButtonWithTitle:@"Ok"];
        [alert runModal];
    }
}