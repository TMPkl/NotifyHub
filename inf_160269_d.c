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

int chanel_subcribers[10][10]; // lista subskrybentów dla danego kanału - maksymalnie 10 subskrybentów na 10 kanałów
int connected_clients_id[10] = {0}; // lista id klientów podłączonych do dystrybutora - maksymalnie 10 klientów

int clients_queue_id[10]= {0}; // lista id kolejek klientów - maksymalnie 10 klientów

struct news client_news;
void client_new_sub(int client_queue_id); // Forward declaration




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
        for(int j = 0; j<10; j++){
            if(producents_connected_id[j] == i){
                break;
            }
            if(j == 9){
                return i;
            }
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
    
    struct updating_channels feedback;

    feedback.id_producent = rqst_to_add_chanel.id_producent;
    feedback.new_chanel_to_broadcast = rqst_to_add_chanel.new_chanel_to_broadcast;
    feedback.type = UPDATKING_CHANEL_FEEDBACK;

    if(rqst_to_add_chanel.new_chanel_to_broadcast <=0 || rqst_to_add_chanel.new_chanel_to_broadcast > 10)
    {
        feedback.status = -1;                                   // nie ma takiego kanału

        msgsnd(updating_chanell_id, &feedback, sizeof(feedback) - sizeof(long), 0);
        
        return;
    }
    printf("debug1\n");
    if(chanel_in_use[rqst_to_add_chanel.new_chanel_to_broadcast-1] == 0)
    {
        chanel_in_use[rqst_to_add_chanel.new_chanel_to_broadcast-1] = rqst_to_add_chanel.id_producent;
        add_producent(rqst_to_add_chanel.id_producent);
        
        feedback.status = 0;                                                    // kanał dodany

        msgsnd(updating_chanell_id, &feedback, sizeof(feedback) - sizeof(long), 0);
        return;
    }
    else
    {

        feedback.status = -2;                                               // kanał zajęty
        msgsnd(updating_chanell_id, &feedback, sizeof(feedback) - sizeof(long), 0);
        return;
    }
    
} 

void update_chanel_subscribers(int client_queue_id, struct news_request *news_rqst) {
            for (int i = 0; i < 10; i++) {
                if (news_rqst->chanel[i] != 0) {
                    for (int j = 0; j < 10; j++) {
                        if (chanel_subcribers[i][j] == 0) {
                            chanel_subcribers[i][j] = client_queue_id;
                            break;
                        }
                    }
                }
            }
        }
void client_new_sub(int client_queue_id)
{   
    struct news_request news_rqst;
    if(msgrcv(client_queue_id, &news_rqst, sizeof(news_rqst) - sizeof(long), NEWS_REQUEST, IPC_NOWAIT)>0)
    {
        printf("Nowa subskrypcja \n");
        update_chanel_subscribers(client_queue_id, &news_rqst);
    }
    return;
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
                                                                   // printf("Wysłano feedback\n");
            }
            else
            {   
                                                                    printf("Kanał jest zajęty  ");
                feedback.type = PRODUCENT_DISTRIBUTOR_FEEDBACK;
                feedback.status = -1;
                msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                                                                   // printf("Wysłano feedback\n");

            }

        }
        else
        { 
            if(is_chanel_free(msg.info_type[0]))
                {                                                      printf("Id jest zajęte\n");
                    feedback.type = PRODUCENT_DISTRIBUTOR_FEEDBACK;
                    feedback.status = find_free_id();
                    chanel_in_use[msg.info_type[0]-1] = feedback.status;
                    add_producent(feedback.status);
                    
                    msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                                                                            //printf("Wysłano feedback\n");
                    
                }
            else
            {   
                                                                    printf("Kanał i id jest zajęte  ");
                feedback.type = PRODUCENT_DISTRIBUTOR_FEEDBACK;
                feedback.status = -1;
                msgsnd(init_queue_id, &feedback, sizeof(feedback) - sizeof(long), 0);
                                                                    //printf("Wysłano feedback\n");

            }
        }
    }
}

