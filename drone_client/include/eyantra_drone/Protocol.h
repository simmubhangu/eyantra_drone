
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <vector>
#include <plutodrone/Common.h>

extern int8_t inputBuffer[1024];
extern uint8_t bufferIndex;
//extern NSMutableArray* requests;

static const int MSP_FC_VERSION=3;
static const int MSP_RAW_IMU=102;
static const int MSP_RC = 105;
static const int MSP_ATTITUDE=108;
static const int MSP_ALTITUDE=109;
static const int MSP_ANALOG=110;
static const int MSP_SET_RAW_RC=200;
static const int MSP_ACC_CALIBRATION=205;
static const int MSP_MAG_CALIBRATION=206;
static const int MSP_SET_MOTOR=214;
static const int MSP_SET_ACC_TRIM=239;
static const int MSP_ACC_TRIM=240;
static const int MSP_EEPROM_WRITE = 250;
static const int MSP_SET_POS= 216;

static const int IDLE = 0, HEADER_START = 1, HEADER_M = 2, HEADER_ARROW = 3, HEADER_SIZE = 4, HEADER_CMD = 5, HEADER_ERR = 6;

extern int roll;
extern int pitch;
extern int yaw;
extern float battery;
extern int rssi;

extern float accX;
extern float accY;
extern float accZ;

extern float gyroX;
extern float gyroY;
extern float gyroZ;

extern float magX;
extern float magY;
extern float magZ;

extern float alt;

extern int FC_versionMajor;
extern int FC_versionMinor;
extern int FC_versionPatchLevel;

extern int trim_roll;
extern int trim_pitch;


extern float rcThrottle, rcRoll, rcPitch, rcYaw, rcAUX1 , rcAUX2, rcAUX3, rcAUX4 ;

class Protocol{

public:


int read8();
int read16();
int read32();

void evaluateCommand(int command);


void sendRequestMSP(std::vector<int8_t> data);
void sendMulRequestMSP(std::vector<int8_t> data, int i);

void sendRequestMSP_SET_RAW_RC(int channels[]);
void sendMulRequestMSP_SET_RAW_RC(int channels[]);

void sendRequestMSP_SET_POS(int posArray[]);


void sendRequestMSP_GET_DEBUG(std::vector<int> requests);
void sendMulRequestMSP_GET_DEBUG(std::vector<int> requests, int index);

std::vector<int8_t> createPacketMSP(int msp, std::vector<int8_t>payload);


private:


};


#endif
