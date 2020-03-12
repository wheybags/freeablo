#include <falevelgen/levelgen.h>
#include <falevelgen/tileset.h>
#include <gtest/gtest.h>
#include <misc/md5.h>
#include <random/random.h>
#include <serial/textstream.h>

TEST(LevelGen, BasicDeterminism)
{
    Random::RngMersenneTwister random(1234);

    FALevelGen::TileSet tileset(Misc::getResourcesPath().str() + "/tilesets/l1.ini");
    Level::Dun level = FALevelGen::generateBasic(random, tileset, 100, 100, 1);

    Serial::TextWriteStream saveStream;
    Serial::Saver saver(saveStream);
    level.save(saver);
    auto data = saveStream.getData();

    Misc::md5_byte_t digest[16];
    {
        Misc::md5_state_t state;
        md5_init(&state);
        md5_append(&state, static_cast<Misc::md5_byte_t*>(data.first), data.second);
        md5_finish(&state, digest);
    }

    std::string hash;
    {
        std::stringstream s;
        for (int32_t i = 0; i < 16; i++)
            s << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
        hash = s.str();
    }

    // feel free to update this hash if you have changed level generation
    ASSERT_EQ(hash, "21c715f734eef74c05c04fe34b9ed51d");
}
