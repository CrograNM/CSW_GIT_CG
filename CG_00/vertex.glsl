// vertex shader : �׸��� ��ɾ ���� vertex array object�κ��� ���ؽ� �Ӽ��� ����
// ���ؽ� ��ȯ, �ؽ�ó ��ǥ ����

#version 330 core				// ��� ���� ����
					
//in type in_variable_name;		// �ʿ��� ���� ���� 1
//out type out_variable_name;	// �ʿ��� ���� ���� 2
//in vec3 vPos;					// (����)

void main()						// �����Լ� ���ڰ� ����
{
	// �Է� ���� ó��

	// �ʿ��� �׷��� �۾� ����

	// ��� ���� ����
	const vec4 vertex[3] = vec4[3] (vec4(-0.25, -0.25, 0.5, 1.0), 
									vec4(0.25, -0.25, 0.5, 1.0),
									vec4(0.0, 0.25, 0.5, 1.0));
	gl_Position = vertex[gl_VertexID];
}