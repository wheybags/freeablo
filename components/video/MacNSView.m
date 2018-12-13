#include "MacNSView.h"
#import <Cocoa/Cocoa.h>

void *MacNSViewGetContentViewFromWindow(void *window)
{
    NSWindow *win = (__bridge NSWindow *)window;
    NSView *view = [win contentView];

    return (void *)CFBridgingRetain(view);
}

void MacNSViewFreeContentView(void *contentView)
{
	// Free the retained bridging pointer.
    CFBridgingRelease(contentView);
}
