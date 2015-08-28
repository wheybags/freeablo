#ifndef SAVE_GAME_H
#define SAVE_GAME_H

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

namespace FASaveGame
{
    typedef boost::archive::text_oarchive OutputStream;
    typedef boost::archive::text_iarchive InputStream;
}

#endif
