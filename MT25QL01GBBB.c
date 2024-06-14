/*
 * MT25QL.c
 *
 *  Created on: Aug 22, 2023
 *      Author: User
 */
#include "main.h"
#include "MT25Q.h"


/* static functions prototype declaration */
static void FM_Enable(SPI_HandleTypeDef *SPI);
static void FM_Disable(SPI_HandleTypeDef *SPI);
static void Reset_Enable();

/*----------------------*/

/*
 * @brief	to read the FM identification data
 *
 * @param	SPI			pointer to the handle of SPI connected to Flash from which to read data
 * @param	*rxData		typedef pointer to the data to be read
 *
 * @retval	20 bytes of specific data can be read
 */
void Read_ID(SPI_HandleTypeDef *SPI, DEVICE_ID *rxData) {
	uint8_t cmd = READ_ID;
	DEVICE_ID buff;
	uint8_t data[20];
	int i;
	FM_Enable(SPI);
	HAL_Delay(1);
	HAL_SPI_Transmit(SPI, &cmd, 1, 300);
	HAL_SPI_Receive(SPI, data, 20, 1000);
	HAL_Delay(1);
	FM_Disable(SPI);
	HAL_Delay(500);
	buff.MAN_ID = data[0];
	buff.M_TYPE = data[1];
	buff.M_CAP = data[2];
	buff.REM_BYTES = data[3];
	buff.EXT_ID = data[4];
	buff.DEV_INFO = data[5];
//	for (i = 6; i < 20; i++) {
//		buff.UID[i] = data[i];
//	}
	*rxData = buff;
	return;
}

/*
 * @brief  use 4-byte addressing mode for flash operation
 *
 * @param	uint8_t flag to specify weather to enter 4-byte mode or exit 4-byte mode
 * 			0: exit 4-byte addresssing mode [to 3-BYTE mode]
 * 			1: enter 4-byte addressing mode
 * @retval	none
 *
 */
void ADDRESSING_MODE(SPI_HandleTypeDef *SPI, uint8_t mode) {
	uint8_t CMD;
	switch (mode) {
	case 0:
		CMD = EXIT_4_BYTE_MODE;
		FM_Enable(SPI);
		HAL_SPI_Transmit(SPI, &CMD, 1, 100);
		FM_Disable(SPI);
		HAL_Delay(1);
		break;
	case 1:
		CMD = ENTER_4_BYTE_MODE;
		FM_Enable(SPI);
		HAL_SPI_Transmit(SPI, &CMD, 1, 100);
		FM_Disable(SPI);
		HAL_Delay(1);
		break;
	default:
		break;
	}
}

/*
 * @brief	function to read array of data of any size
 *
 * @param	SPI			pointer to the handle of SPI connected to Flash from which to read data
 * @param	address		address from which the data is to be read
 * 			*data		pointer to store the data which is read from the corresponding address
 * 			size		size of the data to be read
 */
void Bulk_Read(SPI_HandleTypeDef *SPI, uint32_t address, uint8_t *data,
		uint16_t size) {
	uint8_t cmd[4];
	cmd[0] = READ;
	cmd[1] = (uint8_t) (address >> 16 & 0xff);
	cmd[2] = (uint8_t) (address >> 8 & 0xff);
	cmd[3] = (uint8_t) (address & 0xff);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, cmd, 4, 100);
	HAL_SPI_Receive(SPI, data, size, 300);
	FM_Disable(SPI);
	HAL_Delay(10);
	return;
}

/*
 * @brief	function to read a byte of datum of the given address in 4-byte addressing mode
 *
 * @param	SPI			pointer to the handle of SPI connected to Flash from which to read data
 * @param	address		address from which the data is to be read
 *
 * @retval	data of the corresponding addresses of given size
 */
