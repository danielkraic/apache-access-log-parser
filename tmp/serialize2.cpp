#include <fstream>
#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class LogLine
{
private:
    friend class boost::serialization::access;

    std::string d_hostname;
    std::string d_logname;
    std::string d_username;
    std::string d_time;
    std::string d_request;
    unsigned d_status;
    size_t d_bytes;
    std::string d_referer;
    std::string d_useragent;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & d_hostname;
        ar & d_logname;
        ar & d_username;
        ar & d_time;
        ar & d_request;
        ar & d_status;
        ar & d_bytes;
        ar & d_referer;
        ar & d_useragent;
    }

public:
    LogLine() :
        d_hostname(),
        d_logname(),
        d_username(),
        d_time(),
        d_request(),
        d_status(),
        d_bytes(),
        d_referer(),
        d_useragent()
    {}

    LogLine(const std::string& hostname,
            const std::string& logname,
            const std::string& username,
            const std::string& time,
            const std::string& request,
            unsigned status,
            size_t bytes,
            const std::string& referer,
            const std::string& useragent
    ) : d_hostname(hostname),
        d_logname(logname),
        d_username(username),
        d_time(time),
        d_request(request),
        d_status(status),
        d_bytes(bytes),
        d_referer(referer),
        d_useragent(useragent)
    {}
};

int main() {
    std::ofstream ofs("filename");

    const LogLine ll(
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
    return 0;
}