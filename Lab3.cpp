// Lab3.cpp : Anton Kuzmin

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
#include <map>

//C:/Users/Anton/Desktop/Graphics/Lab3/Lab3/cactus.obj
//C:/Users/Anton/Desktop/Graphics/Lab3/Lab3/bunny.obj

std::vector<unsigned int> fixedFaces, shaderFaces;
std::vector<glm::vec3> inputVertices;
GLint type = 0;
GLfloat nearClip = 2, farClip, farClipOriginal;
GLfloat fovLeft, fovRight, fovBottom, fovTop;
GLfloat cameraX, cameraY, cameraZ, lookX, lookY, lookZ;
glm::vec3 lookatpos, cpos, up, forward, left;
static GLfloat cameraSpeed = 0.2f, angle = 0.1f;

GLuint vsID, fsID, pID, VBO, VAO, EBO;
GLfloat projectionMat[16], modelViewMat[16];
GLint window1, window2;

std::vector<glm::vec3> faceVectors;
GLint light = 1, smooth = 1;
GLfloat global_ambient[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat mat_diffuse[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

class vertexNormalData
{
private:
	std::vector<GLfloat> weights;
	std::vector<glm::vec3> normals;
	glm::vec3 vertexNormal;

public:
	void addFace(GLfloat w, glm::vec3 norm)
	{
		weights.push_back(w);
		normals.push_back(norm);
	}

	void calculateNormal()
	{
		glm::vec3 total = glm::vec3(0, 0, 0);
		GLfloat sum = 0;

		for (int i = 0; i < weights.size(); i++)
		{
			total += (weights[i] * normals[i]);
			sum += weights[i];
		}
		vertexNormal = total / sum;
	}

	glm::vec3 getFaceNormal()
	{
		return normals[0];
	}

	glm::vec3 getNormal()
	{
		return vertexNormal;
	}
};

std::map<GLfloat, vertexNormalData> vertexMap;

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

glm::mat3 rotationMatrix(GLfloat angle, glm::vec3 axis)
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

			glm::vec3 face = glm::vec3(vertexIndex[0], vertexIndex[1], vertexIndex[2]);
			faceVectors.push_back(face);
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

void calculateVertexNormals()
{
	for (int i = 0; i < faceVectors.size(); i++)
	{
		GLint one = faceVectors[i].x;
		GLint two = faceVectors[i].y;
		GLint three = faceVectors[i].z;
		glm::vec3 a = inputVertices[two - 1] - inputVertices[one - 1];
		glm::vec3 b = inputVertices[three - 1] - inputVertices[one - 1];
		glm::vec3 c = inputVertices[three - 1] - inputVertices[two - 1];
		glm::vec3 norm = glm::normalize(glm::cross(a, b));

		GLfloat x = glm::length(a);
		GLfloat y = glm::length(b);
		GLfloat z = glm::length(c);
		GLfloat s = 0.5 * (x + y + z);
		GLfloat area = sqrt(s * (s - x) * (s - y) * (s - z));

		if (!vertexMap.count(faceVectors[i].x))
		{
			vertexNormalData temp;
			vertexMap.insert(std::pair<GLfloat, vertexNormalData>(faceVectors[i].x, temp));
		}
		if (!vertexMap.count(faceVectors[i].y))
		{
			vertexNormalData temp;
			vertexMap.insert(std::pair<GLfloat, vertexNormalData>(faceVectors[i].y, temp));
		}
		if (!vertexMap.count(faceVectors[i].z))
		{
			vertexNormalData temp;
			vertexMap.insert(std::pair<GLfloat, vertexNormalData>(faceVectors[i].z, temp));
		}
		vertexMap[faceVectors[i].x].addFace(area, norm);
		vertexMap[faceVectors[i].y].addFace(area, norm);
		vertexMap[faceVectors[i].z].addFace(area, norm);
	}

	for (int i = 1; i < inputVertices.size() + 1; i++)
	{
		vertexMap[i].calculateNormal();
	}
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

	//Material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 12.0);

	if (type == 0) glBegin(GL_TRIANGLES);
	if (type == 1) glBegin(GL_LINE_LOOP);
	if (type == 2) glBegin(GL_POINTS);
	glNormal3fv(&vertexMap[a].getNormal()[0]);
	glVertex3fv(vertices[0]);
	glNormal3fv(&vertexMap[b].getNormal()[0]);
	glVertex3fv(vertices[1]);
	glNormal3fv(&vertexMap[c].getNormal()[0]);
	glVertex3fv(vertices[2]);
	glEnd();
}

void object()
{
	for (int i = 0; i < fixedFaces.size() - 2; i += 3)
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

	GLfloat p[16] = { a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, c, -1,
		0, 0, d, 0 };

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
	GLfloat m[16] = { left[0], up[0], -forward[0], 0,
		left[1], up[1], -forward[1], 0,
		left[2], up[2], -forward[2], 0,
		-x, -y, z, 1 };

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

	if (smooth == 1)
	{
		readShaderFile("vertexshader.txt", vertShaderString);
		readShaderFile("fragshader.txt", fragShaderString);
	}
	else
	{
		readShaderFile("flatvertexshader.txt", vertShaderString);
		readShaderFile("flatfragshader.txt", fragShaderString);
	}

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
	glUniformMatrix4fv(modeViewMatLocation, 1, GL_FALSE, modelViewMat);

	GLint projectionMatLocation = glGetUniformLocation(pID, "projectionMatrix");
	glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, projectionMat);

	GLint cameraPositionLocation = glGetUniformLocation(pID, "cameraPosition");
	glUniform3fv(cameraPositionLocation, 1, &cpos[0]);

	GLint lightPositionLocation = glGetUniformLocation(pID, "lightPosition");
	glm::vec3 lightPosition(0.0f, 0.0f, 5.0f);
	glUniform3fv(lightPositionLocation, 1, &lightPosition[0]);
	
	GLint ambientLightLocation = glGetUniformLocation(pID, "ambientLight");
	glm::vec3 aL(global_ambient[0], global_ambient[1], global_ambient[2]);
	glUniform3fv(ambientLightLocation, 1, &aL[0]);

	GLint matAmbLocation = glGetUniformLocation(pID, "matAmb");
	glm::vec3 mA(mat_ambient[0], mat_ambient[1], mat_ambient[2]);
	glUniform3fv(matAmbLocation, 1, &mA[0]);

	GLint matDiffLocation = glGetUniformLocation(pID, "matDiff");
	glm::vec3 mD(mat_diffuse[0], mat_diffuse[1], mat_diffuse[2]);
	glUniform3fv(matDiffLocation, 1, &mD[0]);

	GLint matSpecLocation = glGetUniformLocation(pID, "matSpec");
	glm::vec3 mS(mat_specular[0], mat_specular[1], mat_specular[2]);
	glUniform3fv(matSpecLocation, 1, &mS[0]);

	GLint lightAmbLocation = glGetUniformLocation(pID, "lightAmb");
	glm::vec3 lA(light_ambient[0], light_ambient[1], light_ambient[2]);
	glUniform3fv(lightAmbLocation, 1, &lA[0]);

	GLint lightDiffLocation = glGetUniformLocation(pID, "lightDiff");
	glm::vec3 lD(light_diffuse[0], light_diffuse[1], light_diffuse[2]);
	glUniform3fv(lightDiffLocation, 1, &lD[0]);

	GLint lightSpecLocation = glGetUniformLocation(pID, "lightSpec");
	glm::vec3 lS(light_specular[0], light_specular[1], light_specular[2]);
	glUniform3fv(lightSpecLocation, 1, &lS[0]);

	GLint lightSwitchLocation = glGetUniformLocation(pID, "lightSwitch");
	glUniform1i(lightSwitchLocation, light);

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
		glm::vec3 vertNorm = vertexMap[counter + 1].getNormal();

		vertDataTemp[i] = inputVertices[counter].x;
		vertDataTemp[i + 1] = inputVertices[counter].y;
		vertDataTemp[i + 2] = inputVertices[counter].z;
		vertDataTemp[i + 3] = vertNorm.x;
		vertDataTemp[i + 4] = vertNorm.y;
		vertDataTemp[i + 5] = vertNorm.z;
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

void keyboard(unsigned char key, int x, int y)
{
	GLboolean change = false;
	if (key == 'o') type = 0; //triangles
	if (key == 'i') type = 1; //wireframe
	if (key == 'p') type = 2; //points
	if (key == 'z') { global_ambient[0] = 1.0; global_ambient[1] = 0.0; global_ambient[2] = 0.0; } //global ambient red
	if (key == 'x') { global_ambient[0] = 0.0; global_ambient[1] = 1.0; global_ambient[2] = 0.0; } //global ambient green
	if (key == 'c') { global_ambient[0] = 0.0; global_ambient[1] = 0.0; global_ambient[2] = 1.0; } //global ambient blue
	if (key == '1') { light_ambient[0] = 1.0; light_ambient[1] = 0.0; light_ambient[2] = 0.0; } //light ambient red
	if (key == '2') { light_ambient[0] = 0.0; light_ambient[1] = 1.0; light_ambient[2] = 0.0; } //light ambient blue
	if (key == '3') { light_ambient[0] = 0.0; light_ambient[1] = 0.0; light_ambient[2] = 1.0; } //light ambient green
	if (key == '4') { light_diffuse[0] = 1.0; light_diffuse[1] = 0.0; light_diffuse[2] = 0.0; } //light diffuse red
	if (key == '5') { light_diffuse[0] = 0.0; light_diffuse[1] = 1.0; light_diffuse[2] = 0.0; } //light diffuse green
	if (key == '6') { light_diffuse[0] = 0.0; light_diffuse[1] = 0.0; light_diffuse[2] = 1.0; } //light diffuse blue
	if (key == '7') { light_specular[0] = 1.0; light_specular[1] = 0.0; light_specular[2] = 0.0; } //light specular red
	if (key == '8') { light_specular[0] = 0.0; light_specular[1] = 1.0; light_specular[2] = 0.0; } //light specular green
	if (key == '9') { light_specular[0] = 0.0; light_specular[1] = 0.0; light_specular[2] = 1.0; } //light specular blue
	if (key == 'v') //decrease near clipping plane
	{
		nearClip -= 0.2f;
		calculateProjectionMatrix();
	}
	if (key == 'b') //increase near clipping plane
	{
		nearClip += 0.2f;
		calculateProjectionMatrix();
	}
	if (key == 'n') //decrease far clipping plane
	{
		farClip -= 0.2f;
		calculateProjectionMatrix();
	}
	if (key == 'm') //increase far clipping plane
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
		glm::mat3 r = rotationMatrix(0.1, left);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		forward = glm::normalize(glm::cross(up, left));
		calculateModelViewMatrix();
	}
	if (key == 's') //look down
	{
		glm::mat3 r = rotationMatrix(-0.1, left);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		forward = glm::normalize(glm::cross(up, left));
		calculateModelViewMatrix();
	}
	if (key == 'a') //look left
	{
		glm::mat3 r = rotationMatrix(0.1, up);
		glm::vec3 newForward = glm::vec3(forward[0] * r[0][0] + forward[1] * r[0][1] + forward[2] * r[0][2],
			forward[0] * r[1][0] + forward[1] * r[1][1] + forward[2] * r[1][2],
			forward[0] * r[2][0] + forward[1] * r[2][1] + forward[2] * r[2][2]);
		forward = newForward;
		left = glm::normalize(glm::cross(forward, up));
		calculateModelViewMatrix();
	}
	if (key == 'd') //look right
	{
		glm::mat3 r = rotationMatrix(-0.1, up);
		glm::vec3 newForward = glm::vec3(forward[0] * r[0][0] + forward[1] * r[0][1] + forward[2] * r[0][2],
			forward[0] * r[1][0] + forward[1] * r[1][1] + forward[2] * r[1][2],
			forward[0] * r[2][0] + forward[1] * r[2][1] + forward[2] * r[2][2]);
		forward = newForward;
		left = glm::normalize(glm::cross(forward, up));
		calculateModelViewMatrix();
	}
	if (key == 'q') //rotationMatrix left
	{
		glm::mat3 r = rotationMatrix(0.175, forward);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		left = glm::cross(forward, up);
		calculateModelViewMatrix();
	}
	if (key == 'e') //roll right
	{
		glm::mat3 r = rotationMatrix(-0.175, forward);
		glm::vec3 newUp = glm::vec3(up[0] * r[0][0] + up[1] * r[0][1] + up[2] * r[0][2],
			up[0] * r[1][0] + up[1] * r[1][1] + up[2] * r[1][2],
			up[0] * r[2][0] + up[1] * r[2][1] + up[2] * r[2][2]);
		up = newUp;
		left = glm::cross(forward, up);
		calculateModelViewMatrix();
	}
	if (key == ',') //move camera forward
	{
		cpos -= cameraSpeed * forward;
		calculateModelViewMatrix();
	}
	if (key == '.') //move camera backward
	{
		cpos += cameraSpeed * forward;
		calculateModelViewMatrix();
	}
	if (key == 'r') //reset camera
	{
		nearClip = 2;
		farClip = farClipOriginal;
		calculateProjectionMatrix();
		initialize();
		calculateModelViewMatrix();
	}
	if (key == 't') //print matrices
	{
		printCamera();
		printModelView();
		printProjection();
	}
	if (key == 'l') //light on/off
	{
		if (light == 1)
		{
			glDisable(GL_LIGHT0);
			light = 0;
		}
		else
		{
			glEnable(GL_LIGHT0);
			light = 1;
		}
	}
	if (key == 'f') //flat and Gouraud shading
	{
		if (smooth == 1)
		{
			glShadeModel(GL_FLAT);
			smooth = 0;
		}
		else
		{
			glShadeModel(GL_SMOOTH);
			smooth = 1;
		}
	}
}

void processSpecialKeys(int key, int xx, int yy)
{
	if (key == GLUT_KEY_LEFT) //move camera left
	{
		cpos -= glm::normalize(glm::cross(forward, up)) * cameraSpeed;
		calculateModelViewMatrix();
	}
	if (key == GLUT_KEY_RIGHT) //move camera right
	{
		cpos += glm::normalize(glm::cross(forward, up)) * cameraSpeed;
		calculateModelViewMatrix();
	}
	if (key == GLUT_KEY_UP) //move camera up
	{
		cpos += cameraSpeed * up;
		calculateModelViewMatrix();
	}
	if (key == GLUT_KEY_DOWN) //move camera down
	{
		cpos -= cameraSpeed * up;
		calculateModelViewMatrix();
	}
}

// fixed pipeline rendering
void fixedDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	//Global ambient light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	//Light source
	GLfloat light_position[] = { 0.0, 0.0, 5.0, 1.0 }; //point light
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

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

	setShaders();
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
	std::string originalPath = "C:/Users/Anton/Desktop/Graphics/Lab3/Lab3/cactus.obj";
	readInObject(originalPath);
	calculateVertexNormals();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);

	glutInitWindowPosition(10, 10);
	window1 = glutCreateWindow("Lab 2 Fixed Pipeline");
	glEnable(GL_LIGHT0);
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
