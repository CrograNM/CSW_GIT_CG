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
#define clientWidth 800
#define clientHeight 600

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

// 도형 관련 함수들
void initFigure();
void moveFigureRand(char dir);
// 최대 10개의 도형을 저장할 변수
#define MAX_FIGURE 10
#define FIGURE_SIZE 0.02f

GLfloat figure[MAX_FIGURE][6][3];  
GLfloat colorData[MAX_FIGURE][6][3];
int figureCount = 0;
int figureType = 1;					//1:point,  2:line,  3:tri,  4:rect
int typeArray[MAX_FIGURE] = { 0, };	//1:point,  2:line,  3:tri,  4:rect

//필요 변수 선언
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
	for (int i = 0; i < figureCount; i++)
	{
		int vertexCount;
		switch (typeArray[i])
		{
		case 1:
			vertexCount = 6;
			glDrawArrays(GL_TRIANGLES, i * 6, vertexCount);
			break;
		case 2:
			vertexCount = 2;
			glDrawArrays(GL_LINES, i * 6, vertexCount);
			break;
		case 3:
			vertexCount = 3;
			glDrawArrays(GL_TRIANGLES, i * 6, vertexCount);
			break;
		case 4:
			vertexCount = 6;
			glDrawArrays(GL_TRIANGLES, i * 6, vertexCount);
			break;
		}
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
	case 'p':		
	{
		std::cout << "--point mod--\n";
		figureType = 1;
		break;
	}
	case 'l':		
	{
		std::cout << "--line mod--\n";
		figureType = 2;
		break;
	}
	case 't':		
	{
		std::cout << "--tri mod--\n";
		figureType = 3;
		break;
	}
	case 'r':		
	{
		std::cout << "--rect mod--\n";
		figureType = 4;
		break;
	}
	case 'c': 
	{
		std::cout << "--Clear client--\n";
		figureCount = 0;
		figureType = 1;
		initFigure();
		break;
	}
	case 'w': case 'a': case 's': case 'd':
	{
		moveFigureRand(key);
		break;
	}
	}
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && figureCount < 10)
	{
		// 마우스 클릭 위치를 GL 좌표로 변환
		float mX = Win_to_GL_X(x);
		float mY = Win_to_GL_Y(y);

		float left = mX - FIGURE_SIZE * 3;
		float right = mX + FIGURE_SIZE * 3;
		float top = mY + FIGURE_SIZE * 4;
		float bottom = mY - FIGURE_SIZE * 4;

		if (figureType == 1)
		{
			typeArray[figureCount] = 1;
			std::cout << "Draw : point\n";
			left = mX - FIGURE_SIZE * 3		/ 4;
			right = mX + FIGURE_SIZE * 3	/ 4;
			top = mY + FIGURE_SIZE * 4		/ 4;
			bottom = mY - FIGURE_SIZE * 4	/ 4;
			// 두개의 삼각형 좌표로 사각형 생성
			//왼쪽 삼각형 
			figure[figureCount][0][0] = left;
			figure[figureCount][0][1] = top;
			figure[figureCount][0][2] = 0.0f;

			figure[figureCount][1][0] = left;
			figure[figureCount][1][1] = bottom;
			figure[figureCount][1][2] = 0.0f;

			figure[figureCount][2][0] = right;
			figure[figureCount][2][1] = bottom;
			figure[figureCount][2][2] = 0.0f;

			//오른쪽 삼각형
			figure[figureCount][3][0] = left;
			figure[figureCount][3][1] = top;
			figure[figureCount][3][2] = 0.0f;

			figure[figureCount][4][0] = right;
			figure[figureCount][4][1] = top;
			figure[figureCount][4][2] = 0.0f;

			figure[figureCount][5][0] = right;
			figure[figureCount][5][1] = bottom;
			figure[figureCount][5][2] = 0.0f;
			
			for (int i = 0; i < 6; i++)
			{
				colorData[figureCount][i][0] = 1.0f; // R
				colorData[figureCount][i][1] = 0.0f; // G
				colorData[figureCount][i][2] = 0.0f; // B
			}
		}
		else if (figureType == 2)
		{
			typeArray[figureCount] = 2;
			std::cout << "Draw : line\n";
			// 두개의 버텍스로 라인 생성 
			figure[figureCount][0][0] = left;
			figure[figureCount][0][1] = top;
			figure[figureCount][0][2] = 0.0f;

			figure[figureCount][1][0] = right;
			figure[figureCount][1][1] = bottom;
			figure[figureCount][1][2] = 0.0f;

			figure[figureCount][2][0] = 0;
			figure[figureCount][2][1] = 0;
			figure[figureCount][2][2] = 0;

			figure[figureCount][3][0] = 0;
			figure[figureCount][3][1] = 0;
			figure[figureCount][3][2] = 0;

			figure[figureCount][4][0] = 0;
			figure[figureCount][4][1] = 0;
			figure[figureCount][4][2] = 0;

			figure[figureCount][5][0] = 0;
			figure[figureCount][5][1] = 0;
			figure[figureCount][5][2] = 0;

			for (int i = 0; i < 6; i++)
			{
				colorData[figureCount][i][0] = 0.0f; // R
				colorData[figureCount][i][1] = 1.0f; // G
				colorData[figureCount][i][2] = 0.0f; // B
			}
		}
		else if (figureType == 3)
		{
			typeArray[figureCount] = 3;
			std::cout << "Draw : tri\n";
			// 클릭한 좌표를 중심으로 삼각형의 정점 좌표 설정
			figure[figureCount][0][0] = left;  // 왼쪽 아래
			figure[figureCount][0][1] = bottom;
			figure[figureCount][0][2] = 0.0f;

			figure[figureCount][1][0] = right;  // 오른쪽 아래
			figure[figureCount][1][1] = bottom;
			figure[figureCount][1][2] = 0.0f;

			figure[figureCount][2][0] = mX;         // 위쪽 중앙
			figure[figureCount][2][1] = top;
			figure[figureCount][2][2] = 0.0f;

			// 나머지 초기화
			figure[figureCount][3][0] = 0;
			figure[figureCount][3][1] = 0;
			figure[figureCount][3][2] = 0;
										
			figure[figureCount][4][0] = 0;
			figure[figureCount][4][1] = 0;
			figure[figureCount][4][2] = 0;
										
			figure[figureCount][5][0] = 0;
			figure[figureCount][5][1] = 0;
			figure[figureCount][5][2] = 0;

			for (int i = 0; i < 6; i++)
			{
				colorData[figureCount][i][0] = 0.0f; // R
				colorData[figureCount][i][1] = 0.0f; // G
				colorData[figureCount][i][2] = 1.0f; // B
			}
		}
		else if (figureType == 4)
		{
			typeArray[figureCount] = 4;
			std::cout << "Draw : rect\n";
			// 두개의 삼각형 좌표로 사각형 생성
			//왼쪽 삼각형 
			figure[figureCount][0][0] = left;  
			figure[figureCount][0][1] = top;
			figure[figureCount][0][2] = 0.0f;

			figure[figureCount][1][0] = left;  
			figure[figureCount][1][1] = bottom;
			figure[figureCount][1][2] = 0.0f;

			figure[figureCount][2][0] = right;        
			figure[figureCount][2][1] = bottom;
			figure[figureCount][2][2] = 0.0f;

			//오른쪽 삼각형
			figure[figureCount][3][0] = left;
			figure[figureCount][3][1] = top;
			figure[figureCount][3][2] = 0.0f;

			figure[figureCount][4][0] = right;  
			figure[figureCount][4][1] = top;
			figure[figureCount][4][2] = 0.0f;

			figure[figureCount][5][0] = right;
			figure[figureCount][5][1] = bottom;
			figure[figureCount][5][2] = 0.0f;

			for (int i = 0; i < 6; i++)
			{
				colorData[figureCount][i][0] = 1.0f; // R
				colorData[figureCount][i][1] = 0.0f; // G
				colorData[figureCount][i][2] = 1.0f; // B
			}
		}
		// VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, (figureCount + 1) * 18 * sizeof(GLfloat), figure, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, (figureCount + 1) * 18 * sizeof(GLfloat), colorData, GL_STATIC_DRAW);
		
		figureCount++;			// 삼각형 개수 증가	
		glutPostRedisplay();	// 화면 갱신
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
		for (int j = 0; j < 6; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				figure[i][j][k] = 0;
				colorData[i][j][k] = 1.0f;
			}
		}
		typeArray[i] = 0;
	}
}
void moveFigureRand(char dir)
{
	std::cout << "move random : " << dir << std::endl;
	int randIndex = rand() % figureCount;
	float v = 0.02f;
	switch (dir)
	{
	case 'w':
	{
		figure[randIndex][0][1] += v;
		figure[randIndex][1][1] += v;
		figure[randIndex][2][1] += v;
		figure[randIndex][3][1] += v;
		figure[randIndex][4][1] += v;
		figure[randIndex][5][1] += v;
		break;
	}
	case 'a':
	{
		figure[randIndex][0][0] -= v;
		figure[randIndex][1][0] -= v;
		figure[randIndex][2][0] -= v;
		figure[randIndex][3][0] -= v;
		figure[randIndex][4][0] -= v;
		figure[randIndex][5][0] -= v;
		break;
	}
	case 's':
	{
		figure[randIndex][0][1] -= v;
		figure[randIndex][1][1] -= v;
		figure[randIndex][2][1] -= v;
		figure[randIndex][3][1] -= v;
		figure[randIndex][4][1] -= v;
		figure[randIndex][5][1] -= v;
		break;
	}
	case 'd':
	{
		figure[randIndex][0][0] += v;
		figure[randIndex][1][0] += v;
		figure[randIndex][2][0] += v;
		figure[randIndex][3][0] += v;
		figure[randIndex][4][0] += v;
		figure[randIndex][5][0] += v;
		break;
	}
	}
	// VBO에 새로운 삼각형 좌표 추가
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (figureCount + 1) * 18 * sizeof(GLfloat), figure, GL_STATIC_DRAW);
}