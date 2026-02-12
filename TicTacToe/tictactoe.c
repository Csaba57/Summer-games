#include <stdio.h>
#include <ncurses.h>

#define WIDTH 10 //10
#define HEIGHT 5 //5
#define POINTS_TO_WIN 5 //3

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

void releaseWindow(WINDOW **window) {
    wclear(*window);
    wrefresh(*window);
    delwin(*window);
    window = NULL;

    clear();
    refresh();
}

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

void drawField() {
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    mvwprintw(window, i + 1, j + 1, "%c", field.matrix[i][j]);
 	}
    }
    wrefresh(window);
}

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
