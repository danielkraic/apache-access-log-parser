#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

void log(const std::string& msg) {
  std::cerr << msg << std::endl;
}

template <typename T>
void log(const std::string& msg, T val) {
  std::cerr << msg << ": " << val << std::endl;
}

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

    static const size_t SIZE = 9;
};


void parseLine(const std::string& s) {
  std::vector<std::string> v;

  std::string::size_type lastPos = 0;
  std::string::size_type pos = 0;

  while (lastPos != std::string::npos) {
    if (s[lastPos] == '"' && s.size() > lastPos) {
      lastPos += 1;
      pos = s.find("\"", lastPos);
      v.push_back( s.substr(lastPos, pos - lastPos) );
      pos = s.find_first_not_of("\"", pos);
    }else if (s[lastPos] == '[' && s.size() > lastPos) {
      lastPos += 1;
      pos = s.find("]", lastPos);
      v.push_back( s.substr(lastPos, pos - lastPos) );
      pos = s.find_first_not_of("]", pos);
    } else {
      pos = s.find(" ", lastPos);
      v.push_back( s.substr(lastPos, pos - lastPos) );
    }
    lastPos = s.find_first_not_of(" ", pos);
  }
}

int main(int argc, char const *argv[])
{
  if (argc < 3) {
    log("missing params: in_file_path out_file_path");
    return 1;
  }
  std::string in_path(argv[1]);
  std::string out_path(argv[2]);

  if (!boost::filesystem::exists(in_path)) {
    log("file not exists", in_path);
    return 1;
  } else if (!boost::filesystem::is_regular_file(in_path)) {
    log("file is not regural file", in_path);
    return 1;
  }

  std::ifstream in_file(in_path);
  if (!in_file.good()) {
    log("failed to open file", in_path);
    return 1;
  }

  if (boost::filesystem::exists(out_path)) {
    log("file already exists", out_path);
    return 1;
  }

  std::ofstream out_file(out_path);
  if (!out_file.good()) {
    log("failed to open file", out_path);
    return 1;
  }

  boost::archive::text_oarchive oa(out_file);

  size_t line_counter = 0;
  std::string line;
  while(getline(in_file, line)) {
    line_counter++;

    std::vector<strd::string> v = parseLine(line);
    log("v.size()", v.size());
    for (const auto& s : v)
      std::cout << " :: " << s << std::endl;

    if (v.size() >= 9) {
      LogLine ll(v[0], v[1], v[2], v[3], v[4], std::stol(v[5]), std::stol(v[6]), v[7], v[8]);
      oa << ll;
    }
  }

  log("done, lines count", line_counter);

  return 0;
}