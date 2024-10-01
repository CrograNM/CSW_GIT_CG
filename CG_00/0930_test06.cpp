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

// 타이머 함수
bool timer_1 = true;
void TimerFunction1(int value);

// 구조체 생성
#define MIN_RECT 5
#define MAX_RECT 10
int create_rect_count = 0;
typedef struct RECTS
{
	bool exist;
	float midX;
	float midY;
	float r, g, b;
	float width;
	float height;
}RECTS;
RECTS rt[MAX_RECT];
typedef struct DIVRECTS
{
	bool exist;
	int timer;
	float midX;
	float midY;
	float r, g, b;
	float width;
	float height;
	float dx, dy;
}DIVRECTS;
// 하나의 사각형에서 쪼개진 divRect는 최대 8개의 사각형을 저장한다.
DIVRECTS divRect[MAX_RECT][8] = {}; // (시계방향) 0: 상, 1: 우상, 2: 우, 3: 우하, 4: 하, 5: 좌하, 6: 좌, 7: 좌상
int div_rect_count = 0;

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
	// init divRect
	for (int i = 0; i < MAX_RECT; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			divRect[i][j].exist = false;
			divRect[i][j].timer = -1;
			divRect[i][j].midX = 0;
			divRect[i][j].midY = 0;
			divRect[i][j].r = 0;
			divRect[i][j].g = 0;
			divRect[i][j].b = 0;
			divRect[i][j].width = 0;
			divRect[i][j].height = 0;
			divRect[i][j].dx = 0; 
			divRect[i][j].dy = 0;
		}
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
void divideRect(float mX, float mY)
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
	//선택에 성공한 경우 아래 코드 실행
	if (select_rect >= 0)
	{
		//int timerNum = rand() % 3;
		int timerNum = 3;
		std::cout << "select : timer " << timerNum << "\n";
		rt[select_rect].exist = false;
		switch (timerNum)
		{
		case 0:
		{
			// 1. 4, 좌우상하 이동
			for (int i = 0; i < 8; i++)
			{
				divRect[div_rect_count][i].timer = 0;
				if (i % 2 == 0)
				{	//8방향 중 상우하좌 방향(0,2,4,6인덱스)
					//공통 적용 요소
					divRect[div_rect_count][i].exist = true;
					divRect[div_rect_count][i].r = rt[select_rect].r;
					divRect[div_rect_count][i].g = rt[select_rect].g;
					divRect[div_rect_count][i].b = rt[select_rect].b;
					divRect[div_rect_count][i].width = rt[select_rect].width / 2;
					divRect[div_rect_count][i].height = rt[select_rect].height / 2;
					//방향에 따라 달라지는 요소들
					if (i == 0)
					{	//상
						divRect[div_rect_count][i].midX = rt[select_rect].midX;
						divRect[div_rect_count][i].midY = rt[select_rect].midY + rt[select_rect].height / 2;
						divRect[div_rect_count][i].dx = 0;
						divRect[div_rect_count][i].dy = 1;
					}
					else if (i == 2)
					{	//우
						divRect[div_rect_count][i].midX = rt[select_rect].midX + rt[select_rect].width / 2;
						divRect[div_rect_count][i].midY = rt[select_rect].midY;
						divRect[div_rect_count][i].dx = 1;
						divRect[div_rect_count][i].dy = 0;
					}
					else if (i == 4)
					{	//하
						divRect[div_rect_count][i].midX = rt[select_rect].midX;
						divRect[div_rect_count][i].midY = rt[select_rect].midY - rt[select_rect].height / 2;
						divRect[div_rect_count][i].dx = 0;
						divRect[div_rect_count][i].dy = -1;
					}
					else
					{	//좌
						divRect[div_rect_count][i].midX = rt[select_rect].midX - rt[select_rect].width / 2;
						divRect[div_rect_count][i].midY = rt[select_rect].midY;
						divRect[div_rect_count][i].dx = -1;
						divRect[div_rect_count][i].dy = 0;
					}
				}
				else
				{	//8방향 중 대각선 부분은 없는 셈이다.
					divRect[div_rect_count][i].exist = false;
					divRect[div_rect_count][i].midX = 0;
					divRect[div_rect_count][i].midY = 0;
					divRect[div_rect_count][i].r = 0;
					divRect[div_rect_count][i].g = 0;
					divRect[div_rect_count][i].b = 0;
					divRect[div_rect_count][i].width = 0;
					divRect[div_rect_count][i].height = 0;
					divRect[div_rect_count][i].dx = 0;
					divRect[div_rect_count][i].dy = 0;
				}
			}
			break;
		}
		case 1:
		{
			// 2. 4, 대각선 이동
			for (int i = 0; i < 8; i++)
			{
				divRect[div_rect_count][i].timer = 1;
				if (i % 2 == 1)
				{	//8방향 중 대각선 방향(1,3,5,7인덱스)
					//공통 적용 요소
					divRect[div_rect_count][i].exist = true;
					divRect[div_rect_count][i].r = rt[select_rect].r;
					divRect[div_rect_count][i].g = rt[select_rect].g;
					divRect[div_rect_count][i].b = rt[select_rect].b;
					divRect[div_rect_count][i].width = rt[select_rect].width / 2;
					divRect[div_rect_count][i].height = rt[select_rect].height / 2;
					//방향에 따라 달라지는 요소들
					if (i == 1)
					{	//우상
						divRect[div_rect_count][i].midX = rt[select_rect].midX + rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY + rt[select_rect].height / 3;
						divRect[div_rect_count][i].dx = 1;
						divRect[div_rect_count][i].dy = 1;
					}
					else if (i == 3)
					{	//우하
						divRect[div_rect_count][i].midX = rt[select_rect].midX + rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY - rt[select_rect].height / 3;
						divRect[div_rect_count][i].dx = 1;
						divRect[div_rect_count][i].dy = -1;
					}
					else if (i == 5)
					{	//좌하
						divRect[div_rect_count][i].midX = rt[select_rect].midX - rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY - rt[select_rect].height / 3;
						divRect[div_rect_count][i].dx = -1;
						divRect[div_rect_count][i].dy = -1;
					}
					else
					{	//좌상
						divRect[div_rect_count][i].midX = rt[select_rect].midX - rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY + rt[select_rect].height / 3;
						divRect[div_rect_count][i].dx = -1;
						divRect[div_rect_count][i].dy = 1;
					}
				}
				else
				{	//8방향 중 좌우상하 부분은 없는 셈이다.
					divRect[div_rect_count][i].exist = false;
					divRect[div_rect_count][i].midX = 0;
					divRect[div_rect_count][i].midY = 0;
					divRect[div_rect_count][i].r = 0;
					divRect[div_rect_count][i].g = 0;
					divRect[div_rect_count][i].b = 0;
					divRect[div_rect_count][i].width = 0;
					divRect[div_rect_count][i].height = 0;
					divRect[div_rect_count][i].dx = 0;
					divRect[div_rect_count][i].dy = 0;
				}
			}
			break;
		}
		case 2:
		{
			// 3. 4, 쪼개지고, 한쪽 방향으로 같이 이동
			int randDir = rand() % 4;
			for (int i = 0; i < 8; i++) 
			{
				divRect[div_rect_count][i].timer = 2;
				switch (randDir)
				{
				case 0:
					divRect[div_rect_count][i].dy = 1;
					break;
				case 1:
					divRect[div_rect_count][i].dx = 1;
					break;
				case 2:
					divRect[div_rect_count][i].dy = -1;
					break;
				case 3:
					divRect[div_rect_count][i].dx = -1;
					break;
				}
				if (i % 2 == 1)
				{	//8방향 중 대각선 방향(1,3,5,7인덱스)
					//공통 적용 요소
					divRect[div_rect_count][i].exist = true;
					divRect[div_rect_count][i].r = rt[select_rect].r;
					divRect[div_rect_count][i].g = rt[select_rect].g;
					divRect[div_rect_count][i].b = rt[select_rect].b;
					divRect[div_rect_count][i].width = rt[select_rect].width / 2;
					divRect[div_rect_count][i].height = rt[select_rect].height / 2;
					//방향에 따라 달라지는 요소들
					if (i == 1)
					{	//우상
						divRect[div_rect_count][i].midX = rt[select_rect].midX + rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY + rt[select_rect].height / 3;
					}
					else if (i == 3)
					{	//우하
						divRect[div_rect_count][i].midX = rt[select_rect].midX + rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY - rt[select_rect].height / 3;			
					}
					else if (i == 5)
					{	//좌하
						divRect[div_rect_count][i].midX = rt[select_rect].midX - rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY - rt[select_rect].height / 3;		
					}
					else
					{	//좌상
						divRect[div_rect_count][i].midX = rt[select_rect].midX - rt[select_rect].width / 3;
						divRect[div_rect_count][i].midY = rt[select_rect].midY + rt[select_rect].height / 3;
					}
				}
				else
				{	//8방향 중 좌우상하 부분은 없는 셈이다.
					divRect[div_rect_count][i].exist = false;
					divRect[div_rect_count][i].midX = 0;
					divRect[div_rect_count][i].midY = 0;
					divRect[div_rect_count][i].r = 0;
					divRect[div_rect_count][i].g = 0;
					divRect[div_rect_count][i].b = 0;
					divRect[div_rect_count][i].width = 0;
					divRect[div_rect_count][i].height = 0;
					divRect[div_rect_count][i].dx = 0;
					divRect[div_rect_count][i].dy = 0;
				}
			}
			break;
		}
		case 3:	
		{
			// 4. 8, 8방향 이동
			for (int i = 0; i < 8; i++)
			{	//공통 적용 요소
				divRect[div_rect_count][i].timer = 3;
				divRect[div_rect_count][i].exist = true;
				divRect[div_rect_count][i].r = rt[select_rect].r;
				divRect[div_rect_count][i].g = rt[select_rect].g;
				divRect[div_rect_count][i].b = rt[select_rect].b;
				divRect[div_rect_count][i].width = rt[select_rect].width / 2;
				divRect[div_rect_count][i].height = rt[select_rect].height / 2;
			}
			//방향에 따라 달라지는 요소들			
			//상
			divRect[div_rect_count][0].midX = rt[select_rect].midX;
			divRect[div_rect_count][0].midY = rt[select_rect].midY + rt[select_rect].height / 2;
			divRect[div_rect_count][0].dx = 0;
			divRect[div_rect_count][0].dy = 1;

			//우상
			divRect[div_rect_count][1].midX = rt[select_rect].midX + rt[select_rect].width / 3;
			divRect[div_rect_count][1].midY = rt[select_rect].midY + rt[select_rect].height / 3;
			divRect[div_rect_count][1].dx = 1;
			divRect[div_rect_count][1].dy = 1;

			//우
			divRect[div_rect_count][2].midX = rt[select_rect].midX + rt[select_rect].width / 2;
			divRect[div_rect_count][2].midY = rt[select_rect].midY;
			divRect[div_rect_count][2].dx = 1;
			divRect[div_rect_count][2].dy = 0;

			//우하
			divRect[div_rect_count][3].midX = rt[select_rect].midX + rt[select_rect].width / 3;
			divRect[div_rect_count][3].midY = rt[select_rect].midY - rt[select_rect].height / 3;
			divRect[div_rect_count][3].dx = 1;
			divRect[div_rect_count][3].dy = -1;

			//하
			divRect[div_rect_count][4].midX = rt[select_rect].midX;
			divRect[div_rect_count][4].midY = rt[select_rect].midY - rt[select_rect].height / 2;
			divRect[div_rect_count][4].dx = 0;
			divRect[div_rect_count][4].dy = -1;

			//좌하
			divRect[div_rect_count][5].midX = rt[select_rect].midX - rt[select_rect].width / 3;
			divRect[div_rect_count][5].midY = rt[select_rect].midY - rt[select_rect].height / 3;
			divRect[div_rect_count][5].dx = -1;
			divRect[div_rect_count][5].dy = -1;

			//좌
			divRect[div_rect_count][6].midX = rt[select_rect].midX - rt[select_rect].width / 2;
			divRect[div_rect_count][6].midY = rt[select_rect].midY;
			divRect[div_rect_count][6].dx = -1;
			divRect[div_rect_count][6].dy = 0;

			//좌상
			divRect[div_rect_count][7].midX = rt[select_rect].midX - rt[select_rect].width / 3;
			divRect[div_rect_count][7].midY = rt[select_rect].midY + rt[select_rect].height / 3;
			divRect[div_rect_count][7].dx = -1;
			divRect[div_rect_count][7].dy = 1;
			break;
		}
		}
		div_rect_count++;	
	}
	else
	{
		std::cout << "select fail\n";
	}
}

