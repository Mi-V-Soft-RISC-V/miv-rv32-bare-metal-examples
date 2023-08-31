/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file mandelbrot.cpp
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Functions needed for the Mandelbrot demo
 *
 * Reference: https://rosettacode.org/wiki/Mandelbrot_set
 *
 */

#include <stdio.h>
#include "common.hpp"
#include "test-utils.h"

struct MandelbrotView
{
  float lookAtX;
  float lookAtY;
  float width;
  float height;
  float gamma;
};

// Coordinate system is similar to Christian Stigen Larsen's approach (Y is upside down as well)
// http://tilde.club/~david/m/
struct MandelbrotView sets[] =
{
  {-0.5f,          0.0f,        2.5f,          3.0f,            1.0f},
  {-1.2059523f,   -0.34705183f, 0.36000158f,   0.17550077f,     2.0f},
  {-0.55f,        -0.64f,       0.15f,         0.115f,          4.0f},
  { 0.5f,          0.0f,        1.0f,          2.0f,            1.0f},
  { 0.003290713f, -0.7907230f,  0.03573984f,   0.04767370f,    11.0f},
  {-0.0068464412f,-0.80686056f, 0.0160606767f, 0.00782957993f, 13.0f}
};

#if VT100_COLORS == 1
const uint8_t fg[] = { 31, 35, 31, 35, 34, 35, 34, 36, 34, 36, 32, 36, 32, 32};
const uint8_t bg[] = { 41, 41, 45, 45, 45, 44, 44, 44, 46, 46, 46, 42, 42, 42};
#else
const unsigned char shades[] = { '.', ',', ';', '+', '*', '#', '%', '@'};
#endif

const char * mchp_logo = "\r\n\
                                  /#########*\r\n\
                             .#################/\r\n\
                           ######################    (R)\r\n\
                         #######/ *########## ,###,\r\n\
                        #######     #######,    ###*\r\n\
                       #######       (####       (##(\r\n\
                      (######(        .##/         ###\r\n\
                      #####.###         ##(         ###\r\n\
                      ###*   (##         ###         ###*\r\n\
                      ##      *##.        (##*        *##\r\n\
                      *        /##(         ##(        .#\r\n\
                              *####(        ####\r\n\
                             ########.    *######\r\n\
                           /##########/  #########(\r\n\
                           ########################(\r\n\
                             *###################*\r\n";
#if VT100_COLORS == 1
const char * mchp_text ="\033[33m     ___                 \033[36m  ___   \033[32m     ___   \033[33m    ___   \033[35m     ___   \033[36m     ___          \033[33m         ___\r\n\
\033[33m    /\\__\\  \033[35m      ___  \033[36m    /\\  \\ \033[32m     /\\  \\  \033[33m   /\\  \\   \033[35m   /\\  \\   \033[36m   /\\__\\   \033[32m     ___   \033[33m   /\\  \\\r\n\
\033[33m   /##|  |   \033[35m   /\\  \\ \033[36m   /##\\  \\\033[32m    /##\\  \\ \033[33m  /##\\  \\  \033[35m  /##\\  \\  \033[36m  /#/  /     \033[32m  /\\  \\  \033[33m  /##\\  \\\r\n\
\033[33m  /#|#|  |   \033[35m   \\#\\  \\\033[36m  /#/\\#\\  \\\033[32m  /#/\\#\\  \\\033[33m /#/\\#\\  \\\033[35m  /#/\\#\\  \\ \033[36m /#/__/ \033[32m       \\#\\  \\ \033[33m /#/\\#\\  \\\r\n\
\033[33m /#/|#|__|__ \033[35m   /##\\__\\\033[36m/#/  \\#\\__\\\033[32m/##\\-\\#\\__V\033[33m#/  \\#\\__\\\033[35m/#/  \\#\\__\\\033[36m/##\\  \\ ___ \033[32m   /##\\__\\\033[33m/##\\-\\#\\__\\\r\n\
\033[33m/#/ |####\\__\\\033[35m__/#/\\/__/\033[36m#/__/ \\/__/\033[32m#/\\#\\/#/  /\033[33m/__//#/  /\033[35m#/__/ \\/__/\033[36m#/\\#\\  /\\__\\\033[32m__/#/\\/__/\033[33m#/\\#\\/#/  /\r\n\
\033[33m\\/__/--/#/  /\033[35m\\/#/  /  \033[36m\\#\\  \\     \033[32m\\/_|###/  /\033[33m#\\  /#/  /\033[35m\\#\\  \\   \033[36m  \\/__\\#\\/#/  /\033[32m\\/#/  / \033[33m \\/__\\##/  /\r\n\
\033[33m      /#/  /\033[35m\\##/__/    \033[36m\\#\\  \\     \033[32m  |#|V__/ \033[33m\\#\\/#/  /  \033[35m\\#\\  \\    \033[36m     \\##/  /\033[32m\\##/__/     \033[33m   \\/__/\r\n\
\033[33m     /#/  / \033[35m \\#\\__\\  \033[36m   \\#\\__\\  \033[32m    |#|  |  \033[33m \\##/  /  \033[35m  \\#\\__\\   \033[36m     /#/  / \033[32m \\#\\__\\\r\n\
\033[33m    /#/  /  \033[35m  \\/__/    \033[36m  \\/__/      \033[32m|#|  |  \033[33m  \\/__/      \033[35m\\/__/     \033[36m  /#/  /  \033[32m  \\/__/\r\n\
\033[33m    \\/__/                      \033[32m      \\|__|                   \033[36m        \\/__/\r\n\
             \033[47m\033[30m-=- IEEE 754 Single floating point mandelbrot fractal demonstration -=-\033[39m\033[49m\r\n";
