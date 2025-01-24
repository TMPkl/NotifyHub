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
        struct news_request list_of_producers;
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
    init_producer();
    int choice;

    printf("Aby zasubskryować nowy kanał wybierz 1, aby odczytać newsy wybierz 2: ");
    while (1)
    {   
        printf("Aby zasubskryować nowy kanał wybierz 1, aby odczytać newsy wybierz 2: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        {
            ///rozpisać dodawanie subskrypcji, w d jest już zrobione 
        }
        case 2:
            //rozpisać odczytywanie newsów
            break;
        default:{
            printf("Nieprawidłowy wybór\n");
            break;
        }}
        choice = 0;
        
        
    }
    
    

    






}