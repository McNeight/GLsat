#ifndef ZPR_H
#define ZPR_H

/* 
 * Zoom-pan-rotate mouse manipulation module for GLUT
 * Version 0.1, August 2001
 *
 * Nigel Stewart
 * School of Computer Science and Information Technology (SCSIT)
 * RMIT University
 * nigels@cs.rmit.edu.au
 *
 * Instructions
 * ------------
 *
 * Insert the following line of code after glutCreateWindow()
 *   zprInit();
 *
 * This will provide mouse manipulation of your GLUT window
 * via the modelview matrix.
 *
 * Left   button -> rotate
 * Middle button -> zoom
 * Right  button -> pan
 *
 * Limitations
 * -----------
 *
 * Only works with ortho projection.
 * You need to have glEnable(GL_NORMALIZATION) enabled.
 * Near and far clip planes are hard-coded.
 * Zooming and rotation is centered on the origin.
 * Only one window can use the callbacks at one time. 
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *
 */

void zprInit();

/*
 *
 */

#ifdef __cplusplus
}
#endif

#endif
