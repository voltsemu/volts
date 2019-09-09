#include "DarwinGUI.h"

@interface DarwinGUI : NSWindow

@end

@implementation DarwinGUI

@end

namespace Volts::PS3
{
    DarwinGUI::DarwinGUI()
    {
        Handle = [DarwinGUI new];
    }
}