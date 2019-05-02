//
//  LogView.m
//  Volts
//
//  Created by Student on 2019-05-02.
//  Copyright © 2019 ApacheHB. All rights reserved.
//

#import "LogView.h"

@implementation LogView

- (void)awakeFromNib
{
    [super awakeFromNib];
    [EmuHandle addLogger:self];
}

- (void)appendLog:(NSView*)message
{
    [self.documentView addSubview:message];
}

@end
