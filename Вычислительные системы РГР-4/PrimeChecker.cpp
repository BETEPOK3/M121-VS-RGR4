#include <fstream>
#include <numeric>
#include <iostream>
#include <thread>
#include "PrimeChecker.h"

using namespace std;

PrimeChecker::PrimeChecker(const std::string filename, const PrimeCheckerData& data, const unsigned long& desCount) : desiredCount { desCount }
{
	// Чтение из файла всех значений 2-байтных чисел
	ifstream fin(filename, ios::binary | ios::in);
	primes.reserve(PRIMES_NUM_COUNT);
	for (size_t i = 0; i < PRIMES_NUM_COUNT; ++i)
	{
		unsigned long num;
		fin.read(reinterpret_cast<char*>(&num), sizeof(num));
		primes.push_back(num);
	}
	fin.close();

	// Определение pc2 и pc3 переменных
	if (data.N <= 6)
	{
		pc2 = static_cast<ValueType>(pow(2, 18 + data.N));
	}
	else
	{
		pc2 = static_cast<ValueType>(pow(2, 31 - data.N));
	}
	unsigned int tempNum = (data.N + data.MM + data.DD) % 7;
	if (tempNum > 3)
	{
		pc3 = static_cast<ValueType>(pow(3, 7 + tempNum));
	}
	else
	{
		pc3 = static_cast<ValueType>(pow(3, 13 - tempNum));
	}
}

bool PrimeChecker::isPrime(const ValueType& num) const
{
	for (const auto& prime : primes)
	{
		if (static_cast<ValueType>(prime) * static_cast<ValueType>(prime) > num)
		{
			return true;
		}
		if (num % prime == 0)
		{
			return false;
		}
	}
	return true;
}

forward_list<pair<PrimeChecker::ValueType, size_t>> PrimeChecker::getMultipliers(ValueType num) const
{
	std::forward_list<pair<ValueType, size_t>> result;
	auto it = primes.begin();
	while (it < primes.end() && *it <= num)
	{
		ValueType mult = *it;
		size_t power = 0;
		while (num % mult == 0)
		{
			++power;
			mult *= *it;
		}
		if (power > 0)
		{
			result.push_front(make_pair(*it, power));
		}
		num /= mult / *it;
		++it;
	}
	if (num > 1)
	{
		result.push_front(make_pair(num, 1));
	}
	return result;
}

void PrimeChecker::runPrimeCheck2(const ValueType& pc)
{
	count = 0;
	maxDiff = { pc, 0 };
	resultPrimes.clear();
	resultMultipliers.clear();
	resultPrimes.reserve(desiredCount);

	// Запись первых desiredCount (по умолчанию 15) пар простых чисел
	ValueType idxNextStart = 1;
	for (ValueType i = idxNextStart; i < pc - 2; ++i)
	{
		ValueType val1 = pc - i;
		ValueType val2 = pc + i;
		if (isPrime(val1) && isPrime(val2))
		{
			++count;
			if (count <= desiredCount)
			{
				resultPrimes.push_back(make_tuple(i, val1, val2));
				if (count == desiredCount)
				{
					idxNextStart = i + 1;
					break;
				}
			}
			else
			{
				idxNextStart = i + 1;
				break;
			}
		}
	}

	// Дальше идёт только подсчёт кол-ва таких чисел
	const auto func = [&](const ValueType idxStart, const ValueType& idxEnd)
	{
		ValueType myCount = 0;
		ValueType myMaxDiff = 0;
		for (ValueType i = idxStart; i < idxEnd; ++i)
		{
			ValueType val1 = pc - i;
			ValueType val2 = pc + i;
			if (isPrime(val1) && isPrime(val2))
			{
				++myCount;
				myMaxDiff = i;
			}
		}
		mutexCount.lock();
		count += myCount;
		if (myMaxDiff > maxDiff.second)
		{
			maxDiff.second = myMaxDiff;
		}
		mutexCount.unlock();
	};
	ValueType idxStep = (pc - idxNextStart - 2) / threadCount;
	vector<thread> threads;
	threads.reserve(threadCount);
	while (threads.size() < threadCount)
	{
		threads.push_back(thread(func, idxNextStart, threads.size() == threadCount - 1 ? pc - 2 : idxNextStart + idxStep));
		idxNextStart += idxStep;
	}
	for (auto& thr : threads)
	{
		thr.join();
	}
}

