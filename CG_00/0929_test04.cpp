#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

// 클라이언트 크기
#define clientWidth 800
#define clientHeight 600

// 클라이언트 배경색
GLclampf clientRed = 0.2f;
GLclampf clientGreen = 0.2f;
GLclampf clientBlue = 0.2f;

// 랜덤 실수값(min ~ max) 반환 함수
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister 엔진
float generateRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dis(min, max);	// 인자로 범위 설정
	return dis(gen);
}

//좌표 변환 함수
int GL_to_Win_X(float x)
{
	return (x + 1) * (clientWidth / 2.0f);  // 2.0f로 실수 나눗셈
}
int GL_to_Win_Y(float y)
{
	return (1 - y) * (clientHeight / 2.0f);  // 2.0f로 실수 나눗셈
}
float Win_to_GL_X(int x)
{
	return (x / (float)clientWidth) * 2 - 1;  // 정수 나눗셈 방지
}
float Win_to_GL_Y(int y)
{
	return 1 - (y / (float)clientHeight) * 2;  // 정수 나눗셈 방지
}

// 사각형 구조체 (배열)
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

// 사각형 만들기 및 초기화 (최대 10)
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
		std::cout << "사각형 개수 초과" << std::endl;
	}
}

// 현재 존재하는 사각형 모두 출력
void drawRect()
{
	for (int i = 0; i < rect_count; i++)
	{
		glColor3f(rt[i].r, rt[i].g, rt[i].b);
		glRectf(rt[i].midX - (rt[i].width / 2), rt[i].midY - (rt[i].height / 2),
			rt[i].midX + (rt[i].width / 2), rt[i].midY + (rt[i].height / 2));
	}
}

// GL 이벤트 함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);								//--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);						//--- 윈도우의 위치 지정
	glutInitWindowSize(clientWidth, clientHeight);		//--- 윈도우의 크기 지정
	glutCreateWindow("test 04");						//--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene);					// 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);					// 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);					// 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);						// 마우스 입력
	glutMainLoop();								// 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(clientRed, clientGreen, clientBlue, 1.0f);			//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT);									//--- 설정된 색으로 전체를 칠하기

	drawRect();	//10개의 사각형
	glutSwapBuffers();												//--- 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':		// 프로그램 종료
		glutLeaveMainLoop(); // OpenGL 메인 루프 종료
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
