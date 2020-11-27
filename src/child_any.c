#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "tramsformres.h"

int main(int argc,char** argv)
{
    srand(time(NULL));
    int fd_to_par = back_transform(argv[0]);
    int fd_from_par = back_transform(argv[1]);

    char mark;
    read(fd_from_par,&mark,1); //ждем от родителя сообщение, что можно писать на stdout
    printf("[%d] process detached | %d <- SEND|GET -> %d\n",getpid(),fd_to_par,fd_from_par);
    write(fd_to_par,"1",1);

    //читаем пересланные из родителя данные для моделирования
    char inp = -1;
    int i=0; 
    char* count_in_str = (char*)malloc(sizeof(char)*1);
    char* rounds_in_str = (char*)malloc(sizeof(char)*1);
    while(inp != 0)
    {
        count_in_str = (char*)realloc(count_in_str,sizeof(char)*(i+1)); 
        read(fd_from_par,&inp,1);
        count_in_str[i] = inp;
        i++;
    }
    count_in_str[i] = 0; 

    inp = -1;i=0;
    while(inp != 0)
    {
        rounds_in_str = (char*)realloc(rounds_in_str,sizeof(char)*(i+1)); 
        read(fd_from_par,&inp,1);
        rounds_in_str[i] = inp;
        i++;
    }
    rounds_in_str[i] = 0; 
    
    long long count_of_tries = back_transform(count_in_str);
    long long count_of_rounds = back_transform(rounds_in_str);
    free(count_in_str);
    free(rounds_in_str);

    i=0;inp = -1;
    char* a_count = (char*)malloc(sizeof(char)*1);
    char* b_count = (char*)malloc(sizeof(char)*1);
    while(inp != 0)
    {
        a_count = (char*)realloc(a_count,sizeof(char)*(i+1)); 
        read(fd_from_par,&inp,1);
        a_count[i] = inp;
        i++;
    }
    a_count[i] = 0; 

    inp = -1;i=0;
    while(inp != 0)
    {
        b_count = (char*)realloc(b_count,sizeof(char)*(i+1)); 
        read(fd_from_par,&inp,1);
        b_count[i] = inp;
        i++;
    }
    b_count[i] = 0; 
    
    long long a_ = back_transform(a_count);
    long long b_ = back_transform(b_count);
    free(a_count);
    free(b_count);
    //завершили чтение

    printf("[%d] %lld atts to generate %lld rounds | start values are %lld %lld\n",getpid(),count_of_tries,count_of_rounds,a_,b_);
    write(fd_to_par,"1",1);

    srand(time(NULL)*getpid());
    long long first_wins = 0; 
    for(long long j = 0; j<count_of_tries;++j)
    {
        long long a = a_, b=b_;
        //printf("in loop, %lld\n",j);
        for(long long i=0;i<count_of_rounds;++i)
        {
            a += rand()%6 + 1 + rand()%6 + 1;
            b += rand()%6 + 1 + rand()%6 + 1;
        }
        
        //printf("%lld | %lld\n",a,b);

        if(a>b)
        first_wins += 1;
    }
    
    write(fd_to_par,transform(first_wins),strlen(transform(first_wins))+1);

    close(fd_from_par);
    close(fd_to_par);

    return 0;
}