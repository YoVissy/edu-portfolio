/* TAAJUUSLASKURI & KANTTIGENERAATTORI   C_taaj_2_2.c   KP  */

/*   kutistettu versio 4 :  */

// V4: Muutettu nayta-funktion toiminta: param. ekan merkin positio, tyhj�t��n alkunollat
//	Volatilet poistettu alkutilavarauksista. N�ytt�pusk. alustus muutettu.
// 	LCD_ohjaus -funktio yksinkertaistettu

#include <avr/io.h>		/* i/o-symbolit: C:\WinAVR-20090313\avr\include\avr\iom128.h  */
#include <avr/interrupt.h>	/* Keskeytysmakrot */
#include <avr/pgmspace.h>	/* FLASH-ohjelmamuistin k�sittely */

uint8_t 
	Laskuri_1s = 1,	// P��ohjelmasilmukan 1s aikalaskuri
	Laskuri_02s = 4,	// P��ohjelmasilmukan 0,2s aikalaskuri
	KeskLippu = 0,	// Jos =1, TIMER1_COMPA-keskeytys tapahtunut (10ms v�lein)
	B1lask,B2lask,	// Kytkinv�r�htelyn eliminointiin k�ytett�v�t laskurit kytkimille B1 ja B2
	Kytk,			// Kytkinportin sis�lt� lukuhetkell�
	db[32],		// N�ytt�puskuri
	dindex,		// N�ytt�puskurin indeksi tulostustilanteessa
	rivinsiirto,		// 1, kun lcd:lle on kirjoitettu siirtokomento alariville
	tulostus_kesken,	// 1, kun 32 merkin tulostus lcd:lle on meneill��n
	PB6_tila = 0;	// Kanttigeneraattorin l�ht�signaalin tila (PINB&0x40) PB6-nastassa, luetaan compb-kesk.palv.alussa
uint16_t 
	KesHetki,		// P��ohjelmasilmukan ajastus: TIMER1_COMPA-keskeytyshetki		
	OVFlaskuri = 0,	// Timer1:n ylivuotokappalelaskuri
	puls_lkm=0,		// Mitattavan kanttiaallon nousureunojen lukum��r�
	kaappausrek,	// Mitattavan kanttiaallon viimeisimm�n nousureunan ajanhetki (ICR1)
	oc1b_hetki,		// Tuotettavan kantin reunan ajanhetki
	tilan_0_kesto,	// Tuotettavan kantin 0- ja 1-tilojen kestoajat
	tilan_1_kesto;
int16_t
	nyt_aika,		// apumuuttuja, Timer1:st� luettu aika
	lcd_aika;		// Seuraavan merkin kirjoitusajankohta LCD:lle (n. 100us v�lein)
uint32_t 		//Vertaa k��nn�stuloksia, jos n�m� olisivat long eli int32_t
	ed_nousuhetki, 	// Mitattavan kantin nousureunan hetki
	nousuhetki, 		// Mitattavan kantin nousureunan hetki
	jakso;  		// Mitattavan kantin jaksonaika
uint8_t
	naytolle,		// =1, kun taajuuden laskentaan tarvittavat uudet tiedot ovat valmiina
	f_ovf;			// =1, kun mitattava taajuus on liian suuri, CAPT-kes.palvelu ei ehdi suoriutua
uint16_t
	jaksolkm,		// Edellisen noin 1 s:n aikana tulleiden nousureunojen lkm, joista taajuus lasketaan
	n1s_puls_lkm;	// Parhaillaan kuluvan sekunnin aikana tulleiden nousureunojen lukum��r�

uint32_t
	laskentajakso,	// Aika, jonka kuluessa on tullut "n1s_puls_lkm" kpl.nousureunoja
	n1s,			// sama kuin ed.
	n1s_alkuhetki;	// Ed. aikap�tk�n alkuhetken kellonaika
float
	ftaajuus;		// = nousureunojen lukum. jaettuna siihen kuluneella ajalla

const uint8_t alkuteksti[] PROGMEM =	// Merkkijono ohjelmamuistissa
"0.00000 Hz      "
"      0 us      ";

