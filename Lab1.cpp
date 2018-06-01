// Lab1.cpp : Anton Kuzmin

#include "stdafx.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "GL/freeglut.h"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <iostream>
#include <string>

//C:/Users/Anton/Desktop/Graphics/Lab1/Lab1/cactus.obj
//C:/Users/Anton/Desktop/Graphics/Lab1/Lab1/bunny.obj

std::vector<unsigned int> faces;
std::vector<glm::vec3> inputVertices;
GLint type = 0;
GLint colorIndex = 0;
GLfloat nearClip = 2, farClip, farClipOriginal;
GLfloat fovLeft, fovRight, fovBottom, fovTop;
GLfloat xa = 0, ya = 0, za = 0;
static GLfloat cameraSpeed = 0.2f;
GLfloat cameraX, cameraY, cameraZ;
GLfloat lookX, lookY, lookZ;
glm::vec3 lookatpos;
glm::vec3 cpos;
glm::vec3 up;
glm::vec3 forward;
glm::vec3 left;

void initialize()
{
	lookatpos = glm::vec3(lookX, lookY, lookZ);
	cpos = glm::vec3(0, 0, 5);
	up = glm::vec3(0, 1, 0);
	forward = glm::normalize(lookatpos - cpos);
	left = glm::normalize(glm::cross(forward, up));
}

void readInObject(std::string path)
{
	GLfloat minX = NULL, minY = NULL, minZ = NULL, maxX = NULL, maxY = NULL, maxZ = NULL;

	FILE * file = fopen(path.c_str(), "r");

	if (file == NULL)
	{
		printf("Impossible to open file!\n");
		return;
	}

	while (1)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			inputVertices.push_back(vertex);

			if (minX == NULL) minX = vertex.x;
			if (minY == NULL) minY = vertex.y;
			if (minZ == NULL) minZ = vertex.z;
			if (maxX == NULL) maxX = vertex.x;
			if (maxY == NULL) maxY = vertex.y;
			if (maxZ == NULL) maxZ = vertex.z;
			if (minX > vertex.x) minX = vertex.x;
			if (minY > vertex.y) minY = vertex.y;
			if (minZ > vertex.z) minZ = vertex.z;
			if (maxX < vertex.x) maxX = vertex.x;
			if (maxY < vertex.y) maxY = vertex.y;
			if (maxZ < vertex.z) maxZ = vertex.z;
		}

		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3];
			int matches = fscanf(file, "%d %d %d", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

			faces.push_back(vertexIndex[0]);
			faces.push_back(vertexIndex[1]);
			faces.push_back(vertexIndex[2]);
		}
	}
	GLfloat diffX = abs(minX) + abs(maxX);
	GLfloat diffY = abs(minY) + abs(maxY);
	GLfloat diffZ = abs(minZ) + abs(maxZ);
	printf("%f %f", minX, maxX);
	GLfloat fovScale = 0;

	if (diffY > diffX) fovScale = diffY / 2;
	if (diffX > diffY) fovScale = diffX / 2;

	fovLeft = fovScale;
	fovRight = fovScale;
	fovBottom = fovScale;
	fovTop = fovScale;

	cameraX = diffX / 2 + minX;
	cameraY = diffY / 2 + minY;
	cameraZ = abs(minZ) + 5;

	farClip = ceil(diffZ) + cameraZ;
	farClipOriginal = farClip;

	lookX = diffX / 2 + minX;
	lookY = diffY / 2 + minY;
	lookZ = diffZ / 2 + minZ;

	initialize();
}

void polygon(int a, int b, int c)
{
	GLfloat vertices[][3] = { {inputVertices[a - 1].x, inputVertices[a - 1].y, inputVertices[a - 1].z},
	{ inputVertices[b - 1].x, inputVertices[b - 1].y, inputVertices[b - 1].z },
	{ inputVertices[c - 1].x, inputVertices[c - 1].y, inputVertices[c - 1].z } };

	GLfloat colors[][3] = { { 1.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0 },{ 0.0, 0.0, 1.0 } };

	int c1, c2, c3;

	if (type == 0) glBegin(GL_TRIANGLES);
	if (type == 1) glBegin(GL_LINE_LOOP);
	if (type == 2) glBegin(GL_POINTS);
	if (colorIndex == 3)
	{
		c1 = 0;
		c2 = 1;
		c3 = 2;
	}
	else
	{
		c1 = colorIndex;
		c2 = colorIndex;
		c3 = colorIndex;
	}
	glColor3fv(colors[c1]);
	glVertex3fv(vertices[0]);
	glColor3fv(colors[c2]);
	glVertex3fv(vertices[1]);
	glColor3fv(colors[c3]);
	glVertex3fv(vertices[2]);
	glEnd();
}

