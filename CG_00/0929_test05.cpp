#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

// 클라이언트 크기
#define clientWidth 800
#define clientHeight 600

// 클라이언트 배경색
GLclampf clientRed = 1.0f;
GLclampf clientGreen = 1.0f;
GLclampf clientBlue = 1.0f;

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
#define MIN_RECT 5
#define MAX_RECT 10
#define RECT_SIZE 0.025f // x3, x4으로 width, height 초기화 (eraser는 x6, x8)
int create_rect_count = 0;
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
RECTS eraser;

// 사각형 만들기 및 초기화 (최대 10)
void setRects()
{
	// init Rect
	for (int i = 0; i < MAX_RECT; i++)
	{
		rt[i].midX	= 0;
		rt[i].midY	= 0;
		rt[i].r		= 0;
		rt[i].g		= 0;
		rt[i].b		= 0;
		rt[i].width = 0;
		rt[i].height = 0;
	}

	std::cout << "set rects\n";
	create_rect_count = (int)generateRandomFloat(MIN_RECT, MAX_RECT);
	for (int i = 0; i < create_rect_count; i++)
	{
		rt[i].midX		= generateRandomFloat(-1.0f, 1.0f);
		rt[i].midY		= generateRandomFloat(-1.0f, 1.0f);
		rt[i].r			= generateRandomFloat(0.0f, 1.0f);
		rt[i].g			= generateRandomFloat(0.0f, 1.0f);
		rt[i].b			= generateRandomFloat(0.0f, 1.0f);
		rt[i].width		= RECT_SIZE * 3.0f;
		rt[i].height	= RECT_SIZE * 4.0f;
	}
	rect_count = 0;
}
// 지우개 초기화 함수
void initEraser()
{
	std::cout << "init Eraser\n";
	eraser.midX = 0;
	eraser.midY = 0;
	eraser.r = 0;
	eraser.g = 0;
	eraser.b = 0;
	eraser.width = 0;
	eraser.height = 0;
}
void makeEraser(float mx, float my)
{
	std::cout << "make Eraser\n";
	eraser.midX		= mx;
	eraser.midY		= my;
	eraser.r		= 0.0f;
	eraser.g		= 0.0f;
	eraser.b		= 0.0f;
	eraser.width	= RECT_SIZE * 6.0f;
	eraser.height	= RECT_SIZE * 8.0f;
}

// 현재 존재하는 사각형 모두 출력
void drawRect()
{
	for (int i = 0; i < create_rect_count; i++)
	{
		glColor3f(rt[i].r, rt[i].g, rt[i].b);
		glRectf(rt[i].midX - (rt[i].width / 2), rt[i].midY - (rt[i].height / 2),
				rt[i].midX + (rt[i].width / 2), rt[i].midY + (rt[i].height / 2));
	}
}
void drawEraser()
{
	glColor3f(eraser.r, eraser.g, eraser.b);
	glRectf(eraser.midX - (eraser.width / 2), eraser.midY - (eraser.height / 2),
			eraser.midX + (eraser.width / 2), eraser.midY + (eraser.height / 2));
}

// 왼쪽 마우스 클릭 확인
bool left_button = false;

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
	glutMotionFunc(Motion);						// 마우스 움직임
	glutMainLoop();								// 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(clientRed, clientGreen, clientBlue, 1.0f);			//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT);									//--- 설정된 색으로 전체를 칠하기

	drawRect();		// 20~40개의 사각형
	drawEraser();	// 지우개 그리기
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
	{
		std::cout << "Quit\n";
		glutLeaveMainLoop(); // OpenGL 메인 루프 종료
		break;
	}
	case 'r':		// 타이머 중지, 사각형 초기화 및 재입력
	{
		setRects();
		std::cout << "rects : " << create_rect_count << std::endl;
		break;
	}
	}
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	float mX = Win_to_GL_X(x);
	float mY = Win_to_GL_Y(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		makeEraser(mX, mY);
		left_button = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		left_button = false;
		initEraser();
	}
	glutPostRedisplay(); // refresh
}
void Motion(int x, int y)
{
	//사각형 위를 클릭한 상태면
	if (left_button == true)
	{
		float mX = Win_to_GL_X(x);
		float mY = Win_to_GL_Y(y);

		//마우스 좌표에 따라 해당 사각형 이동 -> midX, midY에 마우스 좌표 대입
		eraser.midX = mX; 
		eraser.midY = mY;
		glutPostRedisplay(); // refresh
	}
}