uint8_t Read_Byte_4B(SPI_HandleTypeDef *SPI, uint32_t address) {
	uint8_t cmd[5];
	uint8_t data = 0;
	cmd[0] = BYTE_4_READ;
	cmd[1] = (uint8_t) (address >> 24 & 0xff);
	cmd[2] = (uint8_t) (address >> 16 & 0xff);
	cmd[3] = (uint8_t) (address >> 8 & 0xff);
	cmd[4] = (uint8_t) (address & 0xff);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, cmd, 5, 100);
	HAL_SPI_Receive(SPI, &data, 1, 200);
	FM_Disable(SPI);
	HAL_Delay(1);
	return data;
}

/*
 * @brief	similar function as Bulk_Read but 4 byte address mode for the FM
 *
 *
 * @param	SPI			pointer to the handle of SPI connected to Flash from which to read data
 * * @param	address		address from which the data is to be read
 * 			*data		pointer to store the data which is read from the corresponding address
 * 			size		size of the data to be read
 */
void Bulk_Read_4B(SPI_HandleTypeDef *SPI, uint32_t address, uint8_t *data,
		uint16_t size) {
	uint8_t cmd[5];
	cmd[0] = BYTE_4_READ;
	cmd[1] = (uint8_t) (address >> 24 & 0xff);
	cmd[2] = (uint8_t) (address >> 16 & 0xff);
	cmd[3] = (uint8_t) (address >> 8);
	cmd[4] = (uint8_t) (address);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, cmd, 5, 100);
	HAL_SPI_Receive(SPI, data, size, 200);
	FM_Disable(SPI);
	HAL_Delay(5);
	return;
}

/*
 * @brief	to write to a page(256 bytes) of the FM 3-byte-addressing mode
 *
 * @param	address		4 byte starting address from which data is to be read
 * 			*data		data to be written into the address
 * 			size		size of the data
 *@retval	none
 */
void Page_Write(SPI_HandleTypeDef *SPI, uint32_t address,
		volatile uint8_t *data, uint16_t size) {
	uint8_t cmd = PAGE_PROGRAM;
	uint8_t command[3];
	while (size > PAGE_SIZE) {
		command[0] = (uint8_t) (address >> 16 & 0xff);
		command[1] = (uint8_t) (address >> 8 & 0xff);
		command[2] = (uint8_t) (address & 0xff);
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_Delay(1);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, command, 3, 200);
		HAL_SPI_Transmit(SPI, data, PAGE_SIZE, 200);
		FM_Disable(SPI);
		HAL_Delay(5);
		size = size - PAGE_SIZE;
		data = data + 256;
		address = address + 256;
	}
	command[0] = (uint8_t) (address >> 16 & 0xff);
	command[1] = (uint8_t) (address >> 8 & 0xff);
	command[2] = (uint8_t) (address & 0xff);
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_Delay(1);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, command, 3, 200);
	HAL_SPI_Transmit(SPI, data, size, 200);
	FM_Disable(SPI);
	HAL_Delay(5);
	return;
}
/*
 * @brief	to write to a page(256 bytes) of the FM 4-byte-addressing-mode
 *
 * @param	address		4 byte starting address from which data is to be read
 * 			*data		data to be written into the address
 * 			size		size of the data
 *@retval	none
 */

void Page_Write_4B(SPI_HandleTypeDef *SPI, uint32_t address, uint8_t *data,
		uint16_t size) {
	uint8_t cmd = BYTE_4_PAGE_PROGRAM;
	uint8_t command[4];
	while (size > PAGE_SIZE) {
		command[0] = (uint8_t) (address >> 24 & 0xff);
		command[1] = (uint8_t) (address >> 16 & 0xff);
		command[2] = (uint8_t) (address >> 8 & 0xff);
		command[3] = (uint8_t) (address & 0xFF);
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_Delay(1);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, command, 4, 200);
		HAL_SPI_Transmit(SPI, data, PAGE_SIZE, 200);
		FM_Disable(SPI);
		HAL_Delay(10);
		size = size - PAGE_SIZE;
		data = data + 256;
		address = address + 256;
	}
	command[0] = (uint8_t) (address >> 24 & 0xff);
	command[1] = (uint8_t) (address >> 16 & 0xff);
	command[2] = (uint8_t) (address >> 8 & 0xff);
	command[3] = (uint8_t) (address & 0xFF);
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_Delay(1);
	HAL_SPI_Transmit(SPI, &cmd, 1, 50);
	HAL_SPI_Transmit(SPI, command, 4, 100);
	HAL_Delay(1);
	HAL_SPI_Transmit(SPI, data, size, 200);
	FM_Disable(SPI);
	HAL_Delay(10);
	return;
}

