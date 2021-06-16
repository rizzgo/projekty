//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     JMENO PRIJMENI <xlogin00@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author JMENO PRIJMENI
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

class EmptyTree: public ::testing::Test{
protected:
    BinaryTree *emptyTree;
    
    virtual void SetUp(){
        emptyTree = new BinaryTree();
    }
    
    virtual void TearDown(){
        delete emptyTree;
    }
};

class NonEmptyTree: public ::testing::Test{
protected:
    BinaryTree *nonEmptyTree;
    
    virtual void SetUp(){
        nonEmptyTree = new BinaryTree();
        int nodesSize = 7;
		int nodes[7] = {53, 48, 26, 34, 10, 79, 83};
		for (int i = 0; i < nodesSize; i++){
			nonEmptyTree->InsertNode(nodes[i]);
		}
    }
    
    virtual void TearDown(){
        delete nonEmptyTree;
    }
};

class TreeAxioms: public ::testing::Test{
protected:
    BinaryTree *treeAxioms;
    
    virtual void SetUp(){
        treeAxioms = new BinaryTree();
        int nodesSize = 7;
		int nodes[7] = {53, 48, 26, 34, 10, 79, 83};
		for (int i = 0; i < nodesSize; i++){
			treeAxioms->InsertNode(nodes[i]);
		}
    }
    
    virtual void TearDown(){
        delete treeAxioms;
    }
};

Node_t *nullNode = NULL;

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

TEST_F(EmptyTree, InsertNode){

	ASSERT_EQ(emptyTree->GetRoot(), nullNode);

	std::pair<bool, Node_t *> key1 = emptyTree->InsertNode(53);
	std::pair<bool, Node_t *> key2 = emptyTree->InsertNode(53);
	std::pair<bool, Node_t *> key3 = emptyTree->InsertNode(48);

	EXPECT_TRUE(key1.first);
	EXPECT_EQ(key1.second->key, 53);
	EXPECT_FALSE(key2.first);
	EXPECT_EQ(key2.second->key, 53);
	EXPECT_TRUE(key3.first);
	EXPECT_EQ(key3.second->key, 48);
}

TEST_F(EmptyTree, DeleteNode){
	
	emptyTree->InsertNode(26);
	bool delKey1 = emptyTree->DeleteNode(26);
	bool delKey2 = emptyTree->DeleteNode(34);
	
	EXPECT_TRUE(delKey1);
	EXPECT_FALSE(delKey2);

}

TEST_F(EmptyTree, FindNode){

	emptyTree->InsertNode(10);
	Node_t *findKey1 = emptyTree->FindNode(10);
	Node_t *findKey2 = emptyTree->FindNode(79);

	EXPECT_NE(findKey1, nullNode);
	EXPECT_EQ(findKey2, nullNode);

}


TEST_F(NonEmptyTree, InsertNode){

	ASSERT_NE(nonEmptyTree->GetRoot(), nullNode);

	std::pair<bool, Node_t *> key1 = nonEmptyTree->InsertNode(53);
	std::pair<bool, Node_t *> key2 = nonEmptyTree->InsertNode(49);

	EXPECT_FALSE(key1.first);
	EXPECT_EQ(key1.second->key, 53);
	EXPECT_TRUE(key2.first);
	EXPECT_EQ(key2.second->key, 49);
}

TEST_F(NonEmptyTree, DeleteNode){
	
	bool delKey1 = nonEmptyTree->DeleteNode(26);
	nonEmptyTree->DeleteNode(37);
	bool delKey2 = nonEmptyTree->DeleteNode(37);

	EXPECT_TRUE(delKey1);
	EXPECT_FALSE(delKey2);

}

TEST_F(NonEmptyTree, FindNode){

	Node_t *findKey1 = nonEmptyTree->FindNode(10);
	nonEmptyTree->DeleteNode(79);
	Node_t *findKey2 = nonEmptyTree->FindNode(79);

	EXPECT_NE(findKey1, nullNode);
	EXPECT_EQ(findKey2, nullNode);

}

TEST_F(TreeAxioms, Axiom1){
	
	std::vector<Node_t *> leafs;
	treeAxioms->GetLeafNodes(leafs);
	Color_t black = BLACK;

	for (int i = 0; i < leafs.size(); i++){
		EXPECT_EQ(leafs[i]->color, black);
	}

}

TEST_F(TreeAxioms, Axiom2){
	
	std::vector<Node_t *> nonLeafs;
	treeAxioms->GetNonLeafNodes(nonLeafs);
	Color_t black = BLACK;
	Color_t red = RED;

	for (int i = 0; i < nonLeafs.size(); i++){
		if (nonLeafs[i]->color == red){
			EXPECT_EQ(nonLeafs[i]->pLeft->color, black);
			EXPECT_EQ(nonLeafs[i]->pRight->color, black);
		}
	}
}

TEST_F(TreeAxioms, Axiom3){

	std::vector<Node_t *> leafs;
	treeAxioms->GetLeafNodes(leafs);
	Color_t black = BLACK;
	Node_t *parent;
	int buffer = 0;

	parent = leafs[0];
	while (parent != NULL){
		if (parent->color == black){
			buffer ++;
		}
		parent = parent->pParent;
	}
	int count = buffer;

	for (int i = 0; i < leafs.size(); i++){
		parent = leafs[i];
		buffer = 0;
		while (parent != NULL){
			if (parent->color == black){
				buffer ++;
			}
			parent = parent->pParent;
		}
		EXPECT_EQ(count, buffer);
	}

}
/*** Konec souboru black_box_tests.cpp ***/
