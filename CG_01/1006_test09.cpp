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

// 클라이언트 크기
#define clientWidth 600
#define clientHeight 600

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
void initFigure();
void drawNewTriangle(float mX, float mY);
void redrawTriangle(float mX, float mY);

// 최대 10개의 도형을 저장할 변수
#define MAX_FIGURE 4
#define FIGURE_SIZE 0.02f
#define VELOCITY 0.01f
typedef struct FIGURE
{
	float mX;	//중앙점 x
	float mY;	//중앙점 y
	float width;
	float height;
	float dx;
	float dy;
	int dir;	// 0:North, 1:East, 2:South, 3:West
	int type;	// 1:fill,  2:line

	int	verticalMoveCount;     // 세로로 이동할 횟수 (50번 반복 후 변경)	//0
}FIGURE;
FIGURE fg[MAX_FIGURE] = { 0, };

GLfloat figure[MAX_FIGURE][3][3];
GLfloat colorData[MAX_FIGURE][3][3];
int figureCount = 0;
int figureType = 1;						// 1:fill,  2:line

// 필요 변수 선언
GLint width, height;
GLchar* vertexSource, * fragmentSource;		//--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader;		//--- 세이더 객체
GLuint shaderProgramID;						//--- 셰이더 프로그램
GLuint vao, vbo[2];							//--- VAO, VBO

// 사용자 정의 함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid InitBuffer();

// 타이머 관련
int timer_1 = false;
int timer_2 = false;
int timer_3 = false;
int timer_4 = false;

void stopTimer()
{
	std::cout << "stop timer ALL\n";
	timer_1 = false;
	timer_2 = false;
	timer_3 = false;
	timer_4 = false;
}
void TimerFunction1(int value);	// 대각선 이동
void TimerFunction2(int value);	// 좌우 지그재그 이동

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
	glutCreateWindow("test 07");							//--- 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	initFigure();
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	make_shaderProgram();
	InitBuffer();

	glutDisplayFunc(drawScene);					//--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);					//--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();								//--- 이벤트 처리 시작
}

GLvoid drawScene()
{
	GLfloat rColor, gColor, bColor;
	rColor = 1.0;
	gColor = 1.0;
	bColor = 1.0;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	//--- 사용할 VAO 불러오기 (VAO에 VBO의 값들이 모두 저장되어 있는것)
	glBindVertexArray(vao);
	//--- 사용할 VAO 불러오기 (VAO에 VBO의 값들이 모두 저장되어 있는것)
	glBindVertexArray(vao);
	for (int i = 0; i < figureCount; i++)
	{
		switch (fg[i].type)
		{
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 2:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		}
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}
	glutSwapBuffers(); // 화면에 출력하기
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
	case 'a':
	{
		std::cout << "--fill mod--\n";
		figureType = 1;
		break;
	}
	case 'b':
	{
		std::cout << "--line mod--\n";
		figureType = 2;
		break;
	}
	case '1':
	{
		stopTimer();
		if (timer_1 == false)
		{
			std::cout << "timer_1 ON\n";
			timer_1 = true;
			glutTimerFunc(16, TimerFunction1, 1);
		}
		else
		{
			std::cout << "timer_1 OFF\n";
			timer_1 = false;
		}
		break;
	}
	case '2':
	{
		stopTimer();
		if (timer_2 == false)
		{
			std::cout << "timer_2 ON\n";
			timer_2 = true;
			glutTimerFunc(16, TimerFunction2, 1);
		}
		else
		{
			std::cout << "timer_2 OFF\n";
			timer_2 = false;
		}
		break;
	}
	case '3':
		break;
	case '4':
		break;
	}
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	// 마우스 클릭 위치를 GL 좌표로 변환
	float mX = Win_to_GL_X(x);
	float mY = Win_to_GL_Y(y);
	//클릭시 몇사분면인지 검사
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if(0 < figureCount)
		{
			figureCount--;
			redrawTriangle(mX, mY);
			figureCount++;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if(figureCount < MAX_FIGURE)
		{
			drawNewTriangle(mX, mY);
			figureCount++;
		}
	}
	glutPostRedisplay(); // 화면 다시 그리기
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
	glGenVertexArrays(1, &vao);		//--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao);			//--- VAO를 바인드하기
	glGenBuffers(2, vbo);			//--- 2개의 VBO를 지정하고 할당하기

	//--- 1번째 VBO 설정 : 좌표값
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), figure, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO 설정 : 색상
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), colorData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//vbo[0], vbo[1]에 해당 정점들의 위치와 색상이 저장되었다.
}

