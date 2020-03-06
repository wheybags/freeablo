#include <diabloexe/characterstats.h>
#include <faworld/player.h>
#include <gtest/gtest.h>

class TestWorld
{
public:
    TestWorld(FAWorld::PlayerClass playerClass) : exe(""), world(exe, 0), player(world, playerClass, DiabloExe::CharacterStats()) {}

    DiabloExe::DiabloExe exe;
    FAWorld::World world;
    FAWorld::Player player;
};

void assertRangesEqual(IntRange expected, IntRange range, int32_t base) { ASSERT_EQ(expected, IntRange(range.min + base, range.max + base)); }

TEST(CombatFormulas, CharacterBaseMeleeDamage)
{
    TestWorld world(FAWorld::PlayerClass::warrior);

    world.player.mStats.mLevel = 10;
    world.player.mStats.baseStats.strength = 9;
    auto stats = world.player.getStats().getCalculatedStats();
    assertRangesEqual(IntRange(1, 1), stats.meleeDamageBonusRange, stats.meleeDamage);

    world.player.mStats.mLevel = 10;
    world.player.mStats.baseStats.strength = 10;
    stats = world.player.getStats().getCalculatedStats();
    assertRangesEqual(IntRange(2, 2), stats.meleeDamageBonusRange, stats.meleeDamage);
}

TEST(CombatFormulas, ToHitMelee)
{
    TestWorld world(FAWorld::PlayerClass::warrior);

    world.player.mStats.mLevel = 10;
    world.player.mStats.baseStats.dexterity = 20;
    ASSERT_EQ(60, world.player.getStats().getCalculatedStats().toHitMelee.base);

    world.player.mStats.mLevel = 10;
    world.player.mStats.baseStats.dexterity = 10;
    ASSERT_EQ(55, world.player.getStats().getCalculatedStats().toHitMelee.base);

    world.player.mStats.mLevel = 2;
    world.player.mStats.baseStats.dexterity = 22;
    ASSERT_EQ(61, world.player.getStats().getCalculatedStats().toHitMelee.base);
}

TEST(CombatFormulas, ArmorClass)
{
    TestWorld world(FAWorld::PlayerClass::warrior);

    world.player.mStats.mLevel = 10;
    world.player.mStats.baseStats.dexterity = 20;
    ASSERT_EQ(4, world.player.getStats().getCalculatedStats().armorClass);

    world.player.mStats.mLevel = 10;
    world.player.mStats.baseStats.dexterity = 10;
    ASSERT_EQ(2, world.player.getStats().getCalculatedStats().armorClass);

    world.player.mStats.mLevel = 2;
    world.player.mStats.baseStats.dexterity = 27;
    ASSERT_EQ(5, world.player.getStats().getCalculatedStats().armorClass);
}

TEST(CombatFormulas, Life)
{
    TestWorld world(FAWorld::PlayerClass::warrior);
    world.player.mStats.mLevel = 1;

    world.player.mStats.baseStats.vitality = 25;
    ASSERT_EQ(70, world.player.getStats().getHp().max);

    world.player.mStats.mLevel = 2;
    world.player.mStats.baseStats.vitality = 25;
    ASSERT_EQ(72, world.player.getStats().getHp().max);

    world.player.mStats.mLevel = 2;
    world.player.mStats.baseStats.vitality = 30;
    ASSERT_EQ(82, world.player.getStats().getHp().max);
}

TEST(CombatFormulas, Mana)
{
    TestWorld world(FAWorld::PlayerClass::warrior);
    world.player.mStats.mLevel = 1;

    world.player.mStats.baseStats.magic = 10;
    ASSERT_EQ(10, world.player.getStats().getMana().max);

    world.player.mStats.mLevel = 2;
    world.player.mStats.baseStats.magic = 10;
    ASSERT_EQ(11, world.player.getStats().getMana().max);

    world.player.mStats.mLevel = 2;
    world.player.mStats.baseStats.magic = 15;
    ASSERT_EQ(16, world.player.getStats().getMana().max);
}
