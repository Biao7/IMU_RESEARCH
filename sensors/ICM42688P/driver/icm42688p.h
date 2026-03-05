/*
    author: Conbiao
    date: 2026/02/28
    
    This is the code of the ICM-42688P driver.
*/

#ifndef __ICM42688P_H__
#define __ICM42688P_H__

#include "icm42688p.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static const char *TAG = "ICM42688P";


#define I2C_MASTER_SCL_IO            (gpio_num_t)36          /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO            (gpio_num_t)37          /*!< gpio number for I2C master data  */
#define I2C_IRQ_NUM                  (gpio_num_t)38          /*!< interrupt number for I2C master */
#define I2C_MASTER_NUM               (i2c_port_t)I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ           (100000)                /*!< I2C master clock frequency */

#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define ICM42688P_WHO_AM_I          (0x47)                    /*!< WHO_AM_I value of ICM-42688P */

#define ICM42688P_ADDR                 (0x68)                    /*!< ICM-42688P device address */
#define ICM42688P_WHO_AM_I_ADDR        (0x75)                    /*!< WHO_AM_I register address */
#define ICM42688P_PWR_MGMT_0_ADDR      (0x4e)                    /*!< PWR_MGMT_0 register address */
#define ICM42688P_GYRO_CONFIG_0_ADDR   (0x4F)                    /*!< GYRO_CONFIG_0 register address */
#define ICM42688P_ACCEL_CONFIG_0_ADDR  (0x50)                    /*!< ACCEL_CONFIG_0 register address */
#define ICM42688P_DEVICE_CONFIG_ADDR   (0x11)                    /*!< DEVICE_CONFIG register address */
#define ICM42688P_INT_CONFIG_ADDR      (0x14)                    /*!< INT_CONFIG register address */
#define ICM42688P_ACCEL_DATA_X1_ADDR   (0x1F)                    /*!< ACCEL_DATA_X1 register address */
#define ICM42688P_ACCEL_DATA_X0_ADDR   (0x20)                    /*!< ACCEL_DATA_X0 register address */
#define ICM42688P_ACCEL_DATA_Y1_ADDR   (0x21)                    /*!< ACCEL_DATA_Y1 register address */
#define ICM42688P_ACCEL_DATA_Y0_ADDR   (0x22)                    /*!< ACCEL_DATA_Y0 register address */
#define ICM42688P_ACCEL_DATA_Z1_ADDR   (0x23)                    /*!< ACCEL_DATA_Z1 register address */
#define ICM42688P_ACCEL_DATA_Z0_ADDR   (0x24)                    /*!< ACCEL_DATA_Z0 register address */
#define ICM42688P_GYRO_DATA_X1_ADDR    (0x25)                    /*!< GYRO_DATA_X1 register address */
#define ICM42688P_GYRO_DATA_X0_ADDR    (0x26)                    /*!< GYRO_DATA_X0 register address */
#define ICM42688P_GYRO_DATA_Y1_ADDR    (0x27)                    /*!< GYRO_DATA_Y1 register address */
#define ICM42688P_GYRO_DATA_Y0_ADDR    (0x28)                    /*!< GYRO_DATA_Y0 register address */
#define ICM42688P_GYRO_DATA_Z1_ADDR    (0x29)                    /*!< GYRO_DATA_Z1 register address */
#define ICM42688P_GYRO_DATA_Z0_ADDR    (0x30)                    /*!< GYRO_DATA_Z0 register address */

#include <math.h>

#define ICM42688P_ACCEL_SENS_16G    2048.0f
#define ICM42688P_GYRO_SENS_2000DPS  16.4f
#define DEG_TO_RAD                  (M_PI / 180.0)

// Global variables
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

// Function prototypes
static void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle);
static esp_err_t icm42688p_register_read(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len);
static esp_err_t icm42688p_register_write_byte(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data);
static int icm42688p_reset();
static int icm42688p_set_default_mode();
static int icm42688p_set_accel_odr(uint8_t odr);
static int get_who_am_i();
static int icm42688p_get_accel_reg_data(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
static int icm42688p_get_gyro_reg_data(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);
static int icm42688p_get_accel_data(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
static int icm42688p_get_gyro_data(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);
void icm42688p_init(void);

#endif /* __ICM42688P_H__ */
