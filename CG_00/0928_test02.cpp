#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

// Ŭ���̾�Ʈ ũ��
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

// 4���� �簢��
#define MAX_SIZE 0.5f
#define MIN_SIZE 0.1f
typedef struct RECTANGLE
{
	float r;
	float g;
	float b;
	float midX;		//�簢���� �߾� x
	float midY;		//�簢���� �߾� y
	float size;
}rect;
rect r1 = { 1.0f, 0.0f, 0.0f, -0.5f, 0.5f, MAX_SIZE };		// �»��(r, g, b, size)
rect r2 = { 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, MAX_SIZE };		// ����(r, g, b, size)
rect r3 = { 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, MAX_SIZE };		// ���ϴ�(r, g, b, size)
rect r4 = { 1.0f, 1.0f, 0.0f, 0.5f, -0.5f, MAX_SIZE };		// ���ϴ�(r, g, b, size)

// 4�� �簢�� �׸��� �Լ�
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
	glRectf(r1.midX - r1.size, r1.midY + r1.size,
		r1.midX + r1.size, r1.midY - r1.size);

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
	glRectf(r2.midX - r2.size, r2.midY + r2.size,
		r2.midX + r2.size, r2.midY - r2.size);

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
	glRectf(r3.midX - r3.size, r3.midY + r3.size,
		r3.midX + r3.size, r3.midY - r3.size);

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
	glRectf(r4.midX - r4.size, r4.midY + r4.size,
		r4.midX + r4.size, r4.midY - r4.size);
}

// GL �̺�Ʈ �Լ�
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);

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
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	float mX = Win_to_GL_X(x);		
	float mY = Win_to_GL_Y(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// �簢�� ���� Ŭ�� ���� --> Ŭ���� �簢�� ���� ���� ����
		// �簢�� �ܺ� Ŭ�� ���� --> ���� ���� ���� (4���� �簢���� ��� Ŭ������ ����)
		if (r1.midX - r1.size <= mX && mX <= r1.midX + r1.size && 
			r1.midY - r1.size <= mY && mY <= r1.midY + r1.size)
		{	// �»�� �簢�� ����
			r1.r = generateRandomFloat();
			r1.g = generateRandomFloat();
			r1.b = generateRandomFloat();
		}
		else if (r2.midX - r2.size <= mX && mX <= r2.midX + r2.size &&
				 r2.midY - r2.size <= mY && mY <= r2.midY + r2.size)
		{	// ���� �簢�� ����
			r2.r = generateRandomFloat();
			r2.g = generateRandomFloat();
			r2.b = generateRandomFloat();
		}
		else if (r3.midX - r3.size <= mX && mX <= r3.midX + r3.size &&
				 r3.midY - r3.size <= mY && mY <= r3.midY + r3.size)
		{	// ���ϴ� �簢�� ����
			r3.r = generateRandomFloat();
			r3.g = generateRandomFloat();
			r3.b = generateRandomFloat();
		}
		else if (r4.midX - r4.size <= mX && mX <= r4.midX + r4.size &&
				 r4.midY - r4.size <= mY && mY <= r4.midY + r4.size)
		{	// ���ϴ� �簢�� ����
			r4.r = generateRandomFloat();
			r4.g = generateRandomFloat();
			r4.b = generateRandomFloat();
		}
		else
		{	// ��� �簢�� ���ΰ� �ƴ� -> ��� Ŭ��
			clientRed = generateRandomFloat();
			clientGreen = generateRandomFloat();
			clientBlue = generateRandomFloat();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//�簢�� ���� Ŭ�� ���� --> Ŭ���� �簢�� ũ�� ���
		//�簢�� �ܺ� Ŭ�� ���� --> Ŭ���� 4�и��� �簢�� ũ�� Ȯ��
		// �»��
		if (-1.0f <= mX && mX <= 0.0f && 0.0f <= mY && mY <= 1.0f)
		{

		}
		// ����
		else if (0.0f <= mX && mX <= 1.0f && 0.0f <= mY && mY <= 1.0f)
		{

		}
		// ���ϴ�
		else if (-1.0f <= mX && mX <= 0.0f && -1.0f <= mY && mY <= 0.0f)
		{

		}
		// ���ϴ�
		else if (0.0f <= mX && mX <= 1.0f && -1.0f <= mY && mY <= 0.0f)
		{

		}
	}
	glutPostRedisplay(); // refresh
}