// Unit tests for internal logic of HammingDistanceLib.
// Test are ensuring that: 
//	1 - All three hamming distance functions return correct ditance
//	2 - The driver function returns correct results for single thread and multithread cases
// It is best to build for x64 because hammingDistancePopCnt64 is only available for x64 platforms 

#define BOOST_TEST_MODULE Hamming Distance Library Tests
#include <boost/test/included/unit_test.hpp>
#include <string>

// Check if compiling for x64 architecture
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
#define MACHINE64
#endif

// Library's internal logic consists of distance functions and driver function - calculateHammingDistance
namespace HammingDistance {
	// popcnt functions return distance for input strings treated as blobs
#ifdef MACHINE64
	typedef size_t(*PopCountPtr)(int64_t value);
	size_t popCount(int64_t value);
#else
	typedef size_t(*PopCountPtr)(int value);
	size_t popCount(int value);
#endif
	
	template<typename IntType, PopCountPtr pcnt>
	size_t hammingDistancePopCnt(std::string const& blob1, std::string const& blob2, size_t start, size_t end);
	// string distance function returns distance for input strings treated as strings
	size_t hammingDistanceString(std::string const& str1, std::string const& str2, size_t start, size_t end);
	
	typedef size_t(*hammingFunctionPtr)(std::string const& blob1, std::string const& blob2, size_t start, size_t end);
	// the driver function manages single and multithread computation of distance
	size_t calculateHammingDistance(hammingFunctionPtr hammingFunc, std::string const& blob1, std::string const& blob2, int nThreads);
}

using namespace HammingDistance;

BOOST_AUTO_TEST_CASE(test_distance_functions_distant_strings_1)
{
	std::string str1{ "qwekqpwepoasdoaspdkasdoaskdpkoasopdkoqwoe" };
	std::string str2{ "oewqnreiosvoisodsdfwiern233o2oad012k12ok3" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	const size_t strLen = str1.size();
	size_t distance = hammingDistancePopCnt<int, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 119);
#ifdef MACHINE64
	distance = hammingDistancePopCnt<int64_t, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 119);
#endif
	BOOST_TEST(hammingDistanceString(str1, str2, 0, strLen) == 37);
}

BOOST_AUTO_TEST_CASE(test_distance_functions_distant_strings_2)
{
	std::string str1{ "11010101010101010101010101" };
	std::string str2{ "00101010101010101010100011" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	const size_t strLen = str1.size();
	size_t distance = hammingDistancePopCnt<int, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 24);
#ifdef MACHINE64
	distance = hammingDistancePopCnt<int64_t, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 24);
#endif
	BOOST_TEST(hammingDistanceString(str1, str2, 0, strLen) == 24);
}

BOOST_AUTO_TEST_CASE(test_distance_functions_same_strings)
{
	std::string str1{ "123" };
	std::string str2{ "123" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	const size_t strLen = str1.size();
	size_t distance = hammingDistancePopCnt<int, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 0);
#ifdef MACHINE64
	distance = hammingDistancePopCnt<int64_t, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 0);
#endif
	BOOST_TEST(hammingDistanceString(str1, str2, 0, strLen) == 0);
}

BOOST_AUTO_TEST_CASE(test_distance_functions_bits_distance_ne_string_distance)
{
	std::string str1{ "123A" };
	std::string str2{ "123Z" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	const size_t strLen = str1.size();
	size_t distance = hammingDistancePopCnt<int, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 4);
#ifdef MACHINE64
	distance = hammingDistancePopCnt<int64_t, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == 4);
#endif
	BOOST_TEST(hammingDistanceString(str1, str2, 0, strLen) == 1);
}

BOOST_AUTO_TEST_CASE(test_distance_functions_subsequence_distance_sum_to_full_sequence)
{
	std::string str1{ "jkdsfosdifwenrwekiwe0rsdsf" };
	std::string str2{ "sldapqe9299999999991iiiiif" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	const size_t strLen = str1.size();
	// accumulate distances of subsequences in respective accumulator variables
	size_t subseqBlobTotalDistance = 0;
	size_t subseqStrTotalDistance = 0;
#ifdef MACHINE64
	size_t subseqBlob64TotalDistance = 0;
#endif
	const size_t nParts = 5;
	const size_t stride = strLen / nParts;
	for (size_t i = 0; i < nParts - 1; ++i) {
		subseqBlobTotalDistance += hammingDistancePopCnt<int, popCount>(str1, str2, i * stride, (i + 1) * stride );
		subseqStrTotalDistance += hammingDistanceString(str1, str2, i * stride, (i + 1) * stride);
#ifdef MACHINE64
		subseqBlob64TotalDistance += hammingDistancePopCnt<int64_t, popCount>(str1, str2, i * stride, (i + 1) * stride);
#endif
	}
	subseqBlobTotalDistance += hammingDistancePopCnt<int, popCount>(str1, str2, (nParts - 1) * stride, strLen);
	subseqStrTotalDistance += hammingDistanceString(str1, str2, (nParts - 1) * stride, strLen);
	
	// test that sum of distances of subsequences equals distance of the whole sequence
	size_t distance = hammingDistancePopCnt<int, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == subseqBlobTotalDistance);
#ifdef MACHINE64
	subseqBlob64TotalDistance += hammingDistancePopCnt<int64_t, popCount>(str1, str2, (nParts - 1) * stride, strLen);
	distance = hammingDistancePopCnt<int64_t, popCount>(str1, str2, 0, strLen);
	BOOST_TEST(distance == subseqBlob64TotalDistance);
	BOOST_TEST(subseqBlobTotalDistance == subseqBlob64TotalDistance);
#endif
	BOOST_TEST(hammingDistanceString(str1, str2, 0, strLen) == subseqStrTotalDistance);
}

BOOST_AUTO_TEST_CASE(test_driver_function_singleThread_equals_multithread)
{
	std::string str1{ "sldapqe9299999999991iiiiif" };
	std::string str2{ "jkdsfosdifwenrwekiwe0rsdsf" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	const size_t strLen = str1.size();
	const size_t singleThreadResult = calculateHammingDistance(hammingDistancePopCnt<int, popCount>, str1, str2, 1);
	const size_t multiThreadResult = calculateHammingDistance(hammingDistancePopCnt<int, popCount>, str1, str2, -1);
	BOOST_TEST(singleThreadResult == 88);
	BOOST_TEST(singleThreadResult == multiThreadResult);
#ifdef MACHINE64
	const size_t singleThreadResult64 = calculateHammingDistance(hammingDistancePopCnt<int64_t, popCount>, str1, str2, 1);
	const size_t multiThreadResult64 = calculateHammingDistance(hammingDistancePopCnt<int64_t, popCount>, str1, str2, -1);
	BOOST_TEST(singleThreadResult64 == 88);
	BOOST_TEST(singleThreadResult64 == multiThreadResult64);
#endif
	const size_t singleThreadResultStr = calculateHammingDistance(hammingDistanceString, str1, str2, 1);
	const size_t multiThreadResultStr = calculateHammingDistance(hammingDistanceString, str1, str2, -1);
	BOOST_TEST(singleThreadResultStr == 24);
	BOOST_TEST(singleThreadResultStr == multiThreadResultStr);
}