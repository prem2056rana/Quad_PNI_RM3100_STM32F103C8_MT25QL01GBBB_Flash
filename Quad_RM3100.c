/*
 * RM3100QUAD.c
 *
 *  Created on: May 20, 2024
 *      Author: ASUS
 */

#include "main.h"
#include "RM3100QUAD.h"
#include "math.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdint.h"
#include "string.h"


#define SPI_SSN_HIGH 1
#define SPI_SSN_LOW 0

float gain = 38.00;  //Gain for 100 cycle count

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef currentTime;


uint8_t data_buffer[sizeof(float)];

uint32_t mag_X[4];
uint32_t mag_Y[4];
uint32_t mag_Z[4];
uint32_t magnitude[4];


uint8_t read_data[50];
int32_t signed_mag_X[4], signed_mag_Y[4], signed_mag_Z[4];

float X_axis[4];
float Y_axis[4];
float Z_axis[4];
float Magnitude[4];


uint8_t rm3100_MSB_X_buf[] = { rm3100_MSB_X };
uint8_t rm3100_MSB_Y_buf[] = { rm3100_MSB_Y };
uint8_t rm3100_MSB_Z_buf[] = { rm3100_MSB_Z };
uint8_t MSB_X_buf[] = { MSB_X };
uint8_t LSB_X_buf[] = { LSB_X };
uint8_t MSB_Y_buf[] = { MSB_Y };
uint8_t LSB_Y_buf[] = { LSB_Y };
uint8_t MSB_Z_buf[] = { MSB_Z };
uint8_t LSB_Z_buf[] = { LSB_Z };

uint8_t MSBX_r = 0x84;
uint8_t LSBX_r = 0x85;
uint8_t MSBY_r = 0x86;
uint8_t LSBY_r = 0x87;
uint8_t MSBZ_r = 0x88;
uint8_t LSBZ_r = 0x89;

uint8_t rm3100_cmm_buf[] = { rm3100_cmm };
uint8_t rm3100_cmm_mod_buf[] = { rm3100_cmm_Val };
uint8_t rm3100_tmrc_add_buf[] = { rm3100_tmrc_add };
uint8_t rm3100_tmrc_Val_buf[] = { rm3100_tmrc_Val };
uint8_t rm3100_tmrc_read_buf[] = { rm3100_tmrc_read };

uint8_t MRRW2_X_buf[] = { CC2_X };
uint8_t MRRW1_X_buf[] = { CC1_X };
uint8_t MRRW0_X_buf[] = { CC0_X };
uint8_t MRRW2_Y_buf[] = { CC2_Y };
uint8_t MRRW1_Y_buf[] = { CC1_Y };
uint8_t MRRW0_Y_buf[] = { CC0_Y };
uint8_t MRRW2_Z_buf[] = { CC2_Z };
uint8_t MRRW1_Z_buf[] = { CC1_Z };
uint8_t MRRW0_Z_buf[] = { CC0_Z };

GPIO_TypeDef* SSN_PORTS[] = {GPIOB, GPIOB, GPIOB, GPIOB};

uint16_t SSN_PINS[] = {SS1_Pin, SS2_Pin, SS3_Pin, SS4_Pin};

extern uint8_t read_X2[4], read_X1[4], read_X0[4];
extern uint8_t read_Y2[4], read_Y1[4], read_Y0[4];
extern uint8_t read_Z2[4], read_Z1[4], read_Z0[4];


void SET_COUNT(int chip_select) {
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH); //SSN HIGN
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_LOW); //SSN LOW TO COMMUNICATE WITH SENSOR
	HAL_SPI_Transmit(&hspi1, rm3100_MSB_X_buf, sizeof(rm3100_MSB_X_buf), 1000); //Not need to necessary to send multiple value or register
	//Initiate Count cycle
	HAL_SPI_Transmit(&hspi1, MSB_X_buf, sizeof(MSB_X_buf), 1000);
	HAL_SPI_Transmit(&hspi1, LSB_X_buf, sizeof(LSB_X_buf), 1000);

	HAL_SPI_Transmit(&hspi1, MSB_Y_buf, sizeof(MSB_Y_buf), 1000);
	HAL_SPI_Transmit(&hspi1, LSB_Y_buf, sizeof(LSB_Y_buf), 1000);

	HAL_SPI_Transmit(&hspi1, MSB_Z_buf, sizeof(MSB_Z_buf), 1000);
	HAL_SPI_Transmit(&hspi1, LSB_Z_buf, sizeof(LSB_Z_buf), 1000);
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH);
}
//Initiate Continuous Measurements Mode

void Continuous_Mode(int chip_select) {
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH); //SSN HIGN
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_LOW); //SSN LOW TO COMMUNICATE WITH SENSOR
	//Initiate Continuous Measurements Mode
	HAL_SPI_Transmit(&hspi1, rm3100_cmm_buf, sizeof(rm3100_cmm_buf), 1000);
	HAL_SPI_Transmit(&hspi1, rm3100_cmm_mod_buf, sizeof(rm3100_cmm_mod_buf), 1000);
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH); //SSN HIGN
}
//Initiate Time Measurements and Rate Control

void TMRC_Mode(int chip_select) {
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH); //SSN HIGN
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_LOW); //SSN LOW TO COMMUNICATE WITH SENSOR
	//initiate Time Measurements and Reset Control
	HAL_SPI_Transmit(&hspi1, rm3100_tmrc_add_buf, sizeof(rm3100_tmrc_add_buf), 1000);
	HAL_SPI_Transmit(&hspi1, rm3100_tmrc_Val_buf, sizeof(rm3100_tmrc_Val_buf), 1000);
