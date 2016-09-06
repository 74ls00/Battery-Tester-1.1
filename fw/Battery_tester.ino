/* Battery tester */
#include <EEPROM.h>
#include <SPI.h>
//#include <MsTimer2.h>
#include <TimedAction.h>

#define MOSI 11
#define SPICLOCK 13
#define LDAC 10
#define SELECT 9

#define LED1 2
#define LED2 8
#define LED3 3
#define LED4 4
#define LED5 5
#define LED6 6
#define LED7 7

int incomingByte = 0;
int analogVal[4];
byte data[34];
unsigned long time;

TimedAction sendData = TimedAction(NO_PREDELAY, 100, ADCout);

void setup()
{  
  analogReference(EXTERNAL);
  Serial.begin(115200);
  
  // LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);  
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  
  // SPI
  pinMode(MOSI, OUTPUT); // MOSI
  pinMode(SPICLOCK, OUTPUT); // Clock
  pinMode(LDAC, OUTPUT); // LDAC
  digitalWrite(LDAC, HIGH);
  pinMode(SELECT, OUTPUT); // Select
  digitalWrite(SELECT, HIGH);
  SPI.mode(1 << CPHA); // send SPI data on the falling edge of SCLK
  delay(10);
  for (int i=0; i<4; i++) // write zeros to DAC
  {
    digitalWrite(SELECT, LOW); // enable DAC input register
    Spi.transfer((char)(i<<6)); // send MSB
    Spi.transfer(0);
    digitalWrite(SELECT, HIGH); // disable DAC input register    
  }
  digitalWrite(LDAC, LOW); // update DAC outputs
  delay(1); // 1 ms delay
  digitalWrite(LDAC, HIGH); // release LDAC
  
  // Timer
  // MsTimer2::set(100, ADCout); // every 100 ms
  sendData.disable();
  
  for (int l=0; l<3; l++)
  {
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    delay(100);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    delay(100);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, HIGH);
    delay(100);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, HIGH);
    delay(100);
    digitalWrite(LED5, LOW);
    digitalWrite(LED6, HIGH);
    delay(100);
    digitalWrite(LED6, LOW);
    digitalWrite(LED7, HIGH);
    delay(100);
    digitalWrite(LED7, LOW);
  }
}

void loop()
{
  if (Serial.available() > 1)
  {
    incomingByte = Serial.read();
    if (incomingByte == 0x55)
    {
      incomingByte = Serial.read();
      if (incomingByte == 0xAA)      
      {
        do
        {
          delayMicroseconds(1);
        }
        while (Serial.available() == 0);
        incomingByte = Serial.read();
        switch(incomingByte)
        {
        case 0x30:  // read voltages
          ADCout(); 
          break;
        case 0x31:
          setCurrent(); // set current
          break;
        case 0x32:
          setLEDs(); // enable/disable LED1...LED7
          break;
        case 0x33:  // start auto-measurements every 100 ms
          time = millis();
          sendData.enable();
          break;
        case 0x34:  // stop auto-measurements
          sendData.disable();
          break;
        case 0x40: 
          EEPROMread(); // read EEPROM
          break;
        case 0x41: 
          EEPROMwrite();  //write EEPROM
          break;
        }
      }
    }
  }
  // send ADC data every 100 ms
  sendData.check();
}

void ADCout()
{
  unsigned short i;
  for (i=0; i<4; i++)
  {
    analogVal[i] = 0;   
  } 
  for (unsigned short j=0; j<4; j++)
  { 
    for (i=0; i<4; i++)
    {
      analogVal[j] += analogRead(j);   
    }
    analogVal[j] >>= 2;
  }  
  
  Serial.print(0x55, BYTE);
  unsigned long ctime;
  ctime = millis() - time;
  for (i=0; i<4; i++)
  {
      Serial.write(lowByte(ctime));
      ctime >>= 8;
  }
  for (i=0; i<4; i++)
  {
    Serial.write(lowByte(analogVal[i]));    
    Serial.write(highByte(analogVal[i]));    
  }
}

void EEPROMread()
{
  Serial.write(0x55);
  // 1st & 2nd bytes - firmware version
  // 4 bytes - float
  // 4 channels, voltage and current, 12 bytes each channel
  // 2 + 12*4  = 50 bytes total (BLOCKSIZE)
  for (int i=0; i<50; i++)
  {
    Serial.write(EEPROM.read(i));
  }
}

void EEPROMwrite()
{
  int i = 0;
  int j = 0;
  do
  {
    if (Serial.available() > 0)
    {
      data[i] = Serial.read();
      i++;
    }
    delayMicroseconds(10);
    j++;
    if (j>10000) // 100 ms
    {
      // timeout
      Serial.write(0x31);
      return;
    }
  }
  while (i<50);
  // write received bytes to the EEPROM
  for (int j=0; j<50; j++)
  {
    EEPROM.write(j, data[j]);
  }
}

void setCurrent(void)
{
  byte i=0;
  int j=0;
  do
  {
    if (Serial.available() > 0)
    {
      data[i] = Serial.read();
      i++;
    }
    delayMicroseconds(1);
    j++;
    if (j>10000) // 10 ms timeout
    {
      return;
    }
  }
  while (i<8); // 4 channels * 2 bytes each
  
  for (i=0; i<4; i++)
  {
    data[2*i+1] += (char)(i<<6); // DAC output address - first (most significant) two bits
    digitalWrite(SELECT, LOW); // enable DAC input register
    Spi.transfer(data[2*i+1]); // send MSB
    Spi.transfer(data[2*i]); // send LSB
    digitalWrite(SELECT, HIGH); // disable DAC input register
    delayMicroseconds(100);
  }
  digitalWrite(LDAC, LOW); // update DAC outputs
  delay(1); // 1 ms delay
  digitalWrite(LDAC, HIGH); // release LDAC
}

void setLEDs(void)
{
  byte leds;
  int i=0;
  int j=0;
  do
  {
    if (Serial.available() > 0)
    {
      leds = Serial.read();
      break;
    }
    delayMicroseconds(10);
    j++;
    if (j>1000) // 10 ms
    {
      // timeout
      return;
    }
  }
  while (true); // 1 byte
  // enable/disable status LEDs
  digitalWrite(LED7, bitRead(leds, 6));
  digitalWrite(LED6, bitRead(leds, 5));
  digitalWrite(LED5, bitRead(leds, 4));
  digitalWrite(LED4, bitRead(leds, 3));
  digitalWrite(LED3, bitRead(leds, 2));
  digitalWrite(LED2, bitRead(leds, 1));
  digitalWrite(LED1, bitRead(leds, 0));
}

