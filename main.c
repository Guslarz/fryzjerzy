#include "params.h"
#include "waitingRoom/waitingRoom.h"
#include "moneyTransfer/moneyTransfer.h"
#include "chairs/chairs.h"
#include "cashRegister/cashRegister.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


pid_t pid[BARBER_COUNT + CLIENT_COUNT];


void initProcesses();
void endProcesses();
void clearIPC();


int main()
{	
    initWaitingRoom();
    initMoneyTransfer();
    initChairs();
    initCashRegister();
    initProcesses();
    endProcesses();
    clearIPC();
}


void initProcesses()
{
    char arg[10];
    size_t i;

    for (i = 0; i < BARBER_COUNT; ++i)
        switch(pid[i] = fork()) {
        case -1:
            perror("Utworzenie procesu potomnego (fryzjer)");
            exit(1);
        case 0:
            sprintf(arg, "%zu", i);
            execl("barber", "barber", arg, NULL);
            printf("Zmiana programu (fryzjer)");
            exit(1);
        }

    for (i = 0; i < CLIENT_COUNT; ++i)
        switch(pid[BARBER_COUNT + i] = fork()) {
        case -1:
            perror("Utworzenie procesu potomnego (klient)");
            exit(1);
        case 0:
            sprintf(arg, "%zu", i);
            execl("client", "client", arg, NULL);
            printf("Zmiana programu (klient)");
            exit(1);
        }
}


void endProcesses()
{
	int status;
    pid_t firstEnded = wait(&status);
    size_t i;
	
	switch(status >> 8) {
	case 0:
		printf("Wydano reszte %d klientom\n", CLIENTS_TO_BE_SERVED);
		break;
	case 1:
		printf("Blad procesu\n");
		break;
	case 2:
		printf("Wszyscy fryzjerzy czekaja na reszte\n");
		break;
	default:
		printf("Undefined\n");
		break;
	}

    for (i = 0; i < BARBER_COUNT + CLIENT_COUNT; ++i)
        if (pid[i] != firstEnded)
            kill(pid[i], 9);
    
    for (i = 0; i < BARBER_COUNT + CLIENT_COUNT - 1; ++i)
        wait(NULL);
}


void clearIPC()
{
    clearWaitingRoom();
    clearChairs();
    clearMoneyTransfer();
}