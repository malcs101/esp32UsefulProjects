/*

  ESP32 Timer and ISR to allow user to insert events to be executed at 10secs, 1sec, 100ms, 10ms
  m.stitt 2/4/23
  
 */

#define DEBUG  1 // comment this out to avoid serial debug messages

hw_timer_t * timer = NULL;
volatile bool timer10000msTick = false;
volatile bool timer1000msTick = false;
volatile bool timer100msTick = false;
volatile bool timer10msTick = false;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t Millisecond10000Counter = 0;
volatile uint32_t Millisecond1000Counter = 0;
volatile uint32_t Millisecond100Counter = 0;


void setup() {
  Serial.begin(115200);
  // Timer setup code. Don't touch!
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();
  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);
  // Set alarm to call onTimer function every 10 milliseconds (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 10000, true);
  // Start an alarm
  timerAlarmEnable(timer);
  
  //////////////// user setup code goes below /////////////////////




  //////////////// user setup code end        /////////////////////

}

void loop() {
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    portEXIT_CRITICAL(&timerMux);

    // Don't use delay() statements!!

    if(timer10000msTick)
    {
    // Put 10 second code below here
    #ifdef DEBUG 
      Serial.println(); Serial.println("10 second"); 
    #endif  
 
    
    }
 
    if(timer1000msTick)
    {
    ////////////// Put 1 second code below here
    #ifdef DEBUG
      Serial.println(); Serial.println("1 second"); 
    #endif  
     }
    
    if(timer100msTick)
    {
    ////////////// Put 100 millisecond code below here
    #ifdef DEBUG 
      Serial.print(".");
    #endif
          
    }
    
    if(timer10msTick)
    {
    ////////////// Put 10 millisecond code below here
      
    }


    //reset all interrupt semiphores
    portENTER_CRITICAL(&timerMux);
    timer10000msTick = false;
    timer1000msTick = false;
    timer100msTick = false;
    timer10msTick = false;
    portEXIT_CRITICAL(&timerMux);


    
  }
}





//Interrupt service routine that fires every 10 millisecond and keeps track of 10 second, 1 second, 100ms, and 10ms timers
//No need to touch code below
void ARDUINO_ISR_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);

  Millisecond10000Counter+=10;
  Millisecond1000Counter+=10;
  Millisecond100Counter+=10;


  if(Millisecond10000Counter == 10000 )
  {
    timer10000msTick = true;
    Millisecond10000Counter = 0;
  }
  if(Millisecond1000Counter == 1000)
  {
    Millisecond1000Counter = 0;
    timer1000msTick = true;
  }
  if(Millisecond100Counter == 100)
  {
    Millisecond100Counter = 0;
    timer100msTick = true;
  }

  // Timer is every 10 milliseconds so this will always be set to true
  timer10msTick = true;

  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

 
