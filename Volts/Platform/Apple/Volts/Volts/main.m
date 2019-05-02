#import "Interop/External.h"
#import <Cocoa/Cocoa.h>

int main(int argc, const char * argv[]) {
    [EmuHandle setup];
    return NSApplicationMain(argc, argv);
}
