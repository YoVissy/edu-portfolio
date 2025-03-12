#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <fstream>

using namespace std;

int aa;
int bb = 1;
int bc = 1;
int bd = 1;

/*--------------------------------------------------
*
* nimi: Tulostus
* toiminta: Laivataulukon tulostus
* parametri(t):
* paluuarvo(t):
*
*--------------------------------------------------*/

void printOut(char table[7][7])
{
	bb = 1;
	cout << "Laivataulukko on: " << endl;
	cout << "    ";
	for (int aa = 65; aa < 72; aa++) {
		cout << (char)aa << "   ";
	}

	cout << endl;

	for (int i = 0; i < 7; i++)
	{
		cout << bb << " ";
		std::cout << "| ";
		for (int j = 0; j < 7; j++) {
			
			std::cout << table[i][j] << " | ";
		}
		std::endl(std::cout);
		bb = bb + 1;
	}

};

/*--------------------------------------------------
*
* nimi: Tulostus
* toiminta: Laivataulukon tulostus
* parametri(t):
* paluuarvo(t):
*
*--------------------------------------------------*/

void printOut1(char alusta[7][7])
{
	bc = 1;
	cout << "Pelitilanne on seuraava: " << endl;
	cout << "    ";
	for (int aa = 65; aa < 72; aa++) {
		cout << (char)aa << "   ";
	}

	cout << endl;

	for (int i = 0; i < 7; i++)
	{
		cout << bc << " ";
		std::cout << "| ";
		for (int j = 0; j < 7; j++)
		{
			std::cout << alusta[i][j] << " | ";
		}
		std::endl(std::cout);
		bc = bc + 1;
	}

};

/*--------------------------------------------------
*
* nimi: Tulostus
* toiminta: Laivataulukon tulostus
* parametri(t):
* paluuarvo(t):
*
*--------------------------------------------------*/

void printOut2(char tarkistus[7][7])
{
	bd = 1;
	cout << "Tarkistustaulukko: " << endl;
	cout << "    ";
	for (int aa = 65; aa < 72; aa++) {
		cout << (char)aa << "   ";
	}

	cout << endl;

	for (int i = 0; i < 7; i++)
	{
		cout << bd << " ";
		std::cout << "| ";
		for (int j = 0; j < 7; j++)
		{
			std::cout << tarkistus[i][j] << " | ";
		}
		std::endl(std::cout);
		bd = bd + 1;
	}
}

/*--------------------------------------------------
*
* nimi: Taulukoiden tarkistus
* toiminta: Kahden taulukon vertailu (ammunta ja laivojen paikat)
* parametri(t):
* paluuarvo(t):
*
*--------------------------------------------------*/

	void tsek(char alusta[7][7], char table[7][7]) {

		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 7; j++) {
				if (table[i][j] == 'B' && alusta[i][j] == 'x') {
					alusta[i][j] = '*';
				}
			}
		}

}


