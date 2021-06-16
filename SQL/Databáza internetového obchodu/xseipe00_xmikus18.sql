DROP INDEX hodnotenie_pocet_i;
DROP MATERIALIZED VIEW zamestnanci;
DROP MATERIALIZED VIEW vysoke_platby;
DROP SEQUENCE objednavka_auto_id;
DROP SEQUENCE pouzivatel_auto_id;
DROP SEQUENCE kategoria_auto_id;
DROP SEQUENCE produkt_auto_id;
DROP SEQUENCE hodnotenie_auto_id;
DROP SEQUENCE platba_auto_id;
DROP TABLE Spravuje;
DROP TABLE Prida;
DROP TABLE Obsahuje;
DROP TABLE Objednavka_produktu;
DROP TABLE Hodnotenie;
DROP TABLE Platba;
DROP TABLE Objednavka;
DROP TABLE Zakaznik;
DROP TABLE Produkt;
DROP TABLE Zamestnanec;
DROP TABLE Vedenie;
DROP TABLE Pouzivatel;
DROP TABLE Kategoria;

create table Pouzivatel (
    id_pouzivatela NUMBER(10) PRIMARY KEY,
    pouzivatelske_meno VARCHAR(30) NOT NULL ,
    heslo VARCHAR(50) NOT NULL ,
    meno VARCHAR(50) NOT NULL ,
    priezvisko VARCHAR(50) NOT NULL ,
    email VARCHAR(50) NOT NULL ,
    ulica VARCHAR(30) ,
    popisne_cislo NUMBER(10) ,
    psc NUMBER(5) NOT NULL ,
    mesto VARCHAR(30) NOT NULL ,
    telefonne_cislo NUMBER(20)
);

create table Zakaznik (
    id_zakaznika NUMBER(10) PRIMARY KEY,
    FOREIGN KEY (id_zakaznika) REFERENCES Pouzivatel (id_pouzivatela)
);

create table Zamestnanec (
    id_zamestnanca NUMBER(10) PRIMARY KEY,
    FOREIGN KEY (id_zamestnanca) REFERENCES Pouzivatel (id_pouzivatela)
);

create table Vedenie (
    id_vedenia NUMBER(10) PRIMARY KEY,
    FOREIGN KEY (id_vedenia) REFERENCES Pouzivatel (id_pouzivatela)
);

create table Spravuje (
    id_zamestnanca NUMBER(10) NOT NULL,
    id_vedenia NUMBER(10) NOT NULL,
    PRIMARY KEY (id_zamestnanca, id_vedenia),
    FOREIGN KEY (id_zamestnanca) REFERENCES Zamestnanec (id_zamestnanca),
    FOREIGN KEY (id_vedenia) REFERENCES Vedenie (id_vedenia)
);

create table Objednavka (
    id_objednavky NUMBER(10) PRIMARY KEY ,
    dorucenie_ulica VARCHAR(30) NOT NULL ,
    dorucenie_popisne_cislo NUMBER(10) NOT NULL ,
    dorucenie_psc NUMBER(5) NOT NULL ,
    dorucenie_mesto VARCHAR(30) NOT NULL ,
    telefonne_cislo VARCHAR(30) NOT NULL ,
    stav_objednavky NUMBER(1) DEFAULT 0,
    sposob_dopravy NUMBER(1) NOT NULL ,
    cena NUMBER(10, 2) DEFAULT 0,
    datum_vytvorenia TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    datum_vybavenia TIMESTAMP,
    id_zamestnanca NUMBER(10) ,
    id_zakaznika NUMBER(10) NOT NULL ,
    FOREIGN KEY (id_zakaznika) REFERENCES Zakaznik (id_zakaznika),
    FOREIGN KEY (id_zamestnanca) REFERENCES Zamestnanec (id_zamestnanca)
);

create table Kategoria (
    id_kategorie NUMBER(10) PRIMARY KEY ,
    nazov VARCHAR(60) NOT NULL ,
    popis VARCHAR(300)
);

create table Prida (
    id_kategorie   NUMBER(10) ,
    id_zamestnanca NUMBER(10) ,
    PRIMARY KEY (id_kategorie, id_zamestnanca),
    FOREIGN KEY (id_kategorie) REFERENCES Kategoria (id_kategorie),
    FOREIGN KEY (id_zamestnanca) REFERENCES Zamestnanec (id_zamestnanca)
);

