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
void stopAllProm(pPromoter list){
    pPromoter aux=list;
    while(aux!=NULL){
        aux->stop=1;
        aux = aux->prox;
    }
}

int freeAllPromoterList(pPromoter list){   //Liberta e guarda todos os promotores(Chama-se quando o programa é terminado)
    pPromoter aux = list;
    FILE* f;
    f = fopen(getenv(FPROMOTERS),"r+");
    if(f==NULL){
        printf("Error openning promoters file\n");
        return -1;
    }
    while(list!=NULL){
        aux=list->prox;
        fprintf(f,"%s",list->nome);
        if(aux!=NULL){
            fprintf(f,"\n");
        }
        free(list);
        list = aux;
    }
    fclose(f);
    return 0;
}

void freePromoters(pPromoterList list){     //Liberta os promotores que ja nao estao presentes no fpromoters.txt
    pPromoter aux=list->list,node=aux->prox;
    union sigval si;
    si.sival_int=0;

    while(aux!=NULL){
        if(node->stop==1 && node != NULL){
            printf("Searching for prom to remove\n");
            aux->prox = node->prox;
            free(node);
            aux=aux->prox;
            node = aux->prox;
            break;
        }
    }
    printf("After while\n");
    sigqueue(aux->pid,SIGUSR1,si);
}

void printPromList(pPromoter list){
    pPromoter aux=list;
    while(aux!=NULL){
        printf("Promoter-> %s\nState-> %d\n",aux->nome,aux->stop);
        aux = aux->prox;
    }
}

int checkIfPromExists(pPromoter list,pPromoter prom){
    pPromoter aux;
    if(list==NULL){
        return 0;
    }
    else{
        aux = list;
        while(aux!=NULL){
            if(strcmp(aux->nome,prom->nome)==0){
                aux->stop=0;
                return 1;
            }
            aux = aux->prox;
        }
        return 0;
    }
    return -1;
}

pPromoter addPromoterToList(pPromoter list,pPromoter prom){
    pPromoter aux = list;
    if(list == NULL){
        list = prom;
        list->prox==NULL;
    }
    else{
        while(aux->prox!=NULL){
            aux = aux->prox;
        }
        aux->prox = prom;
        aux->stop=0;
    }
    return list;
}

pPromoterList loadPromoterFile(pPromoterList list){
    pPromoter newPromoter;
    int numberRead=0;
    FILE* f;

    stopAllProm(list->list);

    f=fopen(getenv(FPROMOTERS),"r");
    if(f==NULL){
        printf("Error openning promoters file");
        return NULL;
    }
    while(!feof(f)){
        newPromoter= malloc(sizeof(promoter));
        if(newPromoter==NULL){
            printf("Error allocating");
            return NULL;
        }
        fscanf(f,"%s",newPromoter->nome);
        numberRead++;
        if(!checkIfPromExists(list->list,newPromoter)){
            list->list = addPromoterToList(list->list,newPromoter);
        }
    }
    list->numPromoters=numberRead;
    fclose(f);
    return list;
}

void* readFromPromoter(void* data){

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


//CMD

void getCommand(){
    char command[MAX_CMD_SIZE];
    char* token;
    pItem itemList=NULL;
    pPromoterList promList;

    

    promList->numPromoters=0;
    promList->list=NULL;
    itemList = setupItemList(itemList);
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
            printPromList(promList->list);
        }
        else if(strcmp(command,"reprom")==0){
            promList = loadPromoterFile(promList);
            printf("\nPromoters running-> %d\n",promList->numPromoters);

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
    if(freeAllPromoterList(promList->list)==0){
        printf("Promoters freed successfully\n");
    }
    freeSaveList(itemList);
}

int main(int argc,char* argv[]){
    int opt,p[2],pipe;
    message mess;
    char pipeName[STR_SIZE];


    initializeAmbVars();


    if(mkfifo(FIFOBACKEND,0600)==-1){
        if(errno==EEXIST)
            printf("backend already running...\n");
        else{
            printf("Error creating pipe\n");
        }
        exit(1);
    }
    pipe=open(FIFOBACKEND,O_RDWR);

    getCommand();


    //printList(itemList);
    
    close(pipe);
    unlink(pipe);
    endBackend();

    return 0;
}