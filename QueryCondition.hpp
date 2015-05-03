#ifndef QUERY_CONDITION_HPP
#define QUERY_CONDITION_HPP

#include <boost/optional.hpp>
#include <mongo/bson/bson.h>
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

  boost::optional<time_t>      getDate() const   { return m_unix_time; }
  boost::optional<std::string> getMethod() const { return m_method; }
  boost::optional<int>         getCode() const   { return m_code; }

  void setDate(const std::string& date) {
    m_unix_time = Parser::dateToUnixTime(date);
  }

  void setMethod(const std::string& method) {
    m_method = method;
  }

  void setCode(const int code) {
    m_code = code;
  }

  bool isValid() const {
  	return m_unix_time || m_method || m_code;
  }

  mongo::BSONObj getQueryObj() const {
    mongo::BSONObjBuilder ret;

    if (m_unix_time)
      ret << "date" << std::to_string(m_unix_time.get());

    if (m_method)
      ret << "method" << m_method.get();

    if (m_code)
      ret << "code" << std::to_string(m_code.get());

    return ret.obj();
  }

private:
  boost::optional<time_t>      m_unix_time;
  boost::optional<std::string> m_method;
  boost::optional<int>         m_code;
};

#endif // QUERY_CONDITION_HPP
