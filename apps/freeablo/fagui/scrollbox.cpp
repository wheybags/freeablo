#include "scrollbox.h"

namespace FAGui
{
ScrollBox::ScrollBox(Rocket::Core::ElementDocument* document) :
    mScrollBoxValue(0.0f),
    mSpeed(0.0f),
    mIsScrollBoxLoaded(false)
{
    static const float DEFAULT_SPEED_VALUE = 0.0002f;

    mScrollBox = document->GetElementById("scrollbox");
    if(mScrollBox)
    {
        mIsScrollBoxLoaded = true;

        auto speed = mScrollBox->GetProperty("speed");
        if(speed)
        {
            mSpeed = speed->Get<float>();
        }
        else
        {
            mSpeed = DEFAULT_SPEED_VALUE;
        }

    }
}

void ScrollBox::update()
{
    if(!mIsScrollBoxLoaded)
    {
        return;
    }

    static const int MAGIC_INDEX = 2;

    mScrollBoxValue += mSpeed;

    Rocket::Core::Dictionary parameters;
    parameters.Set<float>("value", mScrollBoxValue);

    auto child = mScrollBox->GetChild(MAGIC_INDEX);
    if(child)
    {
        child->DispatchEvent("scrollchange", parameters, false);
    }
}

void ScrollBox::reset()
{
    mScrollBoxValue = 0.0f;
}

bool ScrollBox::isFinished() const
{
    return mScrollBoxValue >= 1.0f;
}


}
