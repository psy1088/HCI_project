#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stdlib.h> 
#include <GL/glut.h> 
#include <GL/GL.h> 
#include <GL/GLU.h> 
#include <Leap.h>
#include <ctime>
#include <mmsystem.h>
#include "stb_image.h"

using namespace std;

class MyListener : public Leap::Listener {
public:
	virtual void onConnect(const Leap::Controller &);
	virtual void onFrame(const Leap::Controller &);
};

//����� ���� x y ��ǥ
GLfloat hand_X = 0.0f;
GLfloat hand_Y = 0.0f;

// �簢�� ����
// # �簢�� ���� ���� (g_rectX, g_rectY)
// # �簢�� ������ �Ʒ��� (g_rectX + g_rectSize, g_rectY - g_rectSize)
GLfloat g_rectX = 0.0f;
GLfloat g_rectY = 0.0f;
GLfloat g_rectSize = 10.0f;

// �簢�� �̵� ����(�ӵ� �� ����)
// �ӵ� ���밪
GLfloat g_step = 0.0f;
// ���� �ӵ�
GLfloat g_xCurStep = g_step;
GLfloat g_yCurStep = g_step;

// Ŭ���� ���� ����
GLfloat g_clipBoxHeight = 200.0f;

// ���� Ŭ������ ���� ũ��
GLfloat g_clipHalfWidth = g_clipBoxHeight;
GLfloat g_clipHalfHeight = g_clipBoxHeight;

GLuint g_timeStep = 17u; // �ð� ����! ���� Ŭ���� �����̴� ����
GLubyte *pBytes; // �����͸� ����ų ������
//GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
BITMAPINFO *info;
GLuint texture[1];
static GLuint Texture;

int random = 0; // ��ü�� �����ϰ� �̵��ϱ� ���� ���� ���� ���� ����
int Catch_flag = 0; // �հ����� �������� Ȯ���ϱ� ���� ����


// ������� ����Ǿ����� Ȯ��
void MyListener::onConnect(const Leap::Controller &) {
	std::cout << "Connected." << std::endl;
}

// ����� �۵�
void MyListener::onFrame(const Leap::Controller & controller) {
	const Leap::Frame frame = controller.frame();
	Leap::InteractionBox iBox = frame.interactionBox();
	Leap::HandList hands = frame.hands();

	hands[0];
	hands[1];

	Leap::FingerList fingers = hands[0].fingers();
	fingers[0];
	fingers[1];
	fingers[2];
	fingers[3];
	fingers[4];
	Leap::Vector leapPoint = fingers[1].stabilizedTipPosition(); // �̰͵� �ΰ� �ε��� ����?
	Leap::Vector normalizedPoint = iBox.normalizePoint(leapPoint, false);

	float appX = normalizedPoint.x * 50; // �� ���ڰ� �����ϸ鼭 �ؾ��ҵ�
	float appY = normalizedPoint.y * 50;

	//cout << appX << ", " << appY << endl;
	hand_X = appX * 3;
	hand_Y = appY * 4;

	glRectf(hand_X, hand_Y, hand_X + g_rectSize, hand_Y - g_rectSize);

	//���� �νĵ��� �ʾ��� ��
	if (!hands[0].isValid()) {
	//	cout << "no hands detected." << endl;
	}
	else { 
		cout << "    " << endl;
		if (fingers[0].isExtended() == 1 && fingers[1].isExtended() == 1) {  // ������ ������ �� �� ����������~
			Catch_flag = 1;
			//cout << Catch_flag << " ���� ���� !!! " << endl;
		}
		else if (fingers[0].isExtended() + fingers[1].isExtended() + fingers[2].isExtended() + fingers[3].isExtended() + fingers[4].isExtended() + fingers[5].isExtended() == 0) {
			Catch_flag = 2;
			//cout << Catch_flag << " ��!!! " << endl;
		}
	}
}

// ����ǿ��� ���� �ν��Ͽ� ���� ��ġ�� ���� ����
void pointScene(GLfloat x, GLfloat y) {
	glBegin(GL_POLYGON);
	glVertex2f(x - 1, y - 1);
	glVertex2f(x - 1, y + 1);
	glVertex2f(x + 1, y - 1);
	glVertex2f(x + 1, y + 1);
	glEnd();
	glFinish();
}