const int32_t dek[]={1,10,100,1000,10000,100000,1000000,10000000,100000000}; // Lukujono miss�???
const float fdek[]={1,10,100,1000,1e4,1e5,1e6};

void nayta (long, uint8_t, uint8_t);
void kopioi_mjono (volatile uint8_t *, const uint8_t *);
void epulssi (void);
void lcd_ohjaus (uint8_t, uint8_t);
void viive_ms (volatile uint8_t);
void lcd_init (void);
void fnayta (float, uint8_t, uint8_t);
void LCD_lle_puskurista_merkki (void);
void painiketoiminnot(void);

/**************************************************************************************************/
ISR (TIMER1_OVF_vect)		// Timer1-ylivuotokeskeytyspalvelu (n. 3 us)
{
	OVFlaskuri++;			// Ajastimen 16-bittisen rekisterin ylivuotojen kappalelaskurin kasvatus
}

/*******************************************************************************************/
ISR (TIMER1_COMPA_vect)		// Timer1-A-vertailukeskeytyspalvelu (n. 5 us), suoritetaan 10 ms v�lein
{						// k�ytet��n p��ohjelmasilmukan ajastamiseen
	KesHetki = KesHetki + 20000;	// Uuden compa-kesk. viritys 10 ms p��h�n. Timer askeltaa 0,5us v�lein, 20000*0,5us = 10 ms
	OCR1A = KesHetki;
	KeskLippu = 1;			// P��ohjelman ajastuslipun asetus
}

/*******************************************************************************************/
ISR (TIMER1_COMPB_vect)		// Timer1-B-vertailukeskeytyspalvelu (n.10 us), suoritetaan aina,
{						// kun oc1b-l�ht� on k��ntynyt eli on saatu tuotettavan kanttiaallon reuna.

	PB6_tila = PINB & 0x40;			// Luetaan t�m�nhetkinen oc1b/PB6-bitin tila
	if (PB6_tila == 0){				// jos l�ht�bitti on nyt 0, ohjelmoidaan nousureuna
		oc1b_hetki += tilan_0_kesto;	// Seuraavan reunan hetki= edellisen reunan hetki + tilan_0_kesto
		TCCR1A |=  0x30;			// viritet��n nousureuna
		OCR1B = oc1b_hetki;
	}
	else {						// jos l�ht�bitti on nyt 1, ohjelmoidaan laskureuna
		oc1b_hetki += tilan_1_kesto;
		TCCR1A &= ~0x10;		// nyt viritet��n laskur.
		OCR1B = oc1b_hetki;
	}
}

