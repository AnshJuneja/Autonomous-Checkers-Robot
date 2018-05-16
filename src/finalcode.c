/* Written by Ansh Juneja, Jack MacKinnon, Noah Kerec, and Rania Rejdal

STANDARD CONFIGURATION OF EV3:
MOTOR A - MAIN MOVEMENT ALONG RAILS
MOTOR B - TRANSVERSAL MOVEMENT (ALONG STRUTS)
MOTOR C - DESCENDING THE ELECTROMAGNET
MOTOR D - TRIGGERING ELECTROMAGNET SWITCH
SENSOR 1 - COLOUR SENSOR 1
SENSOR 2 - COLOUR SENSOR 2
SENSOR 3 - TOUCH SENSOR
SENSOR 4 - ULTRASONIC

ROBOT REGULAR PIECE - RED -----> '2'
ROBOT KING PIECE - DARK GREEN -----> '4'
HUMAN REGULAR PIECE - BLACK = 1 ------> '1'
HUMAN KING PIECE - DARK BLUE = 4 ------> '3'     */

// all distances in cm
const float RADIUS = 2; // radius of wheel across board
const float RADIUSTRAN = 2.94/2; // radius of wheel transversing board
const float MOTORRADIUS = 180; // radius motor has (degrees)
const float BOARD = 38.8; // from square edge to square edge
const float GYARD = -3; // distance from rest position to graveyard (negative)
const float SQUARE = 4.85; //length of a square

// encoder values
const long SQUAREENCODER = SQUARE*360/(2 * PI * RADIUS);
const long SQUARETRANENCODER = SQUARE*360/(2 * PI * RADIUSTRAN);
const long BOARDENCODER = (BOARD-SQUARE) * 360 / (2 * PI * RADIUS);
const long BOARDTRANENCODER = (BOARD-SQUARE) * 360 / (2 * PI * RADIUSTRAN);
const long DESCENDENCODER = 100;
const long BUTTONENCODER = 180;
const long GRAVEYARDENCODER = GYARD * 360 / (2 * PI * RADIUS);
const long KINGENCODER = KINGDIST * 360 / (2 * PI * RADIUS);
const float ATOREST = 3;
const float BTOREST = 9.5;
const long CORNERTORESTA = ATOREST * 360 / (2 * PI * RADIUS);
const long CORNERTORESTB = BTOREST * 360 / (2 * PI * RADIUSTRAN);
const long RESTTOBOARD = 5*360/(2*PI*RADIUS);
const long ENTRYENCODER = RESTTOBOARD;


typedef struct // represents a position on the board
{
  int row;
  int col;
} Position;

typedef struct // represents a list of positions
{
  Position arr[3]; // maximum size of 3 needed
} Pieces;

typedef struct { // represents playing board
	char arr[8][8];
} Board;

typedef struct { // represents a move
	short row;
	short col;
	short deltaX;
	short deltaY;
} Move;

typedef Move SequenceTreeList[20][10]; // represents a list of move sequences

typedef struct { // represents a list of moves
	Move arr[5];
} MoveArray;

typedef struct { // represents a move graph, used when generating binary tree
	Move arr[10];
} MoveGraph;

typedef struct { // represents a weighted move sequence
	Move sequence[5];
	int weighting;
} WeightedMoveSequence;

typedef struct { // represents a list of weighted move sequences
	WeightedMoveSequence arr[20];
} WeightedMoveSequenceArray;

SequenceTreeList list; //global list of total moves
WeightedMoveSequenceArray weightedMoveSequences; //global list of weighted moves
Board board; // global board

// Written by Jack
int decideTurn() //Returns true if the robot is first, false if human is first
{
	int randomNum = rand();

  if (randomNum % 2 == 0) { // robot first turn
		playSound(soundLowBuzz);
  	wait1Msec(2500);
  }

	else { // human first turn
		playSound(soundFastUpwardTones);
  	wait1Msec(2500);
	}
	return randomNum % 2;
}

