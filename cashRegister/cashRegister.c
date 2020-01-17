#include "cashRegister.h"
#include "../params.h"
#include "../keys.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>


int accessSemID, notAllSemID;
Money *storedMoney;
int *changeCounter;


void lockCashRegister();
void unlockCashRegister();
bool fillChange(Money*, int);


void getAccessToCashRegister()
{
    int cashRegisterShmID, changeCounterSemID;

    if ((cashRegisterShmID = shmget(CASH_REGISTER_SHM_KEY, sizeof(Money), 
    IPC_CREAT|0600)) == -1) {
        perror("Dotep do kasy");
        exit(1);
    }
    if ((storedMoney = (Money*)shmat(cashRegisterShmID, 
    NULL, 0)) == NULL) {
        perror("Przylaczenie kasy");
        exit(1);
    }
	
	if ((changeCounterSemID = shmget(CHANGE_COUNTER_SHM_KEY, sizeof(int),
	IPC_CREAT|0600)) == -1) {
		perror("Dostep do licznika wydanej reszty");
		exit(1);
	}
	if ((changeCounter = (int*)shmat(changeCounterSemID,
	NULL, 0)) == NULL) {
		perror("Przylaczenie licznika wydanej reszty");
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
	changeCounter[0] = 0;

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


void lockCashRegister()
{
	struct sembuf buf;
	
	buf.sem_num = 0;
	buf.sem_op = -1;
	buf.sem_flg = 0;
	
	if (semop(accessSemID, &buf, 1) == -1) {
        perror("Opuszczenie semafora kasy");
        exit(1);
    }
}


void unlockCashRegister()
{
	struct sembuf buf;
	
	buf.sem_num = 0;
	buf.sem_op = 1;
	buf.sem_flg = 0;
	
	if (semop(accessSemID, &buf, 1) == -1) {
        perror("Podniesienie semafora kasy");
        exit(1);
    }
}


void putMoneyToCashRegister(const Money *money)
{
	lockCashRegister(accessSemID);
	copyMoney(storedMoney, money);
	unlockCashRegister(accessSemID);
}


bool fillChange(Money *money, int sum)
{
	int count1, count2, count5;
	
	count5 = sum / 5;
	if (count5 > storedMoney->count5)
		count5 = storedMoney->count5;
	sum -= count5;
	
	count2 = sum / 2;
	if (count2 > storedMoney->count2)
		count2 = storedMoney->count2;
	sum -= count2;
	
	count1 = sum / 5;
	if (count1 > storedMoney->count1)
		count1 = storedMoney->count1;
	sum -= count1;
	
	if (sum == 0) {
		initMoney(money, count1, count2, count5);
		subtractMoney(storedMoney, money);
		return true;
	}
	return false;
}


void getChangeFromCashRegister(Money *money)
{
	struct sembuf buf;
	Money change;
	
	while (1) {
		lockCashRegister(accessSemID);
		
		buf.sem_num = 0;
		buf.sem_op = 1;
		buf.sem_flg = IPC_NOWAIT;
		
		if (semop(notAllSemID, &buf, 1) == -1) {
			if (errno == EAGAIN) {
				if (fillChange(&change, money->sum - SERVICE_PRICE)) {
					copyMoney(money, &change);
					break;
				} else exit(2);
			} else {
				perror("Opuszczenie semafora notAll");
				exit(1);
			}
		} else {
			if (fillChange(&change, money->sum - SERVICE_PRICE)) {
				copyMoney(money, &change);
				
				buf.sem_op = -1;
				buf.sem_flg = 0;
				
				if (semop(notAllSemID, &buf, 1) == -1) {
					perror("Podniesienie semafora notAll");
					exit(1);
				}
				break;
			} else unlockCashRegister();
		}
	}
	
	if (++(*changeCounter) == CLIENTS_TO_BE_SERVED)
		exit(0);
	
	unlockCashRegister();
}