/*
 * @brief	to write one byte of data in 4-byte-addressing mode
 *
 * @param	SPI handle of spi on which the flash memory is connected
 * @param	address		address of the location on where to write data
 * @param	data		byte of data to be written
 *
 * @retval none
 */
void Byte_Write_4B(SPI_HandleTypeDef *SPI, uint32_t address, uint8_t data) {
	uint8_t cmd = BYTE_4_PAGE_PROGRAM;
	uint8_t command[4];
	command[0] = (uint8_t) (address >> 24 & 0xff);
	command[1] = (uint8_t) (address >> 16 & 0xff);
	command[2] = (uint8_t) (address >> 8 & 0xff);
	command[3] = (uint8_t) (address & 0xff);
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, command, 4, 200);
	HAL_SPI_Transmit(SPI, &data, 1, 100);
	FM_Disable(SPI);
	HAL_Delay(1);
	return;
}

/************************ Erase commands ******************************************/

/*
 * @brief	Functions to Erase the Sectors of Corresponding size
 * 			Suffix gives the size of the Sector e.g. 32 = 32 KB subsector
 *
 * 	@param	address		any address(starting, middle or end) of the sector or subsector
 *
 * 	@retval 1/2/3/4 if the operation is successful
 * 			0 if the operation failed or input was invalid
 */
uint8_t Sector_Erase(SPI_HandleTypeDef *SPI, uint32_t address,
		uint8_t sector_size) {
	uint8_t addr[4];
	uint8_t cmd = 0;
	addr[0] = (uint8_t) (address >> 24 & 0xff);
	addr[1] = (uint8_t) (address >> 16 & 0xff);
	addr[2] = (uint8_t) (address >> 8 & 0xff);
	addr[3] = (uint8_t) (address & 0xFF);
	switch (sector_size) {
	case 64:
		cmd = SECTOR_ERASE;
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_Delay(1);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, addr, 3, 200);
		FM_Disable(SPI);
		HAL_Delay(1050);
		return 1;
	case 32:
		cmd = SUBSECTOR_ERASE_32KB;
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, addr, 3, 200);
		FM_Disable(SPI);
		HAL_Delay(1000);
		return 2;
	case 4:
		cmd = SUBSECTOR_ERASE_4KB;
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, addr, 3, 200);
		FM_Disable(SPI);
		HAL_Delay(500);
		return 3;
	default:
		return 0;
	}
}

/*
 * @brief	similar function as Sector_Erase but 4 byte address mode for the FM
 * 			Parameters are same as for Sector_Erase
 */
uint8_t Sector_Erase_4B(SPI_HandleTypeDef *SPI, uint32_t address, uint8_t sector_size) {
	uint8_t addr[4];
	uint8_t cmd;
	addr[0] = (uint8_t) (address >> 24);
	addr[1] = (uint8_t) (address >> 16);
	addr[2] = (uint8_t) (address >> 8);
	addr[3] = (uint8_t) (address);
	switch (sector_size) {
	case 64:
		cmd = SECTOR_ERASE_4_BYTE;
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, addr, 4, 100);
		FM_Disable(SPI);
		HAL_Delay(1000);
		return 1;
	case 32:
		cmd = SUBSECTOR_ERASE_32KB_4_BYTE;
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, addr, 4, 100);
		FM_Disable(SPI);
		HAL_Delay(1000);
		return 2;
	case 4:
		cmd = SUBSECTOR_ERASE_4KB_4_BYTE;
		Write_Enable(SPI);
		FM_Enable(SPI);
		HAL_SPI_Transmit(SPI, &cmd, 1, 100);
		HAL_SPI_Transmit(SPI, addr, 4, 100);
		FM_Disable(SPI);
		HAL_Delay(1000);
		return 3;
	default:
		return 0;
	}
}

