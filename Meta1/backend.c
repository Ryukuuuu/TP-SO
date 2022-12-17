#include "utils.h"
#include "backend.h"



// ITEMS -> Linked list + Item file reading

pItem itemConstructorFile(pItem itemList,int id,char name[],char category[],int baseValue,int buyNow,int time,char seller[],char buyer[]){
    pItem newItem,aux;

    newItem = malloc(sizeof(item));
    if(newItem==NULL){
        printf("Malloc error\n");
        return 0;
    }

    newItem->id=id;
    strcpy(newItem->name,name);
    strcpy(newItem->category,category);
    newItem->baseValue=baseValue;
    newItem->buyNow;
    newItem->time=time;
    strcpy(newItem->seller,seller);
    strcpy(newItem->buyer,buyer);

    if(itemList==NULL){
        newItem->prox=NULL;
        itemList=newItem;
        //printf("primeiro elem da lista");
    }
    else{
        //printf("segundo elem-> %s",newItem->name);
        aux=itemList;
        while(aux->prox!=NULL){
            aux = aux->prox;
        }
        aux->prox=newItem;
    }
    return itemList;
}

//Read fitems.txt and initialize the items linked list
pItem setupItemList(pItem itemList){
    pItem aux;
    item newItem;
    FILE* fp;
    int id,baseValue,buyNow,time;
    char name[STR_SIZE],category[STR_SIZE],seller[STR_SIZE],buyer[STR_SIZE];
    //printf("Na funcao");
    fp=fopen(getenv(FITEMS),"r");
    if(fp==NULL){
        printf("No historic/File not found\n");
        return 0;
    }
    while(!feof(fp)){
        fscanf(fp,"%d %s %s %d %d %d %s %s\n",&id,name,category,&baseValue,&buyNow,&time,seller,buyer);
        itemList = itemConstructorFile(itemList,id,name,category,baseValue,buyNow,time,seller,buyer);
        //printf("-----------------------\n%d %s %s %d %d %d %s %s\n--------------------\n",itemList->id,itemList->name,itemList->category,itemList->baseValue,itemList->buyNow,itemList->time,itemList->seller,itemList->buyer);
        //printf("No ciclo\n");
    }
    if(itemList->prox==NULL){
        printf("A lista nao inicializa");
    }
    fclose(fp);
    return itemList;
}

void printList(pItem list){
    while(list!=NULL){
        printf("%d %s %s %d %d %d %s %s\n",list->id,list->name,list->category,list->baseValue,list->buyNow,list->time,list->seller,list->buyer);
        list = list->prox;
    }
}

int freeSaveList(pItem list){  
    pItem aux=list;
    FILE* fp;
    fp=fopen(getenv(FITEMS),"w");
    if(fp==NULL){
        printf("Error opening items file\n");
        exit(1);
    }
    while(list!=NULL){
        aux=list->prox;
        fprintf(fp,"%d %s %s %d %d %d %s %s\n",list->id,list->name,list->category,list->baseValue,list->buyNow,list->time,list->seller,list->buyer);
        free(list);
        list = aux; 
    }

    return 1;
}


//Promoters

void execPromoter(){
    int p[2];
    char msg[STR_SIZE];
    char nomeProm[STR_SIZE];
    //nomeProm=readPromoterFile();

    pipe(p);
    //printf("NO EXEC");
    int pid=fork();
    if(pid==0){
        //printf("\nProcesso filho\n");
        close(STDOUT_FILENO);
        dup(p[1]);
        close(p[0]);
        close(p[1]);
        fflush(stdout);
        //fprintf(stderr,"DFGDF");
        execl("promoters/promotor_oficial","promoters/promotor_oficial",NULL);
    }
    else if(pid>0){
        //prom->pid=pid;
        fflush(stdout);
        fflush(stdin);
        close(p[1]);
        read(p[0],msg,sizeof(msg));
        msg[strcspn(msg,"\n")]=0;
        printf("\n-> %s\n",msg);
    }
}

//Backend setup

