#ifndef PARSER_HPP
#define PARSER_HPP

#include <sstream>
#include <ctime>
#include <stdexcept>
#include <vector>

class Parser
{
public:
  //date format: YYYY-MM-DD
  static time_t dateToUnixTime(const std::string& dateString);

  std::vector<std::string> parseLine(const std::string& s);

  //apache date format: [01/Aug/2013:14:41:53 +0200]
  time_t parseApacheDate(const std::string& dateTimeString);

  int parseHttpCode(const std::string& httpCodeString);

  std::string parseRequestMethod(const std::string& httpRequestString);

private:
  static time_t getTime(int year, int mon, int day);

  static int getMonth(const std::string& mon);
};

#endif // PARSER_HPP
