//a simulation of bug behaviour as a model for planning in the built environment,
// based on the model proposed by Cortois and Heymans
//Agnes Cameron, 2017

//to do: add input filestream to determine bug motion. 
//sockets over openCV


#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <vector>
#include <ctime>
#include <ctype.h>
#include <sstream>
#include <fstream>

using namespace std;

    
    ofstream cellfile;
    ofstream bugfile;
    ofstream coverfile;

    //defines a vector type of mixed 
    struct cell{
        int x;
        int y;
        int pollutant = 0;
        bool hasBug;
        int cellSugarLevel;
        int sugarScapeGrowBack = 0;        
    };
    
    struct bug{
        //these get sent over socket
        int hexID;
        int movex;
        int movey;

        int x;
        int y;
        //leave for now: int age;
        //should metabolism and resource be floats??
        int metabolism;
        int vision;
        //a if true; b if false, all a for now. 
        char affiliation;
        int agentSugarLevel;

    };
    
    struct glance{
        int sugarVal;
        bool hasBug;
        int dist;
        int x;
        int y;
    }

    //define different sets of environental variables for the 
    //switch case??
    // struct env1{
    //     int maxSugar = 5;
    //     int minSugar = 25;
    //     int sugarGrowth = 
    //     int pollutionRate =         
    // }

    // struct env2{
    //     int maxSugar = 4;
    //     int minSugar = 10;
    // }

    // struct env3{
    //     int maxSugar = 5;
    //     int minSugar = 3;
    // }    

//prints the status of the grid (bugs left, sugar right)
void printBugs(vector<cell> cellVector, vector<bug> bugVector, int cycles){
        int gridSize = sqrt(cellVector.size());
    
    cout << "bugs: ";
    for(int i=1; i<gridSize-6; i++){
    cout << "- ";
    }
    cout << "  ||    ||  sugar: ";
    for(int i=1; i<gridSize-7; i++){
    cout << "- ";
    }
    cout << endl;
    for(int j = 1; j<gridSize; j++){
        for(int i=1; i<gridSize; i++){
            if cellVector[i + j*gridSize].hasBug
                cout << "|x";
            else 
                cout << "| ";
        }

        cout << "  ||    ||  ";

        for(int i=1; i<gridSize; i++){
            cout << "|" << cellVector[i + j*gridSize].sugar;
        }
        cout << "|" << endl;   
        cout << "|" << endl;   
    }
    for(int i=1; i<gridSize + 1; i++){
    cout << "- ";
    }
    cout << endl << endl;
}


void writeBugJSON(vector<bug> bugVector){
    string bugJSON = "{{ bugs: [ ";

    for(int i=1; i<bugVector.length(); i++){
        bugJSON = bugJSON + "{ 'hexID':'" + toString(bugVector[i].hexID) + ...
            "', 'movex':'" + toString(bugVector[i].movex) + "', 'movey':'" + ...
            toString(bugVector[i].movey) + "},  ";
    }
    bugJSON = bugJSON + "]}";
    cout << bugJSON << endl;

    //sendBugJSON(bugJSON)
}

//calculates bug motion
void moveBugs(vector<cell> cellVector, vector<bug> &bugVector){
    int a = 0;
    int i = 0;
    bool migrated = false;
    int gridSize = sqrt(cellVector.size());

    do{
        int bugPosi = bugVector[a].x;
        int bugPosj = bugVector[a].y;

        int cellVectorPos = bugPosj*sqrt(cellVector.size()) + bugPosi;
        //useful ? 
        //
        do{
        int randDirection = rand() %8;

        }


        a=a+1;

    }while(a < bugVector.size());
}

void updateLattice(vector<cell> &cellVector, vector<bug> bugVector){
    
    int a = 0;
    int i = 0;
    
    do{
        int bugPosi = bugVector[a].x;
        int bugPosj = bugVector[a].y;

        int cellVectorPos = bugPosj*sqrt(cellVector.size()) + bugPosi;


        a++;

    }while(a < bugVector.size());

}

