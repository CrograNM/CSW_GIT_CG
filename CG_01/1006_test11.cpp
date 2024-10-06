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
void initFigure(); // 모든 사분면에 선, 삼각형, 사각형, 오각형 세팅
void initQuardrant(int quardrant);
// 사분면 그리기
GLfloat divLine[4][3] = {
	{-1.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},
	{0.0f, -1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f}
};

// 총 5개의 도형을 그린다. -> 사분면당 총 넷 + 단독 도형 하나
#define MAX_FIGURE 5
#define FIGURE_SIZE 0.1f

// 각 사분면의 중앙값			1		 2		 3		 4		(각 인덱스가 해당 사분면)
float mX[5] =	{0.0f,		0.5f,	 0.5f,	-0.5f,	-0.5f};
float mY[5] =	{0.0f,		0.5f,	-0.5f,	-0.5f,	 0.5f};

typedef struct FIGURE
{
	bool exist;		//존재여부 : 단독 or 모든 사분면
	//int fType;	//도형타입 : 선, 삼각형, 사각형, 오각형
}FIGURE;
FIGURE fg[5];		//fg[사분면], 1~4사분면, 0:중앙 

#define TRI_COUNT 5
GLfloat	figure[MAX_FIGURE][TRI_COUNT * 3][3];	// 중앙값을 이용해, 삼각형 5개를 쓰자
GLfloat	colorData[MAX_FIGURE][TRI_COUNT * 3][3];

// 아래 변수들이 필요한지는 나중에 체크
int figureCount = 0;						
int figureType = 1;						
int typeArray[MAX_FIGURE] = { 0, };		

// 필요 변수 선언
GLint width, height;
GLchar* vertexSource, * fragmentSource;			//--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader;			//--- 세이더 객체
GLuint shaderProgramID;							//--- 셰이더 프로그램
GLuint vao[2], vbo[3];							//--- VAO, VBO

// 사용자 정의 함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid InitBuffer();

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
	glutCreateWindow("test 11");							//--- 윈도우 생성(윈도우 이름)

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
	glutMainLoop();								//--- 이벤트 처리 시작
}

GLvoid drawScene()
{
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 가로, 세로 이등분 선 그리기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_LINES, 0, 4);

	//--- 사용할 VAO 불러오기 (VAO에 VBO의 값들이 모두 저장되어 있는 것)
	glBindVertexArray(vao[0]);

	for (int i = 0; i < MAX_FIGURE; i++)
	{
		if (fg[i].exist == true)  // 도형이 존재하는 경우에만 그리기
		{
			// 각 도형의 인덱스를 기반으로 삼각형 그리기
			glDrawArrays(GL_TRIANGLES, i*15, 15);  // 총 5개의 삼각형을 그리기 위해 15개의 인덱스 사용
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
	glGenVertexArrays(2, vao);
	glGenBuffers(3, vbo);

	// 도형 및 색상 데이터용 VAO, VBO 초기화
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(figure), figure, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);

	// 이등분 선 좌표용 VAO, VBO 초기화
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(divLine), divLine, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
}

