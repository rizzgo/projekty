# Turingov stroj

## Spustenie

Program je možné preložiť príkazom `make` a následne spustiť ako:
```
./flp22-log < 'cesta k vstupnému súboru' > 'cesta k výstupnému súboru'
```

Napríklad:

```
./flp22-log < tests/ref.in > ref.out
```

## Vstup

Prijímaný vstup je v tvare:
```
S a B a
B a B b
B b B R
B c B a
B c F c 
B c B a
aaacaa
```

Posledný riadok vstupu predstavuje vstupnú pásku turingovho stroja a ostatné riadky predstavujú množinu pravidiel v tvare `<stav> <symbol na páske> <nový stav> 
<nový symbol na páske alebo „L“, „R“>`.

## Výstup

Výstup programu má tvar:
```
Saaacaa
Baaacaa 
Bbaacaa 
bBaacaa 
bBbacaa 
bbBacaa
bbBbcaa
bbbBcaa
bbbFca
```

Každý riadok predstavuje konfiguráciu, ktorou stroj prejde, kým sa dostane do koncového stavu. Konfigurácia má tvar `<obsah pásky pred hlavou><stav><symbol pod hlavou><zvyšok pásky>`. Ak stroj abnormálne zastaví, program je ukončený a na výstup nie sú vypísané žiadne konfigurácie. Ak stroj cyklí, cyklí aj program.

## Detaily implemetácie

Vykonávanie programu začna načítaním vstupu po riadkoch predikátom `process_input`. Každý riadok je spracovávaný predikátom `process_line`, pričom ak začína veľkým znakom je spracovaný ako pravidlo. V opačnom prípade je spracovaný ako vstup na páske. Pravidlá, aj vstup na páske sú dynamicky ukladané vstavaným predikátom `assertz`.

Nasleduje použitie predikátu `next_step`, ktorým sú rekrzívne prehľadávané konfigurácie, do ktorých sa stroj môže dostať v ďalšom kroku. Ten má na vstupe zoznam konfigurácií, ktoré je potrebné prehľadať a na výstupe zoznam výsledných konfigurácií. V jednom kroku vždy spracuje jednu z nich.

Konfigurácia je reprezentovaná aktuálnym stavom, zoznamom prvkov naľavo od pozície hlavy a zoznamom prvkov napravo, v ktorom je prvým prvkom aktuálne načítavaný symbol na páske. Predikátom `bagof` sú vybrané pravidlá, ktoré je v danej konfigurácii možné použiť. Predikát `possible_configs` pre každé z nich vráti novú konfiguráciu.  Nová konfigurácia pásky je zostavená predikátom `next_tape_config`, na základe starej pásky a nového symbolu, či znakov *'L'* a *'R'*. 

Samotné prehľadávanie je založené na princípe prehľadávania do šírky, preto sú všetky možné nasledujúce konfigurácie pridané na koniec zoznamu prehľadávaných konfigurácií. Ten je následne atribútom predikátu `next_step` pri vykonaní ďalšej iterácie prehľadávania.

Pri abnormálnom zastavení prechodom za ľavý koniec pásky je vrátená prázdna páska. Stroj pri pokuse vykonať ďalší krok z konfigurácie s prázdnou páskou zlyhá. Oddialenie zlyhania pri generovaní konfigurácie zaručí vygenerovanie ostatných možných konfigurácií z aktuálnej konfigurácie, ktorými môže neskôr pokračovať. Stroj tiež zlyhá, ak z určitej konfigurácie nemôže použiť žiadne pravidlo. V oboch prípadoch je predikátom `next_step` daná konfigurácia zahodená a prehľadávanie pokračuje spracovávaním nasledujúcej konfigurácie v zozname. 

Nájdením konfigurácie obsahujúcej koncový stav prehľadávanie končí úspechom. Pri spätnom vynáraní z rekurzie je zostavený zoznam výsledných konfigurácii, ktoré viedli k jeho dosiahnutiu. To zabezpečuje predikát `update_output`, ktorý danú konfiguráciu pridá na začiatok zoznamu, ak je predchodcom poslednej konfigurácie v zatiaľ zostavenom zozname. Na konci sú konfigurácie vo výslednom zozname zostavené do reťazcov a vypísané na výstup. Následne je program ukončený.

## Spustenie testov

Repozitár obsahuje súbor `runtests.py` v jazyku python pre automatické spustenie testov a porovnanie výsledkov s referenčnými riešeniami. Priečinok *tests* obsahuje vstupy testov a ich referenčné riešenia. Výsledky spustenia testov sú uložené do preičinka *test-outputs*, ktorý je po ukončení testovania vymazaný. Spustenie testov je možné v tvare `python3 runtests.py`.

## Obsah repozitára

- /tests
- flp22-log.pl
- Makefile
- README.md
- runtests.py
