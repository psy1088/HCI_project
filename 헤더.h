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
#include "thinkgear.h"

using namespace std;

class MyListener : public Leap::Listener {
public:
	virtual void onConnect(const Leap::Controller &);
	virtual void onFrame(const Leap::Controller &);
};

class Blood_mogi {
public:
	GLfloat x;
	GLfloat y;
};

//�׸��� �Լ���
void Draw_mogi();
void Draw_HP();
void Draw_mogi_count();
void Draw_hand_point(GLfloat x, GLfloat y);
void Draw_blood_mogi(GLfloat x, GLfloat y);

Blood_mogi blood_mogi[100]; // ��� ��� ����Ƚ��



char *comPortName = NULL;            //Ȱ��ȭ�� comport������ ������ ���� ���� ����
int connectionld = 0;            //�������ġ�� ID
int packetsRead = 0;            //�����͸� �о� ���̴� ����
int state = 0;               //����� ���� ���� �޴� ����
int ATTSt = 0;               //Attention value�� ���º���
int MEDSt = 0;                 //Meditation value�� ���º���
int ATTD = 0;               //Attention ������ ���� �޴� ����
int MEDD = 0;               //Meditation ������ ���� �޴� ����
int i = 0;
// ��� ���� ��ġ
GLfloat blood_X = 0.0f;
GLfloat blood_Y = 0.0f;

// ��� ����
GLfloat Alpha = 1.0f;

// ���� ��� ���� ����
int catchWidth, catchHeight, catchNrChannels;
unsigned char *howManyImage;
int catchCount = 0;

// ��� ��ü
int bloodWidth, bloodHeight, bloodNrChannels;
unsigned char *blood_mogi_img;

// HP ����
GLfloat HP = 190;
LPCWSTR hpStr = TEXT("HP");

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
GLfloat g_step = 0.3f;
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
GLuint texture[10];
static GLuint Texture;

int random = 0; // ��ü�� �����ϰ� �̵��ϱ� ���� ���� ���� ���� ����
int Catch_flag = 0; // �հ����� �������� Ȯ���ϱ� ���� ����
int blood_img = 0; // ��Ⱑ �׾��� �� �̹����� �ٲٱ�����


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


// ����ǿ��� ���� �ν��Ͽ� ���� ��ġ�� ���� ����
void Draw_hand_point(GLfloat x, GLfloat y) {
	glBegin(GL_POLYGON);
	glVertex2f(x - 1, y - 1);
	glVertex2f(x - 1, y + 1);
	glVertex2f(x + 1, y - 1);
	glVertex2f(x + 1, y + 1);
	glEnd();
	glFinish();
}

// ��� �׸�
void Draw_mogi() {
	/*glGenTextures(3, texture);*/
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("mogi_turn.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	////�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, Alpha);
	glTexCoord2i(0, 0); glVertex2i(g_rectX, g_rectY - g_rectSize);
	glTexCoord2i(0, 1); glVertex2i(g_rectX, g_rectY);
	glTexCoord2i(1, 1); glVertex2i(g_rectX + g_rectSize, g_rectY);
	glTexCoord2i(1, 0); glVertex2i(g_rectX + g_rectSize, g_rectY - g_rectSize);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	//cout << "ALPHA : " << Alpha << endl;
	//glClear(GL_COLOR_BUFFER_BIT); // GL ���º��� ����, ������ ���İ��� 1�̸� ������ 0�̸� ���� 
}

// HP�� �׸���
void Draw_HP() {
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	int hpWidth, hpHeight, hpNrChannels;
	unsigned char *hpData = stbi_load("hpName.png", &hpWidth, &hpHeight, &hpNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, hpWidth, hpHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, hpData);
	////�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2i(0, 0); glVertex2i(110, 100);
	glTexCoord2i(0, 1); glVertex2i(110, 85);
	glTexCoord2i(1, 1); glVertex2i(125, 85);
	glTexCoord2i(1, 0); glVertex2i(125, 100);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// hp ������
	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(125, 95, HP, 90);
	glFlush();
}

// ���� ��� ���� �׸���
void Draw_mogi_count() {
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	howManyImage = stbi_load("one.png", &catchWidth, &catchHeight, &catchNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, catchWidth, catchHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, howManyImage);
	//////�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(-180, 95);
	glTexCoord2i(0, 1); glVertex2i(-180, 80);
	glTexCoord2i(1, 1); glVertex2i(-140, 80);
	glTexCoord2i(1, 0); glVertex2i(-140, 95);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ��� ��ü �׸���
void Draw_blood_mogi(GLfloat x, GLfloat y) {
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	blood_mogi_img = stbi_load("blood.png", &bloodWidth, &bloodHeight, &bloodNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bloodWidth, bloodHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, blood_mogi_img);
	//�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2i(0, 0); glVertex2i(x, y - g_rectSize);
	glTexCoord2i(0, 1); glVertex2i(x, y);
	glTexCoord2i(1, 1); glVertex2i(x + g_rectSize, y);
	glTexCoord2i(1, 0); glVertex2i(x + g_rectSize, y - g_rectSize);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

