//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//  Assignment 1: Gareth Harcombe
//  FILE NAME: RailwayWorld.cpp
//  
//  ========================================================================

#include <math.h>
#include <GL/freeglut.h>
#include "RailModels.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;
using std::ifstream;

// g++ RailModels.cpp RailwayWorld.cpp -o program  -lm -lGL -lGLU -lglut

int cam_hgt = 50;
int theta = 0;
float cam_angl = 0;
float cam_dist = 250;

int cam_mode = 0;

float car_t = 0;

int points;
float* x_points;
float* z_points;

int smokeCounter = 0;

struct Point 
{
	float x, y, z;
};

struct particle
{
	int t;
	float pos[3];
	float dir[3];
	float speed;
	float size;
	float delta;
};

int cmodulus(int a, int b)
{
	return (b + (a % b)) % b;
}

list<particle> parList;

void newParticle()
{
	particle p = {0};
	p.pos[0] = x_points[cmodulus((theta + 10), points)];
	p.pos[1] = 23.0;
	p.pos[2] = z_points[cmodulus((theta + 10), points)]; 
	p.dir[0] = (float)(rand() % 30) / 60.0 - 0.25;
	p.dir[1] = 1.0;
	p.dir[2] = (float)(rand() % 30) / 60.0 - 0.25;
	p.speed = 1;
	p.size = 2.5;
	p.delta = 1;
	parList.push_back(p);
}


//---------------------------------------------------------------------
void initialize(void) 
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};
    float red[4] = {1.0, 0.0, 0.0, 1.0};

	loadTexture();

	glEnable(GL_DEPTH_TEST);   	
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);	
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	
	glClearColor (0.0, 0.0, 0.0, 0.0);  //Background colour
	glClearDepth(1.0f);  
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);

//	Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    
    glLightfv(GL_LIGHT2, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT2, GL_SPECULAR, red);
    
    glLightfv(GL_LIGHT3, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT3, GL_SPECULAR, red);
    
    glLightfv(GL_LIGHT4, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT4, GL_SPECULAR, red);
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	gluQuadricDrawStyle (q, GLU_FILL );
	gluQuadricNormals	(q, GLU_SMOOTH );

	//glEnable(GL_TEXTURE_2D);
	gluQuadricTexture (q, GL_TRUE);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60., 1.0, 11.0, 1000.0);   //Perspective projection
}


