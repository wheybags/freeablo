#include "atlastexture.h"
#include "texture.h"
#include <memory>
#include <misc/assert.h>
#include <render/commandqueue.h>
#include <render/renderinstance.h>

/* Stores many small textures into a large texture (or array of textures)
 * to allow batched drawing commands that increase performance. */

namespace Render
{
    AtlasTexture::AtlasTexture(RenderInstance& instance, CommandQueue& commandQueue) : mInstance(instance)
    {
        // TODO: dynamic configuration of atlas size

        static constexpr int32_t requiredTextureSize = 8192;
        static constexpr int32_t requiredTextureLayers = 3;

        BaseTextureInfo textureInfo{};
        textureInfo.width = requiredTextureSize;
        textureInfo.height = requiredTextureSize;
        textureInfo.arrayLayers = requiredTextureLayers;
        textureInfo.format = Format::RGBA8UNorm;
        textureInfo.minFilter = Filter::Nearest;
        textureInfo.magFilter = Filter::Nearest;

        mTextureArray = mInstance.createTexture(textureInfo);

        this->clear(commandQueue);

        Image blankImage(2, 2);
        this->mEmptySpriteId = this->addTexture(blankImage, false);
    }

    AtlasTexture::~AtlasTexture() = default;

    size_t AtlasTexture::addTexture(const Image& image, bool trim, std::optional<Image::TrimmedData> _trimmedData)
    {
        std::unique_ptr<Image> imageTmp;

        const Image* useImage = &image;
        int32_t trimmedOffsetX = 0;
        int32_t trimmedOffsetY = 0;
        int32_t originalWidth = image.width();
        int32_t originalHeight = image.height();

        if (_trimmedData)
        {
            if (image.width() == 0 || image.height() == 0)
                return mEmptySpriteId;

            const Image::TrimmedData& trimmedData = _trimmedData.value();

            trimmedOffsetX = trimmedData.trimmedOffsetX;
            trimmedOffsetY = trimmedData.trimmedOffsetY;
            originalWidth = trimmedData.originalWidth;
            originalHeight = trimmedData.originalHeight;
        }
        else if (trim)
        {
            bool isEmpty = true;

            int32_t left = image.width() - 1;
            int32_t right = 0;
            int32_t top = image.height() - 1;
            int32_t bottom = 0;

            for (int32_t y = 0; y < image.height(); y++)
            {
                for (int32_t x = 0; x < image.width(); x++)
                {
                    if (image.get(x, y).a != 0)
                    {
                        isEmpty = false;

                        left = std::min(left, x);
                        right = std::max(right, x);
                        top = std::min(top, y);
                        bottom = std::max(bottom, y);
                    }
                }
            }

            if (isEmpty)
                return mEmptySpriteId;

            imageTmp = std::make_unique<Image>(right - left + 1, bottom - top + 1);
            image.blitTo(*imageTmp, left, top, imageTmp->width(), imageTmp->height(), 0, 0);

            useImage = imageTmp.get();
            trimmedOffsetX = left;
            trimmedOffsetY = top;
        }

        RectPacker::Rect dataDestinationRect = {};
        int32_t layer = -1;
        {
            int32_t paddedWidth = useImage->width() + PADDING;
            int32_t paddedHeight = useImage->height() + PADDING;

            release_assert(paddedWidth <= mTextureArray->width() + PADDING * 2 && paddedHeight <= mTextureArray->height() + PADDING * 2);

            for (int32_t layerTmp = 0; layerTmp < mTextureArray->getInfo().arrayLayers; layerTmp++)
            {
                RectPacker::Rect packedPos{0, 0, paddedWidth, paddedHeight};
                if (mRectPacker[layerTmp]->addRect(packedPos))
                {
                    dataDestinationRect = {packedPos.x + PADDING, packedPos.y + PADDING, useImage->width(), useImage->height()};
                    layer = layerTmp;
                    break;
                }
            }

            release_assert(layer != -1);
        }

        mTextureArray->updateImageData(dataDestinationRect.x,
                                       dataDestinationRect.y,
                                       layer,
                                       useImage->width(),
                                       useImage->height(),
                                       reinterpret_cast<const uint8_t*>(useImage->mData.data()));

        AtlasTextureEntry atlasEntry = {};
        atlasEntry.mX = dataDestinationRect.x;
        atlasEntry.mY = dataDestinationRect.y;
        atlasEntry.mLayer = layer;
        atlasEntry.mWidth = originalWidth;
        atlasEntry.mHeight = originalHeight;
        atlasEntry.mTrimmedOffsetX = trimmedOffsetX;
        atlasEntry.mTrimmedOffsetY = trimmedOffsetY;
        atlasEntry.mTrimmedWidth = useImage->width();
        atlasEntry.mTrimmedHeight = useImage->height();

        size_t id = mNextTextureId++;
        mLookupMap[id] = atlasEntry;
        return id;
    }

    void AtlasTexture::printUtilisation() const
    {
        float summedOccupancy = 0;
        for (int32_t i = 0; i < int32_t(mRectPacker.size()); i++)
        {
            const auto& bp = mRectPacker[i];
            printf("Atlas texture layer %d occupancy %.1f%%\n", i, bp->utilisation() * 100.0f);
            summedOccupancy += bp->utilisation();
        }

        printf("Atlas texture total occupancy %.1f%%\n", (summedOccupancy / mRectPacker.size()) * 100.0f);
    }

    void AtlasTexture::clear(CommandQueue& commandQueue)
    {
        mRectPacker.clear();
        for (int32_t layer = 0; layer < mTextureArray->getInfo().arrayLayers; layer++)
            mRectPacker.push_back(std::make_unique<RectPacker>(mTextureArray->width() - PADDING * 2, mTextureArray->height() - PADDING * 2));

        commandQueue.cmdClearTexture(*mTextureArray, Colors::transparent);
    }
}
