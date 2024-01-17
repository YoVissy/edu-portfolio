#include <iostream>
#include <math.h>
#include <random>
#include <string>

using namespace std;

void main() {

	setlocale(LC_ALL, "fi-FI");

	int option, rivit;

	const unsigned int koko = 7;
	int taulukko[10][koko];

	// Randomin teko
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(1, 40);

	for (int i = 0; i < koko; i++) {
		taulukko[0][i] = dis(gen);
	}

	// Randomin teko Jokerille
	random_device rd1;
	mt19937 gen1(rd1());
	uniform_real_distribution<> dis1(1, 9);

	cout << " Lottopelisimulaattori" << "\n" << " ==========================" << endl;
		cout << " 1) Oikean lottorivin arvonta" << endl;
		cout << " 2) Yhden rivin lotto" << endl;
		cout << " 3) Yhden rivin lotto arvotulla tuloksella" << endl;
		cout << " 4) Useamman rivin lotto arvotulla tuloksella" << endl;
		cout << " 5) Yhden rivin jokeri" << endl;
		cout << " l / L) Lopeta" << endl;

		cin >> option;



	

	// 1) Oikean lottorivin arvonta
	if (option == 1) {
/*		for (int i = 0; i < koko; i++) {

			taulukko[0][i] = dis(gen);

		}*/

		cout << taulukko[0][0] << taulukko[0][1] << taulukko[0][2] << taulukko[0][3] << taulukko[0][4] << taulukko[0][5] << taulukko[0][6] << endl;
	}

	// 2) Yhden rivin lotto
	if (option == 2) {
		for (int i = 0; i < koko; i++) {

			cout << "Anna 7 kokonaisnumeroa lottoa varten: ";
			cin >> taulukko[1][i];

		}

		cout << taulukko[1][0] << taulukko[1][1] << taulukko[1][2] << taulukko[1][3] << taulukko[1][4] << taulukko[1][5] << taulukko[1][6] << endl;
		cout << "Tässä on oikea rivi: " << taulukko[0][0] << taulukko[0][1] << taulukko[0][2] << taulukko[0][3] << taulukko[0][4] << taulukko[0][5] << taulukko[0][6] << endl;
	}

	// 3) Yhden rivin lotto arvotulla tuloksella
	if (option == 3) {
		for (int i = 0; i < koko; i++) {

			taulukko[2][i] = dis(gen);

		}

		cout << taulukko[2][0] << taulukko[2][1] << taulukko[2][2] << taulukko[2][3] << taulukko[2][4] << taulukko[2][5] << taulukko[2][6] << endl;
	}

	// 4) Useamman rivin lotto arvotulla tuloksella
	if (option == 4) {
		cout << "Kuinka monen rivin loton haluat arvotulla tuloksella? " << endl;
		cin >> rivit;

		for (int i = 0; i < koko; i++) {
			for (int k = 0; k < rivit; k++) {
				taulukko[k + 3][i] = dis(gen);
			}
		}

		for (int i = 0; i < rivit; i++) {
			cout << taulukko[i + 3][0] << taulukko[i + 3][1] << taulukko[i + 3][2] << taulukko[i + 3][3] << taulukko[i + 3][4] << taulukko[i + 3][5] << taulukko[i + 3][6] << endl;
		}
	}

	// 5) Yhden rivin jokeri
	if (option == 5) {
		for (int i = 0; i < koko; i++) {

			cout << "Anna Jokeri-rivi (0-9): ";
			cin >> taulukko[4][i];

		}

		for (int i = 0; i < koko; i++) {
			taulukko[5][i] = dis1(gen1);
		}

		cout << taulukko[4][0] << taulukko[4][1] << taulukko[4][2] << taulukko[4][3] << taulukko[4][4] << taulukko[4][5] << taulukko[4][6] << endl;
		cout << "Tässä on oikea Jokeri-rivi: " << taulukko[5][0] << taulukko[5][1] << taulukko[5][2] << taulukko[5][3] << taulukko[5][4] << taulukko[5][5] << taulukko[5][6] << endl;
	}

	if (option == 6 || option == 7) {

	}

	else {
	}

		system("pause");

}