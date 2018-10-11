#include <stdio.h>
#include <string.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <virtuabotixRTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>

/*Define relays ports*/
#define in1 22
#define in2 23
#define humidifier 24
#define desumidifier 25

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celcius=0;

/* Declare Buttons pins*/
#define btnSet 0 // Interrupt 0 is hardware pin 4 (digital pin 2)
#define btnSel 1 // Interrupt 1 is hardware pin 5 (digital pin 3)

#define DHT11_PIN 4
dht DHT;

/* Declare LCD settings*/
LiquidCrystal_I2C lcd(0x27,16,2);

// Interrupt state
int togBtnSet = false;
int togBtnSel = false;

// Time and date variables
int tmpMinutes = 0;
int tmpHours = 0;
int tmpWeekDay = 0;
int tmpMonthDay = 0;
int tmpMonth = 0;
int tmpYear = 0;

int counterVal = 1;
int menuCounter = 0;

// Variable to keep track of where we are in the "menu"
int myMenu[7]; // 1=Hour, 2=Minutes, 3=date, 4=Month, 5=Year, 6=DayOfWeek


/* Define array of the week day */
char* days[] = { "NA", "Mon", "Tue", "Wed", "Thu", "Fre", "Sat", "Sun" };

/* Create buffers */
char buf[50];
char day[10];

int numdata[7] ={ 0}, j = 0, mark = 0;

/* Declare RTC pins*/
uint8_t RST_PIN = 8; //RST pin attach to
uint8_t SDA_PIN = 7; //IO pin attach to
uint8_t SCL_PIN = 6; //clk Pin attach to

/* Define a DS1302 object */
virtuabotixRTC myRTC(SCL_PIN, SDA_PIN, RST_PIN); //If you change the wiring change the pins here also


/*Initialize*/
void setup() {
  Serial.begin(9600);
  attachInterrupt(btnSet, increaseValue, RISING);
  attachInterrupt(btnSel, nextItem, RISING);
  Wire.begin();
  sensors.begin();
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight

  setupLightRelay();
  setupHumidityRelays();
}

/*Initialize loop*/
void loop() {
/* print the current time */
  if (menuCounter == 0) {
      print_time();
      verifyLight();
      verifyHumidity();
  }
  if (menuCounter == 7) {
      print_time();
  }
  if (menuCounter == 1) {
    setHour();
  }
  if (menuCounter == 2) {
    setMinute();
  }
  if (menuCounter == 3) {
    setDate();
  }
  if (menuCounter == 4) {
    setMonth();
  }
  if (menuCounter == 5) {
    setYear();
  }
  if (menuCounter == 6) {
    setDOW();
  }
}

// Interrupt function
void increaseValue() {
// Variables
  static unsigned long lastInterruptTime = 0;
// Making a timestamp
  unsigned long interruptTime = millis();

// If timestamp - lastInterruptTime is greater than 200
  if (interruptTime - lastInterruptTime > 200)
  {
// Toggle the variable
    togBtnSet = !togBtnSet;
// Increase the counterVal by 1
    counterVal++;
  }
// Setting lastInterruptTime equal to the timestamp
// so we know we moved on
  lastInterruptTime = interruptTime; 
}

// Next menuItem Interrupt function
void nextItem() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 200)
  {
    togBtnSel = !togBtnSel;
// Increase the menu counter so we move to next item
    menuCounter++;   
// Placing the counterVal in the menucounters array position
    myMenu[menuCounter] = counterVal;
// Reset counterVal, now we start at 0 on the next menuItem
    if (menuCounter == 1) {
      counterVal = 1;
    }
    if (menuCounter == 2) {
      counterVal = 0;
    }
    if (menuCounter == 3) {
      counterVal = 1;
    }
    if (menuCounter == 4) {
      counterVal = 1;
    }
    if (menuCounter == 5) {
      counterVal = 0;
    }
    if (menuCounter == 6) {
      counterVal = 1;
    }
    if (menuCounter == 7) {
      counterVal = 0;
    }

  }
  lastInterruptTime = interruptTime;
}

