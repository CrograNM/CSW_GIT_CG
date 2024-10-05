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
    std::uniform_real_distribution<float> dis(min, max); // 인자로 범위 설정
    return dis(gen);
}

// 좌표 변환 함수
int GL_to_Win_X(float x)
{
    return (x + 1) * (clientWidth / 2.0f); // 2.0f로 실수 나눗셈
}
int GL_to_Win_Y(float y)
{
    return (1 - y) * (clientHeight / 2.0f); // 2.0f로 실수 나눗셈
}
float Win_to_GL_X(int x)
{
    return (x / (float)clientWidth) * 2 - 1; // 정수 나눗셈 방지
}
float Win_to_GL_Y(int y)
{
    return 1 - (y / (float)clientHeight) * 2; // 정수 나눗셈 방지
}

// 도형 관련 함수들
void initFigure();
int quardrant=0;  //사분면 변수 선언

void initFigure_1();    //1사분면  (우상단)
int figureCount_1 = 0;  //max 3

void initFigure_2();    //2사분면  (우하단)
int figureCount_2 = 0;

void initFigure_3();    //3사분면  (좌하단)
int figureCount_3 = 0;

void initFigure_4();    //4사분면  (좌상단)
int figureCount_4 = 0;

void drawTriangle(float mX, float mY);

// 최대 10개의 도형을 저장할 변수
#define MAX_FIGURE 3
#define FIGURE_SIZE 0.02f

