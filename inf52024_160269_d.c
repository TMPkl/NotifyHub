#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>

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

#define INITIAL_COMUNICATION_KEY 11111


struct init_info
{
    int pro_id;
    int info_type[5];
}prod_info;

struct init_msg
{
    long type;
    struct init_info info;
}msg;

struct test_connection
{
    int pro_id;
    int test_int;} test; 


int main(){
    int id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);

    msgrcv(id,&test,sizeof(test),1,0);
    printf("Received message: %d\n", test.test_int);
    return 0;
}
