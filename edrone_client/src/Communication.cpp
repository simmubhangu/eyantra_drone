/*
BSD 2-Clause License

Copyright (c) 2019, Simranjeet Singh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ros/ros.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <eyantra_drone/Communication.h>
#include <eyantra_drone/Protocol.h>


using namespace std;
Protocol pro;
int indx=0;
unsigned int len = 0;
uint8_t checksum=0;
uint8_t command=0;
uint8_t payload_size=0;

int optval;
socklen_t optlen = sizeof(optval);

int socketSyckLock=0;
int socketOpStarted=0;
int checksumIndex=0;
uint8_t recbuf[1024];

int c_state = IDLE;
uint8_t c;
bool err_rcvd = false;
int offset = 0, dataSize = 0;
// uint8_t checksum = 0;
uint8_t cmd;
//byte[] inBuf = new byte[256];
int i = 0;

bool Communication::connectSock(){
  int res;
  struct sockaddr_in addr;
  long arg;
  fd_set myset;
  struct timeval tv;
  int valopt;
  socklen_t lon;

  cout<<"Connecting to eDrone......\n";

  // Create socket
  sockID = socket(AF_INET, SOCK_STREAM, 0);
  //Check if socket is created. If not, socket() returns -1
  if (sockID < 0) {
    // fprintf(stderr, "Error creating socket (%d %s)\n", errno, strerror(errno));
     cout<<"Cannot connect to DroneNode, please try again1\n";
     exit(0);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(23);//23 is the PORT to connect to as defined in DroneNode.cpp. Use port 9060 for Drone camera!
  addr.sin_addr.s_addr = inet_addr("192.168.4.1");//Use 192.168.0.1 for Drone camera!
  
  //socket() sets it to blocking
  // Set to non-blocking. arg will re
  if( (arg = fcntl(sockID, F_GETFL, NULL)) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
    cout<<"Cannot connect to Drone, please try again2\n";
     exit(0);
  }
  arg |= O_NONBLOCK;
  if( fcntl(sockID, F_SETFL, arg) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
    cout<<"Cannot connect to Drone, please try again3\n";
    exit(0);
  }

  // Trying to connect with timeout
  res = connect(sockID, (struct sockaddr *)&addr, sizeof(addr));
  //fprintf(stderr,"socket is ready %d",res);
  //If res < 0, failure. If res == 0, success.
  if (res < 0) {
     if (errno == EINPROGRESS) {
      //fprintf(stderr, "socket is ready %d ",res);
      //fprintf(stderr, "EINPROGRESS in connect() - selecting\n");
      do {
        tv.tv_sec = 7;
        tv.tv_usec = 0;
        FD_ZERO(&myset);
        FD_SET(sockID, &myset);
        res = select(sockID+1, NULL, &myset, NULL, &tv);
        //fprintf(stderr,"socket is ready %d",res);
        if (res < 0 && errno != EINTR) {
          //fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
          cout<<"Cannot connect to Drone, please try again4\n";
          exit(0);
        }
        else if (res > 0) {
          // Socket selected for write
          lon = sizeof(int);
          if (getsockopt(sockID, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
            //fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));
                 cout<<"Cannot connect to Drone, please try again5\n";
                 exit(0);
              }
              // Check the value returned...
              if (valopt) {
                // fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt));
                cout<<"Cannot connect to Drone, please try again6\n";

                 exit(0);
              }
              break;
           }
           else {
            //  fprintf(stderr, "Timeout in select() - Cancelling!\n");
             cout<<"Cannot connect to Drone, please try again7\n";
              exit(0);
           }
        } while (1);
     }
     else {
        //fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
         cout<<"Cannot connect to Drone, please try again8\n";
        exit(0);
     }
  }

  // Set to blocking mode again...
  if( (arg = fcntl(sockID, F_GETFL, NULL)) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
     cout<<"Cannot connect to Drone, please try again9\n";
      exit(0);
  }
  arg &= (~O_NONBLOCK);
  if( fcntl(sockID, F_SETFL, arg) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
     cout<<"Cannot connect to Drone, please try again0\n";
     exit(0);
  }
  // I hope that is all
  //printf("Hello message sent\n");
  /* Check the status for the keepalive option */
  if(getsockopt(sockID, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
    //  perror("getsockopt()");
    cout<<"Cannot connect to Drone, please try again11\n";
    close(sockID);
    exit(EXIT_FAILURE);
  }
  // printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));
  /* Set the option active */
  optval = 1;
  optlen = sizeof(optval);
  if(setsockopt(sockID, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
    //  perror("setsockopt()");
      cout<<"Cannot connect to Drone, please try again12\n";
      close(sockID);
      exit(EXIT_FAILURE);
  }
  //printf("SO_KEEPALIVE set on socket\n");
  /* Check the status again */
  if(getsockopt(sockID, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
  //  perror("getsockopt()");
      cout<<"Cannot connect to Drone, please try again13\n";
      close(sockID);
      exit(EXIT_FAILURE);
  }
  //printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

  int error = 0;
  socklen_t len = sizeof (error);
  int retval = getsockopt (sockID, SOL_SOCKET, SO_ERROR, &error, &len);

  if (retval != 0) {
    /* there was a problem getting the error code */
    //  fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
    cout<<"Cannot connect to Drone, please try again14\n";
    exit(EXIT_FAILURE);
    // return;
  }

  if (error != 0) {
    /* socket has a non zero error status */
    //  fprintf(stderr, "socket error: %s\n", strerror(error));
    cout<<"Cannot connect to Drone, please try again15\n";
    exit(EXIT_FAILURE);
  }else{
    //fprintf(stderr, "socket is up \n");
    cout<<"eDrone Connected\n";
  }
  return true;
}

int Communication::writeSock(const void *buf, int count){
  int k=write(sockID,buf,count);
  socketSyckLock=1;
  return k;
}

uint8_t Communication::readSock(void *buf, int count){
  int k=read(sockID,buf,count);
  if(k>0){
    uint8_t val=recbuf[0];
    return val;
  }else{
    return k;
  }
}

void Communication::readFrame(){
  c = readSock(recbuf,1);
  if (c_state == IDLE){
    c_state = (c == '$') ? HEADER_START : IDLE;
  }else if (c_state == HEADER_START) {
    c_state = (c == 'M') ? HEADER_M : IDLE;
  }else if (c_state == HEADER_M) {
    if (c == '>') {
      c_state = HEADER_ARROW;
    } else if (c == '!') {
      c_state = HEADER_ERR;
    } else {
      c_state = IDLE;
    }
  } else if (c_state == HEADER_ARROW || c_state == HEADER_ERR) {
    /* is this an error message? */
    err_rcvd = (c_state == HEADER_ERR);
    /* now we are expecting the payload size */
    dataSize = (c & 0xFF);
    /* reset index variables */
    //  p = 0;
    offset = 0;
    checksum = 0;
    checksum ^= (c & 0xFF);
    /* the command is to follow */
    c_state = HEADER_SIZE;
  }else if (c_state == HEADER_SIZE) {
    cmd = (uint8_t) (c & 0xFF);
    //printf("cmd Value= %i\n",cmd );
    checksum ^= (c & 0xFF);
    c_state = HEADER_CMD;
  }else if (c_state == HEADER_CMD && offset < dataSize) {
    checksum ^= (c & 0xFF);
    inputBuffer[offset++] = (uint8_t) (c & 0xFF);
    if(cmd==108){
      //  Log.d("#########", "MSP_ATTITUDE: recived payload= "+inBuf[offset-1]);
    }
  }else if (c_state == HEADER_CMD && offset >= dataSize) {
    /* compare calculated and transferred checksum */
    if ((checksum & 0xFF) == (c & 0xFF)) {
      if (err_rcvd) {
        //  Log.e("Multiwii protocol",
        //        "Copter did not understand request type " + c);
      }else {

        bufferIndex=0;
        //printf("cmd Value= %i\n",cmd );
        pro.evaluateCommand(cmd);
        //SONG BO ---------------------------------------
        //  DataFlow = DATA_FLOW_TIME_OUT;
      }
    }else {

    }
    c_state = IDLE;
  }
}

bool Communication::connectMulSock(const std::string& ip, int index){

  int res;
  struct sockaddr_in addr;
  long arg;
  fd_set myset;
  struct timeval tv;
  int valopt;
  socklen_t lon;

  cout<<"Connecting to Drone\n";

  // Create interface
  sockIDList[index] = socket(AF_INET, SOCK_STREAM, 0);
  //Check if socket is created. If not, socket() returns -1
  if (sockIDList[index] < 0) {
    // fprintf(stderr, "Error creating socket (%d %s)\n", errno, strerror(errno));
     cout<<"Cannot connect to Drone, please try again1\n";
     exit(0);
  }

  //address of the server
  addr.sin_family = AF_INET;
  addr.sin_port = htons(23);//23 is the PORT to connect to as defined in DroneNode.cpp!
  addr.sin_addr.s_addr = inet_addr(ip.c_str());

  //socket() sets it to blocking
  // Set to non-blocking. arg will re
  if( (arg = fcntl(sockIDList[index], F_GETFL, NULL)) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
    cout<<"Cannot connect to Drone, please try again2\n";
     exit(0);
  }
  arg |= O_NONBLOCK;
  if( fcntl(sockIDList[index], F_SETFL, arg) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
    cout<<"Cannot connect to Drone, please try again3\n";
    exit(0);
  }

  // Trying to connect with timeout. connect() is blocking
  res = connect(sockIDList[index], (struct sockaddr *)&addr, sizeof(addr));
  //fprintf(stderr,"socket is ready %d",res);
  //If res < 0, failure. If res == 0, success.
  if (res < 0) {
     if (errno == EINPROGRESS) {
      //fprintf(stderr, "socket is ready %d ",res);
      //fprintf(stderr, "EINPROGRESS in connect() - selecting\n");
      do {
        tv.tv_sec = 7;
        tv.tv_usec = 0;
        FD_ZERO(&myset);
        FD_SET(sockIDList[index], &myset);
        res = select(sockIDList[index]+1, NULL, &myset, NULL, &tv);
        //fprintf(stderr,"socket is ready %d",res);
        // cout<<res;
        if (res < 0 && errno != EINTR) {
          //fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
          cout<<"Cannot connect to Drone, please try again4\n";
          exit(0);
        }else if (res > 0) {
          // Socket selected for write
          lon = sizeof(int);
          if (getsockopt(sockIDList[index], SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
            //fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));
            cout<<"Cannot connect to Drone, please try again5\n";
            exit(0);
          }
          // Check the value returned...
          if (valopt) {
            // fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt));
            cout<<"Cannot connect to Drone, please try again6\n";
            exit(0);
          }
          break;
        }else {
          //  fprintf(stderr, "Timeout in select() - Cancelling!\n");
          cout<<"Cannot connect to Drone, please try again7\n";
          exit(0);
        }
      }while (1);
     }
     else {
        //fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
         cout<<"Cannot connect to Drone, please try again8\n";
        exit(0);
     }
  }

  // Set to blocking mode again...
  if( (arg = fcntl(sockIDList[index], F_GETFL, NULL)) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
     cout<<"Cannot connect to Drone, please try again9\n";
      exit(0);
  }
  arg &= (~O_NONBLOCK);
  if( fcntl(sockIDList[index], F_SETFL, arg) < 0) {
    // fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
     cout<<"Cannot connect to Drone, please try again10\n";
     exit(0);
  }

  /* Check the status for the keepalive option */
  if(getsockopt(sockIDList[index], SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
    //  perror("getsockopt()");
    cout<<"Cannot connect to Drone, please try again11\n";
    close(sockIDList[index]);
    exit(EXIT_FAILURE);
  }
  // printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));
  /* Set the option active */
  optval = 1;
  optlen = sizeof(optval);
  if(setsockopt(sockIDList[index], SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
    //  perror("setsockopt()");
      cout<<"Cannot connect to Drone, please try again12\n";
      close(sockIDList[index]);
      exit(EXIT_FAILURE);
  }
  //printf("SO_KEEPALIVE set on socket\n");
  /* Check the status again */
  if(getsockopt(sockIDList[index], SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
  //  perror("getsockopt()");
      cout<<"Cannot connect to Drone, please try again13\n";
      close(sockIDList[index]);
      exit(EXIT_FAILURE);
  }
  //printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

  int error = 0;
  socklen_t len = sizeof (error);
  int retval = getsockopt (sockIDList[index], SOL_SOCKET, SO_ERROR, &error, &len);

  if (retval != 0) {
    /* there was a problem getting the error code */
    //  fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
    cout<<"Cannot connect to Drone, please try again14\n";
    exit(EXIT_FAILURE);
    // return;
  }

  if (error != 0) {
    /* socket has a non zero error status */
    //  fprintf(stderr, "socket error: %s\n", strerror(error));
    cout<<"Cannot connect to Drone, please try again15\n";
    exit(EXIT_FAILURE);
  }else{
    //fprintf(stderr, "socket is up \n");
    cout<<"Drone Connected\n";
  }
  return true;
}

int Communication::writeMulSock(const void *buf, int count, int i){
  // while (socketSyckLock) {
  //   /* code */
  // //printf("value of synclock in write = %i\n",socketSyckLock );
  //   usleep(2);
  // }
  //usleep(2000);
  int k=write(sockIDList[i],buf,count);
  //socketOpStarted=1;
  //usleep(500);
  //readFrame();
  socketSyckLock=1;
  return k;
}

uint8_t Communication::readMulSock(void *buf, int count, int index){
  int k=read(sockIDList[index],buf,count);
  if(k>0){
    uint8_t val=recbuf[0];
    return val;
  }else{
    return k;
  }
}

void Communication::readMulFrame(int index){
  cout<<index;
  c = readMulSock(recbuf,1,index);
  //  Log.v("READ", "Data: " + c);
  //  printf("read Value= %i\n",c );
  //  c_state = IDLE;
  //  Log.e("MultiwiiProtocol", "Read  = null");
  if (c_state == IDLE){
    c_state = (c == '$') ? HEADER_START : IDLE;
  }else if (c_state == HEADER_START) {
    c_state = (c == 'M') ? HEADER_M : IDLE;
  }else if (c_state == HEADER_M) {
    if (c == '>') {
      c_state = HEADER_ARROW;
    } else if (c == '!') {
      c_state = HEADER_ERR;
    } else {
      c_state = IDLE;
    }
  } else if (c_state == HEADER_ARROW || c_state == HEADER_ERR) {
    /* is this an error message? */
    err_rcvd = (c_state == HEADER_ERR);
    /* now we are expecting the payload size */
    dataSize = (c & 0xFF);
    /* reset index variables */
    //  p = 0;
    offset = 0;
    checksum = 0;
    checksum ^= (c & 0xFF);
    /* the command is to follow */
    c_state = HEADER_SIZE;
  }else if (c_state == HEADER_SIZE) {
    cmd = (uint8_t) (c & 0xFF);
    //printf("cmd Value= %i\n",cmd );
    checksum ^= (c & 0xFF);
    c_state = HEADER_CMD;
  }else if (c_state == HEADER_CMD && offset < dataSize) {
    checksum ^= (c & 0xFF);
    inputBuffer[offset++] = (uint8_t) (c & 0xFF);
    if(cmd==108){
      //  Log.d("#########", "MSP_ATTITUDE: recived payload= "+inBuf[offset-1]);
    }
  }else if (c_state == HEADER_CMD && offset >= dataSize) {
    /* compare calculated and transferred checksum */
    if ((checksum & 0xFF) == (c & 0xFF)) {
      if (err_rcvd) {
        //  Log.e("Multiwii protocol",
        //        "Copter did not understand request type " + c);
      }else {
        bufferIndex=0;
        //printf("cmd Value= %i\n",cmd );
        cout<<cmd;
      }
    }else 
    {

    }
    c_state = IDLE;
  }
  // cout<<c_state;
}

