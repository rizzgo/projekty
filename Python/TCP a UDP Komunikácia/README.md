# Klient pre distribuovaný súborový systém

Cieľom projektu je implementovať klienta pre triviálny (read-only) distribuovaný súborový systém. Tento systém používa URL pre identifikáciu súborov a ich umiestnenia. 
Systém pre prístup k súborom používa File Service Protocol. Systém používa symbolické mená, ktoré sú preložené na IP adresy pomocou protokolu Name Service Protocol. 

Klient dokáže sťahovať súbory zo serverov systému. Klient uloží stiahnutý súbor pod jeho menom do aktuálneho adresára a v prípade, že nastane chyba, klient túto chybu vhodným spôsobom vypíše a žiadny súbor nebude
vytvorený. 

Klienta je možné spustiť s týmito parametrami:
- fileget -n NAMESERVER -f SURL
- NAMESERVER - IP adresa a číslo portu menného servera.
- SURL - SURL súboru pre stiahnutie. 

Protokol v URL je vždy FSP.
Oba parametre sú povinné a ich poradie je voliteľné. 
