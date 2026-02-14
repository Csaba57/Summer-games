#include <stdio.h>
#include <ncurses.h>

#define WIDTH 10
#define HEIGHT 5
#define POINTS_TO_WIN 5

typedef struct {
    int width;
    int height;
    char emptyCellChar;
    char matrix[HEIGHT][WIDTH];
}Field;


Field field;
char player1Char;
char player2Char;
//ncurses
int screenWidth;
int screenHeight;
WINDOW *window;
int turnTextRow;
int turnTextCol;
int resultTextRow;
int resultTextCol;


void init();
int fieldIsFull();
void drawField();
int checkWinCondition(char playerChar);
//ncurses
void initWindow();
void releaseWindow(WINDOW **window);
void handleInput(int cursorPosition[]);
void blink();

/*
First it initializes the basic window and draws the field.
Sets the basic cursor positions for both players and enters the main game loop.
After each turn it checks if the win condition is met.
If any of the players win or if the game ends in a draw the appropriate text is displayed.
When the game ends it closes the main window and frees the memory used by the window.

param: none
return: 0 (int) if the game has ended successfully
*/
int main() {

    initscr();
    getmaxyx(stdscr, screenHeight, screenWidth);

    init();
    initWindow();
    noecho();


    drawField();

    int playerNumber = 1;
    int p1Cursor[2] = {1, 1}; // [0] - row, [1] - col
    int p2Cursor[2] = {1, 1};
    while (1) {
	move(turnTextRow, turnTextCol);
	printw((playerNumber == 1) ? "Player1's [x] turn:" :  "Player2's [o] turn:");
	refresh();

	if (playerNumber == 1) {
	    do {
		handleInput(p1Cursor);
	    } while (field.matrix[p1Cursor[0] - 1][p1Cursor[1] - 1] != field.emptyCellChar);

	    field.matrix[p1Cursor[0] - 1][p1Cursor[1] - 1] = player1Char;
	    playerNumber++;
	} else {
	    do {
		handleInput(p2Cursor);
	    } while (field.matrix[p2Cursor[0] - 1][p2Cursor[1] - 1] != field.emptyCellChar);

	    field.matrix[p2Cursor[0] - 1][p2Cursor[1] - 1] = player2Char;
	    playerNumber = 1;
	}
	drawField();


	if (checkWinCondition(player1Char)) {
	    move(turnTextRow, turnTextCol);
	    printw("                      ");
	    refresh();

	    drawField();
	    wrefresh(window);

	    move(resultTextRow, resultTextCol);
	    printw("Player1 won the game!\n");
	    curs_set(0);
	    refresh();

	    blink();
	    break;
	} else if (checkWinCondition(player2Char)) {
	    move(turnTextRow, turnTextCol);
	    printw("                      ");
	    refresh();

	    drawField();
	    wrefresh(window);

	    move(resultTextRow, resultTextCol);
	    printw("Player2 won the game!\n");
	    curs_set(0);
	    refresh();

	    blink();
	    break;
	}
	if (fieldIsFull()) {
	    move(turnTextRow, turnTextCol);
	    printw("                      ");
	    refresh();

	    drawField();
	    wrefresh(window);

	    resultTextCol += 8;
	    move(resultTextRow, resultTextCol);

	    printw("Draw");
	    curs_set(0);
	    refresh();
	    break;
	}

    }

    napms(2000);
    releaseWindow(&window);
    endwin();
    return 0;
}

/*
Sets the values of the field properties and initializes the player characters.

param: none
return: none
*/
void init() {
    field.width = WIDTH;
    field.height = HEIGHT;
    field.emptyCellChar = '.';
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    field.matrix[i][j] = field.emptyCellChar;
	}
    }
    player1Char = 'x';
    player2Char = 'o';
}


/*
Initializes the bordered game window.
Places the game text to the correct position.

param: none
return: none
*/
void initWindow() {
    int windowHeight = field.height + 2;
    int windowWidth = field.width + 2;
    window = newwin(windowHeight, windowWidth, (screenHeight / 2) - (windowHeight / 2), (screenWidth / 2) - (windowWidth / 2));
    box(window, 0, 0);
    refresh();
    wrefresh(window);
    keypad(window, TRUE);


    turnTextRow = (screenHeight / 2) - (windowHeight / 2) - 1;
    turnTextCol = (screenWidth / 2) - (windowWidth / 2) - 3;
    resultTextRow = (screenHeight / 2) + (windowHeight / 2) + 1;
    resultTextCol = (screenWidth / 2) - (windowWidth / 2) - 4;
}


/*
Clears and deletes the given window. Frees up the memory afterwards.

param: pointer to window pointer (WINDOW**)
return: none
*/
void releaseWindow(WINDOW **window) {
    wclear(*window);
    wrefresh(*window);
    delwin(*window);
    *window = NULL;

    clear();
    refresh();
}

