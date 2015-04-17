
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
using namespace std;

class Block {
	public:
		Block();
		bool isValidWall(int a, int b, int c); //excludes outside walls and checked walls
		bool isChecked();
		void setChecked(bool);
		bool isSolid();
		void setSolid(bool);
		Block* locateRoot();
		void setRoot(Block* given);
	private:
		bool checked; //marks a wall that has been assessed
		bool solid; //marks if it is a block or space
		Block* root; //if hole, points towards root
};

class Maze {
	public:
		Maze();
		Maze(int);
		Maze(int, int, int);
		~Maze();
		void generate();
		void printSlice(int); //given: 3rd dim
		//void printSlice(int, int); //4d
		void userPrint();
		int unchecked(); //this, too, might be private
		bool isValidWall(int, int, int); //check these two, might be private
		bool isSpace(int, int, int);
	private:
		int numCols;
		int numRows;
		int numLvls;
		//int numLays; //4d
		Block ***grid; //Ownership resides here
};



int main(int argc, char *argv[]) {
    srand(time(NULL));
    int choice;
    int dimx = 7, dimy = 7, dimz = 7;
    do {
		cout << "What would you like to do?\n";
		cout << "1 - Fast generate (7x7x7)\n";
		cout << "2 - Minecraft cube (side length: 1+6*n blocks)\n";
		cout << "3 - Specific dimensions (each side must be odd)\n";
		cout << "0 - Exit program\n";
		cin >> choice;
		if (choice == 1) {
			Maze fastM(7,7,7);
			fastM.generate();
			fastM.userPrint();
		}
		else if (choice == 2) {
			cout << "This generates a maze that will appear cubical\n"
			     << "in Minecraft, but compensates for Steve's height.\n"
			     << "Please note, you must choose a dimension that is\n"
			     << "a multiple of 6, plus 1.\n";
			cin >> dimx;
			Maze mcM(dimx);
			cout << "Generating maze...";
			mcM.generate();
			mcM.userPrint();
		}
		else if (choice == 3) {
			cout << "This allows you to set specific dimensions for each\n"
			     << "side of the maze. Please note, they must be odd.\n";
			cout << "Number of rows: ";
			cin >> dimx;
			cout << "Number of columns: ";
			cin >> dimy;
			cout << "Number of levels: ";
			cin >> dimz;
			Maze specM(dimx, dimy, dimz);
			cout << "Generating maze...";
			specM.generate();
			specM.userPrint();
		}
	}while (choice != 0);

    //Maze testM(31,31,21);
    //testM.generate();
    //testM.userPrint();
    
    system("PAUSE");
    return EXIT_SUCCESS;
}



Block::Block() {
	checked = false;
	solid = true;
	root = NULL;
}
	
bool Block::isChecked() {
	return checked;
}

void Block::setChecked(bool given) {
	checked = given;
	return;
}

bool Block::isSolid() {
	return solid;
}

void Block::setSolid(bool given) {
	solid = given;
	return;
}

Block* Block::locateRoot() {
	if (this->root == NULL) {
		return this;
	}
	else {
		return this->root->locateRoot();
	}
}

void Block::setRoot(Block* given) {
	// this function should validate input
	this->root = given;
	return;
}

//End Block


//Begin Maze

//Default constructor
Maze::Maze() {
	//default maze size
	numCols = 7;
	numRows = 7;
	numLvls = 7;
}

//Minecraft constructor
Maze::Maze(int dim) {
	if(dim%2==1&&dim%3==1) {
		numCols = dim;
		numRows = dim;
		numLvls = (dim/3)*2+1; //Minecraft adjustment
	}
	else {
		numCols = 7;
		numRows = 7;
		numLvls = 5;
	}
}

//Specific constructor
Maze::Maze(int a, int b, int c) {
	if(a%2==1 && b%2==1 && c%2==1) {
		numCols = a;
		numRows = b;
		numLvls = c;
	}
	else {
		numCols = 7;
		numRows = 7;
		numLvls = 7;
	}
}

