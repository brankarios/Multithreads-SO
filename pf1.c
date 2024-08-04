#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include "pf1.h"
#include <ctype.h>
#include <semaphore.h>

int indexOfFirstFile = 0;
sem_t indexS;
int totalSorted = 0;

int compare( const void *arg1, const void *arg2 )
{
   return -strcasecmp( * ( char** ) arg1, * ( char** ) arg2 );
}

int getNumberOfLinesWithName(char *fileName){

    int count = 0;
    FILE *fp = fopen(fileName, "r");
    char c;

    while(c = getc(fp)){
        if (c == '\n'){ 
            count += 1;
        }
        else if (c == EOF){
            count +=1;
            break;
        }
    }
    fclose(fp);

    return count;
}

char* getLongestString(char* strings[], int numOfStrings){
    char* longestString = strings[0];

    for(int i = 1; i < numOfStrings; i++){
        if(strlen(longestString) < strlen(strings[i])){
            longestString = strings[i];
        }
        else if(strlen(longestString) == strlen(strings[i]) && strcasecmp(longestString, strings[i]) < 0){
            longestString = strings[i];
        }
    }

    return longestString;
}

char* getShortestString(char* strings[], int numOfStrings){
    char* shortestString = strings[0];

    for(int i = 1; i < numOfStrings; i++){
        if(strlen(shortestString) > strlen(strings[i]) && isspace(*strings[i]) == 0){
            shortestString = strings[i];
        }
        else if(strlen(shortestString) == strlen(strings[i]) && strcasecmp(shortestString, strings[i]) < 0){
            shortestString = strings[i];
        }
    }

    return shortestString;
}

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

void sortFile(stats_t *estadistica){
    FILE* original = fopen((*estadistica).fileName, "r");
    char* sortedFileName = malloc(1000);
    strcat(sortedFileName, (*estadistica).fileName);
    strcat(sortedFileName, ".sorted");
    FILE* ordenado = fopen(sortedFileName, "w+");
    char* stringArray[5000];
    unsigned int numOfLines = getNumberOfLinesWithName((*estadistica).fileName);

    for(int i = 0; i < numOfLines; i++){
        stringArray[i] = malloc(5000);
        fgets(stringArray[i], 5000, original);
        stringArray[i] = trim(stringArray[i]);
        if(strlen(stringArray[i]) == 0){
            numOfLines--;
            i--;
            sem_wait(&indexS);
            totalSorted--;
            sem_post(&indexS);
        } 
        sem_wait(&indexS);
        totalSorted++;
        sem_post(&indexS);
    }

    qsort(stringArray, numOfLines, sizeof(malloc(5000)), compare);
    for(int i = 0; i < numOfLines; i++){
        fprintf(ordenado, "%s\n", stringArray[i]);
    }

    (*estadistica).sortedLines = numOfLines;
    (*estadistica).shortestString = malloc(5000 * sizeof(char));
    (*estadistica).shortestString = getShortestString(stringArray, numOfLines);
    (*estadistica).longestString = malloc(5000 * sizeof(char));
    (*estadistica).longestString = getLongestString(stringArray, numOfLines);

    fclose(original);
    fclose(ordenado);
    printf("This worker thread writes %d lines to %c%s%c\n", numOfLines, '"', sortedFileName, '"');
    pthread_exit(NULL);
}

int deleteDuplicates(char* strings[], int size){
    int linesDeleted = 0;

    for(int i = 0; i < size; i++){
        for(int j = 0; j < i; j++){
            if(strcasecmp(strings[i], strings[j]) == 0){
                for (int k = i; k < size - 1; k++) {
                    strings[k] = strings[k + 1];
                }
                linesDeleted++;
                size--;
                i--;
                break;
            }
        }
    }

    return linesDeleted;
}

