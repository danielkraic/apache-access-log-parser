#ifndef QUERY_CONDITION_HPP
#define QUERY_CONDITION_HPP

#include <boost/optional.hpp>
#include "Parser.hpp"

class QueryCondition
{
public:
  QueryCondition() : m_unix_time(), m_method(), m_code() {}

  ~QueryCondition() = default;

  QueryCondition(const QueryCondition&) = delete;
  QueryCondition(QueryCondition&&) = delete;
  QueryCondition& operator=(const QueryCondition&) = delete;
  QueryCondition& operator=(QueryCondition&&) = delete;

  boost::optional<time_t>      getDate()   { return m_unix_time; }  
  boost::optional<std::string> getMethod() { return m_method; }
  boost::optional<int>         getCode()   { return m_code; }

  void setDate(const std::string& date) {
    m_unix_time = Parser::dateToUnixTime(date);
  }

  void setMethod(const std::string& method) {
    m_method = method;
  }

  void setCode(const int code) {
    m_code = code;
  }

  bool isValid() {
  	return m_unix_time || m_method || m_code;
  }

private:
  boost::optional<time_t>      m_unix_time;
  boost::optional<std::string> m_method;
  boost::optional<int>         m_code;
};

#endif // QUERY_CONDITION_HPP