void object()
{
	for (int i = 0; i < faces.size() - 3; i += 3)
	{
		polygon(faces[i], faces[i + 1], faces[i + 2]);
	}
}

glm::mat3 trans(glm::vec3 v)
{
	return glm::mat3(v[0] * v[0], v[0] * v[1], v[0] * v[2],
		v[0] * v[1], v[1] * v[1], v[1] * v[2],
		v[0] * v[2], v[1] * v[2], v[2] * v[2]);
}

glm::mat3 dualM(glm::vec3 v, GLfloat angle)
{
	return glm::mat3(sin(angle) * 0, sin(angle) * -v[2], sin(angle) * v[1],
		sin(angle) * v[2], sin(angle) * 0, sin(angle) * -v[0],
		sin(angle) * -v[1], sin(angle) * v[0], sin(angle) * 0);
}

glm::mat3 id()
{
	return glm::mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
}

glm::mat3 roll(GLfloat angle, glm::vec3 axis)
{
	glm::mat3 temp = id() - trans(axis);
	
	temp[0][0] = cos(angle) * temp[0][0];
	temp[0][1] = cos(angle) * temp[0][1];
	temp[0][2] = cos(angle) * temp[0][2];
	temp[1][0] = cos(angle) * temp[1][0];
	temp[1][1] = cos(angle) * temp[1][1];
	temp[1][2] = cos(angle) * temp[1][2];
	temp[2][0] = cos(angle) * temp[2][0];
	temp[2][1] = cos(angle) * temp[2][1];
	temp[2][2] = cos(angle) * temp[2][2];

	glm::mat3 temp2 = dualM(axis, angle);
	glm::mat3 r = trans(axis) + temp + temp2;
	return r;
}

