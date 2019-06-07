#define STB_IMAGE_IMPLEMENTATION
#define Scissors 1
#define Rock 2

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
void Draw_hand_point_Scissors(GLfloat x, GLfloat y);
void Draw_hand_point_Rock(GLfloat x, GLfloat y);
void Draw_blood_mogi(GLfloat x, GLfloat y);

Blood_mogi blood_mogi[100]; // ��� ��� ����Ƚ��

char *comPortName = NULL;            //Ȱ��ȭ�� comport������ ������ ���� ���� ����
int connectionld = 0;            //�������ġ�� ID
int packetsRead = 0;            //�����͸� �о� ���̴� ����
int state = 0;               //����� ���� ���� �޴� ����
int ATTSt = 0;               //Attention value�� ���º���
int ATTD = 0;               //Attention ������ ���� �޴� ����
int i = 0;

// ��� ���� ��ġ
GLfloat blood_X = 0.0f;
GLfloat blood_Y = 0.0f;

// ��� ����
GLfloat Alpha = 1.0f;

// ������ ����
GLfloat ATTAlpha = 0.0f;
GLfloat recoverAlpha = 0.0f;
GLfloat lightAlpha = 0.0f;
GLfloat itemTime = 0;

//���� �׸�
int BackgroundWidth, BackgroundHeight, BackgroundNrChannels;
unsigned char *Background_img;

// ���� ��� ���� ����
const char *catchImage[6] = { "zero.png", "one.png", "two.png", "three.png","four.png","five.png" };
const char *howManycatchImage = catchImage[0];
int catchCount = 0;

// ��� ��ü
int bloodWidth, bloodHeight, bloodNrChannels;
unsigned char *blood_mogi_img;

//�ָ� �׸�
int rockWidth, rockHeight, rockNrChannels;
unsigned char *rock_img;

//���� �׸�
int scissorsWidth, scissorsHeight, scissorsNrChannels;
unsigned char *scissors_img;


int finger_shape = 0;

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
//GLubyte *pBytes; // �����͸� ����ų ������
//GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
BITMAPINFO *info;
GLuint texture[12];
static GLuint Texture;

int Random_course = 0; // ��ü�� �����ϰ� �̵��ϱ� ���� ���� ���� ���� ����
int Random_speed = 0;
int Catch_flag = 0; // �հ����� �������� Ȯ���ϱ� ���� ����
int blood_img = 0; // ��Ⱑ �׾��� �� �̹����� �ٲٱ�����


				   
// *********************************** ����� �κ� *********************************//

void MyListener::onConnect(const Leap::Controller &) { // ������� ����Ǿ����� Ȯ��
	std::cout << "Connected." << std::endl;
}

