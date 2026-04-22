#include "user_cfg.h"

/* Timer period in milliseconds */
#define TIMER_PERIOD_MSEC   3000U
uint8_t app_heap[512] __attribute__((section (".heap")));
uint8_t app_stack[1024] __attribute__((section (".stack")));

// ADC REGISTERS DEFINITIONS
#define SAR_CTRL (*(volatile uint32_t*) 0x403A0000)
#define SAR_MUX_SWITCH0 (*(volatile uint32_t*) 0x403A0300)
#define SAR_SAMPLE_CTRL (*(volatile uint32_t*) 0x403A0004)
#define SAR_SAMPLE_TIME01 (*(volatile uint32_t*) 0x403A0010)
#define SAR_CHAN_CONFIG0 (*(volatile uint32_t*) 0x403A0080)
#define SAR_CHAN_EN (*(volatile uint32_t*) 0x403A0020)
#define SAR_START_CTRL (*(volatile uint32_t*) 0x403A0024)
#define SAR_INTR (*(volatile uint32_t*) 0x403A0210)
#define SAR_CHAN_RESULT0 (*(volatile uint32_t*) 0x403A0180)


#define GPIO_PRT1_DR (*(volatile uint32_t *)0x40040100)
#define GPIO_PRT1_PC (*(volatile uint32_t *)0x40040108)
#define GPIO_PRT1_DR (*(volatile uint32_t *)0x40040100)
#define GPIO_PRT1_PC (*(volatile uint32_t *)0x40040108)
#define GPIO_PRT2_PC (*(volatile uint32_t *)0x40040208)
#define HSIOM_PORT_SEL1 (*(volatile uint32_t *)0x40020100)
#define  GPIO_PRT1_DR_INV (*(volatile uint32_t *)0x40040148)




//TIMER2 INTERUPT REGISTERS
#define  CM0P_IPR4 (*(volatile uint32_t *)0xE000E410)
#define  CM0P_ICPR (*(volatile uint32_t *)0xE000E280)
#define  CM0P_ISER (*(volatile uint32_t *)0xE000E100)

//TIMER 2 INITITALIZATION
#define TCPWM_CTRL (*(volatile uint32_t *)0x40200000)
#define TCPWM_CNT2_COUNTER (*(volatile uint32_t *)0x40200188)
#define TCPWM_CNT2_TR_CTRL2 (*(volatile uint32_t *)0x402001A8)
#define TCPWM_CNT2_PERIOD (*(volatile uint32_t *)0x40200194)
#define TCPWM_CNT2_CTRL (*(volatile uint32_t *)0x40200180)
#define TCPWM_CNT2_INTR_MASK (*(volatile uint32_t *)0x402001B8)
#define TCPWM_CNT2_INTR (*(volatile uint32_t *)0x402001B0)
#define TCPWM_CMD (*(volatile uint32_t *)0x40200008)

//PERIPHERAL INTERCONNECT REGISTERS
#define PERI_DIV_CMD (*(volatile uint32_t *)0x40010000)
#define PERI_DIV_16_CTL3 (*(volatile uint32_t *)0x4001030C)
#define PERI_PCLK_CTL8 (*(volatile uint32_t *)0x40010120)


volatile uint16_t chanresult_12=0;

int main()
{  
    /* HF CLOCK divider init*/
    Cy_SysClk_ClkHfSetDivider(0u); //0 - No Divider, 1 - DIV by 2, 2 = DIV by 4, 3 = DIV by 8

    /*GPIO pin init*/
    *((uint32_t *)0x40040100) = (1 << 4); //GPIO_PRT1_DR Set default output value of P1.4 to 1 in GPIO_PRT1_DR
    *((uint32_t *)0x40040108) = (6 << 12); //GPIO_PRT1_PC Set drive mode of P1.4 to Digital OP Push Pull in GPIO_PRT1_PC
    /* Peripheral clock initializatio*/
    init_peri_Clock_Config();

    /*TIMER 2 - INTERRUPT*/
    *((uint32_t *)0xE000E410) = (1 << 30); //CM0P_IPR4   Timer 2 IRQn=19 Priority  1 set
    *((uint32_t *)0xE000E280) = 0xFFFFFFFF;//CM0P_ICPR   NVIC Clear Pending IRQs
    *((uint32_t *)0xE000E100) = (1 << 19); //CM0P_ISER   NVIC_EnableIRQ 19
   
    /*TIMER 2 - INIT*/
    *((uint32_t *)0x40200000) &=~ (1<< 2); //TCPWM_CTRL  Disable Timer 2  in TCPWM_CTRL Register

    *((uint32_t *)0x40200188) = 0; //TCPWM_CMT2_COUNTER  Clear the counter register of  TCPWM2 TCPWM_CNT2_COUNTER Register

    *((uint32_t *)0x402001A8) = 0; //TCPWM_CNT2_TR_CTRL2  Clear the  register of  TCPWM2 TCPWM_CNT2_TR_CTRL2 Register

    *((uint32_t *)0x40200194)  = (TIMER_PERIOD_MSEC-1); // TCPWM_CNT2_PERIOD  Set the Period Register of TCPWM2 TCPWM_CNT2_PERIOD Register

    *((uint32_t *)0x40200180) |= (0 << 24); //TCPWM_CNT2_CTRL Mode configuration of for TCPWM2, TCPWM_CNT2_CTRL Regsiter

    *((uint32_t *)0x402001B8) |= (1 << 0); //TCPWM_CNT2_INTR_MASK  Set  interrupt mask  by enabling the Interrupt in TCPWM_CNT2_INTR_MASK Register for TCPWM2

    *((uint32_t *)0x402001B0) |= (1<<0); // TCPWM_CNT2_INTR Clear any previoius interrupt in  interrupt register in TCPWM_CNT2_INTR Register of TCPWM2

    *((uint32_t *)0x40200000) |= (1<< 2); //TCPWM_CTRL Enable Timer 2  in TCPWM_CTRL Register

    *((uint32_t *)0x40200008) = (1 << 26); //TCPWM_CMD Triger start Timer 2  in TCPWM_CTRL Register
    /* Enable Interrupts at CPU level */
    enable_irq();

    //ADC Initilization

     clock_config(); // Initialize the clock settings for the system

    GPIO_PRT2_PC |= (0u <<3); // Set pin 2.1 to analog mode
    GPIO_PRT1_PC |=  (6u << 0); // Set pin 1.0 to strong drive mode for LED control
    GPIO_PRT1_DR |= (1 << 0); // Set P1.0 high to turn off LED10 initially
    //HSIOM_PORT_SEL1 |= (0u << 0); // Set pin 1.0 to GPIO mode (HSIOM selection)
   
    SAR_CTRL |= ((6 << 4) | //Vref_SEL = VddA/2
                (1 << 7) | //Enable Bypass Capacitor
                (7 << 9) | //Neg_SEL = Vreff
                (1 << 30)| //Disable mux switch
                (1 << 31)); //SAR_EN = 1
    SAR_MUX_SWITCH0 |= (1 << 1); //Closing the switch between P2.1 and v+ signal
    SAR_SAMPLE_CTRL |=  (0 << 0) | //Sub-resoluton 0 why??
                        (0 << 1) | //Right allignment
                        (0 << 2) |//Unsigned
                        (0 << 16); // Continuous=False (Why False!!)
    SAR_SAMPLE_TIME01 |= (0xA); // Time0=10 adc clock cycles
    SAR_CHAN_CONFIG0 |= (0x1 << 0)  | // Pin and port address to P2.1 (channel 1)
                        (0x0 << 4) | // Vminus = VSSA
                        (0x0 << 9) | // Resolution=12 bits (Bit 9 = 0)
                        (0x0 << 12); // SAMPLE_TIME=0
    SAR_CHAN_EN |= (0x1<<0);
     for(;;)
    {
    }
return 0;
}


