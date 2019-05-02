#import <Foundation/Foundation.h>

#import "External.h"

#include <Volts/Core/Volts.h>
#include <Volts/Platform/Interface/Native.h>

struct MacNative : public Volts::Native
{
    LogView* Log;
    
    MacNative() {}
    
    virtual ~MacNative() override
    {
        
    }
    
    virtual void WriteLog(const char* Channel, Volts::Level Severity, const char* Message) override
    {
        NSView* View = [NSView new];
        View.translatesAutoresizingMaskIntoConstraints = NO;
        View.wantsLayer = YES;
        View.layer.backgroundColor = [NSColor redColor].CGColor;
        [Log appendLog:View];
    }
};

static MacNative* NativeHandle = nullptr;

@implementation EmuHandle : NSObject

+ (void)setup
{
    NSLog(@"Setup");
    NativeHandle = new MacNative();
}

+ (void)addLogger:(id)logger
{
    NSLog(@"Logger");
    NativeHandle->Log = (LogView*)logger;
}

+ (void)test
{
    VoltsMain(NativeHandle);
}

@end
