// clang-format off
#include <misc/disablewarn.h>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/endian/conversion.hpp>
#include <misc/enablewarn.h>
// clang-format on
#include <iostream>
#include <fstream>
#include <faio/fafileobject.h>
#include <settings/settings.h>
#include <diabloexe/diabloexe.h>
#include "engine/enginemain.h"

namespace bpo = boost::program_options;
namespace bud = boost::uuids::detail;
namespace be = boost::endian;

bool parseOptions(int argc, char** argv, bpo::variables_map& variables)
{
    boost::program_options::options_description desc("Options");
    desc.add_options()("help,h", "Print help")
        // -1 represents the main menu
        ("level,l", bpo::value<int32_t>()->default_value(-1), "Level number to load (0-16)")(
            "character,c", bpo::value<std::string>()->default_value("Warrior"), "Choose Warrior, Rogue or Sorcerer")(
            "invuln", bpo::value<std::string>()->default_value("off"), "on or off")(
            "connect", bpo::value<std::string>()->default_value(""), "Ip Address or hostname to connect to");

    try
    {
        bpo::store(bpo::parse_command_line(argc, argv, desc), variables);

        if (variables.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }

        bpo::notify(variables);

        const int32_t dLvl = variables["level"].as<int32_t>();

        if (dLvl > 16)
            throw bpo::error("There is no level after 16");
    }
    catch (bpo::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
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

    auto cond_reverse = [](unsigned int x) { return be::conditional_reverse(x, be::order::big, be::order::native); };

    const bud::md5::digest_type hashes[] = {
        {cond_reverse(0x68f04986), cond_reverse(0x6b44688a), cond_reverse(0x7af65ba7), cond_reverse(0x66bef75a)}, // us version

        {cond_reverse(0x011bc651), cond_reverse(0x8e616620), cond_reverse(0x6231080a), cond_reverse(0x4440b373)}}; // eu version

    bud::md5 mpqMD5;
    bud::md5::digest_type mpqDigest;

    std::ifstream mpqFile(mpqPath, std::ios::binary);
    mpqFile.seekg(0, std::ios::end);
    int fileSize = mpqFile.tellg();
    mpqFile.seekg(0, std::ios::beg);
    char* mpqContents = new char[fileSize];
    mpqFile.read(mpqContents, fileSize);
    mpqFile.close();

    mpqMD5.process_bytes(mpqContents, fileSize);
    mpqMD5.get_digest(mpqDigest);
    delete[] mpqContents;

    bool mpqValid = false;

    auto md5Equal = [](const bud::md5::digest_type& m1, const bud::md5::digest_type& m2) {
        for (int i = 0; i < 4; i++)
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
        mpqValid = md5Equal(hash, mpqDigest);
    }

    if (!mpqValid)
        std::cerr << "Invalid MPQ file. Please select another.\n";

    return mpqValid;
}

int fa_main(int argc, char** argv)
{
    Settings::Settings settings;

    // Check if we've been configured with data files, and if we haven't, run the launcher to prompt configuration
    if (!(settings.loadUserSettings() && dataFilesSetUp(settings)))
    {
        system((boost::filesystem::system_complete(argv[0]).parent_path() / "launcher").string().c_str());
        return EXIT_SUCCESS;
    }

    if (!FAIO::init(settings.get<std::string>("Game", "PathMPQ")))
        return EXIT_FAILURE;

    Engine::EngineMain engine;

    int retval = EXIT_SUCCESS;

    boost::program_options::variables_map variables;
    if (parseOptions(argc, argv, variables))
        engine.run(variables);
    else
        retval = EXIT_FAILURE;

    FAIO::FAFileObject::quit();
    return retval;
}
