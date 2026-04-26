#include "STM32F401XX.h"
#include <string.h>
#include "STM32F401_SPI.h"

/*
 * we are using the below GPIO Pins
 * PB14 -----> SPI2_MISO
 * PB15 -----> SPI2_MOSI
 * PB13 -----> SPI2_SCLK
 * PB12 -----> SPI2_NSS
 * ALT Function_mode : 5
 */
/*====================================
 * COMMAND CODES FOR THE SPI PROTOCOL
 *====================================*/
#define COMMAND_LED_CTRL      0x50
#define COMMAND_SENSOR_READ   0x51
#define COMMAND_LED_READ      0x52
#define COMMAND_PRINT         0x53
#define COMMAND_ID_READ       0x54

#define LED_ON    1
#define LED_OFF   0

/*======================================
 * ARDUINO analog pins
 *======================================*/
#define ANALOG_PIN0 0
#define ANALOG_PIN1 1
#define ANALOG_PIN2 2
#define ANALOG_PIN3 3
#define ANALOG_PIN4 4
#define ANALOG_PIN5 5

#define LED_PIN 9

void SPI2_GPIO_Init(void)
{
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_config.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_config.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_config.GPIO_PinOptype = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_config.GPIO_Pinpupdcontrol = GPIO_NO_PUPD;
	SPIPins.GPIO_config.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// SCLK pin
	SPIPins.GPIO_config.GPIO_PinNumber = GPIO_PIN_13;
	GPIO_Init(&SPIPins);

	// MOSI pin
	SPIPins.GPIO_config.GPIO_PinNumber = GPIO_PIN_15;
	GPIO_Init(&SPIPins);

	//MISO pin
	 SPIPins.GPIO_config.GPIO_PinNumber = GPIO_PIN_14;
	 GPIO_Init(&SPIPins);

	 // NSS pin
	 SPIPins.GPIO_config.GPIO_PinNumber = GPIO_PIN_12;
	 GPIO_Init(&SPIPins);
}
void GPIO_BUTTON_ENABLE(void)
{
	GPIO_Handle_t GPIOBTN;
	// GPIO BUtton Codes
	GPIOBTN.pGPIOx=GPIOA;
	GPIOBTN.GPIO_config.GPIO_PinMode=GPIO_MODE_IN;
    GPIOBTN.GPIO_config.GPIO_PinSpeed=2;
	GPIOBTN.GPIO_config.GPIO_PinNumber=GPIO_PIN_0;
	GPIOBTN.GPIO_config.GPIO_Pinpupdcontrol=GPIO_PIN_PU;
	GPIO_PLLCLK_Init(GPIOA,ENABLE);
	GPIO_Init(&GPIOBTN);

}
void SPI2_Init(void)
{
	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIX = SPI2;
	SPI2handle.SPIConfig.busConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.deviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.sclkSpeed = SPI_CLOCKDIV8;
	SPI2handle.SPIConfig.cpol = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.cpha = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.ssm = SPI_SSM_DI;
	SPI2handle.SPIConfig.dff = SPI_DFF_8BIT;  // 8-bit data frame

	SPI_Init(&SPI2handle);
}

void delay(void)
{
	for(uint32_t i = 0; i < 500000/2 ; i++);
}

