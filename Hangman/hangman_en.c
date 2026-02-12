#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void init();
void printWord(char *representation);
void printImage(int lifes);
int contains(char* word, char letter);
void revealLetters(char* word, char letter, char* representation);
int checkComplete(char* representation);
void printLogo();


char* words[] = {"apple", "cliff", "hotspring", "plain", "computer"};
int lifes = 5;
char* word;
char* representation;


int main() {

    init();
    while (lifes > 0) {
	printLogo();
	printf("Guess the word, that I'm thinking of!\n");
	printf("\n\n\n");
	printImage(lifes);
	printWord(representation);
	puts("");

	char letter;
	printf("[letter]: ");
	scanf(" %c", &letter);

	if (contains(word, letter)) {
	    revealLetters(word, letter, representation);
	    if (checkComplete(representation))
		break;
	} else {
	    lifes--;
	}
	system("clear");   //only on Linux and Mac! system("cls") for Windows
    }

    if (lifes > 0) {
	printf("\nYou successfully guessed the word! (\"%s\")\n", word);
	printf("Lifes left: %d\n", lifes);
    } else {
	printImage(lifes);
	printf("You're out of lifes! Maybe next time.\n");
    }

    free(representation);

    return 0;
}

void init() {
    srand(time(NULL));

    int randIndex = rand() % 5;
    word = words[randIndex];
    representation = (char*)malloc(strlen(word) * sizeof(char) + 1);
    representation[strlen(word)] = '\0';

    for (int i = 0; i < strlen(word); i++) {
	representation[i] = '_';
    }
}

void printWord(char *representation) {
    for (int i = 0; representation[i] != '\0'; i++) {
	printf("%c ", representation[i]);
    }
    puts("");
}

void printImage(int lifes) {
    char image[200];
    switch (lifes) {
	case 0:
	    strcpy(image, "   -----------#\n"
	    	    "   |        \\ |\n"
	    	    "   |          |\n"
	    	    "  (_)         |\n"
	    	    "  /|\\         |\n"
	    	    "  / \\         |\n"
	    	    "              |\n"
	    	    "              |\n"
	    	    "            / |\n"
	    	    "===================");
	    break;
	case 1:
	    strcpy(image, "   -----------#\n"
	    	    "   |       \\  |\n"
	    	    "   |          |\n"
	    	    "  (_)         |\n"
	    	    "  /|\\         |\n"
	    	    "              |\n"
		    "              |\n"
	    	    "              |\n"
	    	    "            / |\n"
	    	    "===================");
	    break;
	case 2:
	    strcpy(image, "   -----------#\n"
	    	    "   |       \\  |\n"
	    	    "   |          |\n"
	    	    "  (_)         |\n"
	    	    "              |\n"
	    	    "              |\n"
		    "              |\n"
	    	    "              |\n"
	    	    "            / |\n"
	    	    "===================");
	    break;
	case 3:
	    strcpy(image, "   -----------#\n"
	    	    "   |       \\  |\n"
	    	    "   |          |\n"
	    	    "              |\n"
	    	    "              |\n"
	    	    "              |\n"
		    "              |\n"
	    	    "              |\n"
	    	    "            / |\n"
	    	    "===================");
	    break;
	case 4:
	    strcpy(image, "           ---#\n"
	    	    "            \\ |\n"
	    	    "              |\n"
	    	    "              |\n"
	    	    "              |\n"
	    	    "              |\n"
		    "              |\n"
	    	    "              |\n"
	    	    "            / |\n"
	    	    "===================");
	    break;
	case 5:
	    strcpy(image, "               \n"
	    	    "               \n"
	    	    "               \n"
	    	    "               \n"
	    	    "               \n"
	    	    "               \n"
		    "               \n"
	    	    "               \n"
	    	    "            / |\n"
	    	    "===================");
    }
    printf("%s\n", image);
}

void printLogo() {
    printf("===================\n"
	   "      Hangman      \n"
	   "===================\n");
}

int contains(char* word, char letter) {
    int benne = 0;
    for (int i = 0; i < strlen(word); i++) {
	if (word[i] == letter) {
	    benne = 1;
	    break;
 	}
    }
    return benne;
}

void revealLetters(char* word, char letter, char* representation) {
    for (int i = 0; i < strlen(word); i++) {
	if (word[i] == letter) {
	    representation[i] = letter;
	}
    }
}

int checkComplete(char* representation) {
    int komplett = 1;
    for (int i = 0; i < strlen(representation); i++) {
	if (representation[i] == '_') {
	    komplett = 0;
	    break;
	}
    }
    return komplett;
}
