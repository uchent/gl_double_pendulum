#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _USE_MATH_DEFINES

#include "./GL/glut.h"
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "Image.hpp"
#include "DoublePendulum.hpp"

#define TEXTURE_NUM 2

#define LENGTH 13.0

char* texture_name[TEXTURE_NUM] = {
	"./resources/sun.bmp"
	"./resources/ceiling.bmp",
};
GLuint texture[TEXTURE_NUM];

#define deltaTime 10
double passTime = 0.0;

float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float LightAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };

float CameraPosition[] = { 0.0f, 0.0f, -36.0f };
float CameraLookAt[] = { 0.0f, 0.0f, 0.0f };
float CameraUp[] = { 0.0f, 1.0f, 0.0f };
float angle_X = 0;
float angle_Y = 0;
float angle_Z = 0;

Image* img0, * img1;


DoublePendulum* doublePendulumSystems[3];

void RenderPlanes() {
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(-LENGTH, LENGTH, LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(LENGTH, -LENGTH, LENGTH);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(LENGTH, LENGTH, LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(LENGTH, -LENGTH, -LENGTH);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(-LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(-LENGTH, LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(-LENGTH, -LENGTH, LENGTH);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(-LENGTH, LENGTH, LENGTH);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 0);
	glVertex3f(-LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 1);
	glVertex3f(LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(0, 1);
	glVertex3f(LENGTH, -LENGTH, -LENGTH);
	glEnd();
}

void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(CameraPosition[0],
		CameraPosition[1],
		CameraPosition[2],
		CameraLookAt[0],
		CameraLookAt[1],
		CameraLookAt[2],
		CameraUp[0],
		CameraUp[1],
		CameraUp[2]);

	RenderPlanes();

	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	for (int i = 0; i < 3; i++)
	{
		doublePendulumSystems[i]->render();
	}
	LightPos[0] = doublePendulumSystems[0]->bob2_gx;
	LightPos[1] = doublePendulumSystems[0]->bob2_gy;
	LightPos[2] = doublePendulumSystems[0]->bob2_gz;
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);

	glEnable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

void Tick(int id) {
	double d = deltaTime / 1000.0;
	passTime += d;

	for (int i = 0; i < 3; i++)
	{
		if (doublePendulumSystems[i])
			doublePendulumSystems[i]->updatePhysics(d * 3);
	}

	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0);
}

void WindowSize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	img0 = loadTexture(texture_name[0]);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img0->sizeX, img0->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, img0->data);

	img1 = loadTexture(texture_name[1]);
	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img1->sizeX, img1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, img1->data);

	doublePendulumSystems[0] = new DoublePendulum(Vector3f(0.0, 13.0, 0.0), 10.0, 5.0, 5.0, 10.0);
	doublePendulumSystems[1] = new DoublePendulum(Vector3f(-6.5, 13.0, -6.5), 5.5, 5.0, 10.0, 6.0);
	doublePendulumSystems[2] = new DoublePendulum(Vector3f(6.5, 13.0, 6.5), 5.0, 5.5, 4.0, 8.0);
}

void DealKeyboardInput(unsigned char key, int x, int y) {
	std::cout << "Input = " << key << std::endl;
	switch (key)
	{
	case 'd':
		CameraPosition[0] -= 0.5;
		CameraLookAt[0] -= 0.5;
		break;
	case 'a':
		CameraPosition[0] += 0.5;
		CameraLookAt[0] += 0.5;
		break;
	case 'w':
		CameraPosition[1] += 0.5;
		CameraLookAt[1] += 0.5;
		break;
	case 's':
		CameraPosition[1] -= 0.5;
		CameraLookAt[1] -= 0.5;
		break;
	case 'e':
		CameraPosition[2] += 0.5;
		CameraLookAt[2] += 0.5;
		break;
	case 'q':
		CameraPosition[2] -= 0.5;
		CameraLookAt[2] -= 0.5;
		break;
	case 'j':
		angle_X -= 0.05;
		CameraLookAt[1] = CameraPosition[1] + sinf(angle_X);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_X);
		break;
	case 'l':
		angle_X += 0.05;
		CameraLookAt[1] = CameraPosition[1] + sinf(angle_X);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_X);
		break;
	case 'i':
		angle_Y -= 0.05;
		CameraLookAt[0] = CameraPosition[0] + sinf(angle_Y);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_Y);
		break;
	case 'k':
		angle_Y += 0.05;
		CameraLookAt[0] = CameraPosition[0] + sinf(angle_Y);
		CameraLookAt[2] = CameraPosition[2] + cosf(angle_Y);
		break;
	case 'u':
		angle_Z -= 0.05;
		CameraUp[0] = sinf(angle_Z);
		CameraUp[1] = cosf(angle_Z);
		break;
	case 'o':
		angle_Z += 0.05;
		CameraUp[0] = sinf(angle_Z);
		CameraUp[1] = cosf(angle_Z);
		break;
	case 'b':
		glDisable(GL_LIGHT1);
		break;
	}
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(800, 600);
	glutCreateWindow("double pendulum");
	glutDisplayFunc(Display);
	glutReshapeFunc(WindowSize);
	glutTimerFunc(deltaTime, Tick, 0);
	glutKeyboardFunc(DealKeyboardInput);

	init();

	glutMainLoop();
	return 0;
}