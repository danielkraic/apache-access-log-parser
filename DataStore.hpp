#ifndef DATA_STORE_HPP
#define DATA_STORE_HPP

#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
#include "QueryCondition.hpp"
#include "Logger.hpp"

class DataStore
{
public:
	DataStore(Logger& logger);
	~DataStore() = default;

	DataStore(const DataStore&) = delete;
	DataStore(DataStore&&) = delete;
	DataStore operator=(const DataStore&) = delete;
	DataStore operator=(DataStore&&) = delete;

	bool connect();

	bool loadData(const std::string& in_file);

	bool queryData(const QueryCondition& queryCond);

private:
	mongo::DBClientConnection m_mongo;
	Logger& m_logger;
	static const constexpr char * m_host = "localhost";
	static const constexpr char * m_collection_name = "apache_log_data.apache_data";
};

#endif // DATA_STORE_HPP
