#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

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

GLfloat rColor = 0.0;
GLfloat gColor = 0.0;
GLfloat bColor = 0.0;

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

// 스파이럴 생성 함수
void createSpiral(float centerX, float centerY, float length, float angle);
//랜덤 위치에 스파이럴 생성 (개수 i)
void generateRandSpiral(int count);	

// 스파이럴을 구성할 점(point) 벡터 
#define PI 3.141592
std::vector<float> spiralPoints;  // 스파이럴 점 좌표를 저장할 벡터
std::vector<int> spiralSize;	  // 스파이럴을 생성할 때마다의 사이즈를 저장한다. (200, 400 ...)
bool pointMod = true;			

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
int currentPointIndex = 0; // 현재 점의 인덱스
void TimerFunction1(int value);	


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	spiralSize.push_back(0);
	srand(time(0));
	width = clientWidth;
	height = clientHeight;

	//--- 윈도우 생성하기
	glutInit(&argc, argv);									//--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);			//--- 디스플레이 모드 설정
	glutInitWindowPosition(100, 100);						//--- 윈도우의 위치 지정
	glutInitWindowSize(width, height);						//--- 윈도우의 크기 지정
	glutCreateWindow("test 10");							//--- 윈도우 생성(윈도우 이름)

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

	glutTimerFunc(16, TimerFunction1, 1);		// 약 60fps 간격으로 타이머 재설정
	glutDisplayFunc(drawScene);					//--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);					//--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();								//--- 이벤트 처리 시작
}

GLvoid drawScene()
{
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	// VBO 업데이트: spiralPoints의 크기가 변했을 때 다시 초기화
	glBindVertexArray(vao);

	// VBO 업데이트 (새로운 포인트로)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * spiralPoints.size(), spiralPoints.data(), GL_STATIC_DRAW);

	// 색상 VBO 업데이트 (예시로 임의의 색상 사용)
	std::vector<float> colors;
	for (size_t i = 0; i < spiralPoints.size() / 2; ++i)
	{
		colors.push_back(1.0f); // Red
		colors.push_back(1.0f); // Green
		colors.push_back(1.0f); // Blue
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), colors.data(), GL_STATIC_DRAW);

	// 점 크기 설정
	glPointSize(5.0f); // 점의 크기를 5로 설정
	glDrawArrays(GL_POINTS, 0, currentPointIndex); // 현재 점까지만 그리기
	//glDrawArrays(GL_LINES, 0, currentPointIndex); // 선 그리기
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
		pointMod = true;
		break;
	case 'l':
		pointMod = false;
		break;
	case '1': 
		generateRandSpiral(1);
		break;
	case '2':
		generateRandSpiral(2);
		break;
	case '3':
		generateRandSpiral(3);
		break;
	case '4':
		generateRandSpiral(4);
		break;
	case '5':
		generateRandSpiral(5);
		break;
	}
	glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
	//클릭시 몇사분면인지 검사
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

		// 마우스 클릭 위치를 GL 좌표로 변환
		float mX = Win_to_GL_X(x);
		float mY = Win_to_GL_Y(y);

		// 스파이럴 생성
		float randFloat = generateRandomFloat(0.0f, 360.0f);
		createSpiral(mX, mY, 0.3f, randFloat);
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
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo); // 2개의 VBO를 지정하고 할당하기

	//--- 1번째 VBO 설정 : 좌표값
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * spiralPoints.size(), spiralPoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO 설정 : 색상
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * spiralPoints.size(), spiralPoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//vbo[0], vbo[1]에 해당 정점들의 위치와 색상이 저장되었다.
}

// 스파이럴 관련 함수
void createSpiral(float centerX, float centerY, float length, float angle)
{
	switch (pointMod)
	{
	case true:
		spiralSize.push_back(100 + spiralSize[spiralSize.size() - 1]);
		break;
	case false:
		spiralSize.push_back(1000 + spiralSize[spiralSize.size() - 1]);
		break;
	}
	std::cout << "--Create Spiral--\n";
	//spiralPoints.clear(); // 이전 스파이럴 점 제거

	int numPoints = 100;
	float angleIncrement;
	if(pointMod == true)
	{
		numPoints = 100;		// 스파이럴의 점 수
		angleIncrement = 0.3f;	// 각도 증가량
	}
	else
	{
		numPoints = 1000;		// 스파이럴의 점 수
		angleIncrement = 0.03f; // 각도 증가량
	}

	float radius = 0.0f; // 초기 반지름
	float x, y;
	float centerX2, centerY2;

	// 첫 번째 스파이럴을 생성
	for (int i = 0; i < numPoints; i++)
	{
		// 각도와 반지름 계산
		if (i < numPoints / 2)
		{
			// 첫 번째 절반: 반지름 증가
			angle += angleIncrement; // 각도 증가
			radius += length / (float)numPoints; // 반지름 증가

			// 스파이럴 점 계산
			x = centerX + radius * cos(angle);
			y = centerY + radius * sin(angle);
			
			centerX2 = x + (x - centerX);
			centerY2 = y + (y - centerY);

		}	
		else
		{
			// 두 번째 절반: 반지름 감소하고 각도 반전
			angle -= angleIncrement; // 각도 감소
			radius -= length / (float)numPoints; // 반지름 감소

			// 스파이럴 점 계산
			x = centerX2 - radius * cos(angle);
			y = centerY2 - radius * sin(angle);
		}

		// 점을 벡터에 추가
		spiralPoints.push_back(x);
		spiralPoints.push_back(y);
	}

	std::cout << "Number of spiral points: " << spiralPoints.size() << std::endl;
}
void generateRandSpiral(int count)
{
	for (int i = 0; i < count; i++)
	{
		float randX = generateRandomFloat(-1.0f, 1.0f);
		float randY = generateRandomFloat(-1.0f, 1.0f);
		float randFloat = generateRandomFloat(0.0f, 360.0f);
		createSpiral(randX, randY, 0.3f, randFloat);
	}
}
// 스파이럴 애니메이션
void TimerFunction1(int value)
{
	if (currentPointIndex < spiralPoints.size() / 2)
	{	
		//새로운 스파이럴이 생성 될때마다 배경 변경
		for (int i = 1; i < spiralSize.size(); i++)
		{
			if (currentPointIndex == spiralSize[i - 1])
			{
				rColor = generateRandomFloat(0.0f, 1.0f);
				gColor = generateRandomFloat(0.0f, 1.0f);
				bColor = generateRandomFloat(0.0f, 1.0f);
				break;
			}
		}
		
		currentPointIndex++; // 점 하나씩 추가
		glutPostRedisplay(); // 화면 다시 그리기
	}
	glutTimerFunc(4, TimerFunction1, 1);
}