// Written by Jack
void restPosition() // returns cart to origin
{
	if (nMotorEncoder[motorA] != 0) // if y-coordinate not already at origin
	{
		if (nMotorEncoder[motorA] > 0) // if the cart is in front of origin
		{
			motor[motorA] = -20;
			while(nMotorEncoder[motorA] > 0) //move until it hits 0
			{}
			motor[motorA] = 0;
		}
		else // if the cart is behind origin
		{
			motor[motorA] = 20;
			while(nMotorEncoder[motorA] < 0) //move until it hits 0
			{}
			motor[motorA] = 0;
		}

    motor[motorA] = -10;
    // adjustment to motor encoder of motor A
    while (nMotorEncoder[motorA] > 0.5 * 360 / (2 * PI * RADIUS))
    {}
    motor[motorA] = 0;

    nMotorEncoder[motorA] = 0;

	}

	if (nMotorEncoder[motorB] != 0) // if x-coordinate not already at origin
		{
			if (nMotorEncoder[motorB] > 0) //if the cart is to right of origin
			{
				motor[motorB] = -20;
				while(nMotorEncoder[motorB] > 0) //move until it hits 0
				{}
				motor[motorB] = 0;
			}
			else
			{
				motor[motorB] = 20;
				while(nMotorEncoder[motorB] < 0) //move until it hits 0
				{}
				motor[motorB] = 0;
			}
			motor[motorB] = 10; 
			// adjustment to motor encoder of motor B
    		while (nMotorEncoder[motorB] < 0.4 * 360 / (2 * PI * RADIUSTRAN)) {}
    		motor[motorB] = 0;
		}

		if (nMotorEncoder[motorC] != 0) //if magnet not at rest position
		{
			if (nMotorEncoder[motorC] > 0) //if above rest position
			{
				motor[motorC] = -20;
				while(nMotorEncoder[motorC] > 0)
				{}
				motor[motorC] = 0;
			}
			else //if below rest position
			{
				motor[motorC] = 20;
				while(nMotorEncoder[motorC] < 0)
				{}
				motor[motorB] = 0;
			}
		}
}

// Written by Jack
void restToFirstSquare() { //brings cart from origin to first square on board
	motor[motorA] = 20;
	while (nMotorEncoder[motorA] < RESTTOBOARD) {}
	motor[motorA] = 0;
	wait1Msec(500);
}

// Written by Jack
void moveToRestPosition() // brings cart from the corner to its rest position
{
	nMotorEncoder[motorA] = nMotorEncoder[motorB] = 0;

	motor[motorB] = 20;
	while(nMotorEncoder[motorB] < CORNERTORESTB) {
	}
	motor[motorB] = 0;

	motor[motorA] = 20;
	while(nMotorEncoder[motorA] < CORNERTORESTA) {
	}
	motor[motorA] = 0;

	nMotorEncoder[motorA] = nMotorEncoder[motorB] = 0;
	restPosition();

	wait1Msec(750);
}

 // Written by Ansh
void switchButton() { //turns the motor to toggle the button
	nMotorEncoder[motorD] = 0;
	motor[motorD] = 25;
	while(nMotorEncoder[motorD] < BUTTONENCODER) {}
	motor[motorD] = 0;
}

// Written by Ansh
void grabPiece() //grabs or drops a piece
{
	// lowers magnet
	nMotorEncoder[motorC] = 0;
	motor[motorC] = -20;
	while(nMotorEncoder[motorC] > (-1*DESCENDENCODER))
	{}
	motor[motorC] = 0;

	switchButton(); //turns button on/off

	//raises magnet
	motor[motorC] = 30;
	while(nMotorEncoder[motorC] < 0)
	{}
	motor[motorC] = 0;
}

// Written by Jack
void alignB(char direction) // Aligns the electromagnet onto a piece by moving left or right (sensor 1 = left, 2 = right)
{
	if (direction == 'L')
	{
		float moveEncoder = 0;
		moveEncoder = nMotorEncoder[motorB] - SQUAREENCODER - 0.1*SQUAREENCODER; //adjust

		motor[motorB] = -15;
		while(nMotorEncoder[motorB] > moveEncoder)
		{}
	}

	if (direction == 'R')
	{
		float moveEncoder = 0;
		moveEncoder = nMotorEncoder[motorB] + BOARDTRANENCODER / 7;

		motor[motorB] = 15;
		while(nMotorEncoder[motorB] < moveEncoder)
		{}
	}
	motor[motorB] = 0;
}

// Written by Jack
void alignCartA(int row) // aligns cart to grab a piece in the y direction
{
	if (nMotorEncoder[motorA] > ENTRYENCODER + SQUAREENCODER * row) // if it is ahead of target
	{
		motor[motorA] = -25;
		while(nMotorEncoder[motorA] > ENTRYENCODER + SQUAREENCODER * row)
		{}
		motor[motorA] = 0;
	}

	if (nMotorEncoder[motorA] < ENTRYENCODER + SQUAREENCODER * row) // if it is behind target
	{
		motor[motorA] = 25;
		while(nMotorEncoder[motorA] < ENTRYENCODER + SQUAREENCODER * row)
		{}
		motor[motorA] = 0;
	}
}

void alignCartB(int col) // aligns cart to grab a piece in the x direction
{
  if (nMotorEncoder[motorB] > SQUARETRANENCODER * col) // if it is to the right of target
  {
    motor[motorB] = -25;
    while(nMotorEncoder[motorB] > SQUARETRANENCODER * col)
    {}
    motor[motorB] = 0;
  }

  if (nMotorEncoder[motorB] < SQUARETRANENCODER * col) //if it is to the left of target
  {
    motor[motorB] = 25;
    while(nMotorEncoder[motorB] < SQUARETRANENCODER * col)
    {}
    motor[motorB] = 0;
  }
}

