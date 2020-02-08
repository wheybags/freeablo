#include <filesystem/path.h>
#include <fstream>
#include <gtest/gtest.h>
#include <limits>
#include <settings/settings.h>

namespace bfs = filesystem;

class SettingsTest : public ::testing::Test
{
protected:
    SettingsTest();
    bool isContains(const Settings::Container& container, const std::string& entry);

    static const bfs::path THIS_FOLDER;
    static const bfs::path DEFAULT_INI_PATH;
    static const bfs::path USER_INI_PATH;

    Settings::Settings mSettings;
};

const bfs::path SettingsTest::THIS_FOLDER = bfs::path(__FILE__).parent_path();
const bfs::path SettingsTest::DEFAULT_INI_PATH = SettingsTest::THIS_FOLDER / "testdefault.ini";
const bfs::path SettingsTest::USER_INI_PATH = SettingsTest::THIS_FOLDER / "testuser.ini";

SettingsTest::SettingsTest() : mSettings(DEFAULT_INI_PATH.str(), USER_INI_PATH.str()) { mSettings.loadUserSettings(); }

bool SettingsTest::isContains(const Settings::Container& container, const std::string& entry)
{
    return container.cend() != std::find(container.cbegin(), container.cend(), entry);
}

TEST_F(SettingsTest, TestIsSectionExists)
{
    ASSERT_TRUE(mSettings.isSectionExists("Section0"));
    ASSERT_TRUE(mSettings.isSectionExists("Section1"));
    ASSERT_FALSE(mSettings.isSectionExists("NonExistingSection"));
}

TEST_F(SettingsTest, TestGetSections)
{
    const Settings::Container sections = mSettings.getSections();

    ASSERT_EQ(3, sections.size());
    ASSERT_TRUE(isContains(sections, ""));
    ASSERT_TRUE(isContains(sections, "Section0"));
    ASSERT_TRUE(isContains(sections, "Section1"));
    ASSERT_FALSE(isContains(sections, "withoutSection"));
}

TEST_F(SettingsTest, TestSave)
{
    const std::string originalContent =
        R"([Section0]
testProperty0=testValue0
testProperty1=testValue1
)";

    const bfs::path saveTestOriginalPath = THIS_FOLDER / "testsaveoriginal.ini";
    std::ofstream originalFile(saveTestOriginalPath.str());
    originalFile << originalContent;
    originalFile.close();
    mSettings.loadFromFile(saveTestOriginalPath.str());

    const std::string sectionName = "Section0";
    mSettings.set<std::string>(sectionName, "testProperty0", "SaveTestValue0");
    mSettings.set<std::string>(sectionName, "testProperty1", "SaveTestValue1");
    mSettings.save();

    const bfs::path saveTestModifiedPath = THIS_FOLDER / "testsavemodified.ini";
    bfs::copy_file(saveTestOriginalPath, saveTestModifiedPath);

    // FIXME: Is there a way for simpleini to keep the formatting around '=' signs?
    const std::string expectedContent =
        R"([Section0]
testProperty0 = SaveTestValue0
testProperty1 = SaveTestValue1
)";

    std::ifstream modifiedFile(saveTestModifiedPath.str());
    const std::string actualContent((std::istreambuf_iterator<char>(modifiedFile)), (std::istreambuf_iterator<char>()));
    modifiedFile.close();
    bfs::remove(saveTestOriginalPath);
    bfs::remove(saveTestModifiedPath);

    ASSERT_EQ(expectedContent, actualContent);
}

TEST_F(SettingsTest, TestGetPropertiesInSection)
{
    const Settings::Container properties = mSettings.getPropertiesInSection("Section0");
    ASSERT_EQ(2, properties.size());
    ASSERT_TRUE(isContains(properties, "testProperty0"));
    ASSERT_TRUE(isContains(properties, "testProperty1"));
}

TEST_F(SettingsTest, TestGet)
{
    const std::string sectionName = "Section1";
    ASSERT_EQ("Hello World!", mSettings.get<std::string>(sectionName, "stringProperty"));
    ASSERT_EQ(std::numeric_limits<int64_t>::max(), mSettings.get<int64_t>(sectionName, "maxInt64Property"));
    ASSERT_EQ(std::numeric_limits<int32_t>::max(), mSettings.get<int32_t>(sectionName, "maxInt32Property"));
    ASSERT_EQ(std::numeric_limits<int16_t>::max(), mSettings.get<int16_t>(sectionName, "maxInt16Property"));
    ASSERT_EQ(std::numeric_limits<int8_t>::max(), mSettings.get<int8_t>(sectionName, "maxInt8Property"));
    ASSERT_EQ(std::numeric_limits<int64_t>::min(), mSettings.get<int64_t>(sectionName, "minInt64Property"));
    ASSERT_EQ(std::numeric_limits<int32_t>::min(), mSettings.get<int32_t>(sectionName, "minInt32Property"));
    ASSERT_EQ(std::numeric_limits<int16_t>::min(), mSettings.get<int16_t>(sectionName, "minInt16Property"));
    ASSERT_EQ(std::numeric_limits<int8_t>::min(), mSettings.get<int8_t>(sectionName, "minInt8Property"));
    ASSERT_EQ(std::numeric_limits<uint64_t>::max(), mSettings.get<uint64_t>(sectionName, "maxUint64Property"));
    ASSERT_EQ(std::numeric_limits<uint32_t>::max(), mSettings.get<uint32_t>(sectionName, "maxUint32Property"));
    ASSERT_EQ(std::numeric_limits<uint16_t>::max(), mSettings.get<uint16_t>(sectionName, "maxUint16Property"));
    ASSERT_EQ(std::numeric_limits<uint8_t>::max(), mSettings.get<uint8_t>(sectionName, "maxUint8Property"));
    const float epsilon = 0.0001f;
    const float expectedFractional = 55.555f;
    ASSERT_NEAR(expectedFractional, mSettings.get<long double>(sectionName, "fractionalProperty"), epsilon);
    ASSERT_NEAR(expectedFractional, mSettings.get<double>(sectionName, "fractionalProperty"), epsilon);
    ASSERT_NEAR(expectedFractional, mSettings.get<float>(sectionName, "fractionalProperty"), epsilon);
    ASSERT_TRUE(mSettings.get<bool>(sectionName, "trueProperty"));
    ASSERT_FALSE(mSettings.get<bool>(sectionName, "falseProperty"));
    ASSERT_EQ(std::numeric_limits<int64_t>::max(), mSettings.get<int64_t>(sectionName, "hexProperty"));
}

