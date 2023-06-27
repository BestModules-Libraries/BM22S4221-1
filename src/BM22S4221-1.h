/*****************************************************************
File:             BM22S4221-1.h
Author:           KE,BESTMODULES
Description:      Define classes and required variables
History：         
V1.0.1-- initial version；2022-11-02；Arduino IDE : v1.8.13
******************************************************************/

#ifndef  _BM22S4221_h_
#define  _BM22S4221_h_
#include <Arduino.h>
#include <SoftwareSerial.h>
#define  UART_BAUD 9600
#define  AUTO 0x08
#define  PASSIVE  0x00
#define  HIGH_LEVEL 0x08
#define  LOW_LEVEL 0x00
#define  CHECK_OK        0
#define  CHECK_ERROR     1
#define  TIMEOUT_ERROR   2


 class BM22S4221_1
 {
    public:
    BM22S4221_1(uint8_t statusPin,HardwareSerial*theSerial);
    BM22S4221_1(uint8_t statusPin,uint8_t rxPin, uint8_t txPin);
    void begin();
    uint8_t getSTATUS();
    uint8_t requestInfoPackage(uint8_t buff[]);
    uint8_t getFWVer();
    uint8_t getProDate(uint8_t buff[]);  
    bool isAutoTx();
    uint8_t getStatusPinActiveMode();
    uint8_t getVBG();
    bool isInfoAvailable();
    void readInfoPackage(uint8_t array[]);
    uint8_t resetModule();
    uint8_t restoreDefault();
 
    uint8_t setAutoTx(uint8_t state);
    uint8_t setStatusPinActiveMode(uint8_t state);
    uint8_t setOpaGain(uint8_t value);
    uint8_t setAlarmThreshold(uint8_t Threshold);
    uint8_t setAlarmDetectDelay(uint8_t time=3);
    uint8_t setAlarmOutputTime(uint8_t time=3);
    uint8_t setPreheaTime(uint8_t time);
    
    private:
    void clear_UART_FIFO();
    uint8_t readBytes(uint8_t rbuf[], uint8_t len, uint16_t waitTime);
    void wirteBytes(uint8_t wbuf[], uint8_t len);
    uint8_t _recBuf[25] = {0}; // Array for storing received data
    uint8_t _rxPin;
    uint8_t _txPin;
    uint8_t _statusPin;
    HardwareSerial*_serial =NULL;
    SoftwareSerial *_softSerial =NULL;
 };


 
#endif
