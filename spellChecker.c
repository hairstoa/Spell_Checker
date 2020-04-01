#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    
    while (!feof(file)){
        char* word = nextWord(file);
        if (word != NULL){
            hashMapPut(map, word, 0);
            free(word);
        }
    }
}
/**
 * isInputAlphaOnly
 * Checks the input buffer for characters that are not alphabetical.
 * Returns true (1) if characters are only alphabetical, else it returns false (0).
 * False result will print a error message.
 */
int isInputAlphaOnly(char const* input){
    int i = 0;
    if (input[i] == '\n'){
        return 0;
    }
    while (input[i] != '\n'){
        if ((input[i] < 65 ) || (input[i] > 122)){
            printf("\nError: alphabetical characters only. No spaces. \n \n");
            return 0;
        }
        else if(input[i] > 90 && input[i] < 97){
            printf("\nError: alphabetical characters only. No spaces. \n \n");
            return 0;
        }
        else if(input[i] ==  ' '){
            return 0;
        }
        i++;
    }
  
    
    return 1;
}
/** minimum
 * Returns the minimum number out of three integers.*/
int minimum(int a, int b, int c){
    int min = a;
    
    if (b < a) min = b;
    if (c < b && c < a) min = c;
    
    return min;
}
/** LevenshteinDistance
 * Takes a string and its length and  compares it with another provided string (and its length).
 * The "distance " between the two is returned.
 * Citation:
 * Wikipedia(2020)."Levenshtein Distance". en.wikipedia.org/wiki/Levenshtein_distance
 */
int LevenshteinDistance(char* str1, const int s1_len, char* str2, const int s2_len){
    /*A matrix is created one greater than the length of each word. */
    int matrix[(s1_len) + 1][(s2_len) + 1];
    int i;
    int j;
    /* Every spot in the matrix is initialized to 0. */
    for (i = 0; i < (s1_len) + 1; i++){
        for (j = 0; j < (s2_len) + 1; j++){
            matrix[i][j] = 0;
        }
    }
    
    /* Initialize the first column with the edit distance to an empty string. */
    for (i = 1; i < (s1_len) +1; i++){
        matrix[i][0] = i;
    }
    /* Initialize the first row with the edit distance to an empty string. */
    for (j = 1; j <(s2_len) +1; j++){
        matrix[0][j] = j;
    }
    int cost;
    /* Loop through each cell of the matrix. Each cell represents a different
     subproblem. */
    for (i = 1; i < (s1_len) + 1; i++){
         for (j = 1; j < (s2_len) + 1; j++){
             /* If the letters in the position are the same, the cost is 0 */
             if (str1[i - 1] == str2[j - 1]) cost = 0;
             else cost = 1;
             /* The position in the matrix is the edit distance which is the
              minimum of cell above it + 1 (deletion), cell to the left + 1
              (insertion), or cell to the diagonal left-top (substitution).*/
             matrix[i][j] = minimum(matrix[i-1][j] +1,
                                    matrix[i][j - 1] + 1,
                                    matrix[i - 1][j - 1] + cost);
         }
     }
    /* This is the final edit distance. */
    return matrix[s1_len][s2_len];
}
/**
 * spellChecker
 * This function takes a character array (string) and compares it with a dictionary via a file object to see
 * if there is a match. If there is no match, spellChecker will print an array of 5 possible closest guesses.
 */
void spellChecker(char const* word, HashMap* dict){
    /* Test to see if the word is in the dictionary, return if so. */
    if (hashMapContainsKey(dict, word) == 1){
        printf("The inputted word ... is spelled correctly! \n");
        return;
    }
  
    /* Otherwise, compare the user's word to each word in the dictionary. */
    int i;
    for (i = 0; i < dict->capacity; i++){
        HashLink * linkPtr = dict->table[i];
        while (linkPtr != NULL){
            /* Calculate the Levenshtein distance and add it to value to each
             word in the dictionary. */
            linkPtr->value = LevenshteinDistance((char*)word, (int)strlen(word),
                                                 linkPtr->key,
                                                 (int)strlen(linkPtr->key));
            linkPtr = linkPtr->next;
        }
    }
     printf("The inputted word ... is spelled incorrectly.\n");
    /* At this point all the words in the dictionary have been examined and
       no match was found. Print the five closest suggestions. */
    int j;
    const int numSuggestions = 5;
    char* suggestions[5] = {"?", "?", "?", "?", "?"};
    /* Loop through the dict for every suggestion */
    for ( i = 0; i < numSuggestions; i++){
        /* Each value is compared to min. */
        int min = 100;
        int isRepeat = 0;
        for (j = 0; j < dict->capacity; j++){
            HashLink * linkPtr = dict->table[j];
            while (linkPtr != NULL){
                /* If the link's value is less than or equal to the minimum, the
                link's value is set to the minimum if the key does not exist in
                the suggestions already. */
                if (linkPtr->value <= min){
                    int k;
                    for (k = 0; k <= i; k++){
                        if (strcmp(suggestions[k], linkPtr->key) == 0){
                            isRepeat = 1;
                        }
                    }
                    if (isRepeat == 0){
                        suggestions[i]= linkPtr->key;
                        min = linkPtr->value;
                    }
                }
                linkPtr = linkPtr->next;
            }
        }
    }
    /* Print suggestions */
    printf("Did you mean ...");
    for (i = 0; i < numSuggestions; i++){
        printf(" %s?", suggestions[i]);
    }
    printf("\n");
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
 
    HashMap* map = hashMapNew(1000);

    FILE *file;
    if ((file = fopen("dictionary.txt", "r")) == NULL) {
        printf("Error! opening file");
    }
    
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: \n");
        fgets(inputBuffer, 255, stdin);
        /* Change all uppercase letters to lowercase. */
        int i;
        for (i = 0; i < strlen(inputBuffer); i++){
            inputBuffer[i] = tolower(inputBuffer[i]);
        }
        
        if (strcmp(inputBuffer, "quit\n") == 0)
        {
            quit = 1;
        }
        /* If input only has alphabetical characters, spellchecker runs. */
        else if (isInputAlphaOnly(inputBuffer) == 1){
            if (inputBuffer[strlen(inputBuffer)-1] == '\n'){
                  inputBuffer[strlen(inputBuffer)-1] = '\0';
              }
              spellChecker(inputBuffer, map);
        }
         
    }

    hashMapDelete(map);
    return 0;
}
