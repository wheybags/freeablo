#ifndef MAC_NS_VIEW_H
#define MAC_NS_VIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/* Returns the NSView from a NSWindow (i.e. calls NSWindow->contentView).
 * MacNSViewFreeContentView should be called to release the pointer when finished.
 */
void* MacNSViewGetContentViewFromWindow(void* window);
void MacNSViewFreeContentView(void* contentView);

#ifdef __cplusplus
}
#endif

#endif
