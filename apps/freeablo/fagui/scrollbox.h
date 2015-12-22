#ifndef SCROLLBOX_H
#define SCROLLBOX_H

#include <Rocket/Core.h>

namespace FAGui
{
class ScrollBox
{
public:
    ScrollBox(Rocket::Core::ElementDocument* document);
    void reset();
    void update();
    bool isFinished() const;

private:
    float mScrollBoxValue;
    float mSpeed;
    bool mIsScrollBoxLoaded;
    Rocket::Core::Element* mScrollBox;
    Rocket::Core::ElementScroll* mScroll;
};
}
#endif