/**************************************************************************************************/
ISR (TIMER1_CAPT_vect)		// Timer1-kaappauskeskeytyspalvelu  (n. 17 us)
{						// T�h�n tullaan aina, kun ICP1/PD4-nastaan tulee mitattavan signaalin nousureuna
	puls_lkm++;				// Nousureunojen lukum��r�laskurin kasvatus
	kaappausrek = ICR1;		// Luetaan nousureunan tapahtumisen ajanhetki kaappausrekisterist�
	ed_nousuhetki = nousuhetki;	
	// nousuhetki = ((unsigned long)OVFlaskuri<<16)+(unsigned long)kaappausrek; // Koostetaan 32-bittinen "kellonaika"

	*((uint16_t *) &nousuhetki + 1) = OVFlaskuri;			// Sama tehokkaammin
//	*((uint16_t *) &nousuhetki + 0) = kaappausrek; 		//warning: dereferencing type-punned pointer will break strict-aliasing rules
	*((uint8_t *) &nousuhetki + 1) = *((uint8_t *) &kaappausrek+1);
	*((uint8_t *) &nousuhetki + 0) = *((uint8_t *) &kaappausrek+0);


	if ((TIFR&0x04) && (((*((uint8_t *) &kaappausrek +1)) & 0x80)==0)) 	// Jos Timer1-ylivuoto oli palvelematta kaappaushetkell�,
	{
		(*((uint16_t *) &nousuhetki + 1))++;		// korjataan "kellonaika" lis��m�ll� ykk�nen 4-tavuisen muuttujan ylemp��n wordiin.
	}
 
	jakso = nousuhetki - ed_nousuhetki;		// Kahden per�kk�isen nousureunan aikaero = jaksonaika

	n1s_puls_lkm++;					// Nousureunojen lkm n. 1s laskenta-aikana
	n1s = nousuhetki - n1s_alkuhetki;		// Tarkka kestoaika ed. mainitun pulssim��r�n tuloon
	if (n1s>2000000)					// Jos 1 s laskenta-aika on ylittynyt
	{
		laskentajakso=n1s;				// Kopioidaan tiedot p��ohjelmaa varten
		jaksolkm=n1s_puls_lkm;			//
		n1s_puls_lkm=0;				//	ja alustetaan uutta sarjaa varten
		n1s_alkuhetki=nousuhetki;
		naytolle=1;					// P��ohjelmalle tieto, ett� taajuus voidaan laskea
	}
	if (TIFR&0x20)			//ICF1		Jos uusi reuna on jo nyt tullut, vaikka t�st�k��n keskeytyksest�
	{	TIMSK &= ~0x20;		//TICIE1		ei olla ehditty ulos, kiellet��n CAPT-kesk. ja
		f_ovf=1;					//  	laitetaan "taajuusylivuoto"-lippu merkiksi
	}
}
/****************************************************************************************/
void painiketoiminnot(void)
{
		Kytk = PINA;							
												// B1-painikkeen k�sittely (PINA.0)
		if (((Kytk & 0b0001) == 0b0001) && (B1lask > 0))  B1lask--;		//Jos kytk. ei en�� paineta mutta laskuri ei viel� 0, v�henn� laskuria
		else if (((Kytk & 0b0001) == 0) && (B1lask > 0)) B1lask=9;		//Jos kytk. painettuna kun laskuri ei viel� ole 0, laskuriin alkuarvo 9
		else if (((Kytk & 0b0001) == 0) && (B1lask == 0))			//Jos kytk. painetaan laskurin ollessa 0, alkaa toiminta
		{
			B1lask = 9;								//V�r�htelynpoistolaskurin viritys
			// T�H�N TEHT�V�T, JOTKA TEHD��N, KUN ON PAINETTU B1-PAINIKETTA
		}

		if (((Kytk & 0b0010) == 0b0010) && (B2lask > 0))  B2lask--;		//Jos kytk. ei en�� paineta mutta laskuri ei viel� 0, v�henn� laskuria
		else if (((Kytk & 0b0010) == 0) && (B2lask > 0)) B2lask=9; 		//Jos kytk. painettuna kun laskuri ei viel� ole 0, laskuriin alkuarvo 9
		else if (((Kytk & 0b0010) == 0) && (B2lask == 0))			//Jos kytk. painetaan laskurin ollessa 0, alkaa toiminta
		{
			B2lask = 9;								//V�r�htelynpoistolaskurin viritys
			/* T�H�N TEHT�V�T, JOTKA TEHD��N, KUN ON PAINETTU B2-PAINIKETTA  */
		}

		/* T�H�N SEURAAVAN PAINIKKEEN K�SITTELY VASTAAVASTI KUIN YLL� OLEVA  */

		/* T�H�N SIT� SEURAAVAN PAINIKKEEN K�SITTELY VASTAAVASTI KUIN YLL� OLEVA  */

}
/****************************************************************************************/
/* Kokonaislukumuuttujan muunto ascii-merkeiksi n�ytt�puskuriin  				*/

void nayta (long arvo, uint8_t paikka, uint8_t dig_lkm) 
{
uint8_t ekanollastapoikkeava = 0;

	uint8_t digi;
	dig_lkm--;
	while (dig_lkm > 0) {
		digi = arvo / dek[dig_lkm];

		if (digi != 0) ekanollastapoikkeava = 1;
		if (ekanollastapoikkeava != 0)
			db[paikka ++ ] = digi+'0';
		else 
			db[paikka ++ ] = ' ';
		arvo = arvo - digi * dek[dig_lkm];
		dig_lkm--;
	}
	db[paikka] = arvo + '0';
}