#else
const char * mchp_text ="     ___                   ___        ___       ___        ___        ___                   ___\r\n\
    /\\__\\        ___      /\\  \\      /\\  \\     /\\  \\      /\\  \\      /\\__\\        ___      /\\  \\\r\n\
   /##|  |      /\\  \\    /##\\  \\    /##\\  \\   /##\\  \\    /##\\  \\    /#/  /       /\\  \\    /##\\  \\\r\n\
  /#|#|  |      \\#\\  \\  /#/\\#\\  \\  /#/\\#\\  \\ /#/\\#\\  \\  /#/\\#\\  \\  /#/__/        \\#\\  \\  /#/\\#\\  \\\r\n\
 /#/|#|__|__    /##\\__\\/#/  \\#\\__\\/##\\-\\#\\__V#/  \\#\\__\\/#/  \\#\\__\\/##\\  \\ ___    /##\\__\\/##\\-\\#\\__\\\r\n\
/#/ |####\\__\\__/#/\\/__/#/__/ \\/__/#/\\#\\/#/  //__//#/  /#/__/ \\/__/#/\\#\\  /\\__\\__/#/\\/__/#/\\#\\/#/  /\r\n\
\\/__/--/#/  /\\/#/  /  \\#\\  \\     \\/_|###/  /#\\  /#/  /\\#\\  \\     \\/__\\#\\/#/  /\\/#/  /  \\/__\\##/  /\r\n\
      /#/  /\\##/__/    \\#\\  \\       |#|V__/ \\#\\/#/  /  \\#\\  \\         \\##/  /\\##/__/        \\/__/\r\n\
     /#/  /  \\#\\__\\     \\#\\__\\      |#|  |   \\##/  /    \\#\\__\\        /#/  /  \\#\\__\\\r\n\
    /#/  /    \\/__/      \\/__/      |#|  |    \\/__/      \\/__/       /#/  /    \\/__/\r\n\
    \\/__/                            \\|__|                           \\/__/\r\n\
             -=- IEEE 754 Single floating point mandelbrot fractal demonstration -=-\r\n";
#endif

void defaultColors()
{
#if VT100_COLORS == 1
    printf("\033[39m\033[49m");
#endif
}


