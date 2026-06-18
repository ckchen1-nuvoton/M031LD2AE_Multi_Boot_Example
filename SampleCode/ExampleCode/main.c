/****************************************************************************//**
 * @file     main.c
 * @version  V0.10
 * @brief    Implement a multi-boot system to boot from different applications in APROM.
 *           A LDROM code and 2 APROM code are implemented in this sample code.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Select HCLK clock source as HIRC and and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set PLL to Power-down mode and PLLSTB bit in CLK_STATUS register will be cleared by hardware.*/
    CLK_DisablePLL();

#if !defined(__LDROM__)
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
		
		/* Enable TMR0 clock */
    CLK_EnableModuleClock(TMR0_MODULE);

    /* Select TMR0 clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
#endif

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk))
                    |(SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/**
 * @brief    Routine to get a char
 * @param    None
 * @returns  Get value from UART debug port or semihost
 * @details  Wait UART debug port or semihost to input a char.
 */
static char GetChar(void)
{
    while(1)
    {
        if ((UART0->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0)
        {
            return (UART0->DAT);
        }
    }
}

#if !defined(__LDROM__)
/*
 * @returns     Send value from UART debug port
 * @details     Send a target char to UART debug port .
 */
static void SendChar_ToUART(int ch)
{
    while (UART0->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);

    UART0->DAT = ch;
    if(ch == '\n')
    {
        while (UART0->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
        UART0->DAT = '\r';
    }
}

static void PutString(char *str)
{
    while (*str != '\0')
    {
        SendChar_ToUART(*str++);
    }
}
#endif

void TMR0_IRQHandler(void)
{
	
#if defined(__BOOT0__)
		PutString("TMR0 : 1 Hz\n");
#endif
#if defined(__BOOT1__)
    PutString("TMR0 : 2 Hz\n");
#endif
	
    /* clear timer interrupt flag */
    TIMER_ClearIntFlag(TIMER0);
}

int32_t main(void)
{
    uint8_t u8Ch;
    uint32_t u32Data;

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

#if !defined(__LDROM__)
    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);
#endif
	
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC_Open();

    /*
        To use this sample code, please:
        1. Build all targets and download to device individually. The targets are:
            FMC_Boot0, RO=0x0000, ICE download algorithm to APROM
            FMC_Boot1, RO=0x8000, ICE download algorithm to APROM
            FMC_BootLD, RO=0x100000. ICE download algorithm to LDROM
        2. Reset MCU to execute FMC_BootLD.
    */

#if !defined(__LDROM__)
    PutString("\n\n");
    PutString("+---------------------------+\n");
    PutString("|   Multi-Boot Sample Code  |\n");
    PutString("+---------------------------+\n");
#endif	

#if defined(__BOOT0__)

		// Code Example 1
    PutString("Code Example 1\n");
		PutString("Boot from 0x0000\n");		
		
		/* Set timer frequency to 1HZ */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);

    /* Enable timer interrupt */
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

    /* Start Timer 0 */
    TIMER_Start(TIMER0);
#endif
#if defined(__BOOT1__)

    // Code Example 2
    PutString("Code Example 2\n");
		PutString("Boot from 0x8000\n");		
		
		/* Set timer frequency to 2HZ */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 2);

    /* Enable timer interrupt */
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

    /* Start Timer 0 */
    TIMER_Start(TIMER0);
#endif

#if !defined(__LDROM__)
    PutString("Select one boot image: \n");
    PutString("[0] Boot 0, base = 0x0000\n");
    PutString("[1] Boot 1, base = 0x8000\n");
#endif

#if !defined(__LDROM__)
    u8Ch = GetChar();//getchar();

    switch (u8Ch)
    {
				case '0':
						FMC_SetVectorPageAddr(0x0000);
						break;

				case '1':
						FMC_SetVectorPageAddr(0x8000);
						break;
    }
#else
		/* Boot from 0x0*/
		FMC_SetVectorPageAddr(0x0000);
#endif

    /* Reset CPU only to reset to new vector page */
    SYS_ResetCPU();

    /* Reset System to reset to new vector page. */
    //NVIC_SystemReset();

    /* Disable ISP function */
    FMC_Close();

    /* Lock protected registers */
    SYS_LockReg();

    //PutString("\nDone\n");

    while (SYS->PDID) __WFI();
}

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