/*
Checks if there is no empty space on the field.

param: none
return: 1 (int) if the field has no mor empty space, 0 (int) otherwise
*/
int fieldIsFull() {
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    if (field.matrix[i][j] == field.emptyCellChar) {
		return 0;
	    }
 	}
    }
    return 1;
}

/*
Prints the current state of the field to the screen.

param: none
return: none
*/
void drawField() {
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    mvwprintw(window, i + 1, j + 1, "%c", field.matrix[i][j]);
 	}
    }
    wrefresh(window);
}

/*
Checks for identical characters horizontally, vertically, and diagonally.

param: a character that represents a player (char)
return 1 (int) if there are enough characters (POINTS_TO_WIN) near each other, 0 (int) otherwise
*/
int checkWinCondition(char playerChar) {
    int points = 0;
    //horizontal check
    for (int i = 0; i < field.height; i++) {
	points = 0;
	for (int j = 0; j < field.width; j++) {
	    if (field.matrix[i][j] == playerChar) {
		points++;
	    } else {
		points = 0;
	    }

	    if (points == POINTS_TO_WIN) {
	        //horizontal highlight
		int counter = points;
		int col = j;
		while (counter > 0) {
		    field.matrix[i][col] = '#';
		    counter--;
		    col--;

		    drawField();
		    napms(200);
	        }

		return 1;
	    }
	}
    }

    //vertical check
    points = 0;
    for (int i = 0; i < field.width; i++) {
	points = 0;
	for (int j = 0; j < field.height; j++) {
	    if (field.matrix[j][i] == playerChar) {
		points++;
	    } else {
		points = 0;
	    }

	    if (points == POINTS_TO_WIN) {
		//vertical highlight
		int counter = points;
		int row = j;
		while (counter > 0) {
		    field.matrix[row][i] = '#';
		    counter--;
		    row--;

		    drawField();
		    napms(200);
	        }

		return 1;
	    }
	}
    }

    //diagonal check
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    //top left -> bottom right
	    int row = i;
	    int col = j;
	    points = 0;
	    while (row < field.height && col < field.width) {
		if (field.matrix[row][col] == playerChar) {
		    points++;
		} else {
		    points = 0;
		}

		if (points == POINTS_TO_WIN) {
		    //diagonal highlight (top left <- bottom right)
		    int counter = points;
		    while (counter > 0) {
		        field.matrix[row][col] = '#';
			row--;
		        col--;
			counter--;

			drawField();
		        napms(200);
	           }

		    return 1;
		}
		row++;
		col++;
	    }

	    //top right -> bottom left
	    row = i;
	    col = j;
	    points = 0;
	    while (row < field.height && col >= 0) {
		if (field.matrix[row][col] == playerChar) {
		    points++;
		} else {
		    points = 0;
		}

		if (points == POINTS_TO_WIN) {
		    //diagonal highlight (top right <- bottom left)
		    int counter = points;
		    while (counter > 0) {
		        field.matrix[row][col] = '#';
			row--;
		        col++;
			counter--;

			drawField();
		        napms(200);
	           }

		    return 1;
		}
		row++;
		col--;
	    }

	}
    }

    return 0;
}

/*
Gets the palyer input and moves the cursor accordingly.
If the input is the enter or space key and the chosen position is empty the function terminates.

param: an array, which cotains the row and column of the cursor's position (int[])
return: none
*/
void handleInput(int cursorPosition[]) {
    wmove(window, cursorPosition[0], cursorPosition[1]);
    wrefresh(window);

    int input = '0';
    do {
	input = wgetch(window);
	switch (input) {
	    case 'w':
	    case KEY_UP:
		if (cursorPosition[0] - 1 > 0) {
		    cursorPosition[0]--;
		}
	    break;
	    case 's':
	    case KEY_DOWN:
		if (cursorPosition[0] - 1 < field.height - 1) {
		    cursorPosition[0]++;
		}
	    break;
	    case 'a':
	    case KEY_LEFT:
		if (cursorPosition[1] - 1 > 0) {
		    cursorPosition[1]--;
		}
	    break;
	    case 'd':
	    case KEY_RIGHT:
		if (cursorPosition[1] - 1 < field.width - 1) {
		    cursorPosition[1]++;
		}
	}
        wmove(window, cursorPosition[0], cursorPosition[1]);
	wrefresh(window);
    } while(input != '\n' && input != ' ');
}

/*
If there are '#' characaters on the field this function blinks them a given amount of times (blinkCnt)
blink: switching between '#' and '$' characters

param: none
retrun: none
*/
void blink() {
    int blinkCnt = 30;

    while (blinkCnt > 0) {
	for (int i = 0; i < field.height; i++) {
            for (int j = 0; j < field.width; j++) {
	        if (field.matrix[i][j] == '#') {
		    field.matrix[i][j] = '$';
	        } else if (field.matrix[i][j] == '$'){
		    field.matrix[i][j] = '#';
	        }
            }
        }
        drawField();
        napms(100);
        blinkCnt--;
    }
}