void mandelbrot(float lookAtX, float lookAtY, float width, float height, float gamma)
{
  // Calculate boundaries of the fractal
  const float xmin    = lookAtX - (width  / 2);
  const float ymin    = lookAtY - (height / 2);
  const float stepX   = width  / WIDTH;
  const float stepY   = height / HEIGHT;
#if VT100_COLORS == 1
  const int   maxIter = (float)NELEMS(fg) * gamma;  // Max iterations will affect the "exposure"
#else
  const int   maxIter = (float)NELEMS(shades) * gamma;  // Max iterations will affect the "exposure"
#endif
  int iterOld = -1; // Do not use static as it needs to be reset on each frame start

  for (int cursorY = 2; cursorY < HEIGHT; cursorY++)
  {
    // Skip few lines to allow margins for the text on the top
    const float y = ymin + cursorY * stepY;

    for (int cursorX = 0; cursorX < WIDTH; cursorX++)
    {
      const float x = xmin + cursorX * stepX;
      float u  = 0.0f;
      float v  = 0.0f;
      float u2 = 0.0f; // u squared
      float v2 = 0.0f; // v squared
      int iter;        // Iterations executed

      for (iter = 0 ; iter < maxIter && (u2 + v2 < 4.0f); iter++)
      {
        v  = 2 * (u*v) + y;
        u  = u2 - v2 +x;
        u2 = u * u;
        v2 = v * v;
      }

      // Print nothing if iterated too much, or normalize the result and shade accordingly
      if (iter >= maxIter)
      {
          if (iterOld != iter) {
              defaultColors();
          }
          printf(" ");
      }
      else
      {
#if VT100_COLORS == 1
        if (iterOld != iter) {
            int index = iter / gamma;
            printf("\033[%dm\033[%dm", fg[index], bg[index]);
        }
        putchar('#');
#else
        putchar(shades[(int)(iter / gamma)]);
#endif
      }
      testAddToChecksumFloat(iter / gamma);
      iterOld = iter;
    }
    if (cursorY != HEIGHT-1)
    {
      printf("\r\n");
    }
  }
}


float rescale(float oldVal, float newVal, float percentage)
{
  // Make sure even with overflowed percentage will be computed correctly
  return ((newVal - oldVal) * MINF(1.0f, MAXF(0.0f, percentage))) + oldVal;
}


void screenClear()
{
#ifdef SERIAL_TERMINAL_ANIMATION
  printf("\033[2J"); // http://www.termsys.demon.co.uk/vtansi.htm
#endif
}


void screenCursorToTopLeft()
{
#ifdef SERIAL_TERMINAL_ANIMATION
  printf("\033[0;0H");
#endif
}


void printLogoAndText()
{
    screenClear();
#if VT100_COLORS == 1
    printf("\033[31m");
#endif
    printf(mchp_logo);
    defaultColors();
}


void demoMandelbrot()
{
  printLogoAndText();
  screenClear();
  screenCursorToTopLeft();

  // Render following mandelbrot series
  for (int i = 0 ; i < (NELEMS(sets) - 1); i++)
  {
    for (float percentage = 0.0f; percentage <= 1.3f; percentage += ANIMATION_SPEED)
    {
      // Display motion between the sets:
      //   0.0f to 1.0f will be transitions
      //   1.0f to 1.3f will render same frame (timing without using timer)
      for (int iterate = 0; iterate < ITERATIONS; iterate++)
      {
        // Depending on the #define one image can be repeated multiple times
        const int   iNext   = (i +1) % NELEMS(sets);
        const float lookAtX = rescale(sets[i].lookAtX, sets[iNext].lookAtX, percentage);
        const float lookAtY = rescale(sets[i].lookAtY, sets[iNext].lookAtY, percentage);
        const float width   = rescale(sets[i].width,   sets[iNext].width,   percentage);
        const float height  = rescale(sets[i].height,  sets[iNext].height,  percentage);
        const float gamma   = rescale(sets[i].gamma,   sets[iNext].gamma,   percentage);

        printf("Set=%d Progress=%3d%%\r\n", i, (int)(percentage * 100.0f));
        mandelbrot(lookAtX, lookAtY, width, height, gamma);
        screenCursorToTopLeft();
        defaultColors();
      }
    }
  }
  printLogoAndText();

}
