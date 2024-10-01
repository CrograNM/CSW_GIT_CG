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

// 구조체 생성
#define MIN_RECT 5
#define MAX_RECT 10
int create_rect_count = 0;
typedef struct RECTANGLES
{
	bool exist;
	float midX;
	float midY;
	float r, g, b;
	float width;
	float height;
}RECTS;
RECTS rt[MAX_RECT];

// 사각형 생성
void setRects()
{
	// init Rect
	for (int i = 0; i < MAX_RECT; i++)
	{
		rt[i].exist = false;
		rt[i].midX = 0;
		rt[i].midY = 0;
		rt[i].r = 0;
		rt[i].g = 0;
		rt[i].b = 0;
		rt[i].width = 0;
		rt[i].height = 0;
	}
	std::cout << "--set rects--\n";
	create_rect_count = (int)generateRandomFloat(MIN_RECT, MAX_RECT);
	for (int i = 0; i < create_rect_count; i++)
	{
		float rect_size = generateRandomFloat(0.05f, 0.1f);
		rt[i].exist = true;
		rt[i].midX = generateRandomFloat(-1.0f, 1.0f);
		rt[i].midY = generateRandomFloat(-1.0f, 1.0f);
		rt[i].r = generateRandomFloat(0.0f, 1.0f);
		rt[i].g = generateRandomFloat(0.0f, 1.0f);
		rt[i].b = generateRandomFloat(0.0f, 1.0f);
		rt[i].width = rect_size * 3.0f;
		rt[i].height = rect_size * 4.0f;
	}
	std::cout << "rects : " << create_rect_count << std::endl;
}

// 사각형 나누기
void devideRect(float mX, float mY)
{
	//마우스를 클릭하면 사각형 4개로 나눠서 출력
	//해당 사각형들은 기존 사각형의 색을 유지하며, 각 사각형의 크기는 기존의 절반크기
	int select_rect = -1;	//select_rect값이 -1이면, 선택에 실패한 것임
	for (int i = create_rect_count - 1; i >= 0; i--)
	{
		if (rt[i].exist == true)
		{
			if (rt[i].midX - rt[i].width / 2 < mX && mX < rt[i].midX + rt[i].width / 2 &&
				rt[i].midY - rt[i].height / 2 < mY && mY < rt[i].midY + rt[i].height / 2)
			{
				select_rect = i;		// 인덱스 저장
				break;					// for문 탈출
			}
		}
	}
	if (select_rect >= 0)	//선택에 성공한 경우 아래 코드 실행
	{
		//1. 4, 좌우상하 이동
		//2. 4, 대각선 이동
		//3. 4, 쪼개지고, 한쪽 방향으로 같이 이동
		//4. 8, 8방향 이동
	}
}

// 화면 출력
void draw()
{
	//draw Rects
	for (int i = 0; i < create_rect_count; i++)
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

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);								//--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);						//--- 윈도우의 위치 지정
	glutInitWindowSize(clientWidth, clientHeight);		//--- 윈도우의 크기 지정
	glutCreateWindow("test 06");						//--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	//--여기서 구조체 초기화 가능--
	setRects();

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

	draw();		// draw -> WM_PAINT
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
		std::cout << "--Quit--\n";
		glutLeaveMainLoop(); // OpenGL 메인 루프 종료
		break;
	}
	case 'r':		// 타이머 중지, 사각형 초기화 및 재입력
		setRects();
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
		devideRect(mX, mY);
	}
	glutPostRedisplay(); // refresh
}
