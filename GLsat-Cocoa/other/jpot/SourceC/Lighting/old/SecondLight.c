/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 *  light.c
 *  This program demonstrates the use of the OpenGL lighting
 *  model.  A sphere is drawn using a grey material characteristic.
 *  A single light source illuminates the object.
 */
/*  Modified by Mahesh Kumar.  Add a second light , a spot light.
 *  The menu options allow the user to turn on or off both lights,
 *  set the ambient, diffuse, specular components for the spot 
 *  light, the attenuation factors, the postion and the cutoff
 *  angle of the spotlight.
 */
#include <GL/glut.h>
#include <stdlib.h>

GLint   l_menu, spot_menu, sdiff_menu, sspec_menu, spos_menu, samb_menu;
GLint   constatt_menu, linatt_menu, quadatt_menu, cutoff_menu;
GLfloat sdred=1, sdgreen=1, sdblue=1, sared=0.2, sagreen=0.2, sablue=0.2;
GLfloat ssred=1.0, ssgreen=1.0, ssblue=1.0, sposX=-2.0, sposY=0.0, sposZ=2.0;
GLfloat sdirX=2.0, sdirY=0.0, sdirZ=-2.0, scatt=1.5, slatt=0.5, sqatt=0.2;
GLfloat scut=10.0;
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50 };
GLfloat mat_amb_diff_white[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat light1_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light1_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light1_position[] = {-2.0, 0.0, 2.0, 1.0};
GLfloat spot_direction[] = {2.0, 0.0, -2.0};
/*GLfloat cutoff_angle[] = {10};*/
/*  Initialize material property, light source, lighting model,
 *  and depth buffer.
 */
void top_menu(int value)
{if (value==1) exit(0);
 
 }
void light_menu(int condition)
{
  if(condition==1) glEnable(GL_LIGHT0);
  if(condition==2) glDisable(GL_LIGHT0);
  glutPostRedisplay();
}
void spotlight_menu(int toggle)
{
  if(toggle==1) glEnable(GL_LIGHT1);
  if(toggle==2) glDisable(GL_LIGHT1);
  glutPostRedisplay();
}
void spotdiffuse_menu(int sdcolor)
{  
  if(sdcolor==1) { 
     sdred = 1;
     sdgreen=sdblue=0;
  }
  if(sdcolor==2) {
     sdgreen=1;
     sdred=sdblue=0;
  }
  if(sdcolor==3) {
     sdblue=1;
     sdred=sdgreen=0;
  }
  if(sdcolor==4) {
     sdred=sdgreen=1;
     sdblue=0;
  }
  if(sdcolor==5)
     sdred=sdgreen=sdblue=1;
     glutPostRedisplay();
}

void spotambient_menu(int sacolor)
{
  if(sacolor==1) { 
     sared = 1;
     sagreen=sablue=0;

}
  if(sacolor==2) {
     sagreen=1;
     sared=sablue=0;

}
  if(sacolor==3) {
     sablue=1;
     sared=sagreen=0;

  }
  if(sacolor==4) {
     sared=sagreen=1;
     sablue=0;

  }
  if(sacolor==5)
     sared=sagreen=sablue=1;

  if(sacolor==6) 
    sared=sagreen=sablue=0.2;
     glutPostRedisplay();
}
void spotspecular_menu(int sspec)
{
  if(sspec==1) {
    ssred = 1;
    ssgreen = ssblue = 0;
  }
  if(sspec==2) {
    ssgreen=1;
    ssred=ssblue=0;
  }
  if(sspec==3) {
    ssblue=1;
    ssred=ssgreen=0;
  }
  if(sspec==4) {
    ssred=ssgreen=ssblue=1;
  }
  glutPostRedisplay();
}
void spotposition_menu(int position)
{
  if(position==1) {
    sposX=2.0;
    sposY=2.0;
    sposZ=2.0;
    sdirX=-2.0;
    sdirZ=-2.0;
    sdirY=-2.0;
  }
 if(position==2) {
    sposX=0.0;
    sposY=2.0;
    sposZ=3.0;
    sdirX=0.0;
    sdirY=-2.0;
    sdirZ=-3.0;
  }
 if(position==3) {
    sposX=0.0;
    sposY=0.0;
    sposZ=1.5;
    sdirX=0.0;
    sdirY=0.0;
    sdirZ=-1.0;
  }
 if(position==4){
   sposX=-2.0;
   sposY=0.0;
   sposZ=2.0;
   sdirX=2.0;
   sdirY=0.0;
   sdirZ=-2.0;
 }
   glutPostRedisplay();
}


void spot_constattmenu(int catt)
{
  if(catt==1)
    scatt=0.0;
  if(catt==2)
    scatt=3.0;
  if(catt==3)
    scatt=6.0;
  if(catt==4)
    scatt=1.5;
  glutPostRedisplay();
}

void spot_linattmenu(int latt)
{
  if(latt==1)
   slatt=0.0;
  if(latt==2)
   slatt=4.0;
  if(latt==3)
   slatt=8.0;
  if(latt==4)
   slatt=0.5;
  glutPostRedisplay();
}

void spot_quadattmenu(int qatt)
{
   if(qatt==1)
     sqatt=0;
   if(qatt==2)
     sqatt=2.5;
   if(qatt==3)
     sqatt=5.0;
   if(qatt==4)
     sqatt=0.2;
   glutPostRedisplay();
}

void spot_cutoffmenu(int cutoff)
{
  if(cutoff==1)
    scut=0;
  if(cutoff==2)
   scut=2;
  
  if(cutoff==3)
    scut=5;
  
  if(cutoff==4)
    scut=180;
  
  if(cutoff==5)
    scut=12;
    
  if(cutoff==6)
    scut=10;
    
  glutPostRedisplay();
}



void init(void) 
{
  
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);  
   glEnable(GL_DEPTH_TEST);
}


