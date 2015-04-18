#ifndef QUERY_CONDITION_HPP
#define QUERY_CONDITION_HPP

#include <boost/optional.hpp>

class QueryCondition
{
public:
  boost::optional<std::string> date;
  boost::optional<std::string> method;
  boost::optional<std::string> code;

  bool isValid() {
  	return date || method || code;
  }

  //TODO: add setters with validation
};

#endif // QUERY_CONDITION_HPP
