#include <GL/glut.h>
#include <stdlib.h>

static GLfloat light0_position[] = {1.0, 3.0, 1.0, 0.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_red[]     = {1.0, 0.0, 0.0, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_amb_diff_blue[]    = {0.0, 0.0, 1.0, 1.0};
GLfloat mat_amb_diff_yellow[]  = {1.0, 1.0, 0.0, 1.0}; 
GLfloat mat_amb_diff_magenta[] = {1.0, 0.0, 1.0, 1.0}; 
GLfloat mat_amb_diff_cyan[]    = {0.0, 1.0, 1.0, 1.0}; 
GLint id1, id2;

void drawglobalaxes()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_white);
  glLineWidth (2.0);

  glBegin(GL_LINES);
    glVertex3f(-7,0,0);
    glVertex3f(0,0,0);
  glEnd();
	
  glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(7,0,0);
  glEnd();
	
  glBegin(GL_LINES);   
    glVertex3f(0, -7, 0);
    glVertex3f(0, 0, 0);
  glEnd();

  glBegin(GL_LINES);   
    glVertex3f(0, 0, 0);
    glVertex3f(0, 7, 0);
  glEnd();
	
  glBegin(GL_LINES);   
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -7);
  glEnd();
	
  glBegin(GL_LINES);   
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 7);
  glEnd();
}


void drawlocalaxes()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
  glLineWidth (1.5);

  glBegin(GL_LINES);
    glVertex3f(-2.8,0,0);
    glVertex3f(0,0,0);
  glEnd();
	
  glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(2.8,0,0);
  glEnd();
	
  glBegin(GL_LINES);   
    glVertex3f(0, -2.8, 0);
    glVertex3f(0, 0, 0);
  glEnd();
	
  glBegin(GL_LINES);   
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2.8, 0);
  glEnd();
	
  glBegin(GL_LINES);   
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -2.8);
  glEnd();
	
  glBegin(GL_LINES);   
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2.8);
  glEnd();
}


void drawcube() {

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
  /* draw the back */
  glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
  glEnd();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
  /* draw the left */
  glBegin(GL_POLYGON);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
  glEnd();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_blue);
  /* draw the right */
  glBegin(GL_POLYGON);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, 1.0);
  glEnd();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_yellow);
  /* draw the front */
  glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
  glEnd();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_magenta);
  /* draw the bottom */
  glBegin(GL_POLYGON);
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
  glEnd();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
  /* draw the top */
  glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
  glEnd();
}


void init()
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}


void reshape (int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 0.1, 50.0);
  glMatrixMode(GL_MODELVIEW);
}


void display1()
{     
  GLint i = 0;

  glutSetWindow(id1);
  glPushMatrix();
  glLoadIdentity();
  gluLookAt(5, 12, 16, 0, 0, 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  drawglobalaxes();

  for(i = 0; i <= 30; i++)
    {  
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      drawglobalaxes();
      glPushMatrix();
        drawcube();
      glPopMatrix();
      glutSwapBuffers();
    }

  for(i = 0; i <= 30; i++)
    {  
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      drawglobalaxes();
      glPushMatrix();
        glTranslatef(0.2*i, 0.0, 0.0);
        drawcube();
      glPopMatrix();
      glutSwapBuffers();
    }
	
  for(i = 0; i <= 30; i++)
    {  
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      drawglobalaxes();
      glPushMatrix();
        glRotatef(3.0*i, 0.0, 1.0, 0.0);
        glTranslatef(6.0, 0.0, 0.0);
	drawcube();
      glPopMatrix();
      glutSwapBuffers();
    }
        
    for(i = 0; i <= 30; i++)
      {  
        glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawglobalaxes();
	glPushMatrix();
	  glTranslatef(0.1*i, 0.0, 0.0);
	  glRotatef(90, 0.0, 1.0, 0.0);
	  glTranslatef(6.0, 0.0, 0.0);
	  drawcube();
	glPopMatrix();
	glutSwapBuffers();
      }

    glPopMatrix(); 
    glutSwapBuffers();    
    glFlush();
}

void display2()
{     
  GLint i = 0;

  glutSetWindow(id2);
  glPushMatrix();
  glLoadIdentity();
  gluLookAt(5, 12, 16, 0, 0, 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  drawglobalaxes();

  for(i = 0; i <= 30; i++)
    {  
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      drawglobalaxes();
      glPushMatrix();
        drawlocalaxes();
      glPopMatrix();
      glutSwapBuffers();
    }

  for(i = 0; i <= 30; i++)
    {  
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      drawglobalaxes();
      glPushMatrix();
        glTranslatef(0.1*i, 0.0, 0.0);
        drawlocalaxes();
      glPopMatrix();
      glutSwapBuffers();
    }
	
  for(i = 0; i <= 30; i++)
    {  
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      drawglobalaxes();
      glPushMatrix();
        glTranslatef(3.0, 0.0, 0.0);
        glRotatef(3.0*i, 0.0, 1.0, 0.0);
	drawlocalaxes();
      glPopMatrix();
      glutSwapBuffers();
    }
        
    for(i = 0; i <= 30; i++)
      {  
        glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawglobalaxes();
	glPushMatrix();
	  glTranslatef(3.0, 0.0, 0.0);
	  glRotatef(90, 0.0, 1.0, 0.0);
	  glTranslatef(0.2*i, 0.0, 0.0);
	  drawlocalaxes();
	glPopMatrix();
	glutSwapBuffers();
      }

    for(i = 0; i <= 30; i++)
      {  
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        drawglobalaxes();
        glPushMatrix();
          glTranslatef(3.0, 0.0, 0.0);
  	  glRotatef(90, 0.0, 1.0, 0.0);
	  glTranslatef(6.0, 0.0, 0.0);
	  drawlocalaxes();
        glPopMatrix();
        glutSwapBuffers();
      }

    for(i = 0; i <= 30; i++)
      {  
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        drawglobalaxes();
        glPushMatrix();
          glTranslatef(3.0, 0.0, 0.0);
  	  glRotatef(90, 0.0, 1.0, 0.0);
	  glTranslatef(6.0, 0.0, 0.0);
	  drawlocalaxes();
	  drawcube();
        glPopMatrix();
        glutSwapBuffers();
      }

    for(i = 0; i <= 30; i++)
      {  
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        drawglobalaxes();
        glPushMatrix();
          glTranslatef(3.0, 0.0, 0.0);
  	  glRotatef(90, 0.0, 1.0, 0.0);
	  glTranslatef(6.0, 0.0, 0.0);
	  drawcube();
        glPopMatrix();
        glutSwapBuffers();
      }

    glPopMatrix(); 
    glutSwapBuffers();    
    glFlush();
}


void mouse(int button, int state, int x, int y)
{
  switch(button) {
  case GLUT_LEFT_BUTTON:
  case GLUT_MIDDLE_BUTTON:
  case GLUT_RIGHT_BUTTON:
    if(state == GLUT_DOWN)
      glutPostRedisplay();
    break;
  default:
    break;
  }
}


int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE |GLUT_RGB|GLUT_DEPTH);
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(0, 0);
  id1 = glutCreateWindow("Fixed Coordinate System Approach");
  init(); 
  glutDisplayFunc(display1);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(0, 300);
  id2 = glutCreateWindow("Fixed Coordinate System Approach II");
  init();
  glutDisplayFunc(display2);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMainLoop();
  return 0;
}
