/****************************************************************************
    orbit - orbit class
    Copyright (C) 2000 Detlev Zundel <Detlev.Zundel@stud.uni-karlsruhe.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <cmath>
using namespace std;

#include "orbit.h"

Matrix operator*(const Matrix& a, const Matrix& b){
  Matrix c;
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      for (int k=0; k<4; k++)
	c.el[i][j]+=a.el[i][k]*b.el[k][j];
  return c;
}

Matrix operator+(const Matrix& a, const Matrix& b){
  Matrix c;
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      c.el[i][j]=a.el[i][j]+b.el[i][j];
  return c;
}

Vector operator*(const Matrix& a, const Vector& x)
{
  Vector c;
  c.el[3]=0;
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      c.el[i]+=a.el[i][j]*x.el[j];
  return c;
}

// This routine written by Daniel Martin (Daniel.Martin@jhu.edu), March 2000
double operator*(const Vector& x, const Vector& y)
{
  double r = 0;
  for (int i=0; i<4; i++)
    r+=x.el[i]*y.el[i];
  return r;
}

Matrix Matrix::TranslateMatrix(double x, double y, double z)
{
  Matrix c;
  for (int i=0; i<4; i++)
    c.element(i,i)=1.0;
  c.element(0,3)=x;
  c.element(1,3)=y;
  c.element(2,3)=z;
  return c;
}

Matrix Matrix::RotateXMatrix(double alpha)
{
  Matrix c;
  c.element(0,0)=1;
  c.element(1,1)=c.element(2,2)=cos(alpha);
  c.element(1,2)=sin(alpha);
  c.element(2,1)=-sin(alpha);
  c.element(3,3)=1;
  return c;
}

Matrix Matrix::RotateZMatrix(double alpha)
{
  Matrix c;
  c.element(0,0)=c.element(1,1)=cos(alpha);
  c.element(0,1)=-sin(alpha);
  c.element(1,0)=sin(alpha);
  c.element(2,2)=1;
  c.element(3,3)=1;
  return c;
}

Matrix Matrix::RotateYMatrix(double alpha)
{
  Matrix c;
  c.element(1,1)=1;
  c.element(0,0)=c.element(2,2)=cos(alpha);
  c.element(0,2)=-sin(alpha);
  c.element(2,0)=sin(alpha);
  c.element(3,3)=1;
  return c;
}

Matrix Matrix::ScaleMatrix(double x, double y, double z)
{
  Matrix c;
  c.element(0,0)=x;
  c.element(1,1)=y;
  c.element(2,2)=z;
  c.element(3,3)=1.0;
  return c;
}
