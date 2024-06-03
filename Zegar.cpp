#include <iostream>
#include <string>
#include <ctime>
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
	void update(double time, float localTime, float divider) {
		this->transform = MatrixRotate(this->rotation, this->rotationAngle) * MatrixRotateX(PI * localTime / 30 / divider) * MatrixRotateX(PI * time / 30 / divider) * MatrixTranslate(this->position.x, this->position.y, this->position.z);
	}
	void changeColor() {
		static int number = 0;
		switch (number) {
		case 0: this->color = LIGHTGRAY; break;
		case 1: this->color = GRAY; break;
		case 2: this->color = DARKGRAY; break;
		case 3: this->color = YELLOW; break;
		case 4: this->color = GOLD; break;
		case 5: this->color = ORANGE; break;
		case 6: this->color = PINK; break;
		case 7: this->color = RED; break;
		case 8: this->color = MAROON; break;
		case 9: this->color = GREEN; break;
		case 10: this->color = LIME; break;
		case 11: this->color = DARKGREEN; break;
		case 12: this->color = SKYBLUE; break;
		case 13: this->color = BLUE; break;
		case 14: this->color = DARKBLUE; break;
		case 15: this->color = PURPLE; break;
		case 16: this->color = VIOLET; break;
		case 17: this->color = DARKPURPLE; break;
		case 18: this->color = BEIGE; break;
		case 19: this->color = BROWN; break;
		case 20: this->color = DARKBROWN; break;
		case 21: this->color = WHITE; number = -1; break;
		}
		++number;
	}
};

int main(void)
{
	const int screenWidth = 1600;
	const int screenHeight = 900;

	time_t now = time(0);
	tm* localtm = localtime(&now);
	float localTime = localtm->tm_hour * 3600 + localtm->tm_min * 60 + localtm->tm_sec;

	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Zegar");
	InitAudioDevice();

	Sound tick = LoadSound("assets/clock.wav");
	float tickTime = 0;
	float time = 0;
	double multiplier = 1;

	Camera camera = { 0 };
	camera.position = { -10.0f, 10.0f, 10.0f };
	camera.target = { 0.0f, 3.7f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	Shader shader = LoadShader(TextFormat("assets/lighting.vs", GLSL_VERSION),
		TextFormat("assets/lighting.fs", GLSL_VERSION));

	Object zegar = Object(LoadModel("assets/zegar.obj"), { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -90, shader);
	Object wahadlo = Object(LoadModel("assets/wahadlo.obj"), { 0.0f, 2.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI/2, shader);
	Object wskazSek = Object(LoadModel("assets/wskazSek.obj"), { -0.22f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader);
	Object wskazMin = Object(LoadModel("assets/wskazMin.obj"), { -0.2f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI/2, shader);
	Object wskazGodzin = Object(LoadModel("assets/wskazGodzin.obj"), { 0.0f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI/2, shader);
	Object podloga = Object(LoadModel("assets/podloga.obj"), { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, 90, shader);
	Object zebatka1 = Object(LoadModel("assets/zebatka_12.obj"), { 0.2f, 3.97f, -0.81f }, { 0.0f, 1.0f, 0.0f }, { 0.02f, 0.02f, 0.02f }, SKYBLUE, -90, shader);
	Object zebatka2 = Object(LoadModel("assets/zebatki_48_8.obj"), { 0.15f, 3.97f, -0.43f }, { 0.0f, 1.0f, 0.0f }, { 0.02f, 0.02f, 0.02f }, WHITE, -90, shader);
	Object zebatka3 = Object(LoadModel("assets/zebatki_60_8.obj"), { 0.1f, 3.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.02f, 0.02f, 0.02f }, SKYBLUE, -90, shader);
	Object zebatka4 = Object(LoadModel("assets/zebatki_64_8.obj"), { 0.06f, 3.97f, 0.446f }, { 0.0f, 1.0f, 0.0f }, { 0.02f, 0.02f, 0.02f }, WHITE, -90, shader);
	Object zebatka5 = Object(LoadModel("assets/zebatki_60_15.obj"), { 0.02f, 3.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.02f, 0.02f, 0.02f }, SKYBLUE, -90, shader);
	Object zebatka6 = Object(LoadModel("assets/zebatki_45_10.obj"), { -0.02f, 3.97f, 0.38f }, { 0.0f, 1.0f, 0.0f }, { 0.02f, 0.02f, 0.02f }, WHITE, -90, shader);
	Object zebatka7 = Object(LoadModel("assets/zebatka_40_1.obj"), { -0.08f, 3.97f, 0.0f }, { 0.0f, 0.1f, 0.0f }, { 0.025f, 0.025f, 0.025f }, SKYBLUE, -90, shader);

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
		if (time - tickTime >= 1/multiplier) {
			tickTime = time;
			PlaySound(tick);
		}

		time += GetFrameTime()*multiplier;
		std::string temp = "Czas od startu programu: " + std::to_string(time);

		UpdateCamera(&camera, CAMERA_THIRD_PERSON);
		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
		wskazSek.update(int(time), localTime, 1);
		wskazMin.update(time, localTime, 60);
		wskazGodzin.update(time, localTime, 720);

		double lastMultiplier = multiplier;
		if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
		if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
		if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
		if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }
		if (IsKeyPressed(KEY_I)) { multiplier += 1.0f; }
		if (IsKeyPressed(KEY_K)) { multiplier -= 1.0f; }
		if (IsKeyPressed(KEY_O)) { zegar.changeColor(); }
		if (IsKeyPressed(KEY_P)) { podloga.changeColor(); }
		if (IsKeyPressed(KEY_C)) {
			camera.position = { -10.0f, 10.0f, 10.0f };
			camera.target = { 0.0f, 3.7f, 0.0f };
			camera.up = { 0.0f, 1.0f, 0.0f };
			camera.fovy = 45.0f;
			camera.projection = CAMERA_PERSPECTIVE;
		}
		if (lastMultiplier != multiplier)
		std::cout << multiplier << std::endl;
		for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);

		BeginDrawing();
		ClearBackground(SKYBLUE);

		BeginMode3D(camera);

		zegar.drawM();
		wahadlo.draw();
		wskazSek.draw();
		wskazMin.draw();
		wskazGodzin.draw();
		podloga.drawM();
		zebatka1.drawM();
		zebatka2.drawM();
		zebatka3.drawM();
		zebatka4.drawM();
		zebatka5.drawM();
		zebatka6.drawM();
		zebatka7.drawM();


		// Draw spheres to show where the lights are
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
			else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
		}

		EndMode3D();

		DrawFPS(10, 10);
		DrawText(temp.c_str(), 10, screenHeight - 20, 20, LIGHTGRAY);
		EndDrawing();
	}


	// Deinicjalizacja
	UnloadSound(tick);
	CloseAudioDevice();
	CloseWindow();

	return 0;
}