GLfloat divLine[4][3] = {
    {-1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};
GLfloat figure[MAX_FIGURE * 4][3][3];
GLfloat colorData[MAX_FIGURE * 4][3][3];
int figureCount = 0;
int figureType = 1;                         // 1:fill,  2:line
int typeArray[MAX_FIGURE * 4] = { 0, };     // 1:fill,  2:line

// 필요 변수 선언
GLint width, height;
GLchar* vertexSource, * fragmentSource;        //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader;        //--- 세이더 객체
GLuint shaderProgramID;                        //--- 셰이더 프로그램
GLuint vao[2], vbo[3];                        //--- VAO, VBO

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
    glutInit(&argc, argv);                                    //--- glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);            //--- 디스플레이 모드 설정
    glutInitWindowPosition(100, 100);                        //--- 윈도우의 위치 지정
    glutInitWindowSize(width, height);                        //--- 윈도우의 크기 지정
    glutCreateWindow("test 08");                            //--- 윈도우 생성(윈도우 이름)

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

    glutDisplayFunc(drawScene);                    //--- 출력 콜백함수의 지정
    glutReshapeFunc(Reshape);                    //--- 다시 그리기 콜백함수 지정
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMainLoop();                                //--- 이벤트 처리 시작
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

    // 가로, 세로 이등분 선 그리기
    glBindVertexArray(vao[1]);
    glDrawArrays(GL_LINES, 0, 4);

    //--- 사용할 VAO 불러오기 (VAO에 VBO의 값들이 모두 저장되어 있는것)
    glBindVertexArray(vao[0]);
    for (int i = 0; i < figureCount_1; i++)
    {
        switch (typeArray[i])
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
    for (int i = 3; i < 3 + figureCount_2; i++)
    {
        switch (typeArray[i])
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
    for (int i = 6; i < 6 + figureCount_3; i++)
    {
        switch (typeArray[i])
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
    for (int i = 9; i < 9 + figureCount_4; i++)
    {
        switch (typeArray[i])
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
    case 'q':        // 프로그램 종료
    {
        std::cout << "--Quit--\n";
        glutLeaveMainLoop(); // OpenGL 메인 루프 종료
        break;
    }
    case 'a':
    {
        // 면으로 그리기
        std::cout << "--fill mod--\n";
        figureType = 1;
        break;
    }
    case 'b':
    {
        // 선으로 그리기
        std::cout << "--line mod--\n";
        figureType = 2;
        break;
    }
    }
    glutPostRedisplay(); //--- refresh
}
void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // 마우스 클릭 위치를 GL 좌표로 변환
        float mX = Win_to_GL_X(x);
        float mY = Win_to_GL_Y(y);

        //클릭시 몇사분면인지 검사
        if (mX >= 0 && mY >= 0)
        {
            quardrant = 1;
        }
        else if (mX >= 0 && mY < 0)
        {
            quardrant = 2;
        }
        else if (mX < 0 && mY < 0)
        {
            quardrant = 3;
        }
        else if (mX < 0 && mY >= 0)
        {
            quardrant = 4;
        }

        switch (quardrant)
        {
        case 1:
            if (figureCount_1 < 3)
            {
                figureCount = 0 + figureCount_1;
                drawTriangle(mX, mY);
                figureCount_1++;
            }
            break;
        case 2:
            if (figureCount_2 < 3)
            {
                figureCount = 3 + figureCount_2;
                drawTriangle(mX, mY);
                figureCount_2++;
            }
            break;
        case 3:
            if (figureCount_3 < 3)
            {
                figureCount = 6 + figureCount_3;
                drawTriangle(mX, mY);
                figureCount_3++;
            }
            break;
        case 4:
            if (figureCount_4 < 3)
            {
                figureCount = 9 + figureCount_4;
                drawTriangle(mX, mY);
                figureCount_4++;
            }
            break;
        }

        glutPostRedisplay(); // 화면 다시 그리기
    }
}

// VAO, VBO를 초기화
GLvoid InitBuffer()
{
    // 0: 도형 좌표, 1: 색상 데이터, 2: 이등분 선 좌표
    glGenVertexArrays(3, vao);
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

// 셰이더 작성 및 셰이더 프로그램 만들기
char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;

    fptr = fopen(file, "rb"); //---파일열기
    if (!fptr) //---예외처리
        return NULL;
    fseek(fptr, 0, SEEK_END); //---파일 끝으로
    length = ftell(fptr); //---파일 크기 저장
    buf = (char*)malloc(length + 1); //---메모리 할당
    fseek(fptr, 0, SEEK_SET); //---파일 시작으로
    fread(buf, length, 1, fptr); //---소스코드 읽어서 저장
    fclose(fptr); //---파일닫기
    buf[length] = 0; //---문자열 끝에 NULL추가

    return buf; //---버퍼 리턴
}
void make_vertexShaders()
{
    const GLchar* vertexSource = filetobuf("vertex.glsl");

    //---정점 셰이더 객체 만들기
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL); //---셰이더 소스 설정
    glCompileShader(vertexShader);                        //---셰이더 컴파일

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        printf("ERROR: vertex shader 컴파일 실패\n %s\n", errorLog);
    }
}
void make_fragmentShaders()
{
    const GLchar* fragmentSource = filetobuf("fragment.glsl");

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        printf("ERROR: fragment shader 컴파일 실패\n %s\n", errorLog);
    }
}
void make_shaderProgram()
{
    make_vertexShaders();
    make_fragmentShaders();

    //---셰이더 프로그램 객체 만들기
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
        printf("ERROR: shader program 링크 실패\n %s\n", errorLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void initFigure()
{
    for (int i = 0; i < MAX_FIGURE * 4; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                figure[i][j][k] = 0;
                colorData[i][j][k] = 1.0f;
            }
        }
        typeArray[i] = 0;
    }
    // VBO에 새로운 삼각형 좌표 및 색상 데이터 추가
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE * 4) * 9 * sizeof(GLfloat), figure, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, (MAX_FIGURE * 4) * 9 * sizeof(GLfloat), colorData, GL_STATIC_DRAW);
}
void drawTriangle(float mX, float mY)
{
    float left = mX - FIGURE_SIZE * 3;
    float right = mX + FIGURE_SIZE * 3;
    float top = mY + FIGURE_SIZE * 4;
    float bottom = mY - FIGURE_SIZE * 4;

    typeArray[figureCount] = figureType;
    std::cout << "Draw triangle : " << quardrant << "_사분면" << std::endl;
    // 두개의 삼각형 좌표로 사각형 생성
    //왼쪽 삼각형 
    figure[figureCount][0][0] = mX;
    figure[figureCount][0][1] = top;
    figure[figureCount][0][2] = 0.0f;

    figure[figureCount][1][0] = left;
    figure[figureCount][1][1] = bottom;
    figure[figureCount][1][2] = 0.0f;

    figure[figureCount][2][0] = right;
    figure[figureCount][2][1] = bottom;
    figure[figureCount][2][2] = 0.0f;

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