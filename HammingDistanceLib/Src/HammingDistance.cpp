// Hamming distance calculation
// public functions BitsDistance and StringsDistance check if strings lengths match
// (and throw an exception if they are not) and call calculateHammingDistance.
// calculateHammingDistance uses a pointer to actual distance calculation function
// and implements multithreaded sequences processing

#include "HammingDistance.h"
#include <future>
#include <cassert>
#include <list>
#include <stdexcept>

// Check if compiling for x64 architecture
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
#define MACHINE64
#endif

#if defined(__GNUC__) || defined(__GNUG__)
#define popcount(val) __builtin_popcount(val)
#ifdef MACHINE64
#define popcount64(val) __builtin_popcountll(val)
#endif
#elif defined(_MSC_VER)
#include <intrin.h>
#define popcount(val) __popcnt(val)
#ifdef MACHINE64
#define popcount64(val) __popcnt64(val)
#endif
#endif

namespace HammingDistance {

// popcount returns numbers of one bits in a 32-bit unsigned integer
#ifdef MACHINE64
typedef size_t(*PopCountPtr)(int64_t value);
size_t popCount(int64_t value)
{
	return popcount64(value);
}
#else
typedef size_t(*PopCountPtr)(int value);
size_t popCount(int value)
{
	return popcount(value);
}
#endif

typedef size_t(*hammingFunctionPtr)(std::string const& blob1, std::string const& blob2, size_t start, size_t end);

// distance between blob1 and blob2 when both strings are treaded as blob arrays
template<typename IntType, PopCountPtr pcnt>
size_t hammingDistancePopCnt(std::string const& blob1, std::string const& blob2, size_t start, size_t end)
{
	static_assert(std::is_same<char, std::string::value_type>::value, "assume string's value_type is char");
	constexpr int intSize = sizeof(IntType) / sizeof(char);
	const IntType* b1ptr = reinterpret_cast<const IntType*>(&blob1[start]);
	const IntType* b2ptr = reinterpret_cast<const IntType*>(&blob2[start]);
	const size_t chSize = end - start;
	volatile size_t distance = 0;
	// process blobs 32 bits at a time
	for (size_t i = 0; i < chSize / intSize; ++i) {
		distance += pcnt(b1ptr[i] ^ b2ptr[i]);
	}
	// process blobs 8 bits at a time if there any bytes left
	const size_t leftOverBytes = chSize % intSize;
	if (leftOverBytes != 0) {
		for (size_t i = end - leftOverBytes; i < end; ++i) {
			distance += popcount(blob1[i] ^ blob2[i]);
		}
	}
	return distance;
}

namespace {
	const char unequalStringsMessage[] = "hamming distance is undefined for strings of unequal length";
	const char unequalBlobsMessage[] = "hamming distance is undefined for blobs of unequal length";
	// must instantiate <int, popCount> version of template so that HDLibTest links
	hammingFunctionPtr instatiationPtr = hammingDistancePopCnt<int, popCount>;
}

// distance between str1 and str2 when both strings are treaded as actual character strings
size_t hammingDistanceString(std::string const& str1, std::string const& str2, size_t start, size_t end)
{
	size_t distance = 0;
	for (size_t i = start; i < end; ++i) {
		if (str1[i] != str2[i]) {
			++distance;
		}
	}
	return distance;
}

// In singlethreaded case simply call distance calculation function
// For multithread sequences are split equally among threads so that distance is calculated in parallel
size_t calculateHammingDistance(hammingFunctionPtr hammingFunc, std::string const& blob1, std::string const& blob2, int nThreads)
{
	if (nThreads == 1) {
		return hammingFunc(blob1, blob2, 0, blob1.size());
	} else {
		if (nThreads != -1 && nThreads <= 0) {
			throw std::invalid_argument("nThreads must be >= 1 or -1; " + std::to_string(nThreads) + " given");
		}
		const int blobBytesCount = static_cast<int>( blob1.size() );
		// -1 means 'use as many threads as supported by current system'
		if (nThreads == -1) {
			// hardware_concurrency will return 0 if fails to detect number of hardware thread contexts
			nThreads = std::max(1u, std::thread::hardware_concurrency());
		}
		// do not allow more threads than there are bytes in given blobs
		nThreads = std::min(nThreads, blobBytesCount);
		assert(nThreads >= 1);
		// every thread calculates hamming distance for a subsequence of blobs
		const int stride = blobBytesCount / nThreads;
		const size_t nExtraThreads = nThreads - 1;
		std::list<std::future<size_t>> futures;
		for (size_t i = 0; i < nExtraThreads; ++i) {
			futures.emplace_back(std::async(std::launch::async,
				hammingFunc, std::cref(blob1), std::cref(blob2), i * stride, (i + 1) * stride));
		}
		// ditance of the trailing subsequence
		size_t distance = hammingFunc(blob1, blob2, nExtraThreads * stride, blobBytesCount);
		int finished = 1;
		// poll threads while they are runnig
		while (finished < nThreads) {
			for (auto it = futures.begin(); it != futures.end();) {
				if (it->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					distance += it->get();
					++finished;
					// thread's job done, no need to hold it's future anymore
					it = futures.erase(it);
				} else {
					++it;
				}
			}
		}
		return distance;
	}
}

size_t BitsDistance(std::string const& blob1, std::string const& blob2, int nThreads)
{
#ifdef MACHINE64
	// For x64 version a faster popcnt intrinsic will be called
	hammingFunctionPtr hammingDistance = hammingDistancePopCnt<int64_t, popCount>;
#else
	hammingFunctionPtr hammingDistance = hammingDistancePopCnt<int, popCount>;
#endif
	if (blob1.size() != blob2.size()) {
		throw std::invalid_argument(unequalBlobsMessage);
	}
	return calculateHammingDistance(hammingDistance, blob1, blob2, nThreads);
}

size_t StringsDistance(std::string const& str1, std::string const& str2, int nThreads)
{
	if (str1.size() != str2.size()) {
		throw std::invalid_argument(unequalStringsMessage);
	}
	return calculateHammingDistance(hammingDistanceString, str1, str2, nThreads);
}

}