/*********************************************************************************/

int main (void)
{
  DDRA = 0x40;			/* PA6 out = releohjaus, PA0...4 in kytkimet B1...5 */
  DDRB = 0xFF;			/* kaikki output, PB5=oc1a, PB6=oc1b */
  DDRC = 0xFF;			/* LCD: D7 D6 D5 D4 nc E RW RS */
  dindex = 0xFF;			/* N�ytt�puskurin tulostusindeksi = -1 */
  tulostus_kesken = 0;		/* N�yt�lle tulostusta ei ole aloitettu */
  rivinsiirto = 0;			/* N�yt�n tulostuksessa ei ole annettu rivinsiirtokomentoa */
  kopioi_mjono(db,alkuteksti);	/* Kopioidaan n�ytt�puskuriin alkuarvot */
  lcd_init();				/* LCD:lle alustuskomentosekvenssi */

  KesHetki = 20000;		/* Ekan OCR1A-keskeytyksen ajanhetki 10 ms resetist�*/
  OCR1A = KesHetki;

  oc1b_hetki = 30000;		/* Ekan OCR1B-keskeytyksen ajanhetki 15 ms resetist�*/
  OCR1B = oc1b_hetki;
  
  tilan_1_kesto = 100; 		/* Kanttigeneraattorin 1-tilan kesto 50us */
  tilan_0_kesto = 240; 		/* Kanttigeneraattorin 0-tilan kesto 120us */

  TCCR1A = 0b01110000;		/* 0x70; 01 11 00 00 toggle oc1a, oc1b-nousu */
  TCCR1B = 0b01000010;  	/* TCCR1B=(1<<ICES1)+(1<<CS11); nousureunakaappaus,nonPWM,clk/8  */
  TIMSK = 0b00111100;		/* TIMSK = 0x1C; TIMSK = (1<<TICIE1)+(1<<OCIE1A)+(1<<OCIE1B)+(1<<TOIE1);*/
  sei();

  while (1)				// P��OHJELMASILMUKKA
  {
	if (KeskLippu == 1)	{	// Ehto toteutuu 10 ms v�lein, timer1_compa-keskeytys asettaa
		KeskLippu = 0;
		painiketoiminnot();	// Kytkintilojen luenta ja kunkin painikkeen tilan mukainen toiminto

		Laskuri_02s--;					// 0,2 sekunnin v�lein tehd��n seuraavia asioita:
		if (Laskuri_02s == 0) {
			Laskuri_02s = 20;
			if (tulostus_kesken == 0) {		// Jos n�ytt�puskurin tulostus ei ole meneill��n ja
				if ((jakso>>1)<10000000) {		// jos mitattu jaksonaika on pienempi kuin 10 s, niin
					nayta(jakso>>1,16,7);		// muunnetaan jaksonaika n�ytt�puskuriin ASCII-merkkijonoksi
					db[24]='u';			// ja laitetaan per��n yksik�ksi mikrosekunti
					db[25]='s';
				}
				else {
					nayta((jakso>>1)/1000,16,7);	// Jaksonajan n�ytt� LCD:ll� yksikk�n� ms, jos
					db[24]='m';				// jaksonaika onkin suurempi kuin 10 s.
					db[25]='s';
				}
				tulostus_kesken = 1;		// N�ytt�puskurin tulostuksen k�ynnistys 0,2 s v�lein	
				cli();
				lcd_aika = TCNT1 + 200;		// Ensimm�isen merkin kirjoitus LCD:lle n.100 us kuluttua
				sei();
			}
		}
		else {							// Millaisin aikav�lein tehd��n seuraavat asiat?
			if (naytolle==1) {				// Jos kaappauskeskeytys on laskenut tarvittavat alkutiedot
				naytolle =0;
				if(f_ovf==0) {						// ja jos ei ole ylivuototilanne, niin
				    ftaajuus=jaksolkm/(laskentajakso * 0.5e-6);	// lasketaan taajuus. (268,63 us)
				    fnayta(ftaajuus,0,6);				// N�yt�lle taajuus, 6 merk. digitti�. (2063,8 us)
				}
				else {
				    db[0]='-';db[1]='-';db[2]='-';			// Jos mitattava taajuus on liian suuri,
				    db[3]='-';db[4]='-';db[5]='-';db[6]='-';		// n�ytt��n ------------
				}
			}
		}
		Laskuri_1s--;					// Yhden sekunnin v�lein tehd��n seuraavat asiat:
		if (Laskuri_1s == 0) {
			Laskuri_1s = 100;
//			PORTA = PORTA ^ 0x40;		// Releen tilan vaihto 1 s v�lein
			f_ovf=0;					// Nollataan mitatun taajuuden mahdollinen ylivuototilanne
  			TIMSK |= 0b00100000;		//1<<TICIE1;  //    ja sallitaan taas kaappauskeskeytykset 
		}
	}
	else {  // vaikka 10ms keskeytyslippu ei olekaan pystyss�, kirjoitetaan n�yt�lle vuorossa oleva merkki, jos
		if (tulostus_kesken == 1) {			// Jos n�ytt�puskurin tulostus on kesken
	 		cli();					// Miksi t�ss� on keskeytyskielto?
			nyt_aika = TCNT1;			// Luetaan Timer1:st� t�m�nhetkinen aika
			sei();
			if (nyt_aika-lcd_aika > 0){			// Jos timeri on ohittanut n�yt�lle tulostuksessa tarvittavan 100us 
				LCD_lle_puskurista_merkki ();		// merkkiv�liajan, kirjoitetaan n�ytt�puskurista vuorossa oleva merkki LCD:lle
			}	// if (nyt_aika-lcd_aika>0)
		}		// if (tulostus_kesken==1)
	}		// else
  }			// while(1)
}		 	// main

