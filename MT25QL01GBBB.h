/*
 * MT25Q.h
 *
 *  Created on: Jun 11, 2024
 *      Author: ASUS
 */

#ifndef INC_MT25Q_H_
#define INC_MT25Q_H_


/* Includes */
#include "main.h"
#include "string.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi2;
#define MAIN_FM hspi2

/*external variables or structures*/


//Basic Memory Operations
#define		RESET_ENABLE					0x66
#define 	RESET_MEMORY					0x99
#define		WRITE_ENABLE					0x06
#define		WRITE_DISABLE					0x04

//Read Manufacturer ID
#define 	READ_ID							0x9E
#define		READ_SERIAL_FLASH_PARAM			0x5A

//Read Operations
#define 	READ							0x03
#define		FAST_READ						0x0B
#define		BYTE_4_READ						0x13	//4-byte address
#define		BYTE_4_FAST_READ				0x0C	//4-byte address

//Register Read and Write operations
#define 	READ_STATUS_REG						0x05
#define 	READ_FLAG_STATUS_REG				0x70
#define		READ_NONVOLATILE_CONFIG_REG			0xB5
#define 	READ_VOLATILE_CONFIG_REG			0x85
#define 	READ_ENHANCED_VOLATILE_CONFIG_REG	0x65
#define 	READ_EXTENDED_ADDRESS_REG			0xC8
#define 	READ_GENERAL_PURPOSE_READ_REG		0x96


#define 	WRITE_STATUS_REG					0x01
#define 	WRITE_NONVOLATILE_CONFIG_REG		0xB1
#define 	WRITE_VOLATILE_CONFIG_REG			0x81
#define 	WRITE_ENHANCED_VOLATILE_CONFIG_REG	0x61
#define 	WRITE_EXTENDED_ADDRESS_REG			0xC5

#define		CLEAR_FLAG_STATUS_REGISTER			0x50

//Write or Program operations
#define		PAGE_PROGRAM					0x02
#define		BYTE_4_PAGE_PROGRAM				0x12	//4-byte address

//Erase operations
#define		SECTOR_ERASE					0xD8
#define		SUBSECTOR_ERASE_32KB			0x52
#define		SUBSECTOR_ERASE_4KB				0x20
#define		CHIP_ERASE						0xC4

#define		SECTOR_ERASE_4_BYTE				0xDC	//4-byte address
#define		SUBSECTOR_ERASE_32KB_4_BYTE		0x5C	//4-byte address
#define		SUBSECTOR_ERASE_4KB_4_BYTE		0x21	//4-byte address

//Suspend or Resume Operations
#define		PROGRAM_ERASE_SUSPEND			0x75
#define		PROGRAM_ERASE_RESUME			0x7A

//One time programmable operaions
#define		READ_OTP_ARRAY					0x4B
#define		PROGRAM_OTP_ARRAY				0x42

//4-byte address mode operations
#define		ENTER_4_BYTE_MODE				0xB7
#define		EXIT_4_BYTE_MODE				0xE9

//Deep Power down operations
#define		ENTER_DEEP_POWER_DOWN			0xB9
#define		EXIT_DEEP_POWER_DOWN			0xAB

//Sector Protection Operations
#define READ_SECTOR_PROTECTION				0x2D
#define PROGRAM_SECTOR_PROTECTION			0x2C
#define	READ_VOLATILE_LOCK_BITS				0xE8
#define WRITE_VOLATILE_LOCK_BITS			0xE5
#define READ_NONVOLATILE_LOCK_BITS			0xE2
#define WRITE_NONVOLATILE_LOCK_BITS			0xE3
#define ERASE_NONVOLATILE_LOCK_BITS			0xE4
#define READ_GLOBAL_FREEZE_BIT				0xA7
#define WRITE_GLOBAL_FREEZE_BIT				0xA6
#define READ_PASSWORD						0x27
#define WRITE_PASSWORD						0x28
#define UNLOCK_PASSWORD						0x29


#define PAGE_SIZE							256


