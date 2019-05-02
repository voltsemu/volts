#import "../LogView.h"
#import <Cocoa/Cocoa.h>

@interface EmuHandle : NSObject

+ (void)setup;

+ (void)addLogger:(id)logger;

+ (void)test;

@end
