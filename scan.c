
#include "scan.h"

#include "uart.h"

uint8_t is_path_valid(s_object_t* stored, path_t p, uint8_t stored_count){
    float d = 0;
    for(int i = 0; i < stored_count; i++){
        if(!((stored[i].pos_x > p.x1 + 160 && stored[i].pos_x > p.x2 + 160 || stored[i].pos_x < p.x1 - 160 && stored[i].pos_x < p.x2 - 160) && (stored[i].pos_y > p.y1 + 160 && stored[i].pos_y > p.y2 + 160 || stored[i].pos_y < p.y1 - 160 && stored[i].pos_y < p.y2 - 160))){
            d = (abs((p.x2 - p.x1)*(p.y1-stored[i].pos_y) - (p.x1 - stored[i].pos_x) * (p.y2-p.y1)))/(sqrt(pow(p.x2-p.x1,2.0) + pow(p.y2-p.y1,2.0)));
            if(d < (180 + stored[i].width / 2)){
                uart_sendStr("Object obstructing\r");
                return 0;
            }
        }else{
            uart_sendStr("Object out of bounds\r");
        }
    }
    uart_sendStr("Path clear\r\n");
    return 1;
}




int do_scan_filter(scan_point_t* scan_points, object_t* object_list, float* raw_data, oi_t* sensor, s_object_t* stored_objects, uint8_t* stored_count, s_pos *self){
    do_ir_scan(scan_points);
    int object_count = find_objects(object_list, scan_points, 1000.0, sensor);
    compensate_for_displacement(object_list, object_count);
    *stored_count = store_objects(object_list, self, stored_objects, object_count, *stored_count);
    return object_count;
}

void do_ir_scan(scan_point_t* scan_points){
    scan_init(0b00001101);
    // Initialize Temp Sensor + Servo + ADC
    cyBOT_Scan_t scan;
    cyBOT_Scan(0, &scan);
    for(int i = 0; i <= 90; i++){
        cyBOT_Scan((i * 2), &scan);
        scan_points[i].dist = get_dist_from_IR(adc_read());
        scan_points[i].temp = ((temp_read() / 50.0) - 273.15) * 1.8 + 32;
        if(scan_points[i].dist < 1.0){
            scan_points[i].dist = 2000.0;
        }
    }
}

/*float get_dist_from_IR(uint16_t raw_IR){
    return p1 + p2 * pow(raw_IR, p3);
}*/

float get_dist_from_IR(uint16_t raw_IR){
    return 311000000 * pow((float)raw_IR, -1.89);
}

void scan_init(char init_val){
    cyBOT_init_Scan(init_val);
}

void go_to_smallest(object_t* objects, uint8_t* count, oi_t* sensor, s_pos *self, s_object_t* stored_objects){

    s_object_t smallest = stored_objects[0];
    for(int i = 0; i < *count; i++){
        if((stored_objects[i].width < smallest.width)){
            smallest = stored_objects[i];
        }
    }
    short dist_to_smallest = sqrt(pow(self->pos_x - smallest.pos_x, 2.0) + pow(self->pos_y - smallest.pos_y, 2.0));
    dist_to_smallest -= (smallest.width/2 + 185);
    float theta = atan2f((float)smallest.pos_y - self->pos_y, (float)smallest.pos_x - self->pos_x);
    short target_x = dist_to_smallest * cosf(theta) + self->pos_x;
    short target_y = dist_to_smallest * sinf(theta) + self->pos_y;
    char msg[30];
    if(go_to_point(sensor, target_x, target_y, self)){
        short new_obj_x = 0;
        short new_obj_y = 0;
        if(sensor->bumpLeft && sensor->bumpRight){
            new_obj_x = 225*cosf(fdegToRad(self->heading)) + self->pos_x;
            new_obj_y = 225*sinf(fdegToRad(self->heading)) + self->pos_y;
            stored_objects[*count].pos_x = new_obj_x;
            stored_objects[*count].pos_y = new_obj_y;
            stored_objects[*count].width = 130;
            uart_sendStr(msg);

            (*count)++;
        }else if(sensor->bumpLeft){
            while(!sensor->bumpRight){
                oi_setWheels(50, -50);
                oi_update(sensor);
                update_self(sensor, self);
            }
            oi_setWheels(0, 0);
            new_obj_x = 225*cosf(fdegToRad(self->heading)) + self->pos_x;
            new_obj_y = 225*sinf(fdegToRad(self->heading)) + self->pos_y;
            stored_objects[*count].pos_x = new_obj_x;
            stored_objects[*count].pos_y = new_obj_y;
            stored_objects[*count].width = 130;
            uart_sendStr(msg);
            (*count)++;
        }else if(sensor->bumpRight){
            while(!sensor->bumpLeft){
                oi_setWheels(-50, 50);
                oi_update(sensor);
                update_self(sensor, self);
            }
            oi_setWheels(0, 0);
            new_obj_x = 225*cosf(fdegToRad(self->heading)) + self->pos_x;
            new_obj_y = 225*sinf(fdegToRad(self->heading)) + self->pos_y;
            stored_objects[*count].pos_x = new_obj_x;
            stored_objects[*count].pos_y = new_obj_y;
            stored_objects[*count].width = 130;
            uart_sendStr(msg);
            (*count)++;

        }
        go_to_point(sensor, 0, 0, self);
        face_angle(sensor, 90, self);
    }


}