void initFigure()
{
	for (int i = 0; i < MAX_FIGURE; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				figure[i][j][k] = 0;
				colorData[i][j][k] = 1.0f;
			}
		}
		fg[i].mX = 0;
		fg[i].mY = 0;	
		fg[i].width = 0;
		fg[i].height = 0;
		fg[i].dx = VELOCITY;
		fg[i].dy = VELOCITY;
		fg[i].dir = 0;	
		fg[i].type = 1;	
		fg[i].verticalMoveCount = 50;
	}
	// VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE) * 9 * sizeof(GLfloat), figure, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE) * 9 * sizeof(GLfloat), colorData, GL_STATIC_DRAW);
}
void drawNewTriangle(float mX, float mY)
{
	float left = mX - FIGURE_SIZE * 3;
	float right = mX + FIGURE_SIZE * 3;
	float top = mY + FIGURE_SIZE * 3;
	float bottom = mY - FIGURE_SIZE * 3;

	std::cout << "Draw triangle" << std::endl;

	figure[figureCount][0][0] = mX;
	figure[figureCount][0][1] = top;
	figure[figureCount][0][2] = 0.0f;

	figure[figureCount][1][0] = left;
	figure[figureCount][1][1] = bottom;
	figure[figureCount][1][2] = 0.0f;

	figure[figureCount][2][0] = right;
	figure[figureCount][2][1] = bottom;
	figure[figureCount][2][2] = 0.0f;

	// 구조체에 값 대입 -> 해당 정보를 통해 타이머 구동
	fg[figureCount].mX = mX;
	fg[figureCount].mY = mY;
	fg[figureCount].width = right - left;
	fg[figureCount].height = top - bottom;
	fg[figureCount].dx = VELOCITY;
	fg[figureCount].dy = VELOCITY;
	fg[figureCount].dir = 0;
	fg[figureCount].type = figureType;
	fg[figureCount].verticalMoveCount = 50;

	float random1 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	float random2 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	float random3 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	for (int i = 0; i < 3; i++)
	{
		colorData[figureCount][i][0] = random1; // R
		colorData[figureCount][i][1] = random2; // G
		colorData[figureCount][i][2] = random3; // B
	}
	
	// VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (figureCount + 1) * 9 * sizeof(GLfloat), figure, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (figureCount + 1) * 9 * sizeof(GLfloat), colorData, GL_STATIC_DRAW);
}
void redrawTriangle(float mX, float mY)
{   //기존 삼각형을 지우고 하나 그리기 + 사이즈 변경, 색 변경
	float left = mX - FIGURE_SIZE * 3;
	float right = mX + FIGURE_SIZE * 3;
	float top = mY + FIGURE_SIZE * 3;
	float bottom = mY - FIGURE_SIZE * 3;

	float addSize = generateRandomFloat(-0.05f, 0.1f);

	std::cout << "Re_Draw triangle" << std::endl;

	figure[figureCount][0][0] = mX;
	figure[figureCount][0][1] = top + addSize;
	figure[figureCount][0][2] = 0.0f;

	figure[figureCount][1][0] = left - addSize;
	figure[figureCount][1][1] = bottom - addSize;
	figure[figureCount][1][2] = 0.0f;

	figure[figureCount][2][0] = right + addSize;
	figure[figureCount][2][1] = bottom - addSize;
	figure[figureCount][2][2] = 0.0f;

	// 구조체에 값 대입 -> 해당 정보를 통해 타이머 구동
	fg[figureCount].mX = mX;
	fg[figureCount].mY = mY;
	fg[figureCount].width = right - left + (2 * addSize);
	fg[figureCount].height = top - bottom + (2 * addSize);
	fg[figureCount].dx = VELOCITY;
	fg[figureCount].dy = VELOCITY;
	fg[figureCount].dir = 0;
	fg[figureCount].type = figureType;
	//fg[figureCount].verticalMoveCount = 50;

	float random1 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	float random2 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	float random3 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	for (int i = 0; i < 3; i++)
	{
		colorData[figureCount][i][0] = random1; // R
		colorData[figureCount][i][1] = random2; // G
		colorData[figureCount][i][2] = random3; // B
	}
	// VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
	// glBufferData : 지정한 크기만큼 할당 -> 초기에 설정하는 방식
	// glBufferSubData : 일부 업데이트 및 재할당
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, figureCount * 9 * sizeof(GLfloat), 9 * sizeof(GLfloat), figure[figureCount]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, figureCount * 9 * sizeof(GLfloat), 9 * sizeof(GLfloat), colorData[figureCount]);
}

