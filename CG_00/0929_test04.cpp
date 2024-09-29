#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

// Ŭ���̾�Ʈ ũ��
#define clientWidth 800
#define clientHeight 600

// Ŭ���̾�Ʈ ����
GLclampf clientRed = 0.2f;
GLclampf clientGreen = 0.2f;
GLclampf clientBlue = 0.2f;

// ���� �Ǽ���(min ~ max) ��ȯ �Լ�
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister ����
float generateRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dis(min, max);	// ���ڷ� ���� ����
	return dis(gen);
}

//��ǥ ��ȯ �Լ�
int GL_to_Win_X(float x)
{
	return (x + 1) * (clientWidth / 2.0f);  // 2.0f�� �Ǽ� ������
}
int GL_to_Win_Y(float y)
{
	return (1 - y) * (clientHeight / 2.0f);  // 2.0f�� �Ǽ� ������
}
float Win_to_GL_X(int x)
{
	return (x / (float)clientWidth) * 2 - 1;  // ���� ������ ����
}
float Win_to_GL_Y(int y)
{
	return 1 - (y / (float)clientHeight) * 2;  // ���� ������ ����
}

// �簢�� ����ü (�迭)
#define MAX_RECT 5
typedef struct RECTANGLES
{
	float midX;
	float midY;
	float r, g, b;
	float width;
	float height;
}RECTS;
int rect_count = 0;
RECTS rt[MAX_RECT];

// �簢�� ����� �� �ʱ�ȭ (�ִ� 10)
void makeRect(float mx, float my)
{
	if (rect_count < MAX_RECT)
	{
		rt[rect_count].midX = mx;
		rt[rect_count].midY = my;
		rt[rect_count].r = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].g = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].b = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].width = 0.15f;
		rt[rect_count].height = 0.2f;
		rect_count++;
	}
	else
	{
		std::cout << "�簢�� ���� �ʰ�" << std::endl;
	}
}

// ���� �����ϴ� �簢�� ��� ���
void drawRect()
{
	for (int i = 0; i < rect_count; i++)
	{
		glColor3f(rt[i].r, rt[i].g, rt[i].b);
		glRectf(rt[i].midX - (rt[i].width / 2), rt[i].midY - (rt[i].height / 2),
			rt[i].midX + (rt[i].width / 2), rt[i].midY + (rt[i].height / 2));
	}
}

// GL �̺�Ʈ �Լ�
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);								//--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0);						//--- �������� ��ġ ����
	glutInitWindowSize(clientWidth, clientHeight);		//--- �������� ũ�� ����
	glutCreateWindow("test 04");						//--- ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene);					// ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape);					// �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard);					// Ű���� �Է� �ݹ��Լ� ����
	glutMouseFunc(Mouse);						// ���콺 �Է�
	glutMainLoop();								// �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(clientRed, clientGreen, clientBlue, 1.0f);			//--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT);									//--- ������ ������ ��ü�� ĥ�ϱ�

	drawRect();	//10���� �簢��
	glutSwapBuffers();												//--- ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':		// ���α׷� ����
		glutLeaveMainLoop(); // OpenGL ���� ���� ����
		break;
	}
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	float mX = Win_to_GL_X(x);
	float mY = Win_to_GL_Y(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		makeRect(mX, mY);
	}
	glutPostRedisplay(); // refresh
}