create table Produkt (
    id_produktu NUMBER(10) PRIMARY KEY ,
    nazov VARCHAR(60) NOT NULL ,
    popis VARCHAR(300) ,
    fotografia VARCHAR(100) ,
    cena NUMBER(10,2) ,
    pocet_na_sklade NUMBER(10) NOT NULL,
    id_zamestnanca NUMBER(10) NOT NULL,
    FOREIGN KEY (id_zamestnanca) REFERENCES Zamestnanec(id_zamestnanca)
);

create table Obsahuje (
    id_kategorie NUMBER(10) ,
    id_produktu NUMBER(10) ,
    PRIMARY KEY (id_kategorie, id_produktu),
    FOREIGN KEY (id_kategorie) REFERENCES Kategoria (id_kategorie),
    FOREIGN KEY (id_produktu) REFERENCES Produkt (id_produktu)
);

create table Hodnotenie (
    id_hodnotenia NUMBER(10) PRIMARY KEY ,
    pocet_hviezd NUMBER(10) ,
    text_hodnotenia varchar(300) ,
    id_pouzivatela NUMBER(10) NOT NULL ,
    id_produktu NUMBER(10) NOT NULL ,
    CHECK (pocet_hviezd IS NOT NULL OR text_hodnotenia IS NOT NULL),
    FOREIGN KEY (id_pouzivatela) REFERENCES Pouzivatel (id_pouzivatela),
    FOREIGN KEY (id_produktu) REFERENCES Produkt (id_produktu)
);

create table Objednavka_produktu (
    id_objednavky_p NUMBER(10) ,
    id_produktu_o NUMBER(10) ,
    mnozstvo NUMBER(10) DEFAULT 1,
    PRIMARY KEY (id_objednavky_p,id_produktu_o),
    FOREIGN KEY (id_objednavky_p) REFERENCES Objednavka (id_objednavky) ,
    FOREIGN KEY (id_produktu_o) REFERENCES Produkt (id_produktu)
);

create table Platba(
    id_platby NUMBER(10) PRIMARY KEY ,
    id_objednavky NUMBER(10) NOT NULL,
    faktura_ulica VARCHAR(100) NOT NULL ,
    faktura_popisne_cislo NUMBER(5) NOT NULL ,
    faktura_psc NUMBER(5) NOT NULL ,
    faktura_mesto VARCHAR(30) NOT NULL ,
    vyska_platby NUMBER(10,2) DEFAULT NULL,
    sposob_platby NUMBER(1) NOT NULL ,
    stav_platby NUMBER(1) DEFAULT 0,
    FOREIGN KEY (id_objednavky) REFERENCES Objednavka(id_objednavky)
);

/*
 Procedúra na pridanie zamestnanca spolu s priradením vedúceho ktorý ho pridáva/spravuje
 */
CREATE OR REPLACE PROCEDURE pridaj_zamestnanca(v_id_pouzivatela NUMBER, v_id_vedenia NUMBER) AS
    BEGIN
        INSERT INTO Zamestnanec (id_zamestnanca) VALUES (v_id_pouzivatela);
        INSERT INTO Spravuje (id_zamestnanca, id_vedenia)
        VALUES (v_id_pouzivatela, v_id_vedenia);
    END;

/*
 Procedúra vypočíta aktuálnu cenu vybranej objednávky
 */
CREATE OR REPLACE PROCEDURE vypocitaj_novu_cenu(v_id_objednavky Objednavka_produktu.id_objednavky_p%TYPE)
AS
    cena_objednavky Objednavka.cena%TYPE;

    CURSOR v_objednavka IS
    SELECT mnozstvo, cena
    FROM Objednavka_produktu
    INNER JOIN Produkt
        ON Objednavka_produktu.id_produktu_o = Produkt.id_produktu
    WHERE Objednavka_produktu.id_objednavky_p = v_id_objednavky;

    v_mnozstvo Objednavka_produktu.mnozstvo%TYPE;
    v_cena Produkt.cena%TYPE;

