
#include "MY_L3GD20.h"

//SPI Chip Select
#define _L3GD20_CS_ENBALE		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
#define _L3GD20_CS_DISABLE		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);

//Library variables
//1. SPI handle
static SPI_HandleTypeDef accSPI_Handle;

//2. Sensitivity value
static float L3GD20_Sensitivity = 0.0;

//Functions definitions
//Private functions
//1. Write IO
void L3GD20_WriteIO(uint8_t reg, uint8_t *dataW, uint8_t size)
{
	uint8_t spiReg = reg;
	//Enable CS
	_L3GD20_CS_ENBALE;
	//set register value
	HAL_SPI_Transmit(&accSPI_Handle, &spiReg, 1, 10);
	//Transmit data
	HAL_SPI_Transmit(&accSPI_Handle, dataW, size, 10);
	//Disable CS
	_L3GD20_CS_DISABLE;
}
//2. Read IO
void L3GD20_ReadIO(uint8_t reg, uint8_t *dataR, uint8_t size)
{
	uint8_t spiBuf[4];
	spiBuf[0] = reg | 0x80;
	//Enable CS
	_L3GD20_CS_ENBALE;
	//set register value
	HAL_SPI_Transmit(&accSPI_Handle, spiBuf, 1, 10);
	//Transmit data
	HAL_SPI_Receive(&accSPI_Handle, spiBuf, size, 10);
	//Disable CS
	_L3GD20_CS_DISABLE;
	
	for(uint8_t i=0; i<(size&0x3); i++)
	{
		dataR[i] = spiBuf[i];
	}
}

//1. Accelerometer initialise function
void L3GD20_Init(SPI_HandleTypeDef *accSPI, L3GD20_InitTypeDef *accInitDef)
{
	uint8_t spiData = 0;
	
	memcpy(&accSPI_Handle, accSPI, sizeof(*accSPI));
	spiData |= (accInitDef->enableAxes & 0x07);
	spiData |= (accInitDef->dataRate & 0xF0);
	spiData |= ~L3GD20_POWERDOWN_MASK;
	L3GD20_WriteIO(L3GD20_CTRL_REG1_ADDR, &spiData, 1);

	spiData = 0;
	spiData |= (accInitDef->fullScale & 0x30);
	L3GD20_WriteIO(L3GD20_CTRL_REG4_ADDR, &spiData, 1);
	
	//** 3. Interrupt Configuration **//
	if(accInitDef->interruptEnable)
	{
		spiData = 0x88;
		//Write to accelerometer
		L3GD20_WriteIO(L3GD20_CTRL_REG3_ADDR, &spiData, 1);
	}
	
	//Assign sensor sensitivity (based on Full-Scale)
	switch(accInitDef->fullScale)
	{
		case L3GD20_FULLSCALE_250DPS:
			L3GD20_Sensitivity = L3GD20_FULLSCALE_250DPS_SENSITIVITY;
			break;
		
		case L3GD20_FULLSCALE_500DPS:
			L3GD20_Sensitivity = L3GD20_FULLSCALE_500DPS_SENSITIVITY;
			break;
		
		case L3GD20_FULLSCALE_2000DPS:
			L3GD20_Sensitivity = L3GD20_FULLSCALE_2000DPS_SENSITIVITY;
			break;
		
		default:
			L3GD20_Sensitivity = 0.0;
			break;
	}
	_L3GD20_CS_DISABLE;
}
//2. Get Accelerometer raw data
L3GD20_DataRaw L3GD20_GetDataRaw(void)
{
	uint8_t spiBuf[2];
	L3GD20_DataRaw tempDataRaw;
	//Read X data
	L3GD20_ReadIO(L3GD20_OUT_X_L_ADDR, spiBuf, 1);
	L3GD20_ReadIO(L3GD20_OUT_X_H_ADDR, &spiBuf[1], 1);
	tempDataRaw.x = ((spiBuf[1] << 8) | spiBuf[0]);
	
	//Read Y data
	L3GD20_ReadIO(L3GD20_OUT_Y_L_ADDR, spiBuf, 1);
	L3GD20_ReadIO(L3GD20_OUT_Y_H_ADDR, &spiBuf[1], 1);
	tempDataRaw.y = ((spiBuf[1] << 8) | spiBuf[0]);
	
	//Read Z data
	L3GD20_ReadIO(L3GD20_OUT_Z_L_ADDR, spiBuf, 1);
	L3GD20_ReadIO(L3GD20_OUT_Z_H_ADDR, &spiBuf[1], 1);
	tempDataRaw.z = ((spiBuf[1] << 8) | spiBuf[0]);
	
	return tempDataRaw;
	
}
//3. Get Accelerometer mg data
L3GD20_DataScaled L3GD20_GetDataScaled(void)
{
	//Read raw data
	L3GD20_DataRaw tempRawData = L3GD20_GetDataRaw();;
	//Scale data and return 
	L3GD20_DataScaled tempScaledData;
	tempScaledData.x = (tempRawData.x * L3GD20_Sensitivity);
	tempScaledData.y = (tempRawData.y * L3GD20_Sensitivity);
	tempScaledData.z = (tempRawData.z * L3GD20_Sensitivity);
	
	return tempScaledData;
}

bool L3GD20_PollDRDY(uint32_t msTimeout)
{
	uint8_t Acc_status;
	uint32_t startTick = HAL_GetTick();
	do {
		L3GD20_ReadIO(L3GD20_STATUS_ADDR, &Acc_status, 1);
		if(Acc_status & 0x07)
			break;
	} while((Acc_status & 0x07)==0 && (HAL_GetTick() - startTick) < msTimeout);

	if(Acc_status & 0x07)
		return true;
	return false;

}