TEST_F(SettingsTest, TestGetDefaultValue)
{
    const std::string sectionName = "Section1";
    const std::string nonExistingProperty = "nonExisting";
    ASSERT_EQ("Hello World!", mSettings.get<std::string>(sectionName, nonExistingProperty, "Hello World!"));
    const uint8_t expectedDefaultNumber = 100;
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<int64_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<int32_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<int16_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<int8_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<uint64_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<uint32_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<uint16_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    ASSERT_EQ(expectedDefaultNumber, mSettings.get<uint8_t>(sectionName, nonExistingProperty, expectedDefaultNumber));
    const float epsilon = 0.0001f;
    const float expectedFractional = 44.444f;
    ASSERT_NEAR(expectedFractional, mSettings.get<long double>(sectionName, nonExistingProperty, expectedFractional), epsilon);
    ASSERT_NEAR(expectedFractional, mSettings.get<double>(sectionName, nonExistingProperty, expectedFractional), epsilon);
    ASSERT_NEAR(expectedFractional, mSettings.get<float>(sectionName, nonExistingProperty, expectedFractional), epsilon);
    ASSERT_TRUE(mSettings.get<bool>(sectionName, nonExistingProperty, true));
    ASSERT_FALSE(mSettings.get<bool>(sectionName, nonExistingProperty, false));
}

TEST_F(SettingsTest, TestGetDefaultFromFile)
{
    const std::string sectionName = "DefaultSection";
    ASSERT_EQ("Default string from file", mSettings.get<std::string>(sectionName, "stringProperty"));
    ASSERT_EQ(std::numeric_limits<int64_t>::max(), mSettings.get<int64_t>(sectionName, "maxInt64Property"));
    ASSERT_EQ(std::numeric_limits<int32_t>::max(), mSettings.get<int32_t>(sectionName, "maxInt32Property"));
    ASSERT_EQ(std::numeric_limits<int16_t>::max(), mSettings.get<int16_t>(sectionName, "maxInt16Property"));
    ASSERT_EQ(std::numeric_limits<int8_t>::max(), mSettings.get<int8_t>(sectionName, "maxInt8Property"));
    ASSERT_EQ(std::numeric_limits<int64_t>::min(), mSettings.get<int64_t>(sectionName, "minInt64Property"));
    ASSERT_EQ(std::numeric_limits<int32_t>::min(), mSettings.get<int32_t>(sectionName, "minInt32Property"));
    ASSERT_EQ(std::numeric_limits<int16_t>::min(), mSettings.get<int16_t>(sectionName, "minInt16Property"));
    ASSERT_EQ(std::numeric_limits<int8_t>::min(), mSettings.get<int8_t>(sectionName, "minInt8Property"));
    ASSERT_EQ(std::numeric_limits<uint64_t>::max(), mSettings.get<uint64_t>(sectionName, "maxUint64Property"));
    ASSERT_EQ(std::numeric_limits<uint32_t>::max(), mSettings.get<uint32_t>(sectionName, "maxUint32Property"));
    ASSERT_EQ(std::numeric_limits<uint16_t>::max(), mSettings.get<uint16_t>(sectionName, "maxUint16Property"));
    ASSERT_EQ(std::numeric_limits<uint8_t>::max(), mSettings.get<uint8_t>(sectionName, "maxUint8Property"));
    const float epsilon = 0.0001f;
    const float expectedFractional = 33.333f;
    ASSERT_NEAR(expectedFractional, mSettings.get<long double>(sectionName, "fractionalProperty"), epsilon);
    ASSERT_NEAR(expectedFractional, mSettings.get<double>(sectionName, "fractionalProperty"), epsilon);
    ASSERT_NEAR(expectedFractional, mSettings.get<float>(sectionName, "fractionalProperty"), epsilon);
    ASSERT_TRUE(mSettings.get<bool>(sectionName, "trueProperty"));
    ASSERT_FALSE(mSettings.get<bool>(sectionName, "falseProperty"));
    ASSERT_EQ(std::numeric_limits<int64_t>::max(), mSettings.get<int64_t>(sectionName, "hexProperty"));
}