BEGIN
    cena_objednavky := 0;
    OPEN v_objednavka;
    LOOP
        FETCH v_objednavka INTO v_mnozstvo, v_cena;
        EXIT WHEN v_objednavka%NOTFOUND;
        cena_objednavky := cena_objednavky + v_cena * v_mnozstvo;
    END LOOP;
    CLOSE v_objednavka;

    UPDATE Objednavka SET cena = cena_objednavky
    WHERE id_objednavky = v_id_objednavky;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
     DBMS_OUTPUT.PUT_LINE('No data to be processed');
    WHEN TOO_MANY_ROWS THEN
     DBMS_OUTPUT.PUT_LINE('Cant process many rows at once');
    WHEN INVALID_CURSOR THEN
     DBMS_OUTPUT.PUT_LINE('Illegal cursor operation');
    WHEN VALUE_ERROR THEN
     DBMS_OUTPUT.PUT_LINE('An arithmetic, conversion, truncation, or size constraint error occurred.');
END;


/*
  Sekvencie pre autoinkrementáciu každej tabuľky
  */
CREATE SEQUENCE pouzivatel_auto_id
START WITH 1
INCREMENT BY 1
NOCACHE
NOCYCLE;

CREATE SEQUENCE objednavka_auto_id
START WITH 1
INCREMENT BY 1
NOCACHE
NOCYCLE;

CREATE SEQUENCE kategoria_auto_id
START WITH 1
INCREMENT BY 1
NOCACHE
NOCYCLE;

CREATE SEQUENCE produkt_auto_id
START WITH 1
INCREMENT BY 1
NOCACHE
NOCYCLE;

CREATE SEQUENCE hodnotenie_auto_id
START WITH 1
INCREMENT BY 1
NOCACHE
NOCYCLE;

CREATE SEQUENCE platba_auto_id
START WITH 1
INCREMENT BY 1
NOCACHE
NOCYCLE;

/*
 Triggre pre autoinkrementáciu každej tabuľky
 */
CREATE OR REPLACE TRIGGER pridaj_id_pouzivatela
    BEFORE INSERT ON Pouzivatel
    FOR EACH ROW
    BEGIN
        :new.id_pouzivatela := pouzivatel_auto_id.nextval;
    END;

CREATE OR REPLACE TRIGGER pridaj_id_objednavky
    BEFORE INSERT ON Objednavka
    FOR EACH ROW
    BEGIN
        :new.id_objednavky := objednavka_auto_id.nextval;
    END;

CREATE OR REPLACE TRIGGER pridaj_id_kategorie
    BEFORE INSERT ON Kategoria
    FOR EACH ROW
    BEGIN
        :new.id_kategorie := kategoria_auto_id.nextval;
    END;

CREATE OR REPLACE TRIGGER pridaj_id_produktu
    BEFORE INSERT ON Produkt
    FOR EACH ROW
    BEGIN
        :new.id_produktu := produkt_auto_id.nextval;
    END;

CREATE OR REPLACE TRIGGER pridaj_id_hodnotenia
    BEFORE INSERT ON Hodnotenie
    FOR EACH ROW
    BEGIN
        :new.id_hodnotenia := hodnotenie_auto_id.nextval;
    END;

CREATE OR REPLACE TRIGGER pridaj_id_platby
    BEFORE INSERT ON Platba
    FOR EACH ROW
    BEGIN
        :new.id_platby := platba_auto_id.nextval;
    END;

/*
 Trigger v prípade priradenia zamestnanca k objednávke zmení stav objednávky
 */
CREATE OR REPLACE TRIGGER zmena_stavu_objednavky
BEFORE UPDATE ON Objednavka
FOR EACH ROW
BEGIN
    IF :new.id_zamestnanca IS NOT NULL
       THEN  :new.stav_objednavky := 2;
    END IF;
END;

/*
 Trigger pri vytvorení platby zistí cenu platenej objednávky a aktualizuje výšku platbu
 */
CREATE OR REPLACE TRIGGER zisti_cenu_objednavky
BEFORE INSERT OR UPDATE ON Platba
FOR EACH ROW
DECLARE
    cena_objednavky Objednavka.cena%TYPE;