// 대각선 이동
void TimerFunction1(int value)
{
	if (timer_1 == true)
	{
		for (int i = 0; i < figureCount; i++)
		{
			// 삼각형 이동
			fg[i].mX += fg[i].dx;
			fg[i].mY += fg[i].dy;

			// 경계에 닿으면 반대 방향으로 변경
			if (fg[i].mX + fg[i].width / 2 > 1.0f)			//오른쪽
			{
				fg[i].mX = 1.0f - fg[i].width / 2 - 0.001f;  // 벽에서 0.001f 떨어지게 조정
				fg[i].dx = -fg[i].dx;  // X 방향 반전
				fg[i].dir = 3;
			}
			else if (fg[i].mX - fg[i].width / 2 < -1.0f)	//왼쪽
			{
				fg[i].mX = -1.0f + fg[i].width / 2 + 0.001f;  // 벽에서 0.001f 떨어지게 조정
				fg[i].dx = -fg[i].dx;  // X 방향 반전
				fg[i].dir = 1;
			}

			if (fg[i].mY + fg[i].height / 2 > 1.0f)			//위쪽
			{
				fg[i].mY = 1.0f - fg[i].height / 2 - 0.001f;  // 벽에서 0.001f 떨어지게 조정
				fg[i].dy = -fg[i].dy;  // Y 방향 반전
				fg[i].dir = 2;
			}
			else if (fg[i].mY - fg[i].height / 2 < -1.0f)	//아래쪽
			{
				fg[i].mY = -1.0f + fg[i].height / 2 + 0.001f;  // 벽에서 0.001f 떨어지게 조정
				fg[i].dy = -fg[i].dy;  // Y 방향 반전
				fg[i].dir = 0;
			}
			//회전 및 좌표 적용(구조체 좌표 -> 실제 삼각형 좌표)
			switch (fg[i].dir)
			{
			case 0:	//North
			{
				//top			(top)
				figure[i][0][0] = fg[i].mX;
				figure[i][0][1] = fg[i].mY + (fg[i].height / 2.0f);
				figure[i][0][2] = 0.0f;
				//leftbottom	(leftbottom)
				figure[i][1][0] = fg[i].mX - (fg[i].width / 2.0f);
				figure[i][1][1] = fg[i].mY - (fg[i].height / 2.0f);
				figure[i][1][2] = 0.0f;
				//rightbottom	(rightbottom)
				figure[i][2][0] = fg[i].mX + (fg[i].width / 2.0f);
				figure[i][2][1] = fg[i].mY - (fg[i].height / 2.0f);
				figure[i][2][2] = 0.0f;
				break;
			}
			case 1:	//East
			{
				//leftbottom	(lefttop)
				figure[i][0][0] = fg[i].mX - (fg[i].height / 2.0f);
				figure[i][0][1] = fg[i].mY + (fg[i].width / 2.0f);
				figure[i][0][2] = 0.0f;
				//rightbottom	(leftbottom)
				figure[i][1][0] = fg[i].mX - (fg[i].height / 2.0f);
				figure[i][1][1] = fg[i].mY - (fg[i].width / 2.0f);
				figure[i][1][2] = 0.0f;
				//top			(right)
				figure[i][2][0] = fg[i].mX + (fg[i].height / 2.0f);
				figure[i][2][1] = fg[i].mY;
				figure[i][2][2] = 0.0f;
				break;
			}
			case 2:	//South
			{
				//leftbottom	(righttop)
				figure[i][1][0] = fg[i].mX + (fg[i].width / 2.0f);
				figure[i][1][1] = fg[i].mY + (fg[i].height / 2.0f);
				figure[i][1][2] = 0.0f;
				//rightbottom	(lefttop)
				figure[i][2][0] = fg[i].mX - (fg[i].width / 2.0f);
				figure[i][2][1] = fg[i].mY + (fg[i].height / 2.0f);
				figure[i][2][2] = 0.0f;
				//top			(bottom)
				figure[i][0][0] = fg[i].mX;
				figure[i][0][1] = fg[i].mY - (fg[i].height / 2.0f);
				figure[i][0][2] = 0.0f;
				break;
			}
			case 3:	//West
			{
				//top			(left)
				figure[i][0][0] = fg[i].mX - (fg[i].height / 2.0f);
				figure[i][0][1] = fg[i].mY;
				figure[i][0][2] = 0.0f;
				//rightbottom	(righttop)
				figure[i][1][0] = fg[i].mX + (fg[i].height / 2.0f);
				figure[i][1][1] = fg[i].mY + (fg[i].width / 2.0f);
				figure[i][1][2] = 0.0f;
				//leftbottom	(rightbottom)
				figure[i][2][0] = fg[i].mX + (fg[i].height / 2.0f);
				figure[i][2][1] = fg[i].mY - (fg[i].width / 2.0f);
				figure[i][2][2] = 0.0f;
				break;
			}
			}
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferSubData(GL_ARRAY_BUFFER, i * 9 * sizeof(GLfloat), 9 * sizeof(GLfloat), figure[i]);
		}
		glutPostRedisplay();  // 화면 재출력
		glutTimerFunc(16, TimerFunction1, 1);  // 약 60fps 간격으로 타이머 재설정
	}
}
// 지그재그 이동 
void TimerFunction2(int value)
{
	if (timer_2 == true)
	{
		for (int i = 0; i < figureCount; i++)
		{
			switch (fg[i].dir)
			{
			case 0:  // North
			case 2:  // South
				fg[i].mY += fg[i].dy;

				// 세로 이동 중 벽에 부딪힌 경우 (상하 벽 체크)
				if (fg[i].mY + (fg[i].height / 2) >= 1.0f)  // 상단 벽
				{
					fg[i].mY = 1.0f - fg[i].height / 2 - 0.001f;  // 벽에서 0.001f 떨어지게 조정
					fg[i].dy *= -1.0f;
					fg[i].verticalMoveCount = 50;
					fg[i].dir = 2;  // 남쪽으로 전환
				}
				else if (fg[i].mY - (fg[i].height / 2) <= -1.0f)  // 하단 벽
				{
					fg[i].mY = -1.0f + fg[i].height / 2 + 0.001f;  // 벽에서 0.001f 떨어지게 조정
					fg[i].dy *= -1.0f;
					fg[i].verticalMoveCount = 50;
					fg[i].dir = 0;  // 북쪽으로 전환
				}

				fg[i].verticalMoveCount--;

				// 세로로 50번 이동한 후 가로 이동으로 전환
				if (fg[i].verticalMoveCount <= 0)
				{
					fg[i].dir = (fg[i].dx > 0) ? 1 : 3;  // dx가 양수면 동쪽, 음수면 서쪽
					fg[i].verticalMoveCount = 50;  // 가로로 이동하기 전에 카운트 초기화
				}
				break;

			case 1:  // East
			case 3:  // West
				fg[i].mX += fg[i].dx;
				
				// 가로 이동 중 벽에 부딪힌 경우 (좌우 벽 체크)
				if (fg[i].mX + (fg[i].width / 2) >= 1.0f)  // 오른쪽 벽
				{
					fg[i].mX = 1.0f - fg[i].width / 2 - 0.001f;  // 벽에서 0.001f 떨어지게 조정
					fg[i].dx *= -1.0f;
					fg[i].verticalMoveCount = 50;
					fg[i].dir = (fg[i].dy > 0) ? 0 : 2;  // dy가 양수면 북쪽, 음수면 남쪽
				}
				else if (fg[i].mX - (fg[i].width / 2) <= -1.0f)  // 왼쪽 벽
				{
					fg[i].mX = -1.0f + fg[i].width / 2 + 0.001f;  // 벽에서 0.001f 떨어지게 조정
					fg[i].dx *= -1.0f;
					fg[i].verticalMoveCount = 50;
					fg[i].dir = (fg[i].dy > 0) ? 0 : 2;  // dy가 양수면 북쪽, 음수면 남쪽
				}
				break;
			}

			// 회전 및 좌표 적용(구조체 좌표 -> 실제 삼각형 좌표)
			switch (fg[i].dir)
			{
			case 0:	//North
			{
				//top			(top)
				figure[i][0][0] = fg[i].mX;
				figure[i][0][1] = fg[i].mY + (fg[i].height / 2.0f);
				figure[i][0][2] = 0.0f;
				//leftbottom	(leftbottom)
				figure[i][1][0] = fg[i].mX - (fg[i].width / 2.0f);
				figure[i][1][1] = fg[i].mY - (fg[i].height / 2.0f);
				figure[i][1][2] = 0.0f;
				//rightbottom	(rightbottom)
				figure[i][2][0] = fg[i].mX + (fg[i].width / 2.0f);
				figure[i][2][1] = fg[i].mY - (fg[i].height / 2.0f);
				figure[i][2][2] = 0.0f;
				break;
			}
			case 1:	//East
			{
				//leftbottom	(lefttop)
				figure[i][0][0] = fg[i].mX - (fg[i].height / 2.0f);
				figure[i][0][1] = fg[i].mY + (fg[i].width / 2.0f);
				figure[i][0][2] = 0.0f;
				//rightbottom	(leftbottom)
				figure[i][1][0] = fg[i].mX - (fg[i].height / 2.0f);
				figure[i][1][1] = fg[i].mY - (fg[i].width / 2.0f);
				figure[i][1][2] = 0.0f;
				//top			(right)
				figure[i][2][0] = fg[i].mX + (fg[i].height / 2.0f);
				figure[i][2][1] = fg[i].mY;
				figure[i][2][2] = 0.0f;
				break;
			}
			case 2:	//South
			{
				//leftbottom	(righttop)
				figure[i][1][0] = fg[i].mX + (fg[i].width / 2.0f);
				figure[i][1][1] = fg[i].mY + (fg[i].height / 2.0f);
				figure[i][1][2] = 0.0f;
				//rightbottom	(lefttop)
				figure[i][2][0] = fg[i].mX - (fg[i].width / 2.0f);
				figure[i][2][1] = fg[i].mY + (fg[i].height / 2.0f);
				figure[i][2][2] = 0.0f;
				//top			(bottom)
				figure[i][0][0] = fg[i].mX;
				figure[i][0][1] = fg[i].mY - (fg[i].height / 2.0f);
				figure[i][0][2] = 0.0f;
				break;
			}
			case 3:	//West
			{
				//top			(left)
				figure[i][0][0] = fg[i].mX - (fg[i].height / 2.0f);
				figure[i][0][1] = fg[i].mY;
				figure[i][0][2] = 0.0f;
				//rightbottom	(righttop)
				figure[i][1][0] = fg[i].mX + (fg[i].height / 2.0f);
				figure[i][1][1] = fg[i].mY + (fg[i].width / 2.0f);
				figure[i][1][2] = 0.0f;
				//leftbottom	(rightbottom)
				figure[i][2][0] = fg[i].mX + (fg[i].height / 2.0f);
				figure[i][2][1] = fg[i].mY - (fg[i].width / 2.0f);
				figure[i][2][2] = 0.0f;
				break;
			}
			}
			
			// VBO 업데이트
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferSubData(GL_ARRAY_BUFFER, i * 9 * sizeof(GLfloat), 9 * sizeof(GLfloat), figure[i]);
		}

		glutPostRedisplay();  // 화면 재출력
		glutTimerFunc(16, TimerFunction2, 1);  // 약 60fps 간격으로 타이머 재설정
	}
}