// Destructor
Maze::~Maze() {
	//free blocks
	for(int i = 0; i < numCols; i++) {
		for(int j = 0; j < numRows; j++) {
			delete [] grid[i][j];
		}
		delete [] grid[i];
	}
	delete [] grid;
}

//generates maze based on numCols, numRows, and numLvls, which should already be set
void Maze::generate() {
	//Allocate memory for entire grid
	grid = new Block**[numCols];
	for(int i = 0; i < numCols; i++) {
		grid[i] = new Block*[numRows];
		for(int j = 0; j < numRows; j++) {
			grid[i][j] = new Block[numLvls];
		}
	}
	
	int a, b, c;
	while(unchecked()) {
		a = rand()%numCols;
		b = rand()%numRows;
		c = rand()%numLvls;
		
		if(!grid[a][b][c].isChecked()) {
			if(isSpace(a,b,c)) {
				grid[a][b][c].setSolid(false);
			}
			if(isValidWall(a,b,c)) {
				if((a%2==0)&&(grid[a-1][b][c].locateRoot() != grid[a+1][b][c].locateRoot())) {
					grid[a-1][b][c].locateRoot()->setRoot(grid[a+1][b][c].locateRoot());
					grid[a][b][c].setSolid(false);
				}
				else if ((b%2==0)&&(grid[a][b-1][c].locateRoot() != grid[a][b+1][c].locateRoot())) {
					grid[a][b-1][c].locateRoot()->setRoot(grid[a][b+1][c].locateRoot());
					grid[a][b][c].setSolid(false);
				}
				else if ((c%2==0)&&(grid[a][b][c-1].locateRoot() != grid[a][b][c+1].locateRoot())) {
					grid[a][b][c-1].locateRoot()->setRoot(grid[a][b][c+1].locateRoot());
					grid[a][b][c].setSolid(false);
				}
				else { //not a problem, not disjointed sets
					//cout << "Else " << a << b << c << endl;
				}
			}
			grid[a][b][c].setChecked(true);
		}
		//immediately pick new coordinates
	}//end while
    cout << "Your maze has been generated.\n";
	return;
}

void Maze::printSlice(int c) {
    for(int b = numRows-1; b >= 0; b--) { //start at top row
		for(int a = 0; a < numCols; a++) { //start at left-most column
			if(grid[a][b][c].isSolid()) {
				cout << "X";
			}
			else {
				cout << "-";
			}
		}
		cout << endl;
	}
	return;
}

int Maze::unchecked() {
	int count = 0;
	for(int i = 0; i < numCols; i++) {
		for(int j = 0; j < numRows; j++) {
			for(int k = 0; k < numLvls; k++) {//for every i, j, and k
				if (!grid[i][j][k].isChecked()){
					count++;
				}
			}
		}
	}
	return count;
}

bool Maze::isValidWall(int i, int j, int k) {
    return (0<i&&i<(numCols-1)&&0<j&&j<(numRows-1)&&0<k&&k<(numLvls-1)) //not edges
            &&(
               (i%2==0&&j%2==1&&k%2==1) 
            || (i%2==1&&j%2==0&&k%2==1)
            || (i%2==1&&j%2==1&&k%2==0)
            );
}

bool Maze::isSpace(int i, int j, int k) {
	return (0<i&&i<numCols&&0<j&&j<numRows&&0<k&&k<numLvls) //not edges
            &&(i%2==1&&j%2==1&&k%2==1);
}

void Maze::userPrint() {
	cout << "This function prints any level you want to see, on demand.\n";
	cout << "Type floor number to print floor."
	     << " Type zero to exit AND delete the maze.\n";
	int input = 0;
	cout << "Which level do you want to see?";
	cin >> input;
	//cout << "C, R, L: " << numCols << " " << numRows << " " << numLvls << endl;
	while(input > 0 && input <= numLvls) {
		printSlice(input-1);
		cout << "Which level? ";
		cin >> input;
	}
	
}

//End Maze




