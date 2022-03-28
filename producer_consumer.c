//Group:    6388093 Nitit       Ngamphotchanamongkol
//          6388117 Siranut     Akarawuthi
//          6388145 Prapakorn   Saelim

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "common.h"
#include "common_threads.h"

#define BufferSize 5 // Size of the buffer
int MaxItems = 5 ;  // Maximum items per producer can produce
int PRODUCER_NUM = 5; // Process Number
int CONSUMER_NUM = 3; // Devices Number
int max_time = 500; //Maximum Bursttime
int min_time = 200; //Minimun Bursttime
int action = 1; //Action

sem_t empty;
sem_t full;
int in = 0; //Producer Buffer runner
int out = 0; //Consumer Buffer runner
int buffer[BufferSize]; //Buffer Array
int j=0; //Running process number
int turnaroundtime = 0; //for Counting overall time
int waitingtime = 0; //for Counting wait time
int status = 0; //for Counting the Buffer Status
int BUFFER_STATUS = 0; //for Counting the Buffer Status
int length = 0; //Maximun Process
int MAIN_NUM = 0; //Checking Consumer or Producer is Larger
double count_print = 0; //Sucessful Print Counter
double drop_request = 0; //Counting Drop Request
int buffer_replace = 0; 
int temp =0;
int fullaction=0;

pthread_mutex_t mutex;

void *producer(void *pno)
{
    int item;
    if (action == 1){
        /* Wait */
        for (int i = 0; i < MaxItems; i++)
        {

            int random = (rand() % (max_time - min_time + 1)) + min_time; //Randoming Wait time
            turnaroundtime += random; //Added time in to Turnaroundtime
            waitingtime += random; //Added time in to Waittime
            usleep(random*1000); //Wait

            status++; 
            item = j; //Declare process number
            j++; //Adding process number to next number
    
            sem_wait(&empty);  //wait/sleep when there are no empty slots
            pthread_mutex_lock(&mutex);

            buffer[in] = item; //Added process in to Buffer Array

            /*
            Buffer Print Checking
            for (int k=0;k<BufferSize;k++){
                printf("%d ",buffer[k]);
            }
            */

            printf("Producer %d: Insert Item p%d at %d waittime %d ms\n", *((int *)pno), buffer[in], in, random);
            in = (in + 1) % BufferSize; //Point to next Buffer
            BUFFER_STATUS++; //Count that Buffer has added up
            
            //printf("status : %d\n",BUFFER_STATUS);

            pthread_mutex_unlock(&mutex);
            sem_post(&full); //Signal/wake to consumer

        }
    }else if (action == 2){
        //*Drop
        for (int i = 0; i < MaxItems; i++)
        {

            item = j; // Produce an random item
            j++; //Adding process number to next number

            if(BUFFER_STATUS >= BufferSize){ // check buffer is full or not
                drop_request++; //increase drop request
                printf("Drop Request Item p%d\n",item);

            }else{

                int random = (rand() % (max_time - min_time + 1)) + min_time;//Randoming Wait time
                turnaroundtime += random;//Added time in to Turnaroundtime
                waitingtime += random;//Added time in to Waittime
                usleep(random*1000);//Wait

                status++;

                sem_wait(&empty); //wait/sleep when there are no empty slots
                pthread_mutex_lock(&mutex);

                buffer[in] = item; //Added process in to Buffer Array
                printf("Producer %d: Insert Item p%d at %d waittime %d ms\n", *((int *)pno), buffer[in], in, random);
                in = (in + 1) % BufferSize; //Point to next Buffer
                BUFFER_STATUS++; //Count that Buffer has added up
                //printf("status : %d\n",BUFFER_STATUS);

                pthread_mutex_unlock(&mutex);
                sem_post(&full); //Signal/wake to consumer

            } 
        }

    }else if (action == 3){
        //*Replace
        for (int i = 0; i < MaxItems; i++)
        {
            item = j; // Produce an random item
            j++;

            if(BUFFER_STATUS >= BufferSize){ // check buffer is full or not
                
                drop_request++; //increase drop request

                int checkmin = buffer[0]; // declare min item in buffer
                int buffIndex = 0; //position of min number

                for(int h = 1; h < BufferSize; h++){ //find min number and position in buffer
                    if(buffer[h] < checkmin){
                        checkmin = buffer[h];
                        buffIndex = h;
                    }
                }

                // for(int k = 0; k < BufferSize; k++){
                //     printf("%d ",buffer[k]);
                // }
                
                int boom  = buffer[buffIndex]; //get min number

                printf("\n");
                buffer[buffIndex] = item;
                printf("Producer %d: Replace Item p%d with p%d at %d \n",*((int *)pno),boom,item,in);

            }else{

                int random = (rand() % (max_time - min_time + 1)) + min_time; //Randoming Wait time
                //printf("random: %d\n", random);
                turnaroundtime += random;//Added time in to Turnaroundtime
                waitingtime += random;//Added time in to Waittime
                usleep(random*1000);//Wait

                status++;

                sem_wait(&empty);//wait/sleep when there are no empty slots
                pthread_mutex_lock(&mutex);

                buffer[in] = item;//Added process in to Buffer Array
                printf("Producer %d: Insert Item p%d at %d waittime %d ms\n", *((int *)pno), buffer[in], in, random);
                in = (in + 1) % BufferSize;
                BUFFER_STATUS++;
                //printf("status : %d\n",BUFFER_STATUS);

                pthread_mutex_unlock(&mutex);
                sem_post(&full);

            } 
        }
    }
    return NULL;
}

