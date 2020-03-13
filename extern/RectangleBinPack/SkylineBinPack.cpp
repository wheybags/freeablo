/** @file SkylineBinPack.cpp
    @author Jukka Jyl�nki

    @brief Implements different bin packer algorithms that use the SKYLINE data structure.

    This work is released to Public Domain, do whatever you want with it.
*/
#include <utility>
#include <iostream>
#include <limits>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <cmath>

#include "SkylineBinPack.h"

namespace rbp {

using namespace std;

SkylineBinPack::SkylineBinPack()
:binWidth(0), binHeight(0), binAllowFlip(false), usedSurfaceArea(0)
{
}

SkylineBinPack::SkylineBinPack(int width, int height, bool allowFlip)
{
    Init(width, height, allowFlip);
}

void SkylineBinPack::Init(int width, int height, bool allowFlip)
{
    binWidth = width;
    binHeight = height;

    binAllowFlip = allowFlip;

    usedSurfaceArea = 0;
    skyLine.clear();
    SkylineNode node;
    node.x = 0;
    node.y = 0;
    node.width = binWidth;
    skyLine.push_back(node);
}

void SkylineBinPack::Insert(std::vector<RectSize> &rects, std::vector<Rect> &dst, LevelChoiceHeuristic method)
{
    dst.clear();

    while(rects.size() > 0)
    {
        Rect bestNode = Rect();
        int bestScore1 = std::numeric_limits<int>::max();
        int bestScore2 = std::numeric_limits<int>::max();
        int bestSkylineIndex = -1;
        int bestRectIndex = -1;
        for(size_t i = 0; i < rects.size(); ++i)
        {
            Rect newNode = Rect();
            int score1 = std::numeric_limits<int>::max();
            int score2 = std::numeric_limits<int>::max();
            int index = 0;
            switch(method)
            {
            case LevelBottomLeft:
                newNode = FindPositionForNewNodeBottomLeft(rects[i].width, rects[i].height, score1, score2, index);
                break;
            case LevelMinWasteFit:
                newNode = FindPositionForNewNodeMinWaste(rects[i].width, rects[i].height, score2, score1, index);
                break;
            default: assert(false); break;
            }
            if (newNode.height != 0)
            {
                if (score1 < bestScore1 || (score1 == bestScore1 && score2 < bestScore2))
                {
                    bestNode = newNode;
                    bestScore1 = score1;
                    bestScore2 = score2;
                    bestSkylineIndex = index;
                    bestRectIndex = i;
                }
            }
        }

        if (bestRectIndex == -1)
            return;

        // Perform the actual packing.
        AddSkylineLevel(bestSkylineIndex, bestNode);
        usedSurfaceArea += rects[bestRectIndex].width * rects[bestRectIndex].height;
        rects.erase(rects.begin() + bestRectIndex);
        dst.push_back(bestNode);
    }
}

Rect SkylineBinPack::Insert(int width, int height, LevelChoiceHeuristic method)
{
    switch(method)
    {
    case LevelBottomLeft: return InsertBottomLeft(width, height);
    case LevelMinWasteFit: return InsertMinWaste(width, height);
    default: assert(false); return Rect();
    }
}

bool SkylineBinPack::RectangleFits(int skylineNodeIndex, int width, int height, int &y) const
{
    int x = skyLine[skylineNodeIndex].x;
    if (x + width > binWidth)
        return false;
    int widthLeft = width;
    int i = skylineNodeIndex;
    y = skyLine[skylineNodeIndex].y;
    while(widthLeft > 0)
    {
        y = max(y, skyLine[i].y);
        if (y + height > binHeight)
            return false;
        widthLeft -= skyLine[i].width;
        ++i;
        assert(i < (int)skyLine.size() || widthLeft <= 0);
    }
    return true;
}

int SkylineBinPack::ComputeWastedArea(int skylineNodeIndex, int width, int height, int y) const
{
    (void)height;
    int wastedArea = 0;
    const int rectLeft = skyLine[skylineNodeIndex].x;
    const int rectRight = rectLeft + width;
    for(; skylineNodeIndex < (int)skyLine.size() && skyLine[skylineNodeIndex].x < rectRight; ++skylineNodeIndex)
    {
        if (skyLine[skylineNodeIndex].x >= rectRight || skyLine[skylineNodeIndex].x + skyLine[skylineNodeIndex].width <= rectLeft)
            break;

        int leftSide = skyLine[skylineNodeIndex].x;
        int rightSide = min(rectRight, leftSide + skyLine[skylineNodeIndex].width);
        assert(y >= skyLine[skylineNodeIndex].y);
        wastedArea += (rightSide - leftSide) * (y - skyLine[skylineNodeIndex].y);
    }
    return wastedArea;
}

bool SkylineBinPack::RectangleFits(int skylineNodeIndex, int width, int height, int &y, int &wastedArea) const
{
    bool fits = RectangleFits(skylineNodeIndex, width, height, y);
    if (fits)
        wastedArea = ComputeWastedArea(skylineNodeIndex, width, height, y);

    return fits;
}

void SkylineBinPack::AddSkylineLevel(int skylineNodeIndex, const Rect &rect)
{
    SkylineNode newNode;
    newNode.x = rect.x;
    newNode.y = rect.y + rect.height;
    newNode.width = rect.width;
    skyLine.insert(skyLine.begin() + skylineNodeIndex, newNode);

    assert(newNode.x + newNode.width <= binWidth);
    assert(newNode.y <= binHeight);

    for(size_t i = skylineNodeIndex+1; i < skyLine.size(); ++i)
    {
        assert(skyLine[i-1].x <= skyLine[i].x);

        if (skyLine[i].x < skyLine[i-1].x + skyLine[i-1].width)
        {
            int shrink = skyLine[i-1].x + skyLine[i-1].width - skyLine[i].x;

            skyLine[i].x += shrink;
            skyLine[i].width -= shrink;

            if (skyLine[i].width <= 0)
            {
                skyLine.erase(skyLine.begin() + i);
                --i;
            }
            else
                break;
        }
        else
            break;
    }
    MergeSkylines();
}

void SkylineBinPack::MergeSkylines()
{
    for(size_t i = 0; i < skyLine.size()-1; ++i)
        if (skyLine[i].y == skyLine[i+1].y)
        {
            skyLine[i].width += skyLine[i+1].width;
            skyLine.erase(skyLine.begin() + (i+1));
            --i;
        }
}

Rect SkylineBinPack::InsertBottomLeft(int width, int height)
{
    int bestHeight;
    int bestWidth;
    int bestIndex;
    Rect newNode = FindPositionForNewNodeBottomLeft(width, height, bestHeight, bestWidth, bestIndex);

    if (bestIndex != -1)
    {
        // Perform the actual packing.
        AddSkylineLevel(bestIndex, newNode);

        usedSurfaceArea += width * height;
    }
    else
        memset(&newNode, 0, sizeof(Rect));

    return newNode;
}

Rect SkylineBinPack::FindPositionForNewNodeBottomLeft(int width, int height, int &bestHeight, int &bestWidth, int &bestIndex) const
{
    bestHeight = std::numeric_limits<int>::max();
    bestIndex = -1;
    // Used to break ties if there are nodes at the same level. Then pick the narrowest one.
    bestWidth = std::numeric_limits<int>::max();
    Rect newNode;
    memset(&newNode, 0, sizeof(newNode));
    for(size_t i = 0; i < skyLine.size(); ++i)
    {
        int y;
        if (RectangleFits(i, width, height, y))
        {
            if (y + height < bestHeight || (y + height == bestHeight && skyLine[i].width < bestWidth))
            {
                bestHeight = y + height;
                bestIndex = i;
                bestWidth = skyLine[i].width;
                newNode.x = skyLine[i].x;
                newNode.y = y;
                newNode.width = width;
                newNode.height = height;
            }
        }
        if (RectangleFits(i, height, width, y))
        {
            if (y + width < bestHeight || (y + width == bestHeight && skyLine[i].width < bestWidth))
            {
                bestHeight = y + width;
                bestIndex = i;
                bestWidth = skyLine[i].width;
                newNode.x = skyLine[i].x;
                newNode.y = y;
                newNode.width = height;
                newNode.height = width;
            }
        }
    }

    return newNode;
}

Rect SkylineBinPack::InsertMinWaste(int width, int height)
{
    int bestHeight;
    int bestWastedArea;
    int bestIndex;
    Rect newNode = FindPositionForNewNodeMinWaste(width, height, bestHeight, bestWastedArea, bestIndex);

    if (bestIndex != -1)
    {
        // Perform the actual packing.
        AddSkylineLevel(bestIndex, newNode);

        usedSurfaceArea += width * height;
    }
    else
        memset(&newNode, 0, sizeof(newNode));

    return newNode;
}

Rect SkylineBinPack::FindPositionForNewNodeMinWaste(int width, int height, int &bestHeight, int &bestWastedArea, int &bestIndex) const
{
    bestHeight = std::numeric_limits<int>::max();
    bestWastedArea = std::numeric_limits<int>::max();
    bestIndex = -1;
    Rect newNode;
    memset(&newNode, 0, sizeof(newNode));
    for(size_t i = 0; i < skyLine.size(); ++i)
    {
        int y;
        int wastedArea;

        if (RectangleFits(i, width, height, y, wastedArea))
        {
            if (wastedArea < bestWastedArea || (wastedArea == bestWastedArea && y + height < bestHeight))
            {
                bestHeight = y + height;
                bestWastedArea = wastedArea;
                bestIndex = i;
                newNode.x = skyLine[i].x;
                newNode.y = y;
                newNode.width = width;
                newNode.height = height;
            }
        }
        if (binAllowFlip)
        {
            if (RectangleFits(i, height, width, y, wastedArea))
            {
                if (wastedArea < bestWastedArea || (wastedArea == bestWastedArea && y + width < bestHeight))
                {
                    bestHeight = y + width;
                    bestWastedArea = wastedArea;
                    bestIndex = i;
                    newNode.x = skyLine[i].x;
                    newNode.y = y;
                    newNode.width = height;
                    newNode.height = width;
                }
            }
        }
    }

    return newNode;
}

/// Computes the ratio of used surface area.
float SkylineBinPack::Occupancy() const
{
    return (float)usedSurfaceArea / (binWidth * binHeight);
}

}