// Written by Jack
void removePiece(int row, int col, int *graveyard, char direction) // removes a piece from the board
{
	alignCartA(row);
    alignCartB(col);

	alignB(direction);
	grabPiece();

	// move cart in x direction to align with specific location in graveyard
    if (nMotorEncoder[motorB] > SQUARETRANENCODER * (long)((*graveyard) % 8))
    {
      motor[motorB] = -25;
      while(nMotorEncoder[motorB] > SQUARETRANENCODER * (long)((*graveyard) % 8))
      {}
      motor[motorB] = 0;
    }

    if (nMotorEncoder[motorB] < SQUARETRANENCODER * (long)((*graveyard) % 8))
    {
      motor[motorB] = 25;
      while(nMotorEncoder[motorB] < SQUARETRANENCODER * (long)((*graveyard) % 8)) {}
      motor[motorB] = 0;
    }

	alignB('L');

	motor[motorA] = -25;
	while(nMotorEncoder[motorA] > GRAVEYARDENCODER) {} // zero aka rest position
	motor[motorA] = 0;

	grabPiece();
	(*graveyard)++; // increment number of pieces that are out of the game

	restPosition();
}

// Written by Rania
void scanStore(int col, int row) //stores the results of scanning the board
{
	if (SensorValue[S1] == (int)colorBlack)
		board.arr[row][col] = '1';
	else if (SensorValue[S1] == (int)colorRed)
		board.arr[row][col] = '2';
	else
		board.arr[row][col] = 'O';

	if (SensorValue[S2] == (int)colorBlack)
		board.arr[row][col + 2] = '1';
	else if (SensorValue[S2] == (int)colorRed)
		board.arr[row][col + 2] = '2';
	else
		board.arr[row][col + 2] = 'O';
}

// Written by Ansh and Jack
void scanningMovement() //scans the board
{
	for(int col = 0; col < 4; col++)
	{
		int power = 0;
		if (col % 2 == 0) // determines whether to go forward or backward depending on the column
			power = 25;
		else
			power = -25;

		motor[motorA] = power;

		for(int nScan = 0; nScan < 8; nScan++) // goes through each square in the column
		{
			int realCol = col;
			if (col > 1)
				realCol += 2;
			int row = nScan;
			if (power<0)
				row = 7-nScan;
      		if ((col + row) % 2 == 1) { //if it is a white square
      			motor[motorA] = 0;
				wait1Msec(250);
				scanStore(realCol, row);
			}
			float adjust = 0.1*(360)/(2*PI*RADIUS); // adjust

			if(nScan < 7) { 
				// move in y direction
				motor[motorA] = power;
				if (power<0) {
					float trackAEncoder = nMotorEncoder[motorA] - SQUAREENCODER;
					while(nMotorEncoder[motorA] > trackAEncoder) {}
				}
				else {
					float trackAEncoder = nMotorEncoder[motorA] + SQUAREENCODER + adjust;
					while(nMotorEncoder[motorA] < trackAEncoder) {}
				}

				if((col + row) % 2 == 1)
					motor[motorA] = 0;
			}
		}
		// move in x direction at the end of columns
		if (col == 1)
		{
			float arcMovement = 0;
			arcMovement = nMotorEncoder[motorB] + BOARDTRANENCODER * 3 / 7;

			motor[motorB] = 20;

			while(nMotorEncoder[motorB] < arcMovement) {}

			motor[motorB] = 0;
		}
		if (col != 1 && col < 3)
		{
			float arcMovement = 0;
			if (col == 2)
				arcMovement = nMotorEncoder[motorB] + BOARDTRANENCODER / 7 - 1.5;
			else
				arcMovement = nMotorEncoder[motorB] + BOARDTRANENCODER / 7 - 1.5;

			motor[motorB] = 20;

			while(nMotorEncoder[motorB] < arcMovement) {}

			motor[motorB] = 0;
		}
	}
}

// Written by Ansh
void kingPiece(int col, int *nBlackKings, int *nRedKings, int colour) // kings a piece
{
  if (colour == (int)colorBlack)
	{
		// move to where kings are placed beside board
    	motor[motorA] = 20;
    	while(nMotorEncoder[motorA] < SQUAREENCODER * (long)(*nBlackKings + 1)) {}
    	motor[motorA] = 0;

		motor[motorB] = -20;
		while (nMotorEncoder[motorB] > -SQUARETRANENCODER) {}
		motor[motorB] = 0;
		
		//grab king
		alignB('L');
		grabPiece();

		motor[motorB] = 20;
		while(nMotorEncoder[motorB] < SQUARETRANENCODER * col)
		{}
		motor[motorB] = 0;

		motor[motorA] = -20;
		while(nMotorEncoder[motorA] > ENTRYENCODER)
		{}
		motor[motorA] = 0;

		(*nBlackKings)++; // changing nBlackKings
	}

	else
	{
    	motor[motorA] = 20;
    	while(nMotorEncoder[motorA] < SQUAREENCODER * (long)(*nRedKings + 1)) {}
    	motor[motorA] = 0;

    	motor[motorB] = -20;
    	while (nMotorEncoder[motorB] > -SQUARETRANENCODER) {}
    	motor[motorB] = 0;

    	alignB('L');
    	grabPiece();

    	motor[motorB] = 20;
    	while(nMotorEncoder[motorB] < SQUARETRANENCODER * col) {}
    	motor[motorB] = 0;

    	motor[motorA] = -20;
    	while(nMotorEncoder[motorA] > ENTRYENCODER) {}
    	motor[motorA] = 0;

		(*nRedKings)++; // changing nKings
	}

	//place king down
	alignB('L');
	grabPiece();
}

