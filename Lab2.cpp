// Lab2.cpp : Anton Kuzmin

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/gtc/matrix_transform.hpp"
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

//C:/Users/Anton/Desktop/Graphics/Lab2/Lab2/cactus.obj
//C:/Users/Anton/Desktop/Graphics/Lab2/Lab2/bunny.obj

std::vector<unsigned int> fixedFaces, shaderFaces;
std::vector<glm::vec3> inputVertices;
GLint type = 0, colorIndex = 0;
GLfloat nearClip = 2, farClip, farClipOriginal;
GLfloat fovLeft, fovRight, fovBottom, fovTop;
GLfloat cameraX, cameraY, cameraZ, lookX, lookY, lookZ;
glm::vec3 lookatpos, cpos, up, forward, left;
static GLfloat cameraSpeed = 0.2f, angle = 0.1f;
GLint red = 99, green = 150, blue = 115;

GLuint vsID, fsID, pID, VBO, VAO, EBO;
GLfloat projectionMat[16], modelViewMat[16];
GLint window1, window2;

GLint colors[360] = {
	0,   0,   0,   0,   0,   1,   1,   2,
	2,   3,   4,   5,   6,   7,   8,   9,
	11,  12,  13,  15,  17,  18,  20,  22,
	24,  26,  28,  30,  32,  35,  37,  39,
	42,  44,  47,  49,  52,  55,  58,  60,
	63,  66,  69,  72,  75,  78,  81,  85,
	88,  91,  94,  97, 101, 104, 107, 111,
	114, 117, 121, 124, 127, 131, 134, 137,
	141, 144, 147, 150, 154, 157, 160, 163,
	167, 170, 173, 176, 179, 182, 185, 188,
	191, 194, 197, 200, 202, 205, 208, 210,
	213, 215, 217, 220, 222, 224, 226, 229,
	231, 232, 234, 236, 238, 239, 241, 242,
	244, 245, 246, 248, 249, 250, 251, 251,
	252, 253, 253, 254, 254, 255, 255, 255,
	255, 255, 255, 255, 254, 254, 253, 253,
	252, 251, 251, 250, 249, 248, 246, 245,
	244, 242, 241, 239, 238, 236, 234, 232,
	231, 229, 226, 224, 222, 220, 217, 215,
	213, 210, 208, 205, 202, 200, 197, 194,
	191, 188, 185, 182, 179, 176, 173, 170,
	167, 163, 160, 157, 154, 150, 147, 144,
	141, 137, 134, 131, 127, 124, 121, 117,
	114, 111, 107, 104, 101,  97,  94,  91,
	88,  85,  81,  78,  75,  72,  69,  66,
	63,  60,  58,  55,  52,  49,  47,  44,
	42,  39,  37,  35,  32,  30,  28,  26,
	24,  22,  20,  18,  17,  15,  13,  12,
	11,   9,   8,   7,   6,   5,   4,   3,
	2,   2,   1,   1,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0 };

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

GLfloat dot(glm::vec3 a, glm::vec3 b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

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

			fixedFaces.push_back(vertexIndex[0]);
			fixedFaces.push_back(vertexIndex[1]);
			fixedFaces.push_back(vertexIndex[2]);
			shaderFaces.push_back(vertexIndex[0] - 1);
			shaderFaces.push_back(vertexIndex[1] - 1);
			shaderFaces.push_back(vertexIndex[2] - 1);
		}
	}
	GLfloat diffX = abs(minX) + abs(maxX);
	GLfloat diffY = abs(minY) + abs(maxY);
	GLfloat diffZ = abs(minZ) + abs(maxZ);
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

void readShaderFile(const GLchar* shaderPath, std::string& shaderCode)
{
	std::ifstream shaderFile;

	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;

		// Read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();

		// close file handlers
		shaderFile.close();

		// Convert stream into GLchar array
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
}

void polygon(int a, int b, int c)
{
	GLfloat vertices[][3] = { { inputVertices[a - 1].x, inputVertices[a - 1].y, inputVertices[a - 1].z },
	{ inputVertices[b - 1].x, inputVertices[b - 1].y, inputVertices[b - 1].z },
	{ inputVertices[c - 1].x, inputVertices[c - 1].y, inputVertices[c - 1].z } };

	//GLfloat color[] = {r, g, b};
	GLfloat colors[][3] = { { 1.0, 0.5, 0.5 },{ 0.5, 1.0, 0.5 },{ 0.5, 0.5, 1.0 } };

	if (type == 0) glBegin(GL_TRIANGLES);
	if (type == 1) glBegin(GL_LINE_LOOP);
	if (type == 2) glBegin(GL_POINTS);

	glColor3fv(colors[colorIndex]);
	glVertex3fv(vertices[0]);
	glColor3fv(colors[colorIndex]);
	glVertex3fv(vertices[1]);
	glColor3fv(colors[colorIndex]);
	glVertex3fv(vertices[2]);
	glEnd();
}

