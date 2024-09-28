#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

GLclampf Red = 1.0f;
GLclampf Green = 1.0f;
GLclampf Blue = 1.0f;
int timerOnOff = 0;

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
void TimerFunction(int value);

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);								//--- glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- ���÷��� ��� ����
	glutInitWindowPosition(0, 0);						//--- �������� ��ġ ����
	glutInitWindowSize(800, 600);						//--- �������� ũ�� ����
	glutCreateWindow("test 01");						//--- ������ ����(������ �̸�)
	
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
	glClearColor(Red, Green, Blue, 1.0f);			//--- �������� ����
	glClear(GL_COLOR_BUFFER_BIT);					//--- ������ ������ ��ü�� ĥ�ϱ�
	glutSwapBuffers();								//--- ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
void TimerFunction(int value)
{
	Red = generateRandomFloat();
	Green = generateRandomFloat();
	Blue = generateRandomFloat();
	glutPostRedisplay(); // ȭ�� �� ���
	if(timerOnOff == 1)
	{
		glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸��Լ� �� ����
	}
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c':		// ������ û�ϻ����� ����
	{
		Red = 0.0f;
		Green = 1.0f;
		Blue = 1.0f;
		break;
	}
	case 'm':		// ������ ��ȫ������ ����
	{
		Red = 1.0f;
		Green = 0.0f;
		Blue = 1.0f;
		break;
	}
	case 'y':		// ������ ��������� ����
	{
		Red = 1.0f;
		Green = 1.0f;
		Blue = 0.0f;
		break;
	}
	case 'a':		// ������ �������� ����
	{
		Red = generateRandomFloat();
		Green = generateRandomFloat();
		Blue = generateRandomFloat();
		break;
	}
	case 'w':		// ������ ������� ����
	{
		Red = 1.0f;
		Green = 1.0f;
		Blue = 1.0f;
		break;
	}
	case 'k':		// ������ ������� ����
	{
		Red = 0.0f;
		Green = 0.0f;
		Blue = 0.0f;
		break;
	}
	case 't':		// Ÿ�̸� ����
	{
		if (timerOnOff == 0)
		{
			timerOnOff = 1;
			glutTimerFunc(100, TimerFunction, 1);
		}
		break;
	}
	case 's':		// Ÿ�̸� ����
	{
		timerOnOff = 0;
		break;
	}
	case 'q':		// ���α׷� ����
	{
		glutLeaveMainLoop(); // OpenGL ���� ���� ����
		break;
	}
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}