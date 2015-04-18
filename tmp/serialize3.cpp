#include <fstream>
#include <string>
#include <tuple>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using LogLine = std::tuple<
    std::string, // hostname
    std::string, // logname
    std::string, // username
    std::string, // time
    std::string, // request
    unsigned,    // status
    size_t,      // bytes
    std::string, // referer
    std::string  // useragent
>;

template<class Archive>
void serialize(Archive & ar, LogLine & l, const unsigned int version) {
    ar & std::get<0>(l);
    ar & std::get<1>(l);
    ar & std::get<2>(l);
    ar & std::get<3>(l);
    ar & std::get<4>(l);
    ar & std::get<5>(l);
    ar & std::get<6>(l);
    ar & std::get<7>(l);
    ar & std::get<8>(l);
}

int main() {
    //std::ofstream ofs("filename");

    LogLine ll = std::make_tuple(
            "localhost",
            "-",
            "-",
            "2015-01-02-16:56",
            "GET locahost/index.html",
            500,
            1024,
            "Firefox",
            "-"
    );

    // save data to archive
    {
        boost::archive::text_oarchive oa(ofs);
        // write class instance to archive
        oa << ll;
      // archive and stream closed when destructors are called
    }

    /*
    // ... some time later restore the class instance to its orginal state
    LogLine newLL;
    {
        // create and open an archive for input
        std::ifstream ifs("filename");
        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> newLL;
        // archive and stream closed when destructors are called
    }
    */

    return 0;
}