/* Game of Life assignment for CS106B in Stanford Summer Session
 * Authors: Bruce Yang and Kevin Li
 * Description: The Game of Life is a simulation by British mathematician J. H. Conway in 1970. The game models
 * the life cycle of bacteria using a two-dimensional grid of cells. Given an initial pattern, the game
 * simulates the birth and death of future generations of cells using a set of simple rules.
 * Citations:
 * How to flush the cout buffer in Visual C++
 * https://support.microsoft.com/en-us/help/94227/how-to-flush-the-cout-buffer-in-visual-c
 */
#include <iostream>
#include "console.h"
#include "lifegui.h"
#include "grid.h"
#include "strlib.h"
#include <fstream>
#include "filelib.h"
#include "simpio.h"
using namespace std;

void introduce();
void runGame();
void initializeGame(Grid<string>& grid);
void tick(Grid<string>& grid);
void promptAction(Grid<string>& grid);
void loadAnotherFile();
void animate(int frames, Grid<string>& grid);
void singleCell(const Grid<string>& copy, Grid<string>& grid, int r, int c);
bool isCellOccupied(int r, int c, const Grid<string>& copy);
int getNumOfNeighbors(int r, int c, const Grid<string>& copy);
void killCell(int r, int c, Grid<string>& copy);
void createCell(int r, int c, Grid<string>& grid);
void copyGrid(const Grid<string>& original, Grid<string>& copy);
void printGrid(const Grid<string>& grid);
void showGUI(const Grid<string>& grid);
void updateGUI(const Grid<string>& grid);

int main() {
    introduce();
    LifeGUI::initialize();
    runGame();
    cout<<"Have a nice Life!"<<endl;
    return 0;
}

/*
 * Start running the simulation.
 * This function is called every time a new file is loaded
 * @precondition LifeGUI::initialize() is run to make sure that GUI is started
 */
void runGame() {
    Grid<string> grid(0,0);
    initializeGame(grid);
    printGrid(grid);
    showGUI(grid);
    promptAction(grid);
}

/*
 * Print out introduction.
 */
void introduce(){
    cout << "Welcome to the CS 106B/X Game of Life!" << endl;
    cout << "This program simulates the lifecycle of a bacterial colony." << endl;
    cout << "Cells (X) live and die by the following rules:" << endl;
    cout << "* A cell with 1 or fewer neighbors dies." << endl;
    cout << "* Locations with 2 neighbors remain stable." << endl;
    cout << "* Locations with 3 neighbors will create life." << endl;
    cout << "* A cell with 4 or more neighbors dies.\n" << endl;
}

/*
 * Initialize the grid using the input text file.
 * @param grid the simulation grid
 */
void initializeGame(Grid<string>& grid){
    int row;
    int col;
    ifstream file;
    promptUserForFile(file, "Grid input file name? ");
    string line;
    int count = 0;
    while(getline(file, line)) {
        if (count == 0) {
            row= stringToInteger(line);
        } else if(count == 1) {
            col= stringToInteger(line);
        } else if (count > row + 2) {
            break;
        } else {
            if(grid.size() == 0){
                grid.resize(row, col);
            }
            int gridRow = count - 2;
            for(int gridCol = 0; gridCol < line.length(); gridCol++){
                grid.set(gridRow, gridCol, line.substr(gridCol, 1));
            }
        }
        count++;
    }
    file.close();
}

/*
 * Show the GUI of the simulation.
 * @param grid the simulation grid
 */
void showGUI(const Grid<string>& grid) {
    LifeGUI::resize(grid.numRows(), grid.numCols());
    updateGUI(grid);
    LifeGUI::repaint();
}

/*
 * Update the GUI of the simulation (create/kill cells).
 * @param grid the simulation grid
 */
void updateGUI(const Grid<string>& grid) {
    for (int r = 0; r < grid.numRows(); r++) {
        for (int c = 0; c < grid.numCols(); c++) {
            if (isCellOccupied(r, c, grid)) {
                LifeGUI::fillCell(r, c);
            }
        }
    }
}

/*
 * Prompt the user for actions.
 *
 * Type "t" or press ENTER for running a tick
 * Type "a" for running multiple ticks in an animation
 * Type "q" to quit the program or load a new input file
 *
 * @param grid the simulation grid
 */
