/*
Harris Kwong
CIS 3207 Lab section 04
Project 3
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h> 



sem_t semEmpty;
sem_t semFull;
sem_t semEmpty2;
sem_t semFull2;

struct timespec ts;

pthread_mutex_t mutexBuffer; 
int buffer[10]; //buffer for product 1
int buffer2[20]; //buffer for product 2 
int count =0;
int count2 =0;
FILE *fp;
FILE *fh;

//Poducer function: Produce values and increment counter, int num is to indicate which product type
int producer(int num){
      int x = rand()% 90 + 10; //product type 1 only produces double digit numbers
      int y = rand() % 10; //product type 2 only produces single digit numbers
        if(num == 1){
          count++;
          usleep(20000); //sleep  after producing
          return x;  
        }
        else{
          count2++;
          usleep(20000); 
          return y;
        }   
}

//Distributor thread: add value to buffer of product type 1 or 2.
void *distributor(void * args){

  int x;
  while(1){
      x = rand() % 2 + 1; //decide if type 1 or 2 
      if(x == 1){ //product type 1
       //add to the buffer 1
        sem_wait(&semEmpty); //wait here if there are no slots avalibale, if this passes then we have slots open
        pthread_mutex_lock(&mutexBuffer); //critical section
        buffer[count-1] = producer(1); 
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull); //increment with this because we added an element to a slot
      }
      else{ //product type 2 
       //add to the buffer 2
        sem_wait(&semEmpty2); 
        pthread_mutex_lock(&mutexBuffer);
        buffer2[count2-1] = producer(2);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull2);       
      }
  }

}

//Consumer thread (type 1): Consumes product value from buffer and print data to file
void *consumer(void *args){
    
  while(1){
        
        //remove from the buffer
        sem_wait(&semFull); //wait util there is at least one in the buffer
        pthread_mutex_lock(&mutexBuffer);
        int y = buffer[count -1];
        count--;
        fprintf(fp,"Product type:1 ");
        fprintf(fp,"Value:%d ",y);
        fprintf(fp,"Thread id:%ld ",pthread_self());
        fprintf(fp,"Count:%d\n",count);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty); //decrement becasue we consumed an element from slop
  }
}

void *consumer2(void *args){
   
  while(1){
        //remove from the buffer
        sem_wait(&semFull2); //wait util there is at least one in the buffer
        pthread_mutex_lock(&mutexBuffer);     
        int y = buffer2[count2 -1];
        count2--;
        fprintf(fh,"Product type:2 ");
        fprintf(fh,"Value:%d ",y);
        fprintf(fh,"Thread id:%ld ", pthread_self());
        fprintf(fh,"Count:%d\n",count2);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty2); //decrement becasue we consumed an element from slot
  }
}


int main()
{

    srand(clock_gettime(CLOCK_REALTIME, &ts)); //seed random numbers for the producer function
    pthread_mutex_init(&mutexBuffer,NULL); //initalize mutex
    sem_init(&semEmpty,0,10);   //initalize semaphore semEmpty, 10 = number of slots avavlible in the beinging
    sem_init(&semFull,0,0); //initailize semaphore semFull, 0 = number of slots taken up in the beinging
    sem_init(&semEmpty2,0,20);   
    sem_init(&semFull2,0,0); 
 
      fp = fopen("consumer1.txt","w"); //log data into files
      fh = fopen("consumer2.txt","w");

    if(fp == NULL || fh == NULL){
      printf("Error in opening file.\n");
      exit(1);
    }

    pthread_t th[5]; //threads
  

      for(int i =0; i < 5; i++){ //create 5 threads
        if(i == 0){  //distributor thread
            if(pthread_create(&th[i],NULL,&distributor,NULL) != 0){
                printf("Error in creating thread.\n");
                exit(1);
            }
        }
        else if (i == 1 || i == 2){ // consuumer thread for product type 1
            if(pthread_create(&th[i],NULL,&consumer,NULL) != 0){
                printf("Error in creating thread.\n");
                exit(1);
            }
        }
        else{ // consuumer thread for product type 2 
            if(pthread_create(&th[i],NULL,&consumer2,NULL) != 0){
                printf("Error in creating thread.\n");
                exit(1);
            }          
        }
      } 
      for(int i =0 ; i < 5; i++){ //join all threads
        if(pthread_join(th[i],NULL) != 0){
            printf("Error in joining threads.\n");
            exit(1);
        }
      }
    
  
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    sem_destroy(&semEmpty2);
    sem_destroy(&semFull2);  
    fclose(fp);
    fclose(fh);
    pthread_mutex_destroy(&mutexBuffer);
    return 0;
}