void initFigure()
{
	for (int i = 0; i < MAX_FIGURE; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				figure[i][j][k] = 0.0f;
				colorData[i][j][k] = 0.0f;
			}
		}
		typeArray[i] = 0;
	}
	// VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE) * (TRI_COUNT * 9) * sizeof(GLfloat), figure, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE) * (TRI_COUNT * 9) * sizeof(GLfloat), colorData, GL_STATIC_DRAW);

	//사분면 초기화
	initQuardrant(0);
	initQuardrant(1);
	initQuardrant(2);
	initQuardrant(3);
	initQuardrant(4);

	// VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
}
void initQuardrant(int quardrant)
{
	int x = 0;	//가독성을 위한 x
	int y = 1;	//가독성을 위한 y

	fg[quardrant].exist = true;

	float X[6];
	float Y[6];
	switch (quardrant)
	{
	case 0:
	{	//0사분면 : 선
		//0번 정점(중심점)
		X[0] = mX[quardrant];
		Y[0] = mY[quardrant];		
		//1번 정점
		X[1] =  mX[quardrant] - FIGURE_SIZE;
		Y[1] =  mY[quardrant] - FIGURE_SIZE;
		//2번 정점
		X[2] = mX[quardrant] + FIGURE_SIZE;
		Y[2] = mY[quardrant] + FIGURE_SIZE;
		//3번 정점
		X[3] = mX[quardrant];
		Y[3] = mY[quardrant];	
		//4번 정점
		X[4] = mX[quardrant];
		Y[4] = mY[quardrant];
		//5번 정점
		X[5] = mX[quardrant];
		Y[5] = mY[quardrant];
		break;
	}
	case 1:
	{	//1사분면 : 삼각형
		//0번 
		X[0] = mX[quardrant];
		Y[0] = mY[quardrant];
		//1번 
		X[1] = mX[quardrant] - FIGURE_SIZE;
		Y[1] = mY[quardrant] - FIGURE_SIZE;
		//2번 
		X[2] = mX[quardrant] + FIGURE_SIZE;
		Y[2] = mY[quardrant] - FIGURE_SIZE;
		//3번 
		X[3] = mX[quardrant];
		Y[3] = mY[quardrant] + FIGURE_SIZE;
		//4번 
		X[4] = mX[quardrant];
		Y[4] = mY[quardrant] + FIGURE_SIZE;
		//5번 
		X[5] = mX[quardrant];
		Y[5] = mY[quardrant] + FIGURE_SIZE;
		break;
	}
	case 2:
	{	//2사분면 : 오각형
		//0번 
		X[0] = mX[quardrant];
		Y[0] = mY[quardrant];
		//1번 
		X[1] = mX[quardrant] - FIGURE_SIZE;
		Y[1] = mY[quardrant] - FIGURE_SIZE;
		//2번 
		X[2] = mX[quardrant] + FIGURE_SIZE;
		Y[2] = mY[quardrant] - FIGURE_SIZE;
		//3번 
		X[3] = mX[quardrant];
		Y[3] = mY[quardrant] + FIGURE_SIZE;
		//4번 
		X[4] = mX[quardrant] - FIGURE_SIZE;
		Y[4] = mY[quardrant] + FIGURE_SIZE;
		//5번 
		X[5] = mX[quardrant] + FIGURE_SIZE;
		Y[5] = mY[quardrant] + FIGURE_SIZE;
		break;
	}
	case 3:
	{	//3사분면 : 사각형
		//0번 
		X[0] = mX[quardrant];
		Y[0] = mY[quardrant];
		//1번 
		X[1] = mX[quardrant] - FIGURE_SIZE;
		Y[1] = mY[quardrant] - FIGURE_SIZE;
		//2번 
		X[2] = mX[quardrant] + FIGURE_SIZE;
		Y[2] = mY[quardrant] - FIGURE_SIZE;
		//3번 
		X[3] = mX[quardrant];
		Y[3] = mY[quardrant] + FIGURE_SIZE;
		//4번 
		X[4] = mX[quardrant] - FIGURE_SIZE;
		Y[4] = mY[quardrant] + FIGURE_SIZE;
		//5번 
		X[5] = mX[quardrant] + FIGURE_SIZE;
		Y[5] = mY[quardrant] + FIGURE_SIZE;
		break;
	}
	case 4:
	{	//4사분면 : 선
		//0번 
		X[0] = mX[quardrant];
		Y[0] = mY[quardrant];
		//1번 
		X[1] = mX[quardrant] - FIGURE_SIZE;
		Y[1] = mY[quardrant] - FIGURE_SIZE;
		//2번 
		X[2] = mX[quardrant] + FIGURE_SIZE;
		Y[2] = mY[quardrant] + FIGURE_SIZE;
		//3번 
		X[3] = mX[quardrant];
		Y[3] = mY[quardrant];
		//4번 
		X[4] = mX[quardrant];
		Y[4] = mY[quardrant];
		//5번 
		X[5] = mX[quardrant];
		Y[5] = mY[quardrant];
		break;
	}
	}

	//정점 15개에 각 인덱스를 대입한다. (삼각형 5개 * 정점 3개 = 정점 15개)
	if (true)
	{
		// 012인덱스
		figure[quardrant][0][x] = X[0];
		figure[quardrant][0][y] = Y[0];

		figure[quardrant][1][x] = X[1];
		figure[quardrant][1][y] = Y[1];

		figure[quardrant][2][x] = X[2];
		figure[quardrant][2][y] = Y[2];

		// 014인덱스
		figure[quardrant][3][x] = X[0];
		figure[quardrant][3][y] = Y[0];

		figure[quardrant][4][x] = X[1];
		figure[quardrant][4][y] = Y[1];

		figure[quardrant][5][x] = X[4];
		figure[quardrant][5][y] = Y[4];

		// 025인덱스
		figure[quardrant][6][x] = X[0];
		figure[quardrant][6][y] = Y[0];

		figure[quardrant][7][x] = X[2];
		figure[quardrant][7][y] = Y[2];

		figure[quardrant][8][x] = X[5];
		figure[quardrant][8][y] = Y[5];

		// 034인덱스
		figure[quardrant][9][x] = X[0];
		figure[quardrant][9][y] = Y[0];

		figure[quardrant][10][x] = X[3];
		figure[quardrant][10][y] = Y[3];

		figure[quardrant][11][x] = X[4];
		figure[quardrant][11][y] = Y[4];

		// 035인덱스
		figure[quardrant][12][x] = X[0];
		figure[quardrant][12][y] = Y[0];

		figure[quardrant][13][x] = X[3];
		figure[quardrant][13][y] = Y[3];

		figure[quardrant][14][x] = X[5];
		figure[quardrant][14][y] = Y[5];
	}
	float random1 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	float random2 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	float random3 = generateRandomFloat(0.0f, 1.0f); //0~1의 값을 고정시킴
	for (int i = 0; i < 6; i++)
	{
		colorData[quardrant][i][0] = random1; // R
		colorData[quardrant][i][1] = random2; // G
		colorData[quardrant][i][2] = random3; // B
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, quardrant * (TRI_COUNT * 9) * sizeof(GLfloat), (TRI_COUNT * 9) * sizeof(GLfloat), figure[quardrant]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, quardrant * (TRI_COUNT * 9) * sizeof(GLfloat), (TRI_COUNT * 9) * sizeof(GLfloat), colorData[quardrant]);
}