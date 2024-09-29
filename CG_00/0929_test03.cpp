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
#define MAX_RECT 10
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
void makeRect()
{
	if(rect_count < MAX_RECT)
	{
		rt[rect_count].midX = generateRandomFloat(-1.0f, 1.0f);
		rt[rect_count].midY = generateRandomFloat(-1.0f, 1.0f);
		rt[rect_count].r = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].g = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].b = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].width = generateRandomFloat(0.1f, 0.3f);
		rt[rect_count].height = generateRandomFloat(0.1f, 0.4f);
		rect_count++;
	}
	else
	{
		std::cout << "사각형 개수 초과" << std::endl;
	}
}

// 사각형 합치기
RECTS temp1;
RECTS temp2;
int temp1_index = 0;
int temp2_index = 0;

void addRect()
{
	float left, right, top, bottom;
	//left
	if (temp1.midX - temp1.width / 2 < temp2.midX - temp2.width / 2)
		left = temp1.midX - temp1.width / 2;
	else
		left = temp2.midX - temp2.width / 2;
	//right
	if (temp1.midX + temp1.width / 2 < temp2.midX + temp2.width / 2)
		right = temp2.midX + temp2.width / 2;
	else
		right = temp1.midX + temp1.width / 2;
	//top
	if (temp1.midY + temp1.height / 2 < temp2.midY + temp2.height / 2)
		top = temp2.midY + temp2.height / 2;
	else
		top = temp1.midY + temp1.height / 2;
	//bottom
	if (temp1.midY - temp1.height / 2 < temp2.midY - temp2.height / 2)
		bottom = temp1.midY - temp1.height / 2;
	else 
		bottom = temp2.midY - temp2.height / 2;

	// 기존 두 사각형의 인덱스를 삭제하고, 배열을 앞으로 이동시킴
	for (int i = temp1_index; i < rect_count - 1; i++)
		rt[i] = rt[i + 1];
	for (int i = temp2_index; i < rect_count - 2; i++)
		rt[i] = rt[i + 1];

	rect_count -= 2; // 두 개의 사각형이 제거되었으므로 카운트를 2 줄임
	//create
	if (rect_count < MAX_RECT)
	{
		rt[rect_count].midX = (right + left) / 2;
		rt[rect_count].midY = (top + bottom) / 2;
		rt[rect_count].r = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].g = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].b = generateRandomFloat(0.0f, 1.0f);
		rt[rect_count].width = right - left;
		rt[rect_count].height = top - bottom;
		rect_count++;
	}
}

// 현재 존재하는 사각형 모두 출력
void drawRect() {
	for (int i = 0; i < rect_count; i++) {
		glColor3f(rt[i].r, rt[i].g, rt[i].b);
		glRectf(rt[i].midX - (rt[i].width / 2), rt[i].midY - (rt[i].height / 2),
				rt[i].midX + (rt[i].width / 2), rt[i].midY + (rt[i].height / 2));
	}
}

// 왼쪽 마우스 클릭 확인
bool left_button = false;
int click_index = 0;

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
	case 'a':
		// 화면의 랜덤한 위치에 다른 색상의 사각형 생성 -> 최대 10개
		makeRect();
		break;
	}
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	float mX = Win_to_GL_X(x);
	float mY = Win_to_GL_Y(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//if문 사각형 뒤쪽 인덱스부터 검사 (위에 있는 사각형부터 검사)
		if (left_button == false)
		{
			for (int i = rect_count - 1; i >= 0; i--)
			{
				if (rt[i].midX - rt[i].width / 2 < mX && mX < rt[i].midX + rt[i].width / 2 &&
					rt[i].midY - rt[i].height / 2 < mY && mY < rt[i].midY + rt[i].height / 2)
				{
					click_index = i;		// 인덱스 저장
					left_button = true;		// MouseMotion 활성
					break;					// for문 탈출
				}
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		left_button = false;
		// 클릭된 사각형이 놓인 자리 검사 -> 합치기 -> 배열 빈공간 없애기
		// std::cout << "x = " << x << "y = " << y << std::endl; // test
		for (int i = rect_count - 1; i >= 0; i--)
		{
			if(i != click_index)
			{
				if (rt[click_index].midX - rt[click_index].width / 2 < rt[i].midX + rt[i].width / 2 &&
					rt[click_index].midX + rt[click_index].width / 2 > rt[i].midX - rt[i].width / 2 &&
					rt[click_index].midY - rt[click_index].height / 2 < rt[i].midY + rt[i].height / 2 &&
					rt[click_index].midY + rt[click_index].height / 2 > rt[i].midY - rt[i].height / 2)
				{
					//두 인덱스의 값을 저장 -> 두 인덱스의 값 제거 -> 배열의 빈공간 없게 앞으로 밀기 -> 맨 뒤에 새로운 사각형 생성(합쳐짐)
					temp1 = rt[click_index];
					temp2 = rt[i];
					temp1_index = click_index;
					temp2_index = i;
					addRect();
					break;
				}
			}
		}
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
		rt[click_index].midX = mX;
		rt[click_index].midY = mY;
		glutPostRedisplay(); // refresh
	}
}

