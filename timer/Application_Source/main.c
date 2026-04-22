#include "user_cfg.h"


/* Timer period in milliseconds */
#define TIMER_PERIOD_MSEC   5000U


//GPIO REGISTERS
#define  GPIO_PRT1_DR (*(volatile uint32_t *)0x40040100)
#define  GPIO_PRT1_PC (*(volatile uint32_t *)0x40040108)
#define  GPIO_PRT1_DR_INV (*(volatile uint32_t *)0x40040148)


//TIMER2 INTERUPT REGISTERS
#define  CM0P_IPR4 (*(volatile uint32_t *)0xE000E410)
#define  CM0P_ICPR (*(volatile uint32_t *)0xE000E280)
#define  CM0P_ISER (*(volatile uint32_t *)0xE000E100)


//TIMER 2 INITITALIZATION
#define TCPWM_CTRL (*(volatile uint32_t *)0x40200000)
#define TCPWM_CNT1_COUNTER (*(volatile uint32_t *)0x40200148)
#define TCPWM_CNT1_TR_CTRL1 (*(volatile uint32_t *)0x40200164)
#define TCPWM_CNT1_PERIOD (*(volatile uint32_t *)0x40200154)
#define TCPWM_CNT1_CTRL (*(volatile uint32_t *)0x4020013C)
#define TCPWM_CNT1_INTR_MASK (*(volatile uint32_t *)0x40200178)
#define TCPWM_CNT1_INTR (*(volatile uint32_t *)0x40200170)
#define TCPWM_CMD (*(volatile uint32_t *)0x40200008)


//PERIPHERAL INTERCONNECT REGISTERS
#define PERI_DIV_CMD (*(volatile uint32_t *)0x40010000)
#define PERI_DIV_16_CTL3 (*(volatile uint32_t *)0x4001030C)
#define PERI_PCLK_CTL8 (*(volatile uint32_t *)0x40010120)


uint8_t app_heap[512] __attribute__((section (".heap")));
uint8_t app_stack[1024] __attribute__((section (".stack")));


int main()
{  
    /* HF CLOCK divider init*/
    Cy_SysClk_ClkHfSetDivider(0u); //0 - No Divider, 1 - DIV by 2, 2 = DIV by 4, 3 = DIV by 8


    /*GPIO pin init*/
    GPIO_PRT1_DR= (1 << 4); // Set default output value of P1.4 to 1 in GPIO_PRT1_DR
    GPIO_PRT1_PC = (6 << 12); // Set drive mode of P1.4 to Digital OP Push Pull in GPIO_PRT1_PC
    /* Peripheral clock initializatio*/
    init_peri_Clock_Config();


    /*TIMER 2 - INTERRUPT*/
    CM0P_IPR4 = (1 << 30); //Timer 2 IRQn=19 Priority  1 set
    CM0P_ICPR = 0xFFFFFFFF; //NVIC Clear Pending IRQs
    CM0P_ISER = (1 << 18); //NVIC_EnableIRQ 19
   
    /*TIMER 2 - INIT*/
    TCPWM_CTRL &=~ (1<< 1); //Disable Timer 2  in TCPWM_CTRL Register


    TCPWM_CNT1_COUNTER = 0; //Clear the counter register of  TCPWM2 TCPWM_CNT2_COUNTER Register


    TCPWM_CNT1_TR_CTRL1 = 0; //Clear the  register of  TCPWM2 TCPWM_CNT2_TR_CTRL2 Register


    TCPWM_CNT1_PERIOD = (TIMER_PERIOD_MSEC-1); //Set the Period Register of TCPWM2 TCPWM_CNT2_PERIOD Register


    TCPWM_CNT1_CTRL |= (0 << 24); //Mode configuration of for TCPWM2, TCPWM_CNT2_CTRL Regsiter


    TCPWM_CNT1_INTR_MASK |= (1 << 0); // Set  interrupt mask  by enabling the Interrupt in TCPWM_CNT2_INTR_MASK Register for TCPWM2


    TCPWM_CNT1_INTR |= (1<<0); //Clear any previoius interrupt in  interrupt register in TCPWM_CNT2_INTR Register of TCPWM2


     TCPWM_CTRL |= (1<< 1); //Enable Timer 2  in TCPWM_CTRL Register


    TCPWM_CMD = (1 << 1); //Triger start Timer 2  in TCPWM_CTRL Register
    /* Enable Interrupts at CPU level */
    enable_irq();
     for(;;)
    {


    }


    return 0;
}



/*Timer 2 interrupt*/
void tcpwm_interrupts_1_IRQHandler(void)
{
    /* Clear the terminal count interrupt */
     TCPWM_CNT1_INTR |= (1<<0); //clear interrupt  interrupt in TCPWM_CNT2_INTR


    /* Toggle the LED */
     GPIO_PRT1_DR_INV |= (1<<4); //Toggle the LED on P1.4 by inverting in GPIO_PRT1_DR_INV Register
}
/*Peripheral clock initilizations*/
void init_peri_Clock_Config()
{
    //TIMER 2 TIMER- CLOCK
     PERI_DIV_CMD = (1<<30) |(1<<6) | (3 << 0) ; // Disable the Divider using PERI_DIV_CMD


     PERI_DIV_16_CTL3 = (24000 - 1) << 8 ; //Set the divider value in PERI_DIV_16_CTL3, We are configuring Divider 3


    PERI_DIV_CMD |= (1<<31) |(3<<14) |(63<<8) |(1<<6) | (3 << 0); //PERI_DIV_CMD
    //Enable the divder 31:bit, Keep 3 at 15:14 and 63 13:8 this selects the HFCLK as reference , Select 16 bit divider 7:6, and Select the divider no 3 using 5:0;


     PERI_PCLK_CTL8 = (1<<6)|(3<<0); // Specify Divider type 7:6 and Selected Divider 3:0 in register PERI_PCLK_CTL8 TCPWM2 is PERIPHERAL 8


}
