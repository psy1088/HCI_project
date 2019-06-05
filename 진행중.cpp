#include <iostream>
#include <stdlib.h> 
#include <GL/glut.h> 
#include <GL/GL.h> 
#include <GL/GLU.h> 
#include <Leap.h>
using namespace std;


GLubyte *LoadBmp(const char *Path, int *Width, int *Height)
{
	HANDLE hFile;
	DWORD FileSize, dwRead;
	BITMAPFILEHEADER *fh = NULL;
	BITMAPINFOHEADER *ih;
	BYTE *pRaster;

	hFile = CreateFileA(Path, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	FileSize = GetFileSize(hFile, NULL);
	fh = (BITMAPFILEHEADER *)malloc(FileSize);
	ReadFile(hFile, fh, FileSize, &dwRead, NULL);
	CloseHandle(hFile);

	int len = FileSize - fh->bfOffBits;
	pRaster = (GLubyte *)malloc(len);
	memcpy(pRaster, (BYTE *)fh + fh->bfOffBits, len);

	// RGB로 순서를 바꾼다.

	for (BYTE *p = pRaster; p < pRaster + len - 3; p += 3) {
		BYTE b = *p;
		*p = *(p + 2);
		*(p + 2) = b;
	}
	ih = (BITMAPINFOHEADER *)((PBYTE)fh + sizeof(BITMAPFILEHEADER));
	*Width = ih->biWidth;
	*Height = ih->biHeight;

	free(fh);

	return pRaster;
}

void DoDisplay()
{
	GLubyte *data;
	int Width, Height;

	glClear(GL_COLOR_BUFFER_BIT);

	data = LoadBmp("mogi.bmp", &Width, &Height);
	if (data != NULL) {
		glRasterPos2f(-0.5, -0.5);
		glDrawPixels(Width, Height, GL_RGB, GL_UNSIGNED_BYTE, data);
		free(data);
	}
	glFlush();
}

void MyDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);
	// GL상태변수 설정, 프레임 버퍼를 초기화 
	// 초기화 될 색은 glutClearColor에서 사용된 색 
	glViewport(0, 0, 300, 300); glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);

	// 입력요소 기본정의 
	glVertex2f(-0.5, -0.5);
	glVertex2f(0.5, -0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(-0.5, 0.5);

	glEnd(); glFlush();
}


// 사각형 정보
// # 사각형 왼쪽 윗점 (g_rectX, g_rectY)
// # 사각형 오른쪽 아래점 (g_rectX + g_rectSize, g_rectY - g_rectSize)
GLfloat g_rectX = 0.0f;
GLfloat g_rectY = 0.0f;
GLfloat g_rectSize = 25.0f;

// 사각형 이동 벡터(속도 및 방향)
// 속도 절대값
GLfloat g_step = 3.0f;
// 현재 속도
GLfloat g_xCurStep = g_step;
GLfloat g_yCurStep = g_step;

// 클리핑 영역 높이
GLfloat g_clipBoxHeight = 200.0f;

// 현재 클립공간 절반 크기
GLfloat g_clipHalfWidth = g_clipBoxHeight;
GLfloat g_clipHalfHeight = g_clipBoxHeight;

// 시간 간격
GLuint g_timeStep = 17u;

// 지정한 시간뒤 호출됨
void TimerFunc(int value)
{
	// 사각형을 이동시킨다.
	g_rectX += g_xCurStep;
	g_rectY += g_yCurStep;

	// # 경계 검사 : 경계에 사각형이 부딪히면 방향을 바꾼다.
	// 사각형 왼쪽이나 오른쪽이 클립공간을 벗어났으면
	// 좌우 이동 벡터를 바꾼다.
	if (g_rectX < -g_clipHalfWidth)
	{
		g_rectX = -g_clipHalfWidth;
		g_xCurStep = g_step;
	}
	else if (g_rectX > g_clipHalfWidth - g_rectSize)
	{
		g_rectX = g_clipHalfWidth - g_rectSize;
		g_xCurStep = -g_step;
	}

	// 사각형 윗쪽이나 아래쪽이 클립공간을 벗어났으면
	// 상하 이동 벡터를 바꾼다.
	if (g_rectY < -g_clipHalfHeight + g_rectSize)
	{
		g_rectY = -g_clipHalfHeight + g_rectSize;
		g_yCurStep = g_step;
	}
	else if (g_rectY > g_clipHalfHeight)
	{
		g_rectY = g_clipHalfHeight;
		g_yCurStep = -g_step;
	}

	// 장면을 다시 그린다.
	glutPostRedisplay();

	// 다시 타임스탭 이후에 현재 함수가 불릴 수 있도록 설정할다.
	glutTimerFunc(g_timeStep, TimerFunc, 1);
}

// 씬 그리기
void RenderScene(void)
{
	// 화면을 지우기(컬러만)
	glClear(GL_COLOR_BUFFER_BIT);
	
	// 현재 색상을 빨강으로!
	glColor3f(1.0f, 0.0f, 0.0f);

	// *사각형 그리기~
	// 사각형의 왼쪽 위점과 오른쪽 아래점은 계속 변함
	glRectf(g_rectX, g_rectY, g_rectX + g_rectSize, g_rectY - g_rectSize);

	// * 전면 버퍼와 후면버퍼를 교체
	glutSwapBuffers();
}

// 창 크기 변경
void ChangeWindowSize(GLsizei width, GLsizei height)
{
	// 높이가 0이면 0으로 나누는 오류에 빠지므로
	// 1로 보정하여 예외 처리
	if (height == 0)
		height = 1;

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

int main(int argc, char** argv)
{
	Leap::Controller controller;

	glutInit(&argc, argv); // GLUT 윈도우 함수 
	glutInitDisplayMode(GLUT_RGB); // 윈도우 의 기본컬러모드를 RGB모드로 설정 
	glutInitWindowSize(700, 700); // 윈도우 사이즈 설정 
	glutInitWindowPosition(0, 0); // 윈도우 창 위치 설정 
	glutCreateWindow("OpenGL example");

	glClearColor(0.0, 0.0, 0.0, 1.0); // GL 상태변수 설정, 마지막 알파값은 1이면 불투명 0이면 투명 
	//glMatrixMode(GL_PROJECTION); // glLoadIdentity(); 
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	//glutDisplayFunc(MyDisplay); // GLUT 콜백함수 등록 
	//glutMainLoop(); // 이벤트 루프 진입 

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeWindowSize);

	// * 타임스템 이후에 타이머 함수가 불릴 수 있도록 설정
	glutTimerFunc(g_timeStep, TimerFunc, 1);
	glutMainLoop();


	return 0;
}