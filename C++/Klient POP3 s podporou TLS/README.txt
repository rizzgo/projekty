POP3 klient - popcl

Program popcl slúži na ukladanie a mazanie správ z POP3 servera. 
Umožňuje vybrať spôsob zabezpečenia pripojenia a taktiež stiahnuť len nové, ešte nevyzdvihnuté správy.


Použitie: 

./popcl <server> -a <autentifikačný_súbor> -o <výstupný_priečinok>

Povinné parametre:
<server> - ip adresa servera alebo jeho doménové meno
-a <autentifikačný_súbor> - cesta k súboru s menom a heslom pre prihlásenie na server
-o <výstupný_priečinok> - cesta k priečinku pre uloženie stiahnutých správ

Volitelné parametre:
-p <port> - pre výber čísla portu
-n - pre stiahnutie len nových správ
-d - pre vymazanie správ na serveri
-T - spustí šifrovanie celej komunikácie
-S - zaháji šifrované spojenie po pripojení k serveru pomocou príkazu STLS
-c <cesta_k_súboru> - pre pridanie cesty k súboru s certifikátom
-C <cesta_k_priečinku> - pre pridanie cesty s priečinku s cerifikátmi

Parameter server sa zadáva ako prvý.
Všetky ostatné parametre môžu byť použité v ľubovoľnom poradí.


Príklady spustenia:

./popcl pop3.centrum.sk -T -a auth/centrum_auth.txt -o storage/centrum     
You retrieved 2 messages. 

./popcl pop3.centrum.sk -T -a auth/centrum_auth.txt -o storage/centrum -n  
No new messages have been found. 

./popcl pop3.centrum.sk -T -a auth/centrum_auth.txt -o storage/centrum -n -d
You retrieved 1 new message.

./popcl pop3.centrum.sk -T -a auth/centrum_auth.txt -o storage/centrum
Your mailbox is empty.


Obsah archívu:
popcl.h
popcl.cpp
Makefile
manual.pdf
README.txt