uint8_t store_objects(object_t* objects, s_pos *self, s_object_t* stored_objects, uint8_t object_count, uint8_t stored_count){
    s_object_t new_objects[5];
    for(int i = 0; i < object_count; i++){
        new_objects[i].pos_x = objects[i].dist * cos(degToRad(objects[i].center_angle)) + self->pos_x;
        new_objects[i].pos_y = objects[i].dist * sin(degToRad(objects[i].center_angle)) + self->pos_y;
        new_objects[i].width = objects[i].linear_width;
    }
    uint8_t k = 0;
    for(int i = 0; i < object_count; i++){
        for(int j = 0; j < stored_count; j++){
            if(compare_objects(new_objects[i], stored_objects[j])){
                stored_objects[stored_count + k].pos_x = new_objects[i].pos_x;
                stored_objects[stored_count + k].pos_y = new_objects[i].pos_y;
                stored_objects[stored_count + k].width = new_objects[i].width;
                k++;
            }
        }
    }
    for(int i = stored_count; i < object_count; i++){
        stored_objects[stored_count + k].pos_x = new_objects[i].pos_x;
        stored_objects[stored_count + k].pos_y = new_objects[i].pos_y;
        stored_objects[stored_count + k].width = new_objects[i].width;
        k++;
    }
    return stored_count + k;
}

void print_stored(s_object_t* stored_objects, uint8_t stored_count, s_pos *self){
    uart_sendStr("X pos\tY pos\tWidth\tDist\r");
    char msg[30];
    float dist = 0;
    for(int i = 0; i < stored_count; i++){
        dist = sqrt(pow(stored_objects[i].pos_x - self->pos_x,2.0) + pow(stored_objects[i].pos_y - self->pos_y,2.0));
        sprintf(msg, "%d\t%d\t%d\t%3.1f\r", stored_objects[i].pos_x , stored_objects[i].pos_y, stored_objects[i].width, dist);
        uart_sendStr(msg);
    }
    uart_sendChar('\n');
}

uint8_t compare_objects(s_object_t a, s_object_t b){
    short dist = sqrt(pow(a.pos_x - b.pos_x, 2) + pow(a.pos_y - b.pos_y, 2));
    uint8_t radius_sum = a.width/2 + b.width/2;
    if(dist > radius_sum)
        return 1;
    return 0;
}


