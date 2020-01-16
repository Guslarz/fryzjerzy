all:
	make main && make barber && make client

main: money.o chairs.o moneyTransfer.o waitingRoom.o main.c
	gcc money.o chairs.o moneyTransfer.o waitingRoom.o main.c -o run -Wall

barber: money.o chairs.o moneyTransfer.o waitingRoom.o barber.c
	gcc money.o chairs.o moneyTransfer.o waitingRoom.o \
	barber.c -o barber -Wall

client: money.o moneyTransfer.o waitingRoom.o client.c
	gcc money.o moneyTransfer.o waitingRoom.o client.c -o client -Wall

waitingRoom.o: waitingRoom/waitingRoom.c waitingRoom/waitingRoom.h
	gcc waitingRoom/waitingRoom.c \
	-c -o waitingRoom.o -Wall

moneyTransfer.o: moneyTransfer/moneyTransfer.c \
moneyTransfer/moneyTransfer.h
	gcc moneyTransfer/moneyTransfer.c \
	-c -o moneyTransfer.o -Wall

money.o: money/money.c money/money.h
	gcc money/money.c \
	-c -o money.o -Wall

chairs.o: chairs/chairs.c chairs/chairs.h
	gcc chairs/chairs.c \
	-c -o chairs.o -Wall

clean:
	rm -f *.o run