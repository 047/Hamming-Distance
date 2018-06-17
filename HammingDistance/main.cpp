// HammingDistance parses command line and calculates hamming distance 
// between two strings given according to options specified.
//	Usage: HammingDistance.exe blob1 blob2 [options]
//	Allowed options :
//	   --help                print help message
//	   --string              treat input as strings rather than blobs
//	   --threads arg(= 1)    run in parallel using specified number of threads; 
//	                         use - 1 to autodetect the number of logical threads available

#include <iostream>
#include "HammingDistance.h"
#include <string>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

struct SettingsAndInput {
	std::string blob1;
	std::string blob2;
	bool findStringDistance = false;
	int nThreads = 1;
};

// returns 'true' if command line arguments were parsed succesfully, 'false' otherwise
bool parseOptions(SettingsAndInput& settings, int argc, char* argv[]);

int main(int argc, char* argv[])
{
	SettingsAndInput settings;
	if (!parseOptions(settings, argc, argv)) {
		return 0;
	}
	
	try {
		size_t distance = -1;
		if( settings.findStringDistance ) {
			distance = HammingDistance::StringsDistance(settings.blob1, settings.blob2, settings.nThreads);
		} else {
			distance = HammingDistance::BitsDistance(settings.blob1, settings.blob2, settings.nThreads);
		}
		std::cout << distance;
	} catch (std::exception& ex) {
		std::cout << ex.what();
	}
	return 0;
}

bool parseOptions(SettingsAndInput& settings, int argc, char* argv[])
{
	po::options_description cmdLineOptions("Allowed options");
	cmdLineOptions.add_options()
		("help", "print help message")
		("string", "treat input as strings rather than blobs")
		("threads",
			po::value<int>()->default_value(1),
			"run in parallel using specified number of threads;"
			" use -1 to autodetect the number of logical threads available");
	po::options_description hiddenOptions("Hidden options");
	hiddenOptions.add_options()
		("blob1", po::value<std::string>(), "first input blob")
		("blob2", po::value<std::string>(), "second input blob");
	po::positional_options_description positionalOptions;
	positionalOptions.add("blob1", 1);
	positionalOptions.add("blob2", 1);
	po::options_description allOptions("All options");
	allOptions.add(cmdLineOptions).add(hiddenOptions);
	try {
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
			options(allOptions).positional(positionalOptions).run(), vm);
		po::notify(vm);
		if (vm.count("help") > 0 || vm.count("blob1") < 1 || vm.count("blob2") < 1) {
			std::cout << "Usage: " << argv[0] << " blob1 blob2 [options]\n";
			std::cout << cmdLineOptions;
			return false;
		}
		settings.findStringDistance = vm.count("string") > 0;
		settings.nThreads = vm["threads"].as<int>();
		settings.blob1 = vm["blob1"].as<std::string>();
		settings.blob2 = vm["blob2"].as<std::string>();
	} catch (std::exception& ex) {
		std::cout << ex.what();
		return false;
	}

	return true;
}