int find_objects(object_t* object_list, scan_point_t* distances, float threshold, oi_t* sensor){
    int object_count = 0;
    for(int i = 0; i < 91; i++){
        if((i == 0) && (distances[i].dist < threshold)){
            object_list[object_count].start_angle = 0;
        }
        if(i > 0){
            if(distances[i-1].dist > threshold && distances[i].dist < threshold){
                object_list[object_count].start_angle = i * 2;
            }
            if(distances[i-1].dist < threshold && distances[i].dist > threshold){
                object_list[object_count].end_angle = (i - 1) * 2;
                if(object_list[object_count].end_angle - object_list[object_count].start_angle > 2){
                    object_count++;
                }
            }
        }
        if(i == 90 && distances[i].dist < threshold){
            object_list[object_count].end_angle = 180;
            if(object_list[object_count].end_angle - object_list[object_count].start_angle > 2){
                object_count++;
            }
        }
    }
    scan_init(0x03);
    cyBOT_Scan_t scan;
    for(int i = 0; i < object_count; i++){
        object_list[i].center_angle = (object_list[i].start_angle + object_list[i].end_angle)/2;
        object_list[i].width = object_list[i].end_angle - object_list[i].start_angle;
        cyBOT_Scan(object_list[i].center_angle, &scan);
        do{
            cyBOT_Scan(object_list[i].center_angle, &scan);
        }while(scan.sound_dist > 101.0);
        object_list[i].dist = scan.sound_dist * 10;//distance in mm
        object_list[i].object_num = i+1;
        float rad = degToRad(object_list[i].width);
        object_list[i].linear_width = object_list[i].dist * rad;
        object_list[i].dist += object_list[i].linear_width/2;
    }
    return object_count;
}

float get_avg_ir(oi_t* sensor, uint8_t angle){
    float dist = 0;
    scan_init(0x05);
    cyBOT_Scan_t scan;
    for(int i = 0; i < 3; i++){
        cyBOT_Scan(angle, &scan);
        dist += get_dist_from_IR(scan.IR_raw_val);
    }
    return dist / 5.0;
}

void compensate_for_displacement(object_t* object_list, uint8_t object_count){
    int i;
    float a, b, c, aa, ac;
    for(i = 0; i < object_count; i++){
        aa = (float)(object_list[i].center_angle);
        if(aa < 90.0){
            aa += 90.0;
        }else{
            aa = 360.0 - 90.0 - aa;
        }
        aa *= (M_PI/180.0);
        c = object_list[i].dist;
        b = 110.0;
        a = sqrt((b * b) + (c * c) - (2.0 * b * c * cos(aa)));
        ac = asin((c * sin(aa)) / a);
        if((float)(object_list[i].center_angle) <= 90.0){
            ac = ((M_PI / 2.0) - ac);
        }else{
            ac = ((M_PI / 2.0) + ac);
        }
        ac *= (180.0 / M_PI);
        object_list[i].center_angle = ac;
        object_list[i].dist = a;
    }
}


void filter_outlier(float* outliers_filtered, float* distances, float threshold){
    int i;
    for(i = 1; i < 90; i++){
        if(distances[i - 1] > threshold && distances[i] < threshold && distances[i+1] > threshold){
            outliers_filtered[i] = (distances[i - 1] + distances[i + 1])/2;
        }else if(distances[i - 1] < threshold && distances[i] > threshold && distances[i+1] < threshold){
            outliers_filtered[i] = (distances[i - 1] + distances[i + 1])/2;
        }else {
            outliers_filtered[i] = distances[i];

        }
    }
}


void send_scan(scan_point_t *scan_points, float *raw_data){
    char data[30];
    //putty_print("Degrees      Distance (cm)\n");
    uart_sendStr("Degrees\tDist\tTemp\tRaw\tIR\r");
    int i;
    for(i = 0; i <= 90; i++){
        sprintf(data, "%d\t%.2f\t%.2f\t%.2f\r", i * 2, scan_points[i].dist, scan_points[i].temp, raw_data[i]);
        uart_sendStr(data);
    }
    uart_sendChar('\n');
}




void print_objects(object_t* object_list, int object_count){
    char msg[50] = {0};
    uart_sendStr("Object#\tAngle\tDist\tWidth\tLWidth\tTest\r");
    int i;
    for(i = 0; i < object_count; i++){
        sprintf(msg, "%d\t%d\t%.0f\t%d\t%f\r", object_list[i].object_num, object_list[i].center_angle, object_list[i].dist, object_list[i].width, object_list[i].linear_width);
        uart_sendStr(msg);
    }
}