BEGIN
    SELECT Objednavka.cena
    INTO cena_objednavky
    FROM Objednavka
    WHERE Objednavka.id_objednavky = :new.id_objednavky;

    :new.vyska_platby := cena_objednavky;
END;


/*
 Inserty
 */
INSERT INTO Pouzivatel (pouzivatelske_meno, heslo,
                        meno, priezvisko, email, ulica, popisne_cislo, psc, mesto, telefonne_cislo)
VALUES ('jozef', 'jozef32', 'Jozef', 'Jozefin', 'jozef.jozko@mail.com', 'Listova', 32, 08312, 'Brno', 0943854930);

INSERT INTO Pouzivatel (pouzivatelske_meno, heslo,
                        meno, priezvisko, email, ulica, popisne_cislo, psc, mesto, telefonne_cislo)
VALUES ('ignac', 'igi57', 'Ignac', 'Kovac', 'igi.k@mail.com', 'Listova', 32, 08312, 'Brno', 0943854930);

INSERT INTO Pouzivatel (pouzivatelske_meno, heslo,
                        meno, priezvisko, email, ulica, popisne_cislo, psc, mesto, telefonne_cislo)
VALUES ('igor', 'igros21', 'Igor', 'Gross', 'igor.gross@mail.com', 'Kolejni', 2, 08312, 'Brno', 0949645536);

INSERT INTO Pouzivatel (pouzivatelske_meno, heslo,
                        meno, priezvisko, email, ulica, popisne_cislo, psc, mesto, telefonne_cislo)
VALUES ('kristof', 'kristof23', 'Kristof', 'Podolacek', 'kristof.podolacek@mail.com', 'Kolejni', 2, 08312, 'Brno', 0949095536);

INSERT INTO Pouzivatel (pouzivatelske_meno, heslo,
                        meno, priezvisko, email, ulica, popisne_cislo, psc, mesto, telefonne_cislo)
VALUES ('milos', 'milos22', 'Milos','Michalica', 'milos.michalica@mail.com', 'Skacelova', 20, 08312, 'Brno', 0958420158);

SELECT * FROM Pouzivatel;

INSERT INTO Zakaznik (id_zakaznika) VALUES (1);
INSERT INTO Zamestnanec (id_zamestnanca) VALUES (2);
INSERT INTO Zakaznik(id_zakaznika) VALUES (2);
INSERT INTO Vedenie (id_vedenia) VALUES (3);
INSERT INTO Vedenie(id_vedenia) VALUES (5);


/*
 Test pridania zamestnanca
 */
CALL pridaj_zamestnanca(4, 3);


INSERT INTO Spravuje (id_zamestnanca, id_vedenia)
VALUES (2,3);

INSERT INTO Spravuje (id_zamestnanca, id_vedenia)
VALUES (4,5);


INSERT INTO Kategoria (nazov, popis)
VALUES ('Telefony', 'Zariadenie na telefonovanie a momentalne aj na vsetko ostatne');

INSERT INTO Kategoria (nazov, popis)
VALUES ('Prislusenstvo', 'Prislusenstvo k mobilnym telefonom');


INSERT INTO Prida (id_kategorie, id_zamestnanca)
VALUES (1,2);

INSERT INTO Prida (id_kategorie, id_zamestnanca)
VALUES (2,2);


INSERT INTO Produkt (nazov, popis, fotografia, cena, pocet_na_sklade, id_zamestnanca)
VALUES ('Iphone 13 Pro Max', 'Super telefon s 5 kamerami citackou tvare a prstu.',
         'odkaz na fotografiu', 1499.99 , 9, 2 );

INSERT INTO Produkt (nazov, popis, fotografia, cena, pocet_na_sklade, id_zamestnanca)
VALUES ('Samsung S42', 'Je to dobry telefon s vela kamerami androidom',
         'odkaz na fotografiu', 999.99 , 5, 2 );

INSERT INTO Produkt (nazov, popis, fotografia, cena, pocet_na_sklade, id_zamestnanca)
VALUES ( 'Nabijacka na iPhone', 'Nabijacka s lightning konektorom - 1,2m', 'www.odkaz.na.fotografiu.com/nabijacka', 29.19, 11, 4);

