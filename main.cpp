#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/*
193.227.11.118 - - [01/Jun/2014:09:59:13 +0200] "GET /apache-log/access.log HTTP/1.1" 206 33904 "http://www.almhuette-raith.at/apache-log/" "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0)" "-"
180.180.67.158 - - [31/Jul/2014:01:25:26 +0200] "GET /templates/jp_hotel/css/template.css HTTP/1.1" 200 10004 "http://www.almhuette-raith.at/apache-log/access.log" "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:5.0) Gecko/20100101 Firefox/5.0" "-"
*/

/*
const size_t DATA_CHUNK_SIZE = 1000;

using MapKey_t = ;
using MapVal_t = ;
using MapPair_t = std::pair<MapKey_t, MapVal_t>;

*/
 
void parseLine(const std::string& s, std::vector<std::string>& v) {
  v.clear();
  std::string::size_type lastPos = 0;
  std::string::size_type pos = 0;

  while (lastPos != std::string::npos) {
    if (s[lastPos] == '"' && s.size() > lastPos) {
      lastPos += 1;
      pos = s.find("\"", lastPos);
      v.push_back( s.substr(lastPos, pos - lastPos) );
      pos = s.find_first_not_of("\"", pos);
    } else {
      pos = s.find(" ", lastPos);
      v.push_back( s.substr(lastPos, pos - lastPos) );
    }
    lastPos = s.find_first_not_of(" ", pos);
  }
}
 
bool deserializeLine(const std::string& line, MapPair_t& mapPair) {
  std::vector<std::string> v;

  if (!parseLine(s, v))
    return false;

  if (v.size() != LogData::SIZE)
    return false;

  return true;
}

/*
bool storeValue(const MapKey_t& key, const MapVal_t& val) {
  return db->storeVal(key, val);
}

bool storeMap(const Map_t& d_map) {
  for (const auto& obj : d_map) {
    if (!storeValue(obj.first, obj.second)) {
      std::cerr << "Failed to store value" << std::endl;
      return false;
    }
  }
  return true;
}
*/

void simple_log(const std::string& s, size_t pos) {
  std::cout << s << " :: " << pos << std::endl;
}

bool getValue(const std::string& line, size_t *lastPos, std::string& value) {
  simple_log("last", *lastPos);
  size_t begPos = line.find(' ', *lastPos);
  simple_log("beg", begPos);
  *lastPos = line.find(' ', begPos);
  simple_log("last", *lastPos);

  if (begPos == std::string::npos || *lastPos == std::string::npos) {
    simple_log("npos", 0);
    return false;
  }

  simple_log("substr", *lastPos);
  value = line.substr(begPos, *lastPos - begPos);
  return true;
}

bool getQuotedValue(const std::string& line, size_t *lastPos, std::string& value) {
  size_t begPos = line.find(" \"", *lastPos);
  *lastPos = line.find("\" ", begPos);

  if (begPos == std::string::npos || *lastPos == std::string::npos)
    return false;

  value = line.substr(begPos, *lastPos - begPos);
  return true;
}

void parseLine(const std::string& line) {
  std::string host, logname, user/*, time, first_line, status, bytes, referer, userAgent*/;
  size_t pos = 0;

  if (!getValue(line, &pos, host)) {
   std::cerr << "failed to get host: " << pos << std::endl;
   return;
  }

  /*if (!getValue(line, &pos, logname)) {
   std::cerr << "failed to get logname: " << pos << std::endl;
   return;
  }

  if (!getValue(line, &pos, user)) {
   std::cerr << "failed to get user: " << pos << std::endl;
   return;
  }*/
}

void storeLine(const std::string& line) {

  parseLine(line);
  /*
  MapPair_t mapPair;
  if (!deserializeLine(line, mapPair))
    std::cerr << "Failed to deserialize line" << std::endl;
    return false;
  }

  d_map[mapPair.first].insert(mapPair.second);

  if (d_map.size() > DATA_CHUNK_SIZE) {
    if (!storeMap(d_map)) {
      std::cerr << "Failed to store map" << std::endl;
    }
    d_map.clear();
  }
  */
}

int main(int argc, char const *argv[])
{
  if (argc < 2) {
    std::cerr << "Missing param: apache_access_log" << std::endl;
    return 1;
  }

  std::string p = argv[1];
  if (!boost::filesystem::exists(p)) {
    std::cout << "File '" << p << "' does not exist\n";
    return 0;
  } else if (!boost::filesystem::is_regular_file(p)) {
    if (boost::filesystem::is_directory(p))
      std::cout << "File '" << p << "' is a directory\n";
    else
      std::cout << "File '" << p << "' exists, but is neither a regular file nor a directory\n";
  }

  std::ifstream ifile(p);
  if (!ifile.good()) {
    std::cerr << "failed to open file: '" << p << "'" << std::endl;
    return 0;
  }

  std::for_each(
      std::istream_iterator<std::string>(ifile),
      std::istream_iterator<std::string>(),
      [] (const std::string& line) {
        storeLine(line);
      });

  return 0;
}