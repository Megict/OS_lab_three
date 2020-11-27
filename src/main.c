// Вариант 7. Задание:
// Два человека играют в кости. Правила игры следующие: каждый игрок делает бросок 2-ух костей K раз;
// побеждает тот, кто выбросил суммарно большее количество очков.  Задача программы экспериментально 
// определить шансы на победу каждого из игроков. На вход программе подается K, какой сейчас тур,
// сколько очков суммарно у каждого из игроков и количество экспериментов, которые должна произвести
// программа

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
    //printf("argc-> %d\n",argc);
    if(argc != 2)
    {
        printf("wrong args count\n");
        return -10; //неверное кол-во аргументов
    }

    if(strlen(argv[1])<2)
    {
        printf("wrong key structure\n");
        return -11; //невернаяя стурктура ключа
    }

    if(argv[1][0] != '-')
    {
        printf("wrong key structure\n");
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
        execl("child_any.out",transform(fdCP[localid][IN]),transform(fdPC[localid][OUT]),NULL); //запуск программы ребенка
    }
    else //код родителя 
    {
        char mark='0';
        for(i=0;i<num;++i)//в этом цикле происходит чтение из pipe ов связи с детьми, т.е. родительский процесс паузится, пока все дети не пошлют сообщение
        {
            read(fdCP[i][OUT],&mark,1);
        }//отчет процессов из этой проги, перед разделением
        printf("[%d] parent process operating, childs:\n",getpid());
        for(i=0;i<num;++i)
        {
            printf("[%d] child: [%d] lid: [%d]\n",getpid(),id[i],i); //lid (local id) это номер пары pipe-ов, через которые производится связь с процессами
        }

        for(i=0;i<num;++i)//отправка детям сообщений о том что родитель закончил вывод информации и можно написать ченто самим
        {
            write(fdPC[i][IN],"2",1);
        }

        for(i=0;i<num;++i)//в этом цикле происходит чтение из pipe ов связи с детьми, т.е. родительский процесс паузится, пока все дети не пошлют сообщение
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
        printf("input number of rounds\n|-> ");
        scanf("%lld",&K);
        printf("input current round\n|-> ");
        scanf("%lld",&C);
        printf("input current score\n|-> ");
        scanf("%lld %lld",&a,&b);
        printf("input number of tries\n|-> ");
        scanf("%lld",&N);

        long long count_KmC = K-C; //сколько раундов надо смоделировать
        
        long long count_a = N/num + 1; //сколько раз провести эксперимент каждому ребенку
        long long count_b = N%num; //сколько детей должны провести на один эксперимент больше чем остальные
        //long long count_c = count_b;

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
            write(fdPC[i][IN],transform(count_a),strlen(transform(count_a))+1); //пересылаем число ээкспериментов, которые должен промоделировать процесс
            write(fdPC[i][IN],transform(count_KmC),strlen(transform(count_KmC))+1); //пересылаем число попыток в каждом эксперименте
            write(fdPC[i][IN],transform(a),strlen(transform(a))+1); //пересылаем счет первого игрока
            write(fdPC[i][IN],transform(b),strlen(transform(b))+1); //пересылаем счет второго игрока 
        }

        int count_recive = count_a+1; 
        char getter;
        int ittl = 0;

        for(i=0;i<num;++i)//в этом цикле происходит чтение из pipe ов связи с детьми, т.е. родительский процесс паузится, пока все дети не пошлют сообщение
        {
            read(fdCP[i][OUT],&mark,1);
        }
        long long first_wins_total = 0;
        printf("--------------------\n");
        for(i = 0;i<num;++i) //считывем результат работы ребенка. Он отправляет количество экспериментов в которых победил первый игрок
        {
            long long j=0;
            char inp = -1;
            char* wins_in_str = NULL;// = (char*)malloc(sizeof(char)*1);
            while(inp != 0)
            {
                wins_in_str = (char*)realloc(wins_in_str,sizeof(char)*(j+1)); 
                read(fdCP[i][OUT],&inp,1);
                wins_in_str[j] = inp;
                j++;
            }
            long long wins_normal = back_transform(wins_in_str);
            free(wins_in_str);
            first_wins_total +=  wins_normal;
            printf("got %s|%lld wins\n--------------------\n",wins_in_str,wins_normal);
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