#include "atlastexture.h"
#include "../../extern/RectangleBinPack/SkylineBinPack.h"
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
        static constexpr int32_t requiredTextureLayers = 2;

        BaseTextureInfo textureInfo{};
        textureInfo.width = requiredTextureSize;
        textureInfo.height = requiredTextureSize;
        textureInfo.arrayLayers = requiredTextureLayers;
        textureInfo.format = Format::RGBA8UNorm;
        textureInfo.minFilter = Filter::Nearest;
        textureInfo.magFilter = Filter::Nearest;

        mTextureArray = mInstance.createTexture(textureInfo);

        this->clear(commandQueue);
    }

    AtlasTexture::~AtlasTexture() = default;

    size_t AtlasTexture::addTexture(int32_t width, int32_t height, const void* data)
    {
        rbp::Rect dataDestinationRect = {};
        int32_t layer = -1;
        {
            int32_t paddedWidth = width + PADDING;
            int32_t paddedHeight = height + PADDING;

            release_assert(paddedWidth <= mTextureArray->width() + PADDING * 2 && paddedHeight <= mTextureArray->height() + PADDING * 2);

            for (int32_t layerTmp = 0; layerTmp < mTextureArray->getInfo().arrayLayers; layerTmp++)
            {
                rbp::Rect packedPos = mBinPacker[layerTmp]->Insert(paddedWidth, paddedHeight, rbp::SkylineBinPack::LevelMinWasteFit);
                if (packedPos.height != 0)
                {
                    dataDestinationRect = {packedPos.x + PADDING, packedPos.y + PADDING, width, height};
                    layer = layerTmp;
                    break;
                }
            }

            release_assert(layer != -1);
        }

        mTextureArray->updateImageData(dataDestinationRect.x, dataDestinationRect.y, layer, width, height, reinterpret_cast<const uint8_t*>(data));

        auto id = mNextTextureId++;
        mLookupMap[id] = AtlasTextureEntry(dataDestinationRect.x, dataDestinationRect.y, layer, width, height);

        return id;
    }

    float AtlasTexture::getOccupancy() const
    {
        float summedOccupancy = 0;
        for (auto& bp : mBinPacker)
            summedOccupancy += bp->Occupancy();
        return summedOccupancy / mBinPacker.size() * 100;
    }

    void AtlasTexture::clear(CommandQueue& commandQueue)
    {
        mBinPacker.clear();
        for (int32_t layer = 0; layer < mTextureArray->getInfo().arrayLayers; layer++)
            mBinPacker.push_back(std::make_unique<rbp::SkylineBinPack>(mTextureArray->width() + PADDING * 2, mTextureArray->height() + PADDING * 2, false));

        commandQueue.cmdClearTexture(*mTextureArray, Colors::transparent);
    }
}
