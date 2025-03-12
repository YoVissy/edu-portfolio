#include "Header.h"

void main() 
{
	setlocale(LC_ALL, "fi-FI");

	char table[7][7];
	char alusta[7][7];
	char tarkistus[7][7];
	
	int x, y;
	char q;

	/*--------------------------------------------------
	*
	* nimi: Arvonta
	* toiminta: Sattumanvaraisuuksien arvonta
	* parametri(t): random1, random2, random3
	* paluuarvo(t): double-arvot välille 1-7 ja 1-5
	*
	*--------------------------------------------------*/

	random_device rand1;
	mt19937 gen(rand1());
	uniform_real_distribution<> dis(1, 7);
	int random1, random2, random3;

	random_device rand2;
	mt19937 gen2(rand2());
	uniform_real_distribution<> dis2(1, 5);



	for (int i = 0; i < 7; i++)	{
		for (int j = 0; j < 7; j++)
		{
			table[i][j] = '.';
		}
	}

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++)
		{
			alusta[i][j] = '.';
		}
	}

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			tarkistus[i][j] = '.';
		}
	}


	int z;

	tsek(alusta, table);


	restart:
		system("CLS");
	string option;

		cout << " Laivanupotus simulaattori" << "\n" << " ==========================" << endl;
		cout << " 1) Syötä laivat" << endl;
		cout << " 2) Pelaa" << endl;
		cout << " 3) Arvo laivojen paikat" << endl;
		cout << " 4) Tallenna tilanne" << endl;
		cout << " 5) Lataa tilanne" << endl;
		cout << " 6) Näytä pöytä" << endl;
		cout << " 7) Ammu sattumanvaraisia 5kpl" << endl;
		cout << " l / L) Lopeta" << endl;

		cin >> option;

		/*--------------------------------------------------
		*
		* nimi: Laivojen asetus
		* toiminta: Laivojen asetus taulukkoon table pelaajan parametrien mukaisesti
		* parametri(t):
		* paluuarvo(t):
		*
		*--------------------------------------------------*/

		if (option == "1") {
			
			z = 5;
			laiva:
			
			std::cout << "inser da boat" << std::endl;
			std::cout << "X : ";
				std::cin >> y;
			std::cout << "Y : ";
				std::cin >> x;
			std::cout << "P, I, E vai L?";
				std::cin >> q;

			
			
			x = x - 1;
			y = y - 1;


			for (int i = 5; i > 0; i--) {
				
				if (q == 'P' || q == 'p') {
					for (int i = 0; i < z; i++) {
						table[x][y] = 'B';
			//			table[x - i][y] = 'B';
						table[x + i][y] = 'B';
					}
				}
				else if (q == 'I' || q == 'i') {
					for (int i = 0; i < z; i++) {
						table[x][y] = 'B';
			//			table[x][y - i] = 'B';
						table[x][y + i] = 'B';
					}
				}
				if (q == 'E' || q == 'e') {
					for (int i = 0; i < z; i++) {
						table[x][y] = 'B';
			//			table[x - i][y] = 'B';
						table[x + i][y] = 'B';
					}
				}
				else if (q == 'L' || q == 'l') {
					for (int i = 0; i < z; i++) {
						table[x][y] = 'B';
			//			table[x][y - i] = 'B';
						table[x][y + i] = 'B';
					}
				}

			
				z = z - 1;
				if (z > 0) {
					goto laiva;
				} 
			}
		
			


			cout << endl;
			system("pause");
			system("CLS");
			goto restart;
		}

		/*--------------------------------------------------
		*
		* nimi: 5kpl ammuntaa
		* toiminta: Sattumanvaraisesti ammuntaa taulukkoon alusta
		* parametri(t):
		* paluuarvo(t):
		*
		*--------------------------------------------------*/

		if (option == "7") {

			random1 = static_cast<int>(dis(gen));
			random2 = static_cast<int>(dis(gen));

			
				for (int i = 0; i < 5; i++) {
					
					alusta[random1][random2] = 'x';
					random1 = static_cast<int>(dis(gen));
					random2 = static_cast<int>(dis(gen));

				}
			
			// Tykitys taulu
			for (int i = 0; i < 7; i++)
			{
				std::cout << "| ";
				for (int j = 0; j < 7; j++) {
					std::cout << alusta[i][j] << " | ";
				}
				std::endl(std::cout);
			}

			cout << endl;
			system("pause");
			system("CLS");
			goto restart;
		}

		/*--------------------------------------------------
		*
		* nimi: Sattumanvarainen laivojen asetus
		* toiminta: Laivojen asetus sattumanvaraisesti taulukkoon table
		* parametri(t):
		* paluuarvo(t):
		*
		*--------------------------------------------------*/

		if (option == "3") {

			int	abc = 3;
		
			for (int i = 0; i < 5; i++) {

				random1 = static_cast<int>(dis(gen));
				random2 = static_cast<int>(dis(gen));
				random3 = static_cast<int>(dis2(gen2));

				if (random3 = 1) {
					for (int i = 1; i < abc; i++) {
						table[random1][random2] = 'B';
						table[random1 + i][random2] = 'B';
					}
				}

				if (random3 = 2) {
					for (int i = 1; i < abc; i++) {
						table[random1][random2] = 'B';
						table[random1 - i][random2] = 'B';
					}
				}

				if (random3 = 3) {
					for (int i = 1; i < abc; i++) {
						table[random1][random2] = 'B';
						table[random1][random2 + i] = 'B';
					}
				}

				if (random3 = 4) {
					for (int i = 1; i < abc; i++) {
						table[random1][random2] = 'B';
						table[random1][random2 - i] = 'B';
					}
				}

				abc = abc - 1;
			}
			
			system("pause");
			system("CLS");
			goto restart;
		
		}


		/*--------------------------------------------------
		*
		* nimi: Tallennus ja lataus
		* toiminta: Pelitilanteen tallennus
		* parametri(t): Taulukot table, alusta & tarkistus
		* paluuarvo(t):
		*
		*--------------------------------------------------*/


		if (option == "4") {

			tsek(alusta, table);
			
			ofstream tal_laiva("tal-laiva.txt", ios::out);

			for (int i = 0; i < 7; i++) {
				for (int j = 0; j < 7; j++)
					tal_laiva << table[i][j] << " ";
				tal_laiva << endl;
			}

			ofstream tal_tulitus("tal-tulitus.txt", ios::out);

			for (int i = 0; i < 7; i++) {
				for (int j = 0; j < 7; j++)
					tal_tulitus << alusta[i][j] << " ";
				tal_tulitus << endl;
			}

			ofstream tal_upotus("tal-upotus.txt", ios::out);

			for (int i = 0; i < 7; i++) {
				for (int j = 0; j < 7; j++)
					tal_upotus << tarkistus[i][j] << " ";
				tal_upotus << endl;
			}


			system("CLS");
			goto restart;
		}



		if (option == "5") {
			
			ifstream tal_laiva;
			tal_laiva.open("tal-laiva.txt");
			
			for (int i = 0; i < 7; i++) {
				for (int j = 0; j < 7; j++) {
					tal_laiva >> table[i][j];
				}
			}

			ifstream tal_tulitus;
			tal_tulitus.open("tal-tulitus.txt");

			for (int i = 0; i < 7; i++) {
				for (int j = 0; j < 7; j++) {
					tal_tulitus >> alusta[i][j];
				}
			}

			ifstream tal_upotus;
			tal_upotus.open("tal-upotus.txt");

			for (int i = 0; i < 7; i++) {
				for (int j = 0; j < 7; j++) {
					tal_upotus >> tarkistus[i][j];
				}
			}
			
			system("pause");
			system("CLS");
			goto restart;
		}

		/*--------------------------------------------------
		*
		* nimi: Tulostus
		* toiminta: Kaikkien taulukkojen tulostus
		* parametri(t):
		* paluuarvo(t):
		*
		*--------------------------------------------------*/

		if (option == "6") {

			tsek(alusta, table);

			printOut(table);
				cout << endl;
			printOut1(alusta);
				cout << endl;

			system("pause");
			goto restart;
		}

		int kohde_x, kohde_y;

		/*--------------------------------------------------
		*
		* nimi: Pelaajan ammunta haluttuun pisteeseen
		* toiminta: kohde_x ja kohde_y määräävät taulukkoon table johon ammutaan.
		* parametri(t):
		* paluuarvo(t):
		*
		*--------------------------------------------------*/

		if (option == "2") {
					
			cout << "Mihin ammut? " << endl;
 				cin >> kohde_x >> kohde_y;

				if (kohde_x == 8 || kohde_y == 8) {
					printOut(table);
						cout << endl;
				}

				alusta[kohde_x - 1][kohde_y - 1] = 'x';

				tsek(alusta, table);

				printOut1(alusta);
					cout << endl;

			system("pause");
			system("CLS");
			goto restart;
		}

		if (option == "l" || option == "L") {

		}


}