void sortTempFile(char* fileNames[]){

    sem_wait(&indexS);

    char* stringFile[5000];
    FILE* firstFile = fopen(fileNames[indexOfFirstFile], "r");
    FILE* secondFile = fopen(fileNames[indexOfFirstFile+1], "r");
    char tempFileName[L_tmpnam];
    tmpnam(tempFileName);
    FILE* temp = fopen(tempFileName, "w+");

    int linesFirstFile = getNumberOfLinesWithName(fileNames[indexOfFirstFile])-1;
    int linesSecondFile = getNumberOfLinesWithName(fileNames[indexOfFirstFile+1])-1;

    for(int i = 0; i < linesFirstFile + linesSecondFile; i++){
        if(i < linesFirstFile){
            stringFile[i] = malloc(5000);
            fgets(stringFile[i], 5000, firstFile);
        }
        else{
            stringFile[i] = malloc(5000);
            fgets(stringFile[i], 5000, secondFile);
        }
    }
    int linesDeleted = deleteDuplicates(stringFile, linesFirstFile + linesSecondFile);
    int linesFinalFile = linesFirstFile+linesSecondFile-linesDeleted;

    for(int i = 0; i < linesFinalFile; i++){
        if(stringFile[i] != NULL){
            if(strlen(stringFile[i]) > 0 && (*stringFile[i]) != '\n'){
                fprintf(temp, "%s", stringFile[i]);
            }
        }
    }

    printf("Merged %d lines and %d lines into %d lines.\n", linesFirstFile, linesSecondFile, linesFinalFile);

    fileNames[indexOfFirstFile] = strdup(tempFileName);

    sem_post(&indexS);
    fclose(firstFile);
    fclose(secondFile);
    fclose(temp);
    pthread_exit(NULL);
}

void fixArray(char* files[], int size){

    int desplazamiento = 0;
    char* aux = malloc(5000 * sizeof(char));

    for(int i = 0; i < size; i++){
        if(i % 2 == 0){
            aux = files[i];
            files[i] = NULL;
            files[i-desplazamiento] = aux;
            desplazamiento++;
        }
        else{
            files[i] = NULL;
        } 
    }
}

void main(int argc, char* argv[]){

    sem_init(&indexS,0,1);
    
    if (argc < 3){
        perror(0);
    }

    pthread_t workers[argc-1];
    char* files[argc-1];
    stats_t *estadisticas[argc-1];
    
    for(int i = 1; i < argc; i++){
        files[i-1] = argv[i];
    }

    for(int i = 0; i < argc -1; i++){
        estadisticas[i] = malloc(sizeof(stats_t));
        estadisticas[i]->fileName = files[i];
        estadisticas[i]->sortedLines = 0;
        estadisticas[i]->longestString = NULL;
        estadisticas[i]->shortestString = NULL;
    }

    for(int i = 0; i < argc-1; i++){
        pthread_create(&workers[i], NULL, (void*) sortFile, estadisticas[i]);
    }
    for(int i = 0; i < argc-1; i++){
        pthread_join(workers[i], NULL);
    }

    int remainingFiles = argc - 1;
    pthread_t sortingThreads[remainingFiles/2];
    char* nameOfFiles[remainingFiles];

    for(int i = 0; i < argc-1; i++){
        nameOfFiles[i] = malloc(500 * sizeof(char));
        strcat(nameOfFiles[i], argv[i+1]);
        strcat(nameOfFiles[i], ".sorted");
    }

    while(remainingFiles > 1){

        indexOfFirstFile = 0;

        for(int i = 0; i < remainingFiles/2; i++){
            pthread_create(&sortingThreads[i], NULL, (void*) sortTempFile, nameOfFiles);
            sleep(0);
            sem_wait(&indexS);
            indexOfFirstFile += 2;
            sem_post(&indexS);
        }

        for(int i = 0; i < remainingFiles/2; i++){
            pthread_join(sortingThreads[i], NULL);
        }

        remainingFiles-= remainingFiles/2;

        fixArray(nameOfFiles, argc-1);
    }

    
    int linesOfLastFile = getNumberOfLinesWithName(nameOfFiles[0])-1;
    FILE* sorted = fopen("sorted.txt", "w+");
    FILE* lastFile = fopen(nameOfFiles[0], "r");
    char* string[linesOfLastFile];

    for(int i = 0; i < linesOfLastFile; i++){
        string[i] = malloc(5000 * sizeof(char));
        fgets(string[i], 5000 * sizeof(char), lastFile);
    }

    qsort(string, linesOfLastFile, sizeof(char*), compare);

    for(int i = 0; i < linesOfLastFile; i++){
        fprintf(sorted, "%s", string[i]);
    }

    printf("A total of %d strings were passed as input.\n", totalSorted);
    printf("Longest string sorted: %s", getLongestString(string, linesOfLastFile-1));
    printf("Shortest string sorted: %s", getShortestString(string, linesOfLastFile-1));
    sem_destroy(&indexS);
    fclose(sorted);
    fclose(lastFile);
}