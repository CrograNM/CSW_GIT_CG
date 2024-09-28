#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

#define clientWidth 800
#define clientHeight 600

// Ŭ���̾�Ʈ ����
GLclampf clientRed = 1.0f;
GLclampf clientGreen = 1.0f;
GLclampf clientBlue = 1.0f;

// ���� �Ǽ���(0.0f ~ 1.0f) ��ȯ �Լ�
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister ����
float generateRandomFloat()
{
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);	// (0.0f ~ 1.0f) ���� ����
	return dis(gen);
}

// GL �̺�Ʈ �Լ�
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);

//��ǥ��ȯ �Լ���
int GL_to_Win_X(float x);
int GL_to_Win_Y(float y);
float Win_to_GL_X(int x);
float Win_to_GL_Y(int y);

// 4���� �簢��
void drawRect();
#define MAX_SIZE 0.5f
#define MIN_SIZE 0.1f
typedef struct RECTANGLE
{
	float r;
	float g;
	float b;
	float size;
}rect;
rect r1 = { 1.0f, 0.0f, 0.0f, MAX_SIZE };		// �»��(r, g, b, size)
rect r2 = { 0.0f, 1.0f, 0.0f, MAX_SIZE };		// ����(r, g, b, size)
rect r3 = { 0.0f, 0.0f, 1.0f, MAX_SIZE };		// ���ϴ�(r, g, b, size)
rect r4 = { 1.0f, 1.0f, 0.0f, MAX_SIZE };		// ���ϴ�(r, g, b, size)

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);								//--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0);						//--- �������� ��ġ ����
	glutInitWindowSize(clientWidth, clientHeight);		//--- �������� ũ�� ����
	glutCreateWindow("test 02");						//--- ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else 
		std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene);					//--- ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape);					//--- �ٽ� �׸��� �ݹ��Լ� ����
	glutKeyboardFunc(Keyboard);					//--- Ű���� �Է� �ݹ��Լ� ����
	glutMouseFunc(Mouse);
	glutMainLoop();								//--- �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(clientRed, clientGreen, clientBlue, 1.0f);			//--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT);									//--- ������ ������ ��ü�� ĥ�ϱ�

	drawRect();	// 4���� �簢���� �׸��� �Լ� �߰�
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
	case 'a':
		r4.size -= 0.02f;
		break;
	case 's':
		r4.size += 0.02f;
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}
void Mouse(int button, int state, int x, int y)
{
	float mX = Win_to_GL_X(x);		
	float mY = Win_to_GL_Y(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// �簢�� ���� Ŭ�� ���� --> Ŭ���� �簢�� ���� ���� ����
		std::cout << "mX = " << mX << "mY = " << mY << std::endl;
		// �簢�� �ܺ� Ŭ�� ���� --> ���� ���� ���� (4���� �簢���� ��� Ŭ������ ����)

	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//�簢�� ���� Ŭ�� ���� --> Ŭ���� �簢�� ũ�� ���
		
		//�簢�� �ܺ� Ŭ�� ���� --> Ŭ���� 4�и��� �簢�� ũ�� Ȯ��

	}
}
void drawRect()
{
	// �� �簢���� �߾���ǥ���� size��ŭ �ø�.
	// �»��
	glColor3f(r1.r, r1.g, r1.b);
	if (r1.size > MAX_SIZE)
	{
		r1.size = MAX_SIZE;
	}
	else if (r1.size < MIN_SIZE)
	{
		r1.size = MIN_SIZE;
	}
	glRectf(-0.5f - r1.size, 0.5f + r1.size, -0.5f + r1.size, 0.5f - r1.size);
	// ����
	glColor3f(r2.r, r2.g, r2.b);
	if (r2.size > MAX_SIZE)
	{
		r2.size = MAX_SIZE;
	}
	else if (r2.size < MIN_SIZE)
	{
		r2.size = MIN_SIZE;
	}
	glRectf(0.5f - r2.size, 0.5f + r2.size, 0.5f + r2.size, 0.5f - r2.size);
	// ���ϴ�
	glColor3f(r3.r, r3.g, r3.b);
	if (r3.size > MAX_SIZE)
	{
		r3.size = MAX_SIZE;
	}
	else if (r3.size < MIN_SIZE)
	{
		r3.size = MIN_SIZE;
	}
	glRectf(-0.5f - r3.size, -0.5f + r3.size, -0.5f + r3.size, -0.5f - r3.size);
	// ���ϴ�
	glColor3f(r4.r, r4.g, r4.b);
	if (r4.size > MAX_SIZE)
	{
		r4.size = MAX_SIZE;
	}
	else if (r4.size < MIN_SIZE)
	{
		r4.size = MIN_SIZE;
	}
	glRectf(0.5f - r4.size, -0.5f + r4.size, 0.5f + r4.size, -0.5f - r4.size);
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