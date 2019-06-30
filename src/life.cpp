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
bool isCellOccupied(int r, int c, const Grid<string>& copy);
void killCell(int r, int c, Grid<string>& copy);
void createCell(int r, int c, Grid<string>& grid);

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

void singleCell(const Grid<string>& copy, Grid<string>& grid, int r, int c){
    int numOfNeighbors = numOfNeighbors(r, c, copy);
    // 0 - 1 nieghbor: die
    if (numOfNeighbors <= 1) {
        killCell(r, c, grid);
    } else if (numOfNeighbors == 3) {
        createCell(r, c, grid);
    }
}

void createCell(int r, int c, Grid<string>& grid) {
    grid[r][c] = "X";
}

void killCell(int r, int c, Grid<string>& grid) {
    grid[r][c] = "-";
}

bool isCellOccupied(int r, int c, const Grid<string>& copy) {
    int endRow = copy.numRows() - 1;
    int endCol = copy.numCols() - 1;

    // wraps around in both dimensions
    // top-most and bottom-most rows are neighbors
    if (r == -1) {
        r = endRow;
    } else if (r == endRow + 1) {
        r = 0;
    }

    // left-most and right-most columns are neighbors
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

int main() {
    introduce();
    Grid<string> grid(0,0);
    initialize(grid);
    //cout << dequoting(grid) << endl;
    cout<<"Have a nice Life!"<<endl;
    return 0;
}
