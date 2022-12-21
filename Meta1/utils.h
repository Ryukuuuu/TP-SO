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
#define BACKEND_RUN_FILENAME "running.txt"
#define MAX_CMD_SIZE 50
#define STR_SIZE 50

#define FIFOFRONTEND "frontend_%d"
#define FIFOBACKEND "fifoBackend"

typedef struct user user,*pUser;
typedef struct item item,*pItem;
typedef struct bid bid, *pBid;
typedef struct promoter promoter, *pPromoter;
typedef struct promoterList promoterList,*pPromoterList;

struct bid{
    pid_t pid;
    int ammount;
    int itemId;
};

struct user{
    char userName[STR_SIZE];
    char password[STR_SIZE];
    int balance;       //saldo
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

typedef struct promotion{
    char category[STR_SIZE];
    int prom;
    int time;
}promotion;

struct promoter{
    int pid;
    char nome[STR_SIZE];
    promotion prom;
    promoter* ant;
    promoter* prox;
};

struct promoterList{
    int numPromoters;
    promoter* list;
};

typedef struct message{
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
}message;

typedef struct{
    int frontendPipe;
    int stop;
}TDATA;


int argsCount(char cmd[]);
int stringIsNum(char str[]);

#endif