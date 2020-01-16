#include "cashRegister.h"
#include "../params.h"
#include "../keys.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


int accessSemID, notAllSemID;
Money *storedMoney;


void getAccessToCashRegister()
{
    int shmID;

    if ((shmID = shmget(CASH_REGISTER_SHM_KEY, sizeof(Money), 
    IPC_CREAT|0600)) == -1) {
        perror("Dotep do kasy");
        exit(1);
    }
    if ((storedMoney = (Money*)shmat(shmID, 
    NULL, 0)) == NULL) {
        perror("Przylaczenie kasy");
        exit(1);
    }

    if ((accessSemID = semget(CASH_REGISTER_ACCESS_SEM_KEY, 1,
    IPC_CREAT|0600)) == -1) {
        perror("Dostep do semafora dostepu do kasy");
        exit(1);
    }
    if ((notAllSemID = semget(CASH_REGISTER_NOT_ALL_SEM_KEY, 1, 
    IPC_CREAT|0600)) == -1) {
        perror("Dostep do semafora nie wszystkich przy kasie");
        exit(1);
    }
}


void initCashRegister()
{
    getAccessToCashRegister();

    resetMoney(storedMoney);

    if (semctl(accessSemID, 0, SETVAL, 1) == -1) {
        perror("Przypianie wartosci semaforowi dostepu do kasy");
        exit(1);
    }
    if (semctl(notAllSemID, 0, SETVAL, BARBER_COUNT - 1) == -1) {
        perror("Przypisanie wartosic semaforowi"\
        " nie wszystkich przy kasie");
        exit(1);
    }
}


void clearCashRegister()
{
    shmdt(storedMoney);
}