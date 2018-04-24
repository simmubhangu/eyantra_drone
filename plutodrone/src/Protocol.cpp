#include <unistd.h>
#include <plutodrone/Protocol.h>
#include <string>



std::string MSP_HEADER="$M<";

int8_t inputBuffer[1024];
uint8_t bufferIndex=0;
//NSMutableArray* requests;


int roll=0;
int pitch=0;
int yaw=0;
float battery=0;
int rssi=0;

float accX=0;
float accY=0;
float accZ=0;

float gyroX=0;
float gyroY=0;
float gyroZ=0;

float magX=0;
float magY=0;
float magZ=0;

float alt=0;


int FC_versionMajor=0;
int FC_versionMinor=0;
int FC_versionPatchLevel=0;

int trim_roll=0;
int trim_pitch=0;

float rcThrottle = 1500, rcRoll = 1500, rcPitch = 1500, rcYaw = 1500, rcAUX1 = 1500, rcAUX2 = 1500, rcAUX3 = 1500, rcAUX4 = 1500;

int Protocol::read8()
{
  return inputBuffer[bufferIndex++] & 0xff;
}

int Protocol::read16()
{
  int add_1=(inputBuffer[bufferIndex++] & 0xff) ;
  int add_2=((inputBuffer[bufferIndex++]) << 8);

  return add_1+add_2;
}

int Protocol::read32()
{
  return (inputBuffer[bufferIndex++] & 0xff) + ((inputBuffer[bufferIndex++] & 0xff) << 8) 
  + ((inputBuffer[bufferIndex++] & 0xff) << 16) + ((inputBuffer[bufferIndex++] & 0xff) << 24);
}

void Protocol::evaluateCommand(int command)
{
  switch (command) {
    
    case MSP_FC_VERSION:
      FC_versionMajor=read8();
      FC_versionMinor=read8();
      FC_versionPatchLevel=read8();

      break;

    case MSP_RAW_IMU:
      
      accX=read16();
      accY=read16();
      accZ=read16();

      gyroX=read16()/8;
      gyroY=read16()/8;
      gyroZ=read16()/8;

      magX=read16()/3;
      magY=read16()/3;
      magZ=read16()/3;

      break;

    case MSP_ATTITUDE:
      //for(int i=0; i<6;i++)
      //NSLog(@"value of %i",inputBuffer[i]);
      roll=(read16()/10);
      pitch=(read16()/10);
      yaw=read16();

      break;

    case MSP_ALTITUDE:
      alt=(read32()/10)-0;
      
      break;

    case MSP_ANALOG:

      battery=(read8()/10.0);
      rssi=read16();

      break;

    case MSP_ACC_TRIM:

      trim_pitch=read16();
      trim_roll=read16();

      break;

    case MSP_RC:

      rcRoll = read16();
      rcPitch = read16();
      rcYaw = read16();
      rcThrottle = read16();
      rcAUX1 = read16();
      rcAUX2 = read16();
      rcAUX3 = read16();
      rcAUX4 = read16();

      break;
    
    default:
      break;
  }
}

void Protocol::sendRequestMSP(std::vector<int8_t> data)
{
  com.writeSock(&data[0],data.size());
}

void Protocol::sendMulRequestMSP(std::vector<int8_t> data, int i)
{
  com.writeMulSock(&data[0],data.size(),i);
}

std::vector<int8_t>  Protocol::createPacketMSP(int msp, std::vector<int8_t>payload)
{
  if (msp < 0) {
    //return NULL;
  }
  std::vector<int8_t> bf;
  for(std::string::iterator it = MSP_HEADER.begin(); it != MSP_HEADER.end(); ++it) {
    bf.push_back((int8_t) (*it & 0xFF));
  }
  int8_t checksum = 0;
  int8_t pl_size = (int8_t) ((!payload.empty() ? (int) (payload.size()) : 0) & 0xFF);
  bf.push_back(pl_size);
  checksum ^= (pl_size & 0xFF);

  bf.push_back((int8_t) (msp & 0xFF));
  checksum ^= (msp & 0xFF);

  if (!payload.empty()) {
    //printf("Adding payload %d\n");
    for (std::vector<int8_t>::iterator it = payload.begin() ; it != payload.end(); ++it) {
      int8_t k=*it;
      //printf("payload  value %i\n",k);
      bf.push_back((int8_t) (k & 0xFF));
      checksum ^= (k & 0xFF);
    }
  }
  bf.push_back(checksum);
  return (bf);
}

void Protocol::sendRequestMSP_SET_RAW_RC(int channels[])
{
  std::vector<int8_t>rc_signals(16);
  int index = 0;
  for (int i = 0; i < 8; i++) {
    //Log.d("drona", "Data: " + (channels8[i]));
    rc_signals[index++] = (int8_t) (channels[i] & 0xFF);
    rc_signals[index++] = (int8_t) ((channels[i] >> 8) & 0xFF);
    //printf("rcvalue = %i\n" ,channels[i]);
    //rc_signals.push_back((uint8_t) (channels[i] & 0xFF));
    //rc_signals.push_back( (uint8_t) ((channels[i] >> 8) & 0xFF));
  }
  //printf("size of rc_array %d\n",rc_signals.size() );
  sendRequestMSP(createPacketMSP(MSP_SET_RAW_RC, rc_signals));
}

void Protocol::sendMulRequestMSP_SET_RAW_RC(int channels[])
{
  std::vector<int8_t>rc_signals(16);
  int index = 0;
  int droneNumber = channels[8];
  for (int i = 0; i < 8; i++) {
    //Log.d("drona", "Data: " + (channels8[i]));
    rc_signals[index++] = (int8_t) (channels[i] & 0xFF);
    rc_signals[index++] = (int8_t) ((channels[i] >> 8) & 0xFF);
    //printf("rcvalue = %i\n" ,channels[i]);
    //rc_signals.push_back((uint8_t) (channels[i] & 0xFF));
    //rc_signals.push_back( (uint8_t) ((channels[i] >> 8) & 0xFF));
  }
  //printf("size of rc_array %d\n",rc_signals.size() );
  sendMulRequestMSP(createPacketMSP(MSP_SET_RAW_RC, rc_signals), droneNumber);
}

void Protocol::sendRequestMSP_SET_POS(int posArray[])
{
  std::vector<int8_t>posData(8);
  int index = 0;
  for (int i = 0; i < 4; i++) {
    //Log.d("drona", "Data: " + (channels8[i]));
    posData[index++] = (int8_t) (posArray[i] & 0xFF);
    posData[index++] = (int8_t) ((posArray[i] >> 8) & 0xFF);

    //  printf("posvalue = %i\n" ,posArray[i]);
    // rc_signals.push_back((uint8_t) (channels[i] & 0xFF));
    //rc_signals.push_back( (uint8_t) ((channels[i] >> 8) & 0xFF));
  }
 sendRequestMSP(createPacketMSP(MSP_SET_POS, posData));
}

void Protocol::sendRequestMSP_GET_DEBUG(std::vector<int> requests)
{
  for (size_t i = 0; i < requests.size(); i++) {
    {
      sendRequestMSP(createPacketMSP(requests[i], std::vector<int8_t>()));
    }
  }
}

void Protocol::sendMulRequestMSP_GET_DEBUG(std::vector<int> requests, int index)
{
  for (size_t i = 0; i < requests.size(); i++) {
    {
      sendMulRequestMSP(createPacketMSP(requests[i], std::vector<int8_t>()), index);
    }
  }
}