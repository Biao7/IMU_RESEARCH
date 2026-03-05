/*
    author: Conbiao
    date: 2026/02/28
    
    This is the code of the ICM-42688P driver.
*/

static void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle)
{
    i2c_master_bus_config_t bus_cfg = { 
        .i2c_port = I2C_MASTER_NUM,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .clock_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .dev_addr = ICM42688P_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle));

}

// Read data from ICM-42688P register
static esp_err_t icm42688p_register_read(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static esp_err_t icm42688p_register_write_byte(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static int icm42688p_reset()
{
    ESP_LOGI(TAG, "%s start!",__func__);

    // Reset the device
    icm42688p_register_write_byte(dev_handle, ICM42688P_PWR_MGMT_0_ADDR, 0x01);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    return 0;
}

static int icm42688p_set_default_mode()
{
    ESP_LOGI(TAG, "%s start!",__func__);

    // Set default mode
    icm42688p_register_write_byte(dev_handle, ICM42688P_PWR_MGMT_0_ADDR, 0x0f);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "%s end!",__func__);
}

static int icm42688p_set_accel_odr(uint8_t odr)
{
    ESP_LOGI(TAG, "%s start!",__func__);

    // Set ACCEL ODR
    icm42688p_register_write_byte(dev_handle, ICM42688P_ACCEL_CONFIG_0_ADDR, odr);
    vTaskDelay(100 / portTICK_PERIOD_MS);

}

static int get_who_am_i()
{
    ESP_LOGI(TAG, "%s start!",__func__);

    uint8_t who_am_i = 0;
    icm42688p_register_read(dev_handle, ICM42688P_WHO_AM_I_ADDR, &who_am_i, 1);

    ESP_LOGI(TAG, "WHO_AM_I: 0x%02x", who_am_i);

    return who_am_i;
}

static int icm42688p_get_accel_reg_data(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z)
{
    ESP_LOGI(TAG, "%s start!",__func__);

    uint8_t data[6];
    icm42688p_register_read(dev_handle, ICM42688P_ACCEL_DATA_X1_ADDR, data, 6);
    *accel_x = (int16_t)(data[0] << 8 | data[1]);
    *accel_y = (int16_t)(data[2] << 8 | data[3]);
    *accel_z = (int16_t)(data[4] << 8 | data[5]);

    ESP_LOGI(TAG, "ACCEL_X: %d, ACCEL_Y: %d, ACCEL_Z: %d", *accel_x, *accel_y, *accel_z);
}

static int icm42688p_get_gyro_reg_data(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z)
{
    ESP_LOGI(TAG, "%s start!",__func__);

    uint8_t data[6];
    icm42688p_register_read(dev_handle, ICM42688P_GYRO_DATA_X1_ADDR, data, 6);
    *gyro_x = (int16_t)(data[0] << 8 | data[1]);
    *gyro_y = (int16_t)(data[2] << 8 | data[3]);
    *gyro_z = (int16_t)(data[4] << 8 | data[5]);

    ESP_LOGI(TAG, "GYRO_X: %d, GYRO_Y: %d, GYRO_Z: %d", *gyro_x, *gyro_y, *gyro_z);
}

static int icm42688p_get_accel_data(float *accel_x, float *accel_y, float *accel_z)
{
    ESP_LOGI(TAG, "%s start!",__func__);

    int16_t accel_x_reg, accel_y_reg, accel_z_reg;
    icm42688p_get_accel_reg_data(&accel_x_reg, &accel_y_reg, &accel_z_reg);

    *accel_x = (float)accel_x_reg / ICM42688P_ACCEL_SENS_16G;
    *accel_y = (float)accel_y_reg / ICM42688P_ACCEL_SENS_16G;   
    *accel_z = (float)accel_z_reg / ICM42688P_ACCEL_SENS_16G;

    //ESP_LOGI(TAG, "ACCEL_X: %f, ACCEL_Y: %f, ACCEL_Z: %f", *accel_x, *accel_y, *accel_z);
}

static int icm42688p_get_gyro_data(float *gyro_x, float *gyro_y, float *gyro_z)
{
    ESP_LOGI(TAG, "%s start!",__func__);
    int16_t gyro_x_reg, gyro_y_reg, gyro_z_reg;
    icm42688p_get_gyro_reg_data(&gyro_x_reg, &gyro_y_reg, &gyro_z_reg);

    *gyro_x = (float)gyro_x_reg / ICM42688P_GYRO_SENS_2000DPS;
    *gyro_y = (float)gyro_y_reg / ICM42688P_GYRO_SENS_2000DPS;
    *gyro_z = (float)gyro_z_reg / ICM42688P_GYRO_SENS_2000DPS;

    //ESP_LOGI(TAG, "GYRO_X: %f, GYRO_Y: %f, GYRO_Z: %f", *gyro_x, *gyro_y, *gyro_z);
}

void icm42688p_init(void)
{
    ESP_LOGI(TAG, "%s start!", __func__);

    //初始化ESP32S3的i2c master
    i2c_master_init(&bus_handle, &dev_handle);

    //复位ICM-42688P
    icm42688p_reset();

    //获取ICM-42688P的WHO_AM_I
    if(get_who_am_i() != ICM42688P_WHO_AM_I)
    {
        ESP_LOGE(TAG, "ICM-42688P not found!");
        return;
    }

    //设置ACCEL和GYRO的工作模式
    icm42688p_set_default_mode();

    //设置ACCEL的量程和ODR,ACCEL默认使用的是2000g和1000Hz

    //设置GYRO的量程和ODR,GYRO默认使用的是2000dps和1000Hz

    //设置INT1和INT2的配置
    
}
