#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Object.h"
#include "glut.h"
#include <math.h>
using namespace std;

class Vertex {
	public:
		float x;
		float y;
		float z;
		Vertex() {};
		Vertex(float x, float y, float z): x(x), y(y), z(z) {}
		void normalize() {
			float magnitude;
			magnitude = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
			*this = *this / magnitude;
		}
		Vertex operator + (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x + obj.x;
			temp.y = this->y + obj.y;
			temp.z = this->z + obj.z;
			return temp;
		}
		Vertex operator - (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x - obj.x;
			temp.y = this->y - obj.y;
			temp.z = this->z - obj.z;
			return temp;
		}
		Vertex operator / (float const& num) {
			Vertex temp;
			temp.x = this->x / num;
			temp.y = this->y / num;
			temp.z = this->z / num;
			return temp;
		}
		friend ostream& operator<<(ostream& os, const Vertex& v) {
			os << v.x << "," << v.y << "," << v.z;
			return os;
		}
};

class Vector {
	public:
		float x;
		float y;
		float z;
		Vector() {};
		Vector(float x, float y, float z): x(x), y(y), z(z) {}
		Vector(Vertex v): x(v.x), y(v.y), z(v.z) {}
		void normalize() {
			float magnitude;
			magnitude = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
			*this = *this / magnitude;
		}
		Vector operator * (Vector const& obj) {
			Vector temp;
			temp.x = this->y * obj.z - obj.y * this->z;
			temp.y = this->z * obj.x - obj.z * this->x;
			temp.z = this->x * obj.y - obj.x * this->y;
			return temp;
		}
		Vector operator / (float const& num) {
			Vector temp;
			temp.x = this->x / num;
			temp.y = this->y / num;
			temp.z = this->z / num;
			return temp;
		}
};

// From TA
int windowSize[2];
unsigned int texture;
int width, height, nrChannels;
unsigned char* pikachuTexture;
Object* Clock = new Object("clock.obj");
Object* Pikachu = new Object("Pikachu.obj");

// Myself
float triSide = 10, triHeight = triSide*sqrt(3)/2, triHalfSide = 10/2;
Vertex hexaVtx[6] = {
	Vertex(triHalfSide 	, 0,  triHeight), 	
	Vertex(triSide	 	, 0,          0),
	Vertex(triHalfSide 	, 0, -triHeight), 	
	Vertex(-triHalfSide	, 0, -triHeight),
	Vertex(-triSide	 	, 0,          0),	
	Vertex(-triHalfSide	, 0,  triHeight),
};

Vertex SixTriVtx[6][3] = {
	{Vertex(0				, 0,  2*triHeight),	hexaVtx[0], hexaVtx[5]},
	{Vertex(3*triHalfSide	, 0,    triHeight),	hexaVtx[1], hexaVtx[0]},
	{Vertex(3*triHalfSide	, 0,   -triHeight), hexaVtx[2], hexaVtx[1]},
	{Vertex(0				, 0, -2*triHeight), hexaVtx[3], hexaVtx[2]},
	{Vertex(-3*triHalfSide	, 0,   -triHeight), hexaVtx[4], hexaVtx[3]},
	{Vertex(-3*triHalfSide	, 0,    triHeight), hexaVtx[5], hexaVtx[4]},
};
Vertex SoilVtx(0, -5, 0);
Vertex TwlRecVtx[12][4] = {
	{SixTriVtx[0][0], SixTriVtx[0][0]+SoilVtx , hexaVtx[0]+SoilVtx, hexaVtx[0]},
	{SixTriVtx[1][0], hexaVtx[0]              , hexaVtx[0]+SoilVtx, SixTriVtx[1][0]+SoilVtx},
	{SixTriVtx[1][0], SixTriVtx[1][0]+SoilVtx , hexaVtx[1]+SoilVtx, hexaVtx[1]},
	{SixTriVtx[2][0], hexaVtx[1]              , hexaVtx[1]+SoilVtx, SixTriVtx[2][0]+SoilVtx},
	{SixTriVtx[2][0], SixTriVtx[2][0]+SoilVtx , hexaVtx[2]+SoilVtx, hexaVtx[2]},
	{SixTriVtx[3][0], hexaVtx[2]              , hexaVtx[2]+SoilVtx, SixTriVtx[3][0]+SoilVtx},
	{SixTriVtx[3][0], SixTriVtx[3][0]+SoilVtx , hexaVtx[3]+SoilVtx, hexaVtx[3]},
	{SixTriVtx[4][0], hexaVtx[3]              , hexaVtx[3]+SoilVtx, SixTriVtx[4][0]+SoilVtx},
	{SixTriVtx[4][0], SixTriVtx[4][0]+SoilVtx , hexaVtx[4]+SoilVtx, hexaVtx[4]},
	{SixTriVtx[5][0], hexaVtx[4]              , hexaVtx[4]+SoilVtx, SixTriVtx[5][0]+SoilVtx},
	{SixTriVtx[5][0], SixTriVtx[5][0]+SoilVtx , hexaVtx[5]+SoilVtx, hexaVtx[5]},
	{SixTriVtx[0][0], hexaVtx[5]              , hexaVtx[5]+SoilVtx, SixTriVtx[0][0]+SoilVtx},
};
bool Rflag = false, Pflag = false, Mflag = false;
float Rpf = 1, Ppf = 1, Mpf = 2;
float curR = 0, curP = 0, curM = 0;
GLUquadricObj *hourHand = gluNewQuadric(), *minuteHand = gluNewQuadric();
float Hourpf = 1.0f/12.0f, Minutepf = 1;
float curHour = 0, curMinute = 0;