/*
 * @brief	Erases the Flash Memory completely
 * 			Takes around 7.7 minutes to erase the whole chip
 *
 * @param	none
 *
 * @retval	none
 */
void Chip_Erase(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = CHIP_ERASE;
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 300);
	FM_Disable(SPI);
	HAL_Delay(920000);
}

/************************************************************************************/

/*----------------------------Status Register functions-----------------------------*/

/*
 * @brief	Functions below Read Registers of the Flash
 *
 * @param	none
 *
 * @retval	Register status of FM
 */
uint8_t Status_Reg(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = READ_STATUS_REG;
	uint8_t data;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 500);
	HAL_Delay(1);
	HAL_SPI_Receive(SPI, &data, 1, 500);
	FM_Disable(SPI);
	return data;
}

/*
 *@retval  returns the 8 bit data of flag status
 */
uint8_t Status_Reg_Flag(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = READ_FLAG_STATUS_REG;
	uint8_t data;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Receive(SPI, &data, 1, 100);
	FM_Disable(SPI);
	return data;
}

/*
 *@retval  returns the 16 bit data of nonvolatile configuration register
 */
uint16_t Status_Reg_NonVol(SPI_HandleTypeDef *SPI, uint16_t *data) {
	uint8_t cmd = READ_NONVOLATILE_CONFIG_REG;
	uint8_t temp[2];
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Receive(SPI, temp, 2, 100);
	FM_Disable(SPI);
	*data = temp[1] << 8 | temp[0];
	return *data;
}

/*
 * @retval	returns the 8 bit Volatile Configuration Register
 */
uint8_t Status_Reg_Volatile(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = READ_VOLATILE_CONFIG_REG;
	uint8_t data = 0;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Receive(SPI, &data, 1, 100);
	FM_Disable(SPI);
	return data;
}

/*
 * @retval	returns the 8 bit Enhanced Volatile Configuration Register
 */
uint8_t Status_Reg_Enhanced(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = READ_ENHANCED_VOLATILE_CONFIG_REG;
	uint8_t data = 0;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Receive(SPI, &data, 1, 100);
	FM_Disable(SPI);
	return data;
}

/*
 * @retval	returns the 8 bit Extended address register
 */
uint8_t Status_Reg_Extended(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = READ_EXTENDED_ADDRESS_REG;
	uint8_t data = 0;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Receive(SPI, &data, 1, 100);
	FM_Disable(SPI);
	return data;
}

/*
 * @retval	returns the 8 bit General Purpose register
 */
uint8_t Status_Reg_General(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = READ_GENERAL_PURPOSE_READ_REG;
	uint8_t data = 0;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_Delay(1);
	HAL_SPI_Receive(SPI, &data, 1, 100);
	FM_Disable(SPI);
	return data;
}

/*
 * @brief	it returns the corresponding commands for the different FM operations
 * 			refer to:Micron TN-25-06: Serial Flash Discovery Parameters for parameters data
 */
uint8_t Serial_Flash_Parameter(SPI_HandleTypeDef *SPI, uint32_t address) {
	uint8_t cmd[4];
	uint8_t data;
	cmd[0] = READ_SERIAL_FLASH_PARAM;
	cmd[1] = (uint8_t) (address >> 16);
	cmd[2] = (uint8_t) (address >> 8);
	cmd[3] = (uint8_t) (address);
	FM_Enable(SPI);
	HAL_Delay(1);
	HAL_SPI_Transmit(SPI, cmd, 4, 100);
	HAL_Delay(10);
	HAL_SPI_Receive(SPI, &data, 1, 100);
	FM_Disable(SPI);
	HAL_Delay(1);
	return data;

}

/*------------------------------Status regiser read end-----------------------------*/

/****************************status register write***********************************/
void Status_Reg_Write(SPI_HandleTypeDef *SPI, uint8_t data) {
	uint8_t cmd = WRITE_STATUS_REG;
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, &data, 1, 100);
	FM_Disable(SPI);
	HAL_Delay(100);
}

