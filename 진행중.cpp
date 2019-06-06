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
GLubyte *pBytes; // 데이터를 가리킬 포인터
//GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
BITMAPINFO *info;
GLuint texture[1];
static GLuint Texture;

int random = 0; // 객체가 랜덤하게 이동하기 위한 랜덤 값을 넣을 변수
int Catch_flag = 0; // 손가락을 접었는지 확인하기 위한 변수


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

	glRectf(hand_X, hand_Y, hand_X + g_rectSize, hand_Y - g_rectSize);

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

// 립모션에서 손을 인식하여 손의 위치에 도형 생성
void pointScene(GLfloat x, GLfloat y) {
	glBegin(GL_POLYGON);
	glVertex2f(x - 1, y - 1);
	glVertex2f(x - 1, y + 1);
	glVertex2f(x + 1, y - 1);
	glVertex2f(x + 1, y + 1);
	glEnd();
	glFinish();
}

// 씬 그리기
void RenderScene(void)
{
	// 화면을 지우기(컬러만)
	glClear(GL_COLOR_BUFFER_BIT);

	// 현재 색상을 빨강으로!
	glColor3f(1.0f, 1.0f, 1.0f);


	//// 이미지 텍스쳐 부분
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("mogi_turn.png", &width, &height, &nrChannels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	////텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);


	// *사각형 그리기~
	// 사각형의 왼쪽 위점과 오른쪽 아래점은 계속 변함
	glRectf(g_rectX, g_rectY, g_rectX + g_rectSize, g_rectY - g_rectSize);

	// 이미지 텍스쳐 부분
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
	// * 전면 버퍼와 후면버퍼를 교체
	glutSwapBuffers();
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

// 지정한 시간뒤 호출됨
void TimerFunc(int value)
{
	srand((unsigned int)time(0));
	random = rand() % 8;

	// 랜덤하게 물체를 이동시킨다.
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

	// # 경계 검사 : 경계에 사각형이 부딪히면 방향을 바꾼다.
	// 사각형 왼쪽이나 오른쪽이 클립공간을 벗어났으면
	// 좌우 이동 벡터를 바꾼다.
	if (g_rectX < -g_clipHalfWidth) {
		g_rectX = -g_clipHalfWidth;
		g_xCurStep = g_step;
	}
	else if (g_rectX > g_clipHalfWidth - g_rectSize) {
		g_rectX = g_clipHalfWidth - g_rectSize;
		g_xCurStep = -g_step;
	}

	// 사각형 윗쪽이나 아래쪽이 클립공간을 벗어났으면
	// 상하 이동 벡터를 바꾼다.
	if (g_rectY < -g_clipHalfHeight + g_rectSize) {
		g_rectY = -g_clipHalfHeight + g_rectSize;
		g_yCurStep = g_step;
	}
	else if (g_rectY > g_clipHalfHeight) {
		g_rectY = g_clipHalfHeight;
		g_yCurStep = -g_step;
	}

	// 손의 좌표를 나타내는 물체가, 모기의 좌표 범위 안에 있으면~

		if (g_rectX <= hand_X && hand_X <= g_rectX + g_rectSize) {
			if (g_rectY - g_rectSize <= hand_Y && hand_Y <= g_rectY) {
				if (Catch_flag == 2) {
					cout << "@@@@@@@@@@@@@@@@ 잡았죠오 " << endl;
					PlaySound(TEXT("catch.wav"), NULL,0); // 모기 소리를 출력
					PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_NOSTOP); // 모기 소리를 출력
				}
			}
		}

	// 장면을 다시 그린다.
	glutPostRedisplay();

	// 다시 타임스탭 이후에 현재 함수가 불릴 수 있도록 설정할다.
	glutTimerFunc(g_timeStep, TimerFunc, 1);
}

int main(int argc, char** argv)
{
	PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP); // 모기 소리를 출력

	Leap::Controller controller;
	MyListener listener;
	controller.addListener(listener);

	glutInit(&argc, argv); // GLUT 윈도우 함수 
	glutInitDisplayMode(GLUT_RGB); // 윈도우 의 기본컬러모드를 RGB모드로 설정 
	glutInitWindowSize(1000, 500); // 윈도우 사이즈 설정 
	glutInitWindowPosition(0, 0); // 윈도우 창 위치 설정 
	glutCreateWindow("Catch me if you can");

	glClearColor(0.0, 0.0, 0.0, 1.0); // GL 상태변수 설정, 마지막 알파값은 1이면 불투명 0이면 투명 
	//glMatrixMode(GL_PROJECTION); // glLoadIdentity(); 
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glutDisplayFunc(RenderScene); // GLUT 콜백함수 등록 
	glutReshapeFunc(ChangeWindowSize);

	// * 타임스템 이후에 타이머 함수가 불릴 수 있도록 설정
	glutTimerFunc(g_timeStep, TimerFunc, 1);

	glutMainLoop(); // 이벤트 루프 진입 

	return 0;
}