glm::mat4 conv(glm::mat3 m)
{
	glm::mat4 r = glm::mat4(m[0][0], m[1][0], m[2][0], 0, 
							m[0][1], m[1][1], m[2][1], 0,
							m[0][2], m[1][2], m[2][2], 0,
							0, 0, 0, 1);

	printf("conv\n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%f ", r[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	return r;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glLoadIdentity();
	gluLookAt(cpos.x, cpos.y, cpos.z, cpos.x + forward.x, cpos.y + forward.y, cpos.z + forward.z, up.x, up.y, up.z);
	object();
	glFlush();
	glutSwapBuffers();
}

void printM()
{
	printf("\nforward: %f %f %f\n", forward[0], forward[1], forward[2]);
	printf("up: %f %f %f\n", up[0], up[1], up[2]);
	printf("left: %f %f %f\n", left[0], left[1], left[2]);
	printf("position: %f %f %f\n\n", cpos[0], cpos[1], cpos[2]);
}

void pMV()
{
	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("model\n");
	for (int i = 0; i < 16; i++)
	{
		if (i == 4 || i == 8 || i == 12) printf("\n");
		printf("%f ", m[i]);
	}
	printf("\n");
}

/*
void rotationMatrix()
{
	GLfloat rotationMatrix[9] = 
	{cos(za) * cos(ya), (cos(za) * sin(ya) * sin(xa)) - (sin(za) * cos(xa)), (cos(za) * sin(ya) * cos(xa)) + (sin(za) * sin(xa)),
	 sin(za) * cos(ya), (sin(za) * sin(ya) * sin(xa)) + (cos(za) * cos(xa)), (sin(za) * sin(ya) * cos(xa)) - (cos(za) * sin(xa)),
	 -sin(ya), cos(ya) * sin(xa), cos(ya) * cos(xa)};

	printf("rotationMatrix:\n");
	for (int i = 0; i < 9; i++)
	{
		printf("%f ", rotationMatrix[i]);
		if (i == 2 || i == 5 || i == 8)
		{
			printf("\n");
		}
	}

	glm::vec3 newLookAt = glm::vec3(forward[0] * rotationMatrix[0] + forward[1] * rotationMatrix[1] + forward[2] * rotationMatrix[2],
		forward[0] * rotationMatrix[3] + forward[1] * rotationMatrix[4] + forward[2] * rotationMatrix[5],
		forward[0] * rotationMatrix[6] + forward[1] * rotationMatrix[7] + forward[2] * rotationMatrix[8]);
	
	glm::vec3 newUp = glm::vec3(up[0] * rotationMatrix[0] + up[1] * rotationMatrix[1] + up[2] * rotationMatrix[2],
		up[0] * rotationMatrix[3] + up[1] * rotationMatrix[4] + up[2] * rotationMatrix[5],
		up[0] * rotationMatrix[6] + up[1] * rotationMatrix[7] + up[2] * rotationMatrix[8]);

	forward = glm::normalize(newLookAt);
	up = glm::normalize(newUp);
	left = glm::normalize(glm::cross(forward, up));

	printM();
}
*/

void adjustClipPlane()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-fovLeft, fovRight, -fovBottom, fovTop, nearClip, farClip);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	GLboolean change = false;
	if (key == 'o') type = 0; //triangles
	if (key == 'i') type = 1; //wireframe
	if (key == 'p') type = 2; //points
	if (key == 'z') colorIndex = 0; //red
	if (key == 'x') colorIndex = 1; //green
	if (key == 'c') colorIndex = 2; //blue
	if (key == 'f') colorIndex = 3; //rainbow
	if (key == 'v') glFrontFace(GL_CW); //clockwise
	if (key == 'b') glFrontFace(GL_CCW); //counter-clockwise
	if (key == '1') //decrease near clipping plane
	{
		nearClip -= 0.2f;
		adjustClipPlane();
	}
	if (key == '2') //increase near clipping plane
	{
		nearClip += 0.2f;
		adjustClipPlane();
	}
	if (key == '3') //decrease far clipping plane
	{
		farClip -= 0.2f;
		adjustClipPlane();
	}
	if (key == '4') //increase far clipping plane
	{
		farClip += 0.2f;
		adjustClipPlane();
	}
	if (key == 'u') //read in new object
	{
		std::string newPath;
		printf("Enter new object path: \n");
		std::cin >> newPath;
		inputVertices.clear();
		faces.clear();
		readInObject(newPath);
	}

	GLfloat angle = 0.1f;

	if (key == 'w') //look up
	{
		glm::mat3 r = roll(0.1, left);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		forward = glm::normalize(glm::cross(up, left));
	}
	if (key == 's') //look down
	{
		glm::mat3 r = roll(-0.1, left);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		forward = glm::normalize(glm::cross(up, left));
	}
	if (key == 'a') //look left
	{
		glm::mat3 r = roll(0.1, up);
		glm::vec3 newForward = glm::vec3(forward[0] * r[0][0] + forward[1] * r[0][1] + forward[2] * r[0][2],
			forward[0] * r[1][0] + forward[1] * r[1][1] + forward[2] * r[1][2],
			forward[0] * r[2][0] + forward[1] * r[2][1] + forward[2] * r[2][2]);
		forward = newForward;
		left = glm::normalize(glm::cross(forward, up));
	}
	if (key == 'd') //look right
	{
		glm::mat3 r = roll(-0.1, up);
		glm::vec3 newForward = glm::vec3(forward[0] * r[0][0] + forward[1] * r[0][1] + forward[2] * r[0][2],
			forward[0] * r[1][0] + forward[1] * r[1][1] + forward[2] * r[1][2],
			forward[0] * r[2][0] + forward[1] * r[2][1] + forward[2] * r[2][2]);
		forward = newForward;
		left = glm::normalize(glm::cross(forward, up));
	}
	if (key == 'q') //roll left
	{
		glm::mat3 r = roll(0.175, forward);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		left = glm::cross(forward, up);
	}
	if (key == 'e') //roll right
	{
		glm::mat3 r = roll(-0.175, forward);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		left = glm::cross(forward, up);
	}
	if (key == ',') //move camera forward
	{
		cpos += cameraSpeed * forward;
	}
	if (key == '.') //move camera backward
	{
		cpos -= cameraSpeed * forward;
	}
	if (key == 'r') //reset camera
	{
		nearClip = 2;
		farClip = farClipOriginal;
		adjustClipPlane();
		initialize();
		printM();
	}
}

void processSpecialKeys(int key, int xx, int yy)
{
	if (key == GLUT_KEY_LEFT) //move camera left
	{
		cpos -= glm::normalize(glm::cross(forward, up)) * cameraSpeed;
	}
	if (key == GLUT_KEY_RIGHT) //move camera right
	{
		cpos += glm::normalize(glm::cross(forward, up)) * cameraSpeed;
	}
	if (key == GLUT_KEY_UP) //move camera up
	{
		cpos += cameraSpeed * up;
	}
	if (key == GLUT_KEY_DOWN) //move camera down
	{
		cpos -= cameraSpeed * up;
	}
	printM();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-fovLeft, fovRight, -fovBottom, fovTop, nearClip, farClip);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{	
	std::string originalPath = "C:/Users/Anton/Desktop/Graphics/Lab1/Lab1/cactus.obj";
	readInObject(originalPath);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitContextFlags(GLUT_COMPATIBILITY_PROFILE);
	glutCreateWindow("Lab 1");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}
