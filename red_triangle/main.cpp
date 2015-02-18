// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>

#include <AntTweakBar.h>


int main(void)
{
	
	float rotX_central = 0;
	float rotY_central = 0;
	float rotZ_central = 0;

	float rotX_left = 0;
	float rotY_left = 0;
	float rotZ_left = 0;

	float rotX_right = 0;
	float rotY_right = 0;
	float rotZ_right = 0;
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(1024, 768);

	TwBar * gyroscope = TwNewBar("gyroscope");
	TwSetParam(gyroscope, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwDefine(" gyroscope size='150 84' ");
	TwDefine(" gyroscope position='438 180' ");
	TwAddVarRW(gyroscope, "Yaw", TW_TYPE_FLOAT, &rotY_central, "step=0.01");
	TwAddVarRW(gyroscope, "Roll", TW_TYPE_FLOAT, &rotX_central, "step=0.01");
	TwAddVarRW(gyroscope, "Pitch", TW_TYPE_FLOAT, &rotZ_central, "step=0.01");

	TwBar * accelerometer = TwNewBar("accelerometer");
	TwSetParam(accelerometer, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwDefine(" accelerometer size='150 84' ");
	TwDefine(" accelerometer position='126 180' ");
	TwAddVarRW(accelerometer, "Yaw", TW_TYPE_FLOAT, &rotY_left, "step=0.01");
	TwAddVarRW(accelerometer, "Roll", TW_TYPE_FLOAT, &rotX_left, "step=0.01");
	TwAddVarRW(accelerometer, "Pitch", TW_TYPE_FLOAT, &rotZ_left, "step=0.01");

	TwBar * complementary = TwNewBar("complementary");
	TwSetParam(complementary, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwDefine(" complementary size='150 84' ");
	TwDefine(" complementary position='750 180' ");
	TwAddVarRW(complementary, "Yaw", TW_TYPE_FLOAT, &rotY_right, "step=0.01");
	TwAddVarRW(complementary, "Roll", TW_TYPE_FLOAT, &rotX_right, "step=0.01");
	TwAddVarRW(complementary, "Pitch", TW_TYPE_FLOAT, &rotZ_right, "step=0.01");


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -0.1f, -1.0f,
		-1.0f, -0.1f, 1.0f,
		-1.0f, 0.1f, 1.0f,

		1.0f, 0.1f, -1.0f,
		-1.0f, -0.1f, -1.0f,
		-1.0f, 0.1f, -1.0f,

		1.0f, -0.1f, 1.0f,
		-1.0f, -0.1f, -1.0f,
		1.0f, -0.1f, -1.0f,

		1.0f, 0.1f, -1.0f,
		1.0f, -0.1f, -1.0f,
		-1.0f, -0.1f, -1.0f,

		-1.0f, -0.1f, -1.0f,
		-1.0f, 0.1f, 1.0f,
		-1.0f, 0.1f, -1.0f,

		1.0f, -0.1f, 1.0f,
		-1.0f, -0.1f, 1.0f,
		-1.0f, -0.1f, -1.0f,

		-1.0f, 0.1f, 1.0f,
		-1.0f, -0.1f, 1.0f,
		1.0f, -0.1f, 1.0f,

		1.0f, 0.1f, 1.0f,
		1.0f, -0.1f, -1.0f,
		1.0f, 0.1f, -1.0f,

		1.0f, -0.1f, -1.0f,
		1.0f, 0.1f, 1.0f,
		1.0f, -0.1f, 1.0f,

		1.0f, 0.1f, 1.0f,
		1.0f, 0.1f, -1.0f,
		-1.0f, 0.1f, -1.0f,

		1.0f, 0.1f, 1.0f,
		-1.0f, 0.1f, -1.0f,
		-1.0f, 0.1f, 1.0f,

		1.0f, 0.1f, 1.0f,
		-1.0f, 0.1f, 1.0f,
		1.0f, -0.1f, 1.0f
	};

	static const GLfloat g_color_buffer_data[] = {
		0.0f, 1.0f, 0.0f,  //a
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 1.0f, //b
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.4f, 0.2f, //c
		0.0f, 0.4f, 0.2f,
		0.0f, 0.4f, 0.2f,

		0.0f, 0.0f, 1.0f, //b
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f,  //a
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.4f, 0.2f, //c
		0.0f, 0.4f, 0.2f,
		0.0f, 0.4f, 0.2f,

		1.0f, 0.0f, 0.0f, 
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 1.0f, 0.0f,  //q
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,  //q
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		0.8f, 0.5f, 0.0f, //d
		0.8f, 0.5f, 0.0f,
		0.8f, 0.5f, 0.0f,

		0.8f, 0.5f, 0.0f, //d
		0.8f, 0.5f, 0.0f,
		0.8f, 0.5f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);



	do{
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
			rotX_left+= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
			rotX_left -= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
			rotY_left += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
			rotY_left -= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS){
			rotZ_left += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS){
			rotZ_left -= 0.1;
		}


		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			rotX_central += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
			rotX_central -= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			rotY_central += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
			rotY_central -= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS){
			rotZ_central += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS){
			rotZ_central -= 0.1;
		}

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
			rotX_right += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
			rotX_right -= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
			rotY_right += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS){
			rotY_right -= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS){
			rotZ_right += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS){
			rotZ_right -= 0.1;
		}


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 ViewMatrix = glm::lookAt(
			glm::vec3(0, 0, 12), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		glm::mat4 ModelMatrix_left = translate(mat4(), glm::vec3(-4.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0), rotX_left, glm::vec3(1.0f, 0.0f, 0.0f)) *
			rotate(glm::mat4(1.0), rotY_left, glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0), rotZ_left, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 ModelMatrix_central = rotate(glm::mat4(1.0), rotX_central, glm::vec3(1.0f, 0.0f, 0.0f)) *
			rotate(glm::mat4(1.0), rotY_central, glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0), rotZ_central, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 ModelMatrix_right = translate(mat4(), glm::vec3(4.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0), rotX_right, glm::vec3(1.0f, 0.0f, 0.0f)) *
			rotate(glm::mat4(1.0), rotY_right, glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0), rotZ_right, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 MVP_left = ProjectionMatrix * ViewMatrix * ModelMatrix_left;
		glm::mat4 MVP_central = ProjectionMatrix * ViewMatrix * ModelMatrix_central;
		glm::mat4 MVP_right = ProjectionMatrix * ViewMatrix * ModelMatrix_right;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_central[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles



		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_left[0][0]);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles




		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_right[0][0]);


		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles






		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		TwDraw();

		// Swap buffers
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	TwTerminate();

	return 0;
}

