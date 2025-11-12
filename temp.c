#include "temp.h"

void temp_sensor_init(){
    SYSCTL_RCGCI2C_R |= 0x2;

    SYSCTL_RCGCGPIO_R |= 0x1;

    while ((SYSCTL_PRI2C_R & 0x2) == 0){};

    while ((SYSCTL_PRGPIO_R & 0x1) == 0){};

    GPIO_PORTA_DIR_R |= 0xC0;

    GPIO_PORTA_DEN_R |= 0xC0;

    GPIO_PORTA_AFSEL_R |= 0xC0;//Pin 6-7??!?!??!!!?

    GPIO_PORTA_ODR_R = (GPIO_PORTA_ODR_R & ~0xFF) | 0x1;

    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0x00FFFFFF) | 0x33000000;

    I2C1_MCR_R = (I2C1_MCR_R & ~0x20) | 0x10;

    I2C1_MTPR_R = (I2C1_MTPR_R &  ~0xFF) | 0x39;

    I2C1_SACKCTL_R = (I2C1_SACKCTL_R & ~3) | 0x1;
}

void i2c_write_byte(uint8_t data){
    I2C1_MSA_R = (I2C1_MSA_R & ~0xFF) | (IR_TEMP_ADDRESS << 1);

    I2C1_MDR_R = (I2C1_MDR_R & ~0xFF) | data;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x7;

    while(I2C1_MCS_R & 0x40){};
}

uint16_t temp_read(){
    uint16_t ret_val = 0;

    I2C1_MSA_R = (I2C1_MSA_R & ~0xFF) | (0x5A << 1);

    I2C1_MDR_R = (I2C1_MDR_R & ~0xFF) | 0xB4;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x3;

    while(I2C1_MCS_R & 0x40){};

    I2C1_MDR_R = (I2C1_MDR_R & ~0xFF) | 0x07;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x3;

    while(I2C1_MCS_R & 0x1){};

    I2C1_MDR_R = (I2C1_MDR_R & ~0xFF) | 0xB5;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x3;

    while(I2C1_MCS_R & 0x1){};

    I2C1_MSA_R = (I2C1_MSA_R & ~0xFF) | (0x5A << 1) | 0x1;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0xB;

    while(I2C1_MCS_R & 0x1){};

    ret_val += I2C1_MDR_R & 0xFF;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0xB;

    while(I2C1_MCS_R & 0x1){};

    ret_val += (I2C1_MDR_R << 8) & 0xFF00;

    I2C1_MCS_R = (I2C1_MCS_R & ~0xFF) | 0x7;

    while(I2C1_MCS_R & 0x1){};

    return ret_val;
}
