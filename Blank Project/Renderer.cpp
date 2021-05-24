#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0, 0, Vector3(0, 0, 0));

	triangle = Mesh::GenerateTriangle();
	quad = Mesh::GenerateQuad();
	basicShader = new Shader("vertex_basicInstanced.glsl", "fragment_basic.glsl");
	//basicComputeShader = new ComputeShader("compute_basic.glsl");

	particleSystem = new ParticleSystem(100, quad);

	position = Vector3(0, 0, 0);
	

	//offsets[0] = Vector2(-20, 0);
	//offsets[1] = Vector2( 20, 0);

	//int i = 0;
	//
	//for (int y = 0; y < NUM_OF_ROWS; y++)
	//{
	//	for (int x = 0; x < NUM_OF_COLS; x++)
	//	{
	//		float posX = -10.f + x * 2;
	//		float posY = -10.f + y * 2;
	//		offsets[i] = Vector2(posX, posY);
	//		i++;
	//	}
	//}
	//
	//glGenBuffers(1, &SSBO_Test);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_Test);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_OF_INSTANCES * sizeof(Vector2), offsets, GL_STREAM_DRAW);
	//
	////int block = glGetUniformBlockIndex(basicShader->GetProgram(), "Test Block");
	//int slot = 6;
	////glUniformBlockBinding(basicShader->GetProgram(), block, slot);
	////for (int i = 0; i < NUM_OF_INSTANCES; i++)
	////{
	////	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, SSBO_Test, i, sizeof(Vector2));
	////}
	//
	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, SSBO_Test, 0, sizeof(Vector2) * NUM_OF_INSTANCES);
	////glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, SSBO_Test, 1, sizeof(Vector2));



	if (!basicShader->LoadSuccess()) {
		return;
	}

	init = true;
}
Renderer::~Renderer(void) 
{
	delete camera;
	delete triangle;
	delete quad;
	delete basicShader;
	delete particleSystem;
	//delete basicComputeShader;
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	particleSystem->Update(dt);
	//glUseProgram(basicComputeShader->GetProgram());
	//basicComputeShader->Dispatch(NUM_OF_INSTANCES, 1, 1);
	//glUseProgram(0);
}

void Renderer::RenderScene() 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();
	modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(10000000, 10000000, 1));
	particleSystem->Render(modelMatrix, viewMatrix, projMatrix);
}

