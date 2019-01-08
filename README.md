
### Hamming Distance Library and example app
Hamming distance between two strings of equal length is the number of positions at which the corresponding symbols are different. If strings lengths differ then distance is undefined.

HammingDistance.exe parses command line and calculates hamming distance between two strings given according to options specified.
Usage: 

	HammingDistance blob_string_1 blob_string_2 [options]
    
_Supported options_:
		--help                print help message
		--string              treat input as strings rather than blobs
		--threads arg(= 1)    run in parallel using specified number of threads (specify -1 to use all the logical threads available)

Actual distance calculation is implemented in HammingDistanceLib static library.
x64 version of library calculates hamming distance for blob about 2 times faster, because it is able to process blobs 64 bits at a time, while as x86 version can only process up to 32 bits at time.
Library is tests-covered: see the HDLibTest executable.

To build this solution you will need a Visual Studio 2017 (2015) installation and C++ boost libraries.
In order to compile and link with boost, please set environment variables BOOST_ROOT and BOOST_LIBS to paths to boost root folder and boost compiled libs respectively, e.g.

	set BOOST_ROOT=C:\boost_1_67_0
	set BOOST_LIBS=C:\boost_1_67_0\stage\lib

