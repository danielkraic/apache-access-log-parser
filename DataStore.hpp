#ifndef DATA_STORE_HPP
#define DATA_STORE_HPP

#include "QueryCondition.hpp"

class DataStore
{
public:
	DataStore() {}
	~DataStore() = default;

	DataStore(const DataStore&) = delete;
	DataStore(DataStore&&) = delete;
	DataStore operator=(const DataStore&) = delete;
	DataStore operator=(DataStore&&) = delete;

	bool connect() {
		return true;
	}

	bool loadData(const std::string& in_file) {
		return true;
	}

	bool queryData(const QueryCondition& queryCond) {
		return true;
	}

	//TODO: add mongo connector
};

#endif // DATA_STORE_HPP
