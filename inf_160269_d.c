#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "myStructs.h"
#include <stdbool.h>
#include <unistd.h>

/*
ROZRYWKA 0  na kalanł 1
SPORT 1 na kanał 2
SWIAT 2 na kanał 3
POLITYKA 3 na kanał 4
KONFLITKY 4 ... 5
ODKRYCIA 5
POGODA 6
WYWIADY 7
GIELDA 8
DEBATA 9 na kanał 10

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

void add_producent(int id){
    for(int i = 0; i<10; i++){
        if(!producents_connected_id[i] | id == producents_connected_id[i]){
            producents_connected_id[i] = id;
            return;
        }
    }
}


bool is_id_free(int id){
    for(int i = 0; i<10; i++){
        if(producents_connected_id[i] == id){
            return false;
        }
    }
    return true;
}

int find_free_id(){
    for(int i = 1;; i++){
        if(!producents_connected_id[i]){
            return i;
        }
    }
    return -1;
}

void add_chanel_to_producer(int updating_chanell_id){
    struct updating_channels rqst_to_add_chanel;
    int rcv_status = msgrcv(updating_chanell_id, &rqst_to_add_chanel, sizeof(rqst_to_add_chanel) - sizeof(long), UPDATING_CHANEL, IPC_NOWAIT);
    printf("trying to add chanels\n");
    
    if(rcv_status == -1)
        return;
    printf("debug1\n");
    
    if(rqst_to_add_chanel.new_chanel_to_broadcast <=0 || rqst_to_add_chanel.new_chanel_to_broadcast > 10)
    {
        rqst_to_add_chanel.status = -1; // nie ma takiego kanału
        msgsnd(updating_chanell_id, &rqst_to_add_chanel, sizeof(rqst_to_add_chanel) - sizeof(long), 0);
        return;
    }
    printf("debug1\n");
    if(chanel_in_use[rqst_to_add_chanel.new_chanel_to_broadcast-1] == 0)
    {
        chanel_in_use[rqst_to_add_chanel.new_chanel_to_broadcast-1] = rqst_to_add_chanel.id_producent;
        add_producent(rqst_to_add_chanel.id_producent);
        rqst_to_add_chanel.status = 0;
        msgsnd(updating_chanell_id, &rqst_to_add_chanel, sizeof(rqst_to_add_chanel) - sizeof(long), 0);
        return;
    }
    else
    {
        rqst_to_add_chanel.status = -2; // kanał jest zajęty
        msgsnd(updating_chanell_id, &rqst_to_add_chanel, sizeof(rqst_to_add_chanel) - sizeof(long), 0);
        return;
    }
    
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
                chanel_in_use[msg.info_type[0]-1] = msg.id_producent;
                add_producent(msg.id_producent);
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
            chanel_in_use[msg.info_type[0]-1] = feedback.status;
            add_producent(feedback.status);
            
            msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                                                                    printf("Wysłano feedback\n");
            
        }
    }
}
int main(){
    printf("######Distributor:\n");
    int id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    struct news news_to_broadcast;

    news_to_broadcast.type = 1231;
    strcpy(news_to_broadcast.news_content, " "); //wyczyszczenie poprzedniej wiadomosć 

    while (1)
    {   
        news_to_broadcast.type = 1231;
        strcpy(news_to_broadcast.news_content, " "); //wyczyszczenie poprzedniej wiadomosć 

        init_comunicatnion(id);
        add_chanel_to_producer(id);
                                                for(int i = 0;i<10;i++)
                                                {
                                                    printf("%d ",i+1);
                                                }
                                                printf("\n");
                                                for(int i = 0;i<10;i++)
                                                {
                                                    printf("%d ",chanel_in_use[i]);
                                                }
                                                printf("\n");
                                                printf("\n");
                                                sleep(1);

        for(int i = 0; i<10; i++){
                
            if(chanel_in_use[i])
            {
                int a = msgrcv(id, &news_to_broadcast, sizeof(news_to_broadcast) - sizeof(long), i+1, IPC_NOWAIT);
               if( a !=-1)
               {
                printf("Otrzymano wiadomość do przekierowania: %s od %d o id %d\n", news_to_broadcast.news_content, news_to_broadcast.type, news_to_broadcast.id_poroducer);
                
               }
            }
        }
    }
    //msgctl(id, IPC_RMID, NULL);
}