// �� �׸���
void RenderScene(void)
{
	// ȭ���� �����(�÷���)
	glClear(GL_COLOR_BUFFER_BIT);

	// ���� ������ ��������!
	glColor3f(1.0f, 1.0f, 1.0f);


	//// �̹��� �ؽ��� �κ�
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("mogi_turn.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	////�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);


	// *�簢�� �׸���~
	// �簢���� ���� ������ ������ �Ʒ����� ��� ����
	glRectf(g_rectX, g_rectY, g_rectX + g_rectSize, g_rectY - g_rectSize);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(g_rectX, g_rectY - g_rectSize);
	glTexCoord2i(0, 1); glVertex2i(g_rectX, g_rectY);
	glTexCoord2i(1, 1); glVertex2i(g_rectX + g_rectSize, g_rectY);
	glTexCoord2i(1, 0); glVertex2i(g_rectX + g_rectSize, g_rectY - g_rectSize);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	pointScene(hand_X, hand_Y);
	// * ���� ���ۿ� �ĸ���۸� ��ü
	glutSwapBuffers();
}

// â ũ�� ����
void ChangeWindowSize(GLsizei width, GLsizei height)
{
	// ���̰� 0�̸� 0���� ������ ������ �����Ƿ�
	// 1�� �����Ͽ� ���� ó��
	if (height == 0) height = 1;

	// ���� ��� ���� ���� ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Ŭ���� �ڽ� ���� ����(���� ��� ����)
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	g_clipHalfHeight = g_clipBoxHeight * 0.5f;
	g_clipHalfWidth = g_clipHalfHeight * aspect;
	glOrtho(-g_clipHalfWidth, g_clipHalfWidth,
		-g_clipHalfHeight, g_clipHalfHeight,
		1.0f, -1.0f);

	// �� ��Ʈ(���� �������� ��ġ/ũ��) ����
	glViewport(0, 0, width, height);
}

// ������ �ð��� ȣ���
void TimerFunc(int value)
{
	srand((unsigned int)time(0));
	random = rand() % 8;

	// �����ϰ� ��ü�� �̵���Ų��.
	switch (random) {
	case 0:
		g_rectX += g_xCurStep;
		g_rectY += g_yCurStep;
		break;
	case 1:
		g_rectX += g_xCurStep;
		g_rectY -= g_yCurStep;
		break;
	case 2:
		g_rectX -= g_xCurStep;
		g_rectY += g_yCurStep;
		break;
	case 3:
		g_rectX -= g_xCurStep;
		g_rectY -= g_yCurStep;
		break;
	case 4:
		g_rectX += g_xCurStep;
		break;
	case 5:
		g_rectX -= g_xCurStep;
		break;
	case 6:
		g_rectY += g_yCurStep;
		break;
	case 7:
		g_rectY -= g_yCurStep;
		break;
	default:
		g_rectX += g_xCurStep;
		g_rectY += g_yCurStep;
		break;
	}

	//cout << "X : " << g_rectX << "     Y : " << g_rectY << endl;

	// # ��� �˻� : ��迡 �簢���� �ε����� ������ �ٲ۴�.
	// �簢�� �����̳� �������� Ŭ�������� �������
	// �¿� �̵� ���͸� �ٲ۴�.
	if (g_rectX < -g_clipHalfWidth) {
		g_rectX = -g_clipHalfWidth;
		g_xCurStep = g_step;
	}
	else if (g_rectX > g_clipHalfWidth - g_rectSize) {
		g_rectX = g_clipHalfWidth - g_rectSize;
		g_xCurStep = -g_step;
	}

	// �簢�� �����̳� �Ʒ����� Ŭ�������� �������
	// ���� �̵� ���͸� �ٲ۴�.
	if (g_rectY < -g_clipHalfHeight + g_rectSize) {
		g_rectY = -g_clipHalfHeight + g_rectSize;
		g_yCurStep = g_step;
	}
	else if (g_rectY > g_clipHalfHeight) {
		g_rectY = g_clipHalfHeight;
		g_yCurStep = -g_step;
	}

	// ���� ��ǥ�� ��Ÿ���� ��ü��, ����� ��ǥ ���� �ȿ� ������~

		if (g_rectX <= hand_X && hand_X <= g_rectX + g_rectSize) {
			if (g_rectY - g_rectSize <= hand_Y && hand_Y <= g_rectY) {
				if (Catch_flag == 2) {
					cout << "@@@@@@@@@@@@@@@@ ����ҿ� " << endl;
					PlaySound(TEXT("catch.wav"), NULL,0); // ��� �Ҹ��� ���
					PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_NOSTOP); // ��� �Ҹ��� ���
				}
			}
		}

	// ����� �ٽ� �׸���.
	glutPostRedisplay();

	// �ٽ� Ÿ�ӽ��� ���Ŀ� ���� �Լ��� �Ҹ� �� �ֵ��� �����Ҵ�.
	glutTimerFunc(g_timeStep, TimerFunc, 1);
}

int main(int argc, char** argv)
{
	PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP); // ��� �Ҹ��� ���

	Leap::Controller controller;
	MyListener listener;
	controller.addListener(listener);

	glutInit(&argc, argv); // GLUT ������ �Լ� 
	glutInitDisplayMode(GLUT_RGB); // ������ �� �⺻�÷���带 RGB���� ���� 
	glutInitWindowSize(1000, 500); // ������ ������ ���� 
	glutInitWindowPosition(0, 0); // ������ â ��ġ ���� 
	glutCreateWindow("Catch me if you can");

	glClearColor(0.0, 0.0, 0.0, 1.0); // GL ���º��� ����, ������ ���İ��� 1�̸� ������ 0�̸� ���� 
	//glMatrixMode(GL_PROJECTION); // glLoadIdentity(); 
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glutDisplayFunc(RenderScene); // GLUT �ݹ��Լ� ��� 
	glutReshapeFunc(ChangeWindowSize);

	// * Ÿ�ӽ��� ���Ŀ� Ÿ�̸� �Լ��� �Ҹ� �� �ֵ��� ����
	glutTimerFunc(g_timeStep, TimerFunc, 1);

	glutMainLoop(); // �̺�Ʈ ���� ���� 

	return 0;
}