#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "myStructs.h"
/*
ROZRYWKA 0
SPORT 1
SWIAT 2
POLITYKA 3
KONFLITKY 4
ODKRYCIA 5
POGODA 6
WYWIADY 7
GIELDA 8
DEBATA 9

*/

#define INITIAL_COMUNICATION_KEY 123456

struct init_info prod_info;
struct init_msg msg;



int chanel_in_use[10] = {0}; // 0 dla wolnego,  id_producenta dla zajetego



int is_chanel_free(int chanel){
    if(chanel_in_use[chanel] == 0){
        return 1;
    }
    return 0;
}



void init_comunicatnion(int init_queue_id){
    
    struct init_msg msg;
    if (msgrcv(init_queue_id, &msg, sizeof(msg) - sizeof(long), INITIAL_CHANEL, IPC_NOWAIT)>0)
    {   
    //printf("kanał którym ma byc nadawane: %d\n", prod_info.pro_id);
    //printf("Typ nadawanych wiadomości: %d\n", prod_info.info_type[0]);
    if(chanel_in_use[msg.info_type[0]] == 0 and ){
        chanel_in_use[msg.info_type[0]] = msg.id_producent;
        //printf("Kanał %d zajęty przez producenta %d\n", prod_info.info_type[0], prod_info.pro_id);

    
    

    }
}
int main(){
    int id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);

    struct init_msg msg;
    msgrcv(id, &msg, sizeof(msg) - sizeof(long), INI, 0);
    
    printf("kanał którym ma byc nadawane: %d\n", prod_info.pro_id);
    printf("Typ nadawanych wiadomości: %d\n", prod_info.info_type[0]);

    while (1)
    {
        ;
    }
    


}