// calculates the travel required for each cycle
void calculateBugMove(vector<cell> &newCellVector, vector<cell> cellVector,
    vector<bug> &newBugVector, vector<bug> bugVector, int cellVectorPos){
        
    int a=0;
    int b=0;

    int bugPosi = cellVector[cellVectorPos].x;
    int bugPosj = cellVector[cellVectorPos].y;

    vector<glance> sightVector(bugVector[a].vision*4);

    do{

        do{
            //in the + x direction
            sightVector[b + 0].x = bugPosi+b+1;
            sightVector[b + 0].y = bugPosj;
            sightVector[b + 0].sugarVal = cellVector[bugPosi + b + 1 + bugPosj*gridSize].cellSugarLevel;            
            sightVector[b + 0].hasBug = newCellVector[bugPosi + b + 1 + bugPosj*gridSize].hasBug;
            sightVector[b + 0].dist = b+1; 

            //in the -x direction
            sightVector[b + 1].x = bugPosi-b-1;            
            sightVector[b + 1].sugarVal = cellVector[bugPosi - b - 1 + bugPosj*gridSize].cellSugarLevel;
            sightVector[b + 1].hasBug = newCellVector[bugPosi - b - 1 + bugPosj*gridSize].hasBug;

            //in the +y direction
            sightVector[b + 2].y = cellVector[bugPosi + (bugPosj + b + 1)*gridSize].y;
            sightVector[b + 2].x = cellVector[bugPosi + (bugPosj + b + 1)*gridSize].x;

            //in the -y direction
            sightVector[b + 3].y = cellVector[bugPosi + (bugPosj - b - 1)*gridSize].y;            
            sightVector[b + 3].x = cellVector[bugPosi + bugPosj*gridSize].x;

            b++;
        }while(b<bugVector[a].vision)
 
        newBugVector[a].xmove = newBugVector[a].x - bugVector[a].x;
        newBugVector[a].ymove = newBugVector[a].y - bugVector[a].x;

        a++;

    }while(a<bugVector.length())
}

//updates the state of each bug
void updateBug(vector<cell> &cellVector, vector<bug> &bugVector){
    
    vector<bug> newBugVector(population);
    vector<cell> newCellVector(population);

    int a = 0;
    int i = 0;
    
    //randomly shuffles bug vector
    random_shuffle(bugVector.begin(), bugVector.end());

    do{
        int bugPosi = bugVector[a].x;
        int bugPosj = bugVector[a].y;

        int cellVectorPos = bugPosj*sqrt(cellVector.size()) + bugPosi;
        
        //update sugar status

        calculateBugMove(newCellVector, cellVector, newBugVector, bugVector, cellVectorPos);

        a++;

    }while(a < bugVector.size());

    //assign new values (cellVector has to change as bugs come to locations and eat food)
    bugVector = newBugVector;
    cellVector = newCellVector;

}

void initialiseLattice(int minSugar, int maxSugar, vector<cell> &cellVector){
    int a = 0;
    
    do{
        //set cell sugar level
        cellVector[a].cellSugarLevel = minSugar + rand()%(maxSugar-minSugar);


        //increment
        a++; 
    }while(a < cellVector.size());
}


void initialiseBugs(int maxMetabolism, int minSugar, int maxSugar, 
                        vector<cell> cellVector, vector<bug> &bugVector){
    int a = 0;
    
    do{
        //assign hex id
        bugVector[a].hexID = a;

        //choose cell for bug
        int chosenCell = rand()%cellVector.size();
        if(cellVector[chosenCell].hasBug == false){
        bugVector[a].x = cellVector[chosenCell].x;
        bugVector[a].y = cellVector[chosenCell].y;
        }


        //assign metabolism
        bugVector[a].metabolism = rand()%maxMetabolism;

        //assign vision
        bugVector[a].vision = 1 + rand()%3;

        //assign affiliation(none for now)

        bugVector[a].agentSugarLevel = minSugar + rand()%(maxSugar-minSugar);

        //increment
        a++;   
        
    }while(a < bugVector.size());
}


//runs the main loop
int main(){
    int gridSize = 20;
    int population = 30;
    int cycles = 0;

    //these values can be modulated -- do they go here?
    //int minMetabolism = 0;
    int maxMetabolism = 3;
    int minSugar = 5;
    int maxSugar = 25;

    srand (time(NULL));

    vector<cell> cellVector(gridSize*gridSize);
    vector<bug> bugVector(population);
    
    
    //assign each vector element to a cell
    for(int j = 0; j<gridSize; j++){
        for(int i = 0; i<gridSize; i++){
            cellVector[i + j*gridSize].x = i;
            cellVector[i + j*gridSize].y = j;
        }
    }

    //initialise lattice and bug states
    initialiseLattice(cellVector);    
    initialiseBugs(cellVector, bugVector);


        //start the simulation 
        do{

            //calculate the status of each bug
            updateLattice(cellVector, bugVector);

            //calculate the status of each bug
            updateBug(cellVector, bugVector);


            //print the grid status
            //printEarth(cellVector, cycles);
            
            //prints thresholded street plan
            printBugs(cellVector, bugVector, cycles);
            system("sleep 0.2");
            
            
            cycles = cycles+1;

            //writes the bug as JSON
            writeBugJSON(bugVector);

            //open websocket and send bugInfo as JSON string

        }while(cycles < dataIn.length());


    
        cellfile.open("../sample_data/cellfile_newdata.csv");
        //write cell values to csv file
        for(int j = 0; j<gridSize*gridSize; j++){
                cellfile << cellVector[j].x << "," << cellVector[j].y << "," << cellVector[j].cellSugarLevel;
            cellfile << "\n";
        }
        cellfile.close();
        

}
