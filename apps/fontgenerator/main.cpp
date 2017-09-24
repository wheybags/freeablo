#include <vector>
#include <iostream>
#include <cel/celdecoder.h>
#include <faio/fafileobject.h>
#include <misc/disablewarn.h>
#include <StormLib.h>
#include <misc/enablewarn.h>


void help(char** argv) {
    std::cout << argv[0] << " mpq_path " << std::endl;
    std::cout << std::endl << "Application generates part of libRocket font file." << std::endl;
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        help(argv);
        return 0;
    }

    // Open MPQ

    HANDLE handle;
    SFileOpenArchive(argv[1], 0, STREAM_FLAG_READ_ONLY, &handle);
    FAIO::init(argv[1]);

    // For now only smaltext.cel

    Cel::CelDecoder cel("ctrlpan/smaltext.cel");

    // Prepare ascii vector according to order in cel file
    std::vector<int> ascii;

    for(int i = 'a' ; i <= 'z' ; i++)
        ascii.push_back(i);

    for(int i = '1' ; i <= '9'; i++)
        ascii.push_back(i);

    ascii.push_back('0');


    // I can't find mapping in ascii table for one signs so
    // I marked it as 255
    std::vector<int> asciiSigns = { '-', '=', '+', '(', ')', '[', ']', '"', 255,
                                    '`', '\'',':',';',',','.','/','?','!','&','%',
                                    '#','$','*','<','>','@','\\','^','_','|','~'};

    ascii.insert(ascii.end(), asciiSigns.begin(), asciiSigns.end());

    std::map<int, std::string> mapping;

    int positionX = 0;

    for(int32_t i = 0 ; i < cel.numFrames(); i++)
    {
        Cel::CelFrame& frame = cel[i];
        int32_t maximumVisibleX = 0;
        for(int32_t x = 0; x < frame.mWidth; x++)
        {
            for(int32_t y = 0; y < frame.mHeight; y++)
            {
                if(frame[x][y].visible)
                    if(x > maximumVisibleX)
                        maximumVisibleX = x;
            }
        }

        std::string asciiStr, positionXStr, maximumVisibleXStr;

        int asciiIdx = ascii[i];

        // Additional 2 pixels for every letter
        maximumVisibleX += 2;
        if(maximumVisibleX > frame.mWidth)
            maximumVisibleX = frame.mWidth;

        // Convert values to string
        char buffer[10];
        sprintf(buffer, "%d", asciiIdx);
        asciiStr = buffer;

        sprintf(buffer, "%d", maximumVisibleX);
        maximumVisibleXStr = buffer;

        sprintf(buffer, "%d", positionX);
        positionXStr = buffer;

        // Create output
        std::string out = "<char id=\""+asciiStr+"\" x=\""+positionXStr+"\" y=\"0\" width=\""+maximumVisibleXStr+"\" height=\"11\" xoffset=\"0\" yoffset=\"0\" xadvance=\""+maximumVisibleXStr+"\" />";
        mapping[asciiIdx] = out;

        // Move further
        positionX += 13;
    }

    // Sort by ascii

    std::sort(ascii.begin(), ascii.end());
    for(int i : ascii)
    {
        std::cout << mapping[i] << std::endl;
    }

    FAIO::FAFileObject::quit();
}
