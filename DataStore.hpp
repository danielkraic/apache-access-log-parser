#ifndef DATA_STORE_HPP
#define DATA_STORE_HPP

#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
#include "QueryCondition.hpp"
#include "Logger.hpp"

class DataStore
{
public:
	DataStore(Logger& logger, unsigned threads_num);
	~DataStore() = default;

	DataStore(const DataStore&) = delete;
	DataStore(DataStore&&) = delete;
	DataStore operator=(const DataStore&) = delete;
	DataStore operator=(DataStore&&) = delete;

	bool connect();

	bool loadData(const std::string& in_file);

	bool queryData(const QueryCondition& queryCond);

private:
	using DataIter = std::vector<std::string>::const_iterator;

	const unsigned m_threads_num;
	std::vector<mongo::DBClientConnection> m_mongoDBs;
	Logger& m_logger;
	static const constexpr char * m_host = "localhost";
	static const constexpr char * m_collection_name = "apache_log_data.apache_data";

	bool readItems(std::ifstream& f);
	bool insertData(const std::vector<std::string>& lines);
	bool insertDataItems(DataIter itBegin, DataIter itEnd, unsigned thread_num);
};

#endif // DATA_STORE_HPP
