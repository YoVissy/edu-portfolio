/* TAAJUUSLASKURI & KANTTIGENERAATTORI   C_taaj_2_2.c   KP  */

/*   kutistettu versio 4 :  */

// V4: Muutettu nayta-funktion toiminta: param. ekan merkin positio, tyhjätään alkunollat
//	Volatilet poistettu alkutilavarauksista. Näyttöpusk. alustus muutettu.
// 	LCD_ohjaus -funktio yksinkertaistettu

#include <avr/io.h>		/* i/o-symbolit: C:\WinAVR-20090313\avr\include\avr\iom128.h  */
#include <avr/interrupt.h>	/* Keskeytysmakrot */
#include <avr/pgmspace.h>	/* FLASH-ohjelmamuistin käsittely */

uint8_t 
	Laskuri_1s = 1,	// Pääohjelmasilmukan 1s aikalaskuri
	Laskuri_02s = 4,	// Pääohjelmasilmukan 0,2s aikalaskuri
	KeskLippu = 0,	// Jos =1, TIMER1_COMPA-keskeytys tapahtunut (10ms välein)
	B1lask,B2lask,	// Kytkinvärähtelyn eliminointiin käytettävät laskurit kytkimille B1 ja B2
	Kytk,			// Kytkinportin sisältö lukuhetkellä
	db[32],		// Näyttöpuskuri
	dindex,		// Näyttöpuskurin indeksi tulostustilanteessa
	rivinsiirto,		// 1, kun lcd:lle on kirjoitettu siirtokomento alariville
	tulostus_kesken,	// 1, kun 32 merkin tulostus lcd:lle on meneillään
	PB6_tila = 0;	// Kanttigeneraattorin lähtösignaalin tila (PINB&0x40) PB6-nastassa, luetaan compb-kesk.palv.alussa
uint16_t 
	KesHetki,		// Pääohjelmasilmukan ajastus: TIMER1_COMPA-keskeytyshetki		
	OVFlaskuri = 0,	// Timer1:n ylivuotokappalelaskuri
	puls_lkm=0,		// Mitattavan kanttiaallon nousureunojen lukumäärä
	kaappausrek,	// Mitattavan kanttiaallon viimeisimmän nousureunan ajanhetki (ICR1)
	oc1b_hetki,		// Tuotettavan kantin reunan ajanhetki
	tilan_0_kesto,	// Tuotettavan kantin 0- ja 1-tilojen kestoajat
	tilan_1_kesto;
int16_t
	nyt_aika,		// apumuuttuja, Timer1:stä luettu aika
	lcd_aika;		// Seuraavan merkin kirjoitusajankohta LCD:lle (n. 100us välein)
uint32_t 		//Vertaa käännöstuloksia, jos nämä olisivat long eli int32_t
	ed_nousuhetki, 	// Mitattavan kantin nousureunan hetki
	nousuhetki, 		// Mitattavan kantin nousureunan hetki
	jakso;  		// Mitattavan kantin jaksonaika
uint8_t
	naytolle,		// =1, kun taajuuden laskentaan tarvittavat uudet tiedot ovat valmiina
	f_ovf;			// =1, kun mitattava taajuus on liian suuri, CAPT-kes.palvelu ei ehdi suoriutua
uint16_t
	jaksolkm,		// Edellisen noin 1 s:n aikana tulleiden nousureunojen lkm, joista taajuus lasketaan
	n1s_puls_lkm;	// Parhaillaan kuluvan sekunnin aikana tulleiden nousureunojen lukumäärä

uint32_t
	laskentajakso,	// Aika, jonka kuluessa on tullut "n1s_puls_lkm" kpl.nousureunoja
	n1s,			// sama kuin ed.
	n1s_alkuhetki;	// Ed. aikapätkän alkuhetken kellonaika
float
	ftaajuus;		// = nousureunojen lukum. jaettuna siihen kuluneella ajalla

const uint8_t alkuteksti[] PROGMEM =	// Merkkijono ohjelmamuistissa
"0.00000 Hz      "
"      0 us      ";