//-------------------------------------------------------------------
void display(void)
{
	float red[4] = {1.0, 0.0, 0.0, 1.0};
	float orange[4] = {1.0, 215.0 / 256.0, 0.0, 1.0};
	float green[4] = {0.0, 1.0, 0.0, 1.0};
	
	float lgt_pos[] = {0.0f, 70.0f, 0.0f, 1.0f};  //light0 position (directly above the origin)
	float spot_pos[] = {-10.0, 14.0, 0.0, 1.0};
	float spot_dir[] = {-1.0, -1.0, 0.0, 1.0};
	float back_light_pos[] = {8.0, 5, 0.0, 1.0};
	float back_light_dir[] = {1, -1.2, 0.0, 1.0};
	float traffic_lgt_pos[] = {5.0f, 22.0f, 0.0f, 1.0f};
	float* traffic_lgt_colour = red;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(cam_mode == 0) {   // Standard view
		gluLookAt(cam_dist * sin(cam_angl * M_PI / 180), cam_hgt, cam_dist * cos(cam_angl * M_PI / 180), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	} else if(cam_mode == 1) {    // Cab view
		gluLookAt(x_points[cmodulus((theta), points)], 10.0, z_points[cmodulus((theta), points)], x_points[cmodulus((theta + 10), points)], 10, z_points[cmodulus((theta + 10), points)], 0.0, 1.0, 0.0);
	} else if(cam_mode == 2) {    // Station view
		gluLookAt(100, 10, 0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	
	glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position
	
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5);
	
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 20.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 30);
	
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.01);
	glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, 0.01);
	
	floor();

	glPushMatrix();
		glTranslatef(100.0, 0.0, 20.0);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		station(0, 0);
	glPopMatrix();
	
	glPushMatrix();
		float center = 870;
		float stage_1 = center - 102;
		float stage_2 = center - 67;
		float stage_3 = center + 130;
		float stage_4 = center + 165;
	
		glTranslatef(-135.0, 0.0, -120.0);
		glRotatef(-90.0, 0.0, 1.0, 0.0);

		if (theta > stage_1 and theta < stage_2) barrierArm((stage_2 - theta) * 2);   // Bar lowering
		else if (theta >= stage_2 and theta <= stage_3) barrierArm(0.0);   // Bar down
		else if (theta > stage_3 and theta < stage_4) barrierArm((theta - (stage_3)) * 2);  // Bar rising
		else barrierArm(70.0);  // Bar up
		
		glTranslatef(-5.0, 0.0, 0);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		
		if (theta < stage_1 or theta > stage_4) {
			traffic_lgt_pos[1] = 18.0;
			glLightfv(GL_LIGHT3, GL_DIFFUSE, green);
			glLightfv(GL_LIGHT3, GL_SPECULAR, green);
			trafficLight(0.0);
		} else if (theta < stage_2) {
			traffic_lgt_pos[1] = 20.0;
			glLightfv(GL_LIGHT3, GL_DIFFUSE, orange);
			glLightfv(GL_LIGHT3, GL_SPECULAR, orange);
			trafficLight(1.0);
		} else {
			traffic_lgt_pos[1] = 22.0;
			glLightfv(GL_LIGHT3, GL_DIFFUSE, red);
			glLightfv(GL_LIGHT3, GL_SPECULAR, red);
			trafficLight(2.0);
		}
		
		glLightfv(GL_LIGHT3, GL_POSITION, traffic_lgt_pos);
	glPopMatrix();
	
	glPushMatrix();
		center = 750;
		stage_1 = center - 102;
		stage_2 = center - 67;
		stage_3 = center + 130;
		stage_4 = center + 165;
	
		glTranslatef(-60.0, 0.0, -80.0);
		glRotatef(180.0, 0.0, 1.0, 0.0);

		if (theta > stage_1 and theta < stage_2) barrierArm((stage_2 - theta) * 2);   // Bar lowering
		else if (theta >= stage_2 and theta <= stage_3) barrierArm(0.0);   // Bar down
		else if (theta > stage_3 and theta < stage_4) barrierArm((theta - (stage_3)) * 2);  // Bar rising
		else barrierArm(70.0);  // Bar up
		
		glTranslatef(-5.0, 0.0, 0);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		
		if (theta < stage_1 or theta > stage_4) {
			traffic_lgt_pos[1] = 18.0;
			glLightfv(GL_LIGHT4, GL_DIFFUSE, green);
			glLightfv(GL_LIGHT4, GL_SPECULAR, green);
			trafficLight(0.0);
		} else if (theta < stage_2) {
			traffic_lgt_pos[1] = 20.0;
			glLightfv(GL_LIGHT4, GL_DIFFUSE, orange);
			glLightfv(GL_LIGHT4, GL_SPECULAR, orange);
			trafficLight(1.0);
		} else {
			traffic_lgt_pos[1] = 22.0;
			glLightfv(GL_LIGHT4, GL_DIFFUSE, red);
			glLightfv(GL_LIGHT4, GL_SPECULAR, red);
			trafficLight(2.0);
		}
		
		glLightfv(GL_LIGHT4, GL_POSITION, traffic_lgt_pos);
	glPopMatrix();
		
	tracks(120, 7, x_points, z_points, points);  //mean radius 120 units, width 10 units, full 360 degree angle
	
	glPushMatrix();
		glTranslatef(-120.0, 0.0, -60.0);
		road();
		glPushMatrix();
			glRotatef(-car_t, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -48.0);
			car();
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(-40.0, 0.0, 150.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		tunnel();
	glPopMatrix();
   
	glPushMatrix();
		glRotatef(0, 0.0, 1.0, 0.);
		
		int k = 5;
		Point p1 = initPoint(x_points[cmodulus((theta - k), points)], 0, z_points[cmodulus((theta - k), points)]);
		Point p3 = initPoint(x_points[cmodulus((theta + k), points)], 0, z_points[cmodulus((theta + k), points)]);
		float len = sqrt((p3.x - p1.x) * (p3.x - p1.x) + (p3.y - p1.y) * (p3.y - p1.y) + (p3.z - p1.z) * (p3.z - p1.z));
		Point u = initPoint((p3.x - p1.x) / len, (p3.y - p1.y) / len, (p3.z - p1.z) / len);
		
		glPushMatrix();
			
			//list<particle>::iterator it;
			//particle p;
			
			////Update particles
			
			//for (it = parList.begin(); it != parList.end(); it++) {
				//glPushMatrix();
					//glTranslatef(it->pos[0], it->pos[1], it->pos[2]);
					//glTranslatef(-7.0, 0.0, 0.0);
					//glScalef(it->size, it->size, it->size);
					//smokeParticle();
				//glPopMatrix();
			//}
				//it->size = ...;
				//it->delta = ...;
			//for (int i = 0; i < 3; i++) {  // Smoke
				//if (!(smoke[i].z == 150.0 && smoke[i].x < 40 && smoke[i].x > -60 && smoke[i].y > 25.0)) {
					//glPushMatrix();
						//glColor4f(1.0, 1.0, 1.0, 1.0);
						//glTranslatef(smoke[i].x, smoke[i].y, smoke[i].z);
						//glRotatef(atan2(u.z, -u.x) * 180.0 / 3.14159265, 0.0, 1.0, 0.0);
						//glTranslatef(-7.0, 0.0, 0.0);
						//glutSolidSphere(3.0, 10, 10);
					//glPopMatrix();
				//}
			//}
		
			glTranslatef(x_points[theta], 1.0, z_points[theta]);
			glRotatef(atan2(u.z, -u.x) * 180.0 / 3.14159265, 0.0, 1.0, 0.0);
			engine(theta);		 //locomotive
		 
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
			glLightfv(GL_LIGHT1, GL_POSITION, spot_pos);
		glPopMatrix();
	   
		int l = 28;
		
		for (int i = 1; i <= 4; i++) {
			glPushMatrix();
				glTranslatef(x_points[cmodulus((theta - l * i), points)], 1.0, z_points[cmodulus((theta - l * i), points)]);
			
				int k = 5;
				Point p1 = initPoint(x_points[cmodulus((theta - k - l * i), points)], 0, z_points[cmodulus((theta - k - l * i), points)]);
				Point p3 = initPoint(x_points[cmodulus((theta + k - l * i), points)], 0, z_points[cmodulus((theta + k - l * i), points)]);
				float len = sqrt((p3.x - p1.x) * (p3.x - p1.x) + (p3.y - p1.y) * (p3.y - p1.y) + (p3.z - p1.z) * (p3.z - p1.z));
				Point u = initPoint((p3.x - p1.x) / len, (p3.y - p1.y) / len, (p3.z - p1.z) / len);
			
				glRotatef(atan2(u.z, -u.x) * 180.0 / 3.14159265, 0.0, 1.0, 0.0);
				
				if (i % 2 == 0) wagon(theta);
				else woodWagon(theta);
				
				if (i == 4) {
					glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, back_light_dir);
					glLightfv(GL_LIGHT2, GL_POSITION, back_light_pos);
				}
			glPopMatrix();
		}
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		list<particle>::reverse_iterator it;
		particle p;
		
		//Update particles
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);
		for (it = parList.rbegin(); it != parList.rend(); it++) {
			if (!(it->pos[2] > 140.0 && it->pos[2] < 160.0 && it->pos[0] < -10 && it->pos[0] > -60 && it->pos[1] > 25.0)) {
				glPushMatrix();
					double cam_pos[] = {cam_dist * sin(cam_angl * M_PI / 180), (double)cam_hgt, cam_dist * cos(cam_angl * M_PI / 180)};
					glTranslatef(it->pos[0], it->pos[1], it->pos[2]);
					glScalef(it->size, it->size, it->size);
					glRotatef(-1.0 * atan2(cam_pos[2] - it->pos[2], cam_pos[0] - it->pos[0]) * 180 / M_PI + 290.0, 0.0, 1.0, 0.0);
					smokeParticle(it->t);
				glPopMatrix();
			}
		}
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		
	glPopMatrix();
	
	glutSwapBuffers();   //Useful for animation
}


