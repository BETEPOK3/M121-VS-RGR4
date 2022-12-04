#include <iostream>
#include <fstream>
#include "PrimeChecker.h"

using namespace std;

int main()
{
	ios_base::sync_with_stdio(false);
	setlocale(0, "");

	PrimeCheckerData data;
	cout << "Введите данные: N DD MM GGGG (Пример: 12 24 4 2000)\n";
	cin >> data.N >> data.DD >> data.MM >> data.GGGG;

	cout << "\nЧтение файла primes.dat...\n";
	PrimeChecker primeChecker("primes.dat", data);

	unsigned long long pc2 = primeChecker.getPc2();
	unsigned long long pc3 = primeChecker.getPc3();
	cout << "Pc2 = " << pc2 << "\nPc3 = " << pc3 << "\n\n";

	ofstream fout("output.csv", ios::out);
	cout << "1\n";
	primeChecker.runPrimeCheck2(pc2);
	primeChecker.printResult();
	primeChecker.printResultToCsv(fout);
	cout << "2\n";
	primeChecker.runPrimeCheck3(pc2);
	primeChecker.printResult();
	primeChecker.printResultToCsv(fout);
	cout << "3\n";
	primeChecker.runPrimeCheck2(pc3);
	primeChecker.printResult();
	primeChecker.printResultToCsv(fout);
	cout << "4\n";
	primeChecker.runPrimeCheck3(pc3);
	primeChecker.printResult();
	primeChecker.printResultToCsv(fout);
	fout.close();

	system("pause");
}