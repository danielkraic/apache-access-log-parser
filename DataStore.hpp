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
	const unsigned m_threads_num;
	std::vector<mongo::DBClientConnection> m_mongoDBs;
	Logger& m_logger;
	static const constexpr char * m_host = "localhost";
	static const constexpr char * m_collection_name = "apache_log_data.apache_data";

	bool readItems(std::ifstream& f);
	bool insertData(const std::vector<mongo::BSONObj>& loaded_data);
};

#endif // DATA_STORE_HPP
