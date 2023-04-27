/*****************************************************************
  File:             BM22S4221-1.cpp
  Author:           KE,BESTMODULES
  Description:      Communication and operation function with module
  History：
  V1.0.1-- initial version；2022-11-02；Arduino IDE : v1.8.13
******************************************************************/
#include  "BM22S4221-1.h"

/**********************************************************
Description: Select the hardware serial port you need to use
Parameters:  *theSerial：hardware serial 
             BMduino optional:serial(default) serial1/seria2/seria3/seria4
             UNO optional:serial(default)
Return:      none    
Others:      
**********************************************************/
BM22S4221_1::BM22S4221_1(uint8_t statusPin,HardwareSerial*theSerial)
{
  _serial = theSerial;
  _softSerial = NULL;
  _statusPin = statusPin;
}
/**********************************************************
Description: Select the software serial port RX TX you need to use
Parameters:       rxPin:RX pin on the development board
             txPin:TX pin on the development board
Return:      none    
Others:      
**********************************************************/
BM22S4221_1::BM22S4221_1(uint8_t statusPin,uint8_t rxPin, uint8_t txPin)
{
  _serial = NULL;
  _statusPin = statusPin;
  _rxPin = rxPin;
  _txPin = txPin;
  _softSerial = new SoftwareSerial(_rxPin, _txPin);
}
/**********************************************************
Description: Set serial baud rate
Parameters:  uartBaud：9600(default)
Return:      none
Others:
**********************************************************/
void BM22S4221_1::begin()
{
  if (_softSerial != NULL)
  {
    _softSerial->begin(UART_BAUD);
  }
  else
  {
    _serial->begin(UART_BAUD);
  }
  pinMode(_statusPin, INPUT);
}
/**********************************************************
Description: Get STATUS pin level
Parameters: None
Return: None
Others: None
**********************************************************/
uint8_t BM22S4221_1::getSTATUS()
{
  return digitalRead(_statusPin);
}

/**********************************************************
Description: Get all current data of the module
Parameters:  buff(25byte): Store the FW version and production date
Return:     1: module data acquisition failed, there is no correct feedback value
            0: Module data obtained successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::requestInfoPackage(uint8_t buff[])
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xAc, 0x00, 0x00, 0x54};
  wirteBytes(uniCmd, 4);
  delay(50);
  if (readBytes(buff,25,10) == 0x00 && buff[4] == 0xAc)
  {
    return  0;
  }
  else
  {
    return  1;
  }
}
/**********************************************************
Description: Query the FW version and production date.
             The FW version number and production date are both 8421 BCD code.
Parameters:  buff[]:12 byte
             buff[0]:year  buff[1]:month  buff[2]:day
Return:      1: module data acquisition failed, there is no correct feedback value
             0: Module data obtained successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::getFWVer()
{
  clear_UART_FIFO();
  uint16_t FWVer=0;
  uint8_t uniCmd[4] = {0xAD, 0x00, 0x00, 0x53};
  uint8_t uniAck[12];
  wirteBytes(uniCmd,4);
  if (readBytes(uniAck,12,10) == 0x00 && uniAck[4] == 0xAD)
  {
    FWVer=(uniAck[6]<<8 | uniAck[7]);
  }
  return   FWVer;
}
/**********************************************************
Description: Query the FW version
             The FW version number and production date are both 8421 BCD code.
Parameters:  none
Return:      0:read error
             data:16bit FWVer
Others:
**********************************************************/

uint8_t BM22S4221_1::getProDate(uint8_t buff[])
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xAD, 0x00, 0x00, 0x53};
  uint8_t uniAck[12];
  wirteBytes(uniCmd, 4);
  if (readBytes(uniAck,12,10) == 0x00 && uniAck[4] == 0xAD)
  {
    buff[0]=uniAck[8];
    buff[1]=uniAck[9];
    buff[2]=uniAck[10];
    return    0;
  }
  else
  {
    return   1;
  }
}
/**********************************************************
Description: Query whether the serial port data output of the current device is enabled
Parameters:  state: Store data
Return:      state:1/0
             0: Serial port TX disable
             1: Serial port TX enable
Others:
**********************************************************/
bool BM22S4221_1::isAutoTx()
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xd0, 0x1b,0x00,0x15};
  uint8_t uniAck[8];
  uint8_t state=0;
  wirteBytes(uniCmd, 4);
  delay(20);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[6] == AUTO)
  {
    state = 1;
  }
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[6] == PASSIVE)
  {
    state = 0;
  }
  return state;
}