// 화면 출력
void draw()
{
	//draw Rects
	for (int i = 0; i < create_rect_count; i++)
	{
		if (rt[i].exist == true)
		{
			glColor3f(rt[i].r, rt[i].g, rt[i].b);
			glRectf(rt[i].midX - (rt[i].width / 2), rt[i].midY - (rt[i].height / 2),
				rt[i].midX + (rt[i].width / 2), rt[i].midY + (rt[i].height / 2));
		}
	}
	//draw DivRects
	for (int i = 0; i < div_rect_count; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (divRect[i][j].exist == true)
			{
				glColor3f(divRect[i][j].r, divRect[i][j].g, divRect[i][j].b);
				glRectf(divRect[i][j].midX - (divRect[i][j].width / 2), divRect[i][j].midY - (divRect[i][j].height / 2),
						divRect[i][j].midX + (divRect[i][j].width / 2), divRect[i][j].midY + (divRect[i][j].height / 2));
			}
		}
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
	glutTimerFunc(16, TimerFunction1, 1);

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
		divideRect(mX, mY);
	}
	glutPostRedisplay(); // refresh
}
// 타이머 함수 : 무한반복
void TimerFunction1(int value)
{
	for (int i = 0; i < div_rect_count; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			// 조건 추가: width가 너무 작으면 exist를 false로 설정
			if (divRect[i][j].exist == true)
			{
				if (j % 2 == 1)
				{
					divRect[i][j].midX = divRect[i][j].midX + divRect[i][j].dx * 0.0035f * 3;
					divRect[i][j].midY = divRect[i][j].midY + divRect[i][j].dy * 0.0035f * 4;
				}
				else
				{
					divRect[i][j].midX = divRect[i][j].midX + divRect[i][j].dx * 0.005f * 3;
					divRect[i][j].midY = divRect[i][j].midY + divRect[i][j].dy * 0.005f * 4;
				}
				divRect[i][j].width = divRect[i][j].width * 0.99f;
				divRect[i][j].height = divRect[i][j].height * 0.99f;

				if (divRect[i][j].width < 0.05f)
				{
					divRect[i][j].exist = false; // exist가 false로 설정
				}
			}
		}	
	}
	glutPostRedisplay();  // 화면 재출력
	glutTimerFunc(16, TimerFunction1, 1);  // 약 60fps 간격으로 타이머 재설정
}