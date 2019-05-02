//
//  LogView.h
//  Volts
//
//  Created by Student on 2019-05-02.
//  Copyright © 2019 ApacheHB. All rights reserved.
//

#import "Interop/External.h"

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface LogView : NSScrollView

- (void)appendLog:(NSView*)message;

@end

NS_ASSUME_NONNULL_END