/**********************************************************
Description: Query the normal output of equipment alarm output level
Parameters:  store data 1/0
Return:      ActiveMode:
             0: Status output low level, normal state is high level
             1: Status output high level, normal low level
Others:
**********************************************************/
uint8_t BM22S4221_1::getStatusPinActiveMode()
{  
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xd0, 0x1c,0x00,0x14};
  uint8_t uniAck[8];
  uint8_t ActiveMode=0;
  wirteBytes(uniCmd, 4);
  delay(20);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[6] == HIGH_LEVEL)
  {
    ActiveMode = 1;
  }
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[6] ==LOW_LEVEL)
  {
    ActiveMode = 0;
  }
  return  ActiveMode;
}

/**********************************************************
Description: Query internal VBG voltage a/d value
             1.25V/VDD×256=VBG a/d value
             Vbgad: used to store acquired internal Ad value of BG voltage
Return:    0: module data read error
           data:Vbgad internal Ad value of BG voltage
Others:
**********************************************************/
uint8_t BM22S4221_1::getVBG()
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xd2, 0x4c,0x00,0xe2};
  uint8_t uniAck[8];
  uint8_t VBG=0;
  wirteBytes(uniCmd, 4);
  delay(20);
  if(readBytes(uniAck,8,10) == 0x00 && uniAck[4]==0xd2)
  {
  VBG = uniAck[6];
  }
  return  VBG;
}
/**********************************************************
Description: Read the data automatically output by the module
Parameters:  none
Return:      1:is 25 byte data
             0:Incorrect data
Others:
**********************************************************/
bool BM22S4221_1::isInfoAvailable()
{
  uint8_t header[5] = {0xAA,0x19, 0x31, 0x01, 0xAC};
  uint8_t recBuf[25] = {0};
  uint8_t i, num = 0, readCnt = 0, failCnt = 0, checkCode = 0;
  bool isHeader = 0, result = false;

  /* Select hardSerial or softSerial according to the setting */
  if (_softSerial != NULL)
  {
    num = _softSerial->available();
  }
  else if (_serial != NULL)
  {
    num = _serial->available();
  }
  /* Serial buffer contains at least one 32-byte data */
  if (num >= 25)
  {
    while (failCnt < 2) // Didn't read the required data twice, exiting the loop
    {
      /* Find 5-byte data header */
      for (i = 0; i < 5;)
      {
        if (_softSerial != NULL)
        {
          recBuf[i] = _softSerial->read();
        }
        else if (_serial != NULL)
        {
          recBuf[i] = _serial->read();
        }
        if (recBuf[i] == header[i])
        {
          isHeader = 1; // Fixed code is correct
          i++;             // Next byte
        }
        else if (recBuf[i] != header[i] && i > 0)
        {
          isHeader = 0; // Next fixed code error
          failCnt++;
          break;
        }
        else if (recBuf[i] != header[i] && i == 0)
        {
          readCnt++; // 0xAA not found, continue
        }
        if (readCnt >= (num - 5))
        {
          readCnt = 0;
          isHeader = 0; //
          break;
        }
      }
      /* Find the correct data header */
      if (isHeader)
      {
        for (i = 0; i < 5; i++)
        {
          checkCode += recBuf[i]; // Sum checkCode
        }
        for (i = 5; i < 25; i++) // Read subsequent 27-byte data
        {
          if (_softSerial != NULL)
          {
            recBuf[i] = _softSerial->read();
          }
          else if (_serial != NULL)
          {
            recBuf[i] = _serial->read();
          }
          checkCode += recBuf[i]; // Sum checkCode
        }
        checkCode = checkCode - recBuf[24];
        checkCode = (~checkCode) + 1; // Calculate checkCode
        /* Compare whether the check code is correct */
        if (checkCode == recBuf[24])
        {
          for (i = 0; i < 25; i++) // True, assign data to _recBuf[]
          {
            _recBuf[i] = recBuf[i];
          }
          result = true;
          break; // Exit "while (failCnt < 2)" loop
        }
        else
        {
          failCnt++; // Error, failCnt plus 1, return "while (failCnt < 2)" loop
          checkCode = 0;
        }
      }
    }
    clear_UART_FIFO();
  }
  return result;
}
/**********************************************************
Description: Read the data automatically output by the module
Parameters:  array[]:25 byte data
Return:    
Others:     
**********************************************************/
void BM22S4221_1::readInfoPackage(uint8_t array[])
{
  for (uint8_t i = 0; i < 25; i++)
  {
    array[i] = _recBuf[i];
  }
}
/**********************************************************
Description: Send command to restore the module to factory settings
               Factory settings:
               1. internal OPA gain control: maximum gain 37
               2. alarm deviation: 15
               3. alarm detection delay: 3S
               4. alarm signal output: 3S
               5. preheating time: 30s
               6. serial port automatic output de energization
               7. alarm signal output level is high
Parameters:  none
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::restoreDefault()
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xa0, 0x00, 0x00, 0x60};
  uint8_t uniAck[8]={0};
  wirteBytes(uniCmd, 4);
  delay(100);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xa0)
  {
    return  0;
  }
  else
  {
    return  1;
  }
}
/**********************************************************
Description: Send instructions to reset the chip in the module
Parameters:  none
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:     
**********************************************************/
uint8_t BM22S4221_1::resetModule()
{ 
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xaf, 0x00, 0x00, 0x51};
  uint8_t uniAck[8];
  wirteBytes(uniCmd, 4);
  delay(20);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xaf)
  {
    delay(60);//reset time
    return  0;
  }
  else
  {
    return  1;
  }
}

