#include "CubeFire.h"

namespace Animations { namespace Spatial {
  void CubeFire::beforeRender() {
      t1 = ((float)beat16(9.15/speed))/65535;
      t2 = ((float)beat16(7.04/speed))/65535;
      t3 = ((float)beat16(10.77/speed))/65535;

      // Oscillate the scale coefficient of space between 0.25 and 0.75
      scale = (.5 + ((float)beatsin16(9.15))/65535) / 2;
  }

  float wave(float v) {
    return (1+((float)sin16(v*65535))/65535)/2;
  }

  void CubeFire::render(int index) {
    if (index == 0) {
      beforeRender();
    }
    Coordinate& coordinate = context.getLedCoordinate(index);

    // Color is 20% dependent on each axis and cycling every 6.5 seconds
    float h = coordinate.x / 5 + coordinate.y / 5 + coordinate.z / 5 + t1;
  
    // Since wave() returns a 0..1 sinusoid, and we multiply it by other 
    // phase-offset wave()s, the final output will be a series of spheres in space
    // with a value of 0..10
    float v = 10 * (wave(coordinate.x * scale + wave(t1)) * 
              wave(coordinate.y * scale + wave(t2)) * 
              wave(coordinate.z * scale + wave(t3)));
              
    // The outer surface of the spheres, with the lowest values, will be white. v
    // values between 2 and 10 (the core of the spheres) will be colorful.
    float s = v - 1;

    /*
      This looks like typical gamma correction here, but really it only serves to
      increase the negative space between nearby spheres; after this the cores
      will all have v > 1 (e.g. center v == 10^3)
    */
    v = v * v * v;

    byte hb = min((float)255,h*255);
    byte sb = min((float)255,s*255);
    byte vb = min((float)255,v*255);
    context.hsv(CHSV(hb,sb,vb));
  }
}}