uint8_t SPI_Verify_response(uint8_t ackbyte)
{
	if(ackbyte == 0xF5)
	{
		return 1;
	}
	return 0;
}
int main()
{
	uint8_t dummy_write= 0xFF;
	uint8_t dummy_read;
	//Enable the GPIO button
	GPIO_BUTTON_ENABLE();
	// Initialize the SPI GPIO pins
	SPI2_GPIO_Init();
	// Initialize the SPI2 peripheral parameters
	SPI2_Init();
	//ENABLE SSOE Register
	SPI_SSOE_Config(SPI2,ENABLE);
	// While loop infinity
	while(1)
	{
	    uint8_t commandcode;
	    uint8_t ackbyte;
	    uint8_t args[2];

	    // ================= LED CONTROL =================
	    while(!GPIO_Read_Pin(GPIOA,GPIO_PIN_0));
	    delay();

	    SPI_Peripheralcontrol(SPI2, ENABLE);

	    commandcode = COMMAND_LED_CTRL;

	    SPI_Send_data(SPI2,&commandcode,1);
	    SPI_Recieve_data(SPI2,&dummy_read,1);

	    SPI_Send_data(SPI2,&dummy_write,1);
	    SPI_Recieve_data(SPI2,&ackbyte,1);

	    if(SPI_Verify_response(ackbyte))
	    {
	        args[0] = LED_PIN;
	        args[1] = LED_ON;
	        SPI_Send_data(SPI2,args,2);
	    }

	    // ================= SENSOR READ =================
	    while(!GPIO_Read_Pin(GPIOA,GPIO_PIN_0));
	    delay();

	    commandcode = COMMAND_SENSOR_READ;

	    SPI_Send_data(SPI2,&commandcode,1);
	    SPI_Recieve_data(SPI2,&dummy_read,1);

	    SPI_Send_data(SPI2,&dummy_write,1);
	    SPI_Recieve_data(SPI2,&ackbyte,1);

	    if(SPI_Verify_response(ackbyte))
	    {
	        args[0] = ANALOG_PIN0;
	        SPI_Send_data(SPI2,args,1);

	        SPI_Recieve_data(SPI2,&dummy_read,1);
	        delay();

	        SPI_Send_data(SPI2,&dummy_write,1);

	        uint8_t analog_read;
	        SPI_Recieve_data(SPI2,&analog_read,1);
	    }

	    // ================= LED READ =================
	    while(!GPIO_Read_Pin(GPIOA,GPIO_PIN_0));
	    delay();

	    commandcode = COMMAND_LED_READ;

	    SPI_Send_data(SPI2,&commandcode,1);
	    SPI_Recieve_data(SPI2,&dummy_read,1);

	    SPI_Send_data(SPI2,&dummy_write,1);
	    SPI_Recieve_data(SPI2,&ackbyte,1);

	    if(SPI_Verify_response(ackbyte))
	    {
	        args[0] = LED_PIN;
	        SPI_Send_data(SPI2,args,1);

	        SPI_Recieve_data(SPI2,&dummy_read,1);

	        uint8_t led_status;
	        SPI_Send_data(SPI2,&dummy_write,1);
	        SPI_Recieve_data(SPI2,&led_status,1);
	    }

	    // ================= PRINT MESSAGE =================
	    while(!GPIO_Read_Pin(GPIOA,GPIO_PIN_0));
	    delay();

	    commandcode = COMMAND_PRINT;

	    SPI_Send_data(SPI2,&commandcode,1);
	    SPI_Recieve_data(SPI2,&dummy_read,1);

	    SPI_Send_data(SPI2,&dummy_write,1);
	    SPI_Recieve_data(SPI2,&ackbyte,1);

	    if(SPI_Verify_response(ackbyte))
	    {
	        char message[] = "Hello Arduino";

	        uint8_t len = strlen(message);
	        SPI_Send_data(SPI2,&len,1);       // send length
	        SPI_Send_data(SPI2,(uint8_t*)message,len);
	    }

	    // ================= ID READ =================
	    while(!GPIO_Read_Pin(GPIOA,GPIO_PIN_0));
	    delay();

	    commandcode = COMMAND_ID_READ;

	    SPI_Send_data(SPI2,&commandcode,1);
	    SPI_Recieve_data(SPI2,&dummy_read,1);

	    SPI_Send_data(SPI2,&dummy_write,1);
	    SPI_Recieve_data(SPI2,&ackbyte,1);

	    if(SPI_Verify_response(ackbyte))
	    {
	        uint8_t id[10];

	        for(int i = 0; i < 10; i++)
	        {
	            SPI_Send_data(SPI2,&dummy_write,1);
	            SPI_Recieve_data(SPI2,&id[i],1);
	        }
	    }

	    // Wait until SPI not busy
	    while(SPI_getflag_status(SPI2,SPI_BUSY_FLAG));

	    SPI_Peripheralcontrol(SPI2, DISABLE);
	}
	return 0;
}
