#include <iostream>
#include <string>
#include <memory>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "Logger.hpp"
#include "DataStore.hpp"
#include "QueryCondition.hpp"

int main(int argc, char const *argv[])
{
  Logger lg;
  lg.setLevel(Logger::Level::DEBUG);

  if (argc < 2) {
    lg.Notice("No options specified. Use --help for more info");
    return 0;
  }

  int threads_num{1};
  boost::optional<std::string> in_file;
  QueryCondition queryCond;

	try {
    namespace po = boost::program_options;

		po::options_description desc("Allowed options");
		desc.add_options()
		    ("help,h",    "produce help message")
		    ("threads,t", po::value<int>()->default_value(1), "number of insert threads (1-100)")
        ("file,f",    po::value<std::string>(), "input gzipped apache access log file")
        ("date,d",    po::value<std::string>(), "date (in format YYYY-MM-DD) for filtering results")
        ("method,m",  po::value<std::string>(), "request method (GET, POST,...) for filtering results")
        ("code,c",    po::value<int>(), "http code number for filtering results")
  	;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
		  std::cout << desc << "\n";
		  return 1;
		}

    if (vm.count("threads")) {
      threads_num = vm["threads"].as<int>();
      if (threads_num < 1) {
        throw std::runtime_error("'threads' must be positive integer");
      }
      if (threads_num > 100) {
        throw std::runtime_error("'threads' must be <= 100"); 
      }
    }
		if (vm.count("file"))    { in_file = vm["file"].as<std::string>(); }

    if (vm.count("date"))   { queryCond.setDate(vm["date"].as<std::string>()); }
    if (vm.count("method")) { queryCond.setMethod(vm["method"].as<std::string>()); }
    if (vm.count("code"))   { queryCond.setCode(vm["code"].as<int>()); }

	} catch (std::exception &e) {
    lg.Error("Failed to parse cmd args", e.what());
    return 1;
  }
  
  mongo::client::initialize();
  auto dataStore = std::make_shared<DataStore>(lg, static_cast<unsigned>(threads_num));
  if (!dataStore->connect()) {
    lg.Error("Failed to connect to DataStore");
    return 1;
  }

  if (in_file) {
  	if (!boost::filesystem::exists(in_file.get())) {
      lg.Error("file not exists", in_file.get());
      return 1;
    } else if (!boost::filesystem::is_regular_file(in_file.get())) {
      lg.Error("file is not regural file", in_file.get());
      return 1;
    }
    lg.Debug("in_file OK", in_file.get());

    dataStore->loadData(in_file.get());
  }

  if (queryCond.isValid()) {
    dataStore->queryData(queryCond.getQueryObj());
  } else {
    lg.Debug("No query specified");
  }

  return 0;
}