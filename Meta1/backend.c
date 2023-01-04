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

/*int writePromToFile(pPromoterList list){
    pPromoter aux = list;
    FILE* f;


    printf("Active promoters-> %d",list->numPromoters);
    printPromList(list->list);
    
    if(list->numPromoters==1 && list->list->stop == 1){
        list->list = NULL;
    }

    f = fopen(getenv(FPROMOTERS),"w");
    if(f==NULL){
        printf("Error openning promoters file\n");
        return -1;
    }
    while(aux!=NULL){
        fprintf(f,"%s",aux->nome);
        if(aux->prox!=NULL){
            fprintf(f,"\n");
        }
        aux = aux->prox;
    }
    fclose(f);
    return 0;
}*/

int freeAllPromoterList(pPromoter list){   //Liberta e guarda todos os promotores(Chama-se quando o programa é terminado)
    pPromoter aux = list;
    FILE* f;

    printPromList(list);

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
    pPromoter aux=list->list,node;

    union sigval si;
    si.sival_int=0;

    if(aux != NULL && aux->stop==1){
        list->list = list->list->prox;
        list->numPromoters--;
        free(aux);
        return;
    }

    while(aux!=NULL && aux->stop!=1){
        node = aux;
        aux = aux->prox;
    }
    if(aux==NULL){
        return;
    }

    node->prox = aux->prox;
    sigqueue(aux->pid,SIGUSR1,si);
    aux->stop=1;
    list->numPromoters--;
    free(aux);


    //writePromToFile(list);
    
}

void printPromList(pPromoter list){
    pPromoter aux=list;
    while(aux!=NULL){
        printf("Promoter-> %s\nState-> %d\n",aux->nome,aux->stop);
        fflush(stdout);
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
    char msg[STR_SIZE];
    pPromThread pData = (pPromThread) data;
    do{
        read(pData->pipe,msg,sizeof(msg));
        msg[strcspn(msg,"\n")]=0;
        printf("\n-> %s\n",msg);
    }while(pData->p->stop == 0);
    printf("Ending thread\n");
    pthread_exit(NULL);
}

int execPromoter(pPromoter prom,pPromThread threadPromoter){
    int p[2];
    pthread_t newThread;
    threadPromoter->p = prom;
    threadPromoter->stop=0;
    
    pipe(p);

    if(prom->running==1){
        printf("Promoter already running\n");
        return 0;
    }
    
    //printf("NO EXEC\n");
    int pid=fork();
    if(pid==0){
        //printf("\nProcesso filho\n");
        close(STDOUT_FILENO);
        dup(p[1]);
        close(p[0]);
        close(p[1]);
        fflush(stdout);
        prom->running=1;
        //fprintf(stderr,"DFGDF\n");
        execl(prom->nome,prom->nome,NULL);
        prom->running=0;
        prom->pid=0;    
        fprintf(stderr,"Error lauching promoter: %s\n",prom->nome);
        return -1;
    }
    else if(pid>0){
        prom->pid=pid;
        //printf("\nTESTE-> %d\n",prom->pid);
        fflush(stdout);
        fflush(stdin);
        close(p[1]);
        threadPromoter->pipe = p[0];
        //printf("Create thread[PROM]\n");
        if(pthread_create(&newThread,NULL,&readFromPromoter,threadPromoter)!=0){
            printf("promoter thread not created\n");
        }
        threadPromoter->t = newThread;
        prom->running=1;
        //printf("THREAD\n");
        
    }
    return 1;
}

int cancelPromoter(pPromoter list,char* nome){
    pPromoter aux = list;

    while(aux!=NULL){
        if(strcmp(aux->nome,nome)==0){
            aux->stop=1;
        }
        aux=aux->prox;
    }
    printPromList(list);
}

//CMD

void getCommand(pPromoterThreadInfo promoterThreads,pItem itemList){
    char command[MAX_CMD_SIZE];
    char* token;
    pPromoterList promList;

    promList->numPromoters=0;
    promList->list=NULL;
    //printf("GSDFSDF");
    promoterThreads->currentNumThreads=0;
    
    do{
        printf("\nCommand: ");
        fgets(command,sizeof(command),stdin);
        command[strcspn(command,"\n")]=0;

        if(strcmp(command,"list")==0){
            printf("Show list\n");
            printList(itemList);
        }
        else if(strcmp(command,"users")==0){
            printf("Show users list\n");
        }
        else if(strcmp(command,"prom")==0){
            printPromList(promList->list);
            printf("Number promoters->%d",promList->numPromoters);
        }
        else if(strcmp(command,"reprom")==0){
            
            promList = loadPromoterFile(promList);
            pPromoter aux = promList->list;
            
            //printf("\n\n%s\n\n",aux->nome);
            //printf("\nPromoters running-> %d\n",promList->numPromoters);
            for(int i=0;i<promList->numPromoters && aux!=NULL;i++){
                if(execPromoter(aux,&promoterThreads->threadArray[i]) == 1){
                    promoterThreads->currentNumThreads++;
                    //printf("Promotor lancado-> %d\n",promoterThreads->currentNumThreads);
                }
                aux = aux->prox;
            }
            printPromList(promList->list);
            freePromoters(promList);
        }
        else if(strcmp(command,"close")==0){
            printf("Closing\n");
        }
        else{
            token = strtok(command," ");

            if(strcmp(command,"cancel")==0){
                token = strtok(NULL," ");
                printf("Ending promoter %s\n",token);
                cancelPromoter(promList->list,token);
                freePromoters(promList);
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
}

void endPromotersThreads(pPromoterThreadInfo promotersThreads){
    for(int i=0;i<promotersThreads->currentNumThreads;i++){
        promotersThreads->threadArray[i].stop=1;
    }
}

int main(int argc,char* argv[]){
    int opt,p[2],pipe;
    message mess;
    char pipeName[STR_SIZE];
    pPromoterThreadInfo promotersThreads = malloc(sizeof(promoterThreadInfo));
    //promotersThreads->threadArray = malloc(sizeof(pPromThread)*MAX_PROMS);
    promotersThreads->currentNumThreads=0;

    pItem itemList=NULL;

    initializeAmbVars();

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

    getCommand(promotersThreads,itemList);


    //printList(itemList);
    /*for(int i=0;i<promotersThreads->currentNumThreads;i++){
        pthread_join(promotersThreads->threadArray[i].t,NULL);
    }*/
    free(promotersThreads);
    freeSaveList(itemList);
    close(pipe);
    unlink(pipe);

    endBackend();

    return 0;
}