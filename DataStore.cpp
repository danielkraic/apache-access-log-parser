#include <fstream>
#include "DataStore.hpp"
#include "Parser.hpp"

DataStore::DataStore(Logger& logger, unsigned threads_num) :
  m_threads_num(threads_num),
  m_mongoDBs(threads_num),
  m_logger(logger)
{
  if (threads_num < 1) {
    throw std::runtime_error("threads_num must be positive integer");
  }
}

bool DataStore::connect() {
  std::string err;

  for (unsigned i = 0; i < m_threads_num; ++i) {
    if (!m_mongoDBs[i].connect( mongo::HostAndPort(m_host), err)) {
      m_logger.Error("thread", i, "Failed to connect DB", err);
      return false;
    }
    m_logger.Debug("thread", i, "DB connected", m_host);
  }

  size_t cnt = m_mongoDBs[0].count(m_collection_name);
  err = m_mongoDBs[0].getLastError();
  if (!err.empty()) {
    m_logger.Error("Mongo error", err);
    return false;
  }
  m_logger.Debug("collection name: ", m_collection_name);
  m_logger.Debug("collection size: ", cnt);

  return true;
}

bool DataStore::loadData(const std::string& in_file) {
  std::ifstream f(in_file);
  if (!f.good()) {
    m_logger.Error("Failed to open file", in_file);
    return false;
  }

  return readItems(f);
}

bool DataStore::readItems(std::ifstream& f)
{
  auto parser = std::make_shared<Parser>();
  std::string line;
  size_t lines_counter{0};
  size_t items_counter{0};

  using namespace mongo;

  std::vector<BSONObj> loaded_data;

  while (getline(f, line)) {
    lines_counter++;

    try {
      auto items = parser->parseLine(line);
      if (items.size() >= 9) {
        loaded_data.push_back(
          BSON( GENOID
            << "date" << std::to_string(parser->parseApacheDate(items[3]))
            << "code" << std::to_string(parser->parseHttpCode(items[5]))
            << "method" << parser->parseRequestMethod(items[4])
            << "line" << line
          )
        );
        items_counter++;
      }
    } catch (std::exception& e) {
      m_logger.Error("Failed to parse line", e.what());
      continue;
    }

    if (loaded_data.size() > 100) {
      insertData(loaded_data);
      loaded_data.clear();
    }
  }

  m_logger.Debug("Lines count: ", items_counter);
  m_logger.Debug("Items count: ", items_counter);
  return true;
}

bool DataStore::insertData(const std::vector<mongo::BSONObj>& loaded_data) {
  //TODO: insert data to DB

  return true;
}

bool DataStore::queryData(const QueryCondition& queryCond) {
  //TODO: query data from DB

  return true;
}