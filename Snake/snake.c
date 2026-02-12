#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
/*in this code
	.x field means row
	.y field means column
	for all structs
not the same as the mathematical notation
*/

#define FIELD_WIDTH 28
#define FIELD_HEIGHT 14
#define APPLE_START_X 1
#define APPLE_START_Y 9
#define SNAKE_HEAD_START_X 0
#define SNAKE_HEAD_START_Y 3


typedef struct {
    int width;
    int height;
    char repChar;
    char **matrix;
}Field;

typedef struct {
    int x;
    int y;
    char repChar;
}Node;

typedef struct {
    int x;
    int y;
    char repChar;
}Apple;

typedef struct {
    Node *body;
    int bodyLength;
}Snake;

typedef struct {
    int width;
    int height;
}Screen;

void init();
void freeResources();
void syncFieldMatrix();
void drawField();
int checkWinCondition();
void handleInput();
void moveSnake();
int checkAppleCollision();
void randomizeApplePosition(Apple *fruit);
void expandSnake();
void fillField();
int checkGameOver();
void initNcurses();
void endNcurses();
void initWindow();
void releaseWindow(WINDOW **window);
void redrawPoints();
void displayResult(char *text);

Field field;
Apple apple;
Snake snake;
int points;
int playerInput;
//movement directions
int direction;
/*
1 - north
2 - east
3 - south
4 - west
*/
Screen screen;
WINDOW *window;
int pointsPosX;
int pointsPosY;


int main() {

    initNcurses();
    curs_set(0);
    noecho();
    getmaxyx(stdscr, screen.height, screen.width);


    srand(time(NULL));
    init();
    initWindow();
    keypad(window, TRUE);

    drawField();
    while (1) {
	playerInput = wgetch(window);

	handleInput();
	moveSnake();

	if (checkGameOver()) {
	    releaseWindow(&window);
	    displayResult("GAME OVER");
	    break;
	}

	if (checkAppleCollision()) {
	    points++;
	    redrawPoints();
	    expandSnake();
	    randomizeApplePosition(&apple);
	}
	syncFieldMatrix();

	if (checkWinCondition()) {
	    fillField();
	    drawField();
	    napms(2000);
	    releaseWindow(&window);
	    displayResult("All apples eaten sucessfully!");
	    break;
	}
	drawField();
    }

    freeResources();
    endNcurses();
    return 0;
}



void init() {
    field.width = FIELD_WIDTH;
    field.height = FIELD_HEIGHT;
    field.repChar = '.';
    field.matrix = (char**)malloc(field.height * sizeof(char*));
    for (int i = 0; i < field.height; i++) {
	*(field.matrix + i) = (char*)malloc(field.width * sizeof(char));
    }
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    field.matrix[i][j] = field.repChar;
	}
    }

    apple.x = APPLE_START_X;
    apple.y = APPLE_START_Y;
    apple.repChar = '@';

    snake.bodyLength = 3;
    snake.body = (Node*)malloc(snake.bodyLength * sizeof(Node));
    snake.body[0].x = SNAKE_HEAD_START_X;
    snake.body[0].y = SNAKE_HEAD_START_Y;
    snake.body[0].repChar = '#';
    for (int i = 1; i < snake.bodyLength; i++) {
	snake.body[i].x = snake.body[i - 1].x;
	snake.body[i].y = snake.body[i - 1].y - 1;
	snake.body[i].repChar = '#';
    }

    syncFieldMatrix();
    direction = 2;
}

void freeResources() {
    for (int i = 0; i < field.height; i++)
	free(*(field.matrix + i));
    free(field.matrix);
    field.matrix = NULL;
    free(snake.body);
    snake.body = NULL;
}

void syncFieldMatrix() {
    char karakter;
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    if (i == apple.x && j == apple.y) {
		karakter = apple.repChar;
	    } else {
		karakter = field.repChar;
	    }
	    for (int k = 0; k < snake.bodyLength; k++) {
		if (i == snake.body[k].x && j == snake.body[k].y) {
		    karakter = snake.body[k].repChar;
		    break;
		}
	    }

	    field.matrix[i][j] = karakter;
	}
    }
}

void drawField() {
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    if (has_colors()) {
	        switch (field.matrix[i][j]) {
	            case '.': wattron(window, COLOR_PAIR(2)); break;
	            case '#': wattron(window, COLOR_PAIR(1));  break;
	            case '@': wattron(window, COLOR_PAIR(3));  break;
	        }
	        mvwprintw(window, i + 1, j + 1, "%c", field.matrix[i][j]);
	    } else {
		mvwprintw(window, i + 1, j + 1, "%c", field.matrix[i][j]);
	    }
	}
    }
    wrefresh(window);
}

int checkWinCondition() {
    return snake.bodyLength == field.width * field.height;
}

void moveSnake() {
    for (int i = snake.bodyLength - 1; i > 0; i--) {
	snake.body[i].x = snake.body[i - 1].x;
	snake.body[i].y = snake.body[i - 1].y;
    }

    if (direction == 1) { //north
	snake.body[0].x--;
    } else if (direction == 3) { //south
	snake.body[0].x++;
    } else if (direction == 4) { //west
	snake.body[0].y--;
    } else if (direction == 2) { //east
	snake.body[0].y++;
    }
}