// Written by Ansh
void kingMe(int *nBlackKings, int *nRedKings, int *graveyard) // scan last row and king a piece if needed
{
	restPosition();
	// bring robot to last row from human's side
	motor[motorA] = 20;
	while(nMotorEncoder[motorA] < ENTRYENCODER) {}
	motor[motorA] = 0;

	motor[motorB] = 20;
	while (nMotorEncoder[motorB] < SQUARETRANENCODER) {}
	motor[motorB] = 0;
	wait1Msec(1000);

	if (SensorValue[S1] == (int)colorBlack) { // if there is a human piece
		removePiece(0, 1, graveyard, 'L');
		kingPiece(1, nBlackKings, nRedKings, (int)colorBlack);
		board.arr[0][1] = '3';
	}
	else if (SensorValue[S2] == (int)colorBlack) { // if there is a human piece
		removePiece(0, 3, graveyard, 'L');
		kingPiece(3, nBlackKings, nRedKings, (int)colorBlack);
		board.arr[0][3] = '3';
	}
	else // nothing was found in first pair, move to the right and check theres
	{
		motor[motorB] = 20;
		while (nMotorEncoder[motorB] < 5*SQUARETRANENCODER) {}
		motor[motorB] = 0;
		if (SensorValue[S1] == (int)colorBlack) {
			removePiece(0, 5, graveyard, 'L');
			kingPiece(5, nBlackKings, nRedKings, (int)colorBlack);
			board.arr[0][5] = '3';
		}
		else if (SensorValue[S2] == (int)colorBlack) {
			removePiece(0, 7, graveyard, 'R');
			kingPiece(7, nBlackKings, nRedKings,(int)colorBlack);
			board.arr[0][7] = '3';
		}
	}
	restPosition(); // return to origin
}

// Written by Jack and Ansh
void mechMove(int row, int col, int deltaX, int deltaY)
{
	// MOVE MOTOR A
	if (nMotorEncoder[motorA] < ENTRYENCODER + SQUAREENCODER*(row+0.2)) {
  	motor[motorA] = 20;
  	while(nMotorEncoder[motorA] < ENTRYENCODER + SQUAREENCODER*(row+0.2)) {}
  }
  else {
  	motor[motorA] = -20;
  	while(nMotorEncoder[motorA] > ENTRYENCODER + SQUAREENCODER*(row)) {}
  }
  motor[motorA] = 0;

  // MOVE MOTOR B
  if (nMotorEncoder[motorB] < SQUARETRANENCODER * col) {
  	motor[motorB] = 20;
  	while(nMotorEncoder[motorB] < SQUARETRANENCODER * (col)) {}
  }
  else {
  	motor[motorB] = -20;
  	while(nMotorEncoder[motorB] > SQUARETRANENCODER * (col)) {}
  }
  motor[motorB] = 0;

  // ALIGN MAGNET
  if (col == 7)
  	alignB('R');
 	else if (col < 6)
 		alignB('L');

  grabPiece();

  wait1Msec(1000);

  // move to new target
  if (deltaY > 0)
  {
    motor[motorA] = 20;
    while(nMotorEncoder[motorA] < ENTRYENCODER + SQUAREENCODER*(row+deltaY+0.2)) {}
  }
  else
  {
    motor[motorA] = -20;
    while(nMotorEncoder[motorA] > ENTRYENCODER + SQUAREENCODER*(row+deltaY-0.2)) {}
  }
  motor[motorA] = 0;

  long currentBEncoder = nMotorEncoder[motorB];

  long adjust = 1*360/(2*PI*RADIUSTRAN);
  if (deltaX < 0)
  {
    motor[motorB] = -20;
    while(nMotorEncoder[motorB] > currentBEncoder + deltaX*SQUAREENCODER + deltaX*adjust) {}
  }
  else
  {
    motor[motorB] = 20;
    while(nMotorEncoder[motorB] < currentBEncoder + deltaX*SQUAREENCODER + deltaX*adjust) {}
  }
  motor[motorB] = 0;

  // drop piece
  grabPiece();
}

