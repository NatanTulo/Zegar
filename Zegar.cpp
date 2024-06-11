﻿#include <iostream>
#include <string>
#include <ctime>
#include "raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "raymath.h"
#define GLSL_VERSION            330

Matrix operator*(const Matrix& a, const Matrix& b) {
	Matrix result = MatrixMultiply(a, b);
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
	bool isVisible;
	Vector3 originalPosition;
	float xOffset;
public:
	Object(Model model, Vector3 position, Vector3 rotation, Vector3 scale, Color color, float rotationAngle, Shader shader) {
		this->model = model;
		//this->texture = texture;
		this->position = position;
		this->originalPosition = position;
		this->rotation = rotation;
		this->scale = scale;
		this->color = color;
		this->rotationAngle = rotationAngle;
		this->model.materials[0].shader = shader;
		//model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
		this->transform = MatrixRotate(rotation, rotationAngle) * MatrixTranslate(position.x, position.y, position.z);
		this->isVisible = true;
		this->xOffset = 0.0f;
	}
	~Object() {
		UnloadModel(model);
	}
	void draw() {
		DrawMesh(model.meshes[0], model.materials[0], transform);
	}
	void drawM() {
		DrawModelEx(model, position, rotation, rotationAngle, scale, color);
	}
	void update(double time, float divider) {
		this->transform = MatrixRotate(this->rotation, this->rotationAngle) * MatrixRotateX(PI * time / 30 / divider) * MatrixTranslate(this->position.x, this->position.y, this->position.z);
	}
	void wahadloUpdate(double time) {
		const float theta0 = 0.25f;
		const float g = 9.81f;
		const float L = 1.0f;

		float angle = theta0 * sin(sqrt(g / L) * time);
		this->transform = MatrixRotate(this->rotation, this->rotationAngle) * MatrixRotateX(angle) * MatrixTranslate(this->position.x, this->position.y, this->position.z);
	}
	void changeColor() {
		static int number = 0;
		static const Color colors[] = {
			LIGHTGRAY, GRAY, DARKGRAY, YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN,
			LIME, DARKGREEN, SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE,
			BEIGE, BROWN, DARKBROWN, WHITE
		};
		color = colors[number];
		number = (number + 1) % (sizeof(colors) / sizeof(colors[0]));
	}
	float spread(int index, float offset) {
		float speed = 2.0f;
		xOffset += GetFrameTime() * speed;
		if (xOffset >= offset) {
			xOffset = offset;
		}
		position.x = originalPosition.x + xOffset * (index + 1);
		
		transform = MatrixRotate(rotation, rotationAngle) * MatrixTranslate(position.x, position.y, position.z);

		return position.x;
	}

	float resetPosition(int index) {
		float speed = 2.0f;
		xOffset -= GetFrameTime() * speed;
		if (xOffset <= 0.0f) {
			xOffset = 0.0f;
		}
		position.x = originalPosition.x + xOffset * (index + 1); 
		transform = MatrixRotate(rotation, rotationAngle) * MatrixTranslate(position.x, position.y, position.z);

		return position.x;
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
	Sound quack = LoadSound("assets/quack.wav");
	float tickTime = 0;
	float time = 0;
	double multiplier = 1;
	int widoczneZebatki = 11;

	int hours;
	int minutes;
	int seconds;

	Camera camera = { 0 };
	camera.position = { -10.0f, 10.0f, 10.0f };
	camera.target = { 0.0f, 3.7f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	CameraMode cameraMode = CAMERA_CUSTOM;


	float rotationAngleX = 0.0f;
	float rotationAngleY = 0.0f;
	float distanceFromTarget = 10.0f;
	Vector3 targetPoint = { 0.0f, 3.7f, 0.0f };

	bool hourChange = false;
	bool minuteChange = false;
	bool secondChange = false;
	bool localTimeActive = true;
	bool ustawiona = false;


	Shader shader = LoadShader(TextFormat("assets/lighting.vs", GLSL_VERSION),
		TextFormat("assets/lighting.fs", GLSL_VERSION));

	Object zegar = Object(LoadModel("assets/zegar.obj"), { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -90, shader);
	Object wahadlo = Object(LoadModel("assets/wahadlo.obj"), { 0.0f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader);
	Object wskazSek = Object(LoadModel("assets/wskazSek.obj"), { -0.22f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader);
	Object wskazMin = Object(LoadModel("assets/wskazMin.obj"), { -0.2f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader);
	Object wskazGodzin = Object(LoadModel("assets/wskazGodzin.obj"), { 0.0f, 3.95f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader);
	Object podloga = Object(LoadModel("assets/podloga.obj"), { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, 90, shader);
	Object zebatka[12] = {
		Object(LoadModel("assets/zebatka_12_3.obj"), { 0.2f, 3.97f, -0.81f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_48.obj"), { 0.15f, 3.97f, -0.43f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_8.obj"), { 0.15f, 3.97f, -0.43f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_60.obj"), { 0.1f, 3.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_8.obj"), { 0.1f, 3.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_64.obj"), { 0.06f, 3.97f, 0.446f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_8.obj"), { 0.06f, 3.97f, 0.446f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_60.obj"), { 0.02f, 3.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_15.obj"), { 0.02f, 3.97f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_45.obj"), { -0.02f, 3.97f, 0.38f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_10.obj"), { -0.02f, 3.97f, 0.38f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, WHITE, -PI / 2, shader),
		Object(LoadModel("assets/zebatka_40_2.obj"), { -0.08f, 3.97f, 0.0f }, { 0.0f, 0.1f, 0.0f }, { 1.0f, 1.0f, 1.0f }, SKYBLUE, -PI / 2, shader),
	};
	Object kaczka = Object(LoadModel("assets/Duck.glb"), { 0.05f, 4.45f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.002f, 0.002f, 0.002f }, WHITE, 180, shader);

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
		Vector2 mouseDelta = GetMouseDelta();
		rotationAngleX -= mouseDelta.x * 0.003f;
		if (camera.position.y > 0.2f) rotationAngleY += mouseDelta.y * 0.003f; else targetPoint.y += 0.05;
		if (rotationAngleY > PI / 2) rotationAngleY = PI / 2;
		if (rotationAngleY < -PI / 2) rotationAngleY = -PI / 2;
		camera.position.x = targetPoint.x + distanceFromTarget * cos(rotationAngleY) * sin(rotationAngleX);
		camera.position.z = targetPoint.z + distanceFromTarget * cos(rotationAngleY) * cos(rotationAngleX);
		camera.position.y = targetPoint.y + distanceFromTarget * sin(rotationAngleY);
		camera.target = targetPoint;

		if (time - tickTime >= 1 / multiplier) {
			tickTime = time;
			PlaySound(tick);
		}


		time += GetFrameTime() * multiplier;

		if (localTimeActive)
		{
			hours = int(fmod(time + localTime, 86400)) / 3600;
			minutes = int(fmod(time + localTime, 86400) / 60 - 60 * hours);
			seconds = int(fmod(time + localTime, 86400) - (hours * 3600) - (minutes * 60));
		}
		


		std::string hoursS = hours < 10 ? '0'+std::to_string(hours) : std::to_string(hours);
		std::string minutesS = minutes < 10 ? '0' + std::to_string(minutes) : std::to_string(minutes);
		std::string secondsS = seconds < 10 ? '0' + std::to_string(seconds) : std::to_string(seconds);
		std::string napis1 = "Czas: " + hoursS + ":" + minutesS + ":" + secondsS;

		float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
		wskazSek.update(int(time) + localTime, 1);
		wskazMin.update(time + localTime, 60);
		wskazGodzin.update(time + localTime, 720);

		zebatka[0].update(time + localTime, 1 / 6);
		zebatka[1].update(time + localTime, -8);
		zebatka[2].update(time + localTime, -1);
		zebatka[3].update(time + localTime, 60);
		zebatka[4].update(time + localTime, 7.5f);
		zebatka[5].update(time + localTime, -480);
		zebatka[6].update(time + localTime, -60);
		zebatka[7].update(time + localTime, 3600);
		zebatka[8].update(time + localTime, 240);
		zebatka[9].update(time + localTime, 10800);
		zebatka[10].update(time + localTime, -1080);
		zebatka[11].update(time + localTime, 43200);
		wahadlo.wahadloUpdate(time + localTime);
		if(minutes==59&&seconds>55) multiplier = 1.0f;
		if (minutes == 59 && seconds == 59) { 
			kaczka.spread(0, -0.5f); 
			PlaySound(quack);
		}
		if(minutes==0&&seconds==2) kaczka.resetPosition(0);

		if (GetMouseWheelMove() > 0) distanceFromTarget -= 0.5f;
		if (GetMouseWheelMove() < 0) distanceFromTarget += 0.5f;
		if (IsKeyDown(KEY_W)) targetPoint.y += 0.01f;
		if (IsKeyDown(KEY_S)) if (camera.position.y > 0.2f)targetPoint.y -= 0.01f;
		if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
		if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
		if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
		if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }
		if (IsKeyDown(KEY_I)) {
			multiplier += 50.0f;
			std::cout << multiplier << std::endl;
		}
		if (IsKeyDown(KEY_K)) {
			multiplier -= 50.0f;
			std::cout << multiplier << std::endl;
		}
		if(IsKeyPressed(KEY_L)) {
			multiplier = 1.0f;
			std::cout << multiplier << std::endl;
		}
		if (IsKeyPressed(KEY_O)) { zegar.changeColor(); }
		if (IsKeyPressed(KEY_P)) { podloga.changeColor(); }
		if (IsKeyPressed(KEY_C)) {
			rotationAngleX = -PI/4;
			rotationAngleY = PI/8;
			distanceFromTarget = 10.0f;
			targetPoint = { 0.0f, 3.7f, 0.0f };
		}
		if (IsKeyPressed(KEY_T)) {
			now = std::time(0);
			localtm = localtime(&now);
			localTime = localtm->tm_hour * 3600 + localtm->tm_min * 60 + localtm->tm_sec;
			time = 0;
			tickTime = 0;
			multiplier = 1;
			std::cout << multiplier << std::endl;
		}
		if (IsKeyPressed(KEY_EQUAL)) { if (widoczneZebatki < 11) ++widoczneZebatki; }
		if (IsKeyPressed(KEY_MINUS)) { if (widoczneZebatki > 0) --widoczneZebatki; }
		if(IsKeyDown(KEY_M)) {
			for (int i = 0; i < 12; ++i) targetPoint.x = zebatka[widoczneZebatki - i].spread(i, 0.5f)/2;
			
		}
		if (IsKeyDown(KEY_N)) {
			for (int i = 0; i < 12; ++i) targetPoint.x = zebatka[widoczneZebatki - i].resetPosition(i)/2;
		}

		for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);


		if (IsKeyPressed(KEY_Z)) {
			if (localTimeActive)
			{
				hourChange = true;
				localTimeActive = false;
			}
			else if (hourChange || minuteChange || secondChange)
			{
				hourChange = false;
				minuteChange = false;
				secondChange = false;
				localTimeActive = true;
			}
			
		}

		if (IsKeyPressed(KEY_UP)) {
			if (hourChange)
			{
				if (hours < 12)
				{
					hours = (hours + 1);
				}
				else (hours = 0);
			}
			else if (minuteChange)
			{
				if (minutes < 60)
				{
					minutes = (minutes + 1);
				}
				else (minutes = 0);
			}
			else if (secondChange)
			{
				if (seconds < 60)
				{
					seconds = (seconds + 1);
				}
				else (seconds = 0);
			}

		}

		if (IsKeyPressed(KEY_DOWN)) {
			if (hourChange)
			{
				if (hours > 0 )
				{
					hours = (hours - 1);
				}
				else (hours = 12);
			}
			else if (minuteChange)
			{
				if (minutes > 0)
				{
					minutes = (minutes - 1);
				}
				else (minutes = 60);
			}
			else if (secondChange)
			{
				if (seconds > 0)
				{
					seconds = (seconds - 1);
				}
				else (seconds = 60);
			}
		}


		if (IsKeyPressed(KEY_LEFT))
		{
			if (hourChange)
			{
				secondChange = true;
				hourChange = false;
			}
			else if (secondChange)
			{
				minuteChange = true;
				secondChange = false;
			}
			else if (minuteChange)
			{
				hourChange = true;
				minuteChange = false;
			}
			
		}
		
		if (IsKeyPressed(KEY_RIGHT))
		{
			if (hourChange)
			{
				minuteChange = true;
				hourChange = false;
			}
			else if (minuteChange)
			{
				secondChange = true;
				minuteChange = false;
			}
			else if (secondChange)
			{
				hourChange = true;
				minuteChange = false;
			}
		}

		if (IsKeyPressed(KEY_ENTER))
		{
			if(hourChange || minuteChange || secondChange)
			{
				ustawiona = true;
				localTimeActive = false;
				hourChange = false;
				minuteChange = false;
				secondChange = false;
			}
		}

		BeginDrawing();
		ClearBackground(SKYBLUE);
		BeginMode3D(camera);

		zegar.drawM();
		wahadlo.draw();
		wskazSek.draw();
		wskazMin.draw();
		wskazGodzin.draw();
		podloga.drawM();
		kaczka.drawM();
		for (int i = 0; i < widoczneZebatki; ++i) zebatka[11 - i].draw();

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
			else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
		}

		EndMode3D();

		DrawFPS(10, 10);
		if (!hourChange && !minuteChange && !secondChange)
		{
			DrawText(napis1.c_str(), 10, screenHeight - 20, 20, MAGENTA);
		}
		else DrawText(napis1.c_str(), 10, screenHeight - 20, 20, RED);
		
		EndDrawing();
	}

	UnloadSound(tick);
	UnloadSound(quack);
	UnloadShader(shader);
	CloseAudioDevice();
	CloseWindow();

	return 0;
}

