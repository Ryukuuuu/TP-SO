#include "utils.h"


int argsCount(char cmd[]){
    int count=0,i;
    //printf("\nInicio argsCount\n");

    for(i=0;cmd[i]!='\0';i++){
        if(cmd[i]==' '){
            count++;
        }
    }
    //printf("\nCOUNT: %d\n",count);
    return ++count;     
}

int stringIsNum(char str[]){
    int isNum=1;
    //printf("------------------------->%s",str);
    fflush(stdout);
    for(int i=0;i<strlen(str);i++){
        if(!isdigit(str[i])){
            isNum=0;
            break;
        }
    }
    return isNum;
}