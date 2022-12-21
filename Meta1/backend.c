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
    newItem->buyNow=buyNow;
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
    printf("%s", getenv(FITEMS));
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

//Promoters

int loadPromotersFile(promoter updated[]){
    FILE* f;
    int i=0;
    f = fopen(getenv(FPROMOTERS),"r");
    if(f==NULL){
        printf("Error opening the file[PROMOTERS]\n");
        return -1;
    }
    while(fscanf(f,"%s",updated[i].nome)==1){
        printf("%s\n",updated[i].nome);
        i++;
    }
    fclose(f);
    return 0;
}

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

void printAllPromoters(promoter list[]){
    for(int i=0;i<sizeof(list)-1;i++){
        printf("%s\n",list[i].nome);
    }
}

void clearPromList(promoter list[],promoter updatedList[]){
    for(int i=0;i<sizeof(list)-1;i++){
        strcpy(list[i].nome,"");
        list[i].pid=0;
    }
    for(int i=0;i<sizeof(updatedList)-1;i++){
        list[i] = updatedList[i];
    }
}

void launchMissingPromoters(promoter list){

}

void checkRunningPromoters(promoter list[],promoter updatedList[]){
    int exists=0;
    union sigval sig;
    for(int i=0;i<sizeof(updatedList)-1;i++){
        for(int j=0;j<sizeof(list)-1;j++) {
            if (strcmp(list[i].nome, updatedList[i].nome) == 0) {
                exists=1;
            }
        }
        if(exists==0){

            sigqueue(list[i].pid,SIGUSR1,sig);
        }
        exists=0;
    }
    clearPromList(list,updatedList);
    printAllPromoters(list);
}


//CMD

void getCommand(promoter promList[]){
    char command[MAX_CMD_SIZE];
    promoter updatedPromList[MAX_PROMS];
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
            if(loadPromotersFile(updatedPromList)!=0)
                printf("Error loading promoters\n");
            checkRunningPromoters(promList,updatedPromList);
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

int main(int argc,char* argv[]){
    int opt,p[2],pipe;
    pItem itemList=NULL;
    message mess;
    char pipeName[STR_SIZE];
    promoter promotersList[MAX_PROMS];

    strcpy(promotersList[0].nome,"teste");
    strcpy(promotersList[1].nome,"promoters/promotor_oficial");

    initializeAmbVars();
    printf("AmbVars initialized\n");

    itemList = setupItemList(itemList);

    if(mkfifo(FIFOBACKEND,0600)==-1){
        if(errno==EEXIST)
            printf("backend already running...\n");
        else{
            printf("Error creating pipe\n");
        }
        exit(1);
    }
    pipe=open(FIFOBACKEND,O_RDWR);

    getCommand(promotersList);

    //printList(itemList);
    freeSaveList(itemList);
    close(pipe);
    unlink(pipe);
    endBackend();

    return 0;
}