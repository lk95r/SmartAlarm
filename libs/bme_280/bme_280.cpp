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

    return retval;
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