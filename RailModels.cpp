//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  RailModels.cpp
//  A collection of functions for generating the models for a railway scene
//  ========================================================================

#include <cmath>
#include <GL/freeglut.h>
#include "RailModels.h"
#include "loadBMP.h"
#include <iostream>
#include <bits/stdc++.h>

using namespace std;
using std::ifstream;

float toRad = 3.14159265/180.0;  //Conversion from degrees to radians
GLUquadricObj*	q;
GLuint txId[11];

//--------------------------------------------------------------------------------
void loadTexture()
{
	q = gluNewQuadric();
	
	glGenTextures(11, txId);   //Get 6 texture IDs 
	glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture name for the following OpenGL texture
	loadBMP("wheel.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	loadBMP("containers.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	loadBMP("engine-front.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	loadBMP("engine.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[4]);
	loadBMP("tree_rings.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[5]);
	loadBMP("wood.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[6]);
	loadBMP("brick.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[7]);
	loadBMP("road.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[8]);
	loadBMP("red_white.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[9]);
	loadBMP("roof.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[10]);
	loadBMP("car.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

struct Point 
{
	float x, y, z;
};


Point initPoint(float x, float y, float z) 
{
	Point point;
	point.x = x;
	point.y = y;
	point.z = z;
	return(point);
}


void normal(float x1, float y1, float z1,
			float x2, float y2, float z2,
			float x3, float y3, float z3 )
{
	float nx, ny, nz;
	nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
	ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
	nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
	glNormal3f(nx, ny, nz);
}


//--------------- GROUND PLANE ------------------------------------
// This is a square shaped region on the xz-plane of size 400x400 units
// centered at the origin.  This region is constructed using small quads
// of unit size, to facilitate rendering of spotlights
//-----------------------------------------------------------------
void floor()
{
	float white[4] = {1., 1., 1., 1.};
	float black[4] = {0};
	glColor4f(0.7, 0.7, 0.7, 1.0);  //The floor is gray in colour
	glNormal3f(0.0, 1.0, 0.0);
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
	
	//The floor is made up of several tiny squares on a 400 x 400 grid. Each square has a unit size.
	glBegin(GL_QUADS);
	for(int i = -200; i < 200; i++)
	{
		for(int j = -200;  j < 200; j++)
		{
			glVertex3f(i, 0, j);
			glVertex3f(i, 0, j+1);
			glVertex3f(i+1, 0, j+1);
			glVertex3f(i+1, 0, j);
		}
	}
	glEnd();
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
}


int getPoints()
{
	std::ifstream file("Oval.txt");
	std::string line;
	
	int points;
	std::stringstream linestream(line);
	std::getline(file, line);
	linestream >> points;
	return points;
}


//--------------- RAILWAY TRACK ------------------------------------
// A circular railway track of specified median radius and width
// The tracks are modelled using small quadrilateral segments each covering 5 deg arc
// The tracks have a height of 1 unit (refer to lab notes)
//-----------------------------------------------------------------
void tracks(float medRadius, float width, float x_points[], float z_points[], int points)  
{
	float inRad  = medRadius - width * 0.5;
	float outRad = medRadius + width * 0.5;
	Point p1, p2, p3;
	Point A1, A2, B1, B2, C1, C2, D1, D2;  //four points of a quad
    float toRad = 3.14159265/180.0;  //Conversion from degrees to radians

	glColor4f(0.0, 0.0, 0.3, 1.0);
    glBegin(GL_QUADS);
	for (int i = 0; i < points; i += 1)    //5 deg intervals
	{
		p1 = initPoint(x_points[i % points], 0, z_points[i % points]);
		p2 = initPoint(x_points[(i + 1) % points], 0, z_points[(i + 1) % points]);
		p3 = initPoint(x_points[(i + 2) % points], 0, z_points[(i + 2) % points]);
		
		float len = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
		Point u = initPoint((p2.x - p1.x) / len, (p2.y - p1.y) / len, (p2.z - p1.z) / len);
		len = sqrt((p2.z - p1.z) * (p2.z - p1.z) + 1.0 + (p2.x - p1.x) * (p2.x - p1.x)) - 0.15;
		Point v1 = initPoint(-1.0 * (p2.z - p1.z) / len, 1.0 / len, (p2.x - p1.x) / len);
		len = sqrt((p3.z - p2.z) * (p3.z - p2.z) + 1.0 + (p3.x - p2.x) * (p3.x - p2.x)) - 0.15;
		Point v2 = initPoint(-1.0 * (p3.z - p2.z) / len, 1.0 / len, (p3.x - p2.x) / len);
		A1 = initPoint(p1.x + v1.x * (width + 0.5), 1.0, p1.z + v1.z * (width + 0.5));
		A2 = initPoint(p1.x + v1.x * (width - 0.5), 1.0, p1.z + v1.z * (width - 0.5));
		B1 = initPoint(p2.x + v2.x * (width + 0.5), 1.0, p2.z + v2.z * (width + 0.5));
		B2 = initPoint(p2.x + v2.x * (width - 0.5), 1.0, p2.z + v2.z * (width - 0.5));
		C1 = initPoint(p1.x - v1.x * (width + 0.5), 1.0, p1.z - v1.z * (width + 0.5));
		C2 = initPoint(p1.x - v1.x * (width - 0.5), 1.0, p1.z - v1.z * (width - 0.5));
		D1 = initPoint(p2.x - v2.x * (width + 0.5), 1.0, p2.z - v2.z * (width + 0.5));
		D2 = initPoint(p2.x - v2.x * (width - 0.5), 1.0, p2.z - v2.z * (width - 0.5));

		glColor4f(0.0, 0.0, 0.3, 1.0);

		// Outer track
		glNormal3f(0., 1., 0.);       //Quad 1 facing up
		glVertex3f(A1.x, 1.0, A1.z);
		glVertex3f(A2.x, 1.0, A2.z);
		glVertex3f(B2.x, 1.0, B2.z);
		glVertex3f(B1.x, 1.0, B1.z);

		glNormal3f(-v1.x, 0.0, -v1.z);   //Quad 2 facing inward
		glVertex3f(A1.x, 0.0, A1.z);
		glVertex3f(A1.x, 1.0, A1.z);
		glNormal3f(-v2.x, 0.0, -v2.z);
		glVertex3f(B1.x, 1.0, B1.z);
		glVertex3f(B1.x, 0.0, B1.z);

		glNormal3f(v1.x, 0.0, v1.z);   //Quad 3 facing outward
		glVertex3f(A2.x, 1.0, A2.z);
		glVertex3f(A2.x, 0.0, A2.z);
		glNormal3f(v2.x, 0.0, v2.z);
		glVertex3f(B2.x, 0.0, B2.z);
		glVertex3f(B2.x, 1.0, B2.z);
		
		//// Inner track
		glNormal3f(0., 1., 0.);       //Quad 1 facing up
		glVertex3f(C1.x, 1.0, C1.z);
		glVertex3f(C2.x, 1.0, C2.z);
		glVertex3f(D2.x, 1.0, D2.z);
		glVertex3f(D1.x, 1.0, D1.z);

		glNormal3f(v1.x, 0.0, v1.z);   //Quad 2 facing inward
		glVertex3f(C1.x, 0.0, C1.z);
		glVertex3f(C1.x, 1.0, C1.z);
		glNormal3f(v2.x, 0.0, v2.z);
		glVertex3f(D1.x, 1.0, D1.z);
		glVertex3f(D1.x, 0.0, D1.z);

		glNormal3f(-v1.x, 0.0, -v1.z);   //Quad 3 facing outward
		glVertex3f(C2.x, 1.0, C2.z);
		glVertex3f(C2.x, 0.0, C2.z);
		glNormal3f(-v2.x, 0.0, -v2.z);
		glVertex3f(D2.x, 0.0, D2.z);
		glVertex3f(D2.x, 1.0, D2.z);
		
		// Sleepers
		if (i % 5 == 0) {
			float s1 = 10.0;
			float s2 = 1.5;
			
			glColor4f(0.3, 0.0, 0.0, 1.0);
			
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(p1.x + v1.x * s1, 0.1, p1.z + v1.z * s1);
			glVertex3f(p1.x + u.x * s2 + v1.x * s1, 0.1, p1.z + u.z * s2 + v1.z * s1);
			glVertex3f(p1.x + u.x * s2 - v1.x * s1, 0.1, p1.z + u.z * s2 - v1.z * s1);
			glVertex3f(p1.x - v1.x * s1, 0.1, p1.z - v1.z * s1);
		}
	}
	glEnd();
}


//--------------- MODEL BASE --------------------------------------
// This is a common base for the locomotive and wagons
// The base is of rectangular shape (20 length x 10 width x 2 height)
// and has wheels and connectors attached.
//-----------------------------------------------------------------
void base(float theta)
{
    glColor4f(0.2, 0.2, 0.2, 1.0);   //Base color
    glPushMatrix();
      glTranslatef(0.0, 4.0, 0.0);
      glScalef(20.0, 2.0, 10.0);     //Size 20x10 units, thickness 2 units.
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();					//Connector between wagons
      glTranslatef(11.0, 4.0, 0.0);
      glutSolidCube(2.0);
    glPopMatrix();

    //4 Wheels (radius = 2 units)
	//x, z positions of wheels:
	float wx[4] = {  -8,   8,   -8,    8 }; 
	float wz[4] = { 5.1, 5.1, -5.1, -5.1 };
    glColor4f(0.5, 0.5, 0.5, 1.0);    //Wheel color
	
	glEnable(GL_TEXTURE_2D);

	for (int i = 0; i < 4; i++)
	{	
		glBindTexture(GL_TEXTURE_2D, txId[0]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glPushMatrix();
			glTranslatef(wx[i], 2.0, wz[i]);
			glRotatef(theta * 10, 0.0, 0.0, 1.0);
			gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
}


void road()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[7]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPushMatrix();
		glColor4f(1, 1, 1, 1.0);
		
		glTranslatef(0.0, 0.11, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluDisk(q, 35.0, 60, 80, 3);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void car()
{
	glColor4f(0.8, 0.8, 0.0, 1.0);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[10]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glBegin(GL_QUADS);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		float x = 0;
		float z = 0;
		
		float width = 10;  
		float depth = 5; // length
		
		float xs[] = {x - width, x - width, x + width, x + width};
		float ys[] = {1.0,      6.0,     6.0,     1.0};
		float zs[] = {z - depth, z - depth, z - depth, z - depth};
		
		glNormal3f(0.0, 1.0, 0.0);  // Top
		glTexCoord2f(0.9, 0.9); glVertex3f(xs[0], ys[1], zs[0] + 2 * depth);
		glTexCoord2f(0.9, 0.9); glVertex3f(xs[2], ys[1], zs[0] + 2 * depth);
		glTexCoord2f(0.9, 0.9); glVertex3f(xs[2], ys[1], zs[0]);
		glTexCoord2f(0.9, 0.9); glVertex3f(xs[0], ys[1], zs[0]);
		
		glNormal3f(-1.0, 0.0, 0.0);  // Back
		glTexCoord2f(568.0 / 1024.0, 450.0 / 1024.0); glVertex3f(xs[0], ys[0], zs[0]);
		glTexCoord2f(568.0 / 1024.0, 620.0 / 1024.0); glVertex3f(xs[1], ys[1], zs[1]);
		glTexCoord2f(979.0 / 1024.0, 620.0 / 1024.0); glVertex3f(xs[1], ys[1], zs[1] + 2 * depth);
		glTexCoord2f(979.0 / 1024.0, 450.0 / 1024.0); glVertex3f(xs[0], ys[0], zs[0] + 2 * depth);
		
		glNormal3f(1.0, 0.0, 0.0);  // Front
		glTexCoord2f(92.0 / 1024.0, 670.0 / 1024.0); glVertex3f(xs[2], ys[0], zs[0]);
		glTexCoord2f(92.0 / 1024.0, 820.0 / 1024.0); glVertex3f(xs[3], ys[1], zs[1]);
		glTexCoord2f(500.0 / 1024.0, 820.0 / 1024.0); glVertex3f(xs[3], ys[1], zs[1] + 2 * depth);
		glTexCoord2f(500.0 / 1024.0, 670.0 / 1024.0); glVertex3f(xs[2], ys[0], zs[0] + 2 * depth);
		
		glNormal3f(0.0, 0.0, -1.0);   // Side 1
		glTexCoord2f(292.0 / 1024.0, 0.055); glVertex3f(xs[0], ys[0], zs[0]);
		glTexCoord2f(292.0 / 1024.0, 0.23); glVertex3f(xs[1], ys[1], zs[1]);
		glTexCoord2f(702.0 / 1024.0, 0.23); glVertex3f(xs[2], ys[2], zs[2]);
		glTexCoord2f(702.0 / 1024.0, 0.055); glVertex3f(xs[3], ys[3], zs[3]);
		
		glNormal3f(0.0, 0.0, 1.0);   // Side 2
		glTexCoord2f(292.0 / 1024.0, 0.055); glVertex3f(xs[0], ys[0], zs[0] + 2 * depth);
		glTexCoord2f(292.0 / 1024.0, 0.23); glVertex3f(xs[1], ys[1], zs[1] + 2 * depth);
		glTexCoord2f(702.0 / 1024.0, 0.23); glVertex3f(xs[2], ys[2], zs[2] + 2 * depth);
		glTexCoord2f(702.0 / 1024.0, 0.055); glVertex3f(xs[3], ys[3], zs[3] + 2 * depth);
	glEnd();
	
	glBegin(GL_QUADS);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		x = 0;
		z = 0;
		
		width = 10;  
		depth = 5; // length
		
		float xs1[] = {x - width, x - width, x, x};
		float ys1[] = {6.0,       10.0,     10.0,       6.0};
		float zs1[] = {z - depth, z - depth, z - depth, z - depth};
		
		glNormal3f(0.0, 1.0, 0.0);  // Top
		glTexCoord2f(0.9, 0.9); glVertex3f(xs1[0], ys1[1], zs1[0] + 2 * depth);
		glTexCoord2f(0.9, 0.9); glVertex3f(xs1[2], ys1[1], zs1[0] + 2 * depth);
		glTexCoord2f(0.9, 0.9); glVertex3f(xs1[2], ys1[1], zs1[0]);
		glTexCoord2f(0.9, 0.9); glVertex3f(xs1[0], ys1[1], zs1[0]);
		
		glNormal3f(-1.0, 0.0, 0.0);  // Back
		glTexCoord2f(72.0 / 1024.0, 500.0 / 1024.0); glVertex3f(xs1[0], ys1[0], zs1[0]);
		glTexCoord2f(72.0 / 1024.0, 590.0 / 1024.0); glVertex3f(xs1[1], ys1[1], zs1[1]);
		glTexCoord2f(348.0 / 1024.0, 590.0 / 1024.0); glVertex3f(xs1[1], ys1[1], zs1[1] + 2 * depth);
		glTexCoord2f(348.0 / 1024.0, 500.0 / 1024.0); glVertex3f(xs1[0], ys1[0], zs1[0] + 2 * depth);
		
		glNormal3f(1.0, 0.0, 0.0);  // Front
		glTexCoord2f(618.0 / 1024.0, 0.25); glVertex3f(xs1[2], ys1[0], zs1[0]);
		glTexCoord2f(618.0 / 1024.0, 0.32); glVertex3f(xs1[3], ys1[1], zs1[1]);
		glTexCoord2f(778.0 / 1024.0, 0.32); glVertex3f(xs1[3], ys1[1], zs1[1] + 2 * depth);
		glTexCoord2f(778.0 / 1024.0, 0.25); glVertex3f(xs1[2], ys1[0], zs1[0] + 2 * depth);
		
		glNormal3f(0.0, 0.0, -1.0);   // Side 1
		glTexCoord2f(618.0 / 1024.0, 0.25); glVertex3f(xs1[0], ys1[0], zs1[0]);
		glTexCoord2f(618.0 / 1024.0, 0.32); glVertex3f(xs1[1], ys1[1], zs1[1]);
		glTexCoord2f(778.0 / 1024.0, 0.32); glVertex3f(xs1[2], ys1[2], zs1[2]);
		glTexCoord2f(778.0 / 1024.0, 0.25); glVertex3f(xs1[3], ys1[3], zs1[3]);
		
		glNormal3f(0.0, 0.0, 1.0);   // Side 2
		glTexCoord2f(618.0 / 1024.0, 0.25); glVertex3f(xs1[0], ys1[0], zs1[0] + 2 * depth);
		glTexCoord2f(618.0 / 1024.0, 0.32); glVertex3f(xs1[1], ys1[1], zs1[1] + 2 * depth);
		glTexCoord2f(778.0 / 1024.0, 0.32); glVertex3f(xs1[2], ys1[2], zs1[2] + 2 * depth);
		glTexCoord2f(778.0 / 1024.0, 0.25); glVertex3f(xs1[3], ys1[3], zs1[3] + 2 * depth);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	
	float wx[4] = {  -8,   8,   -8,    8 }; 
	float wz[4] = { 5.1, 5.1, -5.1, -5.1 };
    glColor4f(0.2, 0.2, 0.2, 1.0);    //Wheel color
	GLUquadric *q1 = gluNewQuadric();   //Disc

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glTranslatef(wx[i], 2.0, wz[i]);
		gluDisk(q1, 0.0, 2.0, 20, 2);
		glPopMatrix();
	}
}

//--------------- LOCOMOTIVE --------------------------------
// This simple model of a locomotive consists of the base,
// cabin and boiler
//-----------------------------------------------------------
void engine(float theta)
{
    base(theta);

    //Cab
    glColor4f(0.8, 0.8, 0.0, 1.0);
    glPushMatrix();
      glTranslatef(7.0, 8.5, 0.0);
      glScalef(6.0, 7.0, 10.0);
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(6.0, 14.0, 0.0);
      glScalef(4.0, 4.0, 8.0);
      glutSolidCube(1.0);
    glPopMatrix();

    //Boiler
	GLUquadric *q1 = gluNewQuadric();
    glPushMatrix();
      glColor4f(0.15, 0.15, 0.15, 1.0); 
      
      glPushMatrix();  // Chimmley
		glTranslatef(-7.0, 15.0, 0.0);
		glPushMatrix();
			glRotatef(-90.0, 1., 0., 0.);
			gluCylinder(q1, 1.0, 1.0, 3.0, 20, 5);
		glPopMatrix();
	  
		glTranslatef(0.0, 3.0, 0.0);
		glPushMatrix();
			glRotatef(-90.0, 1., 0., 0.);
			gluCylinder(q1, 1.0, 3.0, 3.0, 20, 5);
		glPopMatrix();
		
		glTranslatef(0.0, 3.0, 0.0);
		glPushMatrix();
			glRotatef(-90.0, 1., 0., 0.);
			gluDisk(q1, 0.0, 3.0, 20,2);
		glPopMatrix();
	  glPopMatrix();
	
	  glColor4f(0.8, 0.0, 0.0, 1.0);
	  glBegin(GL_TRIANGLES);   // Front guard
		normal(-15.0, 0.5, 0.0, -10.0, 5.0, 0.0, -10.0, 5.0, -5.0);
		glVertex3f(-10.0, 5.0, 0.0);
		glVertex3f(-15.0, 0.5, 0.0);
		glVertex3f(-10.0, 5.0, -5.0);
		normal(-10.0, 0.5, -5.0, -15.0, 0.5, 0.0, -10.0, 5.0, -5.0);
		glVertex3f(-10.0, 0.5, -5.0);
		glVertex3f(-15.0, 0.5, 0.0);
		glVertex3f(-10.0, 5.0, -5.0);
		
		normal(-10.0, 5.0, 0.0, -15.0, 0.5, 0.0,-10.0, 5.0, 5.0);
		glVertex3f(-10.0, 5.0, 0.0);
		glVertex3f(-15.0, 0.5, 0.0);
		glVertex3f(-10.0, 5.0, 5.0);
		normal(-15.0, 0.5, 0.0, -10.0, 0.5, 5.0, -10.0, 5.0, 5.0);
		glVertex3f(-10.0, 0.5, 5.0);
		glVertex3f(-15.0, 0.5, 0.0);
		glVertex3f(-10.0, 5.0, 5.0);
		
	  glEnd();
    
	  glEnable(GL_TEXTURE_2D);
	  glBindTexture(GL_TEXTURE_2D, txId[3]);   // Main body
	  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	  
      glColor4f(1.0, 1., 1., 1.0);
      glTranslatef(4.0, 10.0, 0.0);
      glRotatef(-90.0, 0., 1., 0.);
      gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
      
      glBindTexture(GL_TEXTURE_2D, txId[2]);   // Front
	  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTranslatef(0.0, 0.0, 14.0);
      gluDisk(q, 0.0, 5.0, 20, 3);
      glDisable(GL_TEXTURE_2D);
      
      glColor4f(1.0, 1.0, 0.0, 1.0);
      glTranslatef(0.0, 4.0, 0.2);
      gluDisk(q1, 0.0, 1.0, 20,2);  //headlight!
    glPopMatrix();

}

//--------------- WAGON ----------------------------------
// This simple model of a rail wagon consists of the base,
// and a cube(!)
//--------------------------------------------------------
void wood(float length, float width)
{
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[5]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glColor4f(1.0, 1., 1., 1.0);
	glRotatef(-90.0, 0., 1., 0.);
	gluCylinder(q, width, width, length, 20, 5);

	glBindTexture(GL_TEXTURE_2D, txId[4]);   // Front
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTranslatef(0.0, 0.0, length);
	gluDisk(q, 0.0, width, 20, 3);
	
	glTranslatef(0.0, 0.0, -length);
	gluDisk(q, 0.0, width, 20, 3);
	glDisable(GL_TEXTURE_2D);
}


void woodWagon(float theta)
{
	base(theta);
    
    glTranslatef(8.0, 7.0, 0.0);
    
    glPushMatrix();
		glTranslatef(0.0, 0.0, -2.0);
		wood(16.0, 2.0);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(0.0, 0.0, 2.0);
		wood(16.0, 2.0);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(0.0, 3.0, 0.0);
		wood(16.0, 2.0);
	glPopMatrix();
}


void wagon(float theta)
{
    base(theta);
    
    glTranslatef(0.0, 10.0, 0.0);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[1]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
    glBegin(GL_QUADS);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		glNormal3f(0.0, 1.0, 0.0);  // Top
		glTexCoord2f(325.0 / 512.0, 256.0 / 512.0); glVertex3f(9.0, 5.0, 5.0);
		glTexCoord2f(218.0 / 512.0, 256.0 / 512.0); glVertex3f(-9.0, 5.0, 5.0);
		glTexCoord2f(218.0 / 512.0, 128.0 / 512.0); glVertex3f(-9.0, 5.0, -5.0);
		glTexCoord2f(325.0 / 512.0, 128.0 / 512.0); glVertex3f(9.0, 5.0, -5.0);
		
		glNormal3f(1.0, 0.0, 0.0);  // Back
		glTexCoord2f(325.0 / 512.0, 128.0 / 512.0); glVertex3f(9.0, 5.0, 5.0);
		glTexCoord2f(325.0 / 512.0, 0.0 / 512.0); glVertex3f(9.0, -5.0, 5.0);
		glTexCoord2f(218.0 / 512.0, 0.0 / 512.0); glVertex3f(9.0, -5.0, -5.0);
		glTexCoord2f(218.0 / 512.0, 128.0 / 512.0); glVertex3f(9.0, 5.0, -5.0);
		
		glNormal3f(1.0, 0.0, 0.0);  // Front
		glTexCoord2f(218.0 / 512.0, 256.0 / 512.0); glVertex3f(-9.0, 5.0, 5.0);
		glTexCoord2f(218.0 / 512.0, 128.0 / 512.0); glVertex3f(-9.0, -5.0, 5.0);
		glTexCoord2f(275.0 / 512.0, 128.0 / 512.0); glVertex3f(-9.0, -5.0, -5.0);
		glTexCoord2f(275.0 / 512.0, 256.0 / 512.0); glVertex3f(-9.0, 5.0, -5.0);
		
		glNormal3f(0.0, 0.0, 1.0);   // Outside
		glTexCoord2f(218.0 / 512.0, 256.0 / 512.0); glVertex3f(9.0, 5.0, 5.0);
		glTexCoord2f(218.0 / 512.0, 128.0 / 512.0); glVertex3f(9.0, -5.0, 5.0);
		glTexCoord2f(325.0 / 512.0, 128.0 / 512.0); glVertex3f(-9.0, -5.0, 5.0);
		glTexCoord2f(325.0 / 512.0, 256.0 / 512.0); glVertex3f(-9.0, 5.0, 5.0);
		
		glNormal3f(0.0, 0.0, -1.0);  // Inside
		glTexCoord2f(218.0 / 512.0, 256.0 / 512.0); glVertex3f(9.0, 5.0, -5.0);
		glTexCoord2f(218.0 / 512.0, 128.0 / 512.0); glVertex3f(9.0, -5.0, -5.0);
		glTexCoord2f(325.0 / 512.0, 128.0 / 512.0); glVertex3f(-9.0, -5.0, -5.0);
		glTexCoord2f(325.0 / 512.0, 256.0 / 512.0); glVertex3f(-9.0, 5.0, -5.0);
    glEnd();
	glDisable(GL_TEXTURE_2D);
}


void barrierArm(float angle)
{
	glColor4f(0.5, 0.5, 0.5, 1.0);
    glPushMatrix();    // Base of barrier arm
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glutSolidCylinder(1.0, 10.0, 10, 10);
	glPopMatrix();
		
	glPushMatrix();
		glTranslatef(0.0, 10.0, 0.0);
		glutSolidSphere(1.0, 10, 10);
    glPopMatrix();
    
    glPushMatrix();    // Barrier arm
		glTranslatef(0.0, 10.0, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);
		
		glPushMatrix();
			glTranslatef(20.0, 0.0, 0.0);
			glColor3f(0.5, 0.0, 0.0);
			glRotatef(90.0, 0.0, 1.0, 0.0);
			gluDisk(q, 0.0, 0.6, 20, 3);
		glPopMatrix();
		
		glColor4f(0.5, 0.5, 0.5, 1.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, txId[8]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluCylinder(q, 0.6, 0.6, 20.0, 10, 10);
		glDisable(GL_TEXTURE_2D);
		//glutSolidCylinder(0.6, 20.0, 10, 10);
    glPopMatrix();
}


void trafficLight(float colour)
{
	glColor4f(0.5, 0.5, 0.5, 1.0);
    glPushMatrix();    // Stem of traffic light
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glutSolidCylinder(1.0, 20.0, 10, 10);
    glPopMatrix();
    
    glColor4f(0.6, 0.6, 0.6, 1.0);
    glPushMatrix();   // Head of the traffic light
		glTranslatef(0.5, 20.0, 0.0);
		glScalef(1.0, 2.0, 1.0);
		glutSolidCube(3.0);
    glPopMatrix();
    
    GLUquadric *q1 = gluNewQuadric();
    
    glPushMatrix();    // Red light
		if (colour == 2.0) {
			glColor4f(1.0, 0.0, 0.0, 1.0);
		} else glColor4f(0.5, 0.0, 0.0, 1.0);
		
		glTranslatef(2.5, 22.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		gluDisk(q1, 0.0, 0.8, 20, 3);
	glPopMatrix();
	
	glPushMatrix();    // Orange light
		if (colour == 1.0) {
			glColor4f(1.0, 215.0 / 256.0, 0.0, 1.0);
		} else glColor4f(0.3, 0.25, 0.0, 1.0);
		
		glTranslatef(2.5, 20.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		gluDisk(q1, 0.0, 0.8, 20, 3);
	glPopMatrix();
	
	glPushMatrix();    // Green light
		if (colour == 0.0) {
			glColor4f(0.0, 1.0, 0.0, 1.0);
		} else glColor4f(0.0, 0.3, 0.0, 1.0);
		
		glTranslatef(2.5, 18.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		gluDisk(q1, 0.0, 0.8, 20, 3);
	glPopMatrix();
}


void tunnel()
{
	float medRadius = 30.0;
	float width = 5.0;
	float length = 50.0;
	float tunnelThickness = 2.5;
	
	float inRad  = medRadius - width * 0.5;

	float angle1,angle2, ca1,sa1, ca2,sa2;
	float x1,y1, x2,y2, x3,y3, x4,y4;  //four points of a quad

	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[6]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
    glBegin(GL_QUADS);
	float radius = inRad;
	for (int i = -90; i < 90; i += 5)    //  5 deg intervals
	{
		angle1 = i * toRad;       //  Computation of angles, cos, sin etc
		angle2 = (i + 5) * toRad;
		ca1 = cos(angle1); ca2 = cos(angle2);
		sa1 = sin(angle1); sa2 = sin(angle2);
		x1 = (radius - tunnelThickness)*sa1; y1 = (radius - tunnelThickness)*ca1;
		x2 = (radius + tunnelThickness)*sa1; y2 = (radius + tunnelThickness)*ca1;
		x3 = (radius + tunnelThickness)*sa2; y3 = (radius + tunnelThickness)*ca2;
		x4 = (radius - tunnelThickness)*sa2; y4 = (radius - tunnelThickness)*ca2;
		
		glNormal3f(0., 0., 1.);       //  Quad 1 end of the tunnel
		glTexCoord2f(0.0, 0.5); glVertex3f(x1, y1, -1 * length / 2.0);
		glTexCoord2f(0.16, 0.5); glVertex3f(x2, y2, -1 * length / 2.0);
		glTexCoord2f(0.16, 0.6); glVertex3f(x3, y3, -1 * length / 2.0);
		glTexCoord2f(0.0, 0.6); glVertex3f(x4, y4, -1 * length / 2.0);
		
		glNormal3f(-sa1, -ca1, 0.0);   //  Quad 2 facing inward
		glTexCoord2f(0.0, 0.0); glVertex3f(x1, y1, -1 * length / 2.0);
		glTexCoord2f(1.8, 0.0); glVertex3f(x1, y1, length / 2.0);
		glNormal3f(-sa2, -ca2, 0.0);
		glTexCoord2f(1.8, 0.1); glVertex3f(x4, y4, length / 2.0);
		glTexCoord2f(0.0, 0.1); glVertex3f(x4, y4, -1 * length / 2.0);
		
		glNormal3f(sa1, ca1, 0.0);   //  Quad 3 facing outward
		glTexCoord2f(0.0, 0.0); glVertex3f(x2, y2, length / 2.0);
		glTexCoord2f(1.8, 0.0); glVertex3f(x2, y2, -1 * length / 2.0);
		glNormal3f(sa2, ca2, 0.0);
		glTexCoord2f(1.8, 0.1); glVertex3f(x3, y3, -1 * length / 2.0);
		glTexCoord2f(0.0, 0.1); glVertex3f(x3, y3, length / 2.0);
		
		glNormal3f(0., 0., -1.);       //  Quad 4 other end of the tunnel
		glTexCoord2f(0.0, 0.5); glVertex3f(x1, y1, length / 2.0);
		glTexCoord2f(0.16, 0.5); glVertex3f(x2, y2, length / 2.0);
		glTexCoord2f(0.16, 0.6); glVertex3f(x3, y3, length / 2.0);
		glTexCoord2f(0.0, 0.6); glVertex3f(x4, y4, length / 2.0);
	}
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}


void station(float x, float z)
{
	glColor4f(0.6445, 0.164, 0.164, 1.0);
    glPushMatrix();   // Base
      glTranslatef(x, 2.5, z);
      glScalef(60.0, 5.0, 30.0);
      glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();   // Back wall
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, txId[6]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			
		glBegin(GL_QUADS);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
			float xs[] = {x - 35, x - 35, x + 35, x + 35};
			float ys[] = {0.0,      40.0,     40.0,     0.0};
			float zs[] = {z - 12.5f, z - 12.5f, z - 12.5f, z - 12.5f};
			
			glNormal3f(-1.0, 0.0, 0.0);  // Side 1
			glTexCoord2f(0.0, 0.0); glVertex3f(xs[0], ys[0], zs[0]);
			glTexCoord2f(0.0, 1.0); glVertex3f(xs[1], ys[1], zs[1]);
			glTexCoord2f(0.3, 1.0); glVertex3f(xs[1], ys[1], zs[1] - 5.0f);
			glTexCoord2f(0.3, 0.0); glVertex3f(xs[0], ys[0], zs[0] - 5.0f);
			
			glNormal3f(1.0, 0.0, 0.0);  // Side 1
			glTexCoord2f(0.0, 0.0); glVertex3f(xs[2], ys[0], zs[0]);
			glTexCoord2f(0.0, 1.0); glVertex3f(xs[3], ys[1], zs[1]);
			glTexCoord2f(0.3, 1.0); glVertex3f(xs[3], ys[1], zs[1] - 5.0f);
			glTexCoord2f(0.3, 0.0); glVertex3f(xs[2], ys[0], zs[0] - 5.0f);
			
			glNormal3f(0.0, 0.0, 1.0);   // Inside
			glTexCoord2f(0.0, 0.0); glVertex3f(xs[0], ys[0], zs[0]);
			glTexCoord2f(0.0, 1.0); glVertex3f(xs[1], ys[1], zs[1]);
			glTexCoord2f(3.0, 1.0); glVertex3f(xs[2], ys[2], zs[2]);
			glTexCoord2f(3.0, 0.0); glVertex3f(xs[3], ys[3], zs[3]);
			
			glNormal3f(0.0, 0.0, -1.0);   // Outside
			glTexCoord2f(0.0, 0.0); glVertex3f(xs[0], ys[0], zs[0] - 5.0f);
			glTexCoord2f(0.0, 1.0); glVertex3f(xs[1], ys[1], zs[1] - 5.0f);
			glTexCoord2f(3.0, 1.0); glVertex3f(xs[2], ys[2], zs[2] - 5.0f);
			glTexCoord2f(3.0, 0.0); glVertex3f(xs[3], ys[3], zs[3] - 5.0f);

		glEnd();
		glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
    glColor4f(0.5, 0.5, 0.5, 1.0);
    glPushMatrix();    // Supports
		glTranslatef(0.0, 42.0, z + 15.0);
		
		for (int i = -1; i <= 1; i += 2) {
			glPushMatrix();
				glTranslatef(x + i * 33.0, 0.0, 0.0);
				glRotatef(90.0, 1.0, 0.0, 0.0);
				gluCylinder(q, 2.0, 2.0, 43.0, 20, 5);
			glPopMatrix();
		}
    glPopMatrix();
    
    glBegin(GL_TRIANGLES);
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(x - 35.0, 40, z + 22.0);
		glVertex3f(x - 35.0, 55, z);
		glVertex3f(x - 35.0, 40, z - 22.0);
		
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(x + 35.0, 40, z + 22.0);
		glVertex3f(x + 35.0, 55, z);
		glVertex3f(x + 35.0, 40, z - 22.0);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[9]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBegin(GL_QUADS);
		glColor3f(1.0, 1.0, 1.0);
		glNormal3f(0.0, 1.0, 0.5);  // Front
		glTexCoord2f(0.0, 0.0); glVertex3f(x - 35.0, 40, z + 22.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(x - 35.0, 55, z);
		glTexCoord2f(3.0, 1.0); glVertex3f(x + 35.0, 55, z);
		glTexCoord2f(3.0, 0.0); glVertex3f(x + 35.0, 40, z + 22.0);		
		
		glNormal3f(0.0, 1.0, -0.5);   // Back
		glTexCoord2f(0.0, 0.0); glVertex3f(x - 35.0, 40, z - 22.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(x - 35.0, 55, z);
		glTexCoord2f(3.0, 1.0); glVertex3f(x + 35.0, 55, z);
		glTexCoord2f(3.0, 0.0); glVertex3f(x + 35.0, 40, z - 22.0);
		
		glColor3f(0.5, 0.5, 0.5);
		glNormal3f(0.0, -1.0, 0.0);   // Bottom
		glVertex3f(x - 35.0, 40, z - 22.0);
		glVertex3f(x - 35.0, 40, z + 22.0);
		glVertex3f(x + 35.0, 40, z + 22.0);
		glVertex3f(x + 35.0, 40, z - 22.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void smokeFace(float t)
{
	float T = 30;
	glBegin(GL_TRIANGLE_FAN);
		int num_points = 13;
		
		glColor4f(0.3 + t / (2 * T), 0.3 + t / (2.0 * T), 0.3 + t / (2.0 * T), 1 - t / T);
		glVertex3f(0.0, 0.0, 0.0);
		
		glColor4f(1, 1, 1, 0);		
		for (int i = 0; i < num_points; i++) glVertex3f(sin(i * 30 * toRad), cos(i * 30 * toRad), 0.0);
	glEnd();
}


void smokeParticle(float t)
{
	int num_faces = 3;

	for (int i = 0; i < num_faces; i++) {
		glPushMatrix();
			glRotatef(i * 60, 0.0, 1.0, 0.0);
			smokeFace(t);
		glPopMatrix();
	}
}
