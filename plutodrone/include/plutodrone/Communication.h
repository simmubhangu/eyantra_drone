
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>

extern int socketSyckLock;

extern int socketOpStarted;

class Communication{


public:


bool connectSock();
bool disconnectSock();

int writeSock(const void *buf, int count);

uint8_t readSock(void *buf, int count);

void readFrame();





private:

int sockID;






};


#endif