const int32_t dek[]={1,10,100,1000,10000,100000,1000000,10000000,100000000}; // Lukujono missä???
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
ISR (TIMER1_COMPA_vect)		// Timer1-A-vertailukeskeytyspalvelu (n. 5 us), suoritetaan 10 ms välein
{						// käytetään pääohjelmasilmukan ajastamiseen
	KesHetki = KesHetki + 20000;	// Uuden compa-kesk. viritys 10 ms päähän. Timer askeltaa 0,5us välein, 20000*0,5us = 10 ms
	OCR1A = KesHetki;
	KeskLippu = 1;			// Pääohjelman ajastuslipun asetus
}

/*******************************************************************************************/
ISR (TIMER1_COMPB_vect)		// Timer1-B-vertailukeskeytyspalvelu (n.10 us), suoritetaan aina,
{						// kun oc1b-lähtö on kääntynyt eli on saatu tuotettavan kanttiaallon reuna.

	PB6_tila = PINB & 0x40;			// Luetaan tämänhetkinen oc1b/PB6-bitin tila
	if (PB6_tila == 0){				// jos lähtöbitti on nyt 0, ohjelmoidaan nousureuna
		oc1b_hetki += tilan_0_kesto;	// Seuraavan reunan hetki= edellisen reunan hetki + tilan_0_kesto
		TCCR1A |=  0x30;			// viritetään nousureuna
		OCR1B = oc1b_hetki;
	}
	else {						// jos lähtöbitti on nyt 1, ohjelmoidaan laskureuna
		oc1b_hetki += tilan_1_kesto;
		TCCR1A &= ~0x10;		// nyt viritetään laskur.
		OCR1B = oc1b_hetki;
	}
}

