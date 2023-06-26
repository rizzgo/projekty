
# Sniffer paketov


Program zachytáva pakety na určenom rozhraní, zisťuje a vypisuje čas ich odoslania, 
adresu odosielateľa a prijímateľa, ich dĺžku a obsah. Zachytávané pakety je možné 
filtrovať alebo obmedziť ich počet.


Odovzdané súbory:
```
ipk-sniffer.h
ipk-sniffer.cpp
Makefile
manual.pdf
README.txt
```

Formát spustenia:
```
./ipk-sniffer [-i rozhranie | --interface rozhranie] {-p ­­port} {[--tcp|-t] [--udp|-u] [--arp] [--icmp] } {-n num}
```
`[]` - povinné, `{}` - voliteľné



Príklady spustenia:
```
sudo ./ipk-sniffer -i wlp0s20f3 -p 8009 -t
2021-04-25T18:42:42.874291+02:00 192.168.0.181 : 38202 > 192.168.0.174 : 8009, length 176 bytes

0x0000: cc 98 8b 5f 32 5a d4 3b 04 57 92 25 08 00 45 00 ..._2Z.;.W.%..E.
0x0010: 00 a2 c5 dd 40 00 40 06 f1 c4 c0 a8 00 b5 c0 a8 ....@.@.........
0x0020: 00 ae 95 3a 1f 49 3b 0a f2 6d 91 98 f8 ae 80 18 ...:.I;..m......
0x0030: 01 f5 83 48 00 00 01 01 08 0a 25 3a 62 fe 00 7e ...H......%:b..~
0x0040: c3 62 17 03 03 00 69 ea 53 93 57 37 c7 97 47 51 .b....i.S.W7..GQ
0x0050: 41 0f 16 1a de c1 46 8c ba 75 e7 44 a1 95 e4 1d A.....F..u.D....
0x0060: 53 51 fb bb 9f 58 83 dd de 71 2d 28 ca fa 82 44 SQ...X...q-(...D
0x0070: be 73 04 e9 c6 4d 78 61 62 15 5b c0 9d 3f 1e 49 .s...Mxab.[..?.I
0x0080: c7 c5 a3 ca 59 dd 11 59 4d a4 68 d0 b0 78 62 7f ....Y..YM.h..xb.
0x0090: e6 05 dc 04 cd ef 0a 0b 47 73 76 c8 26 ef 09 01 ........Gsv.&...
0x00a0: 99 40 ab da 90 0b 5e 93 5c 44 69 40 30 6f 90 c9 .@....^.\Di@0o..


sudo ./ipk-sniffer --interface wlp0s20f3 --icmp
2021-04-25T18:44:23.978197+02:00 192.168.0.181 > 192.168.0.1, length 120 bytes

0x0000: 60 32 b1 40 7d b5 d4 3b 04 57 92 25 08 00 45 c0 `2.@}..;.W.%..E.
0x0010: 00 6a ad d9 00 00 40 01 49 f3 c0 a8 00 b5 c0 a8 .j....@.I.......
0x0020: 00 01 03 03 7f 4f 00 00 00 00 45 00 00 4e 00 00 .....O....E..N..
0x0030: 40 00 40 11 b8 98 c0 a8 00 01 c0 a8 00 b5 bd 3b @.@............;
0x0040: 00 89 00 3a 1f cb 61 aa 00 00 00 01 00 00 00 00 ...:..a.........
0x0050: 00 00 20 43 4b 41 41 41 41 41 41 41 41 41 41 41 .. CKAAAAAAAAAAA
0x0060: 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 41 AAAAAAAAAAAAAAAA
0x0070: 41 41 41 00 00 21 00 01 AAA..!..


sudo ./ipk-sniffer -i wlp0s20f3 -n 2 --arp 
2021-04-25T18:46:33.119001+02:00 cc:98:8b:5f:32:5a > d4:3b:4:57:92:25, length 60 bytes

0x0000: d4 3b 04 57 92 25 cc 98 8b 5f 32 5a 08 06 00 01 .;.W.%..._2Z....
0x0010: 08 00 06 04 00 01 cc 98 8b 5f 32 5a c0 a8 00 ae ........._2Z....
0x0020: 00 00 00 00 00 00 c0 a8 00 b5 00 72 65 4f 72 64 ...........reOrd
0x0030: 65 72 4c 69 73 74 42 79 49 64 00 00 erListById..

2021-04-25T18:46:33.119020+02:00 d4:3b:4:57:92:25 > cc:98:8b:5f:32:5a, length 42 bytes

0x0000: cc 98 8b 5f 32 5a d4 3b 04 57 92 25 08 06 00 01 ..._2Z.;.W.%....
0x0010: 08 00 06 04 00 02 d4 3b 04 57 92 25 c0 a8 00 b5 .......;.W.%....
0x0020: cc 98 8b 5f 32 5a c0 a8 00 ae ..._2Z....
```
