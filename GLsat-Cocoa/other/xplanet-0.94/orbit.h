/****************************************************************************
    orbit - orbit class header
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

// First we have to build up some mathematical toolkit so the orbit
// calculations can be expressed concisely.

// We'll have to have some stripped down matrix and vector handling
// facilities.

// Sorry folks, most of this code is inline. Don't think I like it.

#include <iostream.h>
#include <math.h>

#define DEG2RAD(Phi) (M_PI/180.0*Phi)
#define RAD2DEG(Phi) (180.0/M_PI*Phi)

class Vector;

// The classes matrix and vector do exactly what one would expect from
// them.

class Matrix {
private:
  double el[4][4];
public:
  Matrix() {
    for (int i=0; i<4; i++)
      for (int j=0; j<4; j++)
	el[i][j]=0.0;
  }
  void operator=(const Matrix& a) {
    for (int i=0; i<4; i++)
      for (int j=0; j<4; j++)
        el[i][j]=a.el[i][j];
  }
  Matrix(const Matrix& a) {
    for (int i=0; i<4; i++)
      for (int j=0; j<4; j++)
        el[i][j]=a.el[i][j];
  }
  double& element(int idx1, int idx2) { return(el[idx1][idx2]); }
  void Print() {
    cout << "[ ";
    for (int i=0; i<4; i++) {
      cout << "[ ";
      for (int j=0; j<4; j++)
	cout << el[i][j] << (j<4-1?", ":"");
      cout << "]" << (i<4-1?"\n":"");
    }
    cout << "]\n";
  }
  friend Matrix operator*(const Matrix& a, const Matrix& b);
  friend Matrix operator+(const Matrix& a, const Matrix& b);
  friend Vector operator*(const Matrix& a, const Vector& x);
  static Matrix TranslateMatrix(double x, double y, double z);
  static Matrix RotateXMatrix(double Phi);
  static Matrix RotateZMatrix(double Phi);
  static Matrix RotateYMatrix(double Phi);
  static Matrix ScaleMatrix(double x, double y, double z);
};

Matrix operator*(const Matrix& a, const Matrix& b);
Matrix operator+(const Matrix& a, const Matrix& b);
Vector operator*(const Matrix& a, const Vector& x);

class Vector {
private:
  double el[4];
public:
  Vector(double x, double y, double z) {
    el[0]=x; el[1]=y; el[2]=z; el[3]=1;
  }
  Vector() {
    for (int i=0; i<3; i++)
      el[i]=0.0;
    el[3]=1.0; }
  Vector operator=(const Vector& a) {
    for (int i=0; i<4; i++) el[i]=a.el[i];
    return a;
  }
  Vector(const Vector& a) {
    for (int i=0; i<4; i++) el[i]=a.el[i];
  }
  double& operator[](const int index) { return(el[index]); }
  void Print() {
    cout << "[ ";
    for (int i=0; i<4; i++) cout << el[i] << (i<4-1?", ":"");
    cout << "]";
  }
  void PrintPol() {
    cout << "[ " << RAD2DEG(this->Phi()) << ", "
	 << RAD2DEG(this->Psi()) << ", " << this->Length() << " ]";
  }
  double Length() { return sqrt(el[0]*el[0]+el[1]*el[1]+el[2]*el[2]); }
  double Phi() {
    return(atan2(el[1],el[0]));
  }
  double Psi() {
    return(atan2(el[2],sqrt(el[0]*el[0]+el[1]*el[1])));
  }
  friend Vector operator*(const Matrix& a, const Vector& x);
  friend double operator*(const Vector& a, const Vector& x);
};

// Orbit translates a major circle lying in the aequatorial plane
// so that it passes over the specified starting position at time 0
// and has an inclination at that point of dir degrees (90 = north).

class Orbit {
private:
  double duration;
  Vector startpos, posnow;
  double startdir;
  double localnorth;
  double elapsedtime;
  Matrix translate;
public:
  Orbit(double dur, double pos_lat, double pos_long, double dir) {
    duration=dur;
    startpos=Vector(cos(DEG2RAD(pos_lat))*cos(DEG2RAD(pos_long)),
		    cos(DEG2RAD(pos_lat))*sin(DEG2RAD(pos_long)),
		    sin(DEG2RAD(pos_lat)));
    startdir=dir;
    elapsedtime=0;
    translate=
      Matrix::RotateZMatrix(startpos.Phi())*
      Matrix::RotateYMatrix(startpos.Psi())*
      Matrix::RotateXMatrix(-DEG2RAD(startdir));
    this->Update();
  }
  void SetElapsedTime(double t) { elapsedtime=t; this->Update(); }
  void WaitTime(double t) { elapsedtime+=t; this->Update(); }
  double Long() {
    double ret=RAD2DEG(posnow.Phi());
    return(ret>0?ret:ret+360.0);
  }
  double Lat() { return(RAD2DEG(posnow.Psi())); }
  double Theta() { return(RAD2DEG(localnorth)); }
private:
  // Modified by Daniel Martin (Daniel.Martin@jhu.edu), March 2000
  void Update() {
    Matrix currentcoords;
    Vector compass;  // Because it points north
    Vector ynow, znow;
    if (duration == 0) return;
    currentcoords=translate*
      Matrix::RotateZMatrix(2.0*M_PI*elapsedtime/duration);
    posnow = currentcoords*Vector(1,0,0);
    ynow = currentcoords*Vector(0,1,0);
    znow = currentcoords*Vector(0,0,1);
    compass =
      Matrix::RotateZMatrix(posnow.Phi())*
      Matrix::RotateYMatrix(posnow.Psi())*Vector(0,0,1);
    // The -1 is because these vectors have an extra fourth
    // coordinate (uh, why?) that adds one to the scalar product
    localnorth = atan2(compass*ynow - 1, compass*znow - 1);
  }
};
