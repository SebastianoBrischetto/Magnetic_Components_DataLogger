//Put here sensor libraries and variables
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5
const int minFrequency = 220;
//end of sensor libraries and variables

#include <FileIO.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
char url[] = "/www/log/datalog.txt";
uint32_t startMeasurementMillis;
const byte BUTTON_PIN = 4;
int frequency;
int mode;
char csvSeparator;
bool isOn = false;
bool prevButtonStatus = false;

//Put here how to setup your sensor
void SetupSensor(){
  //Initialize LSM9DS1
  if (!lsm.begin())
  {
    PrintLcd("Impossibile", "Usare sensore");
    while (1);
  }

  // 1.) Set the accelerometer range
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);

  // 3.) Setup the gyroscope
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
}

//Put here how to Measure with your sensor (don't forget to use PrintValues() to print measurements)
void Measure() {
  PrintValues();
  delay(frequency);
}

//Initialize sampling variables here (if needed)
void CheckButton() {
  bool buttonStatus = digitalRead(BUTTON_PIN);
  if (buttonStatus && buttonStatus != prevButtonStatus) {
    if (!isOn) {
      isOn = true;
      CheckBridge();
      File dataFile = FileSystem.open(url, FILE_APPEND);
      dataFile.print("-Parametri: Campionamento ");
      dataFile.print(frequency);
      dataFile.print(" Modalita ");
      dataFile.println(mode);
      dataFile.println("Time[ms];X;Y;Z");
      delay(1000);
      //Put time variables for sampling here
      //end of sampling variables
      startMeasurementMillis = millis();
    } else {
      isOn = false;
      PrintLcd("Pausa, ripremere", "per continuare");
    }
  }
  prevButtonStatus = buttonStatus;
}

//Change how you get x, y, z values
void PrintValues(){
  //Put here how to get x, y, z values
  lsm.read();
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp); 
  float x, y, z;
  y = m.magnetic.x;
  x = m.magnetic.y;
  z = m.magnetic.z;
  //end of how to get x, y, z values
  uint32_t MeasurementMillis;
  MeasurementMillis = millis() - startMeasurementMillis;
  File dataFile = FileSystem.open(url, FILE_APPEND);
  if (dataFile) {
    dataFile.print(MeasurementMillis);
    dataFile.print(csvSeparator);
    dataFile.print(x);
    dataFile.print(csvSeparator);
    dataFile.print(y);
    dataFile.print(csvSeparator);
    dataFile.println(z);
    dataFile.close();
  } else {
    PrintLcd("Errore apertura", "File");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("X: ");
  lcd.setCursor(3, 0);
  lcd.print(x);
  lcd.setCursor(8, 0);
  lcd.print("Y: ");
  lcd.setCursor(11, 0);
  lcd.print(y);
  lcd.setCursor(0, 1);
  lcd.print("Z: ");
  lcd.setCursor(3, 1);
  lcd.print(z);
  lcd.setCursor(8, 1);
  lcd.print("s: ");
  lcd.setCursor(11, 1);
  lcd.print(MeasurementMillis/1000);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  lcd.begin(16, 2);
  PrintLcd("Preparazione", "dispositivo");
  Bridge.begin();
  Wire.begin();
  FileSystem.begin();
  CleanFile();
  SetupSensor();  
  Bridge.put("frequency",String(minFrequency));
  Bridge.put("mode","1");
  Bridge.put("csvSeparator",";");
  PrintLcd("Dispositivo", "Connesso");
  delay(2500);
  PrintLcd("Premere", "per iniziare");
}

void loop() {
  CheckButton();
  if (isOn) {
    Measure();
  } else {
    delay(100);
  }
}

void CleanFile() {
  File dataFile = FileSystem.open(url, FILE_WRITE);
  dataFile.println("Misuratore delle componenti del campo magnetico.");
  dataFile.close();
}

void PrintLcd(char firstLine[], char secondLine[]) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
}

void CheckBridge(){
  char buffer[100]={0};
  Bridge.get("frequency",buffer,99);
  frequency = atoi(buffer) - minFrequency;
  Bridge.get("mode",buffer,99);
  mode = atoi(buffer);
  Bridge.get("csvSeparator",buffer,99);
  csvSeparator = buffer[0];
}
