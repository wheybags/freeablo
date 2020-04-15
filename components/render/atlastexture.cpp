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
    AtlasTexture::AtlasTexture(RenderInstance& instance, CommandQueue& commandQueue) : mInstance(instance), mCommandQueue(commandQueue)
    {
        release_assert(mInstance.capabilities().maxTextureSize >= MINIMUM_ATLAS_SIZE);
    }

    AtlasTexture::~AtlasTexture() = default;

    std::vector<size_t> AtlasTexture::addCategorySprites(const std::string& category, const std::vector<LoadImageData>& images)
    {
        Layers& categoryLayers = mLayersByCategory[category];

        int64_t requiredSize = 0;
        for (const auto& imageData : images)
            requiredSize += (imageData.image.width() + PADDING) * (imageData.image.height() + PADDING);

        while (requiredSize > 0)
        {
            int32_t layerSize = mInstance.capabilities().maxTextureSize;
            int64_t requiredSizePadded = requiredSize + (requiredSize / 100) * 2;

            while (((layerSize / 2) * (layerSize / 2)) > requiredSizePadded)
                layerSize = layerSize / 2;

            requiredSize -= layerSize * layerSize;

            int32_t layerWidth = layerSize;
            int32_t layerHeight = layerSize;

            // If we are about to create a layer where we're likely to waste a large chunk of it, just halve the height
            if (requiredSize < 0 && layerSize != MINIMUM_ATLAS_SIZE)
            {
                int64_t unusedSpace = std::abs(requiredSize);
                int64_t unusedSpacePadded = unusedSpace - (unusedSpace / 100) * 2;

                if (unusedSpacePadded > (layerWidth * layerWidth) / 2)
                    layerHeight = layerHeight / 2;
            }

            // Minimum size layers are added at need when we are actually uploading the data, no need to create it here.
            // This means we shouldn't ever accidentally create a small layer at the end that we never use.
            if (layerWidth != MINIMUM_ATLAS_SIZE || layerHeight != MINIMUM_ATLAS_SIZE)
                categoryLayers.addLayer(mInstance, mCommandQueue, layerWidth, layerHeight);
        }

        Image blankImage(1, 1);
        categoryLayers.emptySpriteId = this->addTexture(blankImage, std::nullopt, category);

        std::vector<size_t> ids;
        ids.reserve(images.size());

        for (const auto& imageData : images)
            ids.push_back(addTexture(imageData.image, imageData.trimmedData, category));

        return ids;
    }

    size_t AtlasTexture::addTexture(const Image& image, std::optional<Image::TrimmedData> _trimmedData, std::string category)
    {
        Layers& categoryLayers = mLayersByCategory.at(category);

        std::unique_ptr<Image> imageTmp;

        const Image* useImage = &image;
        int32_t trimmedOffsetX = 0;
        int32_t trimmedOffsetY = 0;
        int32_t originalWidth = image.width();
        int32_t originalHeight = image.height();

        if (_trimmedData)
        {
            if (image.width() == 0 || image.height() == 0)
                return categoryLayers.emptySpriteId;

            const Image::TrimmedData& trimmedData = _trimmedData.value();

            trimmedOffsetX = trimmedData.trimmedOffsetX;
            trimmedOffsetY = trimmedData.trimmedOffsetY;
            originalWidth = trimmedData.originalWidth;
            originalHeight = trimmedData.originalHeight;
        }

        RectPacker::Rect dataDestinationRect = {};
        Layer* layer = nullptr;
        {
            int32_t paddedWidth = useImage->width() + PADDING;
            int32_t paddedHeight = useImage->height() + PADDING;

            for (int32_t layerIndex = 0;; layerIndex++)
            {
                if (layerIndex >= int32_t(categoryLayers.layers.size()))
                {
                    // If we have a huge texture that won't fit, we just make a dedicated layer for it, otherwise we add a new layer for general use
                    if ((useImage->width() + PADDING * 2) >= MINIMUM_ATLAS_SIZE || (useImage->height() + PADDING * 2) >= MINIMUM_ATLAS_SIZE)
                        categoryLayers.addLayer(mInstance, mCommandQueue, useImage->width() + PADDING * 2, useImage->height() + PADDING * 2);
                    else
                        categoryLayers.addLayer(mInstance, mCommandQueue, MINIMUM_ATLAS_SIZE, MINIMUM_ATLAS_SIZE);
                }

                layer = &categoryLayers.layers[layerIndex];

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
                int32_t width = categoryLayer.layers[i].texture->width();
                int32_t height = categoryLayer.layers[i].texture->height();
                printf("        Layer %d (%dx%d) utilisation %.1f%%\n", i, width, height, rectPacker.utilisation() * 100.0f);
                summedOccupancy += rectPacker.utilisation();
            }

            if (categoryLayer.layers.size() > 1)
                printf("        All layers utilisation %.1f%%\n", (summedOccupancy / categoryLayer.layers.size()) * 100.0f);
        }
    }

    void AtlasTexture::Layers::addLayer(RenderInstance& instance, CommandQueue& commandQueue, int32_t width, int32_t height)
    {
        Layer newLayer;
        newLayer.rectPacker = std::make_unique<RectPacker>(width - PADDING, height - PADDING);

        {
            BaseTextureInfo textureInfo{};
            textureInfo.width = width;
            textureInfo.height = height;
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
