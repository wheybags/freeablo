#include "fareadini.h"

#include <boost/property_tree/ini_parser.hpp>

#include <faio/faio.h>

namespace Misc
{
    namespace bpt = boost::property_tree;

    void readIni(const std::string& path, boost::property_tree::ptree& pt)
    {
        FAIO::FAFile* file = FAIO::FAfopen(path);

        size_t size = FAIO::FAsize(file);
        char* str = new char[size+1];

        FAIO::FAfread(str, 1, size, file);
        str[size] = '\0';

        std::stringstream s;
        s << str;

        bpt::read_ini(s, pt);

        delete[] str;
        FAIO::FAfclose(file);
    }
}
