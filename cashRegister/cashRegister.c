#include "cashRegister.h"
#include "../params.h"
#include "../keys.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>


int cashRegisterSemID;
Money *storedMoney;
int *changeCounter;
bool *tried;


bool allTried();
bool fillChange(Money*, int);


void getAccessToCashRegister()
{
    int cashRegisterShmID, changeCounterShmID, triedShmID;

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
	
	if ((changeCounterShmID = shmget(CHANGE_COUNTER_SHM_KEY, sizeof(int),
	IPC_CREAT|0600)) == -1) {
		perror("Dostep do licznika wydanej reszty");
		exit(1);
	}
	if ((changeCounter = (int*)shmat(changeCounterShmID,
	NULL, 0)) == NULL) {
		perror("Przylaczenie licznika wydanej reszty");
		exit(1);
	}
	
	if ((triedShmID = shmget(TRIED_SHM_KEY, sizeof(bool) * BARBER_COUNT,
	IPC_CREAT|0600)) == -1) {
		perror("Dostep do prob wydania reszty");
		exit(1);
	}
	if ((tried = (bool*)shmat(triedShmID,
	NULL, 0)) == NULL) {
		perror("Przylaczenie prob wydania reszty");
		exit(1);
	}

    if ((cashRegisterSemID = semget(CASH_REGISTER_SEM_KEY, 1,
    IPC_CREAT|0600)) == -1) {
        perror("Dostep do semafora kasy");
        exit(1);
    }
}

void initCashRegister()
{
    getAccessToCashRegister();

    resetMoney(storedMoney);
	changeCounter[0] = 0;
	for (size_t i = 0; i < BARBER_COUNT; ++i)
		tried[i] = false;

    if (semctl(cashRegisterSemID, 0, SETVAL, 1) == -1) {
        perror("Przypianie wartosci semaforowi kasy");
        exit(1);
    }
}


void clearCashRegister()
{
    shmdt(storedMoney);
}


void printCashRegister()
{
	printMoney(storedMoney);
}


void putMoneyToCashRegister(const Money *money)
{
	struct sembuf buf = {0, -1, 0};
	if (semop(cashRegisterSemID, &buf, 1) == -1) {
        perror("Opuszczenie semafora kasy przy wplacie");
        exit(1);
    }
	
	addMoney(storedMoney, money);
	for (size_t i = 0; i < BARBER_COUNT; ++i)
		tried[i] = false;
	
	buf.sem_op = 1;
	if (semop(cashRegisterSemID, &buf, 1) == -1) {
        perror("Podniesienie semafora kasy przy wplacie");
        exit(1);
    }
}


bool allTried()
{
	for (size_t i = 0; i < BARBER_COUNT; ++i)
		if (!tried[i]) return false;
	return true;
}


bool fillChange(Money *money, int sum)
{
	int count1, count2, count5;
	
	count5 = sum / 5;
	if (count5 > storedMoney->count5)
		count5 = storedMoney->count5;
	sum -= count5 * 5;
	
	count2 = sum / 2;
	if (count2 > storedMoney->count2)
		count2 = storedMoney->count2;
	sum -= count2 * 2;
	
	count1 = sum;
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


void getChangeFromCashRegister(size_t id, Money *money)
{
	struct sembuf buf = {0, 0, 0};
	Money change;
	
	while (1) {
		buf.sem_op = -1;
		if (semop(cashRegisterSemID, &buf, 1) == -1) {
			perror("Opuszczenie semafora kasy");
			exit(1);
		}
		tried[id] = true;
		
		if (fillChange(&change, money->sum - SERVICE_PRICE)) {
			if (++(*changeCounter) == CLIENTS_TO_BE_SERVED) exit(0);
			tried[id] = false;
			
			buf.sem_op = 1;
			if (semop(cashRegisterSemID, &buf, 1) == -1) {
				perror("Podniesienie semafora kasy (sukces)");
				exit(1);
			}
			
			copyMoney(money, &change);
			return;
		} else {
			if (allTried()) exit(2);
			
			buf.sem_op = 1;
			if (semop(cashRegisterSemID, &buf, 1) == -1) {
				perror("Podniesienie semafora kasy (porazka)");
				exit(1);
			}
			
			while (tried[id]);
		}
	}
}