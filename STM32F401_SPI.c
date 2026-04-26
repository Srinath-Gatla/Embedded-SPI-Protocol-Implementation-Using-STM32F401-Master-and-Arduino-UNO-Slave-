#include "STM32F401_SPI.h"

/* PERIPHERAL CLOCK */
void SPI_PLLCLK_Init(SPI_RegDef_t *pSPIx, uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_EN();
		}
	}
	else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_DI();
		}
	}
}

/* INITIALIZE AND DEINITIALIZE THE SPI */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	// Peripheral clock enable
	SPI_PLLCLK_Init(pSPIHandle->pSPIX, ENABLE);

	// Configure the SPI CR1 Register
	uint32_t tempreg = 0;

	// Configure the device mode (Master/Slave)
	tempreg |= pSPIHandle->SPIConfig.deviceMode << SPI_CR1_MSTR;

	// Configure the Bus configuration
	if(pSPIHandle->SPIConfig.busConfig == SPI_BUS_CONFIG_FD)
	{
		// BIDI mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		tempreg &= ~(1 << SPI_CR1_RXONLY);  // Clear RXONLY for full duplex
	}
	else if(pSPIHandle->SPIConfig.busConfig == SPI_BUS_CONFIG_HD)
	{
		// BIDI mode to be set
		tempreg |= (1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.busConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		// BIDI Mode should be cleared and RXONLY mode should be enabled
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		tempreg |= (1 << SPI_CR1_RXONLY);
	}

	// Configure the SPI Serial Clock speed (Baud Rate)
	tempreg |= pSPIHandle->SPIConfig.sclkSpeed << SPI_CR1_BR3_5;

	// Configure the DFF (Data Frame Format)
	if(pSPIHandle->SPIConfig.dff == SPI_DFF_8BIT)
	{
		tempreg &= ~(1 << SPI_CR1_DFF);  // 8-bit mode
	}
	else
	{
		tempreg |= (1 << SPI_CR1_DFF);   // 16-bit mode
	}

	// Configure the CPOL (Clock Polarity)
	if(pSPIHandle->SPIConfig.cpol == SPI_CPOL_HIGH)
	{
		tempreg |= (1 << SPI_CR1_CPOL);
	}
	else
	{
		tempreg &= ~(1 << SPI_CR1_CPOL);
	}

	// Configure the CPHA (Clock Phase)
	if(pSPIHandle->SPIConfig.cpha == SPI_CPHA_HIGH)
	{
		tempreg |= (1 << SPI_CR1_CPHA);
	}
	else
	{
		tempreg &= ~(1 << SPI_CR1_CPHA);
	}

	// Configure SSM (Software Slave Management)
	if(pSPIHandle->SPIConfig.ssm == SPI_SSM_EN)
	{
		tempreg |= (1 << SPI_CR1_SSM);
		tempreg |= (1 << SPI_CR1_SSI);  // Set SSI when using SSM
	}
	else
	{
		tempreg &= ~(1 << SPI_CR1_SSM);
	}

	// Write the configuration to CR1 register
	pSPIHandle->pSPIX->CR1 = tempreg;
}

void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	}
	else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	}
	else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	}
	else if(pSPIx == SPI4)
	{
		SPI4_REG_RESET();
	}
}

/* SEND AND RECEIVE DATA (blocking/polling type) */
void SPI_Send_data(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len)
{
	// For SSM mode, make sure SSI is set
	if(pSPIx->CR1 & (1 << SPI_CR1_SSM))
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}

	while(len > 0)
	{
		// Wait for TX buffer empty
		while(SPI_getflag_status(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		// Check if 16-bit or 8-bit mode
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			// 16-bit mode
			pSPIx->DR = *((uint16_t *)pTxBuffer);
			len -= 2;
			pTxBuffer += 2;
		}
		else
		{
			// 8-bit mode
			pSPIx->DR = *pTxBuffer;
			len--;
			pTxBuffer++;
		}
	}

	// Wait for transmission to complete
	while(SPI_getflag_status(pSPIx, SPI_BUSY_FLAG) == FLAG_SET);
}

void SPI_Recieve_data(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len)
{
	// For SSM mode, make sure SSI is set
	if(pSPIx->CR1 & (1 << SPI_CR1_SSM))
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}

	while(len > 0)
	{
		// Wait for RX buffer not empty
		while(SPI_getflag_status(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		// Check if 16-bit or 8-bit mode
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			// 16-bit mode
			*((uint16_t *)pRxBuffer) = (uint16_t)(pSPIx->DR);
			len -= 2;
			pRxBuffer += 2;
		}
		else
		{
			// 8-bit mode
			*pRxBuffer = (uint8_t)(pSPIx->DR & 0xFF);
			len--;
			pRxBuffer++;
		}
	}
}

uint8_t SPI_getflag_status(SPI_RegDef_t *pSPIx, uint32_t flagname)
{
	if(pSPIx->SR & flagname)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/* SPI PERIPHERAL CONTROL */
void SPI_Peripheralcontrol(SPI_RegDef_t *pSPIx, uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}
	else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}
/* SSI CONFIGURATION */
void SPI_SSI_Config(SPI_RegDef_t *pSPIx, uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		// Set SSI bit
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}
	else
	{
		// Clear SSI bit
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}
/*
 * SSOE CONFIGURATION
 */
void SPI_SSOE_Config(SPI_RegDef_t *pSPIx, uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		// Set SSI bit
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}
	else
	{
		// Clear SSI bit
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}
/* SPI INTERRUPT HANDLING (placeholder functions) */
void SPI_IRQCONFIG(uint8_t IRQNumber, uint8_t IRQENorDI)
{
	// Implementation depends on your NVIC setup
	// Add your NVIC configuration code here
}

void SPI_IRQPriority(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// Implementation depends on your NVIC setup
	// Add your NVIC priority configuration code here
}

void SPI_IRQHandle(SPI_Handle_t *pSPIHandle)
{
	// Implementation depends on your interrupt handling needs
	// Add your interrupt handling code here
}
