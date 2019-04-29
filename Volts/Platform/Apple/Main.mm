#import <Foundation/Foundation.h>

#include "../Interface/EmulatorWindow.h"
#include <Volts/Core/Volts.h>

struct MEmuWindow : Volts::EmulatorWindow
{
    virtual ~MEmuWindow() override {}
    virtual void WriteLog(const char* Channel, char Severity, const char* Message) override
    {
        NSLog(@"%d %s: %s", Severity, Channel, Message);
    }
};

int main(int argc, const char** argv)
{
    MEmuWindow* Win = new MEmuWindow();
    return VoltsMain(Win);
}