void handleInput() {
    switch (playerInput) {
	case KEY_UP:
        case 'w':
            if (direction != 3) {
		direction = 1;
            }
        break;
	case KEY_DOWN:
        case 's':
            if (direction != 1) {
		direction = 3;
            }
        break;
	case KEY_LEFT:
        case 'a':
            if (direction != 2) {
		direction = 4;
            }
        break;
	case KEY_RIGHT:
        case 'd':
            if (direction != 4) {
		direction = 2;
            }
    }
}

int checkAppleCollision() {
    return snake.body[0].x == apple.x && snake.body[0].y == apple.y;
}

void randomizeApplePosition(Apple *fruit) {
    //Not using the Node struct's repChar property here
    int freeCellNumber = 0;
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    if (field.matrix[i][j] != snake.body[0].repChar && field.matrix[i][j] != apple.repChar) {
		freeCellNumber++;
	    }
	}
    }

    if (freeCellNumber > 0) {
	Node *freeCells = (Node*)malloc(freeCellNumber * sizeof(Node));
        int idx = 0;
        for (int i = 0; i < field.height; i++) {
	    for (int j = 0; j < field.width; j++) {
	        if (field.matrix[i][j] != snake.body[0].repChar && field.matrix[i][j] != apple.repChar) {
		    freeCells[idx].x = i;
		    freeCells[idx].y = j;
		    idx++;
	        }
	    }
        }

        int random = rand() % freeCellNumber;
        Node randNode = freeCells[random];
        fruit->x = randNode.x;
        fruit->y = randNode.y;
        free(freeCells);
    }
}

void expandSnake() {
    snake.body = (Node*)realloc(snake.body, (snake.bodyLength + 1) * sizeof(Node));
    snake.bodyLength++;
    snake.body[snake.bodyLength - 1].x = snake.body[snake.bodyLength - 2].x;
    snake.body[snake.bodyLength - 1].y = snake.body[snake.bodyLength - 2].y;
    snake.body[snake.bodyLength - 1].repChar = snake.body[snake.bodyLength - 2].repChar;
}

void fillField() {
    for (int i = 0; i < field.height; i++) {
	for (int j = 0; j < field.width; j++) {
	    field.matrix[i][j] = snake.body[0].repChar;
	}
    }
}


int checkGameOver() {
    if (snake.body[0].x < 0 || snake.body[0].x > field.height - 1)
	return 1;
    if (snake.body[0].y < 0 || snake.body[0].y > field.width - 1)
    	return 1;

    for (int i = 0; i < snake.bodyLength - 1; i++) {
	for (int j = i + 1; j < snake.bodyLength; j++) {
	    if (snake.body[i].x == snake.body[j].x && snake.body[i].y == snake.body[j].y) {
		return 1;
	    }
	}
    }
    return 0;
}


void initNcurses() {
    initscr();

    if (has_colors()) {
	if (start_color() == OK) {
	    use_default_colors(); //makes it possible to use default terminal colors (-1)
	    init_pair(1, -1, -1);
	    init_pair(2, COLOR_GREEN, -1);
	    init_pair(3, COLOR_RED, -1);
	    init_pair(4, COLOR_BLACK, COLOR_BLACK);
	}
    }
}

void endNcurses() {
    endwin();
}

void initWindow() {
    int windowHeight = field.height + 2;
    int windowWidth = field.width + 2;
    window = newwin(windowHeight, windowWidth, (screen.height / 2) - (windowHeight / 2), (screen.width / 2) - (windowWidth / 2));
    refresh();

    box(window, 0, 0);
    wrefresh(window);

    char text[] = "points: ___";
    pointsPosX = (screen.height / 2) + (windowHeight / 2);
    pointsPosY = (screen.width / 2) + (windowWidth / 2) - strlen(text);
    move(pointsPosX, pointsPosY + 2);
    printw("points: %d", points);
    refresh();

    wtimeout(window, 250);
}

void releaseWindow(WINDOW **window) {
    wclear(*window);
    wrefresh(*window);
    delwin(*window);
    window = NULL;

    clear();
    refresh();
}

void redrawPoints() {
    if (points < 10) {
	move(pointsPosX, pointsPosY + 2);
    } else if (points >= 10 && points < 100) {
	move(pointsPosX, pointsPosY + 1);
    } else {
	move(pointsPosX, pointsPosY + 2);
    }
    printw("points: %d", points);
    refresh();
}

void displayResult(char *text) {
    int height = 3;
    int width = strlen(text) + 10;
    int row = (screen.height / 2) - (height / 2);
    int column = (screen.width / 2) - (width / 2);

    WINDOW *defeatWindow = newwin(height, width, row, column);
    box(defeatWindow, '*', '=');
    refresh();
    mvwprintw(defeatWindow, height / 2, (width / 2) - (strlen(text) / 2), "%s", text);
    wrefresh(defeatWindow);

    napms(2000);
    releaseWindow(&defeatWindow);
}
