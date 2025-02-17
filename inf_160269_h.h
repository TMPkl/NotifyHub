#ifndef INF_16029_H_H
#define INF_16029_H_H

#define INITIAL_CHANEL 11
#define UPDATING_CHANEL 12
#define PRODUCENT_DISTRIBUTOR_FEEDBACK 13
#define UPDATKING_CHANEL_FEEDBACK 14

#define INITIAL_CLIENT_DISTRIBUTOR_CHANEL 15
#define INITIAL_CLIENT_DISTRIBUTOR_FEEDBACK 16

#define NEWS_REQUEST 11 //klient pyta o newsy
#define NEWS_BROADCAST 12 //distributor wysyła liste dostepnych newsów
#define SUBS_DEL 13 //klient chce usunąć subskrypcje

struct init_client
{
    long type;
    int id_client; //musi być unikalne, 4 cyfry
};

struct ping
{
    long type;
    int id_client;
};

struct news_request
{
    long type;
    int id_client;
    int chanel[10];
};


struct init_msg
{
    long type;  //INITIAL_CHANEL dla inicjalizacji kanału UPDAING_CHANEL dla aktualizacji
    int info_type[5];
    int id_producent;
};

struct news
{
    long type;   // typ zależy od treści [0-9]
    int id_poroducer;
    char news_content[100];
};

struct producent_distributor_feedback{
    long type;
    int status; //0 if all good, [1-10] if id_producent is useed and the status is producent_id , or -1 if info_type is used
};

struct updating_channels{
    long type;
    int id_producent;
    int new_chanel_to_broadcast;
    int status; 
};
struct delete_chanel{
    long type;
    int id_producent;
    int chanel_to_delete;
};
static char* types_of_info[]={
    "ROZRYWKA" , //1
    "SPORT" , //2
    "SWIAT" , //3
    "POLITYKA" , //4
    "KONFLITKY" , //5   
    "ODKRYCIA" , //6
    "POGODA" , //7
    "WYWIADY" , //8
    "GIELDA" , //9
    "DEBATA"  //10
    };


#endif //  INF_16029_H_H