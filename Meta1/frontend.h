#ifndef FRONTEND_H
#define FRONTEND_H

#include "utils.h"


void getCommand();
pItem readItemFromBackend(int pipe);
void list(pItem itemList);
void licat(pItem itemList, char category[]);
void lisel(pItem itemList, char seller[]);
void lival(pItem itemList, int baseValue);
void litime(pItem itemList, int time);
int setupFrontendPipe(char fifoName[]);
int writeToBackend(int pipe, message mess);
pItem readItemFromBackend(int pipe);
int login(int pipe);
#endif