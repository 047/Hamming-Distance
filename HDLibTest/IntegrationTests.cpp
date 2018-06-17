// Integration tests for HammingDistanceLib.
// Test are ensuring that library's public functions return correct distance
// and throw an exception in case of invalid arguments

#include <boost/test/unit_test.hpp>
#include "HammingDistance.h"
#include <limits>

using namespace HammingDistance;

BOOST_AUTO_TEST_CASE(funcitonal_test_distant_strings)
{
	std::string str1{ "qwekqpwepoasdoaspdkasdoaskdpkoasopdkoqwoe" };
	std::string str2{ "oewqnreiosvoisodsdfwiern233o2oad012k12ok3" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	BOOST_TEST(BitsDistance(str1, str2) == 119);
	BOOST_TEST(StringsDistance(str1, str2) == 37);
	BOOST_TEST(StringsDistance(str1, str2) == StringsDistance(str2, str1));
}

BOOST_AUTO_TEST_CASE(funcitonal_test_equal_strings)
{
	std::string str1{ "qwerty" };
	std::string str2{ "qwerty" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	BOOST_TEST(BitsDistance(str1, str2) == 0);
	BOOST_TEST(StringsDistance(str1, str2) == 0);
}

BOOST_AUTO_TEST_CASE(funcitonal_test_zero_strings)
{
	std::string str1{ "" };
	std::string str2{ "" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	BOOST_TEST(BitsDistance(str1, str2) == 0);
	BOOST_TEST(StringsDistance(str1, str2) == 0);
}

BOOST_AUTO_TEST_CASE(funcitonal_test_distant_strings_multithread_equals_singlethread)
{
	std::string str1{ "qwekqpwepoasdoaspdkasdoaskdpkoasopdkoqwoe" };
	std::string str2{ "oewqnreiosvoisodsdfwiern233o2oad012k12ok3" };
	BOOST_TEST_REQUIRE(str1.size() == str2.size());
	const size_t singleThread = BitsDistance(str1, str2);
	const size_t multiThread = BitsDistance(str1, str2, -1);
	const int maxThreads = std::numeric_limits<int>::max();
	const size_t maxMultiThread = BitsDistance(str1, str2, maxThreads);
	BOOST_TEST(singleThread == 119);
	BOOST_TEST(singleThread == multiThread);
	BOOST_TEST(singleThread == maxMultiThread);

	const size_t singleThreadStr = StringsDistance(str1, str2);
	const size_t multiThreadStr = StringsDistance(str1, str2, -1);
	const size_t maxMultiThreadStr = StringsDistance(str1, str2, maxThreads);
	BOOST_TEST(singleThreadStr == 37);
	BOOST_TEST(singleThreadStr == multiThreadStr);
	BOOST_TEST(singleThreadStr == maxMultiThreadStr);
}

BOOST_AUTO_TEST_CASE(funcitonal_test_unequal_length_strings)
{
	std::string str1{ "qwekqpwepoasdoaspdkasdoaskdp" };
	std::string str2{ "oewqnreiosvoisodsdfwiern233owwq" };
	BOOST_TEST_REQUIRE(str1.size() != str2.size());
	BOOST_CHECK_THROW(BitsDistance(str1, str2), std::invalid_argument);
	BOOST_CHECK_THROW(StringsDistance(str1, str2), std::invalid_argument);
	// check that invalid_argument is thrown when negative number of threads (but != -1) is specified
	BOOST_CHECK_THROW(StringsDistance("", "", -2), std::invalid_argument);
}
