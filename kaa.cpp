#include <iostream>
#include <math.h>
#include <random>
#include <ctime>
#include "h.h"

using namespace	std;

void kaa() {

	int arvo, temp, kruuna = 0, klaava = 0, kylki = 0, heitot;
	char arvaus;
	srand(time(NULL));

	cout << "Kuinka monta heittoa heitetään? "; cin >> heitot;

	// Randomin teko
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(1, 3);



	// Tulosta randomit
	for (int count = 0; count < heitot; ++count)
	{
		arvo = dis(gen);


		// Lasketaan randomin tulokset

		if (arvo == 1) {
			kruuna++;
		}

		else if (arvo == 2) {
			klaava++;
		}

		else {
			kylki++;
		}

	}

	cout << "Kruuna: " << kruuna << " ja Klaava: " << klaava << " ja epätodennäköisin, kyljellään: " << kylki << endl;
}