INSERT INTO Obsahuje (id_kategorie, id_produktu)
VALUES (1,1);

INSERT INTO Obsahuje (id_kategorie, id_produktu)
VALUES (2,2);

INSERT INTO Obsahuje (id_kategorie, id_produktu)
VALUES (1,3);

INSERT INTO Objednavka (dorucenie_ulica, dorucenie_popisne_cislo, dorucenie_psc, dorucenie_mesto,
                        telefonne_cislo, stav_objednavky, sposob_dopravy, cena, datum_vytvorenia, datum_vybavenia,
                        id_zamestnanca, id_zakaznika)
VALUES ('Listova', 32, 08312, 'Brno', 0943854930, NULL, 3, NULL, CURRENT_TIMESTAMP, NULL, NULL, 1);

INSERT INTO Objednavka (dorucenie_ulica, dorucenie_popisne_cislo, dorucenie_psc, dorucenie_mesto,
                        telefonne_cislo, stav_objednavky, sposob_dopravy, cena, datum_vytvorenia, datum_vybavenia,
                        id_zamestnanca, id_zakaznika)
VALUES ('Kolejni', 32, 08312, 'Brno', 0943835678, NULL, 3, NULL, CURRENT_TIMESTAMP, NULL, NULL, 1);


INSERT INTO Objednavka_produktu (id_objednavky_p, id_produktu_o, mnozstvo)
VALUES (1, 1, 8);

INSERT INTO Objednavka_produktu (id_objednavky_p, id_produktu_o, mnozstvo)
VALUES (1, 2, 2);

INSERT INTO Objednavka_produktu (id_objednavky_p, id_produktu_o, mnozstvo)
VALUES (2, 3, 6);

INSERT INTO Objednavka_produktu (id_objednavky_p, id_produktu_o, mnozstvo)
VALUES (2, 1, 6);


/*
 Prepočítanie ceny objednávok
 */
CALL vypocitaj_novu_cenu(1);

CALL vypocitaj_novu_cenu(2);


INSERT INTO Platba (id_objednavky, faktura_ulica, faktura_popisne_cislo, faktura_psc,
                    faktura_mesto, vyska_platby, sposob_platby, stav_platby)
VALUES (1,'Listova', 32, 08312, 'Brno', NULL, 1, NULL);

INSERT INTO Platba (id_objednavky, faktura_ulica, faktura_popisne_cislo, faktura_psc,
                    faktura_mesto, vyska_platby, sposob_platby, stav_platby)
VALUES (2,'Purkinova', 32, 08312, 'Brno', NULL, 1, NULL);

INSERT INTO Hodnotenie (pocet_hviezd, text_hodnotenia, id_pouzivatela, id_produktu)
VALUES (5,'super produkt ziadne minusy vsetko ide ako ma krasne foti ', 1, 1);

INSERT INTO Hodnotenie (pocet_hviezd, text_hodnotenia, id_pouzivatela, id_produktu)
VALUES (1,' nefunguje ako ma velka nespokojnost ', 2, 1);

INSERT INTO Hodnotenie (pocet_hviezd, text_hodnotenia, id_pouzivatela, id_produktu)
VALUES (0, 'tovar neprišiel', 1, 3);

INSERT INTO Hodnotenie (pocet_hviezd, text_hodnotenia, id_pouzivatela, id_produktu)
VALUES (5, 'dokonalý', 2, 1);

INSERT INTO Hodnotenie (pocet_hviezd, text_hodnotenia, id_pouzivatela, id_produktu)
VALUES (2,'  nevyhovuje mi vysoka cena niesom spokojny ', 1, 1);

INSERT INTO Hodnotenie (pocet_hviezd, text_hodnotenia, id_pouzivatela, id_produktu)
VALUES (4,' pomer cena vykon skvela kupil by som aj na buduce ', 2, 2);

UPDATE Objednavka SET id_zamestnanca = 2
WHERE id_objednavky = 1;


/*
 Porovnanie s indexovaním a bez
 */
EXPLAIN PLAN
SET statement_id = 'hodn' FOR
SELECT Produkt.nazov, Produkt.cena, Avg.pocet
FROM Produkt
INNER JOIN
(
    SELECT Hodnotenie.id_produktu, AVG(pocet_hviezd) pocet
    FROM Hodnotenie
    GROUP BY Hodnotenie.id_produktu
) Avg
ON Produkt.id_produktu = Avg.id_produktu;