void initializeAmbVars(){
    setenv(FPROMOTERS,"files/fpromoters.txt",1);
    setenv(FITEMS,"files/fitems.txt",1);
    setenv(FUSERS,"files/fusers.txt",1);
}


void endBackend(){
    //remove(BACKEND_RUN_FILENAME);
    unsetenv(FPROMOTERS);
    unsetenv(FITEMS);
    unsetenv(FUSERS);
    unlink(FIFOBACKEND);
}

//CMD

void getCommand(){
    char buf,command[MAX_CMD_SIZE];
    scanf("%c",&buf);
    char* token;
    fflush(stdin);
    do{
        printf("\nCommand: ");
        fgets(command,sizeof(command),stdin);
        command[strcspn(command,"\n")]=0;

        if(strcmp(command,"list")==0){
            printf("Show list\n");
        }
        else if(strcmp(command,"users")==0){
            printf("Show users list\n");
        }
        else if(strcmp(command,"prom")==0){
            printf("Promoters\n");
        }
        else if(strcmp(command,"reprom")==0){
            printf("Update promoters");
        }
        else if(strcmp(command,"close")==0){
            printf("Closing\n");
        }
        else{
            token = strtok(command," ");

            if(strcmp(command,"cancel")==0){
                token = strtok(NULL," ");
                printf("Ending promoter %s\n",token);
                
            }
            else if(strcmp(command,"kick")==0){
                token = strtok(NULL," ");
                printf("Kick user %s\n",token);
            }
            else{
                printf("Invalid command!\n");
            }
        }
    }while(strcmp(command,"close")!=0);
}

int initializeBackendPipe(){
    if(mkfifo(FIFOBACKEND,O_RDONLY)==-1){
        if(errno==EEXIST)
            printf("Pipe already exists\n");
        else{
            printf("Error creating pipe\n");
            return 1;
        }
    
    }
    return  open(FIFOBACKEND, O_RDONLY);
}

//------HOJE------//
void sendItemList(message mess, pItem firstNode){
    char pipeName[STR_SIZE];

     mess.list = firstNode;
     
     sprintf(pipeName,FIFOFRONTEND,mess.pid);
     int pipe = open(pipeName,O_WRONLY);

     if(pipe == NULL){
        printf("Error opening the pipe\n");
     }
     else{
        int size = write(pipe, &mess, sizeof(message));
        if(size <= 0){
            printf("error\n");
        }
     }
     close(pipe);
}

int main(int argc,char* argv[]){
    int opt,p[2], pipe, pipe_front;
    pItem itemList=NULL;
    int v;
    initializeAmbVars();
    pipe = initializeBackendPipe();
    message mess;
    char pipeName[STR_SIZE];


    int size = read(pipe, &mess, sizeof(mess));
    if(size>0){
        printf("Function\n");
    }

    sprintf(pipeName,FIFOFRONTEND,mess.pid);
    pipe_front = open(pipeName,O_WRONLY);

    if(pipe_front == NULL){
        printf("Error\n");
    }
    int valid = 0;

    int size2 = write(pipe_front, &valid, sizeof(int));
    printf("depois do write\n");

    do{
    printf("\nQual funcionalidade deseja testar?\n1-Lancamento de promotor e read\n2-Carregamento e atualizacao do saldo dos utilizadores\n3-Carregamentos dos items\n4-Commands\n5-Exit\n");
    scanf("%d",&opt);
    switch (opt)
    {
    case 1:
        execPromoter();
        break;
    case 2:
        if(loadUsersFile(getenv(FUSERS))!=-1){
            updateUserBalance("username1",getUserBalance("username1")-1);
            updateUserBalance("username2",getUserBalance("username2")-1);
            saveUsersFile(getenv(FUSERS));
        }
        break;
    case 3:
        itemList=setupItemList(itemList);
        printList(itemList);
        freeSaveList(itemList);
        break;
    case 4:
        getCommand();
        break;
    case 5:
        printf("Exiting\n");
        break;
    default:
        printf("Opt not found\n");
        break;
    }
    }while(opt != 5);

    close(pipe);
    endBackend();

    return 0;
}