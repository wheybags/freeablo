#include <iostream>
#include <string>
#include <fstream>

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <boost/range.hpp>
#include <SDL.h>
#include <SDL_image.h>

#define private public
#include <cel/celfile.h>
#include <faio/fafileobject.h>
#include <misc/md5.h>
#include <misc/stringops.h>




std::string hashImageData(int32_t width, int32_t height, Cel::Colour* data)
{
    int32_t size = width * height * sizeof(Cel::Colour);
    Misc::md5_byte_t* buff = (Misc::md5_byte_t*)data;

    Misc::md5_state_t state;
    Misc::md5_byte_t digest[16];

    md5_init(&state);
    md5_append(&state, buff, size);
    md5_finish(&state, digest);

    std::stringstream s;

    for(int32_t i = 0; i < 16; i++)
       s << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];

    return s.str();
}

std::string hashCelFrame(Cel::CelFrame& frame)
{
    return hashImageData(frame.mWidth, frame.mHeight, &frame.mRawImage[0]);
}

std::vector<std::string> getCelsFromListfile(const std::string& path)
{
    FILE* f = fopen(path.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    int32_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc(size);
    fread(buf, 1, size, f);

    std::string str(buf);

    free(buf);
    fclose(f);

    std::vector<std::string> lines = Misc::StringUtils::split(str, '\n');

    std::vector<std::string> celFiles;
    for(int32_t i = 0; i < (int32_t)lines.size(); i++)
    {
        if(Misc::StringUtils::ciEndsWith(lines[i], ".cel") || Misc::StringUtils::ciEndsWith(lines[i], ".cl2"))
        {
            std::string path_local = Misc::StringUtils::toLower(lines[i]);

            if(FAIO::exists(path_local))
                celFiles.push_back(path_local);
        }
    }

    return celFiles;
}


Cel::Colour getPixel(SDL_Surface* s, int x, int y)
{
    Uint32 pix;

    int bpp = s->format->BytesPerPixel;
    // Here p is the address to the pixel we want to retrieve
    Uint8 *p = (Uint8 *)s->pixels + y * s->pitch + x * bpp;

    switch(bpp) {
        case 1:
            pix = *p;
            break;

        case 2:
            pix = *(Uint16 *)p;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                pix = p[0] << 16 | p[1] << 8 | p[2];
            else
                pix = p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            pix = *(Uint32 *)p;
            break;

        default:
            pix = 0;
    }

    Uint8 r, g, b, a;
    SDL_GetRGBA(pix, s->format, &r, &g, &b, &a);

    return Cel::Colour(r, g, b, a == 255);
}

std::string hashOneFramePng(const std::string& path)
{
    FILE* f = fopen(path.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    int32_t buffer_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = new char[buffer_size];
    fread(buffer, 1, buffer_size, f);
    fclose(f);

    SDL_Surface* s = IMG_LoadTyped_RW(SDL_RWFromMem(buffer, buffer_size), 1, "png");
    delete[] buffer;


    Cel::Colour* data = new Cel::Colour[s->w * s->h];

    for(int32_t y = 0; y < s->h; y++)
    {
        for(int32_t x = 0; x < s->w; x++)
        {
            Cel::Colour c = getPixel(s, x, y);
            data[x + ((s->h-1-y)*s->w)] = c;
        }
    }

    std::string hash = hashImageData(s->w, s->h, data);
    delete[] data;

    return hash;
}

namespace bfs = boost::filesystem;
void hashPngFolder(const std::string& folderPath, std::ofstream& out)
{
    std::vector<std::string> paths;
    for(bfs::directory_iterator it(folderPath); it != bfs::directory_iterator(); it++)
        paths.push_back((*it).path().string());

    std::sort(paths.begin(), paths.end());

    for(auto p : paths)
    {
        if(Misc::StringUtils::ciEndsWith(p, ".png"))
            out << "," << hashOneFramePng(p);
    }
}

///< blizzconv: https://github.com/mewrnd/blizzconv
///< blizzconvBase is the _dump_ folder created after converting all the cel/cl2 files
void getBlizzconvHashes(const std::string& blizzconvBase, const std::string& listFile, const std::string& destFile)
{
    std::vector<std::string> celPaths = getCelsFromListfile(listFile);

    std::ofstream out(destFile);

    for(int32_t i = 0; i < (int32_t)celPaths.size(); i++)
    {
        std::string path = Misc::StringUtils::replaceEnd(".cel", "", celPaths[i]);
        Misc::StringUtils::replace(path, "\\", "/"); // fukken windows path separators...

        if(path == "monsters/darkmage/dmagew") // this cl2 file is completely broken, but probably just meant to be transparent, see https://github.com/mewrnd/blizzconv/issues/4#issuecomment-201929273
            continue;

        if(bfs::exists(blizzconvBase + "/" + path + ".png"))
        {
            std::string hash = hashOneFramePng(blizzconvBase + "/" + path + ".png");
            out << celPaths[i] << "," << hash << std::endl;
        }
        else
        {
            std::string folderPath = blizzconvBase + "/" + path;

            if(bfs::exists(folderPath))
            {
                out << celPaths[i];
                hashPngFolder(folderPath, out);
                out << std::endl;
            }
            else
            {
                std::string pngPath = blizzconvBase + "/" + path + std::to_string(0) + ".png";
                if(bfs::exists(pngPath))
                {
                    out << celPaths[i];
                    hashPngFolder(blizzconvBase + "/" + bfs::path(path).parent_path().string(), out);
                    out << std::endl;
                }
                else
                {

                    std::string subCelFolderPath = blizzconvBase + "/" + path + std::to_string(0);
                    assert(bfs::exists(subCelFolderPath));

                    out << celPaths[i];

                    int32_t folderNum = 0;
                    while(bfs::exists(subCelFolderPath))
                    {
                        hashPngFolder(subCelFolderPath, out);

                        subCelFolderPath = blizzconvBase + "/" + path + std::to_string(folderNum);
                        folderNum++;
                    }

                    out << std::endl;
                }
            }
        }
    }
}

// the cel_hashes.txt file was generated by getBlizzconvHashes above
std::map<std::string, std::vector<std::string> > getCelHashes()
{
    std::string thisFolder = bfs::path(__FILE__).parent_path().string();

    std::ifstream in(thisFolder + "/cel_hashes.txt");
    std::string line;
    std::map<std::string, std::vector<std::string> > retval;

    while(std::getline(in, line))
    {
        auto components = Misc::StringUtils::split(line, ',');
        std::string path = components[0];

        components.erase(components.begin());

        retval[path] = components;
    }

    return retval;
}

TEST (Cel, TestOpen)
{
    std::string thisFolder = bfs::path(__FILE__).parent_path().string();

    std::vector<std::string> celPaths = getCelsFromListfile(thisFolder+ "/Diablo I.txt");
    auto celHashes = getCelHashes();

    int32_t succeededFiles = 0;
    int32_t succeededFrames = 0;
    int32_t totalFrames = 0;

    for(auto p : celPaths)
    {
        if(Misc::StringUtils::endsWith(p, "unravw.cel")) // this cel file is broken, see https://github.com/mewrnd/blizzconv/issues/2#issuecomment-58065868
            continue;

        if(p == "monsters\\darkmage\\dmagew.cl2") // this cl2 file is completely broken, but probably just meant to be transparent, see https://github.com/mewrnd/blizzconv/issues/4#issuecomment-201929273
            continue;

        Cel::CelFile cel(p);

        bool fileSucceeded = true;
        totalFrames += cel.numFrames();

        for(int32_t i = 0; i < cel.numFrames(); i++)
        {
            if(celHashes[p].size() == 0)
            {
                std::cout << "TEST_CEL " << p << ", FAIL, NO DATA" << std::endl;
                continue;
            }

            std::string savedHash = celHashes[p][i];
            std::string newHash = hashCelFrame(cel[i]);

            if(savedHash == newHash)
            {
                succeededFrames++;
                std::cout << "TEST_CEL " << p << "[" << i << "] SUCCESS" << std::endl;
            }
            else
            {
                fileSucceeded = false;
                std::cout << "TEST_CEL " << p << "[" << i << "] FAIL, OLD: " << savedHash << ", NEW: " << newHash << std::endl;
            }
        }

        if(fileSucceeded)
            succeededFiles++;
    }

    float percentFiles = ((100.0f/((float)celPaths.size())) * ((float)succeededFiles));
    std::cout << succeededFiles << "/" << celPaths.size() << " files succeeded, " << percentFiles << "%" << std::endl;

    float percentFrames = ((100.0f/((float)totalFrames)) * ((float)succeededFrames));
    std::cout << succeededFrames << "/" << totalFrames << " frames succeeded, " << percentFrames << "%" << std::endl;

    ASSERT_EQ(succeededFrames, totalFrames);
}

int main(int argc, char **argv)
{
    FAIO::init();

    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();

    FAIO::FAFileObject::quit();

    return retval;
}
