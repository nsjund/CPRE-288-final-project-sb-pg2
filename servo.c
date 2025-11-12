#include "servo.h"


// 32,000?

// pulse_period - pulse_width = 304,000 ?


int features = 0;

void servo_init(void) {

	SYSCTL_RCGCGPIO_R |= 0x2;
	
	SYSCTL_RCGCTIMER_R |= 0x2;
	
	while ((SYSCTL_PRGPIO_R & 0x2) == 0){};
	
	while ((SYSCTL_PRTIMER_R & 0x2) == 0){};
	
	GPIO_PORTB_DIR_R |= 0x20;
	
	GPIO_PORTB_DEN_R |= 0x20;
	
	GPIO_PORTB_AFSEL_R |= 0x20;
	
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~(0x0F00000)) | 0x700000;
	
	// following steps in the data sheet for setting up PWM mode
	
	TIMER1_CTL_R &= ~(0x100);
	
	TIMER1_CFG_R = (TIMER1_CFG_R & ~(0x7)) | 0x4;
	
	TIMER1_TBMR_R = (TIMER1_TBMR_R & ~(0x5)) | 0xA;
	
	pulse_width = 0;
	
	servo_move(pulse_width);
	
	// lower 16 bits of interval
	
	TIMER1_TBILR_R = (TIMER1_TBILR_R & ~(0xFFFF)) | (pulse_period & 0xFFFF);
	
	// upper 8 bits of interval
	
	TIMER1_TBPR_R = (TIMER1_TBPR_R & ~(0xFF)) | (pulse_period >> 16);
	
	// lower 16 bits of pulse width
	
	TIMER1_TBMATCHR_R = (TIMER1_TBMATCHR_R & ~(0xFFFF)) | ((pulse_period - pulse_width) & 0xFFFF);
	
	// upper 8 bits of pulse width
	
	TIMER1_TBPMR_R = (TIMER1_TBPMR_R & ~(0xFF)) | ((pulse_period - pulse_width) >> 16);
	
	TIMER1_CTL_R |= 0x0100;
	
}
	
	
void servo_move(int degrees) {

    degrees += cal_offset_value;
	
	pulse_width = (int)(((float)(left_calibration_value - right_calibration_value))*(degrees/180.0) + right_calibration_value);
	
	TIMER1_CTL_R &= ~(0x100);
	
	TIMER1_TBMATCHR_R = (TIMER1_TBMATCHR_R & ~(0xFFFF)) | ((pulse_period - pulse_width) & 0xFFFF);

	// TIMER1_TBMATCHR_R = (int)((float)((left_calibration_value - right_calibration_value) * degrees)/180.0 + right_calibration_value);

	TIMER1_TBPMR_R = (TIMER1_TBPMR_R & ~(0xFF)) | ((pulse_period - pulse_width) >> 16);
	
	TIMER1_CTL_R |= 0x0100;
	
	timer_waitMillis(50);

}


void cyBOT_SERVO_cal(){
    lcd_init();
    button_init();
    servo_init();
    uint8_t c;
    int servo_pos = 0;
    int sign = 1;

    while(1){
        c = button_getButton();
        if(c == 1){
            servo_pos += sign;
            servo_move(servo_pos);
            c = 0;
            print_calibration(sign, servo_pos);
        }else if(c == 2){
            servo_pos += sign * 5;
            servo_move(servo_pos);
            c = 0;
            print_calibration(sign, servo_pos);
        }else if(c == 3){
            sign = -sign;
            c = 0;
            print_calibration(sign, 0);
        }else if(c == 4){
            servo_move(90 - sign * 90);
            c = 0;
            print_calibration(sign, 90 - sign * 90);
        }
    }
}

void print_calibration(int sign, int degrees){
    if(sign == 1){
        lcd_printf("Degrees:%d\nMatch:%d\nGoing higher", degrees, 0xFFFF - TIMER1_TBMATCHR_R);
    }else{
        lcd_printf("Degrees:%d\nMatch:%d\nGoing lower", degrees, 0xFFFF - TIMER1_TBMATCHR_R);
    }

}