/* Typedef structures */

/*to check the device ID of Flash memory*/
typedef struct {
	uint8_t MAN_ID;
	uint8_t M_TYPE;
	uint8_t M_CAP;
	uint8_t REM_BYTES;
	uint8_t EXT_ID;
	uint8_t DEV_INFO;
	uint8_t UID[14];
}DEVICE_ID;

/* To check the status register of the flash memory */
typedef struct{
	uint8_t READY;		//0 indicates the device is ready to read or write
	uint8_t WEN;		//0 is default
	uint8_t	BP0;
	uint8_t BP1;
	uint8_t BP2;
	uint8_t	TOP;
	uint8_t	STATUS;		//non-volatile bit, used with w# pin to control writing to status reg
}STATUS_REGISTER;


/* To check the flag status of the Flash memory */
typedef struct{
	uint8_t PROGRAM_CONTROL;
	uint8_t ERASE_SUSPEND;
	uint8_t	ERASE;
	uint8_t	PROGRAM;
	uint8_t	RESERVED;
	uint8_t	PROGRAM_SUSPEND;
	uint8_t	PROTECTION;
	uint8_t	ADDRESSING;
}FLAG_STATUS_REGISTER;


/* Functions prototype declaration */

/*Flash Initialization*/

void Configure_Flash_Pins(void);
void FLASH_INIT_EXECUTION();
void MAIN_FM_MODE();
void MSN_FM_MODE();
void MUX_DISABLE();


/* Read write Operations */
void Read_ID(SPI_HandleTypeDef *SPI,DEVICE_ID *rxData) ;
void ADDRESSING_MODE(SPI_HandleTypeDef *SPI, uint8_t mode);

void Bulk_Read(SPI_HandleTypeDef *SPI,uint32_t address, uint8_t *data, uint16_t size);
void Bulk_Read_4B(SPI_HandleTypeDef *SPI,uint32_t address, uint8_t *data, uint16_t size);
uint8_t Read_Byte_4B(SPI_HandleTypeDef *SPI, uint32_t address) ;

void Page_Write(SPI_HandleTypeDef *SPI, uint32_t address,volatile uint8_t *data,
		uint16_t size);
void Page_Write_4B(SPI_HandleTypeDef *SPI,uint32_t address, uint8_t *data, uint16_t size) ;

void Byte_Write_4B(SPI_HandleTypeDef *SPI,uint32_t address, uint8_t data);
/* Erase operations */
uint8_t Sector_Erase(SPI_HandleTypeDef *SPI,uint32_t address, uint8_t sector_size);
uint8_t Sector_Erase_4B(SPI_HandleTypeDef *SPI,uint32_t address, uint8_t sector_size);

void Chip_Erase();


/*Status Registers Read operations*/
uint8_t Status_Reg(SPI_HandleTypeDef *SPI);
uint16_t Status_Reg_NonVol(SPI_HandleTypeDef *SPI, uint16_t *data);
uint8_t Status_Reg_Flag(SPI_HandleTypeDef *SPI);
uint8_t Status_Reg_Volatile(SPI_HandleTypeDef *SPI);

uint8_t Serial_Flash_Parameter(SPI_HandleTypeDef *SPI,uint32_t address);

/* Status Register Write Operations */
void Status_Reg_Write(SPI_HandleTypeDef *SPI,uint8_t data);
void Status_Reg_Write_NonVol(SPI_HandleTypeDef *SPI,uint32_t data);
void Status_Reg_Write_Vol(SPI_HandleTypeDef *SPI,uint8_t data) ;
void Status_Reg_Write_Enhanced(SPI_HandleTypeDef *SPI,uint8_t data);
void Status_Reg_Write_Extended(SPI_HandleTypeDef *SPI,uint8_t data);

//To clear the flag status
void Status_Reg_Clear_Flag();

//To reset memory through software
void Reset_Memory();
void Write_Enable(SPI_HandleTypeDef *SPI);



#endif /* INC_MT25Q_H_ */
