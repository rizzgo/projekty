//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Richard Seipel <xseipe00@stud.fit.vutbr.cz>
// $Date:       $2019-10-04
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Richard Seipel
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

class TestMatrix: public ::testing::Test{
protected:
	Matrix *testMatrix;
	Matrix *testMatrix2;

	virtual void SetUp(){
		testMatrix = new Matrix();
		testMatrix2 = new Matrix();
	}

	std::vector<std::vector< double > > values = {
		{1, 3, 74, 5, 8, 345},
		{5, 324, 33, 66, 7}
	};
	std::vector<std::vector< double > > values2 = {
		{1, 3, 74, 586, 8, 345},
		{5, 324, 33, 66, 787, 54}
	};
	std::vector<std::vector< double > > values3 = {
		{1, 3, 74, 5, 8},
		{5, 3, 33, 66, 787, 54, 67}
	};
    std::vector<std::vector< double > > values4 = {
		{1}, {74}, {5}, {324}, {33}, {66}, {787}, {54}, {67}
	};
	std::vector<std::vector< double > > values3x3_1 = {
		{1, 54, 43}, 
		{32, 46, 6}, 
		{78, 70, 67}
	};
	std::vector<std::vector< double > > values3x3_2 = {
		{1, 54, 43}, 
		{32, 46, 6}, 
		{78, 70, 59}
	};
	std::vector<std::vector< double > > values3x3_add = {
		{2, 108, 86}, 
		{64, 92, 12}, 
		{156, 140, 126}
	};
	std::vector<std::vector< double > > values3x3_mMul = {
		{5083, 5548, 2904}, 
		{1972, 4264, 2006}, 
		{7544, 12122, 7727}
	};
	std::vector<std::vector< double > > values3x3_sMul = {
		{3, 162, 129}, 
		{96, 138, 18}, 
		{234, 210, 201}
	};
	std::vector<std::vector< double > > values1x1_add = {
		{24}
	};
	std::vector<std::vector< double > > values1x1_mMul = {
		{144}
	};
	std::vector<std::vector< double > > values1x1_sMul = {
		{36}
	};
	std::vector< double > rightVal = {1, 3, 74};
	std::vector< double > rightVal2 = {1, 3};
	std::vector< double > rightVal_36 = {36};

    virtual void TearDown(){
        delete testMatrix;
        delete testMatrix2;
    }
};

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

TEST_F(TestMatrix, Constructor){

	ASSERT_NO_THROW(testMatrix = new Matrix());
	ASSERT_EQ(testMatrix->get(0, 0), 0);

}

TEST_F(TestMatrix, Constructor4x4){

	int iSize = 4;
	int jSize = 4;

	ASSERT_NO_THROW(testMatrix = new Matrix(4, 4));
	for (int i = 0; i < iSize; i++){
		for (int j = 0; j < jSize; j++){
			ASSERT_EQ(testMatrix->get(i, j), 0);
		}
	}

	EXPECT_ANY_THROW(testMatrix = new Matrix(0, 5));
	EXPECT_ANY_THROW(testMatrix = new Matrix(3, 0));
	EXPECT_ANY_THROW(testMatrix = new Matrix(0, 0));

}

TEST_F(TestMatrix, Set){
	testMatrix = new Matrix(8, 7);
	EXPECT_TRUE(testMatrix->set(7, 6, 6.0));
	EXPECT_TRUE(testMatrix->set(0, 0, 6.0));
	EXPECT_FALSE(testMatrix->set(8, 4, 6.0));
	EXPECT_FALSE(testMatrix->set(3, 7, 6.0));
	EXPECT_FALSE(testMatrix->set(-1, -1, 6.0));
	EXPECT_FALSE(testMatrix->set(3, -1, 6.0));
	EXPECT_FALSE(testMatrix->set(-1, 3, 6.0));

}

TEST_F(TestMatrix, SetVector){
	testMatrix = new Matrix(2, 6);
	EXPECT_TRUE(testMatrix->set(values));
	EXPECT_TRUE(testMatrix->set(values2));
	EXPECT_FALSE(testMatrix->set(values3));
	testMatrix = new Matrix(2, 5);
	EXPECT_FALSE(testMatrix->set(values));
	EXPECT_TRUE(testMatrix->set(values3));
	EXPECT_FALSE(testMatrix->set(values4));
}

TEST_F(TestMatrix, Get){
	testMatrix = new Matrix(9, 1);
	testMatrix->set(values4);
	EXPECT_EQ(testMatrix->get(8, 0), 67);
	EXPECT_ANY_THROW(testMatrix->get(1, 9));
}

