#import <Foundation/Foundation.h>
#import "Window.h"

#include <Core/Volts.h>
#include <Platform/Interface/EmulatorWindow.h>

struct MEmuWindow : public Volts::EmulatorWindow
{
    EmuWindow* Win;
    
    MEmuWindow(EmuWindow* Window)
    {
        Win = Window;
    }
    
    virtual ~MEmuWindow() override {}
    
    virtual void WriteLog(const char* Channel, Volts::Level S, const char* Message) override
    {
        
        NSLog(@"%s %s", Channel, Message);
    }
};


@implementation EmuWindow

+ (void)init
{
    auto* Emu = new MEmuWindow(self);
    VoltsMain(Emu);
}

@end
