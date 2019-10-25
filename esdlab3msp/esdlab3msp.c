/******************************************************************************
                                ESD LAB 3
                       RUCHA BORWANKAR (RUBO1268)
                   MSP432P401R LAUNCHPAD,CODE COMPOSER STUDIO
********************************************************************************/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ti/devices/msp432p4xx/inc/msp.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include<math.h>

/*Global variables*/
uint32_t cal30;
uint32_t cal85;
float calDifference;
float tempC;
float tempF;
char RevStr[5];
char revstr[5];
unsigned int num=40;
unsigned change;

//![Simple Timer_A Config]
/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        32000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        12800
};
//![Simple Timer_A Config]

//https://www.tutorialgateway.org/c-program-to-reverse-a-string and /https://www.geeksforgeeks.org/implement-itoa/
char* itoa(int num,char* str,int base)
{
    int i=0;
    while (num != 0)
        {
            int rem = num % base;
            str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
            num = num/base;
        }
    str[i] = '\0';
        int m, j, len;
        j = 0;
        len = strlen(str);
        for (m = len - 1; m >= 0; m--)
        {
            RevStr[j++] = str[i];
        }
        RevStr[i] = '\0';

 return RevStr;
}
int inttostr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
    while (i < d)
        str[i++] = '0';
    int m, j, len;
    j = 0;
    len = strlen(str);
    for (m = len - 1; m >= 0; m--)
    {
        revstr[j++] = str[i];
    }
    revstr[i] = '\0';
    str[i] = '\0';
    return i;
}

void ftoa(float n, char *res, int ap)
{
    int integer = (int)n;

    float floating = n - (float)integer;

    // convert integer part to string
    int i = inttostr(integer, res, 0);

    // check for display option after point
    if (ap != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        floating = floating * pow(10, ap);

        inttostr((int)floating, res + i + 1, ap);
    }
}
int main(void)

{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    //![Simple Timer_A Example]
    /* Setting MCLK to REFO at 128Khz for LF mode
     * Setting SMCLK to 64Khz */
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_2);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

    /* Configuring GPIO2.4 as peripheral output for PWM  and P6.7 for button
     * interrupt */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);   //led red
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);   //led green
 //   MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);      //led blue


    /* Configuring Timer_A to have a period of approximately 500ms and * an initial duty cycle of 10% of that (3200 ticks)  */
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    //![Simple Timer_A Example]

    /* Enabling interrupts and starting the watchdog timer */
    MAP_Interrupt_enableInterrupt(INT_PORT1);
   // MAP_Interrupt_enableSleepOnIsrExit();
   // MAP_Interrupt_enableMaster();

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
       CS->CTL0 = 0;                           // Reset tuning parameters
       CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
       CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
               CS_CTL1_SELS_3 |                // SMCLK = DCO
               CS_CTL1_SELM_3;                 // MCLK = DCO
       CS->KEY = 0;                            // Lock CS module from unintended accesses

       // Configure UART pins
       P1->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

       // Configure UART
       EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
       EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
               EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
       // Baud Rate calculation
       // 12000000/(16*9600) = 78.125
       // Fractional portion = 0.125
       // User's Guide Table 21-4: UCBRSx = 0x10
       // UCBRFx = int ( (78.125-78)*16) = 2
       EUSCI_A0->BRW = 78;                     // 12000000/16/9600
       EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
               EUSCI_A_MCTLW_OS16;

       EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
       EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
       EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

       /* Enabling the FPU with stacking enabled (for use within ISR) */
         FPU_enableModule();
         FPU_enableLazyStacking();

         /* Initializing ADC (MCLK/1/1) with temperature sensor routed */
         ADC14_enableModule();
         ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
                 ADC_TEMPSENSEMAP);

         /* Configuring ADC Memory (ADC_MEM0 A22 (Temperature Sensor) in repeat
          * mode).
          */
         ADC14_configureSingleSampleMode(ADC_MEM0, true);
         ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                 ADC_INPUT_A22, false);

         /* Configuring the sample/hold time for 192 */
         ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192,ADC_PULSE_WIDTH_192);

         /* Enabling sample timer in auto iteration mode and interrupts*/
         ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
         ADC14_enableInterrupt(ADC_INT0);

         /* Setting reference voltage to 2.5 and enabling temperature sensor */
         REF_A_enableTempSensor();
         REF_A_setReferenceVoltage(REF_A_VREF2_5V);
         REF_A_enableReferenceVoltage();

         cal30 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF,
                 SYSCTL_30_DEGREES_C);
         cal85 = SysCtl_getTempCalibrationConstant(SYSCTL_2_5V_REF,
                 SYSCTL_85_DEGREES_C);
         calDifference = cal85 - cal30;

         /* Enabling Interrupts */
         Interrupt_enableInterrupt(INT_ADC14);
         Interrupt_enableMaster();

         /* Triggering the start of the sample */
         ADC14_enableConversion();
         ADC14_toggleConversionTrigger();
       // Enable global interrupt
       __enable_irq();

       // Enable eUSCIA0 interrupt in NVIC module
       NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

       // Enable sleep on exit from ISR
    //   SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

       // Ensures SLEEPONEXIT occurs immediately
      // __DSB();

