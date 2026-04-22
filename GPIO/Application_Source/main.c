#include <stdint.h>

#define GPIO_PRT1_DR (*(volatile uint32_t *)0x40040100)
#define GPIO_PRT1_PS (*(volatile uint32_t *)0x40040104)
#define GPIO_PRT1_PC (*(volatile uint32_t *)0x40040108)

int main(void)
{
    //GPIO_PRT1_PC |= (0xFF );
    GPIO_PRT1_DR |= ( );
    while(1);
}
