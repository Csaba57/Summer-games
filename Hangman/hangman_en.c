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
char* word = NULL;
char* representation = NULL;


/*
The main loop of the game.
If the number of lifes reaches 0 or if the player manages to guess the letters correctly the game ends.
In the end the function displays the result.

params: none
return: 0 (int), if the program ends with no error
*/
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


/*
Picking a random word from the wordlist.
Creating the all '_' character representation afterwards.

params: none
return: none
*/
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

/*
Displays the representation of the given word.
This representation contains both underline and letter characters.
Thanks to this function the player can see how many letters have been revealed.

param: the pointer of the representation string (char*)
return: none
*/
void printWord(char *representation) {
    for (int i = 0; representation[i] != '\0'; i++) {
	printf("%c ", representation[i]);
    }
    puts("");
}


/*
Prints the state of the hangman image, wich depends on the health of the player.

param: number of lifes (int)
return: none
*/
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

/*
Prints the title of the game.

param: none
return: none
*/
void printLogo() {
    printf("===================\n"
	   "      Hangman      \n"
	   "===================\n");
}

/*
Checks if the given word contains the given letter.

params: a word (char*), the letter to be found (char)
return: 1 (int), if the word contains the letter, 0 (int) if the letter is not in the word
*/
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

/*
Changes the characters of the representation according to the given word and the given character.

params: the word to be guessed(char*), the letter to look for (char), the visual representation of the word
return: none
*/
void revealLetters(char* word, char letter, char* representation) {
    for (int i = 0; i < strlen(word); i++) {
	if (word[i] == letter) {
	    representation[i] = letter;
	}
    }
}

/*
Checks if the visual representation of the word contatins '_' characters.

param: the visual representation of the word (char*)
return: 1 (int) if the representation has no '_' characters, 0 (int) otherwise
*/
int checkComplete(char* representation) {
    int complete = 1;
    for (int i = 0; i < strlen(representation); i++) {
	if (representation[i] == '_') {
	    complete = 0;
	    break;
	}
    }
    return complete;
}
