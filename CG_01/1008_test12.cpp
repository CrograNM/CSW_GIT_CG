#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <gl/glew.h>			
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <random>

// 클라이언트
#define clientWidth 600
#define clientHeight 600
GLfloat rColor = 1.0;
GLfloat gColor = 1.0;
GLfloat bColor = 1.0;

// 랜덤 실수값(min ~ max) 반환 함수
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister 엔진
float generateRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dis(min, max);	// 인자로 범위 설정
	return dis(gen);
}

// 좌표 변환 함수
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

// 도형 관련 함수들
void setFigures();				// 점(미니 사각), 선, 삼각형, 사각형, 오각형 3개씩 랜덤 위치에 세팅
void makeFigureRandPos(int p);	// 꼭지점의 개수를 받아서 해당 도형을 랜덤 위치에 생성
void addFigure();

// 총 5개의 도형을 그린다. -> 사분면당 총 넷 + 단독 도형 하나
#define MAX_FIGURE 15
#define FIGURE_SIZE 0.05f

typedef struct FIGURE
{
	bool exist;			//출력 및 존재 여부
	bool isMoving;		//애니메이션 여부
	int type;			//도형타입 : 1점, 2선, 3삼각형, 4사각형, 5오각형

	float mX;			//중앙점 x
	float mY;			//중앙점 y
}FIGURE;

FIGURE fg[MAX_FIGURE];	// 인덱스 관리 : (0,1,2:점), (3,4,5:선), (6,7,8:삼각), (9,10,11:사각), (12,13,14:오각)
int figureCount = 0;	// 순서대로 생성되는 도형 인덱스에 사용 


#define TRI_COUNT 5
GLfloat	figure[MAX_FIGURE][TRI_COUNT * 3][3];	// 중앙점을 이용해, 삼각형 5개를 쓰자
GLfloat	colorData[MAX_FIGURE][TRI_COUNT * 3][3];

// 필요 변수 선언
GLint width, height;
GLchar* vertexSource, * fragmentSource;			//--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader;			//--- 세이더 객체
GLuint shaderProgramID;							//--- 셰이더 프로그램
GLuint vao, vbo[2];								//--- VAO, VBO

bool left_button = false;

// 사용자 정의 함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid InitBuffer();

// 타이머 관련
int timer_1 = false;
float timerCount = 0.0f;
#define FULLTIME 100
#define TIMER_VELOCITY 16
void TimerFunction1(int value);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(time(0));
	width = clientWidth;
	height = clientHeight;

	//--- 윈도우 생성하기
	glutInit(&argc, argv);									//--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);			//--- 디스플레이 모드 설정
	glutInitWindowPosition(100, 100);						//--- 윈도우의 위치 지정
	glutInitWindowSize(width, height);						//--- 윈도우의 크기 지정
	glutCreateWindow("test 12");							//--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	//setFigures();	//15개의 도형 세팅

	//--- 세이더 읽어와서 세이더 프로그램 만들기
	make_shaderProgram();
	InitBuffer();

	glutDisplayFunc(drawScene);					//--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);					//--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);					// 키보드 입력
	glutMouseFunc(Mouse);						// 마우스 입력
	glutMotionFunc(Motion);						// 마우스 움직임
	glutMainLoop();								//--- 이벤트 처리 시작
}

GLvoid drawScene()
{
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	//--- 사용할 VAO 불러오기 (VAO에 VBO의 값들이 모두 저장되어 있는 것)
	glBindVertexArray(vao);

	/*glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
	for (int i = 0; i < MAX_FIGURE; i++)
	{
		if (fg[i].exist == true)  // 도형이 존재하는 경우에만 그리기
		{
			if (fg[i].type == 2)
			{
				glLineWidth(2);	//타입이 선이면 선 폴리곤 모드로 출력
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{	
				//선 외의 도형은 모두 채우기 모드로 출력
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			// 각 도형의 인덱스를 기반으로 삼각형 그리기
			glDrawArrays(GL_TRIANGLES, i * 15, 15);  // 총 5개의 삼각형을 그리기 위해 15개의 인덱스 사용
		}
	}
	glutSwapBuffers();  // 화면에 출력하기
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
	}
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	float mX = Win_to_GL_X(x);
	float mY = Win_to_GL_Y(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (left_button == false)
		{
			//도형을 잡았는지 검사하여 left_button 활성화
			left_button = true;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		left_button = false;
		// 클릭된 사각형이 놓인 자리 검사 -> 합치기 -> 배열 빈공간 없애기
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

		//마우스 좌표에 따라 해당 도형 이동 -> 중점에 마우스 좌표 대입

		glutPostRedisplay(); // refresh
	}
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");			// Open file for reading 
	if (!fptr)							// Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END);			// Seek to the end of the file 
	length = ftell(fptr);				// Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1);	// Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET);			// Go back to the beginning of the file 
	fread(buf, length, 1, fptr);		// Read the contents of the file in to the buffer 
	fclose(fptr);						// Close the file 
	buf[length] = 0;					// Null terminator 
	return buf;							// Return the buffer 
}
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");

	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);

	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);

	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");

	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);

	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);

	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기

	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}
void InitBuffer()
{
	// 0: 도형 좌표, 1: 색상 데이터, 2: 이등분 선 좌표
	glGenVertexArrays(1, &vao);		//--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao);			//--- VAO를 바인드하기
	glGenBuffers(3, vbo);

	// 도형 및 색상 데이터용 VAO, VBO 초기화
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(figure), figure, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
}

void setFigures()
{
	// 점(미니 사각), 선, 삼각형, 사각형, 오각형 3개씩 랜덤 위치에 세팅
	// init Figure
	for (int i = 0; i < MAX_FIGURE; i++)
	{
		for (int j = 0; j < (TRI_COUNT*3); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				figure[i][j][k] = 0.0f;		//도형 위치 : (초기값) 3개 버텍스 모두 (x0, y0, z0)
				colorData[i][j][k] = 0.0f;	//도형 색상 : (초기값) 검은색
			}
		}
		fg[i].type = 2;
		fg[i].exist = false;
		fg[i].isMoving = false;
		fg[i].mX = 0.0f;
		fg[i].mY = 0.0f;
	}
	// VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE) * (TRI_COUNT * 9) * sizeof(GLfloat), figure, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE) * (TRI_COUNT * 9) * sizeof(GLfloat), colorData, GL_STATIC_DRAW);

	for (int i = 1; i < 5 + 1; i++)
	{
		makeFigureRandPos(i);	//1~5 : 점 ~ 오각형 3개씩 생성
		makeFigureRandPos(i);
		makeFigureRandPos(i);
	}
}
void makeFigureRandPos(int p)
{
	// 꼭지점의 개수를 받아서 해당 도형을 랜덤 위치에 생성
	switch (p)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	}
}
void addFigure()
{

}