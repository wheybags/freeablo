#include "engine/enginemain.h"
#include <cxxopts.hpp>
#include <diabloexe/diabloexe.h>
#include <faio/fafileobject.h>
#include <filesystem/path.h>
#include <fstream>
#include <iostream>
#include <misc/md5.h>
#include <misc/misc.h>
#include <settings/settings.h>

bool parseOptions(int argc, char** argv, cxxopts::ParseResult& variables)
{
    cxxopts::Options desc("Options");
    desc.add_options()("h,help", "Print help")
        // -1 represents the main menu
        ("l,level", "Level number to load (0-16)", cxxopts::value<int32_t>()->default_value("-1"))(
            "c,character", "Choose Warrior, Rogue or Sorceror", cxxopts::value<std::string>()->default_value("Warrior"))(
            "invuln", "on or off", cxxopts::value<std::string>()->default_value("off"))(
            "connect", "Ip Address or hostname to connect to", cxxopts::value<std::string>()->default_value(""))(
            "seed", "Seed for level generation", cxxopts::value<uint32_t>()->default_value("0"));

    try
    {
        variables = desc.parse(argc, argv);

        if (variables.count("help"))
        {
            std::cout << desc.help() << std::endl;
            return false;
        }

        const int32_t dLvl = variables["level"].as<int32_t>();

        if (dLvl > 16)
            throw cxxopts::OptionParseException("There is no level after 16");
    }
    catch (cxxopts::OptionParseException& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc.help() << std::endl;
        return false;
    }

    return true;
}

bool dataFilesSetUp(const Settings::Settings& settings)
{
    std::string mpqPath = settings.get<std::string>("Game", "PathMPQ");
    std::string exePath = settings.get<std::string>("Game", "PathEXE");

    if (mpqPath.empty() || exePath.empty())
        return false;

    if (DiabloExe::DiabloExe::getVersion(exePath).empty())
        return false;

    using digest_type = Misc::md5_byte_t[16];

    constexpr digest_type hashes[] = {{0x68, 0xf0, 0x49, 0x86, 0x6b, 0x44, 0x68, 0x8a, 0x7a, 0xf6, 0x5b, 0xa7, 0x66, 0xbe, 0xf7, 0x5a}, // us version

                                      {0x01, 0x1b, 0xc6, 0x51, 0x8e, 0x61, 0x66, 0x20, 0x62, 0x31, 0x08, 0x0a, 0x44, 0x40, 0xb3, 0x73}}; // eu version

    std::ifstream mpqFile(mpqPath, std::ios::binary);
    mpqFile.seekg(0, std::ios::end);
    std::size_t fileSize = mpqFile.tellg();
    mpqFile.seekg(0, std::ios::beg);
    char* mpqContents = new char[fileSize];
    mpqFile.read(mpqContents, fileSize);
    mpqFile.close();

    Misc::md5_state_t state;
    Misc::md5_byte_t digest[16];

    Misc::md5_init(&state);
    Misc::md5_append(&state, reinterpret_cast<Misc::md5_byte_t*>(mpqContents), fileSize);
    Misc::md5_finish(&state, digest);
    delete[] mpqContents;

    bool mpqValid = false;

    auto md5Equal = [](const digest_type& m1, const digest_type& m2) {
        for (int i = 0; i < 16; i++)
        {
            if (m1[i] != m2[i])
            {
                return false;
            }
        }

        return true;
    };

    for (const auto& hash : hashes)
    {
        mpqValid = md5Equal(hash, digest);

        if (mpqValid)
            break;
    }

    if (!mpqValid)
        std::cerr << "Invalid MPQ file. Please select another.\n";

    return mpqValid;
}

int fa_main(int argc, char** argv)
{
    Misc::saveArgv0(argv[0]);

    Settings::Settings settings;

    // Check if we've been configured with data files, and if we haven't, run the launcher to prompt configuration
    if (!(settings.loadUserSettings() && dataFilesSetUp(settings)))
    {
        filesystem::path path = (filesystem::path(argv[0]).parent_path() / "launcher").make_absolute();
        system(Misc::escapePathForShell(path.str()).c_str());
        return EXIT_SUCCESS;
    }

    if (!FAIO::init(settings.get<std::string>("Game", "PathMPQ")))
        return EXIT_FAILURE;

    Engine::EngineMain engine;

    int retval = EXIT_SUCCESS;

    cxxopts::ParseResult variables;
    if (parseOptions(argc, argv, variables))
        engine.run(variables);
    else
        retval = EXIT_FAILURE;

    FAIO::FAFileObject::quit();
    return retval;
}
