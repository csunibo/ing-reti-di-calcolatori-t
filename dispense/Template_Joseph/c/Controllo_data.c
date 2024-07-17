#define DATA 11
#include <string.h>


//formato gg/mm/aaaa
int checkData(char data[DATA]){
    int res = -1;


    if( strlen(data) == 10 &&
        data[2] == '/' && data[5] == '/'  &&
        data[0]>= '0' && data[0] <= '9' &&
        data[1]>= '0' && data[1] <= '9' &&
        data[3]>= '0' && data[3] <= '9' &&
        data[4]>= '0' && data[4] <= '9' &&
        data[6]>= '0' && data[6] <= '9' &&
        data[7]>= '0' && data[7] <= '9' &&
        data[8]>= '0' && data[8] <= '9' &&
        data[9]>= '0' && data[9] <= '9'
    ) res = 0;


    return res;
}