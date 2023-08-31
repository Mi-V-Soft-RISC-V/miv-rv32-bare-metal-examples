/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file raytracer.cpp
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Functions needed for the Raytracer demo
 * 
 * References:
 * https://www.ics.uci.edu/~gopi/CS211B/RayTracing%20tutorial.pdf
 * https://tmcw.github.io/literate-raytracer/
 * http://www.3dcpptutorials.sk/index.php?id=16
 * http://en.cppreference.com/w/cpp/language/operators
 * http://en.cppreference.com/w/c/numeric/math
 *
 */

#include <stdio.h>
#include "raytracer.hpp"
#include "common.hpp"
#include "test-utils.h"


struct Shade
{
  float value;

  // https://stackoverflow.com/questions/926752/why-should-i-prefer-to-use-member-initialization-list
  Shade(): value(0.0f)
  {
  }

  Shade(float valueInit): value(valueInit)
  {
  }

  Shade operator *(float scalar)
  {
    return Shade(this->value * scalar);
  }

  Shade operator +(Shade secondShade)
  {
    return Shade(this->value + secondShade.value);
  }

  // Normalize
  Shade operator ~()
  {
    this->value = MINF(1.0f, MAXF(0.0f, this->value)); // Make value within range 0 >= value <=1
    return *this;
  }

  // Casting Shade to "char" will trigger equivalent of a toString() method
  // http://en.cppreference.com/w/cpp/language/cast_operator
  operator char()
  {
    const char shades[] = {' ', '.', ',', '-', ':', '+', '=', '*', '%', '@', '#'};
    return shades[(int) ((~*this).value * (array_size(shades)-1))];
  }

  void colorize() {
      static int indexOld = -1;

      const char * colors[] = {
          "\033[30m\033[40m \0", "\033[34m\033[40m-\0", "\033[34m\033[40m#\0",
          "\033[30m\033[44m#\0", "\033[30m\033[44m-\0", "\033[34m\033[44m \0",
          "\033[36m\033[44m-\0", "\033[36m\033[44m#\0", "\033[34m\033[46m#\0",
          "\033[34m\033[46m-\0", "\033[36m\033[46m \0", "\033[37m\033[46m-\0",
          "\033[37m\033[46m#\0", "\033[36m\033[47m#\0", "\033[36m\033[47m-\0",
          "\033[37m\033[47m \0"
      };

      const char characters[] = { ' ', '-', '#', '#', '-', ' ', '-', '#', '#',
              '-', ' ', '-', '#', '#', '-', ' ' };

      int index = (~*this).value * (array_size(colors)-1);

      if (indexOld != index)
      {
          printf(colors[index]);      // New index => print color + character
      }
      else
      {
          putchar(characters[index]); // Same index => print character only
      }

      indexOld = index;
  }

};


class Vector3
{
  float x, y, z;

public:
  Vector3(): x(0.0f), y(0.0f), z(0.0f)
  {
  }

  Vector3(float xInit, float yInit, float zInit): x(xInit), y(yInit), z(zInit)
  {
  }

  Vector3 operator +(Vector3 secondVector)
  {
    return Vector3(x + secondVector.x, y + secondVector.y, z + secondVector.z);
  }

  Vector3 operator -(Vector3 secondVector)
  {
    return Vector3(x - secondVector.x, y - secondVector.y, z - secondVector.z);
  }

  Vector3 operator *(float scalar)
  {
    return Vector3(x * scalar, y * scalar, z * scalar);
  }

  Vector3 operator /(float scalar)
  {
    return Vector3(x / scalar, y / scalar, z / scalar);
  }

  // Dot product
  float operator%(Vector3 secondVector)
  {
    return (x * secondVector.x + y * secondVector.y + z * secondVector.z);
  }

  // Normalize
  Vector3 operator~() {
    const float magnitude = SQUARE_ROOT(x * x + y * y + z * z);
    return Vector3(x / magnitude, y / magnitude, z / magnitude);
  }
};


struct Light: public Vector3
{
  Shade shade;
  Light(Vector3 source, Shade shadeInit): Vector3(source), shade(shadeInit)
  {
  }
};


struct Ray
{
  Vector3 source;
  Vector3 direction;

  Ray(Vector3 sourceInit, Vector3 directionInit): source(sourceInit), direction(directionInit)
  {
  }
};


class Sphere
{
  Vector3 center;
  float   radius;

public:
  Sphere(Vector3 centerInit, float radiusInit): center(centerInit), radius(radiusInit)
  {
  }

