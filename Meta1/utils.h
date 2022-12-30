#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>

#define MAX_USERS 20
#define MAX_PROMS 10
#define MAX_ITEMS 30
#define MAX_CMD_SIZE 50
#define STR_SIZE 50

#define FIFOFRONTEND "frontend_%d"
#define FIFOBACKEND "fifoBackend"

typedef struct item item,*pItem;
typedef struct bid bid, *pBid;
typedef struct promoter promoter, *pPromoter;
typedef struct promoterList promoterList, *pPromoterList;
typedef struct promotion promotion, *pPromotion;
typedef struct TDATA TDATA, *pTDATA;
typedef struct message message, *pMessage;


struct bid{
    pid_t pid;
    int ammount;
    int itemId;
};

struct item{
    int id;
    char name[STR_SIZE];
    char category[STR_SIZE];
    int baseValue;
    int buyNow;
    int time;
    char seller[STR_SIZE];
    char buyer[STR_SIZE];
    struct item* ant;
    struct item* prox;
    bid topBid;
};

struct promotion{
    char category[STR_SIZE];
    int prom;
    int time;
};

struct promoter{
    pid_t pid;
    char nome[STR_SIZE];
    promotion prom;
    int running;
    int stop;
    promoter* ant;
    promoter* prox;
};

struct promoterList{
    int numPromoters;
    pPromoter list;
};

struct message{
    pid_t pid;
    int commandType;        //0->listas|1->Strings;
    char username[STR_SIZE];
    char password[STR_SIZE];
    char command[STR_SIZE];
    char args[5][STR_SIZE];
    int numArgs;
    int res;
    pItem list;
    char info[256]; //ainda ns se fica assim
};

struct TDATA{
    int frontendPipe;
    int stop;
};

struct teste{
    pthread_mutex_t* m;
};

int stringIsNum(char str[]);

#endif