// Funcion to set the hour
void setHour() {
  lcd.setCursor(0,0);
  lcd.print("Set hour.       "); 
  lcd.setCursor(0,1);
  lcd.print("                ");
 // Checks if interrupt has occured = button pressed
 if (togBtnSet)
 {
  // Update array value with counterVal
  if (counterVal > 24 ){
    counterVal = 1;
    myMenu[menuCounter] = 1;
  } else {
    myMenu[menuCounter] = counterVal;  
  }
  lcd.setCursor(7,1);
  // Print the new value
  lcd.print(myMenu[menuCounter]); lcd.print("  ");
 }
 else
 {
  // Update array value with counterVal
  if (counterVal > 24 ){
    counterVal = 0;
    myMenu[menuCounter] = 0;
  } else {
    myMenu[menuCounter] = counterVal;  
  }
  lcd.setCursor(7,1);
  // Print the new value
  lcd.print(myMenu[menuCounter]); lcd.print("  ");
 }
}

// Function to set minutes
void setMinute() {
  lcd.setCursor(0,0);
  lcd.print("Set minute.     ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  if (togBtnSet) {
    if (counterVal > 59 ){
      counterVal = 0;
      myMenu[menuCounter] = 0;
    } else {
      myMenu[menuCounter] = counterVal;  
    }
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }
  else {
    if (counterVal > 59 ){
      counterVal = 0;
      myMenu[menuCounter] = 0;
    } else {
      myMenu[menuCounter] = counterVal;  
    }

    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }
}