void object()
{
	for (int i = 0; i < fixedFaces.size() - 3; i += 3)
	{
		polygon(fixedFaces[i], fixedFaces[i + 1], fixedFaces[i + 2]);
	}
}

void printCamera()
{
	printf("Camera:\n");
	printf("forward: %f %f %f\n", forward[0], forward[1], forward[2]);
	printf("up: %f %f %f\n", up[0], up[1], up[2]);
	printf("left: %f %f %f\n", left[0], left[1], left[2]);
	printf("position: %f %f %f\n\n", cpos[0], cpos[1], cpos[2]);
}

void printModelView()
{
	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("Model View Matrix:\n");
	for (int i = 0; i < 16; i++)
	{
		if (i == 4 || i == 8 || i == 12) printf("\n");
		printf("%f ", m[i]);
	}
	printf("\n\n");
}

void printProjection()
{
	GLfloat p[16];
	glGetFloatv(GL_PROJECTION_MATRIX, p);
	printf("Projection Matrix:\n");
	for (int i = 0; i < 16; i++)
	{
		if (i == 4 || i == 8 || i == 12) printf("\n");
		printf("%f ", p[i]);
	}
	printf("\n\n");
}

void calculateProjectionMatrix()
{
	GLfloat a, b, c, d;
	a = nearClip / fovRight;
	b = nearClip / fovTop;
	c = -(farClip + nearClip) / (farClip - nearClip);
	d = -(2 * farClip * nearClip) / (farClip - nearClip);

	GLfloat p[16] = {a, 0, 0, 0, 
					0, b, 0, 0, 
					0, 0, c, -1, 
					0, 0, d, 0};
	
	for (int i = 0; i < 16; i++)
	{
		projectionMat[i] = p[i];
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(projectionMat);
	glMatrixMode(GL_MODELVIEW);
}

void calculateModelViewMatrix()
{
	GLfloat x = dot(cpos, left);
	GLfloat y = dot(cpos, up);
	GLfloat z = dot(cpos, forward);
	GLfloat m[16] = {left[0], up[0], -forward[0], 0,
					left[1], up[1], -forward[1], 0,
					left[2], up[2], -forward[2], 0,
					-x, -y, z, 1};

	for (int i = 0; i < 16; i++)
	{
		modelViewMat[i] = m[i];
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(modelViewMat);
}

void setShaders()
{
	char *vs = NULL, *fs = NULL;

	std::string vertShaderString; //empty string
	std::string fragShaderString; //empty string

	vsID = glCreateShader(GL_VERTEX_SHADER);
	fsID = glCreateShader(GL_FRAGMENT_SHADER);

	readShaderFile("vertexshader.txt", vertShaderString);
	readShaderFile("fragshader.txt", fragShaderString);

	const GLchar * pVertShaderSource = vertShaderString.c_str();
	const GLchar * pFragShaderSource = fragShaderString.c_str();

	glShaderSource(vsID, 1, &pVertShaderSource, NULL);
	glShaderSource(fsID, 1, &pFragShaderSource, NULL);

	glCompileShader(vsID);
	glCompileShader(fsID);

	pID = glCreateProgram();
	glAttachShader(pID, vsID);
	glAttachShader(pID, fsID);

	glLinkProgram(pID);
	glUseProgram(pID);

	// get uniform input 
	GLint modeViewMatLocation = glGetUniformLocation(pID, "modelViewMatrix");
	GLint projectionMatLocation = glGetUniformLocation(pID, "projectionMatrix");

	glUniformMatrix4fv(modeViewMatLocation, 1, GL_FALSE, modelViewMat);
	glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, projectionMat);

	glDeleteShader(vsID);
	glDeleteShader(fsID);
}

void initBufferObject(void)
{
	// Setup object vertex data
	int vertDataSize = inputVertices.size() * 6;
	std::vector<GLfloat> vertDataTemp(vertDataSize);
	int counter = 0;

	for (int i = 0; i < vertDataSize; i += 6)
	{
		vertDataTemp[i] = inputVertices[counter].x;
		vertDataTemp[i + 1] = inputVertices[counter].y;
		vertDataTemp[i + 2] = inputVertices[counter].z;
		vertDataTemp[i + 3] = colors[red] / 256.0f;
		vertDataTemp[i + 4] = colors[green] / 256.0f;
		vertDataTemp[i + 5] = colors[blue] / 256.0f;
		if (red > 255) red = 0;
		if (green > 255) green = 0;
		if (blue > 255) blue = 0;
		red++;
		green++;
		blue++;
		counter++;
	}

	GLfloat *vertData = &vertDataTemp[0];
	GLuint *indices = &shaderFaces[0];

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// setup VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertDataSize * sizeof(GLfloat), vertData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shaderFaces.size() * sizeof(GLuint), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Use depth buffering for hidden surface elimination
	glEnable(GL_DEPTH_TEST);

	calculateProjectionMatrix();
	calculateModelViewMatrix();
	setShaders();
}

void updateColor()
{
	int vertDataSize = inputVertices.size() * 6;
	std::vector<GLfloat> vertDataTemp(vertDataSize);
	int counter = 0;

	for (int i = 0; i < vertDataSize; i += 6)
	{
		vertDataTemp[i] = inputVertices[counter].x;
		vertDataTemp[i + 1] = inputVertices[counter].y;
		vertDataTemp[i + 2] = inputVertices[counter].z;
		vertDataTemp[i + 3] = colors[red] / 256.0f;
		vertDataTemp[i + 4] = colors[green] / 256.0f;
		vertDataTemp[i + 5] = colors[blue] / 256.0f;
		if (red > 255) red = 0;
		if (green > 255) green = 0;
		if (blue > 255) blue = 0;
		red++;
		green++;
		blue++;
		counter++;
	}

	GLfloat *vertData = &vertDataTemp[0];

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertDataSize * sizeof(GLfloat), vertData);
}

