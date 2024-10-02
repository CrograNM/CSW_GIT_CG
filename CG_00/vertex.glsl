// vertex shader : 그리기 명령어에 의해 vertex array object로부터 버텍스 속성을 설정
// 버텍스 변환, 텍스처 좌표 설정

#version 330 core				// 사용 버전 선언
					
//in type in_variable_name;		// 필요한 변수 선언 1
//out type out_variable_name;	// 필요한 변수 선언 2
//in vec3 vPos;					// (예시)

void main()						// 메인함수 인자값 없음
{
	// 입력 값을 처리

	// 필요한 그래픽 작업 수행

	// 출력 변수 저장
	const vec4 vertex[3] = vec4[3] (vec4(-0.25, -0.25, 0.5, 1.0), 
									vec4(0.25, -0.25, 0.5, 1.0),
									vec4(0.0, 0.25, 0.5, 1.0));
	gl_Position = vertex[gl_VertexID];
}