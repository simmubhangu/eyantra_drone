
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>
#include <string>

extern int socketSyckLock;

extern int socketOpStarted;

class Communication{

public:

bool connectSock();
bool connectMulSock(const std::string& ip, int index);

bool disconnectSock();

int writeSock(const void *buf, int count);
int writeMulSock(const void *buf, int count, int i);

uint8_t readSock(void *buf, int count);
uint8_t readMulSock(void *buf, int count, int index);

void readFrame();
void readMulFrame(int index);

private:
int sockID;
int sockIDList[1000];
};
#endif
