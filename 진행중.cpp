#include <iostream>
#include <stdlib.h> 
#include <GL/glut.h> 
#include <GL/GL.h> 
#include <GL/GLU.h> 
#include <Leap.h>
using namespace std;

class MyListener : public Leap::Listener {
public:
	virtual void onConnect(const Leap::Controller &);
	virtual void onFrame(const Leap::Controller &);
	int R_S_P;
	void game(void);
	int random;
};

//
//GLubyte *LoadBmp(const char *Path, int *Width, int *Height)
//{
//	HANDLE hFile;
//	DWORD FileSize, dwRead;
//	BITMAPFILEHEADER *fh = NULL;
//	BITMAPINFOHEADER *ih;
//	BYTE *pRaster;
//
//	hFile = CreateFileA(Path, GENERIC_READ, 0, NULL,
//		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE) {
//		return NULL;
//	}
//
//	FileSize = GetFileSize(hFile, NULL);
//	fh = (BITMAPFILEHEADER *)malloc(FileSize);
//	ReadFile(hFile, fh, FileSize, &dwRead, NULL);
//	CloseHandle(hFile);
//
//	int len = FileSize - fh->bfOffBits;
//	pRaster = (GLubyte *)malloc(len);
//	memcpy(pRaster, (BYTE *)fh + fh->bfOffBits, len);
//
//	// RGB�� ������ �ٲ۴�.
//
//	for (BYTE *p = pRaster; p < pRaster + len - 3; p += 3) {
//		BYTE b = *p;
//		*p = *(p + 2);
//		*(p + 2) = b;
//	}
//	ih = (BITMAPINFOHEADER *)((PBYTE)fh + sizeof(BITMAPFILEHEADER));
//	*Width = ih->biWidth;
//	*Height = ih->biHeight;
//
//	free(fh);
//
//	return pRaster;
//}
//
//void DoDisplay()
//{
//	GLubyte *data;
//	int Width, Height;
//
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	data = LoadBmp("mogi.bmp", &Width, &Height);
//	if (data != NULL) {
//		glRasterPos2f(-0.5, -0.5);
//		glDrawPixels(Width, Height, GL_RGB, GL_UNSIGNED_BYTE, data);
//		free(data);
//	}
//	glFlush();
//}

//void MyDisplay() {
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	// GL���º��� ����, ������ ���۸� �ʱ�ȭ 
//	// �ʱ�ȭ �� ���� glutClearColor���� ���� �� 
//	glViewport(0, 0, 300, 300); glColor3f(1.0, 1.0, 1.0);
//	glBegin(GL_POLYGON);
//
//	// �Է¿�� �⺻���� 
//	glVertex2f(-0.5, -0.5);
//	glVertex2f(0.5, -0.5);
//	glVertex2f(0.5, 0.5);
//	glVertex2f(-0.5, 0.5);
//
//	glEnd(); glFlush();
//}


GLfloat leap_X, leap_Y; //����� ���� x y ��ǥ

// �簢�� ����
// # �簢�� ���� ���� (g_rectX, g_rectY)
// # �簢�� ������ �Ʒ��� (g_rectX + g_rectSize, g_rectY - g_rectSize)
GLfloat g_rectX = 0.0f;
GLfloat g_rectY = 0.0f;
GLfloat g_rectSize = 25.0f;

// �簢�� �̵� ����(�ӵ� �� ����)
// �ӵ� ���밪
GLfloat g_step = 2.0f;
// ���� �ӵ�
GLfloat g_xCurStep = g_step;
GLfloat g_yCurStep = g_step;

// Ŭ���� ���� ����
GLfloat g_clipBoxHeight = 200.0f;

// ���� Ŭ������ ���� ũ��
GLfloat g_clipHalfWidth = g_clipBoxHeight;
GLfloat g_clipHalfHeight = g_clipBoxHeight;

// �ð� ����
GLuint g_timeStep = 17u;

