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
    AtlasTexture::AtlasTexture(RenderInstance& instance, CommandQueue& commandQueue) : mInstance(instance), mCommandQueue(commandQueue) {}

    AtlasTexture::~AtlasTexture() = default;

    size_t AtlasTexture::addTexture(const Image& image, bool trim, std::optional<Image::TrimmedData> _trimmedData, std::string category)
    {
        Layers* categoryLayers = nullptr;
        {
            auto it = mLayersByCategory.find(category);
            if (it == mLayersByCategory.end())
            {
                categoryLayers = &mLayersByCategory[category];

                Image blankImage(2, 2);
                categoryLayers->emptySpriteId = this->addTexture(blankImage, false);
            }
            else
            {
                categoryLayers = &it->second;
            }
        }

        std::unique_ptr<Image> imageTmp;

        const Image* useImage = &image;
        int32_t trimmedOffsetX = 0;
        int32_t trimmedOffsetY = 0;
        int32_t originalWidth = image.width();
        int32_t originalHeight = image.height();

        if (_trimmedData)
        {
            if (image.width() == 0 || image.height() == 0)
                return categoryLayers->emptySpriteId;

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
                return categoryLayers->emptySpriteId;

            imageTmp = std::make_unique<Image>(right - left + 1, bottom - top + 1);
            image.blitTo(*imageTmp, left, top, imageTmp->width(), imageTmp->height(), 0, 0);

            useImage = imageTmp.get();
            trimmedOffsetX = left;
            trimmedOffsetY = top;
        }

        RectPacker::Rect dataDestinationRect = {};
        Layer* layer = nullptr;
        {
            int32_t paddedWidth = useImage->width() + PADDING;
            int32_t paddedHeight = useImage->height() + PADDING;

            // release_assert(paddedWidth <= mTextureArray->width() + PADDING * 2 && paddedHeight <= mTextureArray->height() + PADDING * 2);

            for (int32_t layerIndex = 0;; layerIndex++)
            {
                if (layerIndex >= int32_t(categoryLayers->layers.size()))
                    categoryLayers->addLayer(mInstance, mCommandQueue);

                layer = &categoryLayers->layers[layerIndex];

                RectPacker::Rect packedPos{0, 0, paddedWidth, paddedHeight};
                if (layer->rectPacker->addRect(packedPos))
                {
                    dataDestinationRect = {packedPos.x + PADDING, packedPos.y + PADDING, useImage->width(), useImage->height()};
                    break;
                }
            }
        }

        layer->texture->updateImageData(
            dataDestinationRect.x, dataDestinationRect.y, 0, useImage->width(), useImage->height(), reinterpret_cast<const uint8_t*>(useImage->mData.data()));

        AtlasTextureEntry atlasEntry = {};
        atlasEntry.mX = dataDestinationRect.x;
        atlasEntry.mY = dataDestinationRect.y;
        atlasEntry.mLayer = 0;
        atlasEntry.mWidth = originalWidth;
        atlasEntry.mHeight = originalHeight;
        atlasEntry.mTrimmedOffsetX = trimmedOffsetX;
        atlasEntry.mTrimmedOffsetY = trimmedOffsetY;
        atlasEntry.mTrimmedWidth = useImage->width();
        atlasEntry.mTrimmedHeight = useImage->height();
        atlasEntry.mTexture = layer->texture.get();

        size_t id = mNextTextureId++;
        mLookupMap[id] = atlasEntry;
        return id;
    }

    void AtlasTexture::printUtilisation() const
    {
        printf("Texture atlas utilisation by category:\n");

        for (const auto& pair : mLayersByCategory)
        {
            printf("    %s:\n", pair.first.c_str());

            const Layers& categoryLayer = pair.second;

            float summedOccupancy = 0;
            for (int32_t i = 0; i < int32_t(categoryLayer.layers.size()); i++)
            {
                const RectPacker& rectPacker = *categoryLayer.layers[i].rectPacker;
                printf("        Layer %d occupancy %.1f%%\n", i, rectPacker.utilisation() * 100.0f);
                summedOccupancy += rectPacker.utilisation();
            }

            printf("        All layers occupancy %.1f%%\n", (summedOccupancy / categoryLayer.layers.size()) * 100.0f);
        }
    }

    void AtlasTexture::Layers::addLayer(RenderInstance& instance, CommandQueue& commandQueue)
    {
        static constexpr int32_t requiredTextureSize = 4096;

        Layer newLayer;
        newLayer.rectPacker = std::make_unique<RectPacker>(requiredTextureSize - PADDING * 2, requiredTextureSize - PADDING * 2);

        {
            BaseTextureInfo textureInfo{};
            textureInfo.width = requiredTextureSize;
            textureInfo.height = requiredTextureSize;
            textureInfo.arrayLayers = 1;
            textureInfo.forceTextureToBeATextureArray = true;
            textureInfo.format = Format::RGBA8UNorm;
            textureInfo.minFilter = Filter::Nearest;
            textureInfo.magFilter = Filter::Nearest;

            newLayer.texture = instance.createTexture(textureInfo);
            commandQueue.cmdClearTexture(*newLayer.texture, Colors::transparent);
        }

        layers.push_back(std::move(newLayer));
    }
}
