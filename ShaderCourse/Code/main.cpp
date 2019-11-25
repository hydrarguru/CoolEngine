#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Texture.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "Screen.h"
#include "CubeMap.h"
#include "PointLight.h"
#include "SpotLight.h"
//
bool ShouldQuit = false;
bool KeyState[GLFW_KEY_LAST] = { false };
Camera TheCamera;

double MouseX = 0.0;
double MouseY = 0.0;

bool CameraMovementActive = false;

Mesh TriangleMesh;
Mesh QuadMesh;
Mesh CubeMesh;
Material DefaultMaterial;
Material SkyboxMaterial;
Material LightSourceMaterial;
Texture CherylTexture;
CubeMap SkyboxTexture;
PointLight MainLight;
PointLight SecondLight;
SpotLight FlashLight;

const float CubeVertexData[] = {
	// FRONT FACE
	-1.f, -1.f, +1.f,		0.f, 0.f, 1.f,		0.f, 0.f,	// 0
	+1.f, -1.f, +1.f,		0.f, 0.f, 1.f,		1.f, 0.f,	// 1
	+1.f, +1.f, +1.f,		0.f, 0.f, 1.f,		1.f, 1.f,	// 2
	-1.f, +1.f, +1.f,		0.f, 0.f, 1.f,		0.f, 1.f,	// 3

	// BACK FACE
	-1.f, -1.f, -1.f,		0.f, 0.f, -1.f,		0.f, 0.f,	// 4
	+1.f, -1.f, -1.f,		0.f, 0.f, -1.f,		1.f, 0.f,	// 5
	+1.f, +1.f, -1.f,		0.f, 0.f, -1.f,		1.f, 1.f,	// 6
	-1.f, +1.f, -1.f,		0.f, 0.f, -1.f,		0.f, 1.f,	// 7

	// RIGHT FACE
	+1.f, -1.f, +1.f,		1.f, 0.f, 0.f,		0.f, 0.f,	// 8
	+1.f, -1.f, -1.f,		1.f, 0.f, 0.f,		1.f, 0.f,	// 9
	+1.f, +1.f, -1.f,		1.f, 0.f, 0.f,		1.f, 1.f,	// 10
	+1.f, +1.f, +1.f,		1.f, 0.f, 0.f,		0.f, 1.f,	// 11

	// LEFT FACE
	-1.f, -1.f, +1.f,		-1.f, 0.f, 0.f,		0.f, 0.f,	// 12
	-1.f, -1.f, -1.f,		-1.f, 0.f, 0.f,		1.f, 0.f,	// 13
	-1.f, +1.f, -1.f,		-1.f, 0.f, 0.f,		1.f, 1.f,	// 14
	-1.f, +1.f, +1.f,		-1.f, 0.f, 0.f,		0.f, 1.f,	// 15

	// TOP FACE
	-1.0, +1.f, +1.f,		0.f, 1.f, 0.f,		0.f, 0.f,	// 16
	+1.f, +1.f, +1.f,		0.f, 1.f, 0.f,		1.f, 0.f,	// 17
	+1.f, +1.f, -1.f,		0.f, 1.f, 0.f,		1.f, 1.f,	// 18
	-1.f, +1.f, -1.f,		0.f, 1.f, 0.f,		0.f, 1.f,	// 19

	// BOTTOM FACE
	-1.0, -1.f, +1.f,		0.f, -1.f, 0.f,		0.f, 0.f,	// 20
	+1.f, -1.f, +1.f,		0.f, -1.f, 0.f,		1.f, 0.f,	// 21
	+1.f, -1.f, -1.f,		0.f, -1.f, 0.f,		1.f, 1.f,	// 22
	-1.f, -1.f, -1.f,		0.f, -1.f, 0.f,		0.f, 1.f,	// 23
};

const unsigned int CubeIndexData[] = {
	// FRONT FACE
	0, 1, 2,	0, 2, 3,
	// BACK FACE
	4, 5, 6,	4, 6, 7,
	// RIGHT FACE
	8, 9, 10,	8, 10, 11,
	// LEFT FACE
	12, 13, 14,	12, 14, 15,
	// TOP FACE
	16, 17, 18, 16, 18, 19,
	// BOTTOM FACE
	20, 21, 22, 20, 22, 23,
};

const char* SkyboxImages[] = {
	"Res/Skybox/Alps/alps_ft.tga",	// POSITIVE X (front)
	"Res/Skybox/Alps/alps_bk.tga",	// NEGATIVE X (back)
	"Res/Skybox/Alps/alps_up.tga",	// POSITIVE Y (up)
	"Res/Skybox/Alps/alps_dn.tga",	// NEGATIVE Y (down)
	"Res/Skybox/Alps/alps_rt.tga",	// POSITIVE Z (right)
	"Res/Skybox/Alps/alps_lf.tga",	// NEGATIVE Z (left)
};