// Written by Noah
bool endGame() // check if game has ended
{
	int remainingHumanPiece = 0, remainingRobotPiece = 0;

	for(int pass = 0; pass < 8; pass++)
	{
		for(int index = 0; index < 8; index++)
		{
			if (board.arr[pass][index] == '1' || board.arr[pass][index] == '3')
				remainingHumanPiece++;

			if (board.arr[pass][index] == '2' || board.arr[pass][index] == '4')
				remainingRobotPiece++;
		}
	}

	bool isGameOver = false;
	if (remainingHumanPiece == 0 || remainingRobotPiece == 0 ||
		(remainingHumanPiece == 1 && remainingRobotPiece == 1))
	{
		isGameOver = true;
	}
	return isGameOver;
}

/////////////////////////////////////////////////////////    AI    //////////////////////////////////////////////////////////////

// Written by Ansh
bool isInvalid (Move move) { // checks if passed move is invalid (different from illegal)
	if (move.row < 0 || move.row > 7)
		return true;

	return false;
}

// Written by Ansh
bool isJump (Move move) { // checks if passed move is a jump
	if (abs(move.deltaX) == 2)
		return true;
	return false;
}

// Written by Ansh
void makeMove (Move move) { // updates board according to passed move
	char piece = board.arr[move.row][move.col];
	board.arr[move.row][move.col] = 'O';
	board.arr[move.row+move.deltaY][move.col+move.deltaX] = piece;
}

// Written by Ansh
bool isUnderAttack (Move move) { //Returns whether moving the piece to the new position would place it under attackss
	makeMove(move);
	const char EMPTY = 'O', BLACK = '1', BLACK_KING = '3';
	int newRow = move.row + move.deltaY, newCol = move.col + move.deltaX;
	bool underAttack = false;

	if (newRow+1 < 8 && newCol+1 < 8 && (board.arr[newRow+1][newCol+1] == BLACK || board.arr[newRow+1][newCol+1] == BLACK_KING) && newRow-1 >= 0 && newCol-1 >= 0 && board.arr[newRow-1][newCol-1] == EMPTY
	|| newRow+1 < 8 && newCol-1 >= 0 && (board.arr[newRow+1][newCol-1] == BLACK || board.arr[newRow+1][newCol-1] == BLACK_KING) && newRow-1 >= 0 && newCol+1 < 8 && board.arr[newRow-1][newCol+1] ==  EMPTY
	|| newRow-1 >= 0 && newCol+1 < 8 && board.arr[newRow-1][newCol+1] == BLACK_KING && newRow+1 < 8 && newCol-1 >= 0 && board.arr[newRow+1][newCol-1] == EMPTY
	|| newRow-1 >= 0 && newCol-1 >= 0 && board.arr[newRow-1][newCol-1] == BLACK_KING && newRow+1 < 8 && newCol+1 < 8 && board.arr[newRow+1][newCol+1] == EMPTY)
		underAttack = true;

	Move reverseMove;
	reverseMove.row = newRow;
	reverseMove.col = newCol;
	reverseMove.deltaY = move.deltaY*-1;
	reverseMove.deltaX = move.deltaX*-1;
	makeMove(reverseMove);

	return underAttack;
}

// Written by Ansh
bool isIndirectlyUnderAttack(Move move) { // Returns whether moving the piece to the new position would place another piece under attack
	const char BLACK = '1';
	const char RED = '2';
	const char BLACK_KING = '3';
	const char RED_KING = '4';
	int row = move.row, col = move.col;
	if (row+2 < 8 && col+2 < 8 && (board.arr[row+1][col+1] == RED || board.arr[row+1][col+1] == RED_KING) && board.arr[row+2][col+2] == BLACK
	|| (row+2 < 8 && col+1 < 8 && col-2 >= 0 && (board.arr[row+1][col-1] == RED || board.arr[row+1][col+1] == RED_KING)  && board.arr[row+2][col-2] == BLACK)
	|| (row-2 >= 0 && col+2 < 8 && (board.arr[row-1][col+1] == RED || board.arr[row-1][col+1] == RED_KING) && board.arr[row-2][col+2] == BLACK_KING)
	|| (row-2 >= 0 && col-2 >= 0 && (board.arr[row-1][col-1] == RED || board.arr[row-1][col-1] == RED_KING) && board.arr[row-2][col-2] == BLACK_KING))
		return true;
	return false;
}

// Written by Ansh
bool isLegalMove (Board *board, int row, int col, int deltaY, int deltaX) { // Move is legal if there is no piece in the new position and it is not off the board
	if (row + deltaY >= 8 || row + deltaY < 0 || col + deltaX >= 8 || col + deltaX < 0 ||
	board->arr[row+deltaY][col+deltaX] != 'O')
		return false;
	return true;
}

