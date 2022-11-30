#ifndef H_PRIME_CHECKER
#define H_PRIME_CHECKER

#include <vector>
#include <string>
#include <forward_list>
#include <mutex>
#include "PrimeCheckerData.h"

class PrimeChecker
{
	using ValueType = unsigned long long;

	static const size_t PRIMES_NUM_COUNT = 203280221;
	static const size_t threadCount = 1;
	const unsigned long desiredCount;

	std::mutex mutexCount;

	std::vector<unsigned long> primes;
	ValueType pc2;
	ValueType pc3;

	ValueType count = 0;
	std::vector<std::tuple<size_t, ValueType, ValueType>> resultPrimes;
	std::vector<std::pair<std::forward_list<ValueType>, std::forward_list<ValueType>>> resultMultipliers;

public:
	explicit PrimeChecker(const std::string filename, const PrimeCheckerData& data, const unsigned long& desCount = 15);

	bool isPrime(const ValueType& num) const;
	std::forward_list<ValueType> getMultipliers(const ValueType& num) const;

	ValueType getPc2() const { return pc2; }
	ValueType getPc3() const { return pc3; }

	void runPrimeCheck2(const ValueType& pc);
	void runPrimeCheck3(const ValueType& pc);

	void printResult() const;
};

#endif