//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     JMENO PRIJMENI <xlogin00@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author JMENO PRIJMENI
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
	dlList = new DlList;
	dlList->head = NULL;
}

PriorityQueue::~PriorityQueue()
{
	Element_t *next;
	while (dlList->head != NULL){
		next = dlList->head->pNext;
		delete dlList->head;
		dlList->head = next;
	}
	delete dlList;
}

void PriorityQueue::Insert(int value)
{
	Element_t *tmp = dlList->head;
	Element_t *newEl = new Element_t;
	if (tmp == NULL || tmp->value >= value){
		newEl->pNext = tmp;
		newEl->pPrev = NULL;
		dlList->head = newEl;
		if (tmp != NULL) tmp->pPrev = newEl;
		newEl->value = value;
	}
	else {
		while (tmp->pNext != NULL && tmp->pNext->value < value){
			tmp = tmp->pNext;
		}
		if (tmp->pNext == NULL){
			newEl->pNext = NULL;
		}
		else{
			newEl->pNext = tmp->pNext;
			newEl->pNext->pPrev = newEl;
		}
		newEl->pPrev = tmp;
		tmp->pNext = newEl;
		newEl->value = value;
	}
}

bool PriorityQueue::Remove(int value)
{
	Element_t *tmp = dlList->head;
	while (tmp != NULL && tmp->value != value){
		tmp = tmp->pNext;
	}
	if (tmp == NULL)  return false;
	if (tmp->pNext != NULL && tmp->pPrev != NULL){
		tmp->pNext->pPrev = tmp->pPrev;	
		tmp->pPrev->pNext = tmp->pNext;
	} 
	else if (tmp->pPrev != NULL){
		tmp->pPrev->pNext = NULL;	
	}
	else if (tmp->pNext != NULL){
		tmp->pNext->pPrev = NULL;
		dlList->head = tmp->pNext;
	}
	else dlList->head = NULL;
	delete tmp;
	return true;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
 	Element_t *tmp = dlList->head;
	while (tmp != NULL && tmp->value != value){
		tmp = tmp->pNext;
	}
    return tmp;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return dlList->head;
}

/*** Konec souboru tdd_code.cpp ***/