// Written by Ansh
void addMovesToCurrentSequence (int row, int col, MoveGraph* moveGraph, int node) { // USES ARRAY BINARY TREE: 2i+1 is left node, 2i+2 is right node
	/* This is a recursive function that adds moves to a binary tree (moveGraph)
	to capture any double or triple moves that might be possible on the board
	*/
	const char BLACK = '1';

	if (row+1 < 8 && col-1 >= 0 && board.arr[row+1][col-1] == BLACK && isLegalMove (board, row, col, 2, -2)) { // if there is a black piece to attack on left
		Move move;
		move.row = row;
		move.col = col;
		move.deltaY = 2;
		move.deltaX = -2;
		moveGraph->arr[2*node+1].row = move.row;
		moveGraph->arr[2*node+1].col = move.col;
		moveGraph->arr[2*node+1].deltaX = move.deltaX;
		moveGraph->arr[2*node+1].deltaY = move.deltaY;
		addMovesToCurrentSequence (row+2, col-2, moveGraph, 2*node+1);
	}
	if (row+1 < 8 && col+1 < 8 && board.arr[row+1][col+1] == BLACK && isLegalMove (board, row, col, 2, 2)) { // if there is a black piece to attack on right
		Move move;
		move.row = row;
		move.col = col;
		move.deltaY = 2;
		move.deltaX = 2;
		moveGraph->arr[2*node+2].row = move.row;
		moveGraph->arr[2*node+2].col = move.col;
		moveGraph->arr[2*node+2].deltaX = move.deltaX;
		moveGraph->arr[2*node+2].deltaY = move.deltaY;
		addMovesToCurrentSequence (row+2, col+2, moveGraph, 2*node+2);
	}
}

// Written by Ansh
int addAllMoveSequencesForPiece (int row, int col, short size) { // adds all the possible moves for the passed piece (row,col) into the global list of moves
	const char EMPTY = 'O', BLACK = '1';
	int dir = 0;
	if (row+2 < 8 && col-2 >= 0 && board.arr[row+1][col-1] == BLACK && board.arr[row+2][col-2] == EMPTY) //if there is black piece on top left to attack
		dir = -1;
	else if (row+2 < 8 && col+2 < 8 && board.arr[row+1][col+1] == BLACK && board.arr[row+2][col+2] == EMPTY) //if there is black piece on top right to attack
		dir = 1;

	if (dir != 0) { //if there is a black piece to attack
		MoveGraph moveGraph;
		for (int i = 0 ; i < 10 ; i++) {
			Move invalid;
			invalid.row = -1;
			moveGraph.arr[i].row = invalid.row;
		}
		// create move
		Move move;
		move.row = row;
		move.col = col;
		move.deltaY = 2;
		move.deltaX = 2*dir;
		moveGraph.arr[0].row = move.row;
		moveGraph.arr[0].col = move.col;
		moveGraph.arr[0].deltaX = move.deltaX;
		moveGraph.arr[0].deltaY = move.deltaY;
		addMovesToCurrentSequence (row + 2, col + 2*dir, &moveGraph, 0); // adds to the move sequence if there is a double or triple move present
		for (int i = 0 ; i < 10 ; i++) { // adds to the global move list
			list[size][i].row = moveGraph.arr[i].row;
			list[size][i].col = moveGraph.arr[i].col;
			list[size][i].deltaX = moveGraph.arr[i].deltaX;
			list[size][i].deltaY = moveGraph.arr[i].deltaY;
		}
		size++;
	}
	else { //if there are no black pieces to attack
		if (isLegalMove(board, row, col, 1, 1)) { //check top right
			Move move;
			move.row = row;
			move.col = col;
			move.deltaY = 1;
			move.deltaX = 1;
			Move invalid;
			invalid.row = -1;
			list[size][0].row = move.row;
			list[size][0].col = move.col;
			list[size][0].deltaX = move.deltaX;
			list[size][0].deltaY = move.deltaY;
			list[size][1].row = invalid.row;
			list[size][2].row = invalid.row;
			size++;
		}
		if (isLegalMove(board, row, col, 1, -1)) { //check top left
			Move move;
			move.row = row;
			move.col = col;
			move.deltaY = 1;
			move.deltaX = -1;
			Move invalid;
			invalid.row = -1;
			list[size][0].row = move.row;
			list[size][0].col = move.col;
			list[size][0].deltaX = move.deltaX;
			list[size][0].deltaY = move.deltaY;
			list[size][1].row = invalid.row;
			list[size][2].row = invalid.row;
			size++;
		}
	}
	return size;
}

// Written by Ansh
void copyArray (MoveArray* oldArray, MoveArray* newArray, int size) { //copies the contents of old array into new array
	for (int i = 0 ; i < size ; i++) {
		newArray->arr[i].row = oldArray->arr[i].row;
		newArray->arr[i].col = oldArray->arr[i].col;
		newArray->arr[i].deltaX = oldArray->arr[i].deltaX;
		newArray->arr[i].deltaY = oldArray->arr[i].deltaY;
	}
}

