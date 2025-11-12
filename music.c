#include "music.h"

void play_song(unsigned char* pitch, unsigned char* duration, short length, oi_t *sensor){
    int song_blocks = length / 16;
    char song = 0;
    oi_loadSong(song, 16, pitch, duration);
    oi_play_song(song);
    oi_update(sensor);
    for(int i = 1; i < song_blocks; i++){
        song = (song + 1) % 2;
        oi_loadSong(song, 16, pitch + (16 * i), duration + (16 * i));
        while(sensor->songPlaying){oi_update(sensor);}
        oi_play_song(song);
        oi_update(sensor);
    }

}
