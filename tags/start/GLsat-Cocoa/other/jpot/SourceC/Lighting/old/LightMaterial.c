#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "../../mesa_sgi.h"
GLint please_read_data=0;
FILE * fp=NULL;
GLfloat option=0;
GLint spec=0;
void sighandle(int);
void init(int argc, char * arg_vec[])
{
  spec = atoi (arg_vec[1]);
  signal(SIGUSR2,sighandle);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
} 

void drawaxes()
{
  /* Next draw the Coordinate Axes */
  glLineWidth(2.0);
  glBegin(GL_LINES);
    glVertex3f(-7,0,0);
    glVertex3f(7,0,0);
  glEnd();
  glBegin(GL_LINES);
    glVertex3f(0,-7,0);
    glVertex3f(0,7,0);
  glEnd();
  glBegin(GL_LINES);   
    glVertex3f(0, 0, -12);
    glVertex3f(0, 0, 7);
  glEnd();
}

void display()
{ 
  GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat material_shininess[] = { 50 };
  GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat material_ambient[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat material_emission[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0};
  if (ALIASMODE == 0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);  
   }
  
  
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   
  glLoadIdentity();          /* clear the matrix */
  if(spec==1) {
    gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);
    glTranslatef(0, 0, 3);
  }
  else
  gluLookAt(2, 4, 5, 0 , 0 , 0, 0, 1, 0);
  
  if( (fp=fopen("link","r")) == NULL)
    { }
  
  else {
    while ( !feof(fp) ) {
    fscanf(fp, "%f\n", &option);
    if(option==0){
    fscanf(fp, "%f %f %f \n", 
          &light_ambient[0],
          &light_ambient[1],
          &light_ambient[2]);
    }
    else if(option == 1) {
     fscanf(fp, "%f %f %f \n", 
          &light_diffuse[0],
          &light_diffuse[1],
          &light_diffuse[2]);
      
     }
    else if(option == 2) {
     fscanf(fp, "%f %f %f \n", 
          &light_specular[0],
          &light_specular[1],
          &light_specular[2]);
     material_shininess[0] = 10;
     }

    else if(option == 3) {
     fscanf(fp, "%f %f %f %f\n", 
          &light_position[0],
          &light_position[1],
          &light_position[2],
          &light_position[3]);
      }
    
     else if(option == 4) {
     fscanf(fp, "%f %f %f \n", 
          &material_ambient[0],
          &material_ambient[1],
          &material_ambient[2]);
      }
     else if(option == 5) {
     fscanf(fp, "%f %f %f \n", 
          &material_diffuse[0],
          &material_diffuse[1],
          &material_diffuse[2]);
      }

   else if(option == 6) {
     fscanf(fp, "%f %f %f \n", 
          &material_specular[0],
          &material_specular[1],
          &material_specular[2]);
          material_shininess[0] = 10;
      }

  else if(option == 7) {
      fscanf(fp, "%f\n", 
          &material_shininess[0]);
      }
  
  else if(option == 8) {
     fscanf(fp, "%f %f %f \n", 
          &material_emission[0],
          &material_emission[1],
          &material_emission[2]);

      }
    else if(option == 9) {
      fscanf(fp, "%f %f %f \n", 
          &light_ambient[0],
          &light_ambient[1],
          &light_ambient[2]);
      
      fscanf(fp, "%f %f %f \n", 
          &light_diffuse[0],
          &light_diffuse[1],
          &light_diffuse[2]);
      
      fscanf(fp, "%f %f %f \n", 
          &light_specular[0],
          &light_specular[1],
          &light_specular[2]);
      
      fscanf(fp, "%f %f %f %f \n",     
          &light_position[0],
          &light_position[1],
          &light_position[2],
          &light_position[3]);

      fscanf(fp, "%f %f %f \n", 
          &material_ambient[0],
          &material_ambient[1],
          &material_ambient[2]);

      fscanf(fp, "%f %f %f \n", 
          &material_diffuse[0],
          &material_diffuse[1],
          &material_diffuse[2]);

      fscanf(fp, "%f %f %f \n", 
          &material_specular[0],
          &material_specular[1],
          &material_specular[2]);

      fscanf(fp, "%f\n", 
          &material_shininess[0]);

      fscanf(fp, "%f %f %f \n", 
          &material_emission[0],
          &material_emission[1],
          &material_emission[2]);

    }
 
  }
 fclose (fp);
  } 
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);  
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  drawaxes();
  glTranslatef(0, 0, -3);
  glutSolidSphere(1.0, 30, 30);;
  glFlush();
}

void reshape (int w, int h)
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 2.0, 40.0);
  glMatrixMode(GL_MODELVIEW);
}

void idlefunc(void)
{
  if(please_read_data) {
     please_read_data=0;
     glutPostRedisplay();
  }
}

void sighandle(int s)
{
  please_read_data=1;
  signal(SIGUSR2,sighandle);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow ("Light - Diffuse Component");
   init (argc, argv);
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutIdleFunc(idlefunc);
   glutMainLoop();
   return 0;
}


/* 
 *  by Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
