/*Project: Smart Classroom
  Author: Phan Duy
  Version 1.0 December, 2013
*/

/*LIGHT SENSOR B1750-Arduino UNO R3 (I2C)
  VCC-5v
  GND-GND
  SCL-SCL(analog pin5)
  SDA-SDA(analog pin4)
  ADD-NC
*/

/*Temperature_Humidity DHT11-Arduino UNO R3
  VCC (3V-5V)
  GND-GNE
  DATA OUT - PIN2
 */
 
/*IRremote-Arduino UNO R3
  RECV_PIN-PIN11
  SEND-PIN3
*/
  
/*RF-HC11(UART)-Arduino UNO R3
  3.3V
  RXD-TXD(PIN1)
  TXD-RXD(PIN0)

*/
 /*SoftwareSerial
 RXD-PIN9
 TXD-PIN10
*/
 
#include <Wire.h>//BH1750 I2C
#include <math.h>
#include <idDHT11.h>
#include <IRremote.h>
#include <SoftwareSerial.h>

//Declare SofrwareSerial
SoftwareSerial softUART(9,10);

//B1750
int B1750_Slave_address = 0x23; //setting i2c address

//DHT11
int idDHT11pin = 2;// Digital pin for communications
int idDHT11intNumber = 0; //interrupt number (must be the one that use the previus defined pin (see table above)
//declaration
void dht11_wrapper();// must be declared before the lib initialization
//Lib instantiate
idDHT11 DHT11(idDHT11pin, idDHT11intNumber,dht11_wrapper);
byte buff[2];

//IRremote
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;
int codeType = -1;

void setup(){
  Wire.begin();
  Serial.begin(9600);
  softUART.begin(9600);
  irrecv.enableIRIn(); //Start the reveiver
}

// This wrapper is in charge of calling 
// must be defined like this for the lib work
void dht11_wrapper(){
  DHT11.isrCallback();
}

//Read BH1750 value
int BH1750_Read(int address){
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
  return i;
}

//Init connection to BH1750
void BH1750_Slave_Init(int address){
  Wire.beginTransmission(address);
  Wire.write(0x10); //llx resolution 120ms
  Wire.endTransmission();  
}

//Detect type of IR Code
void detect_type(decode_results *results){
  codeType = results->decode_type;
  if (codeType == UNKNOWN){
      Serial.println("Received Unknown Code");
    } else if (codeType == NEC){
      Serial.println("Received NEC:");
    }else if (codeType == SONY){
      Serial.println("Received SONY:");
    }else if (codeType == RC5){
      Serial.println("Received RC5:");
    }else if (codeType == RC6){
      Serial.println("Received RC6:");
    }else {
      Serial.print("Unexpected codeType ");
    }
}

//Function send IR Code
int send_code(unsigned long code){
  for(int i=0; i<3; i++){
    irsend.sendNEC(code, 32);
  }  
}


//main program
void loop(){
  char buf_tem[2], buf_hum[2], buf_send[7];
  float tem, hum;
  char c;
  String buf_lig;
  uint16_t val = 0;
  int i = 0;
  boolean learn_code = true;
  while(softUART.available()>0){
    c = softUART.read();
    //get sensor info i=info
    if(c == 'i'){
      //read then send light value
       BH1750_Slave_Init(B1750_Slave_address);
       delay(200);
       //read then send light value
       if(BH1750_Read(B1750_Slave_address) == 2){
        val = ((buff[0]<<8) |buff[1])/1.2;
        buf_lig = String(val);
        Serial.print("Light:");
        Serial.println(buf_lig); 
      } 
      
      //read then send tem-hum value
//      delay(250);
      int result = DHT11.acquireAndWait();
      delay(200);
      hum = DHT11.getHumidity();
      delay(100);
      tem = DHT11.getCelsius();
      delay(100);
      //dtostrf() - convert float to string
      //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
      dtostrf(tem, 2,0, buf_tem);
//      Serial.print("Temperature:");
//      Serial.println(buf_tem);
      dtostrf(hum, 2,0, buf_hum);
//      Serial.print("Humidity:");
//      Serial.println(buf_hum); 
      while(i<2){
        buf_send[i] = buf_tem[i];
        buf_send[i+2] = buf_hum[i];
        buf_send[i+4] = buf_lig[i];
        i++;
      }
      i = 0;
      while(i<3){
        buf_send[i+4] = buf_lig[i];
        i++;
      }
      Serial.write(buf_send);
      //send sensor value to server   
      softUART.write(buf_send);
       
    }
    
    //learn IR code - c = code
    if(c == 'c'){
      while(learn_code){
        if(Serial.available()>0){
          c = Serial.read();
          //learn button code
          if(c = 'p'){
            if(irrecv.decode(&results)){
              detect_type(&results);
              //store IR Code
              //...............//
              irrecv.resume();//Receive the next value
            }
          }
          
          //complete learn code
          if(c = 'c'){
            learn_code = false;
          }
        }
      }
    }
    
    //send IR code
    if(c = 'w'){
      //call Function send IR code
    }
  }
  
}
