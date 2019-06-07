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

//그리기 함수들
void Draw_mogi();
void Draw_HP();
void Draw_mogi_count();
void Draw_hand_point_Scissors(GLfloat x, GLfloat y);
void Draw_hand_point_Rock(GLfloat x, GLfloat y);
void Draw_blood_mogi(GLfloat x, GLfloat y);

Blood_mogi blood_mogi[100]; // 모기 잡기 가능횟수

char *comPortName = NULL;            //활성화된 comport값으로 수정된 값을 가질 변수
int connectionld = 0;            //연결된장치의 ID
int packetsRead = 0;            //데이터를 읽어 들이는 변수
int state = 0;               //연결된 상태 값을 받는 변수
int ATTSt = 0;               //Attention value의 상태변수
int ATTD = 0;               //Attention 데이터 값을 받는 변수
int i = 0;

// 모기 죽은 위치
GLfloat blood_X = 0.0f;
GLfloat blood_Y = 0.0f;

// 모기 투명도
GLfloat Alpha = 1.0f;

// 아이템 투명도
GLfloat ATTAlpha = 0.0f;
GLfloat recoverAlpha = 0.0f;
GLfloat lightAlpha = 0.0f;
GLfloat itemTime = 0;

//가위 그림
int BackgroundWidth, BackgroundHeight, BackgroundNrChannels;
unsigned char *Background_img;

// 잡은 모기 갯수 정보
const char *catchImage[6] = { "zero.png", "one.png", "two.png", "three.png","four.png","five.png" };
const char *howManycatchImage = catchImage[0];
int catchCount = 0;

// 모기 시체
int bloodWidth, bloodHeight, bloodNrChannels;
unsigned char *blood_mogi_img;

//주먹 그림
int rockWidth, rockHeight, rockNrChannels;
unsigned char *rock_img;

//가위 그림
int scissorsWidth, scissorsHeight, scissorsNrChannels;
unsigned char *scissors_img;


int finger_shape = 0;

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
GLfloat g_step = 0.0f;
// 현재 속도
GLfloat g_xCurStep = g_step;
GLfloat g_yCurStep = g_step;

// 클리핑 영역 높이
GLfloat g_clipBoxHeight = 200.0f;

// 현재 클립공간 절반 크기
GLfloat g_clipHalfWidth = g_clipBoxHeight;
GLfloat g_clipHalfHeight = g_clipBoxHeight;

GLuint g_timeStep = 17u; // 시간 간격! 값이 클수록 버벅이는 느낌
//GLubyte *pBytes; // 데이터를 가리킬 포인터
//GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
BITMAPINFO *info;
GLuint texture[12];
static GLuint Texture;

int Random_course = 0; // 객체가 랜덤하게 이동하기 위한 랜덤 값을 넣을 변수
int Random_speed = 0;
int Catch_flag = 0; // 손가락을 접었는지 확인하기 위한 변수
int blood_img = 0; // 모기가 죽었을 때 이미지를 바꾸기위해


				   
// *********************************** 립모션 부분 *********************************//

void MyListener::onConnect(const Leap::Controller &) { // 립모션이 연결되었는지 확인
	std::cout << "Connected." << std::endl;
}

void MyListener::onFrame(const Leap::Controller & controller) { // 립모션 작동
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

	float appX = normalizedPoint.x * 50; // 이 숫자값 수정하면서 해야할듯
	float appY = normalizedPoint.y * 50 - 7;

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
			Catch_flag = Scissors;
			//cout << Catch_flag << " 엄지 검지 !!! " << endl;
		}
		else if (fingers[0].isExtended() + fingers[1].isExtended() + fingers[2].isExtended() + fingers[3].isExtended() + fingers[4].isExtended() + fingers[5].isExtended() == 0) {
			Catch_flag = Rock;
			//cout << Catch_flag << " 묵!!! " << endl;
		}
	}
}
// *********************************** 립모션 부분 끝 *********************************//


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
void Draw_hand_point_Rock(GLfloat x, GLfloat y) {
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	rock_img = stbi_load("주먹1.png", &rockWidth, &rockHeight, &rockNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rockWidth, rockHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rock_img);
	//텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
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

// 립모션에서 손을 인식하여 손의 위치에 도형 생성
void Draw_hand_point_Scissors(GLfloat x, GLfloat y) {
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	scissors_img = stbi_load("집게손1.png", &scissorsWidth, &scissorsHeight, &scissorsNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scissorsWidth, scissorsHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, scissors_img);
	//텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
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
	glTexCoord2i(0, 0); glVertex2i(110, 95);
	glTexCoord2i(0, 1); glVertex2i(110, 80);
	glTexCoord2i(1, 1); glVertex2i(125, 80);
	glTexCoord2i(1, 0); glVertex2i(125, 95);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// hp 게이지
	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(125, 90, HP, 85);
	glFlush();
}

// 잡은 모기 갯수 그리기
void Draw_mogi_count() {
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	int catchWidth, catchHeight, catchNrChannels;
	unsigned char *howManyImage;

	howManyImage = stbi_load(howManycatchImage, &catchWidth, &catchHeight, &catchNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, catchWidth, catchHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, howManyImage);

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


// 아이템 부분 그리기
void Draw_Item() {
	// 집중도
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	int Width, Height, NrChannels;
	unsigned char *attention = stbi_load("attention.png", &Width, &Height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, attention);
	////텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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

	// HP 회복 아이템
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	unsigned char *item1 = stbi_load("recover.png", &Width, &Height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, item1);
	////텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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

	// 불 켜기 아이템
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	unsigned char *item2 = stbi_load("light.png", &Width, &Height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, item2);
	////텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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

// 점수 화면
void Draw_score() {
	glBindTexture(GL_TEXTURE_2D, texture[11]);
	int width, height, NrChannels;
	unsigned char *gameEnd = stbi_load("GameEnd.png", &width, &height, &NrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gameEnd);
	//텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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
	//텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
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


// 배경
void Draw_Background() {
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	Background_img = stbi_load("배경.png", &BackgroundWidth, &BackgroundHeight, &BackgroundNrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, BackgroundWidth, BackgroundHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Background_img);
	//텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// 이미지 텍스쳐 부분
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(-200, -120); // 왼쪽 아래 좌표
	glTexCoord2i(0, 1); glVertex2i(-200, 150); // 왼쪽 위 좌표
	glTexCoord2i(1, 1); glVertex2i(200, 150); // 오른쪽 위 좌표
	glTexCoord2i(1, 0); glVertex2i(200, -120); // 오른쪽 아래 좌표
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// 시작 화면 그리기
void Draw_difficulty() {
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("GameStart.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Game Start 부분
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

	// 이미지 텍스쳐 부분
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

	// 이미지 텍스쳐 부분
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

	// 이미지 텍스쳐 부분
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