void promptAction(Grid<string>& grid) {
    string actionName = toLowerCase(getLine("a)nimate, t)ick, q)uit? "));
    if (actionName == "t" || actionName == "") {
        tick(grid);
    } else if (actionName == "a") {
        int frames = getInteger("How many frames? ");
        if (frames > 0) {
            animate(frames, grid);
        }
    } else if (actionName == "q") {
        loadAnotherFile();
        return;
    } else {
        cout << "Invalid choice; please try again." << endl;
    }
    promptAction(grid);
}

/*
 * Prompt the user to load a new file or end the simulation.
 */
void loadAnotherFile() {
    if (getYesOrNo("Load another file? (y/n) ")) {
        runGame();
    }
}

/*
 * Run an animation a number of frames.
 *
 * Basically tick, wait for 100ms, and repeat these two steps for a number of times.
 *
 * @param frames the number of frames to repeat
 * @param grid   the simulation grid
 */
void animate(int frames, Grid<string>& grid) {
    for (int i = 0; i < frames; i++) {
        tick(grid);
        pause(100);
        clearConsole();
    }
}

/*
 * Advance the simulation one generation forward.
 * @param grid the simulation grid
 */
void tick(Grid<string>& grid) {
    Grid<string> copy(0, 0);
    copyGrid(grid, copy);
    LifeGUI::resize(grid.numRows(), grid.numCols());
    for (int r = 0; r < grid.numRows(); r++) {
        for (int c = 0; c < grid.numCols(); c++) {
            singleCell(copy, grid, r, c);
        }
    }
    LifeGUI::repaint();
    printGrid(grid);
}

/*
 * Test if a single cell should be killed, created, or stay the same.
 * @param copy the copied version of the simulation grid that stays the same
 * @param grid the simulation grid that is modified
 * @param r    the row index of the cell to test
 * @param c    the column index of the cell to test
 */
void singleCell(const Grid<string>& copy, Grid<string>& grid, int r, int c) {
    int numOfNeighbors = getNumOfNeighbors(r, c, copy);
    if (numOfNeighbors <= 1 || numOfNeighbors >= 4) {
        killCell(r, c, grid);
    } else if (numOfNeighbors == 2) {
        if (isCellOccupied(r, c, copy)) {
            LifeGUI::fillCell(r, c);
        }
    } else if (numOfNeighbors == 3) {
        createCell(r, c, grid);
    }
}

/*
 * Create a new cell at (r, c) in the simulation grid.
 * @param r    the row index of the cell to create
 * @param c    the column index of the cell to create
 * @param grid the simulation grid
 */
void createCell(int r, int c, Grid<string>& grid) {
    grid[r][c] = "X";
    LifeGUI::fillCell(r, c);
}

/*
 * Kill the cell at (r, c) in the simulation grid.
 * @param r    the row index of the cell to create
 * @param c    the column index of the cell to create
 * @param grid the simulation grid
 */
void killCell(int r, int c, Grid<string>& grid) {
    grid[r][c] = "-";
}

/*
 * Check if the cell at (r, c) in the simulation grid is occupied or not.
 * @param r    the row index of the cell to create
 * @param c    the column index of the cell to create
 * @param copy the copied version of the simulation grid that stays the same
 */
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
        return true;
    }
    return false;
}

/*
 * Get the number of neighbors of the cell at (r, c) in the grid.
 * @param r    the row index of the cell to create
 * @param c    the column index of the cell to create
 * @param copy the copied version of the simulation grid that stays the same
 */
int getNumOfNeighbors(int r, int c, const Grid<string>& copy) {
    return isCellOccupied(r - 1, c - 1, copy) + isCellOccupied(r - 1, c + 1, copy) +
            isCellOccupied(r + 1, c - 1, copy) + isCellOccupied(r + 1, c + 1, copy) +
            isCellOccupied(r - 1, c, copy) + isCellOccupied(r + 1, c, copy) +
            isCellOccupied(r, c - 1, copy) + isCellOccupied(r, c + 1, copy);
}

/*
 * Copy a grid.
 * @param original the grid to copy from
 * @param copy     the new grid to copy to
 */
void copyGrid(const Grid<string>& original, Grid<string>& copy) {
    copy.resize(original.numRows(), original.numCols());
    for (int r = 0; r < original.numRows(); r ++) {
        for (int c = 0; c < original.numCols(); c ++) {
            copy[r][c] = original[r][c];
        }
    }
}

/*
 * Print a grid in 2D
 * @param grid the grid to print
 */
void printGrid(const Grid<string>& grid) {
    string output = "";
    for (int r = 0; r < grid.numRows(); r ++) {
        for (int c = 0; c < grid.numCols(); c ++) {
            output += grid[r][c];
        }
        output += "\n";
    }
    cout << output << flush;
}