/**********************************************************
Description: Modify the device serial port data output Disable
Parameters:  state：PASSIVE 0x00 / AUTO 0x08
             AUTO:Automatic output of enabling module TX pin
             PASSIVE:Automatic output de energization of enabling module TX pin
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::setAutoTx(uint8_t state)
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xe0,0x1b,state,0x51};
  uint8_t uniAck[8]={0};
  uint16_t a=0xe0 + 0x1b + state;
  uniCmd[3] = ~(lowByte(a)) + 1;
  wirteBytes(uniCmd,4);
  delay(100);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xe0)
    {
      return  0;
    }
    else
    {
      return  1;
    }
}
/**********************************************************
Description: Modify device alarm output level
Parameters:  state:HIGH/LOW
             LOW:When alerting, Status outputs HIGH level, and the normal state is LOW level
             HIGH:When alerting, Status outputs LOW level, and the normal state is HIGH leve
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::setStatusPinActiveMode(uint8_t state)
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xe0, 0x1c, state, 0x51};
  uint8_t uniAck[8]={0};
    uint16_t a=0xe0 + 0x1c + state;
    uniCmd[3] = ~(lowByte(a)) + 1;
    wirteBytes(uniCmd, 4);
    delay(100);
    if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xe0)
    {
      return  0;
    }
    else
    {
      return  1;
    }
}
/**********************************************************
Description: Modify Internal OPA Gain
Parameters:  value:setting range is 0!31
             OPA gain=128 + value*8
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::setOpaGain(uint8_t value)
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xe0, 0x05, value, 0x51};
  uint8_t uniAck[8];
  uint16_t a=0xe0 + 0x05 + value;
  uniCmd[3] = ~(lowByte(a)) + 1;
  wirteBytes(uniCmd, 4);
  delay(100);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xe0)
  {
    return  0;
  }
  else
  {
    return  1;
  }
}
/**********************************************************
Description: Modify detection deviation value
             deviation value:Change amount of alarm detection
Parameters:  value:Setting range 15~120
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::setAlarmThreshold(uint8_t Threshold)
{
  clear_UART_FIFO();
  uint8_t uniCmd[4] = {0xe0, 0x07, Threshold, 0x51};
  uint8_t uniAck[8];
  uint16_t a=0xe0 + 0x07 + Threshold;
  uniCmd[3] = ~(lowByte(a)) + 1;
  wirteBytes(uniCmd, 4);
  delay(100);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xe0)
  {
    return  0;
  }
  else
  {
    return  1;
  }
}
/**********************************************************
Description: Modify alarm detection delay time          
Parameters:  time:Alarm detection delay time = n × 0.5s=time
             unit s.
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::setAlarmDetectDelay(uint8_t time)
{
  clear_UART_FIFO();
  time=time * 2;
  uint8_t uniCmd[4] = {0xe0, 0x08, time, 0x51};
  uint8_t uniAck[8];
  uint16_t a=0xe0 + 0x08 + time;
  uniCmd[3] = ~(lowByte(a)) + 1;
  wirteBytes(uniCmd, 4);
  delay(100);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xe0 )
  {
    return  0;
  }
  else
  {
    return  1;
  }
}
/**********************************************************
Description: Modify the output time of alarm signal status pin           
Parameters:  time:Alarm status pin output time = n × 0.5s=time  unit s.
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::setAlarmOutputTime(uint8_t time)
{
  clear_UART_FIFO();
  time=time * 2;
  uint8_t uniCmd[4] = {0xe0, 0x09, time, 0x51};
  uint8_t uniAck[8];
  uint16_t a=0xe0 + 0x09 + time;
  uniCmd[3] = ~(lowByte(a)) + 1;
  wirteBytes(uniCmd, 4);
  delay(100);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xe0)
  {
    return  0;
  }
  else
  {
    return  1;
  }
}
/**********************************************************
Description: Modify preheating time
             Change to less than 30s
Parameters:  time:The setting range is 30~127 and the preheating time is not repairable
Return:      1: Module setting failed without correct feedback value
             0: Module set successfully
Others:
**********************************************************/
uint8_t BM22S4221_1::setPreheaTime(uint8_t time)
{
  clear_UART_FIFO();
  time=time * 2;
  uint8_t uniCmd[4] = {0xe0, 0x0c, time, 0x51};
  uint8_t uniAck[8];
  uint16_t a=0xe0 + 0x0c + time;
  uniCmd[3] = ~(lowByte(a)) + 1;
  wirteBytes(uniCmd, 4);
  delay(100);
  if (readBytes(uniAck,8,10) == 0x00 && uniAck[4] == 0xe0)
  {
    return  0;
  }
  else
  {
    return  1;
  }
}
/**********************************************************
Description: UART readBytes
Parameters:  rbuf:Variables for storing Data to be read
             len:Length of data plus command
Return:      0: check ok
             1: check error
             2: timeout error
Others:
**********************************************************/

