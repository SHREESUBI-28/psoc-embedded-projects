#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "./../Special_Libraries/cmsis_gcc.h"

void init_peri_Clock_Config(); //Peripheral clock initilizations function prototype
void clock_config(void);	//HF CLOCK divider init function prototype
void Delay(int32_t delayNumber); //Delay function prototype 

volatile uint16_t chanresult_12 = 0;
volatile float inputVoltage_12 = 0.0f;
volatile uint16_t chanresult_10 = 0;
volatile float inputVoltage_10 = 0.0f;
volatile uint16_t chanresult_8 = 0;
volatile float inputVoltage_8 = 0.0f;

int main()
{  
    /* HF CLOCK divider init*/
    clock_config();

	/* Configure P2.1 for ADC*/
	*((uint32_t *)0x40040208) &= ~(0x7 << 3u); // clear drive mode of P2.1 to analog mode -> GPIO2.PC // GPIO_PRT2_PC

	/* Configure P2.2 for LED*/
	*((uint32_t *)0x40040200) = (1 << 2); // Set default output value of P2.2 to 1 GPIO_PRT2_DR
	*((uint32_t *)0x40040208) |= (0x6 << 6u); // set drive mode of P2.2 to strong drive mode -> GPIO2.2.PC
	*((uint32_t *)0x40020200) &= ~(0xF << 8u); // Clear HSIOM_PORT_SEL2 pin 2 -> SW GPIO

    /* Peripheral clock initializatio*/
    init_peri_Clock_Config(); // ADC Peripheral clock init function call 

	/*ADC INIT*/
	//SAR_CTRL  4-> Vref=VDDA/2V, 7->BypassEn, 9->NEG_SEL=Vref, 30->SAR_SEQ=Disable, 31->SAR_En=1
	*((uint32_t*)0x403A0000) |= ((0x6 << 4) | // Vref=VDDA/2V
								 (0x1 << 7) | // BypassEn // Bypass capacitor enable
								 (0x7 << 9) | // NEG_SEL=Vref 
								 (0x1 << 30)| // SAR_SEQ=Disable 
								 (0x1 << 31)); // SAR_En=1 

	// SAR_MUX_SWITCH0 -> select Vplus to P2.1
	*((uint32_t*)0x403A0300) = (0x1<<1u);  // P2.1 is connected to MUX A input 1 

	//SAR_SAMPLE_CTRL 1->Left/Right Align, 2->SingleEnded=Unsigned, 16->Continuous=False
	*((uint32_t*)0x403A0004) |= (0x0 << 0) | 
								(0x0 << 1) | // Left/Right Align=Right // Right align
								(0x0 << 2) | // SingleEnded=Unsigned  
								(0x0 << 16); // Continuous=False 
	
	//SAMPLE_TIME01 -> set Time0 to 10 sampling adc clock cycles
	*((uint32_t*)0x403A0010) |= (0xA); // Time0=10 adc clock cycles 
	
	// SAR_CHAN0_CONFIG -> 0-6 ->pin and port address to P2.1, 12&13->SAMPLE_TIME=0, 9->RESOLUTION=MAX(12)
	*((uint32_t*)0x403A0080) =  (0x1 << 0)  | // Pin and port address to P2.1 (channel 1) 
								(0x0 << 4) | // Vminus = VSSA
								(0x0 << 9) | // Resolution=12 bits (Bit 9 = 0)
								(0x0 << 12); // SAMPLE_TIME=0

								
	//SAR_CHAN_EN -> Channel 0 enable
	*((uint32_t*)0x403A0020) = (0x1<<0); // Enable Channel 0 // Channel 0 corresponds to SAR_CHAN0_CONFIG register

    enable_irq(); // Enable global interrupts

    for(;;) // infinite loop
    {
		//SAR_START_CTRL -> Start ADC conversion
        *((uint32_t*)0x403A0024) = (0x1<<0);  // Start the conversion by writing 1 to START bit of SAR_START_CTRL register

		// SAR_INTR-> check for EOS -Wail till ADC completes conversion
		while( (*((uint32_t*)0x403A0210) & 0x1) != 0x1)  // Wait for EOS bit in SAR_INTR register // Busy wait /* Do nothing */ 
        { }

		// SAR_INTR -> Clear EOS by writing 1
		*((uint32_t*)0x403A0210) |= (0x01); // Clear EOS bit in SAR_INTR register by writing 1 to it

		// SAR_CHAN_RESULT0 -> Read result
		chanresult_12 = (uint16_t)((*(uint32_t*)0x403A0180) & 0xFFF);


		//Turn ON LED if light is low on LDR
		if(chanresult_12 > 2048) // Using 12-bit result for LED logic
		{
			// clear P2.2 -> TURNS ON LED10
			*((uint32_t *)0x40040200) &= ~(1 << 2); // Turn ON LED10 // GPIO_PRT2_DR
		}
		else
		{
			// Set P2.2 -> TURNS OFF LED10
			*((uint32_t *)0x40040200) |= (1 << 2);  // Turn OFF LED10 // GPIO_PRT2_DR
		}

		Delay(5000);
    }

    return 0;
}

/*Delay with simple for loops*/
void Delay(int32_t delayNumber)
{
    for(int32_t i=0; i<delayNumber; i++); // Outer loop 
    for(int32_t i=0; i<delayNumber; i++); // Inner loop
}

/*Peripheral clock initilizations*/
void init_peri_Clock_Config() // ADC Peripheral clock init function 
{
	/* ADC CLOCK CONFIGURATION*/
	*((uint32_t *)0x40010000) = (1<<30)|(1<<6)|(1<<0); // Disable the Divider 1 using PERI_DIV_CMD 
	*((uint32_t *)0x40010304) = (2 - 1) << 8 ; //Set the divider value in PERI_DIV_16_CTL1, We are configuring Divider 1 to 12MHz
	*((uint32_t *)0x40010000) |= (1<<31) |(3<<14) |(63<<8) |(1<<6)|(1<<0); //PERI_DIV_CMD 
	*((uint32_t *)0x40010148) = (1<<6)|(1<<0); // Specify Divider type 7:6 and Selected Divider 3:0 (Divider 1) in register PERI_PCLK_CTL18 ADC is PERIPHERAL 18
}
void clock_config(void) //HF CLOCK divider init function 
{
    *((uint32_t *)0x40030F08) = (0<<0); //selecting IMO clock frequency of 24MHz in CLK_IMO_ SELECT register.
    //*((uint32_t *)0x40030030)=(1<<31);//enbale the the IMO CLOCK Using CLK_IMO_CONFIG. By default it is one so no need to set this.
    *((uint32_t *)0x40030028)  = (0<<0)| (0<<2); //Selecting HFCLK_SEL as IMO[1:0] and HFCLK_DIV as '0'[3:2] in CLk_SELECT register.

}