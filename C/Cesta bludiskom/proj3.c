/**
* IZP Treti projekt
* Autor: Richard Seipel, xseipe00
* Datum: 09.12.2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//definition of map
typedef struct{
	int rows;
	int cols;
	unsigned char *cells;
} Map;

//direction definition
enum Direction {Left, Right, Middle};

//
//map alocation
Map mapCtor(FILE *f){
	Map map;
	fscanf(f, "%d", &map.rows);
	fgetc(f);
	fscanf(f, "%d", &map.cols);
	map.cells = malloc(map.rows * map.cols * sizeof(unsigned char) + 1);
	return map;
}

//
//loading map
//returns if map size is correct
bool mapLoad(Map *map, FILE * f){
	int mapSize = 0;
	int mapAllocated = map->rows * map->cols;
	char ch;
	unsigned char *pointer = map->cells;
	while (EOF != (ch = fgetc(f))){
		if ('0' <= ch && ch <= '7'){
			if (mapSize < mapAllocated){
				pointer = pointer + sizeof(unsigned char);
				*pointer = (unsigned char)ch;
				mapSize++;
			}
			else return false;
		}
	}
	return mapSize == mapAllocated;
}

//
//map dealocation
void mapDtor(Map *map){
	free(map->cells);
	map = NULL;
}

//
//if is middle down
bool isRoof(int r, int c){
	return (r+c)%2;
}

//
//if is border on cell in direction
bool isBorder(Map *map, int r, int c , enum Direction dir){
	int cellValue = (map->cells[(r-1) * map->cols + c]) - '0'; // char - '0' changes char to int
	int mask = 1<<dir; //2^dir
	return cellValue & mask;
}

//
//one turn left or right
//adds or subtracts 1 from direction (0-2)
int changeDir(int r, int c, bool right, enum Direction dir){
	int leftright = -1;
	int orientation = 1;
	if (right) leftright = 1;
	if (isRoof(r, c)) orientation = -1;
	return (3 + dir + leftright * orientation)%3;
}

//
//set new coordinates in chosen direction
void newCoordinates(int *r, int *c, enum Direction dir){
	if (dir == 0) (*c)--; //move left
	else if (dir == 1) (*c)++; //move right
	else if (dir == 2){
		if (isRoof(*r, *c)) (*r)++; //move down
		else (*r)--; //move up
	}
}

//
//set start direction
//returns -1 if start coordinations are out of range
int start(Map *map, int r, int c, int leftright){
	enum Direction dir;
	int startCellValue = (map->cells[(r-1) * map->cols + c]) - '0'; // char to int
	if (!(1 <= r && r <= map->rows && 1 <= c && c <= map->cols) || startCellValue == 7){
		fprintf(stderr, "Invalid cell.\n");
		return -1;
	}
	else if (c == 1){
		if (r == 1){ // |'
			if (leftright) dir = Left;
			else dir = Middle;
		}
		else if (r == map->rows){ // |_
			if (leftright) dir = Middle;
			else dir = Left;
		}
		else dir = Left;
	}
	else if (c == map->cols){
		if (r == 1){ // '|
			if (leftright) dir = Middle;
			else dir = Right;
		}
		else if (r == map->rows){ // _|
			if (leftright) dir = Right;
			else dir = Middle;
		}
		else dir = Right;
	}
	else dir = Middle;
	return dir;
}

//
//does move and retuns direction from which it came
int move(Map *map, int *r, int *c, int leftright, enum Direction dir){
	do {
		dir = changeDir(*r, *c, leftright, dir);
	}
	while (isBorder(map, *r, *c, dir)); //turn to direction where isn't border
	newCoordinates(r, c, dir); //move 
	if (dir == Right) dir = Left; //when goes right comes form left
	else if (dir == Left) dir = Right; //when goes left comes from right
	return dir;
}

//
//arguments control
bool checkArgs(int argc,char *argv){
	//first argument not empty
	if (argc < 2){
		fprintf(stderr, "No arguments.\n");
		return true;
	}
	//help
	else if (!strcmp(argv, "--help")){ 
		if (argc > 2) {
			fprintf(stderr, "A lot of arguments.\n");
			return true;
		}
		return false;
	}
	//test
	else if (!strcmp(argv, "--test")){
		if (argc < 3){
			fprintf(stderr, "Few arguments.\n");
			return true;
		}
		else if (argc > 3) {
			fprintf(stderr, "A lot of arguments.\n");
			return true;
		}
	}
	//leftright
	else if (argc < 5){
		fprintf(stderr, "Few arguments.\n");
		return true;
	}
	else if (argc > 5) {
		fprintf(stderr, "A lot of arguments.\n");
		return true;
	}
return false;
}

//
//deep test
int test(char *fileName){
	FILE *f = fopen(fileName, "r");
	if (!f) return -1;
	Map map = mapCtor(f);
	if (!mapLoad(&map, f)){ //if incorrect size 
		fclose(f);
		mapDtor(&map);
		return 0; 
	}
	fclose(f);

	for (int r = 1; r < map.rows; r++){
		for (int c = 1; c < map.cols; c++){
			if(isBorder(&map, r, c, Right) != isBorder(&map, r, c + 1, Left)){
				mapDtor(&map);
				return 0;
			}
			if (isRoof(r, c)){
				if (isBorder(&map, r, c, Middle) != isBorder(&map, r + 1, c, Middle)){
					mapDtor(&map);
					return 0;
				}
			}
		}
		//last column
		if (isRoof(r, map.cols)){
			if (isBorder(&map, r, map.cols, Middle) != isBorder(&map, r + 1, map.cols, Middle)){
				mapDtor(&map);
				return 0;
			}
		}
	}
	//last row
	for (int c = 1; c < map.cols; c++){
		if(isBorder(&map, map.rows, c, Right) != isBorder(&map, map.rows, c + 1, Left)){
			mapDtor(&map);
			return 0;
		}
	}
	//map deallocation
	mapDtor(&map);
	return 1;
}

//
//main
int main(int argc, char *argv[]){
	//variables
	bool leftright;
	enum Direction dir;

	//arguments control
	if (checkArgs(argc, argv[1])) return -1;

	//--help
	if (!strcmp(argv[1], "--help")){
		printf("\n%s\n%s\n%s\n%s\n\n%s\n\n",
			"--help -> you are here",
			"--test <filename.txt> -> controls map validity",
			"--lpath <start row> <start column> <filename.txt> -> goes around left borders",
			"--rpath <start row> <start column> <filename.txt> -> goes around right borders",
			"Support on: xseipe00@stud.fit.vutbr.cz");
		return 0;
	}
	//--test
	if (!strcmp(argv[1], "--test")){
		int testStatus = test(argv[2]);
		if (testStatus == 1){
			printf("%s", "Valid\n");
		}
		else if (testStatus == 0){
			printf("%s", "Invalid\n");
		}
		else {
			fprintf(stderr, "Incorrect filename.\n");
			return -1;
		}
		return 0;
	}
	else if (test(argv[4]) == 0){
		fprintf(stderr, "Invalid map.\n");
		return -1;
	}

	//--leftright
	if (!strcmp(argv[1], "--rpath")){
		leftright = true;
	}
	else if (!strcmp(argv[1], "--lpath")){
		leftright = false;
	}
	else {
		fprintf(stderr, "Invalid argument.\n");
		return -1;
	}

	//rows and columns
	char *endptr1;
	char *endptr2;
	int r = strtol(argv[2], &endptr1, 10);
	int c = strtol(argv[3], &endptr2, 10);
	if (*endptr1 != '\0' || *endptr2 != '\0'){
		fprintf(stderr, "Row or column is not a number.\n");
		return -1;
	}

	//map loading
	FILE *f = fopen(argv[4], "r");
	if (!f){
		fprintf(stderr, "Incorrect filename.\n");
		return -1;
	}
	Map map = mapCtor(f);
	mapLoad(&map, f);
	fclose(f);

	//do walk
	int d;
	if ((d = start(&map, r, c, leftright)) == -1){ //invalid start
		mapDtor(&map);
		return -1;	
	}
	dir = d; // if r and c are in range of map, wall on direction dir is start 
	while (1 <= r && r <= map.rows && 1 <= c && c <= map.cols){
		printf("%d,%d\n", r, c);
		dir = move(&map, &r, &c, leftright, dir);
	}

	//map dealocation
	mapDtor(&map);
	return 0;
}