void myTimer(int value)
{	
	theta++;
	int delay = 5;
	if (theta >= points) {
		theta = 0;
		delay = 3000;
	} 
	else if (theta > points - 100) {
		delay = 20;
	}
	else if (theta > points - 50) {
		delay = 30;
	}
	else if (theta < 50) {
		delay = 30;
	}
	else if (theta < 100) {
		delay = 20;
	}
	glutPostRedisplay();
	glutTimerFunc(delay, myTimer, 0);
}


void backLightTimer(int value)
{
	if (glIsEnabled(GL_LIGHT2)) glDisable(GL_LIGHT2);
	else glEnable(GL_LIGHT2);
	glutPostRedisplay();
	glutTimerFunc(500, backLightTimer, 0);
}


void carTimer(int value)
{
	if (car_t > 360) car_t = 0;
	if (car_t == 330.0) {
		if (theta < 768 || theta > 1035) car_t += 2;
	} else if (car_t == 50.0) {
		if (theta < 648 || theta > 915) car_t += 2;
	}
	
	else car_t += 2;
	glutPostRedisplay();
	glutTimerFunc(10, carTimer, 0);
}


void updateParticles () {
	list<particle>::iterator it;
	particle p;
	int T = 30;
	//Remove particles that have passed lifetime
	if (!parList.empty()) {
		p = parList.front();
		if (p.t > T) parList.pop_front();
	}
	//Update parameters
	for (it = parList.begin(); it != parList.end(); it++) {
		(it->t)++;
		for (int i = 0; i < 3; i++) (it->pos[i]) += (it->dir[i]) * it->speed;
		//it->size = ...;
		//it->delta = ...;
	}
}