// TA's function
void light();
void display();
void idle();
void reshape(GLsizei, GLsizei);
void InitTexture();
void LoadModel(Object*, bool is_Pikachu = false);
void keyboard(unsigned char key, int x, int y);
void DrawBase();


// Myself function
void calNormal(int, float []);
void DrawPikachu();
void DrawClock();
void DrawHand();


// default light color, can be use to reset the material color
GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f};
GLfloat default_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f};


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("HW1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	InitTexture();
	glutMainLoop();
	return 0;
}

void light()
{
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_position[] = { 10.0, 10.0, 5.0, 0.0 };
	glShadeModel(GL_SMOOTH);
	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_specular);
	GLfloat mat_shininess[] = { 100 };
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], 1.0, 1000.0);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0.0, 15.0, 40.0,   // eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up

	light();
	
	////TO DO : add model, translate, rotate, scale ,material
	//

	if (Rflag) 	curR += Rpf;
	if (Pflag)	curP += Ppf;
	if (Mflag)	curM += Mpf;
	curHour += Hourpf;
	curMinute += Minutepf;
	

	glPushMatrix();
	glRotatef(curR, 0, 1, 0);
	DrawHand();
	DrawBase();
	DrawClock();
	glPopMatrix();

	// 邏輯：每個物體都有自己獨立的座標系統，
	// 因此當皮卡丘先在原點旋轉再平移後，會是公轉的樣子，若是先平移再旋轉，就是自轉的效果
	glPushMatrix();
	glRotatef(curM, 0, 1, 0);
	// curP is in DrawPikachu()
	DrawPikachu();
	glPopMatrix();

	

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void idle() {
	glutPostRedisplay();
}

void InitTexture() {
	stbi_set_flip_vertically_on_load(true);
	pikachuTexture = stbi_load("Pikachu.png", &width, &height, &nrChannels, 0);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pikachuTexture);

	glDisable(GL_TEXTURE_2D);
}

void LoadModel(Object* Model,bool is_Pikachu) {
	if (is_Pikachu) {
		GLfloat pikachu_diffuse[] = { 1, 1, 0, 1 };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pikachu_diffuse);
		glEnable(GL_TEXTURE_2D);
	}
	for (size_t i = 0; i < Model->fNum; ++i)
	{
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < 3; ++j)
		{
			if (is_Pikachu) {
				int idx = i * 3 * 2 + j * 2;
				glTexCoord2f(Model->texcoords[idx],Model->texcoords[idx+1]);
			}
			glNormal3fv(Model->nList[Model->faceList[i][j].nIndex].ptr);
			glVertex3fv(Model->vList[Model->faceList[i][j].vIndex].ptr);
		}
		glEnd();
	}
	if (is_Pikachu) {
		// set amibent back to default
		glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);
		glDisable(GL_TEXTURE_2D);
	}
}