void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   light1_diffuse[0] = sdred;
   light1_diffuse[1] = sdgreen;
   light1_diffuse[2] = sdblue;
   light1_diffuse[3] = 1.0;
   light1_ambient[0] = sared;
   light1_ambient[1] = sagreen;
   light1_ambient[2] = sablue;
   light1_ambient[3] = 1.0;
   
   light1_specular[0] = ssred;
   light1_specular[1] = ssgreen;
   light1_specular[2] = ssblue;
   light1_specular[3] = 1.0;
   light1_position[0] = sposX;
   light1_position[1] = sposY;
   light1_position[2] = sposZ;
   light1_position[3] = 1.0;
   spot_direction[0]= sdirX;
   spot_direction[1]= sdirY;
   spot_direction[2]= sdirZ;
      
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_white);
   glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

   glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, scatt);
   glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, slatt);
   glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, sqatt);
   glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, scut);
   glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
   glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
  
   glEnable(GL_DEPTH_TEST);
   glutSolidSphere (1.0, 40, 40);
   glFlush ();
}
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow ("A Second(Spot) Light Source");
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   
   l_menu = glutCreateMenu(light_menu);
   glutAddMenuEntry("Enable Light1", 1);
   glutAddMenuEntry("Disable Light1", 2);
   spot_menu = glutCreateMenu(spotlight_menu);
   glutAddMenuEntry("Enable spotlight", 1);
   glutAddMenuEntry("Disable spotlight", 2);
   
   sdiff_menu = glutCreateMenu(spotdiffuse_menu);
   glutAddMenuEntry("red light", 1);
   glutAddMenuEntry("green light", 2);
   glutAddMenuEntry("blue light", 3);
   glutAddMenuEntry("yellow light", 4);
   glutAddMenuEntry("white light", 5);
   
   samb_menu = glutCreateMenu(spotambient_menu);
   glutAddMenuEntry("red light", 1);
   glutAddMenuEntry("green light", 2);
   glutAddMenuEntry("blue light", 3);
   glutAddMenuEntry("yellow light", 4);
   glutAddMenuEntry("white light", 5);
   glutAddMenuEntry("default" , 6);

   sspec_menu = glutCreateMenu(spotspecular_menu);
   glutAddMenuEntry("All Red:", 1);
   glutAddMenuEntry("All Green", 2);
   glutAddMenuEntry("All Blue", 3);
   glutAddMenuEntry("All White",4);
   
   spos_menu = glutCreateMenu(spotposition_menu);
   glutAddMenuEntry("X=2.0, Y=2.0,Z=2.0", 1);
   glutAddMenuEntry("X=0.0, Y=2.0, Z=3.0", 2);
   glutAddMenuEntry("X=Y=0.0, Z=1.5", 3);
   glutAddMenuEntry("X=-2.0, Y=0.0,Z=2.0 - default", 4);
   
   constatt_menu = glutCreateMenu(spot_constattmenu);
   glutAddMenuEntry("Att. Factor = 0.0", 1);
   glutAddMenuEntry("Att. Factor = 3.0", 2);
   glutAddMenuEntry("Att. Factor = 6.0", 3);
   glutAddMenuEntry("Att. Factor = 1.5", 4);
  
   linatt_menu = glutCreateMenu(spot_linattmenu);
   glutAddMenuEntry("Att. Factor = 0.0", 1);
   glutAddMenuEntry("Att. Factor = 4.0", 2);
   glutAddMenuEntry("Att. Factor = 8.0", 3);
   glutAddMenuEntry("Att. Factor = 0.5", 4);
   
   quadatt_menu = glutCreateMenu(spot_quadattmenu);
   glutAddMenuEntry("Att. Factor = 0.0", 1);
   glutAddMenuEntry("Att. Factor = 2.5", 2);
   glutAddMenuEntry("Att. Factor = 5.0", 3);
   glutAddMenuEntry("Att. Factor = 0.2", 4);
   
   cutoff_menu = glutCreateMenu(spot_cutoffmenu);
   glutAddMenuEntry("Angle = 0", 1);
   glutAddMenuEntry("Angle = 2", 2);
   glutAddMenuEntry("Angle = 5", 3);
   glutAddMenuEntry("Angle = 180", 4);
   glutAddMenuEntry("Angle = 12", 5);
   glutAddMenuEntry("Angle = 10", 6);
   
   glutCreateMenu(top_menu);
   glutAddSubMenu("Light1 ON/OFF", l_menu);
   glutAddSubMenu("SpotLight ON/OFF", spot_menu);
   glutAddSubMenu("SpotLight Diffuse", sdiff_menu);
   glutAddSubMenu("Spotlight Ambient", samb_menu);
   glutAddSubMenu("Spotlight Specular", sspec_menu);
   glutAddSubMenu("Spotlight Position", spos_menu);
   glutAddSubMenu("Spotlight Const. Att", constatt_menu);
   glutAddSubMenu("Spotlight Linear Att", linatt_menu);
   glutAddSubMenu("Spotlight Quad Att", quadatt_menu);
   glutAddSubMenu("Spotlight Cutoff angle", cutoff_menu);
   glutAddMenuEntry("quit", 1);
   
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}


