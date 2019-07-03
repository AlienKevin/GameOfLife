/* Game of Life assignment for CS106B in Stanford Summer Session
 * Extra features:
 *  - Add random world generator
 *  - Make tick function detect stable world to stop extra calculations and animations
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
#include "random.h"
using namespace std;

const int MAX_ROW_LENGTH = 50;
const int MAX_COLUMN_LENGTH = 50;
const int MAX_STATS_TRIALS = 300;

void introduce();
void runGame();
bool promptForInput(ifstream& file);
void initializeGame(Grid<string>& grid);
int findDuplicatedGrid(Grid<string> grid, Vector<Grid<string>> grids);
int numberOfLiveCells(Grid<string> grid);
void statistics(const Grid<string>& grid);
bool tick(Grid<string>& grid, bool isPrintingGrid = true);
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
    if (promptForInput(file)) { // a filename is inputed
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
    } else { // generate a random world
        // randomly generate grid's row and column length
        row = randomInteger(3, MAX_ROW_LENGTH);
        col = randomInteger(3, MAX_COLUMN_LENGTH);

        grid.resize(row, col);

        // randomly fill the grid
        for (int r = 0; r < row; r++) {
            for (int c = 0; c < col; c++) {
                bool createNewCell = randomBool();
                if (createNewCell) {
                    grid[r][c] = "X";
                } else {
                    grid[r][c] = "-";
                }
            }
        }
    }
}

/*
 * Prompt the user for input file name and the user can type "random" to generate a random grid.
 * @param file the input file variable to assign to if user input is not "random"
 * @return true if the user inputted a valid file name, false if user inputted "random"
 */
bool promptForInput(ifstream& file) {
    string filename = "";
    while (!fileExists(filename) && filename != "random") {
        filename = getLine("Grid input file name? (or random)");
    }
    if (filename == "random") {
        return false;
    } else {
        openFile(file, filename);
        return true;
    }
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
    string actionName = toLowerCase(getLine("a)nimate, t)ick, s)tatistics, q)uit? "));
    if (actionName == "t" || actionName == "") {
        bool keepRunning = tick(grid);
        if (!keepRunning) { // the grid is stable
            cout << "No grid is displayed because this world is stable." << endl;
        }
    } else if (actionName == "a") {
        int frames = getInteger("How many frames? ");
        if (frames > 0) {
            animate(frames, grid);
        }
    } else if (actionName == "s") {
        statistics(grid);
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

void statistics(const Grid<string>& grid) {
    Grid<string> copy;
    copyGrid(grid, copy);
    Vector<Grid<string>> grids;
    int lastGenerationIndex = MAX_STATS_TRIALS - 1;
    int duplicatedIndex = -1;
    for (int i = 0; i < MAX_STATS_TRIALS; i++) {
        tick(copy, false);
        duplicatedIndex = findDuplicatedGrid(copy, grids);
        if (duplicatedIndex >= 0) {
            lastGenerationIndex = i - 1;
            break;
        } else {
            Grid<string> copyOfCopy;
            copyGrid(copy, copyOfCopy);
            grids.add(copyOfCopy);
        }
    }
    if (duplicatedIndex >= 0) {
        if (duplicatedIndex == lastGenerationIndex) {
            if (numberOfLiveCells(copy) == 0) { // all cells are dead
                cout << "All cells are dead!" << endl;
            } else {
                cout << "Stable grid found at frame " << duplicatedIndex << "." << endl;
            }
        } else {
            cout << "Pattern found between frames " << duplicatedIndex << " and " <<
                    lastGenerationIndex << "." << endl;
        }
        string actionName = toLowerCase(getLine("p)rint or a)nimate the pattern frames? (type no to skip) "));
        if (startsWith(actionName, "p")) {
            for (int j = duplicatedIndex; j < lastGenerationIndex + 1; j++) {
                printGrid(grids[j]);
                cout << endl;
            }
        } else if (startsWith(actionName, "a")) {
            for (int j = duplicatedIndex; j < lastGenerationIndex + 1; j++) {
                printGrid(grids[j]);
                pause(100);
                clearConsole();
            }
        } else if (startsWith(actionName, "n")) {

        }
   } else {
        cout << "No pattern found. Simulation ends at " << lastGenerationIndex << " frames." << endl;
        if (getYesOrNo("Print the last frame? (y/n) ")) {
            printGrid(copy);
        }
    }
}

int numberOfLiveCells(Grid<string> grid) {
    int numOfLiveCells = 0;
    for (string cell : grid) {
        if (cell == "X") {
            numOfLiveCells ++;
        }
    }
    return numOfLiveCells;
}

int findDuplicatedGrid(Grid<string> grid, Vector<Grid<string>> grids) {
    for (int i = 0; i < grids.size(); i++) {
        if (grids[i] == grid) {
            return i;
        }
    }
    return -1;
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
        bool keepRunning = tick(grid);
        if (keepRunning) { // the grid is changing (not stable)
            pause(100);
            clearConsole();
        } else { // the grid is stable
            cout << "animation ended at " << i << " frames because the world is stable." << endl;
            break;
        }
    }
}

/*
 * Advance the simulation one generation forward.
 * @param  grid the simulation grid
 * @return true if the grid changes after this generation and false if the grid is stable
 */
bool tick(Grid<string>& grid, bool isPrintingGrid) {
    Grid<string> copy(0, 0);
    copyGrid(grid, copy);
    for (int r = 0; r < grid.numRows(); r++) {
        for (int c = 0; c < grid.numCols(); c++) {
            singleCell(copy, grid, r, c);
        }
    }
    if (grid == copy) { // no change after this generation
        return false;
    } else {
        if (isPrintingGrid) {
            printGrid(grid);
        }
        return true;
    }
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
        // the cell stays the same
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
    LifeGUI::resize(grid.numRows(), grid.numCols());
    string output = "";
    for (int r = 0; r < grid.numRows(); r ++) {
        for (int c = 0; c < grid.numCols(); c ++) {
            output += grid[r][c];
            if (isCellOccupied(r, c, grid)) {
                LifeGUI::fillCell(r, c);
            }
        }
        output += "\n";
    }
    cout << output << flush;
    LifeGUI::repaint();
}
