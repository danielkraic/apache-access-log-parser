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

  boost::optional<std::string> in_file;
  QueryCondition queryCond;

	try {
    namespace po = boost::program_options;

		po::options_description desc("Allowed options");
		desc.add_options()
		    ("help", "produce help message")
		    ("file", po::value<std::string>(), "input apache access log file path")
        ("date", po::value<std::string>(), "date for filtering results")
        ("method", po::value<std::string>(), "request method for filtering results")
        ("code", po::value<std::string>(), "http code for filtering results")
  	;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
		  std::cout << desc << "\n";
		  return 1;
		}

		if (vm.count("file")) { in_file = vm["file"].as<std::string>(); }

    if (vm.count("date"))   { queryCond.date   = vm["date"].as<std::string>(); }
    if (vm.count("method")) { queryCond.method = vm["method"].as<std::string>(); }
    if (vm.count("code"))   { queryCond.code   = vm["code"].as<std::string>(); }

	} catch (std::exception &e) {
    lg.Error("failed to parse cmd args, use --help");
    return 1;
  }

  auto dataStore = std::make_shared<DataStore>();
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
    dataStore->queryData(queryCond);
  }

  return 0;
}