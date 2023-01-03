#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "dos.h"

#define  SCREEN_WIDTH 320
#define  SCREEN_HEIGHT 320
#define  SCALE_FACTOR 100.0

uint8_t* heightmap = NULL;
uint8_t* colormap = NULL;

typedef struct {
  float x;
  float y;
  float height;
  float zfar;
} camera_t;

camera_t camera = {
  .x         = 512.0,
  .y         = 512.0,
  .height    = 150.0,
  .zfar      = 400.0
};

void processinput(){
  if (keystate(KEY_UP)) {
    camera.y++;
  }
  if (keystate(KEY_DOWN)) {
    camera.y--;
  }
  if (keystate(KEY_LEFT)) {
    camera.x--;
  }
  if (keystate(KEY_RIGHT)) {
    camera.x++;
  }
  if (keystate(KEY_E)) {
    camera.height++;
  }
  if (keystate(KEY_D)) {
    camera.height--;
  }
}

int main(int argc, char* args[]) {

    setvideomode(videomode_320x200);

    uint8_t palette[256 * 3];
    int map_width, map_height, pal_count;

    colormap = loadgif("maps/colormap.gif", &map_width, &map_height, &pal_count, palette);
    heightmap = loadgif("maps/heightmap.gif", &map_width, &map_height, NULL, NULL);
   
    for (int i = 0; i < pal_count; i++) {
        setpal(i, palette[3 * i + 0], palette[3 * i + 1], palette[3 * i + 2]);
    }
    setpal(0, 36, 36, 56);

    setdoublebuffer(1);
    uint8_t* framebuffer = screenbuffer();

    while (!shuttingdown()) {
      waitvbl();
      clearscreen();
      processinput();

      float plx = -camera.zfar;
      float ply = +camera.zfar;

      float prx = +camera.zfar;
      float pry = +camera.zfar;

      for (int i = 0; i < SCREEN_WIDTH; i++) {
        float delta_x = (plx + (prx - plx) / SCREEN_WIDTH * i) / camera.zfar;
        float delta_y = (ply + (pry - ply) / SCREEN_WIDTH * i) / camera.zfar;

        float rx = camera.x;
        float ry = camera.y;

        float max_height = SCREEN_HEIGHT;
         
        for (int z = 1; z < camera.zfar; z++) {
          rx += delta_x;
          ry += delta_y;

          int mapoffset = ((1024 * ((int)(ry) & 1023)) + ((int)(rx) & 1023 ));

          int heightonscreen = (int)((camera.height - heightmap[mapoffset]) / z * SCALE_FACTOR);

          if (heightonscreen < 0) {
            heightonscreen = 0; 
          }
          if (heightonscreen > SCREEN_HEIGHT) {
            heightonscreen = SCREEN_HEIGHT - 1;
          }
          if (heightonscreen < max_height) {
            for (int y = heightonscreen; y < max_height; y++) {
                framebuffer[(SCREEN_WIDTH * y) + i] = (uint8_t)colormap[mapoffset];
            }
            max_height = heightonscreen;
          }
        }
      }

      framebuffer = swapbuffers();
      
      if (keystate(KEY_ESCAPE))
        break;
    }
      return 0;
  }