void Status_Reg_Write_NonVol(SPI_HandleTypeDef *SPI, uint32_t data) {
	uint8_t cmd = WRITE_NONVOLATILE_CONFIG_REG;
	uint8_t command[2];
	command[0] = (uint8_t) (data >> 8);
	command[1] = (uint8_t) (data);
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_Delay(1);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, command, 2, 100);
	FM_Disable(SPI);
	HAL_Delay(1000);
}

void Status_Reg_Write_Vol(SPI_HandleTypeDef *SPI, uint8_t data) {
	uint8_t cmd = WRITE_VOLATILE_CONFIG_REG;
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, &data, 1, 100);
	FM_Disable(SPI);
	HAL_Delay(100);
}

void Status_Reg_Write_Enhanced(SPI_HandleTypeDef *SPI, uint8_t data) {
	uint8_t cmd = WRITE_ENHANCED_VOLATILE_CONFIG_REG;
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, &data, 1, 100);
	FM_Disable(SPI);
	HAL_Delay(50);
}

void Status_Reg_Write_Extended(SPI_HandleTypeDef *SPI, uint8_t data) {
	uint8_t cmd = WRITE_EXTENDED_ADDRESS_REG;
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, &data, 1, 100);
	FM_Disable(SPI);
	HAL_Delay(50);
}

void Status_Reg_Clear_Flag(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = CLEAR_FLAG_STATUS_REGISTER;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	FM_Disable(SPI);
	HAL_Delay(10);
}

/*******************************Sector Protection ******************/

/*
 * @breif	function to read which sectors of the FM are protected
 *
 * @param	none
 *
 * @retval	returns 16 bit sector protection status of FM
 */
uint16_t Read_Sector_Protection(SPI_HandleTypeDef *SPI, uint16_t *data) {
	uint8_t cmd = READ_SECTOR_PROTECTION;
	uint8_t temp[2];
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Receive(SPI, temp, 2, 100);
	FM_Disable(SPI);
	*data = temp[1] << 8 | temp[0];
	return *data;
}

/*
 * @brief	function to write the Sector Protection of the FM to lock/unlock sectors
 *
 * @param	data	command to be written to the FM to Lock/Unlock sectors
 *
 * @retval	none
 */
void Write_Sector_Protection(SPI_HandleTypeDef *SPI, uint16_t data) {
	uint8_t cmd = PROGRAM_SECTOR_PROTECTION;
	uint8_t dummy[2];
	dummy[0] = (uint8_t) (data >> 8);
	dummy[1] = (uint8_t) (data);
	Write_Enable(SPI);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	HAL_SPI_Transmit(SPI, dummy, 2, 100);
	FM_Disable(SPI);
}

/*******************************status register write end**************************/
/*
 *@brief	function resets the Memory through software
 *
 *@param	none
 *
 *@retval	none
 */
void Reset_Memory(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = RESET_MEMORY;
	Reset_Enable(SPI);					//reset enable is a must to reset memory
	HAL_Delay(1);
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 200);
	FM_Disable(SPI);
	HAL_Delay(500);
}

static void Reset_Enable(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = RESET_ENABLE;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	FM_Disable(SPI);
}

void Write_Enable(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = WRITE_ENABLE;
	FM_Enable(SPI);
	HAL_Delay(1);
	HAL_SPI_Transmit(SPI, &cmd, 1, 500);
	FM_Disable(SPI);
	HAL_Delay(10);
}

static void Write_Disable(SPI_HandleTypeDef *SPI) {
	uint8_t cmd = WRITE_DISABLE;
	FM_Enable(SPI);
	HAL_SPI_Transmit(SPI, &cmd, 1, 100);
	FM_Disable(SPI);
}

static void FM_Enable(SPI_HandleTypeDef *SPI) {
	if (SPI == &MAIN_FM) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_Delay(1);
	}
}

static void FM_Disable(SPI_HandleTypeDef *SPI) {
	if (SPI == &MAIN_FM) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_Delay(1);
	}

}

