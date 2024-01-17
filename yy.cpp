#include <iostream>
#include <math.h>
#include <random>
#include <string>
#include "h.h"

using namespace	std;

void main() {

	setlocale(LC_ALL, "fi-FI");

//	yy();
//	kaa();
	koo();

		system("pause");


}

void yy() {

	int day, year;

	cout << "Anna vuosi ja kuukausi: " << endl;
	cin >> year;

	if (year % 400 == 0) {
		day = 29;
	}

	else if (year % 100 == 0) {
		day = 28;
	}

	else if (year % 4 == 0) {
		day = 29;
	}

	else {
		day = 28;
	}

	cout << day << endl;
}