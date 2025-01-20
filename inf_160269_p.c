#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "myStructs.h" 
#include <stdbool.h>
#include <unistd.h>


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

// 
#define INITIAL_COMUNICATION_KEY 123456

struct init_msg msg;


int initial_connection_to_dist(int id){
    if(id == -1){
        printf("msgget not connected\n");
        return -1;
    }
 
    msg.type = INITIAL_CHANEL;

    msgsnd(id, &msg,  sizeof(msg) - sizeof(long), 0); //wysłanie zapytania czy mg nadawać takie informację i sprawdzenie czy id nie jest już zajęte
    struct producent_distributor_feedback feedback;
    printf("Waiting for feedback, request sended\n"); //debug
    msgrcv(id, &feedback, sizeof(feedback) - sizeof(long), PRODUCENT_DISTRIBUTOR_FEEDBACK, 0);

    if(feedback.status == 0){
        printf("Initialized communication with producent_id: %d\n", msg.id_producent);
        return 0;
        }
    else if(feedback.status == -1){
        printf("Somebody arleady broadcast this type of information\n");
        return -1;
    }
    else{
        printf("Your prefered id is used by andother broadcaster, distributor has given you differend producer_id: %d\n", feedback.status);
        msg.id_producent = feedback.status;
        return 0;
    }


    return feedback.status;
}

void choose_news_type(struct init_msg *msg){
    printf("Choose news type: \n");
    for(int i = 0; i<10; i++){
        printf("%d. %s\n", i+1, types_of_info[i]);
    }
    int type;
    scanf("%d", &type);
    for(int i = 0; i < 5; i++) {
        msg->info_type[i] = 0;
    }
    msg->info_type[0] = type;
    return;
}



int main(){
    
    int queue_id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    int initial_info_type;
    int my_id;

    choose_news_type(&msg);

    printf("Podaj pod jakie chcesz mieć id: \n");  
    scanf("%d", &my_id);
    msg.id_producent = my_id;

    int connection_status = initial_connection_to_dist(queue_id);
    struct news news_to_broadcast;
    while (1)
    {   
        switch (connection_status)
        {
        case 0: //połączenie zostało nawiązane, można nadawać informacje
        { 
            my_id = msg.id_producent;
            break;
        }
        case -1: //nie można nadawać tego typu informacji
        {
            printf("Somebody already broadcasts this type of informaton.\n");
            choose_news_type(&msg);
            connection_status = initial_connection_to_dist(queue_id);
            break;
        }
        default://trzeba zmienić id bo jest zajęte
        {
            printf("trzeba zmienić id bo jest zajęte\n");
            my_id = connection_status;
            connection_status = 0;
            break;
        }}

        if(connection_status == 0)
        {
            printf("Wybierz akcję: \n 1. Nadaj wiadomość. [1] \n 2. Dodaj kanał do nadawania. [2] \n 3. Wyjdź [3] \n\n");
            int input_option = 0;
            scanf("%d",&input_option);
            switch(input_option)
            {
                case 1:
                {   
                    printf("Podaj treść wiadomości do napisania:\n");
                    char message_content[256] = " ";
                    scanf(" %[^\n]", message_content);
                    strcpy(news_to_broadcast.news_content, message_content);
                    printf("Podaj na który kanał nadać wiadomość, masz do wyboru:\n");
                    for(int i = 0;i<5;i++)
                    {
                         if(msg.info_type[i])
                         {
                            printf("%d \n",msg.info_type[i]);
                         }
                    }
                    int input_channel_to_broadcast;
                    scanf("%d", &input_channel_to_broadcast);

                    for(int i = 0; i <5; i++ )
                    {
                            if(msg.info_type[i] == input_channel_to_broadcast)
                            {
                                break;
                            }
                            else if(i == 4)
                        {
                            printf("You are not allowed to broadcast on this chanel:\n");
                            break;
                        }
                    }

                    news_to_broadcast.type = input_channel_to_broadcast;
                    news_to_broadcast.id_poroducer = my_id;
                    msgsnd(queue_id, &news_to_broadcast, sizeof(news_to_broadcast) - sizeof(long), 0);
                    printf("Wiadomość została nadana, na kanale %d. \n", news_to_broadcast.type);
                    break;
                }
                case 3:
                {
                    //nie wiem czy trzeba zwalniać miejsce ???????????????????????????
                    return 0; 
                }
                case 2: ////prosze o dostep do nowego kanału
                {   
                    bool flag = false;
                                        
                    printf("Choose new chanel to broadcast type: \n");
                    for(int i = 0; i<10; i++){
                        printf("%d. %s\n", i+1, types_of_info[i]);
                    }
                    int type;
                    scanf("%d", &type);

                    for(int i =0; i<5; i++)
                    {
                        if(msg.info_type[i] == type)
                        {
                            printf("You are already broadcasting on this chanel\n");
                            break;
                        }
                        else if(msg.info_type[i] == 0)
                        {
                            msg.info_type[i] = type;
                            
                            break;
                        }
                        else if(i == 4)
                        {
                            printf("You are broadcasting at maximum number of channels possible for one producer.\n");
                            flag = true;
                        }
                    }
                    if(flag)
                    {
                          break;
                    }

                    struct updating_channels rqst_to_add_chanel;
                    rqst_to_add_chanel.type = UPDATING_CHANEL;
                    rqst_to_add_chanel.id_producent = my_id;
                    rqst_to_add_chanel.new_chanel_to_broadcast = type;
                    msgsnd(queue_id, &rqst_to_add_chanel, sizeof(rqst_to_add_chanel) - sizeof(long), 0);
                    msgrcv(queue_id, &rqst_to_add_chanel, sizeof(rqst_to_add_chanel) - sizeof(long), UPDATING_CHANEL, 0);
                    printf("%d",rqst_to_add_chanel.status);
                    if(rqst_to_add_chanel.status == 0)
                    {
                        printf("Kanał został dodany\n");
                        ///musze dodac to pierwszego wolnego miejsca ze mam ten kanał
                    }
                    else if(rqst_to_add_chanel.status == -1)
                    {
                        printf("Nie ma takiego kanału\n");
                    }
                    else if(rqst_to_add_chanel.status == -2)
                    {
                        printf("Kanał jest zajęty\n");
                    }
                    break;
                }
                default:
                {
                    printf("Nie ma takiej opcji\n");
                    break;
                }
            }


        }

    }
    
    //msg.type = 1 ;
    //msg.info = prod_info;
    //msgsnd(id, &msg,sizeof(msg) - sizeof(long), 0);

    return 0;
}