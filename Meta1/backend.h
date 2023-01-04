#ifndef BACKEND_H
#define BACKEND_H

#include "utils.h"
#include "userLib/users_lib.h"

#define FPROMOTERS "FPROMOTERS"
#define FITEMS "FITEMS"
#define FUSERS "FUSERS"


//Items
pItem itemConstructorFile(pItem itemList,int id,char name[],char category[],int baseValue,int buyNow,int time,char seller[],char buyer[]);
pItem setupItemList(pItem itemList);
void printList(pItem list);
int freeSaveList(pItem list);

//Promoters
int execPromoter(pPromoter prom,pPromThread threadPromoters);

//backend setup
void initializeAmbvars();
void backendRunning();
void endBackend();

//CMD
void getCommand();

#endif
        