void cyBOT_Scan(int angle, cyBOT_Scan_t* getScan){
    if(features & 1){
        servo_move(angle);
    }
    if(features & 2){
        getScan->sound_dist = ping_read();
    }else{
        getScan->sound_dist = -1.0;
    }
    if(features & 4){
        getScan->IR_raw_val = adc_read();
    }else{
        getScan->IR_raw_val = -1;
    }
}

// feature is like a 3 bit binary number

void cyBOT_init_Scan(int feature){
    features = feature;
    if(feature & 1){
        servo_init();
    }
    if(feature & 2){
        ping_init();
    }
    if(feature & 4){
        adc_init();
    }
}

void adc_init(void){

    SYSCTL_RCGCGPIO_R |= 0b00000010;

    while ((SYSCTL_RCGCGPIO_R & 0x2) == 0) {};

    // Choos Ain for PB4 -- disable digital, set as input

    GPIO_PORTB_AFSEL_R |= 0b00010000;

    GPIO_PORTB_AMSEL_R |= 0b00010000;

    GPIO_PORTB_DIR_R &= 0b11101111;

    GPIO_PORTB_DEN_R   &= 0b11101111;

    // enable ADC CLK

    SYSCTL_RCGCADC_R |= 0b01;

    while ((SYSCTL_RCGCADC_R & 0x1) == 0) {};

    // initialize port trigger

    GPIO_PORTB_ADCCTL_R = 0x00;

    // disable ADC SS0

    ADC0_ACTSS_R &= ~0x00000001;

    // Initialize the ADC trigger source

    ADC0_EMUX_R &= 0xFFF0;

    // Set 1st sample to use the AIN10 ADC pin

    ADC0_SSMUX0_R &= ~0x000F;

    ADC0_SSMUX0_R |= 0xA;


    // Enable raw interrupt status

    ADC0_SSCTL0_R &= 0xFFFFFFF0;

    ADC0_SSCTL0_R |=0x00000006;

    // enable oversampling to average

    ADC0_SAC_R &= 0xFFFFFFF0;

    ADC0_SAC_R |= 0x00000006;


    // re-enable ADC SS0

    ADC0_ACTSS_R |= 0x00000001;


}

uint16_t adc_read(void){

    uint16_t data;

    ADC0_PSSI_R = 0x0001;

    // ADC_PSSI_SS0

    while ((ADC0_RIS_R & 0x01) == 0){};

    // ADC_RIS_INR0

    data = (ADC0_SSFIFO0_R & 0xFFF);

    // ADC_SSFIFO0_DATA_M

    ADC0_ISC_R |= 0x0001;

    // ADC_ISC_IN0

    return data;

    // ADC0_SSFIFO0_R
}

volatile enum {LOW, HIGH, DONE} state = LOW; // set by ISR


volatile unsigned int rising_time = 0; //Pulse start time: Set by ISR

volatile unsigned int falling_time = 0; //Pulse end time: Set by ISR

volatile int numberOverflows = 0;

