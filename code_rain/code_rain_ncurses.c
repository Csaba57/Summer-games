#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h> //makes it easy to display colors if the system is capable of displaying colors


#define activeCount 23
int _WIDTH;
int _HEIGHT;
int _LENGTH = 45;
int row = 0, col = 0;


typedef struct {
    int width;
    int height;
    char **field;
}Screen;

typedef struct {
    int colNumber;
    int tail;
    int head;
}Column;


void init();
void freeResources();
void draw();
void drawFull();   //[debug] function
int isActiveCol(Column array[], int arrLength, int colNum);
int contains(int array[], int arrLength, int number);
void updateActiveColumns();
void randomizeScreen(Screen *screen);


Screen screen;
Column activeColumns[activeCount];   //the initial value of heads are always a negative number, the initial value of tails are always less than the value of heads
int takenColumns[activeCount];

int main() {

    initscr();
    getmaxyx(stdscr, _HEIGHT, _WIDTH);
    curs_set(0);

    srand(time(NULL));
    init();


    while(1) {
	draw();
	updateActiveColumns();

	napms(100);
	randomizeScreen(&screen);
	clear();
    }


    curs_set(1);
    freeResources();
    endwin();

    return 0;
}


void init() {
    screen.width = _WIDTH;
    screen.height = _HEIGHT;

    screen.field = (char**)malloc(_HEIGHT * sizeof(char*));   //free!
    for (int i = 0; i < _HEIGHT; i++) {
	*(screen.field + i) = (char*)malloc(_WIDTH * sizeof(char));   //free!
    }

    for (int i = 0; i < screen.height; i++) {
	for (int j = 0; j < screen.width; j++) {
	    screen.field[i][j] = rand() % 80 + 33;   //character = integer
	}
    }

    //int takenColumns[] = {-1, -1, -1, -1, -1 ...};
    for (int i = 0; i < activeCount; i++)
	takenColumns[i] = -1;

    for (int i = 0; i < activeCount; i++) {
	int colNumber;
	do {
	    colNumber = rand() % screen.width;;
	} while(contains(takenColumns, 5, colNumber));
	takenColumns[i] = colNumber;

	activeColumns[i].colNumber = colNumber;
	activeColumns[i].head = (rand() % 39 + 1) * -1; //rand() % 6 + 1) * -1
	activeColumns[i].tail = activeColumns[i].head - _LENGTH;
	//[debug]
	//printf("tail: %d, head: %d\n", activeColumns[i].tail, activeColumns[i].head);
    }

    //defining colors
    if (has_colors()) {
	if (start_color() == OK) {
	    init_pair(1, COLOR_GREEN, COLOR_BLACK);
	    init_pair(2, COLOR_WHITE, COLOR_BLACK);
	}
    }

}

void freeResources() {
    for (int i = 0; i < screen.height; i++)
	free(screen.field[i]);
    free(screen.field);
    screen.field = NULL;

}

//[debug]
void drawFull() {
    for (int i = 0; i < screen.height; i++) {
	for (int j = 0; j < screen.width; j++) {
	    printw("%c", screen.field[i][j]);
	}
	row++;
	move(row, col);
	refresh();
    }
    row = 0;
    col = 0;
}

void draw() {
    char karakter;
    int color = 0;
    for (int i = 0; i < screen.height; i++) {
	for (int j = 0; j < screen.width; j++) {
	    karakter = ' ';
	    for (int k = 0; k < activeCount; k++) {
		if (activeColumns[k].colNumber == j && (i >= activeColumns[k].tail && i == activeColumns[k].head) && activeColumns[k].head != _HEIGHT - 1) {

		    if (has_colors()) {
			color = 2;
		    }
		    karakter = screen.field[i][j];
		    break;

		} else if (activeColumns[k].colNumber == j && (i >= activeColumns[k].tail && i <= activeColumns[k].head)) {

		    if (has_colors()) {
			color = 1;
		    }
		    karakter = screen.field[i][j];
		    break;
		}
	    }
	    if (color != 0) {
		attron((color == 1) ? COLOR_PAIR(1) : COLOR_PAIR(2));
		printw("%c", karakter);
		attroff((color == 1) ? COLOR_PAIR(1) : COLOR_PAIR(2));
	    } else {
	        printw("%c", karakter);
	    }
	}
	row++;
	move(row, col);
	refresh();
    }
    row = 0;
    col = 0;
}

int isActiveCol(Column array[], int arrLength, int column) {
    for (int i = 0; i < arrLength; i++) {
	if (array[i].colNumber == column) {
	    return 1;
	}
    }
    return 0;
}

int contains(int array[], int arrLength, int number) {
    for (int i = 0; i < arrLength; i++) {
	if (array[i] == number) {
	    return 1;
	}
    }
    return 0;
}

void updateActiveColumns() {
    //adding +1 to every Column's (element's) head and tail, until they reach the value (HEIGHT-1), if the heads reach this value we only adding +1 to the tails
	//if the head reaches (HEIGHT-1) we no longer incrementing the head
	//if the tail also reaches (HEIGHT-1) we set the initial value to head and tail and we choose a different column (randomly)

    for (int i = 0; i < activeCount; i++) {
	if (activeColumns[i].head < _HEIGHT - 1) {
	    activeColumns[i].head++;
	}
	if (activeColumns[i].tail < _HEIGHT - 1) {
	    activeColumns[i].tail++;
	}
	if (activeColumns[i].head == (_HEIGHT - 1) && activeColumns[i].tail == (_HEIGHT - 1)) {
	    int colNumber;
	    do {
	        colNumber = rand() % screen.width;
	    } while(contains(takenColumns, 5, colNumber));
	    takenColumns[i] = colNumber;

	    activeColumns[i].colNumber = colNumber;
	    activeColumns[i].head = (rand() % 6 + 1) * -1;
	    activeColumns[i].tail = activeColumns[i].head - _LENGTH;
	}
    }
}

void randomizeScreen(Screen *screen) {
    int x;
    int y;
    for (int i = 0; i < _WIDTH * 2; i++) {
	x = rand() % _WIDTH;
	y = rand() % _HEIGHT;
	screen->field[y][x] = rand() % 80 + 33;
    }
}
