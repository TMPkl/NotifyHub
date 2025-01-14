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


    return id;
}

int main(){
    int queue_id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    int initial_info_type;
    int my_id;


    printf("Podaj jakie informację będziesz nadawać: \n");
    for(int i=0; i<10; i++){
        printf("wybierz %d dla:  %s \n",i, types_of_info[i]);     
    }

    scanf("%d", &initial_info_type);
    for(int i = 0 ; i <5; i++)
    {   if(i == 0)
        msg.info_type[i] = initial_info_type;
        else
        msg.info_type[i]  = 0;
    }
    printf("Podaj pod jakie chcesz mieć id: \n");  
    scanf("%d", &my_id);
    msg.id_producent = my_id;



    initial_connection_to_dist(queue_id);

    struct news news_to_broadcast;
    news_to_broadcast.type = msg.info_type[0];
    strcpy(news_to_broadcast.news_content,"Hello");
    msgsnd(queue_id, &news_to_broadcast, sizeof(news_to_broadcast) - sizeof(long), 0);

    printf("Wiadomość nadana\n");




    //msg.type = 1 ;
    //msg.info = prod_info;
    //msgsnd(id, &msg,sizeof(msg) - sizeof(long), 0);

    return 0;
}