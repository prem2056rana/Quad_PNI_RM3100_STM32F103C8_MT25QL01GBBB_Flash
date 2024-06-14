/*
 * RM3100QUAD.h
 *
 *  Created on: May 20, 2024
 *      Author: ASUS
 */

#ifndef INC_RM3100QUAD_H_
#define INC_RM3100QUAD_H_
#include "main.h"
#include "stm32f1xx_hal_rtc.h"


//Not necessary to send multiple register address, only first address should be send then others register value automatically increments on the clock cycle
//RM3100 cycle count register address
#define rm3100_MSB_X 0x04
#define rm3100_LSB_X 0x05
#define rm3100_MSB_Y 0x06
#define rm3100_LSB_Y 0x07
#define rm3100_MSB_Z 0x08
#define rm3100_LSB_Z 0x09

//RM3100 100 Cycle Count set
#define MSB_X 0x0
#define LSB_X 0x64
#define MSB_Y 0x0
#define LSB_Y 0x64
#define MSB_Z 0x0
#define LSB_Z 0x64


//RM3100 Register map
#define rm3100_poll 0x00       //Single Measurements Mode
#define rm3100_cmm 0x01       //Continuous measurements mode
#define rm3100_cmm_Val 0x79   //initiate Continuous Measurement without Alarm
//#define rm3100_cmm_al 0xFF    //Initiate Continuous Measurement with Alarm
#define rm3100_tmrc_add 0x0B  //address of TMRC(Time Measurement and Reset Control)
                              // How often the sensor takes a new measurements and updates output data
#define rm3100_tmrc_Val 0x9A  //0.8s 300Hz
#define rm3100_tmrc_read 0x8B  //Read TMRC register
//#define rm3100_REVID_REG 0x36  //hexadecimal address for the revid internal register
//#define rm3100_status_reg 0x34 //hexadecimal address for the for status external register

//Measurement Results Register write address
#define MRRW2_X 0x24
#define MRRW1_X 0x25
#define MRRW0_X 0x26
#define MRRW2_Y 0x27
#define MRRW1_Y 0x28
#define MRRW0_Y 0x29
#define MRRW2_Z 0x2A
#define MRRW1_Z 0x2B
#define MRRW0_Z 0x2C

//Read the measurement results(//Not necessary to send multiple register address, only first address should be send then others register value automatically increments on the clock cycle )
#define CC2_X 0xA4
#define CC1_X 0xA5
#define CC0_X 0xA6
#define CC2_Y 0xA7
#define CC1_Y 0xA8
#define CC0_Y 0xA9
#define CC2_Z 0xAA
#define CC1_Z 0xAB
#define CC0_Z 0xAC


//Address in flash memory to write and read data
extern uint8_t read_data[50];
extern  uint16_t address;


extern uint8_t data_buffer[sizeof(float)];

void SET_COUNT(int chip_select);
void Continuous_Mode(int chip_select);
void TMRC_Mode(int chip_select);
void Mea_Result(int chip_select);
void Comb_measurement(int chip_select);
int bufferSize(char *buffer);
void myDebug(const char *fmt, ...);
void UART_TransmitString(char *str);
void set_time (void);
void get_time(void);
//void select_mag(uint8_t chip_select);
//void Deselect_mag(uint8_t chip_Deselect);



#endif /* INC_RM3100QUAD_H_ */
