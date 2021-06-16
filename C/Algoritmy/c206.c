
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	// Kontrola alokácie zoznamu.
	if (L == NULL) {
		DLError();
		return;
	}

	// Počiatočná hodnota ukazateľov. 
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	while (L->First != NULL) { // Prechádza celý zoznam.
		L->Act = L->First->rptr; // Uloží nasledujúci prvok (Pri poslednom sa uloží NULL).
		free(L->First); // Uvolní prvý.
		L->First = L->Act; // Nasledujúci sa stane prvým.
	}
	L->Last = NULL; // Všetky ukazatele po inicializácii ukazujú na NULL.
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	// Alokácia nového prvku.
	tDLElemPtr newElemPtr;
	newElemPtr = malloc(sizeof(struct tDLElem));
	if (newElemPtr == NULL) {
		DLError();
		return;
	}

	// Naplnenie nového prvku.
	newElemPtr->data = val;
	newElemPtr->lptr = NULL;
	newElemPtr->rptr = L->First;

	// Pridanie do zoznamu.
	if (L->First == NULL) {
		L->Last = newElemPtr; // Ak je vkladaný prvok prvý v zozname, je aj posledný.
	}
	else {
		L->First->lptr = newElemPtr; // Ak nie, tak posunutý prvok ukazuje spätne na nový.
	}

	L->First = newElemPtr; // Nový prvok je prvý.
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	// Alokácia nového prvku.
	tDLElemPtr newElemPtr;
	newElemPtr = malloc(sizeof(struct tDLElem));
	if (newElemPtr == NULL) {
		DLError();
		return;
	}

	// Naplnenie nového prvku.
	newElemPtr->data = val;
	newElemPtr->lptr = L->Last;
	newElemPtr->rptr = NULL;
	
	// Pridanie do zoznamu.
	if (L->First == NULL) {
		L->First = newElemPtr; // Ak bol zoznam prázdny, je nový prvok zároveň prvým.
	}
	else {
		L->Last->rptr = newElemPtr;  // Ak nie, tak je pridaný za posledný.
	}

	L->Last = newElemPtr; // Nový prvok je posledný.
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if (L->First == NULL) {
		DLError();
		return;
	}

	*val = L->First->data;

}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if (L->Last == NULL) {
		DLError();
		return;
	}

	*val = L->Last->data;

}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if (L->First == L->Act) { // Kontrola aktivity.
		L->Act = NULL;
	}
	if (L->First != NULL) { // Vymazanie prvého prvku, ak nejaký je.
		tDLElemPtr nextElemPtr;
		nextElemPtr = L->First->rptr; // Uloží ukazateľ na ďalší prvok.
		free(L->First); // Uvoľní sa prvý prvok.
		nextElemPtr->lptr = NULL; // Nový prvý prvok neukazuje nikam vľavo.
		L->First = nextElemPtr; // Nový prvok je prvý.
	}
	if (L->First == NULL) {
		L->Last = NULL; // Ak bol vymazaný prvok zároveň posledným prvkom. 
	}
	
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if (L->Last == L->Act) { // Kontrola aktivity.
		L->Act = NULL;
	}
	if (L->Last != NULL) { // Vymazanie posledného prvku, ak nejaký je.
		if (L->Last->lptr != NULL) { // Ak posledný nebol jediný v zozname.
			L->Last = L->Last->lptr; // Predposledný prvok sa stáva posledným.
			free(L->Last->rptr); // Uvoľnenie prvku.
			L->Last->rptr = NULL;
		}
		else {
			L->First = NULL; // Ak bol jediný, zoznam je prázdny.
			free(L->Last);
			L->Last = NULL;
		}
	}
	
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if (L->Act != NULL && L->Act->rptr != NULL) { // Kontrola aktivity.
		tDLElemPtr nextElemPtr;
		nextElemPtr = L->Act->rptr->rptr; // Uloží prvok za vymazávanym.
		free(L->Act->rptr); // Vymaže sa prvok za aktívnym.
		
		if (L->Act->rptr == NULL) { // Ak bol vymazaný prvok posledný v zozname.
			L->Last = L->Act; // Aktívny prvok sa stane posledným. 
		}
		else {
			nextElemPtr->lptr = L->Act; // Ak nie, prepojí sa nasledujúci s aktívnym.
		}
		L->Act->rptr = nextElemPtr; // Prepojí aktívny s nasledujúcim.
		

		
	}
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if (L->Act != NULL && L->Act->lptr != NULL) { // Kontrola aktivity.
		tDLElemPtr prevElemPtr;
		prevElemPtr = L->Act->lptr->lptr; // Uloží prvok pred vymazávanym.
		free(L->Act->lptr); // Vymaže sa prvok pred aktívnym.

		if (prevElemPtr == NULL) { // Ak bol vymazaný prvok prvý v zozname.
			L->First = L->Act; // Aktívny prvok sa stane prvým.
		}
		else {
			prevElemPtr->rptr = L->Act; // Ak nie, prepojí sa predchádzajúci s aktívnym.
		}
		
		L->Act->lptr = prevElemPtr; // Prepojí aktívny s predchádzajúcim.

		
	}
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL) {	
		// Alokácia nového prvku.
		tDLElemPtr newElemPtr;
		newElemPtr = malloc(sizeof(struct tDLElem));
		if (newElemPtr == NULL) {
			DLError();
			return;
		}

		// Naplnenie nového prvku.
		newElemPtr->data = val;
		newElemPtr->lptr = L->Act;
		newElemPtr->rptr = L->Act->rptr;

		// Pridanie do zoznamu.
		if (L->Act->rptr != NULL) { // Ak nie je nový prvok posledný.
			L->Act->rptr->lptr = newElemPtr; // Nasledujúci prvok ukazuje spätne na nový.
		}
		else {
			L->Last = newElemPtr; // Ak je, je posledný.
		}

		L->Act->rptr = newElemPtr; // Aktívny ukazuje na nový.

	}



}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL) {
		// Alokácia nového prvku.
		tDLElemPtr newElemPtr;
		newElemPtr = malloc(sizeof(struct tDLElem));
		if (newElemPtr == NULL) {
			DLError();
			return;
		}

		// Naplnenie nového prvku.
		newElemPtr->data = val;
		newElemPtr->lptr = L->Act->lptr;
		newElemPtr->rptr = L->Act;

		// Pridanie do zoznamu.
		if (L->Act->lptr != NULL) { // Ak nie je nový prvok prvý v zozname.
			 L->Act->lptr->rptr = newElemPtr; // Predchádzajúci prvok ukazuje na nový.
		}
		else {
			L->First = newElemPtr; // Ak je, je prvý.
		}
	}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if (L->Act == NULL) { // Kontrola aktivity.
		DLError();
		return;
	}

	*val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if (L->Act != NULL) {
		L->Act->data = val;
	}
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL) {
		L->Act = L->Act->rptr;
	}
}

void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL) {
		L->Act = L->Act->lptr;
	}
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return L->Act != NULL;
}

/* Konec c206.c*/
