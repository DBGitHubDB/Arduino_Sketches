/****************************PWM Frequency****************************************************

  Set PWM on output D2, D5, D6, D7 @ 100 Hz
  Each timer counts up to a maximum or TOP value set by the PER register,
  this determines the frequency of the PWM operation:
  20000 = 50Hz, 10000 = 100Hz, 2500  = 400Hz  2000 = 500Hz
*/
unsigned int pwmFreq = 10000;


void setup() {

  setPWM();

}
void loop() {

  out_D2(128);
  out_D5(128);
  out_D6(128);
  out_D7(128);

}
/*******************************************************************************************************************************
  void setPWM()
********************
  Purpose: Set PWM  onpin 2, 5, 6, 7 @ 100Hz : you can change frequency changing pwmFreq
  Entry:   N/A
  Exit:    N/A

  ToDo: ...
*******************************************************************************************************************************/
void setPWM()
{
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |          // Divide the 48MHz clock source by divisor 3: 48MHz/3=16MHz
                    GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK4
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Enable the port multiplexer for the 4 PWM channels: timer TCC0 outputs
  const uint8_t CHANNELS = 4;
  const uint8_t pwmPins[] = { 2, 5, 6, 7 };
  for (uint8_t i = 0; i < CHANNELS; i++)
  {
    PORT->Group[g_APinDescription[pwmPins[i]].ulPort].PINCFG[g_APinDescription[pwmPins[i]].ulPin].bit.PMUXEN = 1;
  }
  // Connect the TCC0 timer to the port outputs - port pins are paired odd PMUO and even PMUXE
  // F & E specify the timers: TCC0, TCC1 and TCC2
  PORT->Group[g_APinDescription[2].ulPort].PMUX[g_APinDescription[2].ulPin >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;
  PORT->Group[g_APinDescription[6].ulPort].PMUX[g_APinDescription[6].ulPin >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;

  // Feed GCLK4 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Dual slope PWM operation: timers countinuously count up to PER register value then down 0
  REG_TCC0_WAVE |= TCC_WAVE_POL(0xF) |         // Reverse the output polarity on all TCC0 outputs
                   TCC_WAVE_WAVEGEN_DSBOTTOM;    // Setup dual slope PWM on TCC0
  while (TCC0->SYNCBUSY.bit.WAVE);               // Wait for synchronization

  // Each timer counts up to a maximum or TOP value set by the PER register,
  // this determines the frequency of the PWM operation:
  // 20000 = 50Hz, 10000 = 100Hz, 2500  = 400Hz  2000 = 500Hz
  REG_TCC0_PER = pwmFreq;      // Set the frequency of the PWM on TCC0 to 500Hz
  while (TCC0->SYNCBUSY.bit.PER);

  // Divide the 16MHz signal by 8 giving 2MHz (0.5us) TCC0 timer tick and enable the outputs
  REG_TCC0_CTRLA |= TCC_CTRLA_PRESCALER_DIV8 |     // Divide GCLK4 by 8
                    TCC_CTRLA_ENABLE;                // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);               // Wait for synchronization
}
/*******************************************************************************************************************************
  void out_D2(byte i)
********************
  Purpose: Output @ 100Hz 10 bit on D2
  Entry:   unsigned int  ( 0,255)
  Exit:    pwm 100Hz @ 10 bit ( 0,pwmFreq)

  ToDo: ...
*******************************************************************************************************************************/
void out_D2(unsigned int i)
{
  int a = map(i, 0, 255, 0, pwmFreq);
  // The CCBx register value corresponds to the pulsewidth in microseconds (us)
  REG_TCC0_CCB0 = a;       // TCC0 CCB0 - center the servo on D2
  while (TCC0->SYNCBUSY.bit.CCB0);
}

/*******************************************************************************************************************************
  void out_D5(byte i)
********************
  Purpose: Output @ 100Hz 10 bit on D5
  Entry:   unsigned int  ( 0,255)
  Exit:    pwm 100Hz @ 10 bit ( 0,pwmFreq)

  ToDo: ...
*******************************************************************************************************************************/
void out_D5(unsigned int i)
{
  int a = map(i, 0, 255, 0, pwmFreq);
  // The CCBx register value corresponds to the pulsewidth in microseconds (us)
  REG_TCC0_CCB1 = a;       // TCC0 CCB1 - center the servo on D5
  while (TCC0->SYNCBUSY.bit.CCB1);
}
/*******************************************************************************************************************************
  void out_D6(byte i)
********************
  Purpose: Output @ 100Hz 10 bit on D6
  Entry:   unsigned int ( 0,255)
  Exit:    pwm 100Hz @ 10 bit ( 0,pwmFreq)

  ToDo: ...
*******************************************************************************************************************************/
void out_D6(unsigned int i)
{
  int a = map(i, 0, 255, 0, pwmFreq);
  // The CCBx register value corresponds to the pulsewidth in microseconds (us)
  REG_TCC0_CCB2 = a;       //  TCC0 CCB2 - center the servo on D6
  while (TCC0->SYNCBUSY.bit.CCB2);
}
/*******************************************************************************************************************************
  void out_D7(byte i)
********************
  Purpose: Output @ 100Hz 10 bit on D7
  Entry:   unsigned int  ( 0,255)
  Exit:    pwm 100Hz @ 10 bit ( 0,pwmFreq)

  ToDo: ...
*******************************************************************************************************************************/
void out_D7(unsigned int i)
{
  int a = map(i, 0, 255, 0, pwmFreq);
  // The CCBx register value corresponds to the pulsewidth in microseconds (us)
  REG_TCC0_CCB3 = a;       // TCC0 CCB3 - center the servo on D7
  while (TCC0->SYNCBUSY.bit.CCB3);
}
