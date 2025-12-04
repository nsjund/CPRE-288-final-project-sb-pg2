#include "temp.h"

void temp_sensor_init(){
    SYSCTL_RCGCI2C_R |= 0x2;

    SYSCTL_RCGCGPIO_R |= 0x1;

    while ((SYSCTL_PRI2C_R & 0x2) == 0){};

    while ((SYSCTL_PRGPIO_R & 0x1) == 0){};

    GPIO_PORTA_DEN_R |= 0xC0;

    GPIO_PORTA_AFSEL_R |= 0xC0;//Pin 6-7??!?!??!!!?

    GPIO_PORTA_ODR_R |= 0x80;

    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0x00FFFFFF) | 0x33000000;

    I2C1_MCR_R = 0x10;

    I2C1_MTPR_R = 0x07;
}


uint16_t temp_read_amb(){
    uint16_t ret_val = 0;

    I2C1_MSA_R = 0x5A << 1;//Master slave address of 5A set to transmit

    I2C1_MDR_R = 0x06;

    I2C1_MCS_R =  0x3;

    while(I2C1_MCS_R & 0x1){};

    I2C1_MSA_R = (I2C1_MSA_R & ~0xFF) | (0x5A << 1) | 0x1;

    I2C1_MCS_R = (I2C1_MCS_R & ~0x1F) | 0xB;

    while(I2C1_MCS_R & 0x1){};

    ret_val += I2C1_MDR_R & 0xFF;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x9;

    while(I2C1_MCS_R & 0x1){};

    ret_val += (I2C1_MDR_R << 8) & 0xFF00;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x5;

    while(I2C1_MCS_R & 0x1){};

    uint8_t pecByte = I2C1_MDR_R;

    return ret_val;
}

uint16_t temp_read(){
    uint16_t ret_val = 0;

    I2C1_MSA_R = 0x5A << 1;//Master slave address of 5A set to transmit

    I2C1_MDR_R = 0x07;

    I2C1_MCS_R =  0x3;

    while(I2C1_MCS_R & 0x1){};

    I2C1_MSA_R = (I2C1_MSA_R & ~0xFF) | (0x5A << 1) | 0x1;

    I2C1_MCS_R = (I2C1_MCS_R & ~0x1F) | 0xB;

    while(I2C1_MCS_R & 0x1){};

    ret_val += I2C1_MDR_R & 0xFF;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x9;

    while(I2C1_MCS_R & 0x1){};

    ret_val += (I2C1_MDR_R << 8) & 0xFF00;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x5;

    while(I2C1_MCS_R & 0x1){};

    uint8_t pecByte = I2C1_MDR_R;

    return ret_val;
}
