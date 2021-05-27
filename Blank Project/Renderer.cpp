#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0, 0, Vector3(0, 0, 10));

	triangle = Mesh::GenerateTriangle();
	quad = Mesh::GenerateQuad();
	basicShader = new Shader("vertex_basicInstanced.glsl", "fragment_basic.glsl");
	//basicComputeShader = new ComputeShader("compute_basic.glsl");

	particleSystem = new ParticleSystem(10, quad);

	position = Vector3(0, 0, -5);




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
	modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(0.1, 0.1, 0.1));
	particleSystem->Render(modelMatrix, viewMatrix, projMatrix);
}

