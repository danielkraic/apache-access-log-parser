#include "Parser.hpp"

time_t Parser::dateToUnixTime(const std::string& dateString) {
  time_t date;

  try {
    if (dateString.size() != 10) {
      throw std::invalid_argument("Incorrect size of date string");
    }

    if (dateString[4] != '-' || dateString[7] != '-') {
      throw std::invalid_argument("Incorrect date string format");
    }

    std::string year( dateString.substr(0, 4) );
    std::string mon(  dateString.substr(5, 2) );
    std::string day(  dateString.substr(8, 2) );

    date = getTime( std::stoi(year), std::stoi(mon), std::stoi(day) );
  } catch (std::exception& e) {
    std::stringstream ss;
    ss << "Parsing YYYY-MM-DD date error: " << e.what() << ", input string: '" << dateString << "'";
    throw std::invalid_argument(ss.str());
  }

  return date;
}

std::vector<std::string> Parser::parseLine(const std::string& s) {
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

time_t Parser::parseApacheDate(const std::string& dateTimeString) {
  time_t date;

  try {
    if (dateTimeString.size() < 11) {
      throw std::invalid_argument("Incorrect size of datetime string");
    }

    if (dateTimeString[2] != '/' || dateTimeString[6] != '/') {
      throw std::invalid_argument("Incorrect datetime string format");
    }

    std::string day(  dateTimeString.substr(0, 2) );
    std::string mon(  dateTimeString.substr(3, 3) );
    std::string year( dateTimeString.substr(7, 4) );

    date = getTime( std::stoi(year), getMonth(mon), std::stoi(day) );
  } catch (std::exception& e) {
    std::stringstream ss;
    ss << "Parsing apache datetime error: " << e.what() << ", input string: '" << dateTimeString << "'";
    throw std::invalid_argument(ss.str());
  }

  return date;
}

int Parser::parseHttpCode(const std::string& httpCodeString) {
  int httpCode{0};
  try {
    httpCode = std::stoi(httpCodeString);
  } catch (std::exception& e) {
    std::stringstream ss;
    ss << "Parsing http code error: " << e.what() << ", parsing from string: '" << httpCodeString << "'";
    throw std::invalid_argument(ss.str());
  }

  return httpCode;
}

std::string Parser::parseRequestMethod(const std::string& httpRequestString) {
  auto pos = httpRequestString.find(" ", 0);
  if (pos == std::string::npos) {
    std::stringstream ss;
    ss << "Failed to parse request method from string: '" << httpRequestString << "'" << std::endl;
    throw std::invalid_argument(ss.str());
  }

  return httpRequestString.substr(0, pos);
}

time_t Parser::getTime(int year, int mon, int day) {
  struct std::tm t;
  t.tm_sec = 0;
  t.tm_min = 0;
  t.tm_hour = 0;
  t.tm_mday = day;       // day of month (0 .. 31)
  t.tm_mon = mon-1;      // month of year (0 .. 11)
  t.tm_year = year-1900; // year since 1900
  t.tm_isdst = -1;       // determine whether daylight saving time

  time_t tt = std::mktime(&t);
  if (tt == -1) {
    std::stringstream ss;
    ss << "Invalid time (day: " << day << ", mon: " << mon << ", year: " << year << ")";
    throw std::invalid_argument(ss.str());
  }

  return tt;
}

int Parser::getMonth(const std::string& mon) {
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