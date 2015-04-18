#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "mongo/client/dbclient.h"
#include "mongo/bson/bson.h"

//g+1 apache_log_parser.cpp -lboost_system -lboost_filesystem -lmongoclient -lboost_thread

void log(const std::string& msg) {
  std::cerr << msg << std::endl;
}

template <typename T>
void log(const std::string& msg, T val) {
  std::cerr << msg << ": " << val << std::endl;
}

int getMonth(const std::string& mon) {
  if (mon == "Jan")
    return 1;
  else if (mon == "Feb")
    return 2;
  else if (mon == "Mar")
    return 3;
  else if (mon == "Apr")
    return 4;
  else if (mon == "May")
    return 5;
  else if (mon == "Jun")
    return 6;
  else if (mon == "Jul")
    return 7;
  else if (mon == "Aug")
    return 8;
  else if (mon == "Sep")
    return 9;
  else if (mon == "Oct")
    return 10;
  else if (mon == "Nov")
    return 11;
  else if (mon == "Dec")
    return 12;
  else
    return -1;
}

size_t getTime(int year, int mon, int day) {
  struct std::tm t;
  t.tm_sec = 0;
  t.tm_min = 0;
  t.tm_hour = 0;
  t.tm_mday = day;       // day of month (0 .. 31)
  t.tm_mon = mon-1;      // month of year (0 .. 11)
  t.tm_year = year-1900; // year since 1900
  t.tm_isdst = -1;       // determine whether daylight saving time

  std::time_t tt = std::mktime(&t);
  if (tt == -1) {
    std::string err_msg = "Invalid time (day: " + std::to_string(day) + ", mon: " + std::to_string(mon) +  ", year: " + std::to_string(year) + ")"; 
    throw err_msg.c_str();
  }

  return static_cast<size_t>(tt);
}

bool parseDate(const std::string& dateTimeString, size_t& date) {
  //[01/Aug/2013:14:41:53 +0200]

  std::string::size_type lastPos = 0;
  std::string::size_type pos = 0;

  pos = dateTimeString.find('/', lastPos);
  if (pos == std::string::npos) return false;
  std::string day = dateTimeString.substr(lastPos, pos - lastPos);

  lastPos = dateTimeString.find_first_not_of('/', pos);
  pos = dateTimeString.find('/', lastPos);
  if (pos == std::string::npos) return false;
  std::string mon = dateTimeString.substr(lastPos, pos - lastPos);

  lastPos = dateTimeString.find_first_not_of('/', pos);
  pos = dateTimeString.find(':', lastPos);
  if (pos == std::string::npos) return false;
  std::string year = dateTimeString.substr(lastPos, pos - lastPos);

  try {
    date = getTime(
      std::stoi(year),
      getMonth(mon),
      std::stoi(day)
    );
  } catch (std::exception& e) {
    std::cerr << "Parsing date error: " << e.what() << ", input string: '" << dateTimeString << "'" << std::endl;
    return false;
  }

  return true;
}

bool parseHttpCode(const std::string& httpCodeString, int& httpCode) {
  try {
    httpCode = std::stoi(httpCodeString);
  } catch (std::exception& e) {
    std::cerr << "Parsing http code error: " << e.what() << ", parsing from string: '" << httpCodeString << "'" << std::endl;
    return false;
  }

  return true;
}

bool parseRequestMethod(const std::string& httpRequestString, std::string& requestMethod) {
  auto pos = httpRequestString.find(" ", 0);
  if (pos == std::string::npos) {
    std::cerr << "Failed to parse request method from string: '" << httpRequestString << "'" << std::endl;
    return false;
  }

  requestMethod = httpRequestString.substr(0, pos);
  return (!requestMethod.empty());
}

std::vector<std::string> parseLine(const std::string& s) {
  std::vector<std::string> v;

  std::string::size_type lastPos = 0;
  std::string::size_type pos = 0;

  while (lastPos != std::string::npos) {
    if (s[lastPos] == '"' && s.size() > lastPos) {
      lastPos += 1;
      pos = s.find("\"", lastPos);
      v.push_back( s.substr(lastPos, pos - lastPos) );
      pos = s.find_first_not_of("\"", pos);
    } else if (s[lastPos] == '[' && s.size() > lastPos) {
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

  return v;
}

int main(int argc, char const *argv[])
{
  if (argc < 2) {
    log("missing params: in_file_path");
    return 1;
  }
  std::string in_path(argv[1]);

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

  using namespace mongo;
  const char * MONGO_SERVER = "localhost";
  const char * MONGO_COLLECTION = "test.apachelog";
  
  DBClientConnection c;
  c.connect(MONGO_SERVER);

  size_t cnt = c.count(MONGO_COLLECTION);
  std::string err = c.getLastError();
  if (!err.empty()) {
    std::cerr << "DB error: " << err << std::endl;
    return 1;
  }
  std::cout << MONGO_COLLECTION << ", size: " << cnt << std::endl;

  std::string httpRequestMethod;
  int httpCode;
  size_t date;

  size_t line_counter = 0;
  std::string line;
  while(getline(in_file, line)) {
    line_counter++;

    std::vector<std::string> v = parseLine(line);
    /*
    log("v.size()", v.size());
    for (const auto& s : v)
      std::cout << " :: " << s << std::endl;
    */

    if (v.size() >= 9) {
      if (!parseDate(v[3], date) || !parseRequestMethod(v[4], httpRequestMethod) || !parseHttpCode(v[5], httpCode)) {
        std::cerr << "Failed to parse line" << std::endl;
        continue;
      }      
      
      if (line_counter % 10000 == 0) {
        std::cout << "size: " << line_counter << std::endl;
        //std::cout << "http: " << httpCode << ", req: " << httpRequestMethod << ", date: " << date << std::endl;
      }

      c.insert(MONGO_COLLECTION, BSON(GENOID << "code" << httpCode << "method" << httpRequestMethod << "date" << static_cast<unsigned>(date) << "line" << line));
      /*
      BSONObjBuilder b;
      b.append("code", httpCode);
      b.append("method", httpRequestMethod);
      b.append("date", static_cast<unsigned>(date));
      b.append("line", line);
      BSONObj p = b.obj();
      c.insert(MONGO_COLLECTION, p);
      */

      err = c.getLastError();
      if (!err.empty()) {
        std::cerr << "DB error: " << err << std::endl;
        return 1;
      }
    }
  }

  log("done, lines count", line_counter);

  cnt = c.count(MONGO_COLLECTION);
  err = c.getLastError();
  if (!err.empty()) {
    std::cerr << "DB error: " << err << std::endl;
    return 1;
  }
  std::cout << MONGO_COLLECTION << ", size: " << cnt << std::endl;

  return 0;
}