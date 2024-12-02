/*
*   FIFO
*/
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000
#define STACKSIZE 1024
#define PRIORITY 0
#define limiteFIFO 1000

K_FIFO_DEFINE(my_fifo);
int elementosFIFO = 0;

struct data_item_t {
    void *fifo_reserved;   /* 1st word reserved for use by FIFO */
    char value[250];
};

struct data_item_t tx1_data;
struct data_item_t tx2_data;

void Escreva1(void){
    while(1){
        if(elementosFIFO >= limiteFIFO){
            while(elementosFIFO > (limiteFIFO/2)){
                //FIFO cheia
                k_msleep(SLEEP_TIME_MS);
            }
        } else{
            strcpy(tx1_data.value, "oioi!"); //manda info para tx1_data
            k_fifo_put(&my_fifo, &tx1_data); //envia info p/fifo
            elementosFIFO++;
        }
        k_msleep(SLEEP_TIME_MS);
    }
}

void Escreva2(void){
    while(1){
        //confere se está cheia
        //se estiver, espera esvaziar pela metade para depois enviar novamente
        if(elementosFIFO >= limiteFIFO){
            while(elementosFIFO > (limiteFIFO/2)){
                //FIFO cheia
                k_msleep(SLEEP_TIME_MS);
            }
        } else{
            strcpy(tx2_data.value, "sou eu!"); //manda info para tx2_data
            k_fifo_put(&my_fifo, &tx2_data); //envia info p/fifo
            elementosFIFO++;
        }
        k_msleep(SLEEP_TIME_MS);
    }
}

void Leia1(void){
    struct data_item_t  *rx1_data;

    while(1){
        rx1_data = k_fifo_get(&my_fifo, K_FOREVER);

        if(rx1_data == NULL){ //ou se elementosFIFO == 0;
            printk("\n Lista vazia.\n");
        } else{
            printf("\n RX1: %s \n", rx1_data->value);
            elementosFIFO--;
        }
        printf("\nItens na fila: %d\n", elementosFIFO);

        k_msleep(SLEEP_TIME_MS);
    }
}

void Leia2(void){
    struct data_item_t  *rx2_data;

    while(1){
        rx2_data = k_fifo_get(&my_fifo, K_FOREVER);

        if(rx2_data == NULL){ //ou se elementosFIFO == 0;
            printk("\n Lista vazia.\n");
        } else{
            printf("\n RX2: %s \n", rx2_data->value);
            elementosFIFO--;
        }
        printf("\nItens na fila: %d\n", elementosFIFO);

        k_msleep(SLEEP_TIME_MS);
    }
}

//define threads
K_THREAD_DEFINE(tx1_id, STACKSIZE, Escreva1, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(tx2_id, STACKSIZE, Escreva2, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(rx1_id, STACKSIZE, Leia1, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(rx2_id, STACKSIZE, Leia2, NULL, NULL, NULL, PRIORITY, 0, 0);