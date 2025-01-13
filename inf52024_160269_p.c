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

// 
#define INITIAL_COMUNICATION_KEY 11111

static char* types_of_info[]={
    "ROZRYWKA", //0
    "SPORT" , //1
    "SWIAT" , //2
    "POLITYKA" , //3
    "KONFLITKY" , //4   
    "ODKRYCIA" , //5
    "POGODA" , //6
    "WYWIADY" , //7
    "GIELDA" , //8
    "DEBATA"  //9
    };

struct init_info
{
    int pro_id;
    int info_type[5];
}prod_info;

struct test_connection
{
    int pro_id;
    int test_int;} test; 

struct init_msg
{
    long type;  //1 dla init 
    struct init_info info;
}msg;

int initial_connection_to_dist(){
    int id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    if(id == -1){
        printf("msgget not connected\n");
        return -1;
    }
    msg.type = 1;
    msg.info = prod_info;
    msgsnd(id, &prod_info, sizeof(prod_info), 0); //wysłanie zapytania czy mg nadawać takie informację i sprawdzenie czy id nie jest już zajęte
    printf("Sent message: %d\n", msg.info.pro_id);

    return id;
}

int main(){
    int id;
    int initial_info_type;

    printf("Podaj jakie informację będziesz nadawać: \n");
    for(int i=0; i<10; i++){
        printf("wybierz %d dla:  %s \n",i, types_of_info[i]);     
    }
    scanf("%d", &initial_info_type);


    for(int i = 0 ; i <5; i++)
    {   if(i == 0)
        prod_info.info_type[i] = initial_info_type;
        else
        prod_info.info_type[i]  = 0;
    }
    prod_info.pro_id = 1;   



    //initial_connection_to_dist();
    id = msgget(INITIAL_COMUNICATION_KEY, IPC_CREAT | 0644);
    test.pro_id = 1;
    test.test_int = 22;
    msgsnd(id, &test, sizeof(test), 0);


    return 0;
}