//	HAL_SPI_Transmit(&hspi1, rm3100_tmrc_read_buf, sizeof(rm3100_tmrc_read_buf),
//			1000);
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH); //SSN HIGN
}

//Read Measurements Results
void Mea_Result(int chip_select) {
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH); //SSN HIGN
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_LOW); //SSN LOW TO COMMUNICATE WITH SENSOR

	HAL_SPI_Transmit(&hspi1, MRRW2_X_buf, sizeof(MRRW2_X_buf), 1000);
	HAL_SPI_Receive(&hspi1, &read_X2[chip_select], sizeof(read_X2[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_X1[chip_select], sizeof(read_X1[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_X0[chip_select], sizeof(read_X0[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_Y2[chip_select], sizeof(read_Y2[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_Y1[chip_select], sizeof(read_Y1[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_Y0[chip_select], sizeof(read_Y0[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_Z2[chip_select], sizeof(read_Z2[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_Z1[chip_select], sizeof(read_Z1[chip_select]), 1000);
	HAL_SPI_Receive(&hspi1, &read_Z0[chip_select], sizeof(read_Z0[chip_select]), 1000);
	HAL_GPIO_WritePin(SSN_PORTS[chip_select], SSN_PINS[chip_select], SPI_SSN_HIGH); //SSN HIGN
}

void UART_TransmitString(char *str) {
    while (*str != '\0') {
        HAL_UART_Transmit(&huart2, (uint8_t*)str, 1, HAL_MAX_DELAY);
        str++;
    }
}

//Set time
void set_time (void)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    sTime.Hours = 0x04; // set hours
    sTime.Minutes = 0x09; // set minutes
    sTime.Seconds = 0x30; // set seconds
//    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE; // Remove this line
//    sTime.TimeFormat = RTC_HOURFORMAT12_AM; // Remove this line
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
    sDate.Month = RTC_MONTH_MAY;
    sDate.Date = 0x15; // date
    sDate.Year = 0x24; // year
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
}


//Get time

void get_time(void)
{
    RTC_DateTypeDef gDate;
    RTC_TimeTypeDef gTime;
    /* Get the RTC current Time */
    HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
}


//Combine Measurements
void Comb_measurement(int chip_select) {

    // Get current time from RTC
    RTC_DateTypeDef gDate;
    RTC_TimeTypeDef gTime;
    // Get current time from RTC
    HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

    // Combine bytes for X-axis
    mag_X[chip_select] = (read_X2[chip_select] << 16) | (read_X1[chip_select] << 8) | read_X0[chip_select];
    // Extend the sign bit for negative values
    signed_mag_X[chip_select] = (mag_X[chip_select] & 0x800000) ? (mag_X[chip_select] | 0xFF000000) : mag_X[chip_select];

    // Combine bytes for Y-axis
    mag_Y[chip_select] = (read_Y2[chip_select] << 16) | (read_Y1[chip_select] << 8) | read_Y0[chip_select];
    // Extend the sign bit for negative values
    signed_mag_Y[chip_select] = (mag_Y[chip_select] & 0x800000) ? (mag_Y[chip_select] | 0xFF000000) : mag_Y[chip_select];

    // Combine bytes for Z-axis
    mag_Z[chip_select] = (read_Z2[chip_select] << 16) | (read_Z1[chip_select] << 8) | read_Z0[chip_select];
    // Extend the sign bit for negative values
    signed_mag_Z[chip_select] = (mag_Z[chip_select] & 0x800000) ? (mag_Z[chip_select] | 0xFF000000) : mag_Z[chip_select];

    // Calculate magnitude
    magnitude[chip_select] = sqrt((float)(signed_mag_X[chip_select] * signed_mag_X[chip_select]) + (signed_mag_Y[chip_select] * signed_mag_Y[chip_select]) + (signed_mag_Z[chip_select] * signed_mag_Z[chip_select]));

    // Calculate axis values
    X_axis[chip_select] = (float)signed_mag_X[chip_select] / gain;
    Y_axis[chip_select] = (float)signed_mag_Y[chip_select] / gain;
    Z_axis[chip_select] = (float)signed_mag_Z[chip_select] / gain;
    Magnitude[chip_select] = (float)magnitude[chip_select] / gain;


    // Convert float to uint8_t array

        // Write X_axis
        memcpy(data_buffer, &X_axis[chip_select], sizeof(float));
        Page_Write_4B(&hspi2, address, data_buffer, sizeof(float));
        address += sizeof(float);

        // Write Y_axis
        memcpy(data_buffer, &Y_axis[chip_select], sizeof(float));
        Page_Write_4B(&hspi2, address, data_buffer, sizeof(float));
        address += sizeof(float);

        // Write Z_axis
        memcpy(data_buffer, &Z_axis[chip_select], sizeof(float));
        Page_Write_4B(&hspi2, address, data_buffer, sizeof(float));
        address += sizeof(float);

        // Write Magnitude
        memcpy(data_buffer, &Magnitude[chip_select], sizeof(float));
        Page_Write_4B(&hspi2, address, data_buffer, sizeof(float));
        address += sizeof(float);

        // Read back data
        Bulk_Read_4B(&hspi2, 0, &read_data, sizeof(read_data));

         //Transmitting float data via UART
        myDebug(" %.2f \t", X_axis[chip_select]);
        myDebug(" %.2f \t", Y_axis[chip_select]);
        myDebug(" %.2f \t", Z_axis[chip_select]);
        myDebug(" %.2f \t", Magnitude[chip_select]);
}

void myDebug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[100];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    HAL_UART_Transmit(&huart2, (uint8_t*) buffer, strlen(buffer), HAL_MAX_DELAY);
    va_end(args);
}

int bufferSize(char *buffer) {
    int i = 0;
    while (*buffer++ != '\0')
        i++;
    return i;
}







