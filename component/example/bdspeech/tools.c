//
// Created by ligaofeng on 19-3-20.
//

#include "tools.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

unsigned long long get_current_time()
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    unsigned long long time = current_time.tv_sec;
    time = time * 1000000 + current_time.tv_usec;
    return time;
}

#include <string.h>
void set_current_time(char* time_str) {
    printf("=======cmd========%s================\n", time_str);
    char second[11] = {0}; 
    char usecond[10] = {0};
    memcpy(second, time_str + 5, 10);
    char* time_us = strstr(time_str, ".");
    memcpy(usecond, time_us + 1, 9);

    printf("=======str========%s,%s================\n", second, usecond);

    struct timeval current_time;
    current_time.tv_sec = strtol(second, NULL, 10);
    current_time.tv_usec = strtol(usecond, NULL, 10);

    printf("=======long========%ld,%ld================\n", strtol(second, NULL, 10), strtol(usecond, NULL, 10));
    settimeofday(&current_time, NULL);
}

int generate_uuid(char* uuid_out)
{
    const char *c = "89ab";
    char buf[37];
    char *p = buf;
    int n = 0;
    srand((unsigned)get_current_time() * 16);
    for (n = 0; n < 16; ++n)
    {
        int b = rand() % 255;
        switch(n)
        {
            case 6:
                sprintf(p, "4%x", b % 15);
                break;
            case 8:
                sprintf(p, "%c%x", c[rand() % strlen(c)], b % 15);
                break;
            default:
                sprintf(p, "%02x", b);
                break;
        }

        p += 2;
        switch(n)
        {
            case 3:
            case 5:
            case 7:
            case 9:
                *p++ = '-';
                break;
        }
    }
    *p = 0;

    strcpy(uuid_out, buf);
    return strlen(buf);
}

