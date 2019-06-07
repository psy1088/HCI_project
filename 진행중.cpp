#include "pch.h"
#include "���.h"

// 0: ���� ȭ��, 1: ���� ȭ��, 2: ���� ����
int ch = 0;
//�ʱ�ȭ��
void FirstScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT); // ȭ���� �����(�÷���)

	Draw_difficulty();
	if (hand_X >= -70 && hand_X <= -30 && hand_Y > -20 && hand_Y < 45) {
		if (Catch_flag == 2) {
			g_step = 2.0f;
			g_xCurStep = g_step;
			g_yCurStep = g_step;
			ch = 1;
		}
	}
	else if (hand_X >= -20 && hand_X <= 20 && hand_Y > -20 && hand_Y < 45) {
		if (Catch_flag == 2) {
			g_step = 1.5f;
			g_xCurStep = g_step;
			g_yCurStep = g_step;
			ch = 1;
		}
	}
	else if (hand_X >= 30 && hand_X <= 70 && hand_Y > -20 && hand_Y < 45) {
		if (Catch_flag == 2) {
			g_step = 1.0f;
			g_xCurStep = g_step;
			g_yCurStep = g_step;
			ch = 1;
		}
	}

	//glutSwapBuffers();    // ���� ���ۿ� �ĸ���۸� ��ü
}
// �� �׸���
void RenderScene(void)
{
	if (ch == 0) {
		glClear(GL_COLOR_BUFFER_BIT);
		FirstScene();
		Draw_hand_point_Rock(hand_X, hand_Y);
	}
	else if (ch == 1) {
		glClear(GL_COLOR_BUFFER_BIT); // ȭ���� �����(�÷���)

		//glColor3f(1.0f, 1.0f, 1.0f); // ���� ����
		Draw_Background();
		if (HP > 125.0f) Draw_mogi();
		else ch = 2;

		Draw_Item();
		Draw_HP();
		Draw_mogi_count();

		if (blood_img == 1) { // ��⸦ �������, �ش� ��ġ�� ��� ��ü �̹��� �׸���
			Draw_blood_mogi(blood_mogi[i].x, blood_mogi[i].y);
			//i++;
		}
	}
	else if (ch == 2) {
		glClear(GL_COLOR_BUFFER_BIT);
		Draw_score();
	}

	// �ո���� ��������, �ָ������� ���� �׸� �ٸ���~
	if (Catch_flag == Scissors) {
		Draw_hand_point_Scissors(hand_X, hand_Y);
	}
	else if (Catch_flag == Rock) {
		Draw_hand_point_Rock(hand_X, hand_Y);
	}

	glutSwapBuffers();    // ���� ���ۿ� �ĸ���۸� ��ü
}

// ������ �ð��� ȣ���
void TimerFunc(int value)
{
	blood_img = 0;

	srand((unsigned int)time(0));
	Random_course = rand() % 8;

	packetsRead = TG_ReadPackets(connectionld, -1);//����� ��⿡�� ���� �����͸� ��Ʈ�� ���� ��ȯ�Ͽ� ���ķ� �����մϴ�.

	if (itemTime <= 0) {
		if ((packetsRead > 0)) {

			ATTAlpha = 0.0f; recoverAlpha = 0.0f; lightAlpha = 0.0f;

			ATTD = TG_GetValue(connectionld, TG_DATA_ATTENTION); //ATTD ���� ����� ����� ID, ��ȯ�� ���������·� ������ ���� �޽��ϴ�.
			printf("ATT =%3d\n", ATTD);

			if (ATTD == 0) Alpha = 0.0f;
			else if (ATTD < 30) Alpha = 0.2f;
			else if (ATTD < 50) Alpha = 0.5f;
			else if (ATTD < 70) Alpha = 0.7f;
			else Alpha = 1.0f;

			if (ATTD > 50) {
				itemTime = 20;
				int randomItem = rand() % 2;
				if (randomItem) { recoverAlpha = 1.0f; lightAlpha = 0.0f; }
				else { recoverAlpha = 0.0f; lightAlpha = 1.0f; }
			}
		}
	}
	else {

		ATTAlpha = 1.0f;
		if (recoverAlpha == 1.0f) {
			if (HP < 190) HP = HP + 0.3f;
		}
		if (lightAlpha == 1.0f) {
			Alpha = 1.0f;
		}
		itemTime = itemTime - 0.5f;
	}

	// �����ϰ� ��⸦ �̵���Ų��.
	switch (Random_course) {
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

	// ���� ��ǥ�� ��Ÿ���� ��ü��, ����� ��ǥ ���� �ȿ� ���� ��~
	if (g_rectX <= hand_X && hand_X <= g_rectX + g_rectSize) {
		if (g_rectY - g_rectSize <= hand_Y && hand_Y <= g_rectY) {
			if (Catch_flag == Rock) { // �հ����� �� �����ٸ�~
				cout << "@@@@@@@@@@@@@@@@ ����ҿ� " << endl;
				PlaySound(TEXT("Slap.wav"), NULL, 0); // ��Ҵٴ� �Ҹ��� ���
				PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_NOSTOP | SND_LOOP); // ��� �Ҹ��� ���
				blood_mogi[i].x = g_rectX + g_rectSize / 2.0; // ����� ���� ��� ��ǥ�� ����
				blood_mogi[i].y = g_rectY - g_rectSize / 2.0;
				blood_img = 1;
				catchCount++;
				howManycatchImage = catchImage[catchCount];
			}
		}
	}
	// HP ����
	if (HP > 125.0f) HP = HP - 0.1f;

	// ����� �ٽ� �׸���.
	glutPostRedisplay();

	// �ٽ� Ÿ�ӽ��� ���Ŀ� ���� �Լ��� �Ҹ� �� �ֵ��� �����Ѵ�.
	glutTimerFunc(g_timeStep, TimerFunc, 1);
}


int main(int argc, char** argv)
{
	// ���ε���̺� �����
	connectionld = TG_GetNewConnectionId();//connectionId ���� ����� ��⿡ ���� ���ο� ID�� �ο��Ѵ�
	comPortName = (char*)"\\\\.\\COM3";
	state = TG_Connect(connectionld, comPortName, TG_BAUD_57600, TG_STREAM_PACKETS);
	if (!state) cout << "connect success!" << endl;
	else cout << "connect fail." << endl;

	PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP); // ��� �Ҹ��� ���

	//����� �����
	Leap::Controller controller;
	MyListener listener;
	controller.addListener(listener);

	glutInit(&argc, argv); // GLUT ������ �Լ� 
	glutInitDisplayMode(GLUT_RGB); // ������ �� �⺻�÷���带 RGB���� ���� 
	glutInitWindowSize(1000, 500); // ������ ������ ���� 
	glutInitWindowPosition(0, 0); // ������ â ��ġ ���� 
	glutCreateWindow("Catch me if you can");

	glClearColor(0.0, 0.0, 0.0, 1.0); // GL ���º��� ����, ������ ���İ��� 1�̸� ������ 0�̸� ���� 

	glutDisplayFunc(RenderScene); // GLUT �ݹ��Լ� ��� 
	glutReshapeFunc(ChangeWindowSize);

	// * Ÿ�ӽ��� ���Ŀ� Ÿ�̸� �Լ��� �Ҹ� �� �ֵ��� ����
	glutTimerFunc(g_timeStep, TimerFunc, 1);

	glutMainLoop(); // �̺�Ʈ ���� ���� 

	cout << "���� ����� ���� : " << catchCount << endl;

	return 0;
}