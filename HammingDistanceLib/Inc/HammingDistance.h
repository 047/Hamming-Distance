// Hamming distance between two strings of equal length is the number 
// of positions at which the corresponding symbols are different.
// Hamming distance is undefined if strings have different length.

#pragma once

#include <string>

namespace HammingDistance {
	
	// Returns hamming distance between two blobs
	// For nThreads > 1 calculates distance using multiple threads
	// For nThreads = -1 automatically detects the maximum number of logical threads available
	// Example: BitsDistance("123A", "123Z") returns 4
	// Throws invalid_argument in case if strings length mismatch OR nThreads <= 0 and not -1
	size_t BitsDistance(std::string const& blob1, std::string const& blob2, int nThreads = 1);
	
	// Returns hamming distance between two strings
	// For nThreads > 1 calculates distance using multiple threads
	// For nThreads = -1 automatically detects the maximum number of logical threads available
	// Example: StringsDistance("123A", "123Z") returns 1
	// Throws invalid_argument in case if strings length mismatch OR nThreads <= 0 and not -1
	size_t StringsDistance(std::string const& str1, std::string const& str2, int nThreads = 1);

}