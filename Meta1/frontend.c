#include "utils.h"
#include "frontend.h"



void getCommand(){
    char* token;
    char command[STR_SIZE];
    item newItem;
    bid newBid;
    int i;

    do {
        printf("\nCommand: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;


        if (strcmp(command, "list") == 0) {
            printf("Lista itens\n");
        } else if (strcmp(command, "time") == 0) {
            printf("Show time\n");
        } else if (strcmp(command, "cash") == 0) {
            printf("Show users cash\n");
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting\n");
        } else {
            token = strtok(command, " ");

            if (strcmp(command, "licat") == 0) {
                token = strtok(NULL, " ");
                printf("Listing from category %s\n", token);
            } else if (strcmp(command, "lisel") == 0) {
                token = strtok(NULL, " ");
                printf("Listing items selled by %s\n", token);
            } else if (strcmp(command, "lival") == 0) {
                token = strtok(NULL, " ");
                if (stringIsNum(token)) {
                    printf("Listing with value %s\n", token);
                } else {
                    printf("Value must be integer\n");
                }
            } else if (strcmp(command, "litime") == 0) {
                token = strtok(NULL, " ");
                if (stringIsNum(token)) {
                    printf("Listing items with time %s\n", token);
                } else {
                    printf("Time must be an integer\n");
                }
            } else if (strcmp(command, "add") == 0) {
                token = strtok(NULL, " ");
                if (stringIsNum(token)) {
                    printf("Added %s to user's balance\n", token);
                } else {
                    printf("Value must be integer\n");
                }
            } else if (strcmp(command, "buy") == 0) {
                for (i = 0; i < 2; i++) {
                    token = strtok(NULL, " ");
                    if (token == NULL) break;
                    if (stringIsNum(token)) {
                        if (i == 0)
                            newBid.itemId = atoi(token);
                        else
                            newBid.ammount = atoi(token);
                    } else {
                        printf("Item id and ammount must be an integers\n");
                        break;
                    }
                }
                if (i != 0) {
                    printf("Bid succesfull\n");
                }
            } else if (strcmp(command, "sell") == 0) { 
                i = 0;
                printf("selling");
                token = strtok(NULL, " ");
                if (token != NULL) {
                    strcpy(newItem.name, token);
                    i++;
                }
                token = strtok(NULL, " ");
                if (token != NULL) {
                    strcpy(newItem.category, token);
                    i++;
                }
                token = strtok(NULL, " ");
                if (stringIsNum(token) && token != NULL)
                    newItem.baseValue = atoi(token);
                else
                    printf("Basevalue must be integer\n");
                token = strtok(NULL, " ");
                if (stringIsNum(token) && token != NULL)
                    newItem.buyNow = atoi(token);
                else
                    printf("BuyNow must be integer\n");
                token = strtok(NULL, " ");
                if (stringIsNum(token) && token != NULL)
                    newItem.time = atoi(token);
                else
                    printf("Time must be integer\n");
            } else {
                printf("Not enought args\n");
            }
        }
    }while(strcmp(command,"exit"));
}

void list(pItem itemList){
    while(itemList!=NULL){
        printf("%d %s %s %d %d %d %s %s\n",itemList->id,itemList->name,itemList->category,itemList->baseValue,itemList->buyNow,itemList->time,itemList->seller,itemList->buyer);
        itemList = itemList->prox; 
    }
}

void licat(pItem itemList, char category[]){

    while(itemList != NULL){
        if(strcmp(itemList->category, category[STR_SIZE])==0){
            printf("%d %s %s %d %d %d %s %s\n",itemList->id,itemList->name,itemList->category,itemList->baseValue,itemList->buyNow,itemList->time,itemList->seller,itemList->buyer);
        }
        itemList = itemList->prox;
    }

}

void lisel(pItem itemList, char seller[]){
      while(itemList != NULL){
        if(strcmp(itemList->seller, seller[STR_SIZE])==0){
            printf("%d %s %s %d %d %d %s %s\n",itemList->id,itemList->name,itemList->category,itemList->baseValue,itemList->buyNow,itemList->time,itemList->seller,itemList->buyer);
        }
        itemList = itemList->prox;
    }

}

void lival(pItem itemList, int baseValue){
    while(itemList != NULL){
        if(itemList->baseValue > 0 && itemList->baseValue < baseValue){
            printf("%d %s %s %d %d %d %s %s\n",itemList->id,itemList->name,itemList->category,itemList->baseValue,itemList->buyNow,itemList->time,itemList->seller,itemList->buyer);
        }
        itemList = itemList->prox;
    }
}

void litime(pItem itemList, int time){
    while(itemList != NULL){
        if(itemList->time > 0 && itemList->time < time){
            printf("%d %s %s %d %d %d %s %s\n",itemList->id,itemList->name,itemList->category,itemList->baseValue,itemList->buyNow,itemList->time,itemList->seller,itemList->buyer);
        }
        itemList = itemList->prox;
    }
}

int setupFrontendPipe(char fifoName[]){
    if(mkfifo(fifoName, 0666)==-1){
        if(errno == EEXIST){
            printf("Pipe already exists\n");
        }
        else{
            printf("Error creating the pipe\n");
            exit(1);
        }
    }
    return open(fifoName, O_RDWR);
}

int writeToBackend(int pipe, message mess){
    int size = write(pipe, &mess, sizeof(mess));
    if(size != 0){
        return 0;
    }
    return 1;
}
pItem readItemFromBackend(int pipe){
    pItem node;
    int size=read(pipe,node,sizeof(item));
    if(size==0){
        printf("Error reading item");
        return NULL;
    }
    return node;
}

int login(int pipe){
    int status=0;
    fflush(stdin);
    int size = read(pipe,&status, sizeof(int));
    printf("read %d bytes",size);
    if(status == 0){
        printf("Error login in\n");
        return status;
    }
    printf("Logged in succesfully\n");
}

int main(int argc, char* argv[]){
    struct timeval tv;
    char command[STR_SIZE],newCommand[STR_SIZE];
    message mess;
    mess.pid = getpid();
    char buf;
    char fifoName[STR_SIZE];
    int pipe,pipeBackend;

    if(argc < 3){
        printf("Invalid number of arguments\n");
        exit(1);
    }

    sprintf(fifoName,FIFOFRONTEND,getpid());

    /*pipe = setupFrontendPipe(fifoName);
    pipeBackend = open(FIFOBACKEND,O_WRONLY);
    strcpy(mess.username, argv[1]);
    strcpy(mess.password, argv[2]);
    writeToBackend(pipeBackend, mess);
    
    login(pipe);*/

    
    //scanf("%c",&buf);

    printf("Welcome!\n");
    getCommand();

    close(pipe);
    unlink(fifoName);
}