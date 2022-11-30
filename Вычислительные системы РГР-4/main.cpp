#include <iostream>
#include "PrimeChecker.h"

using namespace std;

int main()
{
	ios_base::sync_with_stdio(false);

	const PrimeCheckerData data = { 12, 24, 4, 2000 };
	cout << "Reading file...\n";
	PrimeChecker primeChecker("primes.dat", data);

	unsigned long long pc2 = primeChecker.getPc2();
	unsigned long long pc3 = primeChecker.getPc3();
	cout << "Pc2 = " << pc2 << "\nPc3 = " << pc3 << "\n\n";

	cout << "1\n";
	primeChecker.runPrimeCheck2(pc2);
	primeChecker.printResult();
	cout << "2\n";
	primeChecker.runPrimeCheck3(pc2);
	primeChecker.printResult();
	cout << "3\n";
	primeChecker.runPrimeCheck2(pc3);
	primeChecker.printResult();
	cout << "4\n";
	primeChecker.runPrimeCheck3(pc3);
	primeChecker.printResult();

	system("pause");
}