#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

GLclampf Red = 1.0f;
GLclampf Green = 1.0f;
GLclampf Blue = 1.0f;
int timerOnOff = 0;

// 랜덤 실수값(0.0f ~ 1.0f) 반환 함수
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister 엔진
float generateRandomFloat()
{
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);	// (0.0f ~ 1.0f) 범위 설정
	return dis(gen);
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);								//--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		//--- 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);						//--- 윈도우의 위치 지정
	glutInitWindowSize(800, 600);						//--- 윈도우의 크기 지정
	glutCreateWindow("test 01");						//--- 윈도우 생성(윈도우 이름)
	
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
	glutMainLoop();								//--- 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(Red, Green, Blue, 1.0f);			//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT);					//--- 설정된 색으로 전체를 칠하기
	glutSwapBuffers();								//--- 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
void TimerFunction(int value)
{
	Red = generateRandomFloat();
	Green = generateRandomFloat();
	Blue = generateRandomFloat();
	glutPostRedisplay(); // 화면 재 출력
	if(timerOnOff == 1)
	{
		glutTimerFunc(100, TimerFunction, 1); // 타이머함수 재 설정
	}
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c':		// 배경색을 청록색으로 설정
	{
		Red = 0.0f;
		Green = 1.0f;
		Blue = 1.0f;
		break;
	}
	case 'm':		// 배경색을 자홍색으로 설정
	{
		Red = 1.0f;
		Green = 0.0f;
		Blue = 1.0f;
		break;
	}
	case 'y':		// 배경색을 노랑색으로 설정
	{
		Red = 1.0f;
		Green = 1.0f;
		Blue = 0.0f;
		break;
	}
	case 'a':		// 배경색을 랜덤으로 설정
	{
		Red = generateRandomFloat();
		Green = generateRandomFloat();
		Blue = generateRandomFloat();
		break;
	}
	case 'w':		// 배경색을 백색으로 설정
	{
		Red = 1.0f;
		Green = 1.0f;
		Blue = 1.0f;
		break;
	}
	case 'k':		// 배경색을 흑색으로 설정
	{
		Red = 0.0f;
		Green = 0.0f;
		Blue = 0.0f;
		break;
	}
	case 't':		// 타이머 시작
	{
		if (timerOnOff == 0)
		{
			timerOnOff = 1;
			glutTimerFunc(100, TimerFunction, 1);
		}
		break;
	}
	case 's':		// 타이머 종료
	{
		timerOnOff = 0;
		break;
	}
	case 'q':		// 프로그램 종료
	{
		glutLeaveMainLoop(); // OpenGL 메인 루프 종료
		break;
	}
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}