// fragment shader : Rasterizer에서 생성된 프래그먼트를 일련의 색상과 깊이 값으로 처리
// 색상 설정, 조명 설정

#version 330 core				// 사용 버전 선언
					
//in type in_variable_name;		// 필요한 변수 선언 1
//out type out_variable_name;	// 필요한 변수 선언 2
//out vec4 FragColor;			// (예시)
out vec4 color;

void main()						// 메인함수 인자값 없음
{
	// 입력 값을 처리

	// 필요한 그래픽 작업 수행

	// 출력 변수 저장
	color = vec4(1.0, 0.0, 0.0, 1.0);	//빨강색
}