// This is the CPP file you will edit and turn in. (Remove this comment!)

#include <iostream>
#include "console.h"
#include "lifegui.h"
#include "grid.h"
#include "strlib.h"
#include <fstream>
#include "filelib.h"
using namespace std;

void introduce();
void initialize(Grid<string>& grid);
void singleCell(Grid<string> copy, Grid<string>& original, int r, int c);
bool isCellOccupied(int r, int c, Grid<string> copy);

void introduce(){
    cout<<"Welcome to the CS 106B/X Game of Life!"<<endl;
    cout<<"This program simulates the lifecycle of a bacterial colony."<<endl;
    cout<<"Cells (X) live and die by the following rules:"<<endl;
    cout<<"* A cell with 1 or fewer neighbors dies."<<endl;
    cout<<"* Locations with 2 neighbors remain stable."<<endl;
    cout<<"* Locations with 3 neighbors will create life."<<endl;
    cout<<"* A cell with 4 or more neighbors dies."<<endl;
}

void initialize(Grid<string>& grid){
    int row;
    int col;
    ifstream file;
    promptUserForFile(file, "Grid input file name? ");
    string line;
    int count=0;
    while(getline(file,line)){
        if (count == 0) {
            row= stringToInteger(line);
        } else if(count == 1) {
            col= stringToInteger(line);
        } else if (count > row + 2){
            break;
        } else {
            if(grid.size()==0){
                grid.resize(row,col);}
                int gridRow = count - 2;
                for(int gridCol=0; gridCol<line.length();gridCol++){
                    grid.set(gridRow, gridCol, line.substr(gridCol,1));
                }
        }

        count++;
    }

}

void singleCell(const Grid<string>& copy, Grid<string>& original, int r, int c){
//    int endRow = original.numRows() - 1;
//    int endCol= original.numCols() - 1;
//    if (row == 0 && col == 0) { // top left corner
//        numOfNeighbors();
//    } else if (row == 0 && col == endCol) { // top right corner

//    } else if (row == endRow && col == 0) { // bottom left corner

//    } else if (row == endRow && col == endCol) { // bottom right corner

//    } else { // in the middle somewhere

//    }
    int numOfNeighbors = numOfNeighbors(r, c, copy);

}

bool isCellOccupied(int r, int c, Grid<string>& copy) {
    int endRow = copy.numRows() - 1;
    int endCol = copy.numCols() - 1;

    if (r == -1) {
        r = endRow;
    } else if (r == endRow + 1) {
        r = 0;
    }

    if (c == -1) {
        c = endCol;
    } else if (c == endCol + 1) {
        c = 0;
    }

    string cell = copy[r][c];

    if (cell == "X") {
        return 1;
    }
    return 0;
}

int numOfNeighbors(int r, int c, const Grid<string>& copy) {
    return isCellOccupied(r - 1, c - 1, copy) + isCellOccupied(r - 1, c + 1, copy) +
            isCellOccupied(r + 1, c - 1, copy) + isCellOccupied(r + 1, c + 1, copy) +
            isCellOccupied(r - 1, c, copy) + isCellOccupied(r + 1, c, copy) +
            isCellOccupied(r, c - 1, copy) + isCellOccupied(r, c + 1, copy);
}

//string dequoting(Grid<string> grid){
//    string result = "";
//    for(string n: grid){
//        result.append(n);
//    }
//    return result;
//}

int main() {
    introduce();
    Grid<string> grid(0,0);
    initialize(grid);
    //cout << dequoting(grid) << endl;
    cout<<"Have a nice Life!"<<endl;
    return 0;
}