// Written by Ansh
void traverseGraph (MoveGraph* graph, int graphIndex, MoveArray* sequence, int size, int* sequenceIndex) { //converts binary tree into linear sequences
	if (isInvalid(graph->arr[2*graphIndex+1]) && isInvalid(graph->arr[2*graphIndex+2])) { //if we have reached the leaf node
		MoveArray newSequence;
		copyArray(sequence, &newSequence, size);
		newSequence.arr[size].row = graph->arr[graphIndex].row;
		newSequence.arr[size].col = graph->arr[graphIndex].col;
		newSequence.arr[size].deltaX = graph->arr[graphIndex].deltaX;
		newSequence.arr[size].deltaY = graph->arr[graphIndex].deltaY;
		Move invalid;
		invalid.row = -1;
		newSequence.arr[size+1].row = invalid.row;
		for (int i = 0 ; i < size+2 ; i++) {
			list[*sequenceIndex][i].row = newSequence.arr[i].row;
			list[*sequenceIndex][i].col = newSequence.arr[i].col;
			list[*sequenceIndex][i].deltaX = newSequence.arr[i].deltaX;
			list[*sequenceIndex][i].deltaY = newSequence.arr[i].deltaY;
		}
		(*sequenceIndex)++;
	}
	else { //if there are more children of the node
		if (!isInvalid(graph->arr[2*graphIndex+1])) {
			MoveArray leftSequence;
			copyArray(sequence, &leftSequence, size);
			leftSequence.arr[size].row = graph->arr[graphIndex].row;
			leftSequence.arr[size].col = graph->arr[graphIndex].col;
			leftSequence.arr[size].deltaX = graph->arr[graphIndex].deltaX;
			leftSequence.arr[size].deltaY = graph->arr[graphIndex].deltaY;
			traverseGraph(graph, 2*graphIndex+1, &leftSequence, size+1, sequenceIndex);
		}
		if (!isInvalid(graph->arr[2*graphIndex+2])) {
			MoveArray rightSequence;
			copyArray(sequence, &rightSequence, size);
			rightSequence.arr[size].row = graph->arr[graphIndex].row;
			rightSequence.arr[size].col = graph->arr[graphIndex].col;
			rightSequence.arr[size].deltaX = graph->arr[graphIndex].deltaX;
			rightSequence.arr[size].deltaY = graph->arr[graphIndex].deltaY;
			traverseGraph(graph, 2*graphIndex+2, &rightSequence, size+1, sequenceIndex);
		}
	}
}

// Written by Ansh
void createMoveSequences(short size) { //converts list of binary trees into list of linear sequences
	int sequenceIndex = 0;
	for (short i = 0 ; i < size ; i++) {
		MoveArray sequence;
		MoveGraph graph;
		for (int x = 0 ; x < 10 ; x++) {
			graph.arr[x].row = list[i][x].row;
			graph.arr[x].col = list[i][x].col;
			graph.arr[x].deltaX = list[i][x].deltaX;
			graph.arr[x].deltaY = list[i][x].deltaY;
		}
		traverseGraph(&graph, 0, &sequence, 0, &sequenceIndex);
	}
}

// Written by Ansh
int calculateWeighting (MoveArray* sequence) { //calculates weighting of the passed move sequence
	int weighting = 0;
	for (int i = 0 ; !isInvalid(sequence->arr[i]) ; i++) { //goes through sequence and checks each move
		if (isJump(sequence->arr[i])) { // if move is a jump
			weighting += 1;
		}
		else { //if move is not a jump
			if (isUnderAttack(sequence->arr[i])) {
				weighting += -1;
			}
			if (isIndirectlyUnderAttack(sequence->arr[i])) {
				weighting += -1;
			}
		}
	}
	return weighting;
}

// Written by Ansh
void assignWeightings (short size) { // assigns weighting to all the move sequences generated
	for (short i = 0 ; i < size ; i++) {
		int x;
		Move invalid;
		invalid.row = -1;
		MoveArray movArr;
		for (x = 0 ; !isInvalid(list[i][x]) ; x++) {
			movArr.arr[x].row = list[i][x].row;
			movArr.arr[x].col = list[i][x].col;
			movArr.arr[x].deltaX = list[i][x].deltaX;
			movArr.arr[x].deltaY = list[i][x].deltaY;
			weightedMoveSequences.arr[i].sequence[x].row = list[i][x].row;
			weightedMoveSequences.arr[i].sequence[x].col = list[i][x].col;
			weightedMoveSequences.arr[i].sequence[x].deltaX = list[i][x].deltaX;
			weightedMoveSequences.arr[i].sequence[x].deltaY = list[i][x].deltaY;
		}
		weightedMoveSequences.arr[i].sequence[x].row = invalid.row;
		movArr.arr[x].row = invalid.row;
		weightedMoveSequences.arr[i].weighting = calculateWeighting(movArr);
	}
}

