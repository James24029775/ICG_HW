
#include "glut.h"
#include <iostream>

using namespace std;

void display();
void reshape(int _width, int _height);

int width = 400, height = 400;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	// https://www.easyatm.com.tw/wiki/glutSwapBuffers
	// GLUT_DOUBLE 意思為宣告 2 個 buffer，一個給螢幕顯示，一個給作畫運算。目的是避免只有一個 buffer 時，會邊計算邊顯示，造成螢幕閃爍
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("WindowName");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}

void display()
{
	// https://blog.csdn.net/dcrmg/article/details/53106457
	// ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
	// Viewport 在此程式無作用。此 function 可以在原本的 windows 割一塊小視窗，並把結果 map 到這。通常會用這個函式做出切割視窗的效果
	// Viewport Matrix
	glViewport(0, 0, width, height);

	// glClearColor, glClearDepth 填入的參數，要在 glClear 執行時才會 assign 到 buffer
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0);
	// 以下兩行設定depth buffer，第一行開啟深度測試，第二行開啟條件，符合此條件(小於等於depth buffer儲存的值)則通過測試，否則拋棄
	glEnable(GL_DEPTH_TEST);    //depth test
	glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);

	// https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/04%20Face%20culling/
	// glEnable: 開啟 Culling 功能
	// glCullFace: 預設 cull back。可以指定 cull front, cull back or cull front and back
	// glFrontFace: 預設 CCW。可指定順時針(左手)，逆時針(右手)，根據繪製點的順序決定 polygon 的法向量
	// Face Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glBegin(GL_TRIANGLES);

	//colorful triangle (z = 0) (vertex order is counterclockwise)
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	//blue triangle (z = -1) (vertex order is clockwise)
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, -1.0f);

	glEnd();

	// 交還給前台 buffer
	glutSwapBuffers();
}

void reshape(int _width, int _height) {
	width = _width;
	height = _height;
}