// ������� ����Ǿ����� Ȯ��
void MyListener::onConnect(const Leap::Controller &) {
	std::cout << "Connected." << std::endl;
}

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

	Leap::Vector leapPoint = fingers[1].stabilizedTipPosition();
	Leap::Vector normalizedPoint =  iBox.normalizePoint(leapPoint, false);
	float appX = normalizedPoint.x * 100;
	float appY = normalizedPoint.y * 100;

	cout << appX << ", " << appY << endl;
	leap_X = appX;
	leap_Y = appY;

	// ���� �νĵ��� �ʾ��� ��
	//if (!hands[0].isValid()) {
	//	std::cout << "no hands detected." << std::endl;
	//}
	//while (1) {
	//	if (fingers[0].isExtended() + fingers[1].isExtended() + fingers[2].isExtended() + fingers[3].isExtended() + fingers[4].isExtended() == 0) {
	//		printf("0��!");
	//		g_step = 0.1f;
	//	}
	//	else if (fingers[0].isExtended() + fingers[1].isExtended() + fingers[2].isExtended() + fingers[3].isExtended() + fingers[4].isExtended() == 1) {
	//		printf("1��!");
	//		g_step = 5.0f;
	//	}
	//}
}




// ������ �ð��� ȣ���
void TimerFunc(int value)
{
	// �簢���� �̵���Ų��.
	g_rectX += g_xCurStep;
	g_rectY += g_yCurStep;

	// # ��� �˻� : ��迡 �簢���� �ε����� ������ �ٲ۴�.
	// �簢�� �����̳� �������� Ŭ�������� �������
	// �¿� �̵� ���͸� �ٲ۴�.
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

	// �簢�� �����̳� �Ʒ����� Ŭ�������� �������
	// ���� �̵� ���͸� �ٲ۴�.
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

	// ����� �ٽ� �׸���.
	glutPostRedisplay();

	// �ٽ� Ÿ�ӽ��� ���Ŀ� ���� �Լ��� �Ҹ� �� �ֵ��� �����Ҵ�.
	glutTimerFunc(g_timeStep, TimerFunc, 1);
}

// �� �׸���
void RenderScene(void)
{
	// ȭ���� �����(�÷���)
	glClear(GL_COLOR_BUFFER_BIT);
	
	// ���� ������ ��������!
	glColor3f(1.0f, 0.0f, 0.0f);

	// *�簢�� �׸���~
	// �簢���� ���� ������ ������ �Ʒ����� ��� ����
	glRectf(g_rectX, g_rectY, g_rectX + g_rectSize, g_rectY - g_rectSize);

	// * ���� ���ۿ� �ĸ���۸� ��ü
	glutSwapBuffers();
}

// â ũ�� ����
void ChangeWindowSize(GLsizei width, GLsizei height)
{
	// ���̰� 0�̸� 0���� ������ ������ �����Ƿ�
	// 1�� �����Ͽ� ���� ó��
	if (height == 0)
		height = 1;

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

int main(int argc, char** argv)
{
	Leap::Controller controller;
	MyListener listener;
	controller.addListener(listener);

	glutInit(&argc, argv); // GLUT ������ �Լ� 
	glutInitDisplayMode(GLUT_RGB); // ������ �� �⺻�÷���带 RGB���� ���� 
	glutInitWindowSize(300, 300); // ������ ������ ���� 
	glutInitWindowPosition(0, 0); // ������ â ��ġ ���� 
	glutCreateWindow("OpenGL example");

	glClearColor(0.0, 0.0, 0.0, 1.0); // GL ���º��� ����, ������ ���İ��� 1�̸� ������ 0�̸� ���� 
	//glMatrixMode(GL_PROJECTION); // glLoadIdentity(); 
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	//glutDisplayFunc(MyDisplay); // GLUT �ݹ��Լ� ��� 
	//glutMainLoop(); // �̺�Ʈ ���� ���� 

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeWindowSize);

	// * Ÿ�ӽ��� ���Ŀ� Ÿ�̸� �Լ��� �Ҹ� �� �ֵ��� ����
	glutTimerFunc(g_timeStep, TimerFunc, 1);
	glutMainLoop();

	return 0;
}