#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "inf_160269_h.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>



#define INITIAL_COMUNICATION_KEY 123456

int my_id;
struct init_client client;
struct news_request list_of_producers;
int news_queue_id;

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
        
        for (int i = 0; i < 10; i++)
        {
            list_of_producers.chanel[i] = 0;
        }
        msgrcv(news_queue_id, &list_of_producers, sizeof(list_of_producers) - sizeof(long), NEWS_BROADCAST, 0);
        printf("Lista producentów: \n");

        int j =1;
       
        for (int i = 0; i < 10; i++)
        {
            if(list_of_producers.chanel[i] != 0){
                printf("%d. kanał o treści: %s\n", i+1, types_of_info[list_of_producers.chanel[i]-1]);
                j++;
        }
        }
        printf("Wybierz kanał: ");
        printf("\n");
        int chanel;
        scanf("%d", &chanel);

        for(int i =0; i<10; i++)
        {
            if(list_of_producers.chanel[i] == chanel)
            {
                printf("Wybrany kanał: %s\n", types_of_info[chanel-1]);
                break;
            }
            else if(i == 9)
            {
                printf("Nie ma takiego kanału, spróbuj ponownie później.\n");
                struct news_request news_rqst;
                news_rqst.type = NEWS_REQUEST;
                news_rqst.id_client = my_id;
                for (int i = 0; i < 10; i++)
                {
                news_rqst.chanel[i] = 0;
                }
                msgsnd(news_queue_id, &news_rqst, sizeof(news_rqst) - sizeof(long), 0);

                return;
            }
        }

        struct news_request news_rqst;
        news_rqst.type = NEWS_REQUEST;
        news_rqst.id_client = my_id;
        for (int i = 0; i < 10; i++)
        {
            news_rqst.chanel[i] = 0;
        }
        news_rqst.chanel[chanel-1] = 1;
        msgsnd(news_queue_id, &news_rqst, sizeof(news_rqst) - sizeof(long), 0);

        return;
    }


}


int main(){

    printf("######Klient:\n");
    my_id = -1;
    while(my_id == -1)
        init_producer();
    
    int choice;

    while (1)
    {   
        printf("Aby zasubskryować nowy kanał wybierz 1, aby odczytać newsy wybierz 2: ");
        scanf("%d", &choice);
        system("clear");
        switch (choice)
        {
        case 1:
        {
            struct news_request list_of_channels;
            struct ping ping;
            ping.type = NEWS_REQUEST;
            ping.id_client = my_id;
            msgsnd(news_queue_id, &ping, sizeof(ping) - sizeof(long), 0);
            //printf("Wysłano zapytanie o listę kanałów\n");
            msgrcv(news_queue_id, &list_of_channels, sizeof(list_of_channels) - sizeof(long), NEWS_BROADCAST, 0);
            printf("Lista kanałów: \n");
            for(int i = 0; i<10; i++)
            {
                if(list_of_channels.chanel[i] != 0)
                {
                    printf("%d. %s\n", i+1, types_of_info[i]);
                }
            }
            printf("Wybierz kanał: ");
            int chanel;
            scanf("%d", &chanel);
            struct news_request news_rqst;
            news_rqst.type = NEWS_REQUEST;
            news_rqst.id_client = my_id;
            for (int i = 0; i < 10; i++)
            {
                news_rqst.chanel[i] = 0;
            }
            news_rqst.chanel[chanel-1] = 1;
            msgsnd(news_queue_id, &news_rqst, sizeof(news_rqst) - sizeof(long), 0);
            break;
        }
        case 2:
            {
            struct news news_to_read;
            bool was_news = false;
                while (msgrcv(news_queue_id, &news_to_read, sizeof(news_to_read) - sizeof(long), -10, IPC_NOWAIT) > 0)
                {
                    printf("#######Otrzymano news; tytuł:%s, treść: \n %s\n", types_of_info[news_to_read.type-1], news_to_read.news_content);
                    printf("\n");

                    was_news = true;
                }
                printf("\n");
                printf(was_news ? "Prczeczytano wszyskie newsy\n" : "Brak nowych newsów\n");
                break;
            }
        default:{
            printf("Nieprawidłowy wybór\n");
            break;
        }}
        choice = 0;
        
        
    }
    
    

    






}