/****************************************************************************************/
/* Reaalilukumuuttujan muunto asciimerkeiksi n�ytt�puskuriin tyyliin 357.629Hz tai .076928Hz	*/

void fnayta (float arvo, uint8_t paikka, uint8_t dig_lkm)
{
	uint8_t eka=0, i=0, dig_lask=0, apu;
	float fapu;
	fapu = arvo/fdek[dig_lkm];
	while (i<6) {
		fapu = fapu * 10.;
		apu = fapu;
		fapu = fapu - apu;
		if ((apu !=0)||(eka==1)) {
			db[paikka] = apu + '0';
			paikka++;
			dig_lask++;
			eka=1;
		}
		i++;
	}
	db[paikka] = '.';			// Desimaalipiste
	paikka++;
	while (dig_lask<dig_lkm) {
		fapu = fapu * 10.;
		apu = fapu;
		fapu = fapu - apu;
		db[paikka] = apu + '0';
		paikka++;
		dig_lask++;
	}
}

/***************************************************************/
/* LCD-n�yt�lle E-signaalin pulssi                             */
void epulssi (void) __attribute__ ((noinline));
void epulssi (void)
{	volatile char i = 2;
	PORTC |= 0x04;			// E-signaali yl�s
	while (i>0) i--;			// Noin mikrosekunnin viive
	PORTC &= ~0x04;			// E-signaali alas
}
/***************************************************************************/
/* Merkin tai ohjauskomennon kirjoitus LCD:lle					*/

void lcd_ohjaus (uint8_t tyyppi, uint8_t data)	// LCD: D7 D6 D5 D4 nc E RW RS
{
	PORTC = (data & 0xF0) | (PORTC & 0x0E)| tyyppi;		// merkin 4 MSB:t� LCD:lle
	epulssi();
	PORTC = (data<<4) | (PORTC & 0x0F);			// merkin 4 LSB:t� LCD:lle
	epulssi();  
}
/****************************************************************************/
/* Parametrin mukainen viive millisekunteja  @ 16 MHz					*/
void viive_ms (volatile uint8_t aika) __attribute__ ((noinline));
void viive_ms (volatile uint8_t aika)
{
	volatile uint16_t i;
	while (aika>0) {
		for (i=0;i<900;i++)
		{
		}
		aika--;
	}
}
/*******************************************************************************/
/* Nestekiden�ytt�modulin vaatima alustus s�hk�n kytkemisen j�lkeen       */