void add_new_subs()
{
    struct ping ping;
    struct news_request list_of_producers;
    list_of_producers.type = NEWS_BROADCAST;
    for(int i = 0;i<10; i++)
    {
        if(chanel_in_use[i] != 0)
        {
            list_of_producers.chanel[i] = 1;                
        }
        else
        list_of_producers.chanel[i] = 0;
    }
    

    for(int i = 0; i<10; i++)
        {
            
            //client_new_sub(clients_queue_id[i]);
            if(msgrcv(clients_queue_id[i], &ping, sizeof(ping) - sizeof(long), NEWS_REQUEST, IPC_NOWAIT)>0)
            {
                msgsnd(clients_queue_id[i], &list_of_producers, sizeof(list_of_producers) - sizeof(long), 0);             
                client_new_sub(clients_queue_id[i]);
            }
        
        }
}

void init_client(int id)
{
    struct init_client client;
    struct producent_distributor_feedback feedback;
    client.type = INITIAL_CLIENT_DISTRIBUTOR_CHANEL;
    feedback.type = INITIAL_CLIENT_DISTRIBUTOR_FEEDBACK;

    if (msgrcv(id, &client, sizeof(client) - sizeof(long), INITIAL_CLIENT_DISTRIBUTOR_CHANEL, IPC_NOWAIT)>0)
    { 
        printf("Otrzymano wiadomość o checi uzyskania ip przez klienta, wysylanie feedback\n");
        if (client.id_client <0 || client.id_client > 999)
        {
            feedback.status = -1;
            msgsnd(id, &feedback, sizeof(feedback) - sizeof(long), 0);
            return;
        }
        for(int i = 0; i <10; i++)
        {
            if(connected_clients_id[i] == client.id_client)
            {
                feedback.status = -1;
                msgsnd(id, &feedback, sizeof(feedback) - sizeof(long), 0);
                return;
            }
        }
        for (int i = 0; i < 10; i++)
        {
            if (connected_clients_id[i] == 0)
            {
                connected_clients_id[i] = client.id_client;
                break;
            }
        }


        feedback.status = 0;
        msgsnd(id, &feedback, sizeof(feedback) - sizeof(long), 0);

        key_t  key = client.id_client*1000;
        int client_queue_id = msgget(key, IPC_CREAT | 0644);
        if(client_queue_id == -1)
        {
            printf("Nie udało się utworzyć kolejki dla klienta\n");
            return;
        }
        for(int i = 0; i<10; i++)
        {
            if(clients_queue_id[i] == 0)
            {
                clients_queue_id[i] = client_queue_id;
                break;
            }
        }
        
        struct news_request news_rqst;
        struct news_request list_of_producers;
        list_of_producers.type = NEWS_BROADCAST;
        list_of_producers.id_client = client.id_client;
        int j = 0;
        for(int i = 0; i<10; i++)
        {   
            if(chanel_in_use[i] != 0)
            {
                list_of_producers.chanel[j] = i+1;                 //wysłanie listy producentów, kolejne numery kanałów
                j++;         
            }
            else
            {
                list_of_producers.chanel[j] = 0;
                j++;
            }
        }
        msgsnd(client_queue_id, &list_of_producers, sizeof(list_of_producers) - sizeof(long), 0);
        msgrcv(client_queue_id, &news_rqst, sizeof(news_rqst) - sizeof(long), NEWS_REQUEST, 0);
        printf("Otrzymano zapytanie o newsy od klienta o id: %d, o kanał: %d\n", news_rqst.id_client);

        update_chanel_subscribers(client_queue_id, &news_rqst);
        
        return;
    }
    else
    {
        return;
    }
}




int main(){
    printf("######Distributor:\n");

    for(int i = 0; i<10; i++){
        for(int j = 0; j<10; j++){
            chanel_subcribers[i][j] = 0;
        }
    }



    int id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    struct news news_to_broadcast;

    while (1)
    {   
        news_to_broadcast.type = 1231;
        strcpy(news_to_broadcast.news_content, " "); //wyczyszczenie poprzedniej wiadomosć 

        init_comunicatnion(id);
        add_chanel_to_producer(id);
        init_client(id);
        
                                                // for(int i = 0;i<10;i++)
                                                // {
                                                //     printf("%d ",i+1);
                                                // }
                                                // printf("\n");
                                                // for(int i = 0;i<10;i++)
                                                // {
                                                //     printf("%d ",chanel_in_use[i]);
                                                // }
                                                // printf("\n");
                                                // printf("\n");
        for(int i = 0; i<10; i++){
            for(int j = 0; j<10; j++){
                printf("%d ", chanel_subcribers[i][j]);
            }
            printf("\n");
        }

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