void smokeTimer(int value)
{
	int m = 5;
	smokeCounter++;
	if (smokeCounter % m == 0) newParticle();
	updateParticles();
	glutTimerFunc(30, smokeTimer, 0);
	glutPostRedisplay();
}


//--Special keyboard event callback function ---------
void special(int key, int x, int y)
{
	if(key == GLUT_KEY_PAGE_UP && cam_hgt < 200) cam_hgt += 2;
	else if(key == GLUT_KEY_PAGE_DOWN && cam_hgt > 2) cam_hgt -= 2;
	else if(key == GLUT_KEY_DOWN && cam_dist < 400) cam_dist += 2;
	else if(key == GLUT_KEY_UP && cam_dist > 2) cam_dist -= 2;
	else if(key == GLUT_KEY_LEFT) cam_angl -= 5;
	else if(key == GLUT_KEY_RIGHT) cam_angl += 5;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 'c') {
		cam_mode++;
		if (cam_mode > 2) cam_mode = 0;
	}
	glutPostRedisplay();
}


//---------------------------------------------------------------------
int main(int argc, char** argv)
{
	//points = getPoints();
	points = 1243;
	std::ifstream file("track.txt");
	std::string line;
	
	x_points = (float*) malloc((points + 2)*sizeof(float));
	z_points = (float*) malloc((points + 2)*sizeof(float));
	int i = 0;

	while(std::getline(file, line))
	{
		if (i != 0) {
			std::stringstream linestream(line);
			float val1;
			float val2;
			
			// Read the integers using the operator >>
			linestream >> val1 >> val2;
			x_points[i - 1] = val1;
			z_points[i - 1] = val2;
		}
		i++;
	}
	
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize (600, 600); 
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("Toy Train");
	initialize ();

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(500, backLightTimer, 0);
	glutTimerFunc(10, carTimer, 0);
	glutTimerFunc(10, smokeTimer, 0);
	glutTimerFunc(10, myTimer, 0);
	glutDisplayFunc(display); 
	glutMainLoop();
	return 0;
}
