#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "tramsformres.h"
#define IN 1
#define OUT 0
#define TESTER_LIMIT 1009

//система разделения процессов
//генерирует процесс родителя и n детей
//программы родителей и детей хранятнся отдельно

int main(int argc,char** argv)
{
    srand(time(NULL));
    //получаем значение ключа
    printf("argc-> %d\n",argc);
    if(argc != 2)
    {
        return -10; //неверное кол-во аргументов
    }

    if(strlen(argv[1])<2)
    {
        return -11; //невернаяя стурктура ключа
    }

    if(argv[1][0] != '-')
    {
        return -11; //неверная структура ключа
    }

    int i=strlen(argv[1]) - 1, inv = 1;
    int num = 0;
    while(i>0)
    {
        if(argv[1][i] < '0' || argv[1][i] > '9')
        {
            return 0; //в ключе не число
        }
//        printf("|%c|\n",argv[1][i]);
        num += inv*(argv[1][i]-'0'); //сборка числового ключа
        inv*=10;
        --i;
    }

    if(num<=0 || num>=256)
    {
        printf("incorrect proc num\n");
        return -18;
    }

    printf("procon-> %d\n",num);
    //значение ключа собрано
    //создаем pipe-ы
    
    int fdPC[num][2];
    int fdCP[num][2];

    for(i=0;i<num;++i)
    {
        pipe(fdPC[i]);
        pipe(fdCP[i]);
    }

    //начинаем разделение. идея в том чтобы в итоге мы имели один родительский процесс и num ЕГО детей
    int id[num]; //это будет массив с id детей
    for(i=0;i<num;++i)
    {
        id[i] = 0;
    }
    //!invalid! первый элемент уже проинициализирован чтобы можно было потом одним циклом записать все в этот массив
    printf("<-| initiating process separtion\n");
    for(i=0;i<num;++i)
    {
        if(i!= 0 && id[i-1] == 0)
        {
            break;
        }
        id[i] = fork();
    }

    if(id[i-1] == 0) //вывод инфы по процессам, вывод синхронизирован
    {
        int localid = i-1; //обозначает, на каком шаге (при отсчете от нуля) разделения отделился этот процесс, используется при общении через pipe
        printf("[%d] child process started. local ID = %d\n",getpid(),localid); 
        write(fdCP[localid][IN],"1",1);

        //место для запуска программы ребенка
        //printf("[%d] sending: %d <-SEND|GET-> %d\n",getpid(),fdCP[localid][IN],fdPC[localid][OUT]);
        execl("child_any.out",transform(fdCP[localid][IN]),transform(fdPC[localid][OUT]),NULL);
        //printf("[%d] process detached\n",getpid());
    }
    else //код родителя (может вынесу в отдельный файл еще)
    {
        char mark='0';
        for(i=0;i<num;++i)//в этом цикле происходит чтение из pipe ов связи с детьми, т.е. родительский процесс паузится, пока все дети не пошлют сигнал
        {
            read(fdCP[i][OUT],&mark,1);
        }//отчет процессов из этой проги, перед разделением
        printf("[%d] parent process operating, childs:\n",getpid());
        for(i=0;i<num;++i)
        {
            if(id[i]==0)
            {
                break;
            }
            printf("[%d] child: [%d] lid: [%d]\n",getpid(),id[i],i);
        }

        for(i=0;i<num;++i)
        {
            write(fdPC[i][IN],"2",1);
        }

        for(i=0;i<num;++i)//в этом цикле происходит чтение из pipe ов связи с детьми, т.е. родительский процесс паузится, пока все дети не пошлют сигнал
        {
            read(fdCP[i][OUT],&mark,1);
        }//отчет процессов из их отдельных программ, после разделения

        //блок проверки вспомогательных функций
        //особо не нужен, но при работе выглядит красиво :)
        printf("<-| testing auxiliary functions| ");
        int slider = 0;
        while(slider<=TESTER_LIMIT)
        {
            if(back_transform(transform(slider))!=slider)
            {
                printf("incorrect work detected\n");
                return(-101);
            }
            ++slider;
        }
        printf("functions capability confirmed\n");
        //завершение проверки
            
        long long K;
        long long a,b; 
        long long C;
        long long N; 
        printf("input number of rouwnds\n|-> ");
        scanf("%lld",&K);
        printf("input current round\n|-> ");
        scanf("%lld",&C);
        printf("input current score\n|-> ");
        scanf("%lld %lld",&a,&b);
        printf("input number of tries\n|-> ");
        scanf("%lld",&N);

        int count_KmC = K-C;
        
        long long count_a = N/num + 1;
        long long count_b = N%num; 
        long long count_c = count_b;

    //    printf("a-> %d|%s|%d\n",count_a,transform(count_a),back_transform(transform(count_a)));
    //    printf("b-> %d|%s|%d\n",count_b,transform(count_b),back_transform(transform(count_b)));

        for(i = 0;i<num;++i)
        {
            if(count_b == 0) //распределение остатка по потокам (по 1 вычислению каждому, пока доп. вычислений не останется)
            {
                count_a-=1;
                count_b--;
            }
            else 
            {
                if(count_b > 0)
                count_b--;
            }
            printf("sent-> %lld\n",count_a);
            write(fdPC[i][IN],transform(count_a),strlen(transform(count_a))+1); //пересылаем число попрыток, которые должен промоделировать процесс
            write(fdPC[i][IN],transform(count_KmC),strlen(transform(count_KmC))+1); //пересылаем число попрыток, которые должен промоделировать процесс   
            write(fdPC[i][IN],transform(a),strlen(transform(a))+1); //пересылаем число попрыток, которые должен промоделировать процесс
            write(fdPC[i][IN],transform(b),strlen(transform(b))+1); //пересылаем число попрыток, которые должен промоделировать процесс   
        }

        int count_recive = count_a+1; 
        char getter;
        int ittl = 0;

        for(i=0;i<num;++i)//в этом цикле происходит чтение из pipe ов связи с детьми, т.е. родительский процесс паузится, пока все дети не пошлют сигнал
        {
            read(fdCP[i][OUT],&mark,1);
        }
        long long first_wins_total = 0;
        printf("--------------------\n");
        for(i = 0;i<num;++i)
        {
            long long j=0;
            char inp = -1;
            char* wins_in_str = (char*)malloc(sizeof(char)*1);
            while(inp != 0)
            {
 //               printf("from [%d] reading... \n",i);
                wins_in_str = (char*)realloc(wins_in_str,sizeof(char)*(j+1)); 
                read(fdCP[i][OUT],&inp,1);
                wins_in_str[j] = inp;
 //               printf("read -> %c|%d \n",inp,inp);
                j++;
            }
            //wins_in_str[j] = 0;
            long long wins_normal = back_transform(wins_in_str);
            free(wins_in_str);
            first_wins_total +=  wins_normal;
            printf("\n--------------------\ngot %s|%lld wins\n--------------------\n",wins_in_str,wins_normal);
        }
        printf("total 1st wins: %lld of %lld games\n",first_wins_total,N); 
        double chance = (double)first_wins_total/(double)N;
        printf("first wins chance: %lf\n",chance);

        for(i=0;i<num;++i)//закрытие всех pipe-ов
        {
            close(fdPC[i][OUT]);
            close(fdCP[i][IN]);
        }
    }

    return 0;
}