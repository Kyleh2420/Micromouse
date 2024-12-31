#include <iostream>
#include <string>
#include "API.h"
#include <queue>

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define LEFT 0
#define FRONT 1
#define RIGHT 2

using namespace std;
/*
    Micromouse Rules dictate that the goal of the maze is a 2x2 square in the middle of the maze. Thus, that will be our goal.


    //To store the walls of the maze, we use an integer array called "mazeWalls". The integer stores NESW in the lower 4 bits, where a 1 indicates a wall, and a 0 indicates no wall.

    //We also use a weight graph to store the number of cells it'll take to get to the center, an integer array called "mazeWeight".
    //This will be used to calcualte the flood fill algorithm every run. 

    A standard run looks like this:
    1. Detect Walls
    2. Store walls to mazeWalls array
    3. Perform flood fill to recalculate the mazeWeights array. 
    4. Select neighbor with smallest weight
        a. Loop though neighbors (NESW)
        b. Return the cell with the lowest weight
    5. Orient towards the lowest weighted call
    6. Move Forward

*/

//Global Variables Used
#define MAZESIZE 16   //Size of the maze
int currentCol = 0;
int currentRow = 0;
int orient = 0;    //0 = N, 1 = E, 2 = S, 3 = W
int startLorR = 0;      //0 if starting on the left, 1 if starting on the right
int goalRow = MAZESIZE/2;   //On a standard MM board, this equals 8
int goalCol = MAZESIZE/2-1; //On a standard MM board, this equals 7

//Set up the array used to store the walls
int mazeWalls[MAZESIZE][MAZESIZE] = {0};

//Used to determine if we have visited the cell or not
bool MAZEVISIT[MAZESIZE][MAZESIZE] = {false};

//Maze weights - will be used to dictate the weight of a given cell (As in, how far it is from the center)
int mazeWeight[MAZESIZE][MAZESIZE] = {0};

//This array will be used to translate the direction. 
int relativeToAbsolute[4][3] = {{WEST, NORTH, EAST},
                                {NORTH, EAST, SOUTH},
                                {EAST, SOUTH, WEST},
                                {SOUTH, WEST, NORTH}};

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

void turnLeft() {
    API::turnLeft();
    // log("Turning Left");
    //Saturation Detection
    if (orient == 0) {
        orient = 3;
    } else {
        orient -= 1;   //Rotate left on the orient
    }
}

void turnRight() {
    API::turnRight();
    // log("Turning Right");

    //Saturation Detection
    if (orient == 3) {
        orient = 0;
    } else {
        orient += 1;   //Rotate right on the orient
    }
}

void moveFwd() {
    API::moveForward();
    switch(orient) {
        case 0: 
            //If we are headed north
            currentRow--;
            break;
        case 1: 
            //If we are headed East
            currentCol++;
            break;
        case 2: 
            //If we are headed south
            currentRow++;
            break;
        case 3: 
            //If we are headed west
            currentCol--;
            break;
    }
}

//Performs flood fill on the given global array mazeWalls and mazeWeight
void floodFill() {

    int tmpRow, tmpCol;

    //Set up the queue to store all the nodes that still need to be looked at
    //For the sake of simplicity, I am setting up two queues to work in tandem.
    //To optimize for memory, create a struct of X,Y and use that to set up the queue.
    //Better yet, refer to a single int for the entire matrix. Use / and % to find the X and Y
    queue<int> unvisitedRow;
    queue<int> unvisitedCol;

    //Set the entire weights array back to INT_MAX
    for (int i = 0; i < MAZESIZE; i++) {
        for (int j = 0; j < MAZESIZE; j++) {
            mazeWeight[i][j] = INT_MAX;
        }
    }

    //Set the goal cell to 0
    mazeWeight[goalRow][goalCol] = 0;

    //Push the first row/col pair to the queue to process
    unvisitedRow.push(goalRow);
    unvisitedCol.push(goalCol);

    while(!unvisitedCol.empty()) {
        //Pop from the queue. Set all neighbors that are INT_MAX to cell val+1
        tmpRow = unvisitedRow.front();
        tmpCol = unvisitedCol.front();
        unvisitedRow.pop();
        unvisitedCol.pop();

    // cerr << "\n----------------\nCurrent Weights\n";
    // // Debug
    // for (int i = 0; i < MAZESIZE; i++) {
    //     for (int j = 0; j < MAZESIZE; j++) {
    //         cerr << mazeWeight[i][j] << " ";
    //     }
    //     cerr << endl;
    // }

    // cerr << "\n----------------\nCurrent Walls\n";
    // for (int i = 0; i < MAZESIZE; i++) {
    //         for (int j = 0; j < MAZESIZE; j++) {
    //             cerr << mazeWalls[i][j] << " ";
    //         }
    //         cerr << endl;
    //     }

        //Check NESW neighbors to see if they are accessable. If they are accessable and INT_MAX, then change weight to cellVal+1
        //North of the cell
        if (!(mazeWalls[tmpRow][tmpCol] & (1 << 3-NORTH))) {  //If there is no wall north
            if (mazeWeight[tmpRow-1][tmpCol] == INT_MAX) {
                mazeWeight[tmpRow-1][tmpCol] = mazeWeight[tmpRow][tmpCol] + 1;
                unvisitedRow.push(tmpRow-1);
                unvisitedCol.push(tmpCol);
            }
        }

        //East of the cell
        if (!(mazeWalls[tmpRow][tmpCol] & (1 << 3-EAST))) {  //If there is no wall north
            if (mazeWeight[tmpRow][tmpCol+1] == INT_MAX) {
                mazeWeight[tmpRow][tmpCol+1] = mazeWeight[tmpRow][tmpCol] + 1;
                unvisitedRow.push(tmpRow);
                unvisitedCol.push(tmpCol+1);
            }
        }

        //South of the Cell
        if (!(mazeWalls[tmpRow][tmpCol] & (1 << 3-SOUTH))) {  //If there is no wall south
            if (mazeWeight[tmpRow+1][tmpCol] == INT_MAX) {
                mazeWeight[tmpRow+1][tmpCol] = mazeWeight[tmpRow][tmpCol] + 1;
                unvisitedRow.push(tmpRow+1);
                unvisitedCol.push(tmpCol);
            }
        }

        //West of the cell
        if (!(mazeWalls[tmpRow][tmpCol] & (1 << 3-WEST))) {  //If there is no wall west
            // cerr << "Selection: [" << tmpRow << "] [" << tmpCol << "] with walls: " << mazeWalls[tmpRow][tmpCol] << endl;
            if (mazeWeight[tmpRow][tmpCol-1] == INT_MAX) {
                mazeWeight[tmpRow][tmpCol-1] = mazeWeight[tmpRow][tmpCol] + 1;
                unvisitedRow.push(tmpRow);
                unvisitedCol.push(tmpCol-1);
            }
        }
    }
}