/*Timer 2 interrupt*/
void tcpwm_interrupts_2_IRQHandler(void)
{
    /* Clear the terminal count interrupt */
    *((uint32_t *)0x402001B0) |= (1<<0); //clear interrupt  interrupt in TCPWM_CNT2_INTR
    //ADC


      SAR_START_CTRL |= (1 << 0);
        while( (SAR_INTR & 0x1) != 0x1)  // Wait for EOS bit in SAR_INTR register // Busy wait /* Do nothing */
        { }
        SAR_INTR = (0x01); //And mainly why i have to clear when the above loop comes out when 1??
        chanresult_12 = (uint16_t)((SAR_CHAN_RESULT0)& 0xFFF);
        if(chanresult_12 > 1500) // Using 12-bit result for LED logic
        {
            // clear P2.2 -> TURNS ON LED10
            GPIO_PRT1_DR &= ~(1 << 0); // Turn ON LED10 // GPIO_PRT2_DR
        }
        else
        {
            // Set P2.2 -> TURNS OFF LED10
            GPIO_PRT1_DR |= (1 << 0);  // Turn OFF LED10 // GPIO_PRT2_DR
        }

}
/*Peripheral clock initilizations*/
void init_peri_Clock_Config(void)
{
    /* ========= ADC CLOCK : Divider 1 ========= */


    /* Disable Divider 1 */
    PERI_DIV_CMD = (1<<30) | (1<<6) | (1<<0);


    /* Divider value for ADC (example: 12 MHz from 24 MHz) */
    *((uint32_t *)0x40010304) = (2 - 1) << 8;   // PERI_DIV_16_CTL1


    /* Enable Divider 1 */
    PERI_DIV_CMD = (1<<31) | (3<<14) | (63<<8) | (1<<6) | (1<<0);


    /* Route Divider 1 to ADC (Peripheral 18) */
    *((uint32_t *)0x40010148) = (1<<6) | (1<<0);   // PERI_PCLK_CTL18




    /* ========= TIMER CLOCK : Divider 3 ========= */


    /* Disable Divider 3 */
    PERI_DIV_CMD = (1<<30) | (1<<6) | (3<<0);


    /* Divider value for TIMER */
    PERI_DIV_16_CTL3 = (24000 - 1) << 8;


    /* Enable Divider 3 */
    PERI_DIV_CMD = (1<<31) | (3<<14) | (63<<8) | (1<<6) | (3<<0);


    /* Route Divider 3 to TCPWM2 (Peripheral 8) */
    PERI_PCLK_CTL8 = (1<<6) | (3<<0);
}
void clock_config(void) //HF CLOCK divider init function
{
    *((uint32_t *)0x40030F08) = (0<<0); //selecting IMO clock frequency of 24MHz in CLK_IMO_ SELECT register.
    //*((uint32_t *)0x40030030)=(1<<31);//enbale the the IMO CLOCK Using CLK_IMO_CONFIG. By default it is one so no need to set this.
    *((uint32_t *)0x40030028)  = (0<<0)| (0<<2); //Selecting HFCLK_SEL as IMO[1:0] and HFCLK_DIV as '0'[3:2] in CLk_SELECT register.
}







// code interfacing LDR and timer the LDR will take a sample every 5 seconds once timer count for 5  seconds 



//LDR interface with 2.1

