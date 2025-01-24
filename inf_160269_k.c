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
        return;
    }
    

    else{
        return;
    }


}


int main(){
    printf("######Klient:\n");







}