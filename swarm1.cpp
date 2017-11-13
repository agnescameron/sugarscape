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
        //int pollutant = 0;
        bool hasBug;
        int cellSugarLevel;        
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
        //char affiliation;
        int agentSugarLevel;

    };
    
struct newbug{
    int x;
    int y;
};

    struct glance{
        int sugarVal;
        bool hasBug;
        int dist;
        int x;
        int y;
    };

    int population = 30;
    int gridSize = 20;
    int maxMetabolism = 3;
    int minSugar = 5;
    int maxSugar = 25;
    int minSugarGrowBack = 1;
    int maxSugarGrowBack = 5;
 

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
            if (cellVector[i + j*gridSize].hasBug == true)
                cout << "|x";
            else 
                cout << "| ";
        }

        cout << "  ||    ||  ";

        for(int i=1; i<gridSize; i++){
            cout << "|" << cellVector[i + j*gridSize].cellSugarLevel;
        }
        cout << "|" << endl;   
        cout << "|" << endl;   
    }
    for(int i=1; i<gridSize + 1; i++){
    cout << "- ";
    }
    cout << endl << endl;
}


void writeBugJSON(vector<bug> bugVector, ofstream &bugJSONFile){
    string bugJSON = "{\"bugs\": [" ;

    for(int i=1; i<bugVector.size()-1; i++){
        bugJSON = bugJSON + "{ \"hexID\":\"" + to_string(bugVector[i].hexID) + 
            "\", \"movex\":\"" + to_string(bugVector[i]
                .movex) + "\", \"movey\":\"" + 
            to_string(bugVector[i].movey) + "\"},  ";
    }


    bugJSON = bugJSON + "]},";
    bugJSONFile << bugJSON << endl;

    //sendBugJSON(bugJSON)
}


//used to sort list of sight structs by sugar val
//poss return to comparator to minimise distance
bool compareInt(const glance &a, const glance &b)
    {
            return a.sugarVal < b.sugarVal;
    }


// calculates the travel required for each cycle
    //this is a risky bit
void calculateBugMove(vector<cell> &newCellVector, vector<cell> cellVector,
    vector<newbug> &newBugVector, vector<bug> &bugVector, int a){
        
    int b=0;

        int bugPosi = bugVector[a].x;
        int bugPosj = bugVector[a].y;

    cout << "vision is " << bugVector[a].vision << endl;
    cout << "bug pos i is " << bugPosi << ", bug pos j is " << bugPosj;

    vector<glance> sightVector(bugVector[a].vision*4);
            cout << "moving, a is: " << a << endl;

        do{
            //in the + x direction
            sightVector[b + 0].x = bugPosi+b+1;
            sightVector[b + 0].y = bugPosj;
            sightVector[b + 0].sugarVal = newCellVector[bugPosi + b + 1 + bugPosj*gridSize].cellSugarLevel;            
            sightVector[b + 0].hasBug = newCellVector[bugPosi + b + 1 + bugPosj*gridSize].hasBug;
            sightVector[b + 0].dist = b+1; 

            //in the -x direction
            sightVector[b + 1].x = bugPosi-b-1;
            sightVector[b + 1].y = bugPosj;                   
            sightVector[b + 1].sugarVal = newCellVector[bugPosi - b - 1 + bugPosj*gridSize].cellSugarLevel;
            sightVector[b + 1].hasBug = newCellVector[bugPosi - b - 1 + bugPosj*gridSize].hasBug;
            sightVector[b + 1].dist = b+1; 


            //in the +y direction
            sightVector[b + 2].x = bugPosi;
            sightVector[b + 2].y = bugPosj + b + 1;                     
            sightVector[b + 2].sugarVal = newCellVector[bugPosi + (bugPosj + b + 1)*gridSize].cellSugarLevel;
            sightVector[b + 2].hasBug = newCellVector[bugPosi + (bugPosj + b + 1)*gridSize].hasBug;
            sightVector[b + 2].dist = b+1; 


            //in the -y direction
            sightVector[b + 3].x = bugPosi;
            sightVector[b + 3].y = bugPosj - b - 1;               
            sightVector[b + 3].sugarVal = newCellVector[bugPosi + (bugPosj - b - 1)*gridSize].cellSugarLevel;            
            sightVector[b + 3].hasBug = newCellVector[bugPosi + (bugPosj - b - 1)*gridSize].hasBug;
            sightVector[b + 3].dist = b+1; 

            b++;
        }while(b<bugVector[a].vision);
        cout << "sight vector ok" << endl;

        sort(sightVector.begin(), sightVector.end(), compareInt);
        int x=0;
        do{
            cout << "sight vector " << x << " is " << sightVector[sightVector.size() - x -1].sugarVal << endl;
            x++;
        }while(x<sightVector.size());

        cout << "sort ok, vector begin is " << endl;
            int c =0;
            bool newPosChosen = false;

            do{
                if(sightVector[sightVector.size()-c].hasBug == false && newPosChosen == false){
                    //set bug in cell vector
                    cout << "no bug, sugar level is: " << sightVector[sightVector.size()-c-1].sugarVal << endl;                    
                    newBugVector[a].x = sightVector[sightVector.size()-c-1].x;
                    newBugVector[a].y = sightVector[sightVector.size()-c-1].y;  
                    //newBugVector[a].agentSugarLevel = bugVector[a].agentSugarLevel + sightVector[sightVector.size()-c-1].sugarVal;                                    
                    newCellVector[newBugVector[a].x + newBugVector[a].y*gridSize].cellSugarLevel = 0;    
                    newCellVector[newBugVector[a].x + newBugVector[a].y*gridSize].hasBug = true;                    
                    newPosChosen = true;
                }

                else{
                    cout << "bug there" << endl;
                }

                c++;

            }while(c < sightVector.size());
        
            if (newPosChosen == false){
                newBugVector[a].x = bugPosi;
                newBugVector[a].y = bugPosj;  
                newCellVector[newBugVector[a].x + newBugVector[a].y*gridSize].cellSugarLevel = 0;
                newCellVector[newBugVector[a].x + newBugVector[a].y*gridSize].hasBug = true;                     
            }

        cout << "new position chosen" << endl;

        bugVector[a].movex = newBugVector[a].x - bugVector[a].x;
        bugVector[a].movey = newBugVector[a].y - bugVector[a].y;

        sightVector.clear();

}


