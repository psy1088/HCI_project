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

//그리기 함수들
void Draw_mogi();
void Draw_HP();
void Draw_mogi_count();
void Draw_hand_point(GLfloat x, GLfloat y);
void Draw_blood_mogi(GLfloat x, GLfloat y);

Blood_mogi blood_mogi[100]; // 모기 잡기 가능횟수



char *comPortName = NULL;            //활성화된 comport값으로 수정된 값을 가질 변수
int connectionld = 0;            //연결된장치의 ID
int packetsRead = 0;            //데이터를 읽어 들이는 변수
int state = 0;               //연결된 상태 값을 받는 변수
int ATTSt = 0;               //Attention value의 상태변수
int MEDSt = 0;                 //Meditation value의 상태변수
int ATTD = 0;               //Attention 데이터 값을 받는 변수
int MEDD = 0;               //Meditation 데이터 값을 받는 변수
int i = 0;
// 모기 죽은 위치
GLfloat blood_X = 0.0f;
GLfloat blood_Y = 0.0f;

// 모기 투명도
GLfloat Alpha = 1.0f;

// 잡은 모기 갯수 정보
int catchWidth, catchHeight, catchNrChannels;
unsigned char *howManyImage;
int catchCount = 0;

// 모기 시체
int bloodWidth, bloodHeight, bloodNrChannels;
unsigned char *blood_mogi_img;

// HP 정보
GLfloat HP = 190;
LPCWSTR hpStr = TEXT("HP");

//립모션 손의 x y 좌표
GLfloat hand_X = 0.0f;
GLfloat hand_Y = 0.0f;


// 사각형 정보
// # 사각형 왼쪽 윗점 (g_rectX, g_rectY)
// # 사각형 오른쪽 아래점 (g_rectX + g_rectSize, g_rectY - g_rectSize)
GLfloat g_rectX = 0.0f;
GLfloat g_rectY = 0.0f;
GLfloat g_rectSize = 10.0f;

// 사각형 이동 벡터(속도 및 방향)
// 속도 절대값
GLfloat g_step = 0.3f;
// 현재 속도
GLfloat g_xCurStep = g_step;
GLfloat g_yCurStep = g_step;

// 클리핑 영역 높이
GLfloat g_clipBoxHeight = 200.0f;

// 현재 클립공간 절반 크기
GLfloat g_clipHalfWidth = g_clipBoxHeight;
GLfloat g_clipHalfHeight = g_clipBoxHeight;

GLuint g_timeStep = 17u; // 시간 간격! 값이 클수록 버벅이는 느낌
GLubyte *pBytes; // 데이터를 가리킬 포인터
//GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
BITMAPINFO *info;
GLuint texture[10];
static GLuint Texture;

int random = 0; // 객체가 랜덤하게 이동하기 위한 랜덤 값을 넣을 변수
int Catch_flag = 0; // 손가락을 접었는지 확인하기 위한 변수
int blood_img = 0; // 모기가 죽었을 때 이미지를 바꾸기위해


// 립모션이 연결되었는지 확인
void MyListener::onConnect(const Leap::Controller &) {
	std::cout << "Connected." << std::endl;
}

// 립모션 작동
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
	Leap::Vector leapPoint = fingers[1].stabilizedTipPosition(); // 이것도 핑거 인덱스 조절?
	Leap::Vector normalizedPoint = iBox.normalizePoint(leapPoint, false);

	float appX = normalizedPoint.x * 50; // 이 숫자값 수정하면서 해야할듯
	float appY = normalizedPoint.y * 50;

	//cout << appX << ", " << appY << endl;
	hand_X = appX * 3;
	hand_Y = appY * 4;

	//손이 인식되지 않았을 시
	if (!hands[0].isValid()) {
		//	cout << "no hands detected." << endl;
	}
	else {
		cout << "    " << endl;
		if (fingers[0].isExtended() == 1 && fingers[1].isExtended() == 1) {  // 엄지와 검지가 둘 다 펴져있으면~
			Catch_flag = 1;
			//cout << Catch_flag << " 엄지 검지 !!! " << endl;
		}
		else if (fingers[0].isExtended() + fingers[1].isExtended() + fingers[2].isExtended() + fingers[3].isExtended() + fingers[4].isExtended() + fingers[5].isExtended() == 0) {
			Catch_flag = 2;
			//cout << Catch_flag << " 묵!!! " << endl;
		}
	}
}

// 창 크기 변경
void ChangeWindowSize(GLsizei width, GLsizei height)
{
	// 높이가 0이면 0으로 나누는 오류에 빠지므로
	// 1로 보정하여 예외 처리
	if (height == 0) height = 1;

	// 투영 행렬 설정 모드로 변경
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 클리핑 박스 영역 설정(투영 행렬 설정)
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	g_clipHalfHeight = g_clipBoxHeight * 0.5f;
	g_clipHalfWidth = g_clipHalfHeight * aspect;
	glOrtho(-g_clipHalfWidth, g_clipHalfWidth,
		-g_clipHalfHeight, g_clipHalfHeight,
		1.0f, -1.0f);

	// 뷰 포트(실제 보여지는 위치/크기) 설정
	glViewport(0, 0, width, height);
}


// 립모션에서 손을 인식하여 손의 위치에 도형 생성
void Draw_hand_point(GLfloat x, GLfloat y) {
	glBegin(GL_POLYGON);
	glVertex2f(x - 1, y - 1);
	glVertex2f(x - 1, y + 1);
	glVertex2f(x + 1, y - 1);
	glVertex2f(x + 1, y + 1);
	glEnd();
	glFinish();
}

// 모기 그림
void Draw_mogi() {
	/*glGenTextures(3, texture);*/
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("mogi_turn.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	////텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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
	//glClear(GL_COLOR_BUFFER_BIT); // GL 상태변수 설정, 마지막 알파값은 1이면 불투명 0이면 투명 
}

// HP바 그리기
void Draw_HP() {
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	int hpWidth, hpHeight, hpNrChannels;
	unsigned char *hpData = stbi_load("hpName.png", &hpWidth, &hpHeight, &hpNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, hpWidth, hpHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, hpData);
	////텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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

	// hp 게이지
	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(125, 95, HP, 90);
	glFlush();
}

// 잡은 모기 갯수 그리기
void Draw_mogi_count() {
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	howManyImage = stbi_load("one.png", &catchWidth, &catchHeight, &catchNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, catchWidth, catchHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, howManyImage);
	//////텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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

// 모기 시체 그리기
void Draw_blood_mogi(GLfloat x, GLfloat y) {
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	blood_mogi_img = stbi_load("blood.png", &bloodWidth, &bloodHeight, &bloodNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bloodWidth, bloodHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, blood_mogi_img);
	//텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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

