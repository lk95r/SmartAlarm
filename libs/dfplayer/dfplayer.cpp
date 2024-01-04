#include "dfplayer.h"

DFP::DFP(void){

};

int8_t DFP::init(uart_inst_t* uart_inst,int tx_pin,int rx_pin,int baudrate){
    //TODO: Set GPIO FUNC to uart
    gpio_set_function(tx_pin,GPIO_FUNC_UART);
    gpio_set_function(rx_pin,GPIO_FUNC_UART);
    this->m_uart_inst = uart_inst;
    this->baudrate = baudrate;
    if(uart_init(this->m_uart_inst,this->baudrate)!=this->baudrate)return -1;
    //uart_set_format((uart_inst_t*)this->m_uart_inst,8,1,UART_PARITY_NONE); // try out std first.

    return 0;
};

int8_t DFP::set_track(int16_t no){
    for(int i=0;i<DFP_DEFAULT_MSG_LEN;i++){ //TODO: use memset, seemed to be unavailable in lib
        this->tx_buffer[i]=0;
    }
    this->tx_buffer[3]=CMD_TRACK_NO;
    this->tx_buffer[5]=(uint8_t)((no>>8)&0xFF); //high Byte
    this->tx_buffer[6]=(uint8_t)(no&0xFF);      //low Byte
    m_calculate_checksum();
    if(m_send_cmd()!=0)return -1;
    return 0;
}

int8_t DFP::play(void){
    for(int i=0;i<DFP_DEFAULT_MSG_LEN;i++){ //TODO: use memset, seemed to be unavailable in lib
        this->tx_buffer[i]=0;
    }
    this->tx_buffer[3]=CMD_PLAYBACK;
    this->tx_buffer[5]=0;
    this->tx_buffer[6]=0;
    m_calculate_checksum();
    if(m_send_cmd()!=0)return -1;
    return 0;
}

int8_t DFP::pause(void){
    for(int i=0;i<DFP_DEFAULT_MSG_LEN;i++){ //TODO: use memset, seemed to be unavailable in lib
        this->tx_buffer[i]=0;
    }
    this->tx_buffer[3]=CMD_PAUSE;
    m_calculate_checksum();
    if(m_send_cmd()!=0)return -1;
    return 0;
}

int8_t DFP::set_volume(uint8_t volume){
    for(int i=0;i<DFP_DEFAULT_MSG_LEN;i++){ //TODO: use memset, seemed to be unavailable in lib
        this->tx_buffer[i]=0;
    }
    if(volume<0){
        volume = 0;
    }else if(volume>30){
        volume=30;
    }
    this->tx_buffer[3]=CMD_TRACK_NO;
    this->tx_buffer[5]=0; //high Byte
    this->tx_buffer[6]=(uint8_t)(volume);      //low Byte
    m_calculate_checksum();
    if(m_send_cmd()!=0)return -1;
    return 0;
}

int8_t DFP::m_calculate_checksum(void){
    uint16_t checksum=0;
    for(int i = 1;i<7;i++){
        checksum += this->tx_buffer[i];
    }
    //might need to do checksum = -checksum here
    this->tx_buffer[7]= (uint8_t) ((checksum>>8)& 0xFF) ; //high byte
    this->tx_buffer[8]= (uint8_t) (checksum & 0xFF); // low byte
    return 0;
}

int8_t DFP::m_send_cmd(void){
    this->tx_buffer[0]=DFP_START_BYTE;
    this->tx_buffer[1]=DFP_VERSION;
    if(this->tx_buffer[2]<6)this->tx_buffer[2]=DEFAULT_CMD_LEN;
    this->tx_buffer[4]=DFP_ACK;
    this->tx_buffer[9]=DFP_END_BYTE;
    if(uart_is_writable(m_uart_inst)){
        uart_write_blocking(m_uart_inst,this->tx_buffer,DFP_DEFAULT_MSG_LEN);
    }

    return 0;
}