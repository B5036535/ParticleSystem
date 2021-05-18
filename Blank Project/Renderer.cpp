#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0, 0, Vector3(0, 0, 0));

	triangle = Mesh::GenerateTriangle();
	basicShader = new Shader("vertex_basicInstanced.glsl", "fragment_basic.glsl");
	position = Vector3(0, 0, -50);
	
	//int i = 0;
	//float offset = 0.1f;
	//for(int y = -10; y < 10; y += 2)
	//{
	//	for (int x = -10; x < 10; x += 2)
	//	{
	//		Vector2 translation
	//		(
	//			(float)x / 10.f + offset
	//			,
	//			(float)y / 10.f + offset
	//		);

	//		offsets[i++] = translation;
	//	}
	//}


	//glGenBuffers(1, &instanceVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2) * 100, &offsets[0], GL_STATIC_DRAW);

	//glEnableVertexAttribArray();
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);
	//glVertexAttribDivisor(2, 1);
	//glObjectLabel(GL_BUFFER, instanceVBO, -1, "Offset");


	if (!basicShader->LoadSuccess()) {
		return;
	}

	init = true;
}
Renderer::~Renderer(void) {
	delete camera;
	delete triangle;
	delete basicShader;
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
}

void Renderer::RenderScene() 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	BindShader(basicShader);
	Matrix4 model = Matrix4::Translation(position) * Matrix4::Scale(Vector3(1, 1, 1));
	glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, model.values);
	triangle->Draw();
}