void keyboard(unsigned char key, int x, int y) {
	//// TO DO : implement the keyboard function
	//
	switch (key) 
	{
		// press key 'r' to start/stop whole clock include base (but exclude Pikachu) rotation around itself
		case 'r':
		case 'R':
			if (Rflag) Rflag = false;
			else Rflag = true;
			printf("It's r or R.\n");
			break;
		// press key 'p' to start/stop Pikachu rotation around itself
		case 'p':
		case 'P':
			if (Pflag) Pflag = false;
			else Pflag = true;
			printf("It's p or P.\n");
			break;
		// press key 'm' to start/stop Pikachu revolution around the center
		case 'm':
		case 'M':
			if (Mflag) Mflag = false;
			else Mflag = true;
			printf("It's m or M.\n");
			break;

		default:
			break;
	}
	
}

void DrawBase() {

	//// TO DO : Draw base 
	//
	
	
	//// TO DO : draw the star polygon
	//

	// Draw the triangle with color #f72585
	GLfloat red_diffuse[] = { 247.0f / 255.0f , 37.0f / 255.0f , 133.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);

	glEnable(GL_CULL_FACE);

	glBegin(GL_TRIANGLES);
	for (int i = 0 ; i < 6 ; i++) {
		glNormal3f(0.0f, 1.0f, 0.0f);
		for (int j = 0 ; j < 3 ; j++) {
			glVertex3f(SixTriVtx[i][j].x, SixTriVtx[i][j].y, SixTriVtx[i][j].z);
		}
	}
	glEnd();

	// Draw the hexagon with color #4cc9f0
	GLfloat blue_diffuse[] = { 114.0f / 255.0f , 201.0f / 255.0f , 240.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);

	glEnable(GL_CULL_FACE);

	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 1.0f, 0.0f);
	for (int i = 0 ; i < 6 ; i++){
		glVertex3f(hexaVtx[i].x, hexaVtx[i].y, hexaVtx[i].z);
	}
	glEnd();

	//// TO DO : draw the rectangle edge
	//
	
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
	
	glEnable(GL_CULL_FACE);

	glBegin(GL_QUADS);
	for (int i = 0 ; i < 12 ; i++) {
		float normal[3] = {};
		calNormal(i, normal);
		glNormal3f(normal[0], normal[1], normal[2]);
		for (int j = 0 ; j < 4 ; j++) {
			glVertex3f(TwlRecVtx[i][j].x, TwlRecVtx[i][j].y, TwlRecVtx[i][j].z);
		}
	}
	glEnd();
}

void calNormal(int index, float normal[])
{
	Vertex vtx = TwlRecVtx[index][0], vtx1 = TwlRecVtx[index][1], vtx2 = TwlRecVtx[index][2];
	Vector vtr1(vtx1-vtx), vtr2(vtx2-vtx), cross = (vtr1 * vtr2);
	cross.normalize();
	normal[0] = cross.x;
	normal[1] = cross.y;
	normal[2] = cross.z;
}

void DrawPikachu()
{
	glPushMatrix();
	glTranslatef(14, 0, 0);
	glRotatef(45, 0, 1, 0);
	glRotatef(curP, 0, 1, 0);
	glScalef(10.0, 10.0, 10.0);
	LoadModel(Pikachu, true);
	glPopMatrix();
}

void DrawClock()
{
	glPushMatrix();
	glTranslatef(0, 7.5, 0);
	glScalef(0.08, 0.08, 0.08);
	LoadModel(Clock, false);
	glPopMatrix();
}

void DrawHand()
{
	glPushMatrix();
	glTranslatef(0, 7.5, 0);

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glRotatef(-curMinute, -1, 0, 0);
	GLfloat minuteHandDfs[] = {1, 0, 1, 1};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, minuteHandDfs);
	gluCylinder(minuteHand, 0.3, 0.3, height=5, 30, 30);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glRotatef(-curHour, 0, -1, 0);
	GLfloat hourHandDfs[] = {0, 1, 1, 1};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, hourHandDfs);
	gluCylinder(hourHand, 0.3, 0.3, height=3, 30, 30);
	glPopMatrix();

	glPopMatrix();
}
