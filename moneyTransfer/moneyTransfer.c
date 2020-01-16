#include "moneyTransfer.h"
#include "../params.h"
#include "../keys.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>


int shmID, barberSemID, clientSemID;   
Money *transaction;


void lock(int, int);
void unlock(int, int);


void getAccessToMoneyTransfer()
{
    if ((shmID = shmget(TRANSFER_SHM_KEY, 
    sizeof(Money) * CLIENT_COUNT,
    IPC_CREAT|0600)) == -1) {
        perror("Uzyskanie transferu (shmget)");
        exit(1);
    }
    if ((transaction = (Money*)shmat(
    shmID, NULL, 0)) == NULL) {
        perror("Uzyskanie transferu (shmat)");
        exit(1);
    }

    if ((barberSemID = semget(TRANSFER_BARBER_SEM_KEY, 
    CLIENT_COUNT, IPC_CREAT|0600)) == -1) {
        perror("Uzyskanie transferu (barber semget)");
        exit(1);
    }
    if ((clientSemID = semget(TRANSFER_CLIENT_SEM_KEY, 
    CLIENT_COUNT, IPC_CREAT|0600)) == -1) {
        perror("Uzyskanie transferu (client semget)");
        exit(1);
    }
}


void initMoneyTransfer()
{
    getAccessToMoneyTransfer();
    for (int i = 0; i < CLIENT_COUNT; ++i) {
        if (semctl(barberSemID, i, SETVAL, 0) == -1) {
            perror("Semafor tranferu fryzjera");
            exit(1);
        }
        if (semctl(clientSemID, i, SETVAL, 1) == -1) {
            perror("Semafor transferu klienta");
            exit(1);
        }
    }
}


void clearMoneyTransfer()
{
    shmdt(transaction);
}


void lock(int semid, int semnum)
{
    struct sembuf buffer;

    buffer.sem_num = semnum;
    buffer.sem_op = -1;
    buffer.sem_flg = 0;

    if (semop(semid, &buffer, 1) == -1) {
        perror("Opuszczenie semafora transferu");
        exit(1);
    }
}


void unlock(int semid, int semnum)
{
    struct sembuf buffer;

    buffer.sem_num = semnum;
    buffer.sem_op = 1;
    buffer.sem_flg = 0;

    if (semop(semid, &buffer, 1) == -1) {
        perror("Podniesienie semafora transferu");
        exit(1);
    }
}


void getMoneyForService(size_t id, Money *money)
{
    lock(barberSemID, id);
    copyMoney(money, transaction + id);
}

void giveChange(size_t id, const Money *money)
{
    lock(barberSemID, id);
    copyMoney(transaction, money);
    unlock(clientSemID, id);
}


void payForService(size_t id, Money *money)
{
    lock(clientSemID, id);
    copyMoney(transaction + id, money);
    resetMoney(money);
    unlock(barberSemID, id);
}


void takeChange(size_t id, Money *money)
{
    lock(clientSemID, id);
    addMoney(money, transaction + id);
    unlock(barberSemID, id);
}