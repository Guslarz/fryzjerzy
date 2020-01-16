#ifndef WAITING_ROOM_H
#define WAITING_ROOM_H

#include "../keys.h"

#include <stdlib.h>
#include <stdbool.h>


/*
 *  WAITING ROOM
 */
void getAccessToWaitingRoom();
void clearWaitingRoom();
void initWaitingRoom();
/*
 *  FOR BARBER
 */
size_t getNextClient();
/*
 *  FOR CLIENT
 */
bool tryTakeEmptySeat(size_t);


#endif