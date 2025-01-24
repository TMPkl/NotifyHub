#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "myStructs.h" 
#include <stdbool.h>
#include <unistd.h>


#define INITIAL_COMUNICATION_KEY 123456

int my_id;
struct init_client client;
int news_queue_id

void init_producer()
{   int init_id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    printf("Podaj pod jakie chcesz mieć id: \n");
    scanf("%d", &my_id);
    client.id_client = my_id;
    client.type = INITIAL_CLIENT_DISTRIBUTOR_CHANEL;
    msgsnd(init_id, &client, sizeof(client) - sizeof(long), 0);
    struct producent_distributor_feedback feedback;
    msgrcv(init_id, &feedback, sizeof(feedback) - sizeof(long), INITIAL_CLIENT_DISTRIBUTOR_FEEDBACK, 0);
    if (feedback.status == -1)
    {
        printf("Podane id jest nieprawidłowe, nie udało się połączć.\n");
        my_id = -1;
        return;
    }
    

    else{
        printf("Połączono z id: %d\n", my_id);
        news_queue_id = msgget(my_id*1000, IPC_CREAT | 0644);
        struct news_request list_of_producers;
        msgrcv(news_queue_id, &list_of_producers, sizeof(list_of_producers) - sizeof(long), NEWS_BROADCAST, 0);
        printf("Lista producentów: \n");
        for (int i = 0; i < 10; i++)
        {
            if(list_of_producers.chanel[i] != 0)
                printf("%d. kanał o treści: %d\n", i+1, list_of_producers.chanel[i]);
        }
        


        return;
    }


}


int main(){
    printf("######Klient:\n");

    init_producer();






}