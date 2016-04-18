// SensorGlove - Sensor Handschuh


//================================================================================================================================================
//================================================================================================================================================

//=========                                                       Vorbereitungen                                                     =============

//================================================================================================================================================
//================================================================================================================================================



//Libraries einbetten:
//************************************************************
  #include <SGMux.h>            //Analog-Muxer Library
  #include <Wire.h>             //Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation is used in I2Cdev.h
  #include <I2Cdev.h>           //I2Cdev and MPU9150 must be installed as libraries, or else the .cpp/.h files for both classes must be in the include path of your project
  #include <MPU9150.h>



//Libraries initialisieren
//***********************************************************
  SGMux mux(A3, 4, 7, 8, 12, 13); //SGMux(int Sig, int En, int S0, int S1, int S2, int S3)
  // class default I2C address is 0x68, specific I2C addresses may be passed as a parameter here
  // AD0 low = 0x68 (default for InvenSense evaluation board)
  // AD0 high = 0x69
  MPU9150 accelgyro;




//------------------------------------------------------------------------------------------------------------------




//Parameter einstellen
//***********************************************************

  const bool bluetooth=true;   //Kommunikation über Bluetooth und/oder USB-Schnittstelle
  const bool serial=true;
  const int debug=false;      //debug, dann werden über serial und Bluetooth auch Feedback-Daten übermittelt
  const int imu=true;       //sollen IMU-Daten ausgelesen und angezeigt werden?
  
  const int feedback=1;        //Feedback-Methode - 0: kein Feedback, 1:Vibrationsmotor

  const int freq=25;




//Variablen definieren:
//***********************************************************

//Timervariablen
//----------------------
  unsigned long t_old=0;
  unsigned long t_new=0;
  int t_int=0;
  int t_batt=0;





//Sensorvariablen
//----------------------
  int flex11Val=0;
  int flex12Val=0;
  int flex21Val=0;
  int flex22Val=0;
  int flex31Val=0;
  int flex32Val=0;
  int flex41Val=0;
  int flex42Val=0;
  int flex51Val=0;
  int flex52Val=0;
  
  int fb1=0;
  int fb2=0;
  int fb3=0;
  int fb4=0;
  int fb5=0;

  
//Akku-anzeige - Variablen
//----------------------

  long batteryVal_old=0;
  int batteryVal=0;
  int bat_i=0;
  



//IMU-Variablen
//----------------------
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  int16_t mx, my, mz;

 

//Pins festlegen:
//***********************************************************

  const int batteryPin=A4;
  
  const int flex11Pin=0;
  const int flex12Pin=1;
  const int flex21Pin=2;
  const int flex22Pin=3;
  const int flex31Pin=4;
  const int flex32Pin=5;
  const int flex41Pin=6;
  const int flex42Pin=7;
  const int flex51Pin=8;
  const int flex52Pin=9;
  
  
  const int vib1Pin=5;
  const int vib2Pin=6;
  const int vib3Pin=9;
  const int vib4Pin=10;
  const int vib5Pin=11;



//____________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________

//================================================================================================================================================
//================================================================================================================================================

//=========                                                      Setup                                                               =============

//================================================================================================================================================
//================================================================================================================================================






void setup() {
  
//Serielle Kommunikation
//***********************
  com_begin();


//I2C Bus beginnen
//***********************
  Wire.begin();
 
 
//IMU-Vorbereitungen
//***********************
  imu_init();


//Pinvorbereitung
//***********************
  feedback_init();
  battery_init();


//Frequenzberechnung
//***********************
 t_int=1000/freq;
}





//____________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________

//================================================================================================================================================
//================================================================================================================================================

//=========                                                       Main Loop                                                          =============

//================================================================================================================================================
//================================================================================================================================================
  
  
void loop() {
 t_new=millis();
 if (t_new-t_old>=t_int) {
    t_old=t_new;

  
  //IMU
  //******************************
      
    imu_read();
    imu_print();
      
          
  //Flex-Sensoren Analog-Mux
  //********************************    
    mux.on();
    flex_read();
    mux.off();
    flex_print();



  //Feedback
  //********************************

    
    feedback_read();
    feedback_write();
    feedback_print();



  //Akku-Anzeige und Zeitstempel
  //********************************
    battery_read();
    if (t_batt<10) { t_batt++;}
    if (t_batt==10) {
      battery_print();
      t_batt=0;
    }
    
   com_println("millis: "+String(millis()));
  
  
 }
  
}





