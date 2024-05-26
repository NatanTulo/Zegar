#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "raylib.h"
#include "raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "raymath.h"
#define GLSL_VERSION            330

Matrix operator*(const Matrix& a, const Matrix& b) {
	Matrix result = {};
	result.m0 = a.m0 * b.m0 + a.m1 * b.m4 + a.m2 * b.m8 + a.m3 * b.m12;
	result.m1 = a.m0 * b.m1 + a.m1 * b.m5 + a.m2 * b.m9 + a.m3 * b.m13;
	result.m2 = a.m0 * b.m2 + a.m1 * b.m6 + a.m2 * b.m10 + a.m3 * b.m14;
	result.m3 = a.m0 * b.m3 + a.m1 * b.m7 + a.m2 * b.m11 + a.m3 * b.m15;
	result.m4 = a.m4 * b.m0 + a.m5 * b.m4 + a.m6 * b.m8 + a.m7 * b.m12;
	result.m5 = a.m4 * b.m1 + a.m5 * b.m5 + a.m6 * b.m9 + a.m7 * b.m13;
	result.m6 = a.m4 * b.m2 + a.m5 * b.m6 + a.m6 * b.m10 + a.m7 * b.m14;
	result.m7 = a.m4 * b.m3 + a.m5 * b.m7 + a.m6 * b.m11 + a.m7 * b.m15;
	result.m8 = a.m8 * b.m0 + a.m9 * b.m4 + a.m10 * b.m8 + a.m11 * b.m12;
	result.m9 = a.m8 * b.m1 + a.m9 * b.m5 + a.m10 * b.m9 + a.m11 * b.m13;
	result.m10 = a.m8 * b.m2 + a.m9 * b.m6 + a.m10 * b.m10 + a.m11 * b.m14;
	result.m11 = a.m8 * b.m3 + a.m9 * b.m7 + a.m10 * b.m11 + a.m11 * b.m15;
	result.m12 = a.m12 * b.m0 + a.m13 * b.m4 + a.m14 * b.m8 + a.m15 * b.m12;
	result.m13 = a.m12 * b.m1 + a.m13 * b.m5 + a.m14 * b.m9 + a.m15 * b.m13;
	result.m14 = a.m12 * b.m2 + a.m13 * b.m6 + a.m14 * b.m10 + a.m15 * b.m14;
	result.m15 = a.m12 * b.m3 + a.m13 * b.m7 + a.m14 * b.m11 + a.m15 * b.m15;
	return result;
}

class Object {
	Model model;
	//Texture2D texture;
	Matrix transform;
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
	Color color;
	float rotationAngle;
public:
	Object(Model model, Vector3 position, Vector3 rotation, Vector3 scale, Color color, float rotationAngle, Shader shader) {
		this->model = model;
		//this->texture = texture;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		this->color = color;
		this->rotationAngle = rotationAngle;
		model.materials[0].shader = shader;
		//model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
		this->transform = MatrixRotate(rotation, rotationAngle) * MatrixTranslate(position.x, position.y, position.z);
	}
	void draw() {
		DrawMesh(model.meshes[0], model.materials[0], transform);
	}
	void drawM() {
		DrawModelEx(model, position, rotation, rotationAngle, scale, color);
	}
	void update(double time) {
		this->transform = MatrixRotate(this->rotation, this->rotationAngle) * MatrixRotateX(PI*time/30) * MatrixTranslate(this->position.x, this->position.y, this->position.z);
	}
};

int main(void)
{
	const int screenWidth = 1600;
	const int screenHeight = 900;

	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Zegar");
	InitAudioDevice();

	Sound tick = LoadSound("assets/clock.wav");
	double tickTime = 0;
	double time = 0;
	double multiplier = 1;

	Camera camera = { 0 };
	camera.position = { -10.0f, 10.0f, 10.0f };
	camera.target = { 0.0f, 3.7f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	Shader shader = LoadShader(TextFormat("assets/lighting.vs", GLSL_VERSION),
		TextFormat("assets/lighting.fs", GLSL_VERSION));

	Object zegar = Object(LoadModel("assets/zegar.obj"), { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -90, shader);
	Object wahadlo = Object(LoadModel("assets/wahadlo.obj"), { 0.0f, 2.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI/2, shader);
	Object wskazMin = Object(LoadModel("assets/wskazMin.obj"), { -0.2f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI/2, shader);
	Object wskazGodzin = Object(LoadModel("assets/wskazGodzin.obj"), { 0.0f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI/2, shader);
	Object podloga = Object(LoadModel("assets/podloga.obj"), { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, 90, shader);

	int ambientLoc = GetShaderLocation(shader, "ambient");
	float val_t[]{ 0.1f, 0.1f, 0.1f, 1.0f };
	SetShaderValue(shader, ambientLoc, val_t, SHADER_UNIFORM_VEC4);

	Light lights[MAX_LIGHTS] = { 0 };
	lights[0] = CreateLight(LIGHT_POINT, { -4, 1, -4 }, Vector3Zero(), WHITE, shader);
	lights[1] = CreateLight(LIGHT_POINT, { 4, 1, 4 }, Vector3Zero(), RED, shader);
	lights[2] = CreateLight(LIGHT_POINT, { -4, 1, 4 }, Vector3Zero(), GREEN, shader);
	lights[3] = CreateLight(LIGHT_POINT, { 4, 1, -4 }, Vector3Zero(), BLUE, shader);
	DisableCursor();
	SetTargetFPS(360);

	while (!WindowShouldClose())
	{
		if (GetTime() - tickTime >= 1/multiplier) {
			tickTime = GetTime();
			PlaySound(tick);
		}

		time = GetTime();
		std::string temp = "Aktualny czas: " + std::to_string(time);
		char* timeStr = (char*)malloc(temp.length() + 16);
		if (timeStr != nullptr) strcpy(timeStr, temp.c_str());

		UpdateCamera(&camera, CAMERA_THIRD_PERSON);
		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
		wskazMin.update(time);
		wskazGodzin.update(time/60);

		if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
		if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
		if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
		if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }
		if (IsKeyPressed(KEY_I)) { multiplier += 1.0f; }
		if (IsKeyPressed(KEY_K)) { multiplier += 1.0f; }
		std::cout << multiplier << std::endl;
		for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		zegar.drawM();
		wahadlo.draw();
		wskazMin.draw();
		wskazGodzin.draw();
		podloga.drawM();

		// Draw spheres to show where the lights are
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
			else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
		}

		EndMode3D();

		DrawFPS(10, 10);
		DrawText(timeStr, screenWidth - 210, screenHeight - 20, 20, LIGHTGRAY);
		free(timeStr);
		EndDrawing();
	}

	// Deinicjalizacja
	UnloadSound(tick);
	CloseAudioDevice();
	CloseWindow();

	return 0;
}