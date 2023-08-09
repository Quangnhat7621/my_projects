// Import some important libraries
#include <Arduino_FreeRTOS.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
// outut and input pin configuration 
#define motor 4
#define certain 10
#define rainSensor 3
#define HumSensor A3
// Declare two global variavles in order to tranfer data in other tasks
int globalHumidity;
boolean globalStatus;
// LDC Definition
LiquidCrystal_I2C lcd(0x27,16,2);
// Servo Definition
Servo myservo;

//Declare Task
void TaskCareObject(void *pvParameters);
void TaskWeatherDetect(void *pvParameters);
void TaskPrint(void *pvParameters);

void setup() {
Serial.begin(9600);

// Declare in, out direction 
pinMode(rainSensor, INPUT);
pinMode(certain, OUTPUT);
pinMode(motor, OUTPUT);
// initialize LCD
lcd.init();
// turn on led on screen
lcd.backlight();
// Assign servo pin 
myservo.attach(certain);
// Erase screen
lcd.clear();

// Create Task
xTaskCreate(
  TaskCareObject
  ,"Task1"
  ,128
  ,NULL
  ,1
  ,NULL
  );

xTaskCreate(
  TaskWeatherDetect
  ,"Task2"
  ,128
  ,NULL
  ,1
  ,NULL
  );

xTaskCreate(
  TaskPrint
  ,"Task3"
  ,128
  ,NULL
  ,1
  ,NULL
  );

// schdule
vTaskStartScheduler();
}

void loop() {
  // put your main code here, to run repeatedly:

}

// check moisture and controll the pump 
void TaskCareObject(void *pvParameters)
{ 
  int temp =0;
  while(1)
  {
    // convert analog signal to moisture percent
    temp = analogRead(HumSensor);
    temp = abs(temp - 420);
    temp = abs(600 - temp);
    temp = map(temp, 0, 605, 0, 100);
    globalHumidity = temp;
    // if the moisture smaller than 70%, turn on the pump
    if(temp < 70)
    {
      digitalWrite(motor, HIGH);
      vTaskDelay(10/ portTICK_PERIOD_MS);
      }
      else // when condition is wrong, then turn of the pump
      {
        digitalWrite(motor, LOW);
        vTaskDelay(10/ portTICK_PERIOD_MS);
        }

    }
  }
// detect rain and controll the servo
void TaskWeatherDetect(void *pvParameters)
{
  boolean _status = 0;

  while(1)
  { 
    _status = digitalRead(rainSensor);
    globalStatus = _status;
    if(_status)
    {

      myservo.write(180);
      }
      else
      {
        myservo.write(0);
        }
    }
  }

// wire moisture and weather on screen
void TaskPrint(void *pvParameters)
{
  while(1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Humidity:");
    lcd.print(globalHumidity);
    lcd.print("%");
    lcd.setCursor(0,1);
    if(!globalStatus)
      {
        lcd.print("It's rain");
        }
        else
        {
          lcd.print("It is not rain");
          }
    vTaskDelay(500/ portTICK_PERIOD_MS);
    }
  }