int main(int argc, char* argv[]) {
    log("Starting Algorithm");

    //idk this was standard
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");

    //MAZESIZE = API::mazeHeight();

    //Set up the known walls of the maze (The fact that everything is enclosed)
    for (int i = 0; i < MAZESIZE; i++) {
        for (int j = 0; j < MAZESIZE; j++) {
            if (i == 0) {
                mazeWalls[i][j] |= 1 << 3-NORTH;
            } 

            if (i == MAZESIZE - 1) {
                mazeWalls[i][j] |= 1 << 3-SOUTH;
            }

            if (j == 0) {
                mazeWalls[i][j] |= 1 << 3-WEST;
                
                //Currently being used for testing
                // mazeWalls[i][j] |= 1 << 3-EAST;
            } 

            if (j == MAZESIZE - 1) {
                mazeWalls[i][j] |= 1 << 3-EAST;
            }
            
        }
    }

    //Currently being used for testing
    // mazeWalls[0][0] &= ~(1 << 3-EAST);

    // cerr << "Current Walls\n";
    // // Debug
    // for (int i = 0; i < MAZESIZE; i++) {
    //     for (int j = 0; j < MAZESIZE; j++) {
    //         cerr << mazeWeight[i][j] << " ";
    //     }
    //     cerr << endl;
    // }

    //If we start on the left, start the array on the bottom left.
    //If we start on the right, start the array on the bottom right.
    if(startLorR == 0) {
        //Here, we start on the bottom left
        currentRow = MAZESIZE-1;
        currentCol = 0;
    } else {
        //Here, we start on the bottom right
        currentCol = MAZESIZE-1;
        currentRow = 0;
    }

    floodFill();

    while (true) {
        

        //Step 1: Read the walls for the current Cell
        //Set this to 0 if we're overwriting everything. 
        //For testing purposes right now (And to not crash my floodfill algorithm, we just read-mod-write instead of overwriting)
        int readCurrentWalls = mazeWalls[currentRow][currentCol];   
        //This will also turn the cells into the absolute orientation
        if (API::wallLeft()) {
            // log("Wall left");
            readCurrentWalls |= 1 << (3 - relativeToAbsolute[orient][LEFT]);
        }

        if (API::wallRight()) {
            // log("Wall right");
            readCurrentWalls |= 1 << (3 - relativeToAbsolute[orient][RIGHT]);
        }

        if (API::wallFront()) {
            // log("Wall Front");
            readCurrentWalls |= 1 << (3 - relativeToAbsolute[orient][FRONT]);
        }
        // log(to_string(readCurrentWalls));
        
        //Step 2: Store wall information to mazeWalls array
        mazeWalls[currentRow][currentCol] = readCurrentWalls;     

        //                                                                         _
        //Also update the neighboring cell (They must share a same wall. Thus, if |X|Y)
        //If we are in X, we must also update Y, since they share a common wall   
        //North of the cell
        if (mazeWalls[currentRow][currentCol] & (1 << 3-NORTH)) {  //If there is a wall north
            mazeWalls[currentRow-1][currentCol] |= (1 << 3-SOUTH);
        }

        //East of the cell
        if (mazeWalls[currentRow][currentCol] & (1 << 3-EAST)) {  //If there is a wall east
            mazeWalls[currentRow][currentCol+1] |= (1 << 3-WEST);
        }

        //South of the Cell
        if (mazeWalls[currentRow][currentCol] & (1 << 3-SOUTH)) {  //If there is a wall south
            mazeWalls[currentRow+1][currentCol] |= (1 << 3-NORTH);
        }

        //West of the cell
        if (mazeWalls[currentRow][currentCol] & (1 << 3-WEST)) {  //If there is a wall west
            mazeWalls[currentRow][currentCol-1] |= (1 << 3-EAST);
        }
        
        //Debug
        cerr << "orient: " << orient << " Row: " << currentRow << " Col: " << currentCol << endl;
        for (int i = 0; i < MAZESIZE; i++) {
            for (int j = 0; j < MAZESIZE; j++) {
                cerr << mazeWalls[i][j] << " ";
            }
            cerr << endl;
        }

        
        
/*
    This is the algorithm section. here, we will decide whether to turn left, do nothing, or turn right
*/

        //Step 3: Perform Flood Fill on the updated maze. Store into mazeWeights
        floodFill();

        //Debug
        cerr << "-------------\nWeights Graph" << endl;
        for (int i = 0; i < MAZESIZE; i++) {
            for (int j = 0; j < MAZESIZE; j++) {
                cerr << mazeWeight[i][j] << " ";
            }
            cerr << endl;
        }

        //Step 4: Select the neighbor with the smallest weight. But only if we are not in the edges and if there is not a wall
        int smallestAccessable = INT_MAX;
        int tmpRow = currentRow;
        int tmpCol = currentCol;
        int direction;

        if (mazeWeight[currentRow-1][currentCol] < smallestAccessable && currentRow != 0 && !(mazeWalls[currentRow][currentCol] & (1 << 3-NORTH))) {
            //North
            smallestAccessable = mazeWeight[currentRow-1][currentCol];
            tmpRow = currentRow-1;
            tmpCol = currentCol;
            direction = NORTH;
        } else {
            cerr << "Not Selected: [" << currentRow-1 << "] [" << currentCol << "] with weight: " << mazeWeight[currentRow-1][currentCol] << " and direction: " << NORTH << endl;
        }
        // cerr << "Is there a wall east (selecting): " << mazeWalls[tmpRow][tmpCol] << endl;
        if (mazeWeight[currentRow][currentCol+1] < smallestAccessable && currentCol != MAZESIZE && !(mazeWalls[currentRow][currentCol] & (1 << 3-EAST))) {
            //East
            smallestAccessable = mazeWeight[currentRow][currentCol+1];
            tmpRow = currentRow;
            tmpCol = currentCol+1;
            direction = EAST;
        } else {
            cerr << "Not Selected: [" << currentRow << "] [" << currentCol+1 << "] with weight: " << mazeWeight[currentRow][currentCol+1] << " and direction: " << EAST << endl;
        }
        
        if (mazeWeight[currentRow+1][currentCol] < smallestAccessable && currentRow != MAZESIZE && !(mazeWalls[currentRow][currentCol] & (1 << 3-SOUTH))) {
            //South
            smallestAccessable = mazeWeight[currentRow+1][currentCol];
            tmpRow = currentRow+1;
            tmpCol = currentCol;
            direction = SOUTH;
        } else {
            cerr << "Not Selected: [" << currentRow+1 << "] [" << currentCol << "] with weight: " << mazeWeight[currentRow+1][currentCol] << " and direction: " << SOUTH << endl;
        }
        
        if (mazeWeight[currentRow][currentCol-1] < smallestAccessable && currentCol != 0 && !(mazeWalls[currentRow][currentCol] & (1 << 3-WEST))) {
            //West
            smallestAccessable = mazeWeight[currentRow][currentCol-1];
            tmpRow = currentRow;
            tmpCol = currentCol-1;
            direction = WEST;
        } else {
            cerr << "Not Selected: [" << currentRow << "] [" << currentCol-1 << "] with weight: " << mazeWeight[currentRow][currentCol-1] << " and direction: " << WEST << endl;
        }
        
        // cerr << "Current Cell [" << currentRow << "] [" << currentCol << "has walls: " << endl;
        cerr << "Selection: [" << tmpRow << "] [" << tmpCol << "] with weight: " << smallestAccessable << " and direction: " << direction << endl;

    
        //Step 5: Orient towards that neighbor
        

        //Only take right turns to orient towards the next cell
        while (orient != direction) {
            turnRight();
        }
        

        cerr << "-------------\nNext Location" << endl;
        //Step 6: Move Forward
        moveFwd();
        

       //Perform flood fill with the current knowledge of the walls. 
       //Loop through the immediate 4 cells, going NESW (Relative). Note it's weight. If there is a wall, weight = INT_MAX
       //Select the cell with the lowest weight
       //Turn into the direction. Proceed with the next move statement.

        
    }
}