

#include <cmath>
#include "constants.h"

void drawEllipse(float xradius, float yradius)
{
  glBegin(GL_LINE_LOOP);

  for (int i=0; i < 360; i++)
  {
    long double degInRad = i * deg2rad;
    glVertex2f(cos(degInRad)*xradius,sin(degInRad)*yradius);
  }

  glEnd();
}

void drawCircle(float radius)
{
  glBegin(GL_LINE_LOOP);

  for (int i=0; i < 360; i++)
  {
    long double degInRad = i*deg2rad;
    glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
  }

  glEnd();
}


