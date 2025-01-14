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

struct init_msg msg;
int chanel_in_use[10] = {0}; // 0 dla wolnego,  id_producenta dla zajetego



int is_chanel_free(int chanel){
    if(chanel_in_use[chanel] == 0){
        return 1;
    }
    return 0;
}

int is_id_free(int id){
    for(int i = 0; i<10; i++){
        if(chanel_in_use[i] == id){
            return 0;
        }
    }
    return 1;
}


void init_comunicatnion(int init_queue_id){
    
    struct init_msg msg;
    struct producent_distributor_feedback feedback;
    if (msgrcv(init_queue_id, &msg, sizeof(msg) - sizeof(long), INITIAL_CHANEL, 0)>0)
    { 
        printf("Otrzymano wiadomość, wysylanie feedback\n");  

        //printf("kanał którym ma byc nadawane: %d\n", prod_info.pro_id);
        //printf("Typ nadawanych wiadomości: %d\n", prod_info.info_type[0]);
        if (is_id_free(msg.id_producent))
        {
            printf("Id jest wolne\n");
            if(is_chanel_free(msg.info_type[0]))
            {
                printf("Kanał jest wolny\n");
                chanel_in_use[msg.info_type[0]] = msg.id_producent;
                feedback.type = PRODUCENT_DISTRIBUTOR_FEEDBACK;
                feedback.status = 0;
                msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                printf("Wysłano feedback\n");
            }
            else
            {
                printf("Kanał jest zajęty\n");
            }

        }
        else
        {
            printf("Id jest zajęte\n");
        }
    }
}
int main(){
    int id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);

    //while (1)
    //{
        init_comunicatnion(id);
    //}
}