//____________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________


//================================================================================================================================================
//================================================================================================================================================

//=========                                                       Funktionen                                                     =============

//================================================================================================================================================
//================================================================================================================================================

/*Hier stehen alle im Sketch declarierten Funktionen:
    - Serielle Kommunikation
        *com_begin
        *com_print
        *com_println
        *com_printtab
        *com_write
        *com_available
        *com_read
        
   - Analog-Mux Sensoren auslesen
        *flex_read
        *flex_print
     
   - IMU-Funktionen
        *imu_print   
   
   - Feedback
        *feedback_init
        *feedback_read
        *feedback_write
        *feedback_print

*/




//Serielle Kommunikation
//***************************************************
//Bluetooth und/oder Serielle Übertragung möglich

void com_begin() {
  if (bluetooth==true)  {Serial1.begin(115200);}
  if (serial==true)     {Serial.begin(115200);}  
  }
  
void com_print(String cmd)  {  
  if (bluetooth==true)  {Serial1.print(cmd);}
  if (serial==true)     {Serial.print(cmd);}  
  }
  
void com_println(String cmd)  {  
  if (bluetooth==true)  {Serial1.println(cmd);}
  if (serial==true)     {Serial.println(cmd);}  
  }
  
void com_printtab(String cmd)  {  
  if (bluetooth==true)  {Serial1.print(cmd); Serial1.print("\t");}
  if (serial==true)     {Serial.print(cmd); Serial.print("\t");} 
  }  
  
void com_write(int16_t cmd) {
  if (bluetooth==true)  {Serial1.write(cmd); Serial1.write("\t");}
  if (serial==true)     {Serial.write(cmd); Serial.write("\t");}
  }  

bool com_available() {
  bool i=false;
  bool t=false;
  if (bluetooth==true)  {i=Serial1.available();}
  if (serial==true)     {t= Serial.available();}
  bool k=i&t;
  return k;
  }
  
String com_read() {
  String cmd="";
  if (bluetooth==true)  {
    while (Serial1.available()) {
      cmd=cmd+char(Serial1.read());}
   }
   if (serial==true) {    
    while (Serial.available()) {
      cmd=cmd+char(Serial.read());}
    }
   return cmd;
  }


//Analog-Mux Sensor-Auslesen
//*******************************

  void flex_read() {
    flex11Val=mux.read(flex11Pin);
    flex12Val=mux.read(flex12Pin);
    flex21Val=mux.read(flex21Pin);
    flex22Val=mux.read(flex22Pin);
    flex31Val=mux.read(flex31Pin);
    flex32Val=mux.read(flex32Pin);
    flex41Val=mux.read(flex41Pin);
    flex42Val=mux.read(flex42Pin);
    flex51Val=mux.read(flex51Pin);
    flex52Val=mux.read(flex52Pin);

    flex11Val=map(flex11Val,450,700,0,1000);
    flex12Val=map(flex12Val,450,700,0,1000);
    flex21Val=map(flex21Val,450,700,0,1000);
    flex22Val=map(flex22Val,450,700,0,1000);
    flex31Val=map(flex31Val,450,700,0,1000);
    flex32Val=map(flex32Val,450,700,0,1000);
    flex41Val=map(flex41Val,450,700,0,1000);
    flex42Val=map(flex42Val,450,700,0,1000);
    flex51Val=map(flex51Val,450,700,0,1000);
    flex52Val=map(flex52Val,450,700,0,1000);
    }
    
  void flex_print() {
    com_printtab("flex11: "+String(flex11Val)+";");
    com_printtab("flex12: "+String(flex12Val)+";");
    com_printtab("flex21: "+String(flex21Val)+";");
    com_printtab("flex22: "+String(flex22Val)+";");
    com_printtab("flex31: "+String(flex31Val)+";");
    com_printtab("flex32: "+String(flex32Val)+";");
    com_printtab("flex41: "+String(flex41Val)+";");
    com_printtab("flex42: "+String(flex42Val)+";");
    com_printtab("flex51: "+String(flex51Val)+";");
    com_printtab("flex52: "+String(flex52Val)+";");
  }