/**************************************************************************************************/
ISR (TIMER1_CAPT_vect)		// Timer1-kaappauskeskeytyspalvelu  (n. 17 us)
{						// Tähän tullaan aina, kun ICP1/PD4-nastaan tulee mitattavan signaalin nousureuna
	puls_lkm++;				// Nousureunojen lukumäärälaskurin kasvatus
	kaappausrek = ICR1;		// Luetaan nousureunan tapahtumisen ajanhetki kaappausrekisteristä
	ed_nousuhetki = nousuhetki;	
	// nousuhetki = ((unsigned long)OVFlaskuri<<16)+(unsigned long)kaappausrek; // Koostetaan 32-bittinen "kellonaika"

	*((uint16_t *) &nousuhetki + 1) = OVFlaskuri;			// Sama tehokkaammin
//	*((uint16_t *) &nousuhetki + 0) = kaappausrek; 		//warning: dereferencing type-punned pointer will break strict-aliasing rules
	*((uint8_t *) &nousuhetki + 1) = *((uint8_t *) &kaappausrek+1);
	*((uint8_t *) &nousuhetki + 0) = *((uint8_t *) &kaappausrek+0);


	if ((TIFR&0x04) && (((*((uint8_t *) &kaappausrek +1)) & 0x80)==0)) 	// Jos Timer1-ylivuoto oli palvelematta kaappaushetkellä,
	{
		(*((uint16_t *) &nousuhetki + 1))++;		// korjataan "kellonaika" lisäämällä ykkönen 4-tavuisen muuttujan ylempään wordiin.
	}
 
	jakso = nousuhetki - ed_nousuhetki;		// Kahden peräkkäisen nousureunan aikaero = jaksonaika

	n1s_puls_lkm++;					// Nousureunojen lkm n. 1s laskenta-aikana
	n1s = nousuhetki - n1s_alkuhetki;		// Tarkka kestoaika ed. mainitun pulssimäärän tuloon
	if (n1s>2000000)					// Jos 1 s laskenta-aika on ylittynyt
	{
		laskentajakso=n1s;				// Kopioidaan tiedot pääohjelmaa varten
		jaksolkm=n1s_puls_lkm;			//
		n1s_puls_lkm=0;				//	ja alustetaan uutta sarjaa varten
		n1s_alkuhetki=nousuhetki;
		naytolle=1;					// Pääohjelmalle tieto, että taajuus voidaan laskea
	}
	if (TIFR&0x20)			//ICF1		Jos uusi reuna on jo nyt tullut, vaikka tästäkään keskeytyksestä
	{	TIMSK &= ~0x20;		//TICIE1		ei olla ehditty ulos, kielletään CAPT-kesk. ja
		f_ovf=1;					//  	laitetaan "taajuusylivuoto"-lippu merkiksi
	}
}
/****************************************************************************************/
void painiketoiminnot(void)
{
		Kytk = PINA;							
												// B1-painikkeen käsittely (PINA.0)
		if (((Kytk & 0b0001) == 0b0001) && (B1lask > 0))  B1lask--;		//Jos kytk. ei enää paineta mutta laskuri ei vielä 0, vähennä laskuria
		else if (((Kytk & 0b0001) == 0) && (B1lask > 0)) B1lask=9;		//Jos kytk. painettuna kun laskuri ei vielä ole 0, laskuriin alkuarvo 9
		else if (((Kytk & 0b0001) == 0) && (B1lask == 0))			//Jos kytk. painetaan laskurin ollessa 0, alkaa toiminta
		{
			B1lask = 9;								//Värähtelynpoistolaskurin viritys
			// TÄHÄN TEHTÄVÄT, JOTKA TEHDÄÄN, KUN ON PAINETTU B1-PAINIKETTA
		}

		if (((Kytk & 0b0010) == 0b0010) && (B2lask > 0))  B2lask--;		//Jos kytk. ei enää paineta mutta laskuri ei vielä 0, vähennä laskuria
		else if (((Kytk & 0b0010) == 0) && (B2lask > 0)) B2lask=9; 		//Jos kytk. painettuna kun laskuri ei vielä ole 0, laskuriin alkuarvo 9
		else if (((Kytk & 0b0010) == 0) && (B2lask == 0))			//Jos kytk. painetaan laskurin ollessa 0, alkaa toiminta
		{
			B2lask = 9;								//Värähtelynpoistolaskurin viritys
			/* TÄHÄN TEHTÄVÄT, JOTKA TEHDÄÄN, KUN ON PAINETTU B2-PAINIKETTA  */
		}

		/* TÄHÄN SEURAAVAN PAINIKKEEN KÄSITTELY VASTAAVASTI KUIN YLLÄ OLEVA  */

		/* TÄHÄN SITÄ SEURAAVAN PAINIKKEEN KÄSITTELY VASTAAVASTI KUIN YLLÄ OLEVA  */

}
/****************************************************************************************/
/* Kokonaislukumuuttujan muunto ascii-merkeiksi näyttöpuskuriin  				*/

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
  dindex = 0xFF;			/* Näyttöpuskurin tulostusindeksi = -1 */
  tulostus_kesken = 0;		/* Näytölle tulostusta ei ole aloitettu */
  rivinsiirto = 0;			/* Näytön tulostuksessa ei ole annettu rivinsiirtokomentoa */
  kopioi_mjono(db,alkuteksti);	/* Kopioidaan näyttöpuskuriin alkuarvot */
  lcd_init();				/* LCD:lle alustuskomentosekvenssi */

  KesHetki = 20000;		/* Ekan OCR1A-keskeytyksen ajanhetki 10 ms resetistä*/
  OCR1A = KesHetki;

  oc1b_hetki = 30000;		/* Ekan OCR1B-keskeytyksen ajanhetki 15 ms resetistä*/
  OCR1B = oc1b_hetki;
  
  tilan_1_kesto = 100; 		/* Kanttigeneraattorin 1-tilan kesto 50us */
  tilan_0_kesto = 240; 		/* Kanttigeneraattorin 0-tilan kesto 120us */

  TCCR1A = 0b01110000;		/* 0x70; 01 11 00 00 toggle oc1a, oc1b-nousu */
  TCCR1B = 0b01000010;  	/* TCCR1B=(1<<ICES1)+(1<<CS11); nousureunakaappaus,nonPWM,clk/8  */
  TIMSK = 0b00111100;		/* TIMSK = 0x1C; TIMSK = (1<<TICIE1)+(1<<OCIE1A)+(1<<OCIE1B)+(1<<TOIE1);*/
  sei();

  while (1)				// PÄÄOHJELMASILMUKKA
  {
	if (KeskLippu == 1)	{	// Ehto toteutuu 10 ms välein, timer1_compa-keskeytys asettaa
		KeskLippu = 0;
		painiketoiminnot();	// Kytkintilojen luenta ja kunkin painikkeen tilan mukainen toiminto

		Laskuri_02s--;					// 0,2 sekunnin välein tehdään seuraavia asioita:
		if (Laskuri_02s == 0) {
			Laskuri_02s = 20;
			if (tulostus_kesken == 0) {		// Jos näyttöpuskurin tulostus ei ole meneillään ja
				if ((jakso>>1)<10000000) {		// jos mitattu jaksonaika on pienempi kuin 10 s, niin
					nayta(jakso>>1,16,7);		// muunnetaan jaksonaika näyttöpuskuriin ASCII-merkkijonoksi
					db[24]='u';			// ja laitetaan perään yksiköksi mikrosekunti
					db[25]='s';
				}
				else {
					nayta((jakso>>1)/1000,16,7);	// Jaksonajan näyttö LCD:llä yksikkönä ms, jos
					db[24]='m';				// jaksonaika onkin suurempi kuin 10 s.
					db[25]='s';
				}
				tulostus_kesken = 1;		// Näyttöpuskurin tulostuksen käynnistys 0,2 s välein	
				cli();
				lcd_aika = TCNT1 + 200;		// Ensimmäisen merkin kirjoitus LCD:lle n.100 us kuluttua
				sei();
			}
		}
		else {							// Millaisin aikavälein tehdään seuraavat asiat?
			if (naytolle==1) {				// Jos kaappauskeskeytys on laskenut tarvittavat alkutiedot
				naytolle =0;
				if(f_ovf==0) {						// ja jos ei ole ylivuototilanne, niin
				    ftaajuus=jaksolkm/(laskentajakso * 0.5e-6);	// lasketaan taajuus. (268,63 us)
				    fnayta(ftaajuus,0,6);				// Näytölle taajuus, 6 merk. digittiä. (2063,8 us)
				}
				else {
				    db[0]='-';db[1]='-';db[2]='-';			// Jos mitattava taajuus on liian suuri,
				    db[3]='-';db[4]='-';db[5]='-';db[6]='-';		// näyttöön ------------
				}
			}
		}
		Laskuri_1s--;					// Yhden sekunnin välein tehdään seuraavat asiat:
		if (Laskuri_1s == 0) {
			Laskuri_1s = 100;
//			PORTA = PORTA ^ 0x40;		// Releen tilan vaihto 1 s välein
			f_ovf=0;					// Nollataan mitatun taajuuden mahdollinen ylivuototilanne
  			TIMSK |= 0b00100000;		//1<<TICIE1;  //    ja sallitaan taas kaappauskeskeytykset 
		}
	}
	else {  // vaikka 10ms keskeytyslippu ei olekaan pystyssä, kirjoitetaan näytölle vuorossa oleva merkki, jos
		if (tulostus_kesken == 1) {			// Jos näyttöpuskurin tulostus on kesken
	 		cli();					// Miksi tässä on keskeytyskielto?
			nyt_aika = TCNT1;			// Luetaan Timer1:stä tämänhetkinen aika
			sei();
			if (nyt_aika-lcd_aika > 0){			// Jos timeri on ohittanut näytölle tulostuksessa tarvittavan 100us 
				LCD_lle_puskurista_merkki ();		// merkkiväliajan, kirjoitetaan näyttöpuskurista vuorossa oleva merkki LCD:lle
			}	// if (nyt_aika-lcd_aika>0)
		}		// if (tulostus_kesken==1)
	}		// else
  }			// while(1)
}		 	// main