const char* SpaceSkybox[] = {
	"Res/Skybox/Space/criminalmind_ft.tga",
	"Res/Skybox/Space/criminalmind_bk.tga",
	"Res/Skybox/Space/criminalmind_up.tga",
	"Res/Skybox/Space/criminalmind_dn.tga",
	"Res/Skybox/Space/criminalmind_rt.tga",
	"Res/Skybox/Space/criminalmind_lf.tga",
};

bool IsKeyPressed(int Key)
{
	return KeyState[Key];
}

void OnKeyEvent(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{
	if (Action == GLFW_REPEAT)
		return;

	KeyState[Key] = (Action == GLFW_PRESS); 

	if (Action == GLFW_PRESS)
	{
		printf("OnKeyEvent( %d )\n", Key);

		if (Key == GLFW_KEY_ESCAPE)
			ShouldQuit = true;
	}
}

void OnMouseButton(GLFWwindow* Window, int Button, int Action, int Mods)
{
	if (Button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		CameraMovementActive = (Action == GLFW_PRESS);
		glfwSetInputMode(Window, GLFW_CURSOR, CameraMovementActive ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

void OnCursorMove(GLFWwindow* Window, double X, double Y)
{
	MouseX = X;
	MouseY = Y;
}

void RenderQuad(const Transform& Transform)
{
	QuadMesh.Bind();
	DefaultMaterial.Set("u_World", Transform.GetMatrix());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void RenderTriangle(const Transform& Transform)
{
	TriangleMesh.Bind();
	DefaultMaterial.Set("u_World", Transform.GetMatrix());
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void RenderCube(const Transform& Transform)
{
	CubeMesh.Bind();
	DefaultMaterial.Set("u_World", Transform.GetMatrix());
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void RenderSkybox()
{
	SkyboxTexture.Bind();
	CubeMesh.Bind();
	SkyboxMaterial.Use();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void RenderFullscreenQuad()
{
	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void RenderLightSource(const PointLight& Light)
{
	LightSourceMaterial.Use();
	LightSourceMaterial.Set("u_World", Transform(Light.Position, quat_identity, glm::vec3(0.2f)).GetMatrix());
	LightSourceMaterial.Set("u_Color", Light.Color);
	CubeMesh.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}


void RenderScene(bool RenderPlayer)
{
	CherylTexture.Bind();
	Material::MyValue += 5;

	float Time = glfwGetTime();

	// RENDER: Floor
	RenderCube(Transform(glm::vec3(0.f, -2.f, 0.f), quat_identity, glm::vec3(50.f, 0.4f, 50.f)));

	// Cubes
	{
		Transform BaseTransform;
		BaseTransform.Position = glm::vec3(-5.f, 0.f, 10.f + glm::sin(Time) * 5.f);
		BaseTransform.Rotation = glm::angleAxis(Time, glm::vec3(0.f, 1.f, 0.f));

		Transform ChildTransform;
		ChildTransform.Position = glm::vec3(2.f + glm::sin(Time), 0.f, 0.f);
		ChildTransform.Scale = glm::vec3(0.5f);
		ChildTransform.Rotation = glm::angleAxis(Time, glm::vec3(1.f, 0.f, 0.f));

		Transform BabyTransform;
		BabyTransform.Position = glm::vec3(0.f, 2.5f, 0.f);
		BabyTransform.Scale = glm::vec3(0.5f);

		RenderCube(BaseTransform);
		RenderCube(BaseTransform * ChildTransform);
		RenderCube(BaseTransform * ChildTransform * BabyTransform);
	}

	{
		Transform BaseTransform;
		BaseTransform.Position = glm::vec3(5.f, 2.f, 1.f);
		BaseTransform.Rotation = glm::angleAxis(Time, glm::normalize(glm::vec3(0.5f, -0.4f, 1.f)));

		RenderCube(BaseTransform);

		int NumTriangles = 30;
		for (int i = 0; i < NumTriangles; ++i)
		{
			float AngleInc = (glm::pi<float>() * 2.f) / NumTriangles;
			float Angle = Time + AngleInc * i;
			float SawAngle = Time * 5.f + AngleInc * i;

			Transform ChildTransform;
			ChildTransform.Position = glm::vec3(2.5f, sin(Time * 0.6f + AngleInc * i * 2.f) * 1.f, 0.f);
			ChildTransform.Scale = glm::vec3(0.5f);

			Transform SpinTransform;
			SpinTransform.Rotation = glm::angleAxis(Angle, glm::vec3(0.f, 1.f, 0.f));

			Transform SawTransform;
			SawTransform.Rotation = glm::angleAxis(SawAngle, glm::vec3(0.f, 0.f, 1.f));

			RenderCube(BaseTransform * SpinTransform * ChildTransform * SawTransform);
		}
	}

	{
		Transform CurrentTransform;
		CurrentTransform.Position = glm::vec3(-4.f, 3.f, -2.f);
		RenderCube(CurrentTransform);

		int Iterations = 40;
		for (int i = 0; i < Iterations; ++i)
		{
			Transform Delta;
			Delta.Position = glm::vec3(-1.5f, 0.f, 0.f);
			Delta.Scale = glm::vec3(0.9f);
			Delta.Rotation = glm::angleAxis(sin(-Time + i * 0.04f) * 0.2f, glm::vec3(0.f, 0.f, 1.f));

			Transform TwirlDelta;
			TwirlDelta.Rotation = glm::angleAxis(sin(-Time + i * 0.25f) * 0.15f, glm::vec3(0.f, 1.f, 0.f));

			Transform TwistDelta;
			TwistDelta.Rotation = glm::angleAxis(sin(-Time * 0.4f + i * 0.21f) * 0.12f, glm::vec3(1.f, 0.f, 0.f));

			CurrentTransform = CurrentTransform * TwirlDelta * Delta * TwistDelta;
			RenderCube(CurrentTransform);
		}

		MainLight.Position = CurrentTransform.Position;
		MainLight.UploadToMaterial(0, DefaultMaterial);
	}

	{
		glm::vec3 RotationAxis = glm::vec3(cos(Time * 0.62f), sin(Time * 0.9f) * 2.f, cos(-Time * 0.5f) * 0.5f);
		RotationAxis = glm::normalize(RotationAxis);

		Transform BaseTransform;
		BaseTransform.Position = glm::vec3(3.f, 2.f, 20.f);
		BaseTransform.Rotation = glm::angleAxis(Time, RotationAxis);

		RenderCube(BaseTransform);

		static Transform ChildTransform = Transform(glm::vec3(2.5f, 0.f, 0.f), quat_identity, glm::vec3(0.5f));

		/*
		ChildTransform.Position = glm::vec3(2.5f, 0.f, 0.f);
		ChildTransform.Rotation = glm::angleAxis(Time, glm::vec3(0.f, 1.f, 0.f));
		ChildTransform.Scale = glm::vec3(0.5f);
		*/

		static bool AttachedToCamera = false;

		if (IsKeyPressed(GLFW_KEY_SPACE))
		{
			if (!AttachedToCamera)
			{
				// Transfer the cube from big-box space to camera space
				//Transform WorldSpace = BaseTransform * ChildTransform;
				//Transform CameraSpace = TheCamera.GetTransform().Inverse() * WorldSpace;
				// KeepWorldSpace
				ChildTransform = TheCamera.GetTransform().Inverse() * BaseTransform * ChildTransform;

				AttachedToCamera = true;
			}
			/*
			Transform CameraLocal;
			CameraLocal.Position = glm::vec3(0.f, 0.f, -1.5f);
			CameraLocal.Scale = glm::vec3(0.5f);
			CameraLocal.Rotation = glm::angleAxis(Time, glm::vec3(0.f, 0.f, -1.f));

			Transform AttachWorld = TheCamera.GetTransform() * CameraLocal;
			ChildTransform = BaseTransform.Inverse() * AttachWorld;
			*/

			RenderCube(TheCamera.GetTransform() * ChildTransform);
		}
		else
		{
			if (AttachedToCamera)
			{
				// Transfer from camera space to big-box space
				Transform WorldSpace = TheCamera.GetTransform() * ChildTransform;
				Transform BoxSpace = BaseTransform.Inverse() * WorldSpace;

				ChildTransform = BoxSpace;

				AttachedToCamera = false;
			}

			RenderCube(BaseTransform * ChildTransform);
		}

	}

	RenderCube(Transform(glm::vec3(0.f, 0.f, -10.f), quat_identity, glm::vec3(20.f, 20.f, 0.05f)));
	RenderCube(Transform(glm::vec3(-20.f, -1.f, -10.f)));

	RenderSkybox();
	RenderLightSource(MainLight);
	RenderLightSource(SecondLight);

	// Render the player
	if (RenderPlayer)
		RenderCube(Transform(TheCamera.Position));
}

int main()
{
	/* WINDOW STUFF */
	// Initialize GLFW, create a window and context
	glfwInit();

	GLFWwindow* Window;
	Window = glfwCreateWindow(ScreenWidth, ScreenHeight, "OpenGL 3D", nullptr, nullptr);


	glfwSetKeyCallback(Window, OnKeyEvent);
	glfwSetCursorPosCallback(Window, OnCursorMove);
	glfwSetMouseButtonCallback(Window, OnMouseButton);
	glfwMakeContextCurrent(Window);
	glfwSwapInterval(0);

	// After the context is bound, we can import the OpenGL extensions, through the extension wrangler
	glewInit();

	/* VERTEX STUFF */

	// Triangle Data
	float TriData[] = {
		-0.5f, -0.5f, 0.0f,		0.f, 0.f, 1.f,		0.0f, 0.0f,
		+0.5f, -0.5f, 0.0f,		0.f, 0.f, 1.f,		1.0f, 0.0f,
		+0.0f, +0.5f, 0.0f,		0.f, 0.f, 1.f,		0.5f, 1.0f,
	};
	 
	TriangleMesh.LoadVerts(TriData, sizeof(TriData), nullptr, 0);


	//Quad Data
	float QuadData[] = {
		-0.5f, -0.5f, 0.f,		0.f, 0.f, 1.f,		0.f, 0.f,
		+0.5f, -0.5f, 0.f,		0.f, 0.f, 1.f,		1.f, 0.f,
		-0.5f, +0.5f, 0.f,		0.f, 0.f, 1.f,		0.f, 1.f,
		+0.5f, +0.5f, 0.f,		0.f, 0.f, 1.f,		1.f, 1.f,
	};
	
	QuadMesh.LoadVerts(QuadData, sizeof(QuadData), nullptr, 0);

	CubeMesh.LoadVerts(CubeVertexData, sizeof(CubeVertexData), CubeIndexData, sizeof(CubeIndexData));

	DefaultMaterial.LoadFiles("Res/Shader/default.vert", "Res/Shader/default.frag"); //insert cheese
	DefaultMaterial.Set("u_DirLight.ShadowBuffer", 2);

	Material PostProcessMaterial;
	PostProcessMaterial.LoadFiles("Res/Shader/post_process.vert", "Res/Shader/post_process.frag");
	PostProcessMaterial.Set("u_FrameColor", 0);
	PostProcessMaterial.Set("u_FrameDepth", 1);

	SkyboxMaterial.LoadFiles("Res/Shader/skybox.vert", "Res/Shader/skybox.frag");
	LightSourceMaterial.LoadFiles("Res/Shader/light_source.vert", "Res/Shader/light_source.frag");

	CherylTexture.LoadFile("Res/red_brick.jpg");
	CherylTexture.Bind();

	//SkyboxTexture.LoadFiles(SkyboxImages);

	//space
	SkyboxTexture.LoadFiles(SpaceSkybox);

	float Ratio = (float)ScreenWidth / ScreenHeight;
	float OrthoSize = 2.f;

	TheCamera.Position = glm::vec3(0.f, 4.f, 8.f);

	glm::vec3 LightDirection = glm::normalize(glm::vec3(-1.f, -0.55f, 0.35f));
	DefaultMaterial.Set("u_DirLight.Direction", LightDirection);
	DefaultMaterial.Set("u_DirLight.Color", glm::vec3(0.9f, 0.9f, 1.f));

	// Point light stuff!
	MainLight.Position = glm::vec3(0.f, 1.f, 0.f);
	MainLight.Color =  glm::vec3(1.f, 0.f, 0.f);
	MainLight.Radius = 20.f;
	MainLight.UploadToMaterial(0, DefaultMaterial);

	SecondLight.Position = glm::vec3(0.f, 1.f, 0.f);
	SecondLight.Color =  glm::vec3(0.4f, 0.4f, 1.f);
	SecondLight.Radius = 40.f;
	SecondLight.UploadToMaterial(1, DefaultMaterial);

	FlashLight.Position = glm::vec3(0.f, 2.f, 10.f);
	FlashLight.Direction = glm::vec3(0.f, 0.f, -1.f);
	FlashLight.Color = glm::vec3(0.3f, 1.f, 0.3f);
	FlashLight.Angle = glm::radians(20.f);
	FlashLight.Length = 30.f;
	FlashLight.UploadToMaterial(DefaultMaterial);

	glm::mat4 Projection;
	Projection = glm::perspective(glm::radians(60.f), Ratio, 0.2f, 150.f);
	DefaultMaterial.Set("u_Projection", Projection);

	glEnable(GL_DEPTH_TEST);
	float LastFrameTime = 0.f;

	double LastMouseX = 0.0;
	double LastMouseY = 0.0;

	FrameBuffer LightBuffer;
	LightBuffer.Create(4096, 4096);

	FrameBuffer FB;
	FB.Create(2000, 2000);

	glm::mat4 LightProjection;
	glm::mat4 LightView;
	LightProjection = glm::ortho(-25.f, 25.f, -25.f, 25.f, -30.f, 30.f);
	LightView = glm::lookAt(glm::vec3(0.f), LightDirection, glm::vec3(0.f, 1.f, 0.f));

	// Main loop of the program
	while (!glfwWindowShouldClose(Window) && !ShouldQuit)
	{
		DefaultMaterial.Use();
		float SpeedBoost = 50.f;
		float CurrentTime = glfwGetTime();
		float DeltaTime = CurrentTime - LastFrameTime;
		LastFrameTime = CurrentTime;

		double MouseDeltaX = MouseX - LastMouseX;
		double MouseDeltaY = MouseY - LastMouseY;
		LastMouseX = MouseX;
		LastMouseY = MouseY;

		if (CameraMovementActive)
		{
			TheCamera.AddYaw(-MouseDeltaX * 0.05f);
			TheCamera.AddPitch(-MouseDeltaY * 0.05f);

			glm::vec3 CameraMovement = glm::vec3(0.f);
			if (IsKeyPressed(GLFW_KEY_W))
				CameraMovement += TheCamera.GetForwardVector();
			if (IsKeyPressed(GLFW_KEY_S))
				CameraMovement += -TheCamera.GetForwardVector();
			if (IsKeyPressed(GLFW_KEY_A))
				CameraMovement += -TheCamera.GetRightVector();
			if (IsKeyPressed(GLFW_KEY_D))
				CameraMovement += TheCamera.GetRightVector();

			if (IsKeyPressed(GLFW_KEY_E))
				CameraMovement += glm::vec3(0.f, 1.f, 0.f);
			if (IsKeyPressed(GLFW_KEY_Q))
				CameraMovement -= glm::vec3(0.f, 1.f, 0.f);

			TheCamera.Position += CameraMovement * 7.5f * DeltaTime;

			//Speed boost
			if (IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
				TheCamera.Position += CameraMovement * SpeedBoost * DeltaTime;
		}

		DefaultMaterial.Set("u_View", TheCamera.GetViewMatrix());
		DefaultMaterial.Set("u_EyePosition", TheCamera.Position);

		MainLight.Color = glm::vec3(1.f, 0.3f, 0.3f) * (glm::sin(CurrentTime * 5.f) + 1.f) * 4.f;
		MainLight.Position = glm::vec3(glm::sin(CurrentTime) * 5.f, 0.2f, glm::cos(CurrentTime) * 2.f);
		MainLight.UploadToMaterial(0, DefaultMaterial);

		if (IsKeyPressed(GLFW_KEY_F))
		{
			FlashLight.Position = TheCamera.Position;
			FlashLight.Direction = TheCamera.GetForwardVector();
			FlashLight.UploadToMaterial(DefaultMaterial);	
		}

		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		// Render the light pass!
		DefaultMaterial.Set("u_Projection", LightProjection);
		DefaultMaterial.Set("u_View", LightView);
		LightSourceMaterial.Set("u_Projection", LightProjection);
		LightSourceMaterial.Set("u_View", LightView);

		LightBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderScene(true);

		LightBuffer.Unbind();

		// Render to Frame Buffer
		DefaultMaterial.Set("u_Projection", Projection);
		DefaultMaterial.Set("u_View", TheCamera.GetViewMatrix());
		DefaultMaterial.Set("u_DirLight.ViewProjection", LightProjection * LightView);
		LightSourceMaterial.Set("u_Projection", Projection);
		LightSourceMaterial.Set("u_View", TheCamera.GetViewMatrix());

		SkyboxMaterial.Set("u_Projection", Projection);
		SkyboxMaterial.Set("u_View", TheCamera.GetViewMatrix());

		LightBuffer.DepthTexture.Bind(2);

		FB.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderScene(false);

		FB.Unbind();
		

		// Render to screen!
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//FB.ColorTexture.Bind();
		//RenderQuad(Transform(glm::vec3(0.f, 20.f, -20.f), quat_identity, glm::vec3(40.f)));

		PostProcessMaterial.Use();
		FB.ColorTexture.Bind(0);
		FB.DepthTexture.Bind(1);
		RenderFullscreenQuad();


		// Swap the back-buffer to the front and poll and handle window events
		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	return 0;
}
