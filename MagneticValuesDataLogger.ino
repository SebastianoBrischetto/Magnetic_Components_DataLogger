#include "MPU9250.h"
#include <FileIO.h>
#include <LiquidCrystal.h>

MPU9250 mpu;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
const int BUTTON_PIN = 4;
uint32_t currentMillis;
bool isOn = true;
bool prevButtonStatus = false;
char url[] = "/www/log/datalog.txt";

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  lcd.begin(16, 2);
  PrintLcd("Preparazione","dispositivo");
  Serial.begin(9600);
  Bridge.begin();
  Wire.begin();
  FileSystem.begin();
  CleanFile();
  CheckSensorConnection();
  //Calibrate();
  PrintLcd("Dispositivo","Connesso");
  delay(2500);
  currentMillis = millis();
}

void loop() {
  CheckButton();
  if(isOn){
    if (mpu.update()) {
      if (millis() > currentMillis + 100) {
        PrintMagValues();
        currentMillis = millis();
      }
    }
  }else{
    delay(100);
  }
}

void CleanFile() {
  File dataFile = FileSystem.open(url, FILE_WRITE);
  dataFile.println("X;Y;Z");
  dataFile.close();
}

void CheckButton(){
  bool buttonStatus = digitalRead(BUTTON_PIN);
  if(buttonStatus && buttonStatus != prevButtonStatus){
    if(!isOn){
      isOn = true;
      File dataFile = FileSystem.open(url, FILE_APPEND);
      dataFile.print("-");
      dataFile.println("X;Y;Z");
      currentMillis = millis();
    }
    else{
      isOn = false;
      PrintLcd("Pausa, ripremere","per continuare");
    }
  }
  prevButtonStatus = buttonStatus;
}

void PrintLcd(String firstLine, String secondLine){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(firstLine);
  lcd.setCursor(0,1);
  lcd.print(secondLine);
}

void PrintMagValues() {
  int x, y, z;
  x = mpu.getMagX();
  y = mpu.getMagY();
  z = mpu.getMagZ();
  String dataString;
  dataString += String(x) + ";" + String(y) + ";" + String(z);
  Serial.println(dataString);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("X: ");
  lcd.setCursor(3,0);
  lcd.print(x);
  lcd.setCursor(0,1);
  lcd.print("Y: ");
  lcd.setCursor(3,1);
  lcd.print(y);
  lcd.setCursor(8,1);
  lcd.print("Z: ");
  lcd.setCursor(11,1);
  lcd.print(z);
  File dataFile = FileSystem.open(url, FILE_APPEND);
  if(dataFile){
    dataFile.println(dataString);
    dataFile.close();
  }
  else{
    Serial.println("error opening datalog.txt");
  }

}

void CheckSensorConnection() {
  if (!mpu.setup(0x68)) {
    while (1) {
      Serial.println("MPU connection failed.");
      delay(5000);
    }
  }
}

void Calibrate() {
  PrintLcd("Calibrazione","Giro/Accel");
  Serial.println("Accel/Gyro calibration will start in 5sec.");
  delay(2500);
  PrintLcd("Non muovere il","dispositivo");
  Serial.println("Please leave the device still on the flat plane.");
  delay(2500);
  mpu.verbose(true);
  mpu.calibrateAccelGyro();
  mpu.setMagneticDeclination(0.067);
  PrintLcd("Calibrazione","Magnetometro");
  Serial.println("Mag calibration will start in 5sec.");
  delay(2500);
  PrintLcd("Traccia un 8","col dispositivo");
  Serial.println("Please Wave device in a figure eight until done.");
  delay(2500);
  mpu.calibrateMag();
  PrintLcd("Calibrazione","effettuata");
  Serial.println("Calibration done");
  mpu.verbose(false);
}