  // Get normalized normal vector from sphere's surface point
  Vector3 operator ^ (Vector3 pointOnSurface)
  {
    return ~(pointOnSurface - this->center);
  }

  bool detectHit(Ray ray, Vector3 &hitPoint)
  {
    // http://mathforum.org/mathimages/index.php/Ray_Tracing
    // All points at sphere's surface meet this equation:
    // (point - center).(point - center) - radius^2 = 0
    // While any point on the ray's path can be calculated:
    // point = source + direction * distance
    // Source and direction are known, reversing the equations to find if there
    // is a distance on the path which meets sphere's equation.
    Vector3 inRef   = ray.source - this->center;
    float   dotDir  = ray.direction % ray.direction;
    float   temp1   = ray.direction % inRef;
    float   temp2   = (inRef % inRef) - (this->radius * this->radius);
    float   tempAll = (temp1 * temp1) - (dotDir * temp2);

    if (tempAll < 0.0f) return false; // The ray didn't hit the sphere at all

    // 2 points are intersecting the sphere, chose the closest point to the camera
    const float distance = MINF( (-temp1 + SQUARE_ROOT(tempAll)) / dotDir,
                                 (-temp1 - SQUARE_ROOT(tempAll)) / dotDir );

    hitPoint = ray.source + ray.direction * distance;
    return true;
  }
};


Shade calculateShadeOfTheRay(Ray ray, Light light)
{
  Sphere  sphere(Vector3(0.0f, 0.0f, HEIGHT), HEIGHT/2.0f);
  Shade   ambient = 0.1f; // implicit vs http://en.cppreference.com/w/cpp/language/explicit
  Shade   shadeOfTheRay;
  Vector3 hitPoint;

  if (sphere.detectHit(ray, hitPoint))
  {
    // The ray hit the sphere, let's find the bounce angle and shade it
    // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
    Vector3 hitNormal    = sphere ^ hitPoint;
    Vector3 hitReflected = ray.direction - (hitNormal * 2.0f * (ray.direction % hitNormal));
    Vector3 hitLight     = ~(light - hitPoint);
    float   diffuse      = MAXF(0.0f, hitLight % hitNormal);    // How similar are they?
    float   specular     = MAXF(0.0f, hitLight % hitReflected); // How similar are they?

    // diffuse  = similarity (dot product) of hitLight and hitNormal
    // specular = similarity (dot product) of hitLight and hitReflected
    // https://youtu.be/KDHuWxy53uM
    // And use the diffuse and specular only when they are positive
    // simplifiedPhongShading = specular + diffuse + ambient
    // https://en.wikipedia.org/wiki/Phong_reflection_model
    shadeOfTheRay = light.shade * powf(specular, SMOOTHNESS) + light.shade * diffuse + ambient;
  }
  testAddToChecksumFloat(shadeOfTheRay.value); // Calculating checksums for automated tests
  return shadeOfTheRay;
}


void demoRaytracer()
{
  for (float zoom = 7.5f; zoom <= 20.0f; zoom+=2.5f)
  {
    for (float lightRotate = 0.0f; lightRotate < 2.0f * M_PI_F; lightRotate += M_PI_F / ROTATION_STEPS)
    {
      for (int iteration = 0; iteration < ITERATIONS; iteration++)
      {
        Light light(Vector3(2.0f * WIDTH  *  cosf(lightRotate),
                            3.0f * HEIGHT * (sinf(lightRotate)-0.5f), -100.0f), Shade(0.7f));

        // Calculate ray for each pixel on the scene
        for (int y = 0; y < HEIGHT; y++) {
          for (int x = 0; x < WIDTH; x++) {
            Ray rayForThisPixel( Vector3(0.0f,              0.0f,             0.0f),
                                ~Vector3(x/2 - (WIDTH / 4), y - (HEIGHT / 2), zoom));
#if VT100_COLORS == 1
            calculateShadeOfTheRay(rayForThisPixel, light).colorize();
#else
            putchar(calculateShadeOfTheRay(rayForThisPixel, light));
#endif
          }
          if (y<(HEIGHT-1)) printf("\r\n"); // breaks after each row, except the last
        }
#ifdef SERIAL_TERMINAL_ANIMATION
      printf("\033[0;0H"); // http://www.termsys.demon.co.uk/vtansi.htm
#endif
      }
    }
  }

}