uint8_t BM22S4221_1::readBytes(uint8_t rbuf[], uint8_t len, uint16_t waitTime)
{
  uint8_t i, delayCnt, checkCode;
  /* Select hardSerial or softSerial according to the setting,
     check if there is data in the UART Receive FIFO */
  if (_softSerial != NULL)
  {
    for (i = 0; i < len; i++)
    {
      delayCnt = 0;
      while (_softSerial->available() == 0)
      {
        if (delayCnt > waitTime)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _softSerial->read(); // Receive data
    }
  }
  else if (_serial != NULL)
  {
    for (i = 0; i < len; i++)
    {
      delayCnt = 0;
      while (_serial->available() == 0)
      {
        if (delayCnt > waitTime)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _serial->read(); // Receive data
    }
  }
  /* Calculate check code*/
  for (i = 0, checkCode = 0; i < (len - 1); i++)
  {
    checkCode += rbuf[i];
  }
  checkCode = ~checkCode + 1;
  /* Proofread check code*/
  if (checkCode ==rbuf[len - 1])
  {
    return CHECK_OK ; // Check correct
  }
  else
  {
    return CHECK_ERROR; // Check error
  }

}
/**********************************************************
Description: UART wirteBytes
             Automatically determine whether a hardware or software serial  is used
Parameters:  wbuf:Variables for storing Data to be read
             len:Length of data plus command
Return:      none
Others:
**********************************************************/
void  BM22S4221_1::wirteBytes(uint8_t wbuf[], uint8_t len)
{
  if (_softSerial != NULL)
  {
     while (_softSerial->available() > 0)
    {
      _softSerial->read();
    }
    _softSerial->write(wbuf,len);
  }
  else
  {
    while (_serial->available() > 0)
    {
      _serial->read();
    }
    _serial->write(wbuf,len);
  }
  delay(70);//TDEL-RSP --Response delay time
}
/**********************************************************
Description: eliminate buff data
Parameters:  none
Return:      none    
Others:      
**********************************************************/ 

void BM22S4221_1::clear_UART_FIFO()
{
    if (_softSerial != NULL)
  {
    while (_softSerial->available() > 0)
    {
      _softSerial->read();
    }
  }
  else
  {
    while (_serial->available() > 0)
    {
      _serial->read();
    }
  }
}