// Function to set date
void setDate() {
  lcd.setCursor(0,0);
  lcd.print("Set date.       ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  if (togBtnSet)
  {
    if (counterVal > 31 ){
      counterVal = 1;
      myMenu[menuCounter] = 1;
    } else {
      myMenu[menuCounter] = counterVal;  
    }
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }
  else
  {
    if (counterVal > 31 ){
      counterVal = 1;
      myMenu[menuCounter] = 1;
    } else {
      myMenu[menuCounter] = counterVal;  
    }
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }  
}

// Function to set month
void setMonth() {
  lcd.setCursor(0,0);
  lcd.print("Set month.      ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  if (togBtnSet)
  {
     if (counterVal > 12 ){
      counterVal = 1;
      myMenu[menuCounter] = 1;
    } else {
      myMenu[menuCounter] = counterVal;  
    }
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }
  else
  {
    if (counterVal > 12 ){
      counterVal = 1;
      myMenu[menuCounter] = 1;
    } else {
      myMenu[menuCounter] = counterVal;  
    }
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }  
}

// Function to set year
void setYear() {
  lcd.setCursor(0,0);
  lcd.print("Set year.       ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  if (togBtnSet)
  {
  myMenu[menuCounter] = 2000 + counterVal;
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }
  else
  {
  myMenu[menuCounter] = 2000 + counterVal;
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }  
}

// Function to set the day of week
void setDOW() {
  lcd.setCursor(0,0);
  lcd.print("Set day (1=Mon).");
  lcd.setCursor(0,1);
  lcd.print("                ");

  if (togBtnSet) {
    if (counterVal > 7 ){
      counterVal = 1;
      myMenu[menuCounter] = 1;
    } else {
      myMenu[menuCounter] = counterVal;  
    }
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  } else {
    if (counterVal > 7 ){
      counterVal = 1;
      myMenu[menuCounter] = 1;
    } else {
      myMenu[menuCounter] = counterVal;  
    }
    lcd.setCursor(7,1);
    lcd.print(myMenu[menuCounter]); lcd.print("  ");
  }

  set_time(myMenu[1], myMenu[2], 0, myMenu[3], myMenu[4], myMenu[5], myMenu[6]);
}

// Function that convert decimal numbers to binary
byte decToBCD(byte val) {
  return ((val/10*16) + (val));
}

void set_time(int hours, int minutes, int seconds, int monthDay, int month, int year, int weekDay) {
  myRTC.setDS1302Time(00, minutes, hours, weekDay, monthDay, month, year); 
}

void print_time() {
/* Get the current time and date from the chip */
//  Time t = rtc.time();
/* Name the day of the week */
  myRTC.updateTime();
  memset(day, 0, sizeof(day));
  switch (myRTC.dayofweek)
  {
    case 1: 
    strcpy(day, "Su"); 
    break;
    case 2: 
    strcpy(day, "Mo"); 
    break;
    case 3: 
    strcpy(day, "Tu"); 
    break;
    case 4: 
    strcpy(day, "We"); 
    break;
    case 5: 
    strcpy(day, "Th"); 
    break;
    case 6: 
    strcpy(day, "Fr"); 
    break;
    case 7: 
    strcpy(day, "Sa"); 
    break;
  }
/* Format the time and date and insert into the temporary buffer */
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", myRTC.dayofweek, myRTC.year, myRTC.month, myRTC.dayofmonth, myRTC.hours, myRTC.minutes, myRTC.seconds);
/* Print the formatted string to serial so we can see the time */
//  Serial.println(buf);
  lcd.setCursor(0,0);
  lcd.print(myRTC.dayofmonth/10);
  lcd.print(myRTC.dayofmonth%10);
  lcd.print("/");
  lcd.print(myRTC.month/10);
  lcd.print(myRTC.month%10);
  lcd.print("/");
  lcd.print(myRTC.year-2000);
  lcd.print(" ");
  lcd.print(myRTC.hours);
  lcd.print(":");
  lcd.print(myRTC.minutes/10);
  lcd.print(myRTC.minutes%10);
  lcd.print(" ");
  lcd.print(day);
  lcd.setCursor(0,1);
  lcd.print("H:");
  DHT.read11(DHT11_PIN);
  lcd.print(DHT.humidity,0);
  lcd.print("% ");
  lcd.print("T:");
  sensors.requestTemperatures();
  lcd.print(sensors.getTempCByIndex(0),1);
}

void setupLightRelay() {
  pinMode(in1, OUTPUT);
  digitalWrite(in1, HIGH);
  verifyLight();
}

void setupACRelay() {
  pinMode(in2, OUTPUT);
//  verifyAC();
}

void setupHumidityRelays() {
  pinMode(humidifier, OUTPUT);
  pinMode(desumidifier, OUTPUT);
  digitalWrite(humidifier, HIGH);
  digitalWrite(desumidifier, HIGH);
  verifyHumidity();
}

void verifyLight() {
  Serial.print(myRTC.hours);
  Serial.print(" hour(s), ");
  Serial.print(myRTC.minutes);
  Serial.print(" minute(s)");
  Serial.println(" ");
//  delay (1000);
  
  const int OnHour = 15;
  const int OnMin = 35;
  const int OffHour = 15;
  const int OffMin = 40;

  if (myRTC.hours == OnHour && myRTC.minutes == OnMin) {
    Serial.println("LIGHT ON");
    digitalWrite(in1, LOW);
  } else if(myRTC.hours == OffHour && myRTC.minutes == OffMin) {
      Serial.println("LIGHT OFF");
      digitalWrite(in1, HIGH);
  } 
}

void verifyHumidity() {
  Serial.print(DHT.humidity);
  Serial.print("%");
  delay (1000);
  
  const int minHumidity = 62.00;
  const int maxHumidity = 69.00;
  
  if(DHT.humidity < minHumidity){
    digitalWrite(humidifier,LOW);
    Serial.println("Humidifier ON");
    digitalWrite(desumidifier,HIGH);
    Serial.println("Desumidifier OFF");
  } 
  
  else if(DHT.humidity >= minHumidity && DHT.humidity <= maxHumidity){
    digitalWrite(humidifier,HIGH);
    Serial.println("Humidifier OFF");
    digitalWrite(desumidifier,HIGH);
    Serial.println("Desumidifier OFF");
  } 
  
  else if(DHT.humidity > maxHumidity) {
    digitalWrite(humidifier,HIGH);
    Serial.println("Humidifuer OFF");
    digitalWrite(desumidifier,LOW);
    Serial.println("Desumidifer ON");
  }  
}