//IMU-Funktionen
//*******************************

  void imu_init() {
    if (imu==true) {
      accelgyro.initialize();
    }
  }

  void imu_read() {
    if (imu==true) {
      // read raw accel/gyro measurements from device
      accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
    }
  }

  void imu_print() {
    if (imu==true) {
      com_printtab("acc:  ,"+String(ax)+",");
      com_printtab(","+String(ay)+",");
      com_printtab(","+String(az)+";");
      com_printtab("gyro:  ,"+String(gx)+",");
      com_printtab(","+String(gy)+","); 
      com_printtab(","+String(gz)+";");
      com_printtab("magn:  ,"+String(mx)+",");
      com_printtab(","+String(my)+",");
      com_printtab(","+String(mz)+";");
    }
  }



//Feedback
//*******************************
// 0:kein Feedback, 1:Vibrationsmotor

  void feedback_init() {
    switch (feedback) {
      
      case 0:
        break;  
      
      case 1:
        pinMode(vib1Pin, OUTPUT);
        pinMode(vib2Pin, OUTPUT);
        pinMode(vib3Pin, OUTPUT);
        pinMode(vib4Pin, OUTPUT);
        pinMode(vib5Pin, OUTPUT);
        break;
    }
  } 

    
  int feedback_read() {
  
    if (Serial.available() or Serial1.available()){
    String cmd=com_read();
    int i11, i12, i21, i22, i31, i32, i41, i42, i51, i52=0;
    i11=cmd.indexOf("fb1:");
    i12=cmd.indexOf(";",i11);
    i21=cmd.indexOf("fb2:");
    i22=cmd.indexOf(";",i21);
    i31=cmd.indexOf("fb3:");
    i32=cmd.indexOf(";",i31);
    i41=cmd.indexOf("fb4:");
    i42=cmd.indexOf(";",i41);
    i51=cmd.indexOf("fb5:");
    i52=cmd.indexOf(";",i51);
    String fb1_str=cmd.substring(i11+5,i12);
    fb1=fb1_str.toInt();
    String fb2_str=cmd.substring(i21+5,i22);
    fb2=fb2_str.toInt();
    String fb3_str=cmd.substring(i31+5,i32);
    fb3=fb3_str.toInt();
    String fb4_str=cmd.substring(i41+5,i42);
    fb4=fb4_str.toInt();
    String fb5_str=cmd.substring(i51+5,i52);
    fb5=fb5_str.toInt();
   }      
  }

    
  void feedback_write()  {
    switch (feedback) {
      
      case 0:
        break;  
      
      case 1:
        //if ((fb1_old==fb1) and (fb2_old==fb2) and (fb3_old==fb3) and (fb4_old==fb4) and (fb5_old==fb5)) { ;}
        //else {
        analogWrite(vib1Pin, map(fb1,0,1000,0,255));
        analogWrite(vib2Pin, map(fb2,0,1000,0,255));
        analogWrite(vib3Pin, map(fb3,0,1000,0,255));
        analogWrite(vib4Pin, map(fb4,0,1000,0,255));
        analogWrite(vib5Pin, map(fb5,0,1000,0,255));
        break;    
    }
   } 
    
   void feedback_print() {
    if (debug==true) {
    com_print("fb1="+String(fb1));
    com_print(" .. fb2="+String(fb2));  
    com_print(" .. fb3="+String(fb3));
    com_print(" .. fb4="+String(fb4));
    com_printtab(" .. fb5="+String(fb5));
    }  
  }



//Akku-Anzeige
//*******************************
// misst die Spanung des Akkus und kann somit den aktuellen Ldezustand abschätzen (nur grobe Information, keine Garantie auf Funktionsfähigkeit!)



  void battery_init() {
    pinMode(batteryPin,INPUT); 
  }

  void battery_read() {
    if (bat_i<300) {
      batteryVal_old=batteryVal_old+analogRead(batteryPin);
      bat_i=bat_i+1;
    }
    if (bat_i==300) {
      bat_i=0;
      batteryVal=batteryVal_old/300;
      if (batteryVal>=770) {batteryVal=map(batteryVal,750,855,30,100);}                  //Batterie-Spannung zwischen 6V und 8,4V. D.h. Pro Zelle (die hier gemessen wird) 3V und 3,7V = 630 bis 855 /1023
      else if (batteryVal<770) {batteryVal=map(batteryVal,630,750,0,30);}
      if (batteryVal<0) {batteryVal=0;}
      else if (batteryVal>100) {batteryVal=100;}
      batteryVal=5*int(batteryVal/5+0.5);
      batteryVal_old=0;
    }
   }

  void battery_print() {
    com_printtab("batt: "+String(batteryVal)+";");
  }
  
  


