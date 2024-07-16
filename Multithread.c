#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include "pf1.h"

void getNumberOfLinesInFile(char *fileName /*, stats_t stats*/){

    int count = 1;
    FILE *fp = fopen(fileName, "r");

    for (char c = getc(fp); c != EOF; c = getc(fp)){
        if (c == '\n'){
            count++;
        }
    }
    fclose(fp);

    printf("Number of sorted lines: %d\n", count);

    //stats.sortedLines = count;
}

void getLongestString(char* strings[], int numOfStrings/*, stats_t stats*/){
    char* longestString = strings[0];

    for(int i = 1; i < numOfStrings; i++){
        if(strlen(longestString) < strlen(strings[i])){
            longestString = strings[i];
        }
    }

    printf("Longest String: %s\n", longestString);
    //stats.longestString = longestString;
}

void getShortestString(char* strings[], int numOfStrings /*, stats_t stats*/){
    char* shortestString = strings[0];

    for(int i = 1; i < numOfStrings; i++){
        if(strlen(shortestString) > strlen(strings[i]) && (strings[i] != '\n' || strings[i] != ' ')){
            shortestString = strings[i];
        }
    }

    printf("Shortest String: %s\n", shortestString);
    //stats.shortestString = shortestString;
}

int compare( const void *arg1, const void *arg2 )
{
   return -strcasecmp( * ( char** ) arg1, * ( char** ) arg2 );
}

void sortFile(char *fileName){
    FILE* original = fopen(fileName, "r");
    FILE* tmp = tmpfile(); 

    char fileString[5000]; //El texto del archivo almacenado en un string
    char* stringArray[5000];
    int i = 0;
    for (char c = getc(original); c != EOF; c = getc(original)){
        fileString[i] = c;
        i++;
    }

    char delimitador[] = " \n";
    char *token = strtok(fileString, delimitador);
    int wordsIndex = 0;
    
    while (token != NULL)
    {
        stringArray[wordsIndex] = malloc(5000);
        stringArray[wordsIndex] = token;
        wordsIndex++;
        token = strtok(NULL, delimitador);
    }

    qsort(stringArray, wordsIndex, sizeof(malloc(5000)), compare);

    for(int j = 0; j < wordsIndex; j++){
        printf("%s\n", stringArray[j]);
    }

    getNumberOfLinesInFile(fileName);
    getLongestString(stringArray, wordsIndex);
    getShortestString(stringArray, wordsIndex);

    fclose(original);
    pthread_exit(NULL);
}


void main(int argc, char* argv[]){

    if (argc < 3){
        error(0);
    }

    pthread_t workers[argc-1];
    char* files[argc-1];
    
    for(int i = 1; i < argc; i++){
        files[i-1] = argv[i];
    }

    for(int i = 0; i < argc-1; i++){
        pthread_create(&workers[i], NULL, sortFile , files[i]);
    }

    for(int i = 0; i < argc-1; i++){
        pthread_join(workers[i], NULL);
    }

    //stats_t stats[argc - 1];
}