//       // Enter LPM0
       __sleep();
       __no_operation();                       // For debugger
//    /* Sleeping when not in use */
    while (1)
    {
        //MAP_PCM_gotoLPM0();
    }
}

/* Port1 ISR - This ISR will progressively step up the duty cycle of the PWM
 * on a button press
 */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);

    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status & GPIO_PIN1)
    {
        if(pwmConfig.dutyCycle == 28800)
        {       pwmConfig.dutyCycle = 3200;
        num=0;
        }
        else
            pwmConfig.dutyCycle += 3200;
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);      //led red
        num+=10;
        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    }
    if (status & GPIO_PIN4)
            {
                if(pwmConfig.dutyCycle ==0)
                {
               pwmConfig.dutyCycle = 28800;
                num=100;
            }
               else
                pwmConfig.dutyCycle -= 3200;
                num-=10;
                MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);    ////led green
                MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
               }
//    if (status & (EUSCI_A0->RXBUF=='C'||EUSCI_A0->RXBUF=='c'))
//        {
//        pwmConfig.dutyCycle = 16000;
//        change=50;
//        {
//            int q;
//        for(q=0;q<5000;q++)
//        {
//        }
//
//        }
//        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
//        //P1OUT ^= 0X01;
//        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
//        }
}

void ADC14_IRQHandler(void)
{
    uint64_t status;
    int16_t conRes;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if(status & ADC_INT0)
    {
        conRes = ((ADC14_getResult(ADC_MEM0) - cal30) * 55);
        tempC = (conRes / calDifference) + 30.0f;
        tempF = tempC * 9.0f / 5.0f + 32.0f;
    }

}
void EUSCIA0_IRQHandler(void)
{
    if(EUSCI_A0->RXBUF=='P'||EUSCI_A0->RXBUF=='p')
    {
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            {
             char b[5];
             itoa(num,b,10);
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='d';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='u';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='t';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='y';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=' ';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='c';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='y';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='c';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='l';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='e';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='=';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=b[1];
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=b[0];
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='%';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='\n';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='\r';
            }
    }

    if(EUSCI_A0->RXBUF=='T'||EUSCI_A0->RXBUF=='t')
    {
       while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
       {
            char t[5],f[5];
             ftoa(tempC,t,3);
             ftoa(tempF,f,3);
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='t';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='e';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='m';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='p';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=' ';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='i';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='n';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=' ';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='C';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='&';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='F';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='=';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=t[1];
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=t[0];
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=',';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=f[1];
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF=f[0];
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='\n';
             while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
             EUSCI_A0->TXBUF='\r';

                }
            }
}



/****************************REFERENCES***************************************************/
/**1.MSP example codes-timer_a_pwm mode,uart_pc_Echo,uart_01,adc14_singlechanneltemperature sensor
2.//https://www.tutorialgateway.org/c-program-to-reverse-a-string
3./https://www.geeksforgeeks.org/implement-itoa/
4.https://www.geeksforgeeks.org/convert-floating-point-number-string*/
