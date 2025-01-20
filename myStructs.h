#ifndef MYSTRUCTS_H
#define MYSTRUCTS_H

#define INITIAL_CHANEL 11
#define UPDATING_CHANEL 12
#define PRODUCENT_DISTRIBUTOR_FEEDBACK 13
#define UPDATKING_CHANEL_FEEDBACK 14

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


#endif // MYSTRUCTS_H