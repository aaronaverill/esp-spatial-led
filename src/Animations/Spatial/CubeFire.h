#pragma once
#include "Animations/Spatial/SpatialAnimation.h"

#include "Services/ILedDriverAnimationContext.h"

/*
  Cube fire 3D
  
  3D example: https://youtu.be/iTM-7ILud4M
  
  This pattern is designed for 3D mapped projects, but degrades gracefully in 2D
  and 1D.
  
  The base 3D variant is based on multiplying sine waves of x, y, and z 
  position. This results in a regular 3D array of spheres. The size of the
  spheres pulses, and their position in 3D space oscillates at different
  frequencies.
*/

namespace Animations { namespace Spatial {
  class CubeFire: public SpatialAnimation {
    public:
      CubeFire(Services::ILedDriverAnimationContext& context): SpatialAnimation(context, "Cube Fire") {}
      void renderFrame();
      void renderLed(int index, const Coordinate& coordinate);
    private:
      float speed = 1; // How fast the spheres travel through 3D space
      float t1; // x offset
      float t2; // y offset
      float t3; // z offset
      float scale;

      float t1_wave, t2_wave, t3_wave;
  };
}}