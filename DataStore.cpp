#include <fstream>
#include <future>
#include <chrono>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
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
    m_logger.Error("Mongo count() error", err);
    return false;
  }
  m_logger.Debug("collection name: ", m_collection_name);
  m_logger.Debug("collection size: ", cnt);

  m_mongoDBs[0].dropCollection(m_collection_name);
  err = m_mongoDBs[0].getLastError();
  if (!err.empty()) {
    m_logger.Error("Mongo dropCollection() error", err);
    return false;
  }
  
  cnt = m_mongoDBs[0].count(m_collection_name);
  err = m_mongoDBs[0].getLastError();
  if (!err.empty()) {
    m_logger.Error("Mongo count() error", err);
    return false;
  }
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
  boost::iostreams::filtering_istream in;
  in.push(boost::iostreams::gzip_decompressor());
  in.push(f);

  std::string line;
  std::vector<std::string> lines;
  size_t lines_counter{0};

  while (getline(in, line)) {
    lines_counter++;
    lines.push_back(line);

    if (lines.size() >= 1000000) {
      if (!insertData(lines)) {
        m_logger.Debug("insertData failed");
        return false;
      }
      lines.clear();
    }
  }

  if (!insertData(lines)) {
    m_logger.Debug("insertData failed");
    return false;
  }

  m_logger.Debug("Lines count: ", lines_counter);
  return true;
}

bool DataStore::insertData(const std::vector<std::string>& lines) {
  std::vector<std::future<bool>> futures;
  size_t step = lines.size() / m_threads_num;

  for (size_t i = 0; i < m_threads_num; ++i) {
    DataIter itBeg = lines.begin() + (i*step);
    DataIter itEnd = ((i + 1) != m_threads_num) ? lines.begin() + ((i+1)*step) : lines.end();

    futures.push_back(
      std::async(std::launch::async, &DataStore::insertDataItems, this, itBeg, itEnd, i)
    );
  };

  std::vector<bool> res;
  for (auto& f : futures) {
    res.push_back(f.get());
  }

  return std::all_of(res.begin(), res.end(), [](bool result) { return result == true; } );
}

std::string make_id() {
  mongo::OID o = mongo::OID::gen();
  auto str = o.toString();
  std::vector<char> vec(str.begin(), str.end());
  if (!vec.empty()) {
    std::swap(vec[0], vec[vec.size() - 1]);
  }
  std::string id(vec.begin(), vec.end());

  return (!id.empty()) ? id : mongo::OID::gen().toString();
}

bool DataStore::insertDataItems(DataIter itBegin, DataIter itEnd, unsigned thread_num) {
  using namespace mongo;

  size_t items_counter{0};
  std::vector<BSONObj> loaded_data;
  auto parser = std::make_shared<Parser>();
  
  for (DataIter it = itBegin; it != itEnd; it++) {
    try {
      auto items = parser->parseLine(*it);
      if (items.size() >= 9) {
        BSONObjBuilder b;
        b.append("_id", make_id());
        b.appendTimeT("date", parser->parseApacheDate(items[3]));
        b.append("code", parser->parseHttpCode(items[5]));
        b.append("method", parser->parseRequestMethod(items[4]));
        b.append("line", *it);

        loaded_data.push_back(b.obj());
        items_counter++;
      }
    } catch (std::exception& e) {
      m_logger.Error("Failed to parse line", e.what());
      continue;
    }
  }

  auto time_beg = std::chrono::high_resolution_clock::now();

  m_mongoDBs[thread_num].insert(m_collection_name, loaded_data);

  auto time_end = std::chrono::high_resolution_clock::now();

  long duration = std::chrono::duration_cast<std::chrono::seconds>(time_end - time_beg).count();
  m_logger.Debug(
          "Insert count", items_counter,
          "duration", duration,
          "inserts/second", (items_counter != 0) ? static_cast<double>(loaded_data.size()/duration) : 0.0
  );

  std::string err = m_mongoDBs[thread_num].getLastError();
  if (!err.empty()) {
    m_logger.Error("Mongo error", err);
    return false;
  }

  return true;
}

bool DataStore::queryData(const mongo::BSONObj& queryObj) {
  m_logger.Debug("queryData() ", queryObj.jsonString());

  size_t cnt = m_mongoDBs[0].count(m_collection_name);
  std::string err = m_mongoDBs[0].getLastError();
  if (!err.empty()) {
    m_logger.Error("Mongo count() error", err);
    return false;
  }
  m_logger.Debug("collection name: ", m_collection_name);
  m_logger.Debug("collection size: ", cnt);

  auto cursor = m_mongoDBs[0].query(m_collection_name, queryObj);
  err = m_mongoDBs[0].getLastError();
  if (!err.empty()) {
    m_logger.Error("Mongo query() error", err);
    return false;
  }

  while (cursor->more()) {
    mongo::BSONObj p = cursor->next();
    std::cout << p.getStringField("line") << std::endl;
  }

  return true;
}