// Written by Ansh
int selectMove (short size, Move* move) { // selects best move sequence based on weighting
	int max = weightedMoveSequences.arr[0].weighting, maxIndex = 0;
	for (short i = 1 ; i < size ; i++) {
		if (weightedMoveSequences.arr[i].weighting > max) {
			max = weightedMoveSequences.arr[i].weighting;
			maxIndex = i;
		}
	}
	int row = weightedMoveSequences.arr[maxIndex].sequence[0].row, col = weightedMoveSequences.arr[maxIndex].sequence[0].col, totalX = 0, totalY = 0;
	for (int i = 0 ; !isInvalid(weightedMoveSequences.arr[maxIndex].sequence[i]) ; i++) { // adds up movements in chosen sequence to generate total deltaX,deltaY
		totalX += weightedMoveSequences.arr[maxIndex].sequence[i].deltaX;
		totalY += weightedMoveSequences.arr[maxIndex].sequence[i].deltaY;
	}
	//populates move
	move->row = row;
	move->col = col;
	move->deltaY = totalY;
	move->deltaX = totalX;
	return maxIndex;
}

// Written by Ansh
int generateRemovalPieces(int maxIndex, Pieces* pieces) { // generate pieces to remove (if any) based on chosen move
	int size = 0;
	while (!isInvalid(weightedMoveSequences.arr[maxIndex].sequence[size]) && abs(weightedMoveSequences.arr[maxIndex].sequence[size].deltaX) == 2) {
		pieces->arr[size].row = weightedMoveSequences.arr[maxIndex].sequence[size].row+(weightedMoveSequences.arr[maxIndex].sequence[size].deltaY/2);
		pieces->arr[size].col = weightedMoveSequences.arr[maxIndex].sequence[size].col+(weightedMoveSequences.arr[maxIndex].sequence[size].deltaX/2);
		size++;
	}
	return size;
}

// Written by Ansh
void clearLists (short size) { // clears lists of moves
	for (int i = 0 ; i < 20 ; i++) {
		for (int x = 0 ; x < 10 ; x++) {
			list[i][x].row = -1;
		}
		weightedMoveSequences.arr[i].weighting = 0;
		for (int y = 0 ; y < 5 ; y++) {
			weightedMoveSequences.arr[i].sequence[y].row = -1;
		}
	}
}

// Written by Ansh and Jack
task main()
{
	const char RED = '2', BLACK = '1', EMPTY = 'O';
	int nBlackKings = 0, nRedKings = 0, graveyard = 8;
	short size = 0;

	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);

	SensorType[S2] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Color_Color;
	wait1Msec(50);

	SensorType[S3] = sensorEV3_Touch;
	SensorType[S4] = sensorEV3_Ultrasonic;

	moveToRestPosition(); // moves from corner to default rest position

	int turn = decideTurn();
	if (turn == 0) { //robot's turn
		mechMove(2, 3, 1, 1); //hardcoded optimal first move
		restPosition(); // moves back to rest
		playSound(soundBlip);
		wait1Msec(1000);
	}
	time1[T1] = 0; // starts timer 
	while (!endGame()) {
		while (SensorValue[S3] == 0) { //while button not pressed
			if (SensorValue[S4] < 20) { //if ultrasonic value drops below 20 (hand flash)
				kingMe(&nBlackKings, &nRedKings, &graveyard);
			}
			if (time1[T1] > 30000) {
				playSound(soundLowBuzz); //REMINDER BUZZER
				wait1Msec(1000);
				time1[T1] = 0;
			}
		}
		while (SensorValue[S3] == 1) {} //wait for button to be released

		playSound(soundBeepBeep);
		wait1Msec(500);
		restToFirstSquare(); //move onto board
		scanningMovement(); //scan board

		//create a binary tree of all possible moves for each piece
		for (int row = 0; row < 8 ; row++) {
			for (int col = 0 ; col < 8 ; col++) {
				if (board.arr[row][col] == RED) {
					size = addAllMoveSequencesForPiece (row, col, size);
				}
			}
		}

		// convert trees into linear sequences of moves for each piece
		createMoveSequences(size);

		assignWeightings(size); // assign weightings to moves

		Move selectedMove; //selected move
		int maxIndex = selectMove(size, &selectedMove);
		Pieces removalPieces; //list of pieces to remove
		int remove = generateRemovalPieces(maxIndex, &removalPieces);

		mechMove(selectedMove.row, selectedMove.col, selectedMove.deltaX, selectedMove.deltaY); // perform selected move
		for (int i = 0 ; i < remove ; i++) { //remove pieces that were jumped
			removePiece(removalPieces.arr[i].row, removalPieces.arr[i].col, &graveyard, 'L');
		}
		restPosition(); // back to rest
		clearLists(size); //clear lists
		size = 0;
		playSound(soundBlip);
		wait1Msec(1000);
		time1[T1] = 0;
	}
}