SELECT *
  FROM TABLE(DBMS_XPLAN.DISPLAY(NULL, 'hodn','BASIC'));

CREATE INDEX hodnotenie_pocet_i ON Hodnotenie (id_produktu, pocet_hviezd);

EXPLAIN PLAN
SET statement_id = 'hodn' FOR
SELECT Produkt.nazov, Produkt.cena, Avg.pocet
FROM Produkt
INNER JOIN
(
    SELECT Hodnotenie.id_produktu, AVG(pocet_hviezd) pocet
    FROM Hodnotenie
    GROUP BY Hodnotenie.id_produktu
) Avg
ON Produkt.id_produktu = Avg.id_produktu;

SELECT *
  FROM TABLE(DBMS_XPLAN.DISPLAY(NULL, 'hodn','BASIC'));


/*
    Vyberie všetky produkty v nami zvolenej kategórii.
*/
SELECT P.nazov
FROM Obsahuje
INNER JOIN Kategoria K
    ON K.id_kategorie = Obsahuje.id_kategorie
INNER JOIN Produkt P
    ON Obsahuje.id_produktu = P.id_produktu
WHERE K.id_kategorie = 1;

/*
    Vypočíta a uloží cenu vybranej objednávky.
*/
UPDATE Objednavka SET cena = (
    SELECT SUM(Produkt.cena * Objednavka_produktu.mnozstvo)
    FROM Objednavka_produktu
    INNER JOIN Produkt
        ON Objednavka_produktu.id_produktu_o = Produkt.id_produktu
    WHERE Objednavka_produktu.id_objednavky_p = Objednavka.id_objednavky
    GROUP BY Objednavka.id_objednavky
    ) WHERE Objednavka.id_objednavky = 1 ;

/*
    Overenie uloženej ceny objednávky
 */
SELECT cena FROM Objednavka WHERE id_objednavky = 1;

/*
    Vyberie názvy produktov, ceny produktov a ich priemerné hodnotenie
*/
SELECT Produkt.nazov, Produkt.cena, Avg.pocet
FROM Produkt
INNER JOIN
(
    SELECT Hodnotenie.id_produktu, AVG(pocet_hviezd) pocet
    FROM Hodnotenie
    GROUP BY Hodnotenie.id_produktu
) Avg
ON Produkt.id_produktu = Avg.id_produktu;

/*
   Nájde všetkých používateľov ktorí sú registrovaní ako zamestnanci.
*/
SELECT id_pouzivatela FROM Pouzivatel
WHERE id_pouzivatela IN (
    SELECT id_zamestnanca FROM Zamestnanec
    );

/*
    Uloží výšku platby podľa ceny danej objednávky
 */
UPDATE Platba SET vyska_platby = (
    SELECT Objednavka.cena
    FROM Platba
    INNER JOIN Objednavka
        ON Objednavka.id_objednavky = Platba.id_objednavky
        WHERE Platba.id_objednavky = 1
    ) WHERE Platba.id_objednavky = 1;

/*
    Vyberie všetky platby nad 10000
 */
SELECT id_platby, vyska_platby
FROM Platba
WHERE EXISTS(
        SELECT id_objednavky FROM Objednavka
        WHERE Platba.vyska_platby > 10000 );

/*
 Pohľady
 */
CREATE MATERIALIZED VIEW zamestnanci AS
    SELECT id_pouzivatela FROM Pouzivatel
    WHERE id_pouzivatela IN (
        SELECT id_zamestnanca FROM Zamestnanec
        );

CREATE MATERIALIZED VIEW vysoke_platby AS
SELECT id_platby, vyska_platby
FROM Platba
WHERE EXISTS(
        SELECT id_objednavky FROM Objednavka
        WHERE Platba.vyska_platby > 10000 );

GRANT ALL ON zamestnanci TO XMIKUS18;
GRANT ALL ON vysoke_platby TO XMIKUS18;

/*
 Kontrola triggrov a procedúry
 */
SELECT * FROM Objednavka;
SELECT * FROM Platba;