void ping_init(){

    SYSCTL_RCGCGPIO_R |= 0x00000002; //Enables the port’s system clock Pg.340

    SYSCTL_RCGCTIMER_R |= 0x00000008; // Pg 338 enable clock for Timer 3

    while ((SYSCTL_PRGPIO_R & 0x2) == 0) {};

    while ((SYSCTL_PRTIMER_R & 0x8) == 0) {};

    GPIO_PORTB_DIR_R |= 0x00000008;

    GPIO_PORTB_DEN_R |= 0x00000008; //Digital enable register

    GPIO_PORTB_AFSEL_R |= 0x00000008; // Force Pin 3 to be Zero

    GPIO_PORTB_PCTL_R |= 0x00007000; // T3CCP1 Pg706

    TIMER3_CTL_R &= ~(0x00000100); // Timer B Event is disabled and will trigger on the pos edge


    TIMER3_CFG_R = (TIMER3_CFG_R & ~(0x00000007)) | 0x00000004; // Set to 0x4 timer B use 16-bit mode

    // GPTMTnMR: GPTM Timer n Mode (n is A or B)

    TIMER3_TBMR_R = (TIMER3_TBMR_R & ~(0x0000001F)) | 0x00000007; // Set to capture mode 0x3, pages 729 && 733

    // GPTMTnILR: GPTM Timer n Interval Load

    TIMER3_TBILR_R = 0xFFFF;

    TIMER3_TBPR_R |= 0xFF;

    TIMER3_CTL_R |= 0xC00; //pg. 737

    // GPTMIMR: GPTM Interrupt Mask Register

    TIMER3_IMR_R |= 0x00000400; // Pg.745 Enable the Timer B Interrupt  p. 745 of data sheet

    // GPTMICR: GPTM Interrupt Clear Register

    TIMER3_ICR_R |= 0x00000400; //Pg.754 Clears the Timer n Capture Mode Event Flag


    NVIC_EN1_R |= 0x0000010;

    // GPTMCTL: GPTM (General Purpose Timer) Control enable for set-up

    TIMER3_CTL_R |= 0x00000100; // Timer B Event is enabled and will trigger on the positive edge

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();


}

float ping_read(void) {
    float distance = 0;

    int pulse_width = 0;

    float speed_of_sound_in_cm = 34300.0;

    send_pulse();

    timer_waitMillis(50);

    // Wait for ISR to capture rising edge and falling edge

    while (state != DONE){};

    // wait until the entire signal is complete

    if(falling_time > rising_time){

        pulse_width = (rising_time - falling_time) + 0xFFFF;

        numberOverflows++;

        // increment for overflow count

    }

    else {

        pulse_width = (rising_time - falling_time);
    }

    distance = speed_of_sound_in_cm * (pulse_width / 32000000.0);

    // Time per clock cycle is like 1/(16 mil) which is 6.25 * 10^-8
    // Divide by 2 since the reflection

    // Though... I am unsure...

    return distance;
}




void send_pulse(void){

    state = LOW;

    TIMER3_CTL_R &= ~(0x00000100); //pg. 737 Disable Timer B

    TIMER3_IMR_R = (TIMER3_IMR_R & ~(0x00000400)); // Pg.745 Disable interrupts

    GPIO_PORTB_AFSEL_R &= ~(0x00000008); // Force Pin 3 to be Zero

    GPIO_PORTB_DIR_R |= 0x00000008; // Set as output

    GPIO_PORTB_DATA_R &= ~(0x00000008);

    // state = HIGH;

    GPIO_PORTB_DATA_R |= 0x08;

    // Send trigger, wait the 5 microseconds minimum

    timer_waitMicros(5);

    GPIO_PORTB_DATA_R &= ~(0x00000008);

    // state = LOW;

    GPIO_PORTB_DIR_R  &= ~(0x00000008); // Set as input

    GPIO_PORTB_AFSEL_R |=  (0x00000008); // Force Pin 3 to be 1

    TIMER3_ICR_R |= 0x00000400; //Pg.754 Clear the Timer Flag

    TIMER3_IMR_R = (TIMER3_IMR_R & ~(0x00000400)) | 0x00000400; // Pg.745 Enable interrupts

    TIMER3_CTL_R |= (0x00000100); //pg. 737 Enable Timer B

}

// ISR: Capture rising edge and falling edge time of PING sensor

void TIMER3B_Handler(void)
{

    //Check the receive byte

    if(TIMER3_MIS_R & 0x00000400){

        if(state == LOW){

            // Rising Edge!

            rising_time = (TIMER3_TBR_R & 0xFFFFFF);

            state = HIGH;

        }
        else if(state == HIGH){

            // Falling Edge!

            falling_time = (TIMER3_TBR_R & 0xFFFFFF);

            state = DONE;

            // drop the class
        }


    TIMER3_ICR_R |= 0x00000400;

    //clear the interrupt status

    }

}

int get_overflows(){
    return numberOverflows;
}


