# Knapsack problem 0-1

## Spustenie

Program je možné spustiť s parametrami: 
- **"-i"** - info, 
- **"-b"** - brute force, 
- **"-o"** - optimalizačná metóda.

V tvare:
  - `flp22-fun <mód> <vstupný súbor>`.

## Vstup

Vstupný súbor má tvar:
```
Knapsack {
    maxWeight: 46
    minCost: 324
    items: [
        Item {
            weight: 36
            cost: 3
        }
        Item {
            weight: 43
            cost: 1129
        }
        Item {
            weight: 202
            cost: 94
        }
        Item {
            weight: 149
            cost: 2084
        }
    ]
}
```

## Detaily implemetácie

V projekte je implementovaný parser, ktorý umožňuje zmenu poradia váh a cien. Ďalej obsahuje rekurzívne "brute force" riešenie, ktoré ďalej nepreskumáva kombinácie prekračujúce maximálnu povolenú váhu. Ako optimalizačnú metódu som použil genetický algoritmus. 

## Obsah repozitára

- doc
  - README.md
  - test-description.txt
- src
  - BruteForce.hs
  - GeneticAlgorithm.hs
  - Main.hs
  - Parser.hs
  - Types.hs
- test
  - test1.in
  - test2.in
  - test4.in
  - test10.in
  - test12.in
  - test13.in
  - test14.in
  - test17.in
  - test22.in
  - test33.in
  - test36.in
  - test37.in
  - test43.in
  - test45.in
- Makefile
- runtests.py
