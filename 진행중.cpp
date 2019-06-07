#include "헤더.h"

// 씬 그리기
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT); // 화면을 지우기(컬러만)

	//glColor3f(1.0f, 1.0f, 1.0f); // 현재 색상

	Draw_mogi();
	Draw_HP();
	Draw_mogi_count();
	Draw_hand_point(hand_X, hand_Y);

	if (blood_img == 1) {
		Draw_blood_mogi(blood_mogi[i].x, blood_mogi[i].y);
		i++;
	}

	glutSwapBuffers(); 	// 전면 버퍼와 후면버퍼를 교체
}

// 지정한 시간뒤 호출됨
void TimerFunc(int value)
{
	blood_img = 0;

	if (HP == 125) exit(1);

	srand((unsigned int)time(0));
	random = rand() % 8;

	//packetsRead = TG_ReadPackets(connectionld, -1);//연결된 기기에서 받은 데이터를 비트의 열로 변환하여 직렬로 전송합니다.

	//if ((packetsRead > 0)) {
	//	ATTD = TG_GetValue(connectionld, TG_DATA_ATTENTION); //ATTD 값에 연결된 기기의 ID, 변환할 데이터형태로 구성된 값을 받습니다.
	// //   MEDD = TG_GetValue(connectionld, TG_DATA_MEDITATION);//MEDD 값에 연결된 기기의 ID, 변환할 데이터형태로 구성된 값을 받습니다.
	//	printf("ATT =%3d\n", ATTD);

	//	if (ATTD == 0) Alpha = 0.0f;
	//	else if (ATTD < 30) Alpha = 0.2f;
	//	else if (ATTD < 50) Alpha = 0.5f;
	//	else if (ATTD < 70) Alpha = 0.7f;
	//	else Alpha = 1.0f;
	//}

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

	// 손의 좌표를 나타내는 물체가, 모기의 좌표 범위 안에 있을 때~
	if (g_rectX <= hand_X && hand_X <= g_rectX + g_rectSize) {
		if (g_rectY - g_rectSize <= hand_Y && hand_Y <= g_rectY) {

			if (Catch_flag == 2) { // 손가락을 다 접었다면~
				cout << "@@@@@@@@@@@@@@@@ 잡았죠오 " << endl;
				PlaySound(TEXT("catch.wav"), NULL, SND_ASYNC); // 잡았다는 소리를 출력
				PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_NOSTOP); // 모기 소리를 출력
				blood_mogi[i].x = g_rectX + g_rectSize/2.0; // 잡았을 때의 모기 좌표를 저장
				blood_mogi[i].y = g_rectY - g_rectSize/2.0;
				blood_img = 1; 

			}
		}
	}
	// HP 감소
	if (HP > 125) HP = HP - 0.01;

	// 장면을 다시 그린다.
	glutPostRedisplay();

	// 다시 타임스탭 이후에 현재 함수가 불릴 수 있도록 설정한다.
	glutTimerFunc(g_timeStep, TimerFunc, 1);
}


int main(int argc, char** argv)
{
	//// 마인드웨이브 연결부
	//connectionld = TG_GetNewConnectionId();//connectionId 값에 연결될 기기에 관한 새로운 ID를 부여한다
	//comPortName = (char*)"\\\\.\\COM3";
	//state = TG_Connect(connectionld, comPortName, TG_BAUD_57600, TG_STREAM_PACKETS);
	//if (!state) cout << "connect success!" << endl;
	//else cout << "connect fail." << endl;

	PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP); // 모기 소리를 출력

	//립모션 연결부
	Leap::Controller controller;
	MyListener listener;
	controller.addListener(listener);

	glutInit(&argc, argv); // GLUT 윈도우 함수 
	glutInitDisplayMode(GLUT_RGB); // 윈도우 의 기본컬러모드를 RGB모드로 설정 
	glutInitWindowSize(1000, 500); // 윈도우 사이즈 설정 
	glutInitWindowPosition(0, 0); // 윈도우 창 위치 설정 
	glutCreateWindow("Catch me if you can");

	glClearColor(0.0, 0.0, 0.0, 1.0); // GL 상태변수 설정, 마지막 알파값은 1이면 불투명 0이면 투명 

	glutDisplayFunc(RenderScene); // GLUT 콜백함수 등록 
	glutReshapeFunc(ChangeWindowSize);

	// * 타임스템 이후에 타이머 함수가 불릴 수 있도록 설정
	glutTimerFunc(g_timeStep, TimerFunc, 1);

	glutMainLoop(); // 이벤트 루프 진입 

	cout << "잡은 모기의 갯수 : " << catchCount << endl;

	return 0;
}