/****************************************************************************************/
/* Reaalilukumuuttujan muunto asciimerkeiksi näyttöpuskuriin tyyliin 357.629Hz tai .076928Hz	*/

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
/* LCD-näytölle E-signaalin pulssi                             */
void epulssi (void) __attribute__ ((noinline));
void epulssi (void)
{	volatile char i = 2;
	PORTC |= 0x04;			// E-signaali ylös
	while (i>0) i--;			// Noin mikrosekunnin viive
	PORTC &= ~0x04;			// E-signaali alas
}
/***************************************************************************/
/* Merkin tai ohjauskomennon kirjoitus LCD:lle					*/

void lcd_ohjaus (uint8_t tyyppi, uint8_t data)	// LCD: D7 D6 D5 D4 nc E RW RS
{
	PORTC = (data & 0xF0) | (PORTC & 0x0E)| tyyppi;		// merkin 4 MSB:tä LCD:lle
	epulssi();
	PORTC = (data<<4) | (PORTC & 0x0F);			// merkin 4 LSB:tä LCD:lle
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
/* Nestekidenäyttömodulin vaatima alustus sähkön kytkemisen jälkeen       */

void lcd_init(void)
{
  PORTC = 0x00;
  viive_ms(40);					// min 15 ms after Vcc rises to 4.5V
  PORTC = 0x30; epulssi(); viive_ms(6); 	//Function set, minimiviive 4100 us
  PORTC = 0x30; epulssi(); viive_ms(6); 	//Function set, minimiviive 100 us
  PORTC = 0x30; epulssi(); viive_ms(2); 	//Function set, minimiviive 42 us
  PORTC = 0x20; epulssi(); viive_ms(2); 	//Function set, väylä 4-bittiseksi, minimiviive 42 us

  lcd_ohjaus(0,0x28); viive_ms(2);	//Function set,  4bit, 2rivinen, minimiviive 42 us
  lcd_ohjaus(0,0x08); viive_ms(2);	//display off,curs off, blink off, minimiviive 42 us
  lcd_ohjaus(0,0x01); viive_ms(3);	//clear display, minimiviive 1640 us
  lcd_ohjaus(0,0x06); viive_ms(2);	//entry mode set	
  lcd_ohjaus(0,0x0C); viive_ms(2);	//display on,curs off,blink off
  lcd_ohjaus(0,0x80); viive_ms(1);	//DD-address=0
}

/**********************************************************************************************/
/* Näyttöpuskurista vuorossa olevan yhden ASCII-merkin tai kursorinsiirtokomennon kirjoitus LCD-moduliin  */

void LCD_lle_puskurista_merkki (void)
{
	dindex++;						// Kasvata näyttöpuskurin indeksiä
	switch (dindex)
	{
	case 16:						// Jos ylärivi on kirjoitetu täyteen
		if (rivinsiirto == 0) {			// ja jos rivinsiirtokomentoa ei vielä ole annettu
			rivinsiirto = 1;
			dindex--;				//   peruutetaan indeksiä takaisin
			lcd_ohjaus (0, 0xC0);		//   kursorin siirtokomento alarivin alkuun
		}
		else {						// Kun rivinsiirtokomento on kirjoitettu
			rivinsiirto = 0;
			lcd_ohjaus (1, db[dindex]);	// kirjoitetaan alarivin ensimmäinen merkki
		}
		break;
	case 32:						// Koko puskurillinen on kirjoitettu
		tulostus_kesken = 0;			// Merkkijonon tulostus valmistui
		dindex = 0xFF;				// Puskurin indeksin palautus alkuun, arvoksi -1
		lcd_ohjaus (0, 0x80);			// LCD:lle kursorinsiirtokomento ylärivin alkuun
		return;					// Pois täältä laskematta seuraavaa merkkiväliaikaa
		break;
	default:
		lcd_ohjaus (1, db[dindex]);		// Vuorossa olevan ASCII-merkin kirjoitus LCD:lle
		break;
	}
	cli();
	nyt_aika = TCNT1;		
	sei();
	lcd_aika = nyt_aika + 200; 	// Seuraavan merkin tai komennon kirjoitusajankohdan laskenta 100 us päähän
}
/*******************************************************************/
/* Merkkijonon kopiointi 16-bittisestä FLASH-ohjelmamuistista 8-bittiseen SRAMmiin                 */

void kopioi_mjono (volatile uint8_t *kohde, const uint8_t *lahde)
{
	uint8_t i = 0;
	while (pgm_read_byte (&(lahde[i])) != '\0') {		// Kunnes ohjelmamuistista löytyy NUL-merkki
		kohde[i] = pgm_read_byte (&(lahde[i]));	// luetaan indeksin osoittama merkki FLASHissä olevasta taulukosta kohdetaulukkoon
		i++;							// Kasvatetaan indeksiä.
	}
}
