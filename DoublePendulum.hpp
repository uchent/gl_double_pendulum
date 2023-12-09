#ifndef _DBLPEN_H_
#define _DBLPEN_H_

#include "Vector.hpp"

class DoublePendulum
{
public:

	DoublePendulum(Vector3f rootPosition, float topRodLength, float bottomRodLength, float topBobWeight, float bottomBobWeight)
	{
		this->rootPosition = rootPosition;
		this->topRodLength = topRodLength;
		this->bottomRodLength = bottomRodLength;
		this->topBobWeight = topBobWeight;
		this->bottomBobWeight = bottomBobWeight;
	}
	void render() {
		glPushMatrix();
		glTranslatef(rootPosition.x, rootPosition.y, rootPosition.z);
		Vector3f bob1pos = GetBobPosition(topRodLength, topAngle);
		glColor3f(255.0f, 0.0f, 0.0f);
		glDisable(GL_LIGHT1);
		DrawLine(bob1pos);
		glEnable(GL_LIGHT1);

		glPushMatrix();
		glTranslatef(bob1pos.x, bob1pos.y, bob1pos.z);
		glColor3f(0.5f, 0.5f, 0.5f);

		int angle = topAngle * 180 / 3.1415;
		glPushMatrix();
		glRotatef(angle, 0, 0, 1);
		Draw_cube();
		glPopMatrix();

		Vector3f bob2pos = GetBobPosition(bottomRodLength, bottomAngle);
		glColor3f(0.0f, 255.0f, 0.0f);
		glDisable(GL_LIGHT1);
		DrawLine(bob2pos);
		glEnable(GL_LIGHT1);
		glTranslatef(bob2pos.x, bob2pos.y, bob2pos.z);
		glColor3f(1.0f, 1.0f, 1.0f);
		if (rootPosition.x == 0) {
			glColor3f(255.0f, 255.0f, 255.0f);
			glDisable(GL_LIGHT1);
			glEnable(GL_TEXTURE_2D);
		}
		Draw_sphere(bottomBobWeight, rootPosition.x);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHT1);

		if (rootPosition.x == 0) {
			bob2_gx = bob2pos.x + bob1pos.x + rootPosition.x;
			bob2_gy = bob2pos.y + bob1pos.y + rootPosition.y;
			bob2_gz = bob2pos.z + bob1pos.z + rootPosition.z;
		}
		glPopMatrix();
		if (rootPosition.x == 0) DrawTrajectory(bob1pos, bob2pos);
		glPopMatrix();
	}

	void DrawLine(Vector3f pos) {
		glLineWidth(5.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(pos.x, pos.y, pos.z);
		glEnd();
	}
	void Draw_cube() {
		float scale = powf(topBobWeight, 0.33) * 0.5;
		glScalef(scale, scale, scale);
		glutSolidCube(1);
		glScalef(1 / scale, 1 / scale, 1 / scale);
	}

	void Draw_sphere(int weight, float x) {
		GLUquadricObj* quad = gluNewQuadric();
		if (x == 0)
			gluQuadricTexture(quad, GL_TRUE);
		else
			gluQuadricTexture(quad, GL_FALSE);
		gluSphere(quad, powf(weight, 0.33) * 0.3, 20, 20);
		gluDeleteQuadric(quad);
	}
	void DrawTrajectory(Vector3f bob1pos, Vector3f bob2pos) {
		previous_x1[99] = bob1pos.x;
		previous_y1[99] = bob1pos.y;
		previous_x2[99] = bob2pos.x;
		previous_y2[99] = bob2pos.y;
		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(2.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < 99; i++) {
			if (previous_y2[i] != 0)
				glVertex3f(previous_x2[i] + previous_x1[i], previous_y2[i] + previous_y1[i], 0);
			previous_x1[i] = previous_x1[i + 1];
			previous_y1[i] = previous_y1[i + 1];
			previous_x2[i] = previous_x2[i + 1];
			previous_y2[i] = previous_y2[i + 1];
		}
		glEnd();
	}

	void updatePhysics(float deltaTime) {
		const float g = 9.8;

		const float m1 = topBobWeight;
		const float m2 = bottomBobWeight;
		const float a1 = topAngle;
		const float a2 = bottomAngle;
		const float r1 = topRodLength;
		const float r2 = bottomRodLength;
		const float a1_v = topAngleVelocity;
		const float a2_v = bottomAngleVelocity;

		float num1 = -g * (2 * m1 + m2) * sin(a1);
		float num2 = -m2 * g * sin(a1 - 2 * a2);
		float num3 = -2 * sin(a1 - a2) * m2;
		float num4 = a2_v * a2_v * r2 + a1_v * a1_v * r1 * cos(a1 - a2);
		float den = r1 * (2 * m1 + m2 - m2 * cos(2 * a1 - 2 * a2));
		topAngleAcceleration = (num1 + num2 + num3 * num4) / den;
		num1 = 2 * sin(a1 - a2);
		num2 = (a1_v * a1_v * r1 * (m1 + m2));
		num3 = g * (m1 + m2) * cos(a1);
		num4 = a2_v * a2_v * r2 * m2 * cos(a1 - a2);
		den = r2 * (2 * m1 + m2 - m2 * cos(2 * a1 - 2 * a2));
		bottomAngleAcceleration = (num1 * (num2 + num3 + num4)) / den;

		topAngleVelocity += topAngleAcceleration * deltaTime;
		bottomAngleVelocity += bottomAngleAcceleration * deltaTime;

		topAngle += topAngleVelocity * deltaTime;
		bottomAngle += bottomAngleVelocity * deltaTime;
	}

public:
	float topRodLength;
	float topBobWeight;
	float bottomRodLength;
	float bottomBobWeight;

	float previous_x1[100];
	float previous_y1[100];
	float previous_x2[100];
	float previous_y2[100] = { 0 };

	bool isLightSource = false;
	bool isUsingTexture = false;
	GLuint textureID = 0;

	float bob2_gx;
	float bob2_gy;
	float bob2_gz;

	Vector3f GetBobPosition(float r_length, float angle) {
		Vector3f pos;
		pos.x = r_length * sin(angle);
		pos.y = -r_length * cos(angle);
		pos.z = 0;
		return pos;
	}

private:
	Vector3f rootPosition;

	float topAngle = M_PI / 4;
	float bottomAngle = 0;

	float topAngleVelocity = 0;
	float bottomAngleVelocity = 0;

	float topAngleAcceleration = 0;
	float bottomAngleAcceleration = 0;
};

#endif