void *consumer(void *cno)
{
    while(1)
    {

        status--; 
        int random2 = (rand() % (max_time - min_time + 1)) + min_time; //Randoming Burst time
        //printf("Random: %d\n", random2);
        turnaroundtime += random2; //Added time in to Turnaroundtime

        sem_wait(&full); //wait/sleep when there are no full slots
        pthread_mutex_lock(&mutex);

        BUFFER_STATUS--; //Decrease Buffer count
        
        
        // if(action ==3 && fullaction==1){
        //     out = (out + 1) % BufferSize; //Point to next out Buffer
        //     int items = buffer[out]; //Print out the item in the buffer
        //     printf("Consumer %d: Remove Item p%d from %d bursttime %d \n", *((int *)cno), items, out, random2);
        //     out = (out + 1) % BufferSize; //Point to next out Buffer
        // }else{
        //     int items = buffer[out]; //Print out the item in the buffer
        //     printf("Consumer %d: Remove Item p%d from %d bursttime %d ms\n", *((int *)cno), items, out, random2);
        //     out = (out + 1) % BufferSize; //Point to next out Buffer
        // }

        int item = buffer[out]; //Print out the item in the buffer
        printf("Consumer %d: Remove Item p%d from %d bursttime %d \n", *((int *)cno), item, out, random2);
        out = (out + 1) % BufferSize; //Point to next out Buffer

        //printf("status : %d\n",BUFFER_STATUS);

        pthread_mutex_unlock(&mutex); 
        sem_post(&empty); //Signal/wake the producer
        usleep(random2 * 1000); //wait
        
        count_print++; //Increase Sucessful Print Counter
        if(status <= 0 && out >= length ) break;

    }
    return NULL;
}

int main()
{
    printf("Enter processes number : ");
    scanf("%d",&PRODUCER_NUM);
    printf("Enter devices number : ");
    scanf("%d",&CONSUMER_NUM);
    printf("Enter total requests : ");
    scanf("%d",&MaxItems);
    printf("Enter Minimum time to process : ");
    scanf("%d",&min_time);
    printf("Enter Maximum time to process : ");
    scanf("%d",&max_time);
    printf("Enter The Action (1=Wait,2=Drop,3=Replace) :");
    scanf("%d",&action);

    if(action != 1 && action != 2 && action != 3){
        printf("Fuck off\n");
        exit(0);
    }

    pthread_t pro[PRODUCER_NUM], con[CONSUMER_NUM];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BufferSize);
    sem_init(&full, 0, 0);

    MAIN_NUM = CONSUMER_NUM;
    if (PRODUCER_NUM > CONSUMER_NUM) MAIN_NUM = PRODUCER_NUM;
    int a[MAIN_NUM];
    for (int i=0 ; i<MAIN_NUM ; i++){
        a[i] = i;
    }
    for (int i=0;i<BufferSize;i++){
        buffer[i] = -1;
    }

    // int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9 ,10}; // Just used for numbering the producer and consumer
    /*for (int i=0; i< PRODUCER_NUM*MaxItems ; i++){
        process[i] = i;
    }*/

    for (int i = 0; i < PRODUCER_NUM; i++)
    {
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    }
    for (int i = 0; i < CONSUMER_NUM; i++)
    {
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    }

    for (int i = 0; i < PRODUCER_NUM; i++)
    {
        pthread_join(pro[i], NULL);
    }
    for (int i = 0; i < CONSUMER_NUM; i++)
    {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("==== END ===\n");
    printf("Total Print = %d\n",(int)count_print);
    printf("Drop Request = %d\n",(int)drop_request);
    printf("Average waiting time = %d ms\n",waitingtime/(PRODUCER_NUM*MaxItems));
    printf("Total time of the simulation time = %d second \n",turnaroundtime/1000);
    printf("Percentage of dropped requests = %.2lf %%\n",(double)(drop_request/(count_print+drop_request))*100.00);
    
    return 0;
}