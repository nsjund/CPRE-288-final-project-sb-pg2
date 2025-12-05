#include "movement.h"


uint8_t go_to_point(oi_t *sensor, short x, short y, s_pos *self){
    short Dx = x - self->pos_x;
    short Dy = y - self->pos_y;
    float t_ang = radToDeg(atan2f((float)Dy, (float)Dx));
    face_angle(sensor, t_ang, self);
    short dist = (short)sqrt(Dx * Dx + Dy * Dy);
    return move_forward(sensor, dist, 150, self);
}

void face_angle(oi_t *sensor, float target, s_pos *self){
    short speed;
    float Da = target - self->heading;
    if(fabs(Da) <= 10.0){
        speed = 50;
    }else{
       speed = 150;
    }
    Da = (fabs(Da) > 180.0) ? 180.0 - Da : Da;
    if(Da < 0){
        turn_clockwise(sensor, Da, speed, self);
    }else{
        turn_counter_clockwise(sensor, Da, speed, self);
    }

}

void update_self(oi_t *sensor, s_pos *self){
    self->heading += sensor->angle;
    self->pos_x += sensor->distance * cos(fdegToRad(self->heading));
    self->pos_y += sensor->distance * sin(fdegToRad(self->heading));
}


void turn_clockwise(oi_t *sensor, int degrees, int turn_speed, s_pos *self){
    oi_setWheels(-turn_speed, turn_speed);
    degrees = abs(degrees) - abs(degrees) / 18;
    double a_sum = 0;

    while(a_sum < degrees){
        oi_update(sensor);
        a_sum -= sensor->angle;
        update_self(sensor, self);
    }

    oi_setWheels(0, 0); // stop

}

void turn_counter_clockwise(oi_t *sensor, int degrees, int turn_speed, s_pos *self){
    oi_setWheels(turn_speed, -turn_speed);
    degrees = abs(degrees) - abs(degrees) / 18;
    double a_sum = 0;

    while(a_sum < degrees){
        oi_update(sensor);
        a_sum += sensor->angle;
        update_self(sensor, self);
    }

    oi_setWheels(0, 0); // stop
}

uint8_t move_forward(oi_t *sensor, int mm, int max_speed, s_pos *self){
    double sum = 0;
    double a_sum = 0;
    int move_speed = 0;

    while (sum < mm) {
        oi_update(sensor);
        sum += sensor->distance;
        a_sum += sensor->angle;
        move_speed = get_speed(mm, sum, max_speed, 2);
        oi_setWheels(move_speed - a_sum, move_speed + a_sum);
        update_self(sensor, self);
        if(sensor->bumpRight || sensor->bumpLeft){
            oi_setWheels(0, 0);
            return 1;
        }

        /* IR Value Readings:
         *
         * White Tape: when x > 2650
         *
         * Black Tape: when x < 200
         *
         * Grey Floor is typically between 1600 to 2400
         *
         */
        /*if ((sensor->cliffLeftSignal > 2650) || (sensor->cliffFrontLeftSignal > 2650) || (sensor->cliffFrontRightSignal > 2650) || (sensor->cliffRightSignal > 2650)) {
            turn_clockwise(sensor, 180, 200, self);
            return 3;
            // potentially turn?
        }*/
        if ((sensor->cliffLeftSignal < 100) || (sensor->cliffFrontLeftSignal < 100) || (sensor->cliffFrontRightSignal < 100) || (sensor->cliffRightSignal < 100)) {
            turn_clockwise(sensor, 180, 200, self);
            return 2;
           // potentially turn?
        }
    }

    oi_setWheels(0, 0); // stop
    return 0;
}


int get_speed(int target_disp, int current_disp, int max_speed, int acceleration){
    if(target_disp>(max_speed/acceleration)){
        if(current_disp < (max_speed/(2*acceleration))){
            return MAX(2*acceleration*current_disp, 100);
        }else if(current_disp + max_speed/acceleration < target_disp){
            return max_speed;
        }else if(current_disp < target_disp){
            return MAX(-acceleration * (current_disp-(target_disp+(max_speed/acceleration))) - max_speed, 50);
        }else return 0;
    }else{
        return 50;
    }
}


int move_backwards(oi_t *sensor, int mm, int move_speed, s_pos *self){

    oi_setWheels(-move_speed, -move_speed);

    float move_time_s = (float)mm/move_speed;
    int move_time_ms = (int)(move_time_s * 1000);

    timer_waitMillis(move_time_ms);

    oi_setWheels(0, 0);
    return -mm;
}

void dummy_move(oi_t *sensor, int mm, int move_speed){

    oi_setWheels(move_speed, move_speed);

    float move_time_s = (float)mm/move_speed;
    int move_time_ms = (int)(move_time_s * 1000);

    timer_waitMillis(move_time_ms);

    oi_setWheels(0, 0);
}
