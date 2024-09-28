#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

#define clientWidth 800
#define clientHeight 600

// 클라이언트 배경색
GLclampf clientRed = 1.0f;
GLclampf clientGreen = 1.0f;
GLclampf clientBlue = 1.0f;

// 랜덤 실수값(0.0f ~ 1.0f) 반환 함수
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister 엔진
float generateRandomFloat()
{
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);	// (0.0f ~ 1.0f) 범위 설정
	return dis(gen);
}

// GL 이벤트 함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);

//좌표변환 함수들
int GL_to_Win_X(float x);
int GL_to_Win_Y(float y);
float Win_to_GL_X(int x);
float Win_to_GL_Y(int y);

// 4개의 사각형
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
rect r1 = { 1.0f, 0.0f, 0.0f, MAX_SIZE };		// 좌상단(r, g, b, size)
rect r2 = { 0.0f, 1.0f, 0.0f, MAX_SIZE };		// 우상단(r, g, b, size)
rect r3 = { 0.0f, 0.0f, 1.0f, MAX_SIZE };		// 좌하단(r, g, b, size)
rect r4 = { 1.0f, 1.0f, 0.0f, MAX_SIZE };		// 우하단(r, g, b, size)

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);								//--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);						//--- 윈도우의 위치 지정
	glutInitWindowSize(clientWidth, clientHeight);		//--- 윈도우의 크기 지정
	glutCreateWindow("test 02");						//--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else 
		std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene);					//--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);					//--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);					//--- 키보드 입력 콜백함수 지정
	glutMouseFunc(Mouse);
	glutMainLoop();								//--- 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(clientRed, clientGreen, clientBlue, 1.0f);			//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT);									//--- 설정된 색으로 전체를 칠하기

	drawRect();	// 4개의 사각형을 그리는 함수 추가
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
	case 'a':
		r4.size -= 0.02f;
		break;
	case 's':
		r4.size += 0.02f;
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
void Mouse(int button, int state, int x, int y)
{
	float mX = Win_to_GL_X(x);		
	float mY = Win_to_GL_Y(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// 사각형 내부 클릭 감지 --> 클릭한 사각형 색상 랜덤 변경
		std::cout << "mX = " << mX << "mY = " << mY << std::endl;
		// 사각형 외부 클릭 감지 --> 배경색 랜덤 변경 (4개의 사각형이 모두 클릭되지 않음)

	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//사각형 내부 클릭 감지 --> 클릭한 사각형 크기 축소
		
		//사각형 외부 클릭 감지 --> 클릭된 4분면의 사각형 크기 확대

	}
}
void drawRect()
{
	// 각 사각형의 중앙좌표에서 size만큼 늘림.
	// 좌상단
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
	// 우상단
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
	// 좌하단
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
	// 우하단
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