void keyboard(unsigned char key, int x, int y)
{
	GLboolean change = false;
	if (key == 'o') type = 0; //triangles
	if (key == 'i') type = 1; //wireframe
	if (key == 'p') type = 2; //points
	if (key == 'z') { red = 100; green = 50; blue = 10; colorIndex = 0; updateColor(); } //red
	if (key == 'x') { red = 199; green = 50; blue = 15; colorIndex = 1; updateColor(); } //green
	if (key == 'c') { red = 150; green = 50; blue = 15; colorIndex = 2; updateColor(); } //blue
	if (key == '1') //decrease near clipping plane
	{
		nearClip -= 0.2f;
		calculateProjectionMatrix();
	}
	if (key == '2') //increase near clipping plane
	{
		nearClip += 0.2f;
		calculateProjectionMatrix();
	}
	if (key == '3') //decrease far clipping plane
	{
		farClip -= 0.2f;
		calculateProjectionMatrix();
	}
	if (key == '4') //increase far clipping plane
	{
		farClip += 0.2f;
		calculateProjectionMatrix();
	}
	if (key == 'u') //read in new object
	{
		std::string newPath;
		printf("Enter new object path: \n");
		std::cin >> newPath;
		inputVertices.clear();
		fixedFaces.clear();
		shaderFaces.clear();
		readInObject(newPath);
		initBufferObject();
	}

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
		calculateProjectionMatrix();
		initialize();
	}
	if (key == 't') //print matrices
	{
		printCamera();
		printModelView();
		printProjection();
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
}

// fixed pipeline rendering
void fixedDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glLoadIdentity();
	calculateProjectionMatrix();
	calculateModelViewMatrix();
	object();
	glFlush();
	glutSwapBuffers();
}

// shader-based rendering
void shaderDisplay()
{
	glUseProgram(pID);
	//glUseProgram(0);

	glValidateProgram(pID);
	GLint validate = 0;
	glGetProgramiv(pID, GL_VALIDATE_STATUS, &validate);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);
	if (type == 0) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (type == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (type == 2) glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glDrawElements(GL_TRIANGLES, shaderFaces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glutSwapBuffers();

	calculateModelViewMatrix();
	setShaders();
	updateColor();
}

void idleDisplay()
{
	glutSetWindow(window1);
	fixedDisplay();
	glutPostRedisplay();

	glutSetWindow(window2);
	shaderDisplay();
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	std::string originalPath = "C:/Users/Anton/Desktop/Graphics/Lab2/Lab2/cactus.obj";
	readInObject(originalPath);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);

	glutInitWindowPosition(10, 10);
	window1 = glutCreateWindow("Lab 2 Fixed Pipeline");
	glutDisplayFunc(fixedDisplay);
	glutIdleFunc(idleDisplay);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);

	glutInitWindowPosition(650, 10);
	window2 = glutCreateWindow("Lab 2 Shader Pipeline");
	glewInit();
	initBufferObject();
	glutDisplayFunc(shaderDisplay);
	glutIdleFunc(idleDisplay);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);

	glutMainLoop();
	return 0;
}