void lcd_init(void)
{
  PORTC = 0x00;
  viive_ms(40);					// min 15 ms after Vcc rises to 4.5V
  PORTC = 0x30; epulssi(); viive_ms(6); 	//Function set, minimiviive 4100 us
  PORTC = 0x30; epulssi(); viive_ms(6); 	//Function set, minimiviive 100 us
  PORTC = 0x30; epulssi(); viive_ms(2); 	//Function set, minimiviive 42 us
  PORTC = 0x20; epulssi(); viive_ms(2); 	//Function set, v�yl� 4-bittiseksi, minimiviive 42 us

  lcd_ohjaus(0,0x28); viive_ms(2);	//Function set,  4bit, 2rivinen, minimiviive 42 us
  lcd_ohjaus(0,0x08); viive_ms(2);	//display off,curs off, blink off, minimiviive 42 us
  lcd_ohjaus(0,0x01); viive_ms(3);	//clear display, minimiviive 1640 us
  lcd_ohjaus(0,0x06); viive_ms(2);	//entry mode set	
  lcd_ohjaus(0,0x0C); viive_ms(2);	//display on,curs off,blink off
  lcd_ohjaus(0,0x80); viive_ms(1);	//DD-address=0
}

/**********************************************************************************************/
/* N�ytt�puskurista vuorossa olevan yhden ASCII-merkin tai kursorinsiirtokomennon kirjoitus LCD-moduliin  */

void LCD_lle_puskurista_merkki (void)
{
	dindex++;						// Kasvata n�ytt�puskurin indeksi�
	switch (dindex)
	{
	case 16:						// Jos yl�rivi on kirjoitetu t�yteen
		if (rivinsiirto == 0) {			// ja jos rivinsiirtokomentoa ei viel� ole annettu
			rivinsiirto = 1;
			dindex--;				//   peruutetaan indeksi� takaisin
			lcd_ohjaus (0, 0xC0);		//   kursorin siirtokomento alarivin alkuun
		}
		else {						// Kun rivinsiirtokomento on kirjoitettu
			rivinsiirto = 0;
			lcd_ohjaus (1, db[dindex]);	// kirjoitetaan alarivin ensimm�inen merkki
		}
		break;
	case 32:						// Koko puskurillinen on kirjoitettu
		tulostus_kesken = 0;			// Merkkijonon tulostus valmistui
		dindex = 0xFF;				// Puskurin indeksin palautus alkuun, arvoksi -1
		lcd_ohjaus (0, 0x80);			// LCD:lle kursorinsiirtokomento yl�rivin alkuun
		return;					// Pois t��lt� laskematta seuraavaa merkkiv�liaikaa
		break;
	default:
		lcd_ohjaus (1, db[dindex]);		// Vuorossa olevan ASCII-merkin kirjoitus LCD:lle
		break;
	}
	cli();
	nyt_aika = TCNT1;		
	sei();
	lcd_aika = nyt_aika + 200; 	// Seuraavan merkin tai komennon kirjoitusajankohdan laskenta 100 us p��h�n
}
/*******************************************************************/
/* Merkkijonon kopiointi 16-bittisest� FLASH-ohjelmamuistista 8-bittiseen SRAMmiin                 */

void kopioi_mjono (volatile uint8_t *kohde, const uint8_t *lahde)
{
	uint8_t i = 0;
	while (pgm_read_byte (&(lahde[i])) != '\0') {		// Kunnes ohjelmamuistista l�ytyy NUL-merkki
		kohde[i] = pgm_read_byte (&(lahde[i]));	// luetaan indeksin osoittama merkki FLASHiss� olevasta taulukosta kohdetaulukkoon
		i++;							// Kasvatetaan indeksi�.
	}
}
