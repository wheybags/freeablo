#ifndef FA_READ_INI_H
#define FA_READ_INI_H

#include <boost/property_tree/ptree.hpp>

#include <string>

namespace Misc
{
    ///< utility function to wrap boost's read_ini with FAIO
    void readIni(const std::string& path, boost::property_tree::ptree& pt);
}

#endif    
