#include <fstream>
#include <numeric>
#include <iostream>
#include <thread>
#include "PrimeChecker.h"

using namespace std;

PrimeChecker::PrimeChecker(const std::string filename, const PrimeCheckerData& data, const unsigned long& desCount) : desiredCount { desCount }
{
	// ������ �� ����� ���� �������� 2-������� �����
	ifstream fin(filename, ios::binary | ios::in);
	primes.reserve(PRIMES_NUM_COUNT);
	for (size_t i = 0; i < PRIMES_NUM_COUNT; ++i)
	{
		unsigned long num;
		fin.read(reinterpret_cast<char*>(&num), sizeof(num));
		primes.push_back(num);
	}
	fin.close();

	// ����������� pc2 � pc3 ����������
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
		pc3 = static_cast<ValueType>(pow(3, 9 + tempNum));
	}
	else
	{
		pc3 = static_cast<ValueType>(pow(3, 15 - tempNum));
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

forward_list<PrimeChecker::ValueType> PrimeChecker::getMultipliers(const ValueType& num) const
{
	forward_list<ValueType> result;
	ValueType leftOver = num;
	for (const auto& prime : primes)
	{
		ValueType mult = prime;
		if (mult * mult > num)
		{
			break;
		}
		while (mult <= num && num % mult == 0)
		{
			result.push_front(mult);
			mult *= prime;
			leftOver /= prime;
		}
	}
	if (leftOver != 1)
	{
		result.push_front(leftOver);
	}
	return result;
}

void PrimeChecker::runPrimeCheck2(const ValueType& pc)
{
	count = 0;
	resultPrimes.clear();
	resultMultipliers.clear();
	resultPrimes.reserve(desiredCount);

	// ������ ������ desiredCount (�� ��������� 15) ��� ������� �����
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

	// ������ ��� ������ ������� ���-�� ����� �����
	const auto func = [&](const ValueType idxStart, const ValueType& idxEnd)
	{
		ValueType myCount = 0;
		for (ValueType i = idxStart; i < idxEnd; ++i)
		{
			ValueType val1 = pc - i;
			ValueType val2 = pc + i;
			if (isPrime(val1) && isPrime(val2))
			{
				++myCount;
			}
		}
		mutexCount.lock();
		count += myCount;
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
	resultPrimes.clear();
	resultMultipliers.clear();
	resultPrimes.reserve(desiredCount);
	resultMultipliers.reserve(desiredCount);

	// ������ ������ desiredCount (�� ��������� 15) ����� �� 3-�� ������� ������� �����
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

	// ������ ��� ������ ������� ���-�� ����� ����� �����
	const auto func = [&](const ValueType idxStart, const ValueType& idxEnd)
	{
		ValueType myCount = 0;
		for (ValueType i = idxStart; i < idxEnd; ++i)
		{
			ValueType val1 = pc - i;
			ValueType val2 = pc + i;
			if (gcd(pc, val1) == 1 && gcd(pc, val2) == 1 && gcd(val1, val2) == 1)
			{
				++myCount;
			}
		}
		mutexCount.lock();
		count += myCount;
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
	}
	else
	{
		for (size_t i = 0; i < resultPrimes.size(); ++i)
		{
			const auto& [idx, val1, val2] = resultPrimes[i];
			const auto& [lst1, lst2] = resultMultipliers[i];
			cout << idx << ": " << val1 << ' ' << val2 << '\n';
			const auto printList = [](const forward_list<ValueType>& listIn)
			{
				cout << '[';
				if (!listIn.empty())
				{
					cout << *listIn.begin();
					for (auto it = ++listIn.begin(); it != listIn.end(); ++it)
					{
						cout << ", " << *it;
					}
				}
				cout << "]\n";
			};
			printList(lst1);
			printList(lst2);
		}
	}
	cout << count << "\n\n";
}
