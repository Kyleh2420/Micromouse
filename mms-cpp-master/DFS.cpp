#include <iostream>
#include <string>
#include "API.h"

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

void log(const std::string& text) {
    std::cerr << text << std::endl;
}



int main(int argc, char* argv[]) {
    log("Starting Algorithm");

    //idk this was standard
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");

    //MAZESIZE = API::mazeHeight();

    //Set up the array used to store the walls
    int mazeWalls[MAZESIZE][MAZESIZE] = {0};

    //Used to determine if we have visited the cell or not
    bool MAZEVISIT[MAZESIZE][MAZESIZE] = {false};

    //Maze weights - will be used to dictate the weight of a given cell (As in, how far it is from the center)
    int MAZEWEIGHT[MAZESIZE][MAZESIZE] = {0};

    //This array will be used to translate the direction. 
    int relativeToAbsolute[4][3] = {{WEST, NORTH, EAST},
                                    {NORTH, EAST, SOUTH},
                                    {EAST, SOUTH, WEST},
                                    {SOUTH, WEST, NORTH}};

    //Set up the weights of each of the cells. Upon setup, each cell is counted as if there are no walls in the way
    int tmp = MAZESIZE;
    for (int i = 0; i < MAZESIZE; i++) {
        if (i < MAZESIZE/2) {
            tmp = (MAZESIZE/2 - 1)*2 - i;
        } else if (i == MAZESIZE/2) {
            tmp = (MAZESIZE/2 - 1);
        } else {
            tmp = (MAZESIZE/2 - 1)*2 - (MAZESIZE - i-1);
        }

        for (int j = 0; j < MAZESIZE; j++) {
            if (j < MAZESIZE/2) {
                MAZEWEIGHT[i][j] = tmp--;
            } else if (j == MAZESIZE/2) {
                tmp++;
                MAZEWEIGHT[i][j] = tmp++;
            } else {
                MAZEWEIGHT[i][j] = tmp++;
            }
        }
    }

    //Debug
    // for (int i = 0; i < MAZESIZE; i++) {
    //     for (int j = 0; j < MAZESIZE; j++) {
    //         cerr << MAZEWEIGHT[i][j] << " ";
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

    while (true) {
        int readCurrentWalls = 0;

        //Step 1: Read the walls for the current Cell
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
        
        //Debug
        cerr << "orient: " << orient << " Row: " << currentRow << " Col: " << currentCol << endl;
        for (int i = 0; i < MAZESIZE; i++) {
            for (int j = 0; j < MAZESIZE; j++) {
                cerr << mazeWalls[i][j] << " ";
            }
            cerr << endl;
        }

        cerr << "-------------\nNext Location" << endl;
        
/*
    This is the algorithm section. here, we will decide whether to turn left, do nothing, or turn right
*/

        //Step 3: Perform Flood Fill on the updated maze. Store into mazeWeights

        /*
            Left Wall Following Code
            //if there is no wall left, turn left
        if (!(mazeWalls[currentRow][currentCol] & (1 << 3-relativeToAbsolute[orient][LEFT]))) {
            API::turnLeft();
            log("Turning Left");
            //Saturation Detection
            if (orient == 0) {
                orient = 3;
            } else {
                orient -= 1;   //Rotate left on the orient
            }
        }

        //If there is a wall in front, turn right
        while (mazeWalls[currentRow][currentCol] & (1 << 3-relativeToAbsolute[orient][FRONT])) {
            API::turnRight();
            log("Turning Right");

            //Saturation Detection
            if (orient == 3) {
                orient = 0;
            } else {
                orient += 1;   //Rotate right on the orient
            }
            
        }
        */

       //Perform flood fill with the current knowledge of the walls. 
       //Loop through the immediate 4 cells, going NESW (Relative). Note it's weight. If there is a wall, weight = INT_MAX
       //Select the cell with the lowest weight
       //Turn into the direction. Proceed with the next move statement.

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
}