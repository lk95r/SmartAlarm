#include "bme_280.h"
#include "pico/binary_info.h"

BME280::BME280(void){

}

uint BME280::init(void* i2c, uint baudrate, uint i2c_address,uint pin_scl, uint pin_sda){
    //uint retval = 0;
    //m_i2c_inst =i2c;
    //m_baudrate=baudrate;
    //m_i2c_address=i2c_address;
    //m_pin_scl=pin_scl;
    //m_pin_sda=pin_sda;
    //(i2c_init((i2c_inst_t *)m_i2c_inst,m_baudrate)==baudrate)?retval=0:retval= 255;
    //gpio_set_function(m_pin_sda,GPIO_FUNC_I2C);
    //gpio_set_function(m_pin_scl,GPIO_FUNC_I2C);
    //bi_decl(bi_2pins_with_func(m_pin_sda, m_pin_scl, GPIO_FUNC_I2C));
    //bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));
    //return retval;
    uint retval = 0;
    m_i2c_inst = i2c;
    m_baudrate = baudrate;
    m_i2c_address = i2c_address;
    m_pin_scl = pin_scl;
    m_pin_sda = pin_sda;

    // Initialize I2C interface
    if (i2c_init((i2c_inst_t *)m_i2c_inst, m_baudrate) != m_baudrate) {
        // Handle initialization error
        return 255; // Or some other error code
    }

    // Set GPIO functions
    gpio_set_function(m_pin_sda, GPIO_FUNC_I2C);
    gpio_set_function(m_pin_scl, GPIO_FUNC_I2C);
    i2c_set_slave_mode((i2c_inst_t *)i2c1, false,0);

    // Update binary info
    bi_decl(bi_2pins_with_func(m_pin_sda, m_pin_scl, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));

    // Add a delay after I2C initialization
    sleep_ms(100);
    get_calib_data();
    return retval;
}

void BME280::get_calib_data(void){
    uint8_t calib_data[BME280_LEN_TEMP_PRESS_CALIB_DATA]={0};
    uint8_t tx_buffer[1] = {BME280_REG_TEMP_PRESS_CALIB_DATA};
    int error_code;
    error_code= i2c_write_blocking((i2c_inst_t *)m_i2c_inst,m_i2c_address,tx_buffer,1,true);
    error_code = i2c_read_blocking((i2c_inst_t *)m_i2c_inst,m_i2c_address,calib_data,BME280_LEN_TEMP_PRESS_CALIB_DATA,false);
    parse_temp_pres_calib_data(calib_data,BME280_LEN_TEMP_PRESS_CALIB_DATA);
    memset(calib_data,BME280_LEN_TEMP_PRESS_CALIB_DATA,0);
    tx_buffer[0]=BME280_REG_HUMIDITY_CALIB_DATA;
    error_code = i2c_write_blocking((i2c_inst_t *)m_i2c_inst,m_i2c_address,tx_buffer,1,true);
    error_code = i2c_read_blocking((i2c_inst_t *)m_i2c_inst,m_i2c_address,calib_data,BME280_LEN_HUMIDITY_CALIB_DATA,false);
    parse_hum_calib_data(calib_data,BME280_LEN_HUMIDITY_CALIB_DATA);
}

void BME280::parse_temp_pres_calib_data(uint8_t * calib_data,uint8_t size){
 
    this->m_cal_values.dig_t1 = ((calib_data[1]<<8)|calib_data[0]);
    this->m_cal_values.dig_t2 = ((calib_data[3]<<8)|calib_data[2]);
    this->m_cal_values.dig_t3 = ((calib_data[5]<<8)|calib_data[4]);
    this->m_cal_values.dig_p1 = ((calib_data[7]<<8)|calib_data[6]);
    this->m_cal_values.dig_p2 = ((calib_data[9]<<8)|calib_data[8]);
    this->m_cal_values.dig_p3 = ((calib_data[11]<<8)|calib_data[10]);
    this->m_cal_values.dig_p4 = ((calib_data[13]<<8)|calib_data[12]);
    this->m_cal_values.dig_p5 = ((calib_data[15]<<8)|calib_data[14]);
    this->m_cal_values.dig_p6 = ((calib_data[17]<<8)|calib_data[16]);
    this->m_cal_values.dig_p7 = ((calib_data[19]<<8)|calib_data[18]);
    this->m_cal_values.dig_p8 = ((calib_data[21]<<8)|calib_data[20]);
    this->m_cal_values.dig_p9 = ((calib_data[23]<<8)|calib_data[22]);
    this->m_cal_values.dig_h1=calib_data[25];


    printf("%d",this->m_cal_values.dig_t3);
}

void BME280::parse_hum_calib_data(uint8_t* calib_data,uint8_t size){
    int16_t dig_h4_lsb;
    int16_t dig_h4_msb;
    int16_t dig_h5_lsb;
    int16_t dig_h5_msb;
    this->m_cal_values.dig_h2 = ((calib_data[1]<<8)|calib_data[0]);
    this->m_cal_values.dig_h3 = calib_data[2];



}

uint BME280::test_device_id(void){
    uint8_t send_buf[2];
    uint8_t rec_buf[2];
    int error_code;
    send_buf[0]=0xE0;
    send_buf[1]=0xB6;
    error_code = i2c_write_blocking((i2c_inst_t *)m_i2c_inst,m_i2c_address,send_buf,2,false);
    sleep_ms(2);
    printf("%d",error_code);
    send_buf[0]=0xD0;
    send_buf[1]=00;
    error_code = i2c_write_blocking((i2c_inst_t *)m_i2c_inst,m_i2c_address,send_buf,2,false);
    printf("%d",error_code);
    error_code =i2c_read_blocking((i2c_inst_t *)m_i2c_inst,m_i2c_address,rec_buf,1,false);
    printf("%d",error_code);
    printf("%d",rec_buf[0]);
    return rec_buf[0];
}