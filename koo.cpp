#include <iostream>
#include <math.h>
#include <random>
#include "h.h"

using namespace	std;

void koo() {
	
	int luku;//prime = 0;
	bool prime;
		cout << "Anna mielivaltainen täysluku: "; cin >> luku;

		for (int i = 2; i < luku; i++) {
			if (luku % i == 0) {
				prime = true;
			}
		}

		

		if (luku==1 || luku == 0) {
			cout << "Luku ei ole alkuluku. " << endl;
		}
		
		else if (prime = true) {
			cout << "Luku on alkuluku. " << endl;
		}

		else {
			cout << "Luku ei ole alkuluku. " << endl;
		}
}