#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "myStructs.h"
#include <stdbool.h>
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
int chanel_in_use[10] = {0}; // 0 dla wolnego,  id_producenta dla zajetego - maysalnie obułguje 10 kanałów
int producents_connected_id[10] = {0}; // lista id producentów obecnie podłączonych do nadawcy - maksylnie obsługuje 10 producentów 



bool is_chanel_free(int chanel){
    if(chanel_in_use[chanel] == 0){
        return true;
    }
    return false;
}



bool is_id_free(int id){
    if(!producents_connected_id[id]){
        return true;
    }
    return false;
}

int find_free_id(){
    for(int i = 1;; i++){
        if(!producents_connected_id[i]){
            return i;
        }
    }
    return -1;
}



void init_comunicatnion(int init_queue_id){
    
    struct init_msg msg;
    struct producent_distributor_feedback feedback;
    if (msgrcv(init_queue_id, &msg, sizeof(msg) - sizeof(long), INITIAL_CHANEL, IPC_NOWAIT)>0)
    { 
        printf("Otrzymano wiadomość, wysylanie feedback\n");  

        //printf("kanał którym ma byc nadawane: %d\n", prod_info.pro_id);
        //printf("Typ nadawanych wiadomości: %d\n", prod_info.info_type[0]);
        if (is_id_free(msg.id_producent))
        {
                                                                    //printf("Id jest wolne\n");
            if(is_chanel_free(msg.info_type[0]))
            {
                                                                    printf("Kanał i id jest wolne  ");
                chanel_in_use[msg.info_type[0]] = msg.id_producent;
                producents_connected_id[msg.id_producent] = true;
                feedback.type = PRODUCENT_DISTRIBUTOR_FEEDBACK;
                feedback.status = 0;
                msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                                                                    printf("Wysłano feedback\n");
            }
            else
            {   
                                                                    printf("Kanał jest zajęty  ");
                feedback.type = PRODUCENT_DISTRIBUTOR_FEEDBACK;
                feedback.status = -1;
                msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                                                                    printf("przydzielono inne id: %d\n", feedback.status);
                                                                    printf("Wysłano feedback\n");

            }

        }
        else
        {
                                                                    printf("Id jest zajęte\n");
            feedback.type = PRODUCENT_DISTRIBUTOR_FEEDBACK;
            feedback.status = find_free_id();
            msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                                                                    printf("Wysłano feedback\n");
            
        }
    }
}
int main(){
    printf("######Distributor:\n");
    int id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    
    while (1)
    {   
        struct news news_to_broadcast;
        news_to_broadcast.type = 0;
        strcpy(news_to_broadcast.news_content, " "); //wyczyszczenie poprzedniej wiadomosć 
        init_comunicatnion(id);
        for(int i = 0; i<9; i++){
            if(chanel_in_use[i])
            {
                msgrcv(id, &news_to_broadcast, sizeof(news_to_broadcast) - sizeof(long), i+1, IPC_NOWAIT);
                if(news_to_broadcast.type == i)
                {
                    printf("Otrzymano wiadomość do przekierowania: %s od %d\n", news_to_broadcast.news_content, i);
                }
            }
        }
    }
    //msgctl(id, IPC_RMID, NULL);
}
