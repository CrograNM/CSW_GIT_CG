#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

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

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

float size = 1.0f;
float middleX = 0.0f, middleY = 0.0f;
typedef struct RECTANGLE
{
	float r;
	float g;
	float b;
	float size;
}rect;
rect r1 = { 1.0f, 0.0f, 0.0f, 0.5f };		// �»��(r, g, b, size)
rect r2 = { 0.0f, 1.0f, 0.0f, 0.5f };		// ����(r, g, b, size)
rect r3 = { 0.0f, 0.0f, 1.0f, 0.5f };		// ���ϴ�(r, g, b, size)
rect r4 = { 1.0f, 1.0f, 0.0f, 0.5f };		// ���ϴ�(r, g, b, size)

void drawRect() {
	// �� �簢���� �߾���ǥ���� size��ŭ �ø�.
	// �»��
	glColor3f(r1.r, r1.g, r1.b);
	glRectf(-0.5f - r1.size, 0.5f + r1.size, -0.5f + r1.size, 0.5f - r1.size);
	// ����
	glColor3f(r2.r, r2.g, r2.b);
	glRectf(0.5f - r2.size, 0.5f + r2.size, 0.5f + r2.size, 0.5f - r2.size);
	// ���ϴ�
	glColor3f(r3.r, r3.g, r3.b);
	glRectf(-0.5f - r3.size, -0.5f + r3.size, -0.5f + r3.size, -0.5f - r3.size);
	// ���ϴ�
	glColor3f(r4.r, r4.g, r4.b);
	glRectf(0.5f - r4.size, -0.5f + r4.size, 0.5f + r4.size, -0.5f - r4.size);
}

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);								//--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0);						//--- �������� ��ġ ����
	glutInitWindowSize(800, 600);						//--- �������� ũ�� ����
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
	glutMainLoop();								//--- �̺�Ʈ ó�� ����
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	//--- ����� ���� ����
	glClearColor(clientRed, clientGreen, clientBlue, 1.0f);			//--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT);									//--- ������ ������ ��ü�� ĥ�ϱ�

	drawRect();
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}