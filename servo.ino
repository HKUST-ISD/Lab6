/********************************** README **********************************/
// This lab will have you utilize the MCU PWM peripherial to control some servo
// motors. As the arduino built in PWM API (analogWrite etc.) does not support
// modifying of the PWM output frequency, we are going to use low level MCU 
// register manipulation to wirte our own PWM servo motor driver.

// This is mostly a academic exercise, in  your projects, the usage of the
// Arduino servo library is recommended.

const uint8_t CHANNELS = 4;
const int pwmPins[CHANNELS] = {2, 3 , 11, 12};

void setupPWM();

void setup()
{

    setupPWM();

    /********************************** TASK 1 **********************************/
    // The PWM timer tick period is now set to 1us, and (REG_TCC0_PER *  PWM timer tick period)
    // determines the period of the PWM output, set the frequency of the PWM to 50Hz by writing
    // a value into REG_TCC0_PER:
    REG_TCC0_PER = ;

    /********************************** TASK 2 **********************************/
    // Read this wiki page : https://en.wikipedia.org/wiki/Servo_control
    // Attatch servo motors on pin 2 and pin 3.
    // Set the both motor's servo angle to 0 deg, 90 deg and 180 deg respectively,
    // delaying 2 seconds between angle changes
    // The CCBx register value corresponds to the pulsewidth in microseconds (us)
    REG_TCC0_CCB0 = 1000; // TCC0 CCB0 - set servo angle to 0 deg on D2
    REG_TCC0_CCB1 = 1000; // TCC0 CCB1 - set servo angle to 0 deg on D3
    delay(2000);
    REG_TCC0_CCB0 = ; // TCC0 CCB0 - set servo angle to 180 deg on D2
    REG_TCC0_CCB1 = ; // TCC0 CCB1 - set servo angle to 180 deg on D3
    delay(2000);
    REG_TCC0_CCB0 = ; // TCC0 CCB0 - set servo angle to 360 deg on D2
    REG_TCC0_CCB1 = ; // TCC0 CCB1 - set servo angle to 360 deg on D3
    delay(2000);
}

/********************************** TASK 3 **********************************/
// Now that you are familar with interacting with the timer registers, implement
// a function that takes these two parameters:
// ccbCh    int     the CCB channel controlling the servo motor
// angle    float   servo shaft angle in degrees 
// and controls the servo motors accordingly

void setServo (int ccbCh, float angle) {

}


// The code below will test your setServo function
void loop() { 
  static int pos = 0;
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees in steps of 1
    setServo(0, pos);    
    setServo(1, 180 - pos);            
    delay(15);                       
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    setServo(0, pos);    
    setServo(1, 180 - pos);            
    delay(15);   
  }
}



// Ignore this, here be dragons
void setupPWM() {
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |          // Divide the 48MHz clock source by divisor 3: 48MHz/3=16MHz
                    GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK4
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization  

  for (uint8_t i = 0; i < CHANNELS; i++)
  {
    // Enable the port multiplexer for the 4 PWM channels: timer TCC0 outputs
    PORT->Group[g_APinDescription[pwmPins[i]].ulPort].PINCFG[g_APinDescription[pwmPins[i]].ulPin].bit.PMUXEN = 1;
    // Connect the TCC0 timer to the port outputs - port pins are paired odd PMUXO and even PMUXE
    // F & E specify the timers: TCC0, TCC1 and TCC2
    PORT->Group[g_APinDescription[pwmPins[i]].ulPort].PMUX[g_APinDescription[pwmPins[i]].ulPin >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;
  }

  // Feed GCLK4 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed 16 MHz GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Single slope PWM operation: timers countinuously count up to PER register value
  REG_TCC0_WAVE |= TCC_WAVE_WAVEGEN_NPWM;         // Setup normal PWM on TCC0                   
  while (TCC0->SYNCBUSY.bit.WAVE);                // Wait for synchronization

  // Divide the 16MHz signal by 16 giving 1MHz (1us) TCC0 timer tick and enable the outputs
  REG_TCC0_CTRLA |= TCC_CTRLA_PRESCALER_DIV16 |   // Divide GCLK4 by 16
                    TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
}