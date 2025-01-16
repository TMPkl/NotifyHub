#ifndef MYSTRUCTS_H
#define MYSTRUCTS_H

#define INITIAL_CHANEL 11
#define UPDATING_CHANEL 12
#define PRODUCENT_DISTRIBUTOR_FEEDBACK 13


struct init_msg
{
    long type;  //INITIAL_CHANEL dla inicjalizacji kanału UPDAING_CHANEL dla aktualizacji
    int info_type[5];
    int id_producent;
};

struct news
{
    long type;   // typ zależy od treści [0-9]
    char news_content[100];
};

struct producent_distributor_feedback{
    long type;
    int status; //0 if all good, [1-10] if id_producent is useed and the status is producent_id , or -1 if info_type is used
};

static char* types_of_info[]={
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


#endif // MYSTRUCTS_H