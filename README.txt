Hamming distance between two strings of equal length is the number of positions at which the corresponding symbols are different.
Hamming distance is undefined if strings have different lengths.

HammingDistance.exe parses command line and calculates hamming distance between two strings given according to options specified.
Usage: HammingDistance.exe blob_string_1 blob_string_2 [options]
Allowed options :
	   --help                print help message
	   --string              treat input as strings rather than blobs
	   --threads arg(= 1)    run in parallel using specified number of threads; use - 1 to autodetect the number of logical threads available

Actual distance calculation is implemented in HammingDistanceLib static library.
x64 version of library calculates hamming distance for blob about 2 times faster, 
because it is able to process blobs 64 bits at a time, while as x86 version can only process up to 32 bits at time.
Library is tests-covered: see the HDLibTest executable.

To build library, executable and tests you will need a Visual Studio 2017 (or 2015) installation and C++ boost libraries.
Create new environment variables BOOST_ROOT and BOOST_LIBS and set them to paths to boost root folder and boost compiled libs respectively.
For example: BOOST_ROOT=C:\boost_1_67_0 and BOOST_LIBS=C:\boost_1_67_0\stage\lib.
Finally, open the Hamming Distance.sln solution file and build for configuration and platform of your choice.
