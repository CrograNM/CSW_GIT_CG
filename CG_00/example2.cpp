#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

//사용자 정의 함수
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//필요 변수 선언
GLint width, height;
GLuint shaderProgramID;		//--- 세이더 프로그램 이름
GLuint vertexShader;		//--- 버텍스 세이더 객체
GLuint fragmentShader;		//--- 프래그먼트 세이더 객체

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	width = 500;
	height = 500;

	//--- 윈도우 생성하기
	glutInit(&argc, argv);									//--- glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);			//--- 디스플레이 모드 설정
	glutInitWindowPosition(100, 100);						//--- 윈도우의 위치 지정
	glutInitWindowSize(width, height);						//--- 윈도우의 크기 지정
	glutCreateWindow("example2");							//--- 윈도우 생성(윈도우 이름)

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
	make_vertexShaders();						//--- 버텍스 세이더 만들기
	make_fragmentShaders();						//--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();		//--- 세이더 프로그램 만들기

	glutDisplayFunc(drawScene);					//--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);					//--- 다시 그리기 콜백함수 지정
	glutMainLoop();								//--- 이벤트 처리 시작
}
void make_vertexShaders()
{

}
void make_fragmentShaders()
{

}
GLuint make_shaderProgram()
{
	return 0;
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = 0.0;
	bColor = 1.0;	//--- 배경색을 파랑색으로 설정
	glClearColor(rColor, gColor, bColor, 1.0f);			//--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT);						//--- 설정된 색으로 전체를 칠하기
	glutSwapBuffers();									//--- 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
