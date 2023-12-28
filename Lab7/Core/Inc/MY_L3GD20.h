
//Header files
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <string.h>

//List of Defines
//1. LIS3DHS registers addresses
#define L3GD20_WHO_AM_I_ADDR                0x0F
//#define L3GD20_OFF_X_ADDR                   0x10
//#define L3GD20_OFF_Y_ADDR                   0x11
//#define L3GD20_OFF_Z_ADDR                   0x12

#define L3GD20_CTRL_REG1_ADDR               0x20
#define L3GD20_CTRL_REG2_ADDR               0x21
#define L3GD20_CTRL_REG3_ADDR               0x22
#define L3GD20_CTRL_REG4_ADDR               0x23              
#define L3GD20_CTRL_REG5_ADDR               0x24
//#define L3GD20_CTRL_REG6_ADDR               0x25

#define L3GD20_STATUS_ADDR                  0x27

#define L3GD20_OUT_X_L_ADDR                 0x28
#define L3GD20_OUT_X_H_ADDR                 0x29
#define L3GD20_OUT_Y_L_ADDR                 0x2A
#define L3GD20_OUT_Y_H_ADDR                 0x2B
#define L3GD20_OUT_Z_L_ADDR                 0x2C
#define L3GD20_OUT_Z_H_ADDR                 0x2D

//REG1. Data rate and bandwidth: refer to table 21
#define L3GD20_DATARATE_95HZ            ((uint8_t)0x00)  /* 95 Hz Normal Mode */
#define L3GD20_DATARATE_190HZ           ((uint8_t)0x40)  /* 190Hz Normal Mode */
#define L3GD20_DATARATE_380HZ           ((uint8_t)0x80)  /* 380Hz Normal Mode */
#define L3GD20_DATARATE_760HZ           ((uint8_t)0xC0)  /* 760Hz Normal Mode */

#define L3GD20_POWERDOWN_MASK           ((uint8_t)0xF7)  /* Apply mask to force powerdown */

//REG1. Enabled axis 
#define L3GD20_X_ENABLE                     ((uint8_t)0x01)
#define L3GD20_Y_ENABLE                     ((uint8_t)0x02)
#define L3GD20_Z_ENABLE                     ((uint8_t)0x04)
#define L3GD20_XYZ_ENABLE                   ((uint8_t)0x07)

//3. Full scale & sensitivity
#define L3GD20_FULLSCALE_250DPS             ((uint8_t)0b00)
#define L3GD20_FULLSCALE_500DPS             ((uint8_t)0b01)
#define L3GD20_FULLSCALE_2000DPS            ((uint8_t)0b10)

#define L3GD20_FULLSCALE_250DPS_SENSITIVITY  0.00875
#define L3GD20_FULLSCALE_500DPS_SENSITIVITY  0.0175
#define L3GD20_FULLSCALE_2000DPS_SENSITIVITY 0.070

//Typedefs
//1. Accelerometer Configuration struct
typedef struct 
{
	uint8_t dataRate;
	uint8_t enableAxes;
	uint8_t fullScale;
	bool interruptEnable;
}L3GD20_InitTypeDef;

//2. Accelerometer raw data
typedef struct 
{
	int16_t x;
	int16_t y;
	int16_t z;
}L3GD20_DataRaw;

//3. Accelerometer mg data (scaled data)
typedef struct
{
	float x;
	float y;
	float z;
}L3GD20_DataScaled;

//Functions prototypes
//Private functions
//1. Write IO
void L3GD20_WriteIO(uint8_t reg, uint8_t *dataW, uint8_t size);
//2. Read IO
void L3GD20_ReadIO(uint8_t reg, uint8_t *dataR, uint8_t size);


//1. Accelerometer initialise function
void L3GD20_Init(SPI_HandleTypeDef *accSPI, L3GD20_InitTypeDef *accInitDef);
//2. Get Accelerometer raw data
L3GD20_DataRaw L3GD20_GetDataRaw(void);
//3. Get Accelerometer mg data
L3GD20_DataScaled L3GD20_GetDataScaled(void);
//4. Poll for Data Ready
bool L3GD20_PollDRDY(uint32_t msTimeout);