TEST_F(TestMatrix, Equal){
	testMatrix  = new Matrix(3, 3);
	testMatrix2 = new Matrix(3, 3);
	testMatrix->set(values3x3_1);
	testMatrix2->set(values3x3_1);
	EXPECT_TRUE(testMatrix->operator==(*testMatrix2));
	testMatrix2->set(values3x3_2);
	EXPECT_FALSE(testMatrix->operator==(*testMatrix2));
	testMatrix2 = new Matrix(1, 9);
	testMatrix2->set(values4);
	EXPECT_ANY_THROW(testMatrix->operator==(*testMatrix2));
	testMatrix = new Matrix();
	testMatrix->set(0, 0, 12);
}

TEST_F(TestMatrix, Add){
	testMatrix  = new Matrix(3, 3);
	testMatrix2 = new Matrix(3, 3);
	testMatrix->set(values3x3_1);
	testMatrix2->set(values3x3_2);
	EXPECT_NO_THROW(*testMatrix = testMatrix->operator+(*testMatrix2));
	testMatrix2->set(values3x3_add);
	EXPECT_TRUE(testMatrix->operator==(*testMatrix2));
	testMatrix2 = new Matrix(1, 9);
	testMatrix2->set(values4);
	EXPECT_ANY_THROW(testMatrix->operator+(*testMatrix2));
	testMatrix = new Matrix();
	testMatrix2 = new Matrix();
	testMatrix->set(0, 0, 12);
	EXPECT_NO_THROW(*testMatrix = testMatrix->operator+(*testMatrix));
	testMatrix2->set(values1x1_add);
	EXPECT_TRUE(testMatrix->operator==(*testMatrix2));

}

TEST_F(TestMatrix, MatrixMultiply){
	testMatrix  = new Matrix(3, 3);
	testMatrix2 = new Matrix(3, 3);
	testMatrix->set(values3x3_1);
	testMatrix2->set(values3x3_2);
	EXPECT_NO_THROW(*testMatrix = testMatrix->operator*(*testMatrix2));
	testMatrix2->set(values3x3_mMul);
	EXPECT_TRUE(testMatrix->operator==(*testMatrix2));
	testMatrix2 = new Matrix(1, 9);
	testMatrix2->set(values4);
	EXPECT_ANY_THROW(testMatrix->operator*(*testMatrix2));	
	testMatrix = new Matrix();
	testMatrix2 = new Matrix();
	testMatrix->set(0, 0, 12);
	EXPECT_NO_THROW(*testMatrix = testMatrix->operator*(*testMatrix));
	testMatrix2->set(values1x1_mMul);
	EXPECT_TRUE(testMatrix->operator==(*testMatrix2));
}

TEST_F(TestMatrix, ScalarMultiply){
	testMatrix  = new Matrix(3, 3);
	testMatrix2 = new Matrix(3, 3);
	testMatrix->set(values3x3_1);
	testMatrix2->set(values3x3_2);
	EXPECT_NO_THROW(*testMatrix = testMatrix->operator*(3));
	testMatrix2->set(values3x3_sMul);
	EXPECT_TRUE(testMatrix->operator==(*testMatrix2));
	testMatrix2 = new Matrix(1, 9);
	testMatrix2->set(values4);
	EXPECT_ANY_THROW(testMatrix->operator*(*testMatrix2));
	testMatrix = new Matrix();
	testMatrix2 = new Matrix();
	testMatrix->set(0, 0, 12);
	EXPECT_NO_THROW(*testMatrix = testMatrix->operator*(3));
	testMatrix2->set(values1x1_sMul);
	EXPECT_TRUE(testMatrix->operator==(*testMatrix2));
}

TEST_F(TestMatrix, SolveEquation){
	testMatrix  = new Matrix(3, 3);
	testMatrix->set(values3x3_1);
	EXPECT_NO_THROW(testMatrix->solveEquation(rightVal));
	EXPECT_ANY_THROW(testMatrix->solveEquation(rightVal2));
	testMatrix = new Matrix(2, 6);
	testMatrix->set(values2);
	EXPECT_ANY_THROW(testMatrix->solveEquation(rightVal2));
	testMatrix = new Matrix();
	testMatrix->set(0, 0, 12);
	std::vector< double > res = testMatrix->solveEquation(rightVal_36);
	EXPECT_DOUBLE_EQ(res[0], 3);
}

/*** Konec souboru white_box_tests.cpp ***/