void MyListener::onFrame(const Leap::Controller & controller) { // ����� �۵�
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
	Leap::Vector leapPoint = fingers[1].stabilizedTipPosition(); 
	Leap::Vector normalizedPoint = iBox.normalizePoint(leapPoint, false);

	float appX = normalizedPoint.x * 50; // �� ���ڰ� �����ϸ鼭 �ؾ��ҵ�
	float appY = normalizedPoint.y * 50 - 7;

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
			Catch_flag = Scissors;
			//cout << Catch_flag << " ���� ���� !!! " << endl;
		}
		else if (fingers[0].isExtended() + fingers[1].isExtended() + fingers[2].isExtended() + fingers[3].isExtended() + fingers[4].isExtended() + fingers[5].isExtended() == 0) {
			Catch_flag = Rock;
			//cout << Catch_flag << " ��!!! " << endl;
		}
	}
}
// *********************************** ����� �κ� �� *********************************//


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
void Draw_hand_point_Rock(GLfloat x, GLfloat y) {
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	rock_img = stbi_load("�ָ�1.png", &rockWidth, &rockHeight, &rockNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rockWidth, rockHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rock_img);
	//�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2i(0, 0); glVertex2i(x-7, y-7);
	glTexCoord2i(0, 1); glVertex2i(x-7, y+7);
	glTexCoord2i(1, 1); glVertex2i(x+7, y+7);
	glTexCoord2i(1, 0); glVertex2i(x+7, y-7);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ����ǿ��� ���� �ν��Ͽ� ���� ��ġ�� ���� ����
void Draw_hand_point_Scissors(GLfloat x, GLfloat y) {
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	scissors_img = stbi_load("���Լ�1.png", &scissorsWidth, &scissorsHeight, &scissorsNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scissorsWidth, scissorsHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, scissors_img);
	//�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2i(0, 0); glVertex2i(x - 7, y - 7);
	glTexCoord2i(0, 1); glVertex2i(x - 7, y + 7);
	glTexCoord2i(1, 1); glVertex2i(x + 7, y + 7);
	glTexCoord2i(1, 0); glVertex2i(x + 7, y - 7);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	glTexCoord2i(0, 0); glVertex2i(110, 95);
	glTexCoord2i(0, 1); glVertex2i(110, 80);
	glTexCoord2i(1, 1); glVertex2i(125, 80);
	glTexCoord2i(1, 0); glVertex2i(125, 95);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// hp ������
	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(125, 90, HP, 85);
	glFlush();
}

// ���� ��� ���� �׸���
void Draw_mogi_count() {
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	int catchWidth, catchHeight, catchNrChannels;
	unsigned char *howManyImage;

	howManyImage = stbi_load(howManycatchImage, &catchWidth, &catchHeight, &catchNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, catchWidth, catchHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, howManyImage);

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


// ������ �κ� �׸���
void Draw_Item() {
	// ���ߵ�
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	int Width, Height, NrChannels;
	unsigned char *attention = stbi_load("attention.png", &Width, &Height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, attention);
	////�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, ATTAlpha);
	glTexCoord2i(0, 0); glVertex2i(-110, 95);
	glTexCoord2i(0, 1); glVertex2i(-110, 80);
	glTexCoord2i(1, 1); glVertex2i(-50, 80);
	glTexCoord2i(1, 0); glVertex2i(-50, 95);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// HP ȸ�� ������
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	unsigned char *item1 = stbi_load("recover.png", &Width, &Height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, item1);
	////�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, recoverAlpha);
	glTexCoord2i(0, 0); glVertex2i(-30, 95);
	glTexCoord2i(0, 1); glVertex2i(-30, 80);
	glTexCoord2i(1, 1); glVertex2i(20, 80);
	glTexCoord2i(1, 0); glVertex2i(20, 95);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// �� �ѱ� ������
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	unsigned char *item2 = stbi_load("light.png", &Width, &Height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, item2);
	////�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, lightAlpha);

	glTexCoord2i(0, 0); glVertex2i(40, 95);
	glTexCoord2i(0, 1); glVertex2i(40, 80);
	glTexCoord2i(1, 1); glVertex2i(90, 80);
	glTexCoord2i(1, 0); glVertex2i(90, 95);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ���� ȭ��
void Draw_score() {
	glBindTexture(GL_TEXTURE_2D, texture[11]);
	int width, height, NrChannels;
	unsigned char *gameEnd = stbi_load("GameEnd.png", &width, &height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gameEnd);
	//�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[11]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2i(0, 0); glVertex2i(-70, 50);
	glTexCoord2i(0, 1); glVertex2i(-70, -10);
	glTexCoord2i(1, 1); glVertex2i(60, -10);
	glTexCoord2i(1, 0); glVertex2i(60, -50);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture[12]);
	unsigned char *score = stbi_load(howManycatchImage, &width, &height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, score);
	//�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[12]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2i(0, 0); glVertex2i(-50, -30);
	glTexCoord2i(0, 1); glVertex2i(-50, -70);
	glTexCoord2i(1, 1); glVertex2i(30, -70);
	glTexCoord2i(1, 0); glVertex2i(30, -30);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// ���
void Draw_Background() {
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	Background_img = stbi_load("���.png", &BackgroundWidth, &BackgroundHeight, &BackgroundNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, BackgroundWidth, BackgroundHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Background_img);
	//�ؽ�ó wrapping/filtering �ɼ� ����(���� ���ε��� �ؽ�ó ��ü�� ����)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(-200, -120); // ���� �Ʒ� ��ǥ
	glTexCoord2i(0, 1); glVertex2i(-200, 150); // ���� �� ��ǥ
	glTexCoord2i(1, 1); glVertex2i(200, 150); // ������ �� ��ǥ
	glTexCoord2i(1, 0); glVertex2i(200, -120); // ������ �Ʒ� ��ǥ
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// ���� ȭ�� �׸���
void Draw_difficulty() {
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("GameStart.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Game Start �κ�
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2i(0, 0); glVertex2i(-70, 50);
	glTexCoord2i(0, 1); glVertex2i(-70, -10);
	glTexCoord2i(1, 1); glVertex2i(60, -10);
	glTexCoord2i(1, 0); glVertex2i(60, 50);
	glEnd();
	glDisable(GL_TEXTURE_2D);



	glBindTexture(GL_TEXTURE_2D, texture[8]);
	unsigned char *data2 = stbi_load("high.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(-70, -20);
	glTexCoord2i(0, 1); glVertex2i(-70, -45);
	glTexCoord2i(1, 1); glVertex2i(-30, -45);
	glTexCoord2i(1, 0); glVertex2i(-30, -20);
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glBindTexture(GL_TEXTURE_2D, texture[9]);
	unsigned char *data3 = stbi_load("medium.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(-20, -20);
	glTexCoord2i(0, 1); glVertex2i(-20, -45);
	glTexCoord2i(1, 1); glVertex2i(20, -45);
	glTexCoord2i(1, 0); glVertex2i(20, -20);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture[10]);
	unsigned char *data4 = stbi_load("low.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// �̹��� �ؽ��� �κ�
	glBindTexture(GL_TEXTURE_2D, texture[10]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(30, -20);
	glTexCoord2i(0, 1); glVertex2i(30, -45);
	glTexCoord2i(1, 1); glVertex2i(70, -45);
	glTexCoord2i(1, 0); glVertex2i(70, -20);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}