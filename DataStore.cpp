#include <fstream>
#include "DataStore.hpp"
#include "Parser.hpp"

DataStore::DataStore(Logger& logger) :
  m_mongo(),
  m_logger(logger)
{
}

bool DataStore::connect() {
  std::string err;
  if (!m_mongo.connect( mongo::HostAndPort(m_host), err)) {
    m_logger.Error("Failed to connect DB", err);
    return false;
  }
  m_logger.Debug("DB connected", m_host);
 
  size_t cnt = m_mongo.count(m_collection_name);
  err = m_mongo.getLastError();
  if (!err.empty()) {
    m_logger.Error("Mongo error", err);
    return false;
  }
  m_logger.Debug(m_collection_name, cnt);
  
  return true;
}

bool DataStore::loadData(const std::string& in_file) {
  std::ifstream f(in_file);
  if (!f.good()) {
    m_logger.Error("Failed to open file", in_file);
    return false;
  }

  auto parser = std::make_shared<Parser>();
  std::string line;
  size_t line_counter{0};

  using LineData_t = std::tuple<time_t, int, std::string, std::string>;
  std::vector< LineData_t > loaded_data;

  while (getline(f, line)) {
    line_counter++;

    try {
      auto items = parser->parseLine(line);
      if (items.size() >= 9) {
        loaded_data.push_back(
          LineData_t(
            parser->parseApacheDate(items[3]),
            parser->parseHttpCode(items[5]),
            parser->parseRequestMethod(items[4]),
            line)
        );
      }
    } catch (std::exception& e) {
      m_logger.Error("Failed to parse line", e.what());
      continue;
    }

    //TODO: add mongo insert
  }
  
  m_logger.Debug("Loaded data", loaded_data.size());
  return true;
}

bool DataStore::queryData(const QueryCondition& queryCond) {
  return true;
}