#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*
Joyce Huang
February 16th, 2020
CS-UY 3113 Intro to Game Programming
Project 1: Simple 2D Scene
*/

/*
Instructions: Create a simple 2D scene
- Have at least 2 texture objects (with different textures)
- Have something moving and something rotating
- You can use any images you want or use the ones in the github repository
*/

SDL_Window * displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix_2, projectionMatrix;

float player_x = 0; 
float player_rotate = 0; 

GLuint LoadTexture(const char* filepath);
GLuint playerTextureID; 
GLuint playerTextureID_2; 

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Project 1: Create a Simple 2D Scene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	// program.Load("shaders/vertex.glsl", "shaders/fragment.glsl"); // for the triangles
	// Load the shaders for handling textures 
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl"); // for the textures 

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix_2 = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(0.0f, 1.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Load player image
	playerTextureID = LoadTexture("ctg.png");
	playerTextureID_2 = LoadTexture("ctg.png");


	// Enable blending 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

GLuint LoadTexture(const char* filepath) {
	int w, h, n;
	unsigned char* image = stbi_load(filepath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n"; 
		assert(false); 
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

	stbi_image_free(image); 
	return textureID;
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}
}

float lastTicks = 0.0f; 

void Update() { 

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks; 
	lastTicks = ticks; 
	
	player_x += 1.0 * deltaTime;  
	player_rotate += 45.0 * deltaTime; 

	modelMatrix = glm::mat4(1.0f); 
	// modelMatrix_2 = glm::mat4(1.0f); 

	modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, player_x, 0.0f)); 
	modelMatrix = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

	// modelMatrix_2 = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	// modelMatrix_2 = glm::translate(modelMatrix, glm::vec3(player_x, player_x, 0.0f));
	// modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f)); 

}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	program.SetModelMatrix(modelMatrix);
	program.SetModelMatrix(modelMatrix_2);
	

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 }; 
	
	// /* for the textured images
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute); 

	glBindTexture(GL_TEXTURE_2D, playerTextureID);
	glBindTexture(GL_TEXTURE_2D, playerTextureID_2);
	glDrawArrays(GL_TRIANGLES, 0, 6); 

	glDisableVertexAttribArray(program.positionAttribute); 
	glDisableVertexAttribArray(program.texCoordAttribute); 
	// */

	/* for the triangles
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices_2);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(program.positionAttribute);
	*/

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}