void checkNewCellVector(vector<cell> &newCellVector){
    
    int a=0;
    do{
        if(newCellVector[a].cellSugarLevel<= 0)
            newCellVector[a].cellSugarLevel = minSugarGrowBack;

        else if(newCellVector[a].cellSugarLevel >= maxSugar)
            newCellVector[a].cellSugarLevel = maxSugarGrowBack;
        a++;
    }while(a < gridSize*gridSize);
    cout << "checked"<< endl;

}

//updates the state of each bug
void updateBug(vector<cell> &cellVector, vector<bug> &bugVector){
    
    vector<newbug> newBugVector(population);
    vector<cell> newCellVector(gridSize*gridSize);
    int l=0;

    do{
        newCellVector[l].x = cellVector[l].x;
        newCellVector[l].y = cellVector[l].y;        
        newCellVector[l].cellSugarLevel = cellVector[l].cellSugarLevel;
        newCellVector[l].hasBug= false;        
        l++;
    }while(l< gridSize*gridSize);

    int a = 0;
    int i = 0;
    
    //randomly shuffles bug vector
    random_shuffle(bugVector.begin(), bugVector.end());
    cout << "shuffled" << endl;
    do{
        int bugPosi = bugVector[a].x;
        int bugPosj = bugVector[a].y;
        
        //update sugar status

        calculateBugMove(newCellVector, cellVector, newBugVector, bugVector, a);
        cout << "moved" << endl;

        a++;

    }while(a < population);

    //assign new values (cellVector has to change as bugs come to locations and eat food)

    int j=0;
    do{
        bugVector[j].x = newBugVector[j].x;
        bugVector[j].y = newBugVector[j].y;
        j++;
    }while(j< population);

    //checkNewCellVector(newCellVector);

    int k=0;
    do{
        cellVector[k].cellSugarLevel = newCellVector[k].cellSugarLevel;
        cellVector[k].hasBug = newCellVector[k].hasBug;
        cellVector[k].x = newCellVector[k].x;
        cellVector[k].y = newCellVector[k].y;        
        k++;
    }while(k< gridSize*gridSize);

    newBugVector.clear();
    newCellVector.clear();

}



//updates the lattice elementwise
void updateLattice(vector<cell> &cellVector, vector<bug> bugVector){
    
    int a = 0;
    int i = 0;
    
    do{
        
        cellVector[a].cellSugarLevel = minSugar + rand()%(maxSugarGrowBack-minSugarGrowBack);
        a++;

    }while(a < cellVector.size());

}

void initialiseLattice(vector<cell> &cellVector){
    int a = 0;
    
    do{
        //set cell sugar level
        cellVector[a].cellSugarLevel = minSugar + rand()%(maxSugar-minSugar);
        //increment
        a++; 
    }while(a < cellVector.size());
}


void initialiseBugs(vector<cell> &cellVector, vector<bug> &bugVector){
    int a = 0;
    
    do{
        //assign hex id
        bugVector[a].hexID = a;

        //choose cell for bug
        int chosenCell = rand()%cellVector.size();
        if(cellVector[chosenCell].hasBug == false){
            bugVector[a].x = cellVector[chosenCell].x;
            bugVector[a].y = cellVector[chosenCell].y;
            cellVector[chosenCell].hasBug = true;
            cellVector[chosenCell].cellSugarLevel = 0;            
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
    ofstream bugJSONFile;
    bugJSONFile.open("bugCode.json");

    string bugJSON = "{\"bug_timesteps:\" [ ";
    bugJSONFile << bugJSON << endl;    


    int cycles = 0;

    //these values can be modulated -- do they go here?
    //int minMetabolism = 0;

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
    printBugs(cellVector, bugVector, cycles);

    cout << "initialised" << endl;
        //start the simulation 
        do{

            //calculate the status of each bug
            updateLattice(cellVector, bugVector);
            cout << "updated lattice" << endl;

    printBugs(cellVector, bugVector, cycles);

            //calculate the status of each bug
            updateBug(cellVector, bugVector);
            cout << "updated bug" << endl;

            //print the grid status
            //printEarth(cellVector, cycles);
            
            //prints thresholded street plan
            printBugs(cellVector, bugVector, cycles);
            system("sleep 0.2");
            
            
            cycles = cycles+1;

            //writes the bug as JSON
            writeBugJSON(bugVector, bugJSONFile);

            //open websocket and send bugInfo as JSON string

        }while(cycles < 10000);

        bugJSONFile << "]}";
        bugJSONFile.close();

}