void PrimeChecker::runPrimeCheck3(const ValueType& pc)
{
	count = 0;
	maxDiff = { pc, 0 };
	resultPrimes.clear();
	resultMultipliers.clear();
	resultPrimes.reserve(desiredCount);
	resultMultipliers.reserve(desiredCount);

	// Запись первых desiredCount (по умолчанию 15) групп из 3-ёх взаимно простых чисел
	ValueType idxNextStart = 1;
	for (ValueType i = idxNextStart; i < pc - 2; ++i)
	{
		ValueType val1 = pc - i;
		ValueType val2 = pc + i;
		if (gcd(pc, val1) == 1 && gcd(pc, val2) == 1 && gcd(val1, val2) == 1)
		{
			++count;
			if (count <= desiredCount)
			{
				resultPrimes.push_back(make_tuple(i, val1, val2));
				resultMultipliers.push_back(make_pair(getMultipliers(val1), getMultipliers(val2)));
				if (count == desiredCount)
				{
					idxNextStart = i + 1;
					break;
				}
			}
			else
			{
				idxNextStart = i + 1;
				break;
			}
		}
	}

	// Дальше идёт только подсчёт кол-ва таких групп чисел
	const auto func = [&](const ValueType idxStart, const ValueType& idxEnd)
	{
		ValueType myCount = 0;
		ValueType myMaxDiff = 0;
		for (ValueType i = idxStart; i < idxEnd; ++i)
		{
			ValueType val1 = pc - i;
			ValueType val2 = pc + i;
			if (gcd(pc, val1) == 1 && gcd(pc, val2) == 1 && gcd(val1, val2) == 1)
			{
				++myCount;
				myMaxDiff = i;
			}
		}
		mutexCount.lock();
		count += myCount;
		if (myMaxDiff > maxDiff.second)
		{
			maxDiff.second = myMaxDiff;
		}
		mutexCount.unlock();
	};
	ValueType idxStep = (pc - idxNextStart - 2) / threadCount;
	vector<thread> threads;
	threads.reserve(threadCount);
	while (threads.size() < threadCount)
	{
		threads.push_back(thread(func, idxNextStart, threads.size() == threadCount - 1 ? pc - 2 : idxNextStart + idxStep));
		idxNextStart += idxStep;
	}
	for (auto& thr : threads)
	{
		thr.join();
	}
}

void PrimeChecker::printResult() const
{
	if (resultMultipliers.empty())
	{
		for (const auto& [idx, val1, val2] : resultPrimes)
		{
			cout << idx << ": " << val1 << ' ' << val2 << '\n';
		}

		// Последнее значение
		const ValueType& idx = maxDiff.second;
		const ValueType val1 = maxDiff.first - idx;
		const ValueType val2 = maxDiff.first + idx;
		cout << "...\n" << idx << ": " << val1 << ' ' << val2 << '\n';
	}
	else
	{
		const auto printList = [](const forward_list<pair<ValueType, size_t>>& listIn)
		{
			cout << '[';
			auto lst = *listIn.begin();
			if (!listIn.empty())
			{
				cout << listIn.front().first << '^' << listIn.front().second;
				for (auto it = ++listIn.begin(); it != listIn.end(); ++it)
				{
					cout << ", " << (*it).first << '^' << (*it).second;
				}
			}
			cout << "]\n";
		};
		for (size_t i = 0; i < resultPrimes.size(); ++i)
		{
			const auto& [idx, val1, val2] = resultPrimes[i];
			const auto& [list1, list2] = resultMultipliers[i];
			cout << idx << ": " << val1 << ' ' << val2 << '\n';
			printList(list1);
			printList(list2);
		}

		// Последнее значение
		const ValueType& idx = maxDiff.second;
		const ValueType val1 = maxDiff.first - idx;
		const ValueType val2 = maxDiff.first + idx;
		const forward_list<pair<ValueType, size_t>> list1 = getMultipliers(val1);
		const forward_list<pair<ValueType, size_t>> list2 = getMultipliers(val2);
		cout << "...\n" << idx << ": " << val1 << ' ' << val2 << '\n';
		printList(list1);
		printList(list2);
	}
	cout << '\n';
}

void PrimeChecker::printResultToCsv(ofstream& file) const
{
	if (resultMultipliers.empty())
	{
		for (size_t i = 0; i < resultPrimes.size(); ++i)
		{
			const auto& [idx, val1, val2] = resultPrimes[i];
			file << i + 1 << DELIM << idx << DELIM << val1 << DELIM << val2 << '\n';
		}

		// Последнее значение
		const ValueType& idx = maxDiff.second;
		const ValueType val1 = maxDiff.first - idx;
		const ValueType val2 = maxDiff.first + idx;
		file << "...\n" << count << DELIM << idx << DELIM << val1 << DELIM << val2 << "\n\n";
	}
	else
	{
		const auto printList = [&file](const forward_list<pair<ValueType, size_t>>& listIn)
		{
			auto lst = *listIn.begin();
			if (!listIn.empty())
			{
				file << listIn.front().first << '^' << listIn.front().second;
				for (auto it = ++listIn.begin(); it != listIn.end(); ++it)
				{
					file << " * " << (*it).first << '^' << (*it).second;
				}
			}
			file << DELIM;
		};
		for (size_t i = 0; i < resultPrimes.size(); ++i)
		{
			const auto& [idx, val1, val2] = resultPrimes[i];
			const auto& [list1, list2] = resultMultipliers[i];
			file << i << DELIM << idx << DELIM << val1 << DELIM;
			printList(list1);
			file << val2 << DELIM;
			printList(list2);
			file << '\n';
		}

		// Последнее значение
		const ValueType& idx = maxDiff.second;
		const ValueType val1 = maxDiff.first - idx;
		const ValueType val2 = maxDiff.first + idx;
		const forward_list<pair<ValueType, size_t>> list1 = getMultipliers(val1);
		const forward_list<pair<ValueType, size_t>> list2 = getMultipliers(val2);
		file << "...\n" << count << DELIM << idx << DELIM << val1 << DELIM;
		printList(list1);
		file << val2 << DELIM;
		printList(list2);
		file << "\n\n";
	}
}
