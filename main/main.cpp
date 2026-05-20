#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <string>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

#include "simple_mesh.hpp"
#include "loadobj.hpp"
#include "load_texture.hpp"
#include "defaults.hpp"
#include "rocket.hpp"


namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";

	constexpr float kPi_ = 3.1415926f;

	constexpr float kMovementPerSecond_ = 5.f; // units per second
	constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel


	struct State_
	{
		ShaderProgram* prog;
		int windowWidth; 
		int windowHeight; 
		struct CamCtrl_
		{
			// act
			bool cameraActive;
			bool zoomIn, zoomOut;
			bool moveLeft, moveRight;
			bool moveUp, moveDown;
			bool speedUp, slowDown;
			bool shoot;
			bool is_split, start_split;
			bool split_shift;
			bool showStartupPage;

			// zoom
			float phi, theta;
			float radius;
			float Pos_x, Pos_y;
			float shootTime, height;

			// positon
			float lastX, lastY;

			//each model
			int state_Left, state_Right;
		} camControl;
	};
	void glfw_callback_error_(int, char const*);

	void glfw_callback_key_(GLFWwindow*, int, int, int, int);
	void glfw_callback_motion_(GLFWwindow*, double, double);

	// Bind mouse activities to window
	void mouseButtonCallback(GLFWwindow*, int, int, int);

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

// Task 1.11: for button and mose check in UI implementation
float x1 = -0.5f;
float y11 = -0.85f;
float width1 = 0.2f;
float height1 = 0.15f;
float x2 = 0.3f;
float y2 = -0.85f;

int main() try
{
	// Initialize GLFW
	if (GLFW_TRUE != glfwInit())
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwInit() failed with '%s' (%d)", msg, ecode);
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback(&glfw_callback_error_);

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if (!window)
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwCreateWindow() failed with '%s' (%d)", msg, ecode);
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	// TODO: Additional event handling setup
	State_ state{};
	glfwSetWindowUserPointer(window, &state);
	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);
	// Set mouse button callback
	glfwSetMouseButtonCallback(window, &mouseButtonCallback);
	// Set up drawing stuff
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
		throw Error("gladLoaDGLLoader() failed - cannot load GL API!");

	std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
	std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
	std::printf("VERSION %s\n", glGetString(GL_VERSION));
	std::printf("SHADING_LANGUAGE_VERSION %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	std::printf("\n===== START PAGE / GAMEPLAY GUIDE =====\n");
	std::printf("Enter: start game\n");
	std::printf("Space: enable camera control\n");
	std::printf("W/S: zoom in/out, A/D: move left/right, Q/E: move up/down\n");
	std::printf("F: launch rocket, R: reset rocket\n");
	std::printf("C: switch camera, V: split screen, Shift/Ctrl: speed up/down\n");
	std::printf("Mouse left button: launch button, right green button: reset button\n");
	std::printf("========================================\n\n");

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.f);
	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	int iwidth, iheight;

	glfwGetFramebufferSize(window, &iwidth, &iheight);

	glViewport(0, 0, iwidth, iheight);

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();
	std::unordered_map<std::string, Material> parlahti_materials;
	//std::unordered_map<std::string, Material> launchpad_materials;

	SimpleMeshData parlahti_obj = load_obj_file("assets/parlahti.obj");
	parlahti_materials = load_mtl("assets/parlahti.obj");
	GLuint parlahti_vao = bind_vao(parlahti_obj);
	std::size_t parlahti_vertex = parlahti_obj.vertex_positions.size();
	GLuint parlahti_tex = parlahti_materials["Material.001"].texture;
	
	//landingpad obj data
	SimpleMeshData launchpad_obj = load_obj_file("assets/landingpad.obj");
	GLuint landingpad_vao = bind_vao(launchpad_obj);
	std::size_t landing_vertex = launchpad_obj.vertex_positions.size();

	auto cyl = make_cylinder(true, 8, { 1.f, 0.4f, 0.f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.3f, 0.3f)
	);
	auto cy2 = make_cylinder(true, 6, { 0.0f, 0.3f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.6f, 0.2f, 0.2f) *
		make_translation({ 0.5f, 0.f, 0.f })
	);
	auto cy3 = make_cylinder(true, 10, { 0.0f, 0.6f, 0.3f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.5f, 0.15f, 0.15f) *
		make_translation({ 1.8f, 0.f, 0.f })
	);
	auto cy4 = make_cylinder(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.6f, 0.1f, 0.1f) *
		make_translation({ 2.3f, 0.f, 0.f })
	);
	auto cy5 = make_cylinder(true, 4, { 0.3f, 0.8f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 1.f) *
		make_rotation_z(3.141592f / 2.f * 1.f) *
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ 5.f, 2.8f, 0.f })
	);
	auto cy6 = make_cylinder(true, 4, { 0.3f, 0.4f, 0.2f },
		make_rotation_y(3.141592f / 6.f * 3.f) *
		make_rotation_z(3.141592f / 2.f * 1.f) *
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ 5.f, 2.8f, 0.f })
	);
	auto cy7 = make_cylinder(true, 4, { 0.3f, 0.8f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 5.f) *
		make_rotation_z(3.141592f / 2.f * 1.f) *
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ 5.f, 2.8f, 0.f })
	);
	auto cy8 = make_cylinder(true, 4, { 0.3f, 0.2f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 7.f) *
		make_rotation_z(3.141592f / 2.f * 1.f) *
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ 5.f, 2.8f, 0.f })
	);
	auto cy9 = make_cylinder(true, 4, { 0.3f, 0.8f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 9.f) *
		make_rotation_z(3.141592f / 2.f * 1.f) *
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ 5.f, 2.8f, 0.f })
	);
	auto cy10 = make_cylinder(true, 4, { 0.3f, 0.4f, 0.2f },
		make_rotation_y(3.141592f / 6.f * 11.f) *
		make_rotation_z(3.141592f / 2.f * 1.f) *
		make_scaling(0.1f, 0.1f, 0.1f) *
		make_translation({ 5.f, 2.8f, 0.f })
	);

	// back fire of the ship
	auto cy11 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.1f, 0.1f) *
		make_translation({ -1.f, 0.f, 0.f })
	);
	auto ship1 = concatenate(std::move(cyl), cy2);
	auto ship2 = concatenate(std::move(ship1), cy3);
	auto ship3 = concatenate(std::move(ship2), cy4);
	auto ship4 = concatenate(std::move(ship3), cy5);
	auto ship5 = concatenate(std::move(ship4), cy6);
	auto ship6 = concatenate(std::move(ship5), cy7);
	auto ship7 = concatenate(std::move(ship6), cy8);
	auto ship8 = concatenate(std::move(ship7), cy9);
	auto ship9 = concatenate(std::move(ship8), cy10);
	auto ship = concatenate(std::move(ship9), cy11);
	GLuint ship_vao = bind_vao(ship);
	std::size_t vertexCount_ship = ship.vertex_positions.size();

	// back fire of the ship
	auto cy21 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.1f, 0.1f) *
		make_translation({ -1.f, 0.f, 0.f })
	);
	// back fire of the ship
	auto cy22 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.1f, 0.1f) *
		make_translation({- 2.f, 0.f, 0.f })
	);
	// back fire of the ship
	auto cy23 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.1f, 0.1f) *
		make_translation({ -3.f, 0.f, 0.f })
	);
	// back fire of the ship
	auto cy24 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.1f, 0.1f) *
		make_translation({ -4.f, 0.f, 0.f })
	);// back fire of the ship
	auto cy25 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.1f, 0.1f) *
		make_translation({ -5.f, 0.f, 0.f })
	);
	// back fire of the ship
	auto cy26 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) *
		make_scaling(0.3f, 0.1f, 0.1f) *
		make_translation({ -6.f, 0.f, 0.f })
	);
	auto ship11 = concatenate(std::move(cy21), cy22);
	auto ship12 = concatenate(std::move(ship11), cy23);
	auto ship13 = concatenate(std::move(ship12), cy24);
	auto ship14 = concatenate(std::move(ship13), cy25);
	auto ship15 = concatenate(std::move(ship14), cy26);
	GLuint ship_vao11 = bind_vao(ship15);
	std::size_t vertexCount_ship11 = ship15.vertex_positions.size();

	//build the shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert"},
		{GL_FRAGMENT_SHADER, "assets/default.frag"}
		});
	
	ShaderProgram prog_landingpad({
		{GL_VERTEX_SHADER, "assets/landingpad.vert"},
		{GL_FRAGMENT_SHADER, "assets/landingpad.frag"}
		});
	// for task 1.11 UI elements
	ShaderProgram prog_ui({
		{GL_VERTEX_SHADER, "assets/button.vert"},
		{GL_FRAGMENT_SHADER, "assets/button.frag"}
		});


	static float const kPositions1[] = {
		x1,          y11,           
		x1,          y11 - height1, 
		x1 + width1, y11 - height1, 
		x1 + width1, y11            
	};
	GLuint positionVBO1 = 0;
	glGenBuffers(1, &positionVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kPositions1), kPositions1, GL_STATIC_DRAW);

	static float const kColors1[] = {
		1.f, 0.f, 0.f, // vertex 0 color
		1.f, 0.f, 0.f, // vertex 1 color
		1.f, 0.f, 0.f, // vertex 2 color
		1.f, 0.f, 0.f  // vertex 3 color
	};
	GLuint colorVBO1 = 0;
	glGenBuffers(1, &colorVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kColors1), kColors1, GL_STATIC_DRAW);

	GLuint vao1 = 0;
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO1);
	glVertexAttribPointer(
		0, // location = 0 in vertex shader
		2, GL_FLOAT, GL_FALSE, // 2 floats, not normalized to [0..1] (GL FALSE)
		0, // stride = 0 indicates that there is no padding between inputs
		0 // data starts at offset 0 in the VBO.
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO1);
	glVertexAttribPointer(
		1, // location = 1 in vertex shader
		3, GL_FLOAT, GL_FALSE, // 3 floats, not normalized to [0..1] (GL FALSE)
		0, // see above
		0 // see above
	);
	glEnableVertexAttribArray(1);

	static float const kPositions2[] = {
		x2,          y2,          
		x2,          y2 - height1, 
		x2 + width1, y2 - height1, 
		x2 + width1, y2            
	};
	GLuint positionVBO2 = 0;
	glGenBuffers(1, &positionVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kPositions2), kPositions2, GL_STATIC_DRAW);

	static float const kColors2[] = {
		0.f, 1.f, 0.f, // vertex 0 color (green)
		0.f, 1.f, 0.f, // vertex 1 color (green)
		0.f, 1.f, 0.f, // vertex 2 color (green)
		0.f, 1.f, 0.f  // vertex 3 color (green)
	};
	GLuint colorVBO2 = 0;
	glGenBuffers(1, &colorVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kColors2), kColors2, GL_STATIC_DRAW);

	GLuint vao2 = 0;
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO2);
	glVertexAttribPointer(
		0, // location = 0 in vertex shader
		2, GL_FLOAT, GL_FALSE, // 2 floats, not normalized to [0..1] (GL FALSE)
		0, // stride = 0 indicates that there is no padding between inputs
		0 // data starts at offset 0 in the VBO.
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO2);
	glVertexAttribPointer(
		1, // location = 1 in vertex shader
		3, GL_FLOAT, GL_FALSE, // 3 floats, not normalized to [0..1] (GL FALSE)
		0, // see above
		0 // see above
	);
	glEnableVertexAttribArray(1);

	state.prog = &prog;
	state.camControl.showStartupPage = true;
	state.camControl.Pos_x = 0.0f;
	state.camControl.Pos_y = 0.0f;
	state.camControl.state_Left = 0;
	state.camControl.state_Right = 0;
	state.camControl.radius = 10.f;
	state.camControl.height = 0.f;
	state.camControl.is_split = false;
	state.camControl.split_shift = false;
	state.windowWidth = iwidth;
	state.windowHeight = iheight;

	auto last = Clock::now();

	float angle = 0.f;
	// TODO: global GL setup goes here

	OGL_CHECKPOINT_ALWAYS();

	// Task 1.12: evaluate the time it takes to render a frame
	// Create GL_TIMESTAMP query objects
	GLuint queryID[4];
	int queryIndex = 0;
	// Create three query objects
	glGenQueries(4, queryID);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// 1.12-1 : Start the query timer for the whole frame
		glQueryCounter(queryID[queryIndex], GL_TIMESTAMP);

		// Let GLFW process events
		glfwPollEvents();
		int nwidth, nheight;
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			
			glfwGetFramebufferSize(window, &nwidth, &nheight);

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if (0 == nwidth || 0 == nheight)
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize(window, &nwidth, &nheight);
				} while (0 == nwidth || 0 == nheight);
			}
			
		}

		state.windowHeight = nheight;
		state.windowWidth = nwidth;

		if (state.camControl.showStartupPage)
		{
			glViewport(0, 0, nwidth, nheight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(prog_ui.programId());
			static float const baseColor[] = { 0.2f, 1.f, 1.f };
			glUniform3fv(0, 1, baseColor);
			glBindVertexArray(vao1);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(vao2);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(0);
			glUseProgram(0);
			glfwSwapBuffers(window);
			continue;
		}

		// Update state
		//TODO: update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;


		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		// Update camera state
		// initiate the view state
		Mat44f Rx;
		Mat44f Ry;
		Mat44f T;
		float remote_x = 0.f;
		float remote_y = 0.f;
		float remote_z = 0.f;
		Mat44f T1 = make_translation({ 0.f,0.f,0.f });
		Mat44f T2 = make_rotation_z(0.f);
		Mat44f T_trans = make_rotation_x(0.f);	
		float currentSpeed = kMovementPerSecond_;

		//shoot animation
		if (state.camControl.shoot) {
			if (state.camControl.shootTime < 30.f) {
				state.camControl.shootTime += 0.25f;
			}
			// angle rely on the shoot time
			float th = 0.2f * kPi_ * state.camControl.shootTime * state.camControl.shootTime / 180.0f;
			// change the position of the rocket
			remote_y = 25.0f * sin(th);
			// move camera to follow the rocket
			state.camControl.height = remote_y;
			remote_x = 35.f - 35.f * cos(th);
			// Translating the rocket with matrix
			T_trans = make_rotation_x(kPi_ / 2.f - th - 3.141592f / 2.f);
			T1 = make_translation({ remote_x, remote_y, remote_z }) * T_trans;
			T2 = make_rotation_z(kPi_ / 2.f - th - kPi_ / 2.f);
		}
		if (state.camControl.state_Left == 0) {
			if (state.camControl.speedUp) {
				currentSpeed = currentSpeed * 2.f;
			}
			else if (state.camControl.slowDown) {
				currentSpeed = currentSpeed / 2.f;
			}
			if (state.camControl.zoomIn) {
				state.camControl.radius -= currentSpeed * dt;
			}
			else if (state.camControl.zoomOut) {
				state.camControl.radius += currentSpeed * dt;
			}
			else if (state.camControl.moveLeft) {
				state.camControl.Pos_x -= currentSpeed * dt;
			}
			else if (state.camControl.moveRight) {
				state.camControl.Pos_x += currentSpeed * dt;
			}
			else if (state.camControl.moveDown) {
				state.camControl.Pos_y -= currentSpeed * dt;
			}
			else if (state.camControl.moveUp) {
				state.camControl.Pos_y += currentSpeed * dt;
			}
		
			// avoid reserve
			if (state.camControl.radius <= 0.1f) {
				state.camControl.radius = 0.1f;
			}

			// update individual matrix
			Rx = make_rotation_x(state.camControl.theta);
			Ry = make_rotation_y(state.camControl.phi);
			T = make_translation({ -state.camControl.Pos_x, -state.camControl.Pos_y, -state.camControl.radius });
		}
		//split model 1
		else if (state.camControl.state_Left == 1) {
			Rx = make_rotation_x(0.f);
			Ry = make_rotation_y(0.f);
			T = make_translation({ -remote_x, -remote_y + 0.95f, -5.f });
		}
		// Task.1.8: camera2
		else {
			Rx = make_rotation_x(0.f);
			Ry = make_rotation_y(3.141592f / 2.f);
			T = make_translation({0.f, -0.97f, -10.f});
		}
		
		// World view matrix
		OGL_CHECKPOINT_DEBUG();
		Mat44f projection;
		Mat44f model2world = make_rotation_y(angle);
		Mat44f world2camera = T * Rx * Ry;
		// if spilt the screen just half of screen
		if (state.camControl.start_split) {
			projection = make_perspective_projection( 60.f * 3.1415926f / 180.f,fbwidth / 2 / float(fbheight),0.1f, 100.0f );
		}
		else {
			projection = make_perspective_projection( 60.f * 3.1415926f / 180.f,fbwidth / float(fbheight),0.1f, 100.0f );
		}
		
		Mat44f projCameraWorld = projection * world2camera;
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2world)));


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// check if split screen mode is on
		if (state.camControl.start_split)
		{
			// split the screen
			glViewport(0, 0, static_cast<GLsizei> (nwidth / 2.f), nheight);
		}
		else {
			glViewport(0, 0, nwidth, nheight);
		}
		
		// 1.12-2 : Start the query timer for the world
		glQueryCounter(queryID[queryIndex + 1], GL_TIMESTAMP);

		// Draw the world
		glUseProgram(prog.programId());
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);

		Vec3f lightDir = normalize(Vec3f{ 0.f, 1.f, -1.f });
		glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 1.f, 1.f, 1.f);
		glUniform3f(4, 1.f, 1.f, 1.f);	//change environment light
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, parlahti_tex);
		glBindVertexArray(parlahti_vao);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei> (parlahti_vertex));
		glBindVertexArray(0);
		glUseProgram(0);

		// 1.12-2 : Stop the query timer for the world
		glQueryCounter(queryID[queryIndex + 1], GL_TIMESTAMP);

		// 1.12-3 : Start the query timer for the launchpad
		glQueryCounter(queryID[queryIndex + 2], GL_TIMESTAMP);

		// render the first launchpad
		glUseProgram(prog_landingpad.programId());
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.05f, 0.05f, 0.05f);
		glUniform3f(4, 1.f, 1.f, 1.f);
		glUniform3f(5, state.camControl.Pos_x, state.camControl.Pos_y, state.camControl.radius);
		glUniform1f(6, 0.25f);
		glUniform3f(7, -0.3f + remote_x, -0.3f + remote_y, -0.3f);
		glUniform3f(8, 0.6f, 0.6f, 0.1f);
		glUniform3f(9, 0.f + remote_x, 0.4f + remote_y, 0.f);
		glUniform3f(10, 0.7f, 0.f, 0.f);

		Mat44f position1 = make_translation({ 71.f, -0.97f, -1.0f });
		glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * position1).v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(11, 1, GL_TRUE, position1.v);
		glBindVertexArray(landingpad_vao);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei> (landing_vertex));

		Mat44f position2 = make_translation({ 1.f, -0.97f, -1.0f });
		glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * position2).v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(11, 1, GL_TRUE, position2.v);
		glBindVertexArray(landingpad_vao);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei> (landing_vertex));

		glBindVertexArray(0);
		glUseProgram(0);

		// 1.12-3 : Stop the query timer for the launchpad
		glQueryCounter(queryID[queryIndex + 2], GL_TIMESTAMP);

		// 1.12-4 : Start the query timer for the rocket
		glQueryCounter(queryID[queryIndex + 3], GL_TIMESTAMP);

		// draw rocket
		glUseProgram(prog_landingpad.programId());
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniform3f(2, 0.3f, -0.6f, 0.3f);
		glUniform3f(3, 0.8f, 0.8f, 0.8f);
		glUniform3f(4, 0.1f, 0.5f, 0.1f);
		glUniform3f(5, state.camControl.Pos_x, state.camControl.Pos_y, state.camControl.radius);
		glUniform1f(6, 0.5f);
		glUniform3f(7, -0.3f, -0.3f, -0.3f);
		glUniform3f(8, 0.6f, 0.6f, 0.1f);
		glUniform3f(9, 0.f, 0.4f, 0.f);
		glUniform3f(10, 0.7f, 0.f, 0.f);

		Mat44f rocketPosition = make_translation({ 1.f, -0.95f, -1.f });
		glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * rocketPosition * T1 * T2).v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(11, 1, GL_TRUE, rocketPosition.v);
		glBindVertexArray(ship_vao);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei> (vertexCount_ship));

		glBindVertexArray(0);
		glUseProgram(0);

		// draw tail flame
		glUseProgram(prog_landingpad.programId());
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniform3f(2, 0.3f, -0.6f, 0.3f);
		glUniform3f(3, 0.8f, 0.8f, 0.8f);
		glUniform3f(4, 0.1f, 0.5f, 0.1f);
		glUniform3f(5, state.camControl.Pos_x, state.camControl.Pos_y, state.camControl.radius);
		glUniform1f(6, 0.5f);
		glUniform3f(7, -0.3f, -0.3f, -0.3f);
		glUniform3f(8, 0.6f, 0.6f, 0.1f);
		glUniform3f(9, 0.f, 0.4f, 0.f);
		glUniform3f(10, 0.7f, 0.f, 0.f);


		glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * rocketPosition * T1 * T2).v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(11, 1, GL_TRUE, rocketPosition.v);
		glBindVertexArray(ship_vao11);
		glDrawArrays(GL_POINTS, 0, static_cast<GLsizei> (vertexCount_ship11));

		glBindVertexArray(0);
		glUseProgram(0);

		// 1.12-4 : Stop the query timer for the rocket
		glQueryCounter(queryID[queryIndex + 3], GL_TIMESTAMP);

		// 1.12-1 : stop the query timer for the whole frame
		glQueryCounter(queryID[queryIndex], GL_TIMESTAMP);

		// Draw button
		glUseProgram(prog_ui.programId());
		static float const baseColor[] = { 0.2f, 1.f, 1.f };
		glUniform3fv(0, 1, baseColor);

		// Draw first rectangle
		glBindVertexArray(vao1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Use GL_TRIANGLE_FAN for a rectangle

		// Draw second rectangle
		glBindVertexArray(vao2);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindVertexArray(0);
		glUseProgram(0);


		// Wait for the results
		GLuint64 timestamp[4];
		GLuint available = 0;
		while (!available) {
			glGetQueryObjectuiv(queryID[queryIndex], GL_QUERY_RESULT_AVAILABLE, &available);
		}
		glGetQueryObjectui64v(queryID[queryIndex], GL_QUERY_RESULT, &timestamp[0]);
		glGetQueryObjectui64v(queryID[queryIndex + 1], GL_QUERY_RESULT, &timestamp[1]);
		glGetQueryObjectui64v(queryID[queryIndex + 2], GL_QUERY_RESULT, &timestamp[2]);
		glGetQueryObjectui64v(queryID[queryIndex + 3], GL_QUERY_RESULT, &timestamp[3]);

		// Calculate the elapsed time between each section
		GLuint64 elapsedTime[4];
		elapsedTime[0] = timestamp[1] - timestamp[0];
		elapsedTime[1] = timestamp[2] - timestamp[1];
		elapsedTime[2] = timestamp[3] - timestamp[2];
		elapsedTime[3] = timestamp[3] - timestamp[0];

		// Convert the time to nanoseconds
		float time[4];
		for (int i = 0; i < 4; i++) {
			time[i] = elapsedTime[i] / 1000000.0f;
		}

		// Print the elapsed time between each section
		std::printf("==============Performance Test=================\n\n");
		std::printf("Frame rendering time: %.2f ns\n", time[3]);
		std::printf("Task 1.2 rendering time: %.2f ns\n", time[0]);
		std::printf("Task 1.4 rendering time: %.2f ns\n", time[1]);
		std::printf("Task 1.5 rendering time: %.2f ns\n", time[2]);
		std::printf("\n==============Performance Test End=================\n\n");

		OGL_CHECKPOINT_DEBUG();

		// second time rander if spilt
		if (state.camControl.start_split) {
			// Right screen
			if (state.camControl.state_Right == 0) {
				if (state.camControl.speedUp) {
					currentSpeed = currentSpeed * 2.f;
				}
				else if (state.camControl.slowDown) {
					currentSpeed = currentSpeed / 2.f;
				}

				if (state.camControl.zoomIn) {
					state.camControl.radius -= currentSpeed * dt;
				}
				else if (state.camControl.zoomOut) {
					state.camControl.radius += currentSpeed * dt;
				}
				else if (state.camControl.moveLeft) {
					state.camControl.Pos_x -= currentSpeed * dt;
				}
				else if (state.camControl.moveRight) {
					state.camControl.Pos_x += currentSpeed * dt;
				}
				else if (state.camControl.moveDown) {
					state.camControl.Pos_y -= currentSpeed * dt;
				}
				else if (state.camControl.moveUp) {
					state.camControl.Pos_y += currentSpeed * dt;
				}

				// avoid reserve
				if (state.camControl.radius <= 0.1f) {
					state.camControl.radius = 0.1f;
				}

				// update individual matrix
				Rx = make_rotation_x(state.camControl.theta);
				Ry = make_rotation_y(state.camControl.phi);
				T = make_translation({ -state.camControl.Pos_x, -state.camControl.Pos_y, -state.camControl.radius });
			}
			//split model 1
			else if (state.camControl.state_Right == 1) {
				Rx = make_rotation_x(0.0f);
				Ry = make_rotation_y(0.f);
				T = make_translation({ -remote_x, -remote_y + 0.95f, -5.f });
			}
			else {
				float th = 0.025f * kPi_ * state.camControl.shootTime * state.camControl.shootTime / 180.0f;
				Rx = make_rotation_x(0.f);
				Ry = make_rotation_y(kPi_ - th * 90.f / 140.f);
				T = make_translation({ 10.f , 0.f, -10.f });
			}

			world2camera = T * Rx * Ry;
			projCameraWorld = projection * world2camera;
			glViewport(static_cast<GLsizei> (nwidth / 2.f), 0, static_cast<GLsizei> (nwidth / 2.f), nheight);

			OGL_CHECKPOINT_DEBUG();

			glUseProgram(prog.programId());
			glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);

			Vec3f lightDir_split = normalize(Vec3f{ 0.f, 1.f, -1.f });
			glUniform3fv(2, 1, &lightDir_split.x);
			glUniform3f(3, 1.f, 1.f, 1.f);
			glUniform3f(4, 1.f, 1.f, 1.f);	//change environment light
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, parlahti_tex);
			glBindVertexArray(parlahti_vao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast <GLsizei> (parlahti_vertex));
			glBindVertexArray(0);
			glUseProgram(0);

			// render the first launchpad
			glUseProgram(prog_landingpad.programId());
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniform3fv(2, 1, &lightDir.x);
			glUniform3f(3, 0.05f, 0.05f, 0.05f);
			glUniform3f(4, 1.f, 1.f, 1.f);
			glUniform3f(5, state.camControl.Pos_x, state.camControl.Pos_y, state.camControl.radius);
			glUniform1f(6, 0.25f);
			glUniform3f(7, -0.3f + remote_x, -0.3f + remote_y, -0.3f);
			glUniform3f(8, 0.6f, 0.6f, 0.1f);
			glUniform3f(9, 0.f + remote_x, 0.4f + remote_y, 0.f);
			glUniform3f(10, 0.7f, 0.f, 0.f);

			Mat44f position1_split = make_translation({ 71.f, -0.97f, -1.0f });
			glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * position1_split).v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, position1_split.v);
			glBindVertexArray(landingpad_vao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei> (landing_vertex));

			Mat44f position2_split = make_translation({ 1.f, -0.97f, -1.0f });
			glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * position2_split).v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, position2_split.v);
			glBindVertexArray(landingpad_vao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei> (landing_vertex));

			glBindVertexArray(0);
			glUseProgram(0);

			// draw rocket
			glUseProgram(prog_landingpad.programId());
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniform3f(2, 0.3f, -0.6f, 0.3f);
			glUniform3f(3, 0.8f, 0.8f, 0.8f);
			glUniform3f(4, 0.1f, 0.5f, 0.1f);
			glUniform3f(5, state.camControl.Pos_x, state.camControl.Pos_y, state.camControl.radius);
			glUniform1f(6, 0.5f);
			glUniform3f(7, -0.3f, -0.3f, -0.3f);
			glUniform3f(8, 0.6f, 0.6f, 0.1f);
			glUniform3f(9, 0.f, 0.4f, 0.f);
			glUniform3f(10, 0.7f, 0.f, 0.f);

			Mat44f rocketPosition_split = make_translation({ 1.f, -0.95f, -1.f });
			glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * rocketPosition_split * T1 * T2).v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, rocketPosition.v);
			glBindVertexArray(ship_vao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei> (vertexCount_ship));

			glBindVertexArray(0);
			glUseProgram(0);

			OGL_CHECKPOINT_DEBUG();
		}

		// Display results
		glfwSwapBuffers(window);
	}

	// Cleanup.
	//TODO: additional cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	// Cleanup.
	state.prog = nullptr;

	//Task 1.12 : Clean up
	// Clean up query objects
	glDeleteQueries(4, queryID);

	return 0;
}
catch (std::exception const& eErr)
{
	std::fprintf(stderr, "Top-level Exception (%s):\n", typeid(eErr).name());
	std::fprintf(stderr, "%s\n", eErr.what());
	std::fprintf(stderr, "Bye.\n");
	return 1;
}


namespace
{
	void glfw_callback_error_(int aErrNum, char const* aErrDesc)
	{
		std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
	}

	void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
	{
		if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
		{
			glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.showStartupPage)
			{
				if (GLFW_KEY_ENTER == aKey && GLFW_PRESS == aAction)
					state->camControl.showStartupPage = false;
				return;
			}
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->prog)
				{
					try
					{
						state->prog->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}
			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;

				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			// Start to accpet inputs
			if (state->camControl.cameraActive) {
				// W: zoom in
				if (GLFW_KEY_W == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.zoomIn = true;
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.zoomIn = false;
					}
				}
				// E: move down
				else if (GLFW_KEY_E == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.moveDown = true;
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.moveDown = false;
					}
				}
				// A: move left
				else if (GLFW_KEY_A == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.moveLeft = true;
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.moveLeft = false;
					}
				}
				// S: zoom out
				else if (GLFW_KEY_S == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.zoomOut = true;
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.zoomOut = false;
					}
				}
				// D: move left
				else if (GLFW_KEY_D == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.moveRight = true;
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.moveRight = false;
					}
				}
				// Q: move up
				else if (GLFW_KEY_Q == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.moveUp = true;
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.moveUp = false;
					}
				}
				// change view model
				else if (GLFW_KEY_C == aKey) {
					if (GLFW_RELEASE == aAction) {
						if (state->camControl.state_Left == 2) {
							state->camControl.state_Left = 0;
						}
						else {
							state->camControl.state_Left += 1;
						}
						if (state->camControl.start_split) {
							state->camControl.is_split = true;
							if (state->camControl.split_shift) {
								if (state->camControl.state_Right == 2) {
									state->camControl.state_Right = 0;
								}
								else {
									state->camControl.state_Right += 1;
								}
							}
						}
					}
					else if (GLFW_RELEASE == aAction)
						state->camControl.is_split = false;
				}
				// SHIFT: speed up
				else if (GLFW_KEY_LEFT_SHIFT == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.speedUp = true;
						if (state->camControl.start_split) {
							state->camControl.split_shift = true;
							if (state->camControl.is_split) {
								if (state->camControl.state_Right == 2) {
									state->camControl.state_Right = 0;
								}
								else {
									state->camControl.state_Right += 1;
								}
							}
						}
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.speedUp = false;
						state->camControl.split_shift = false;
					}
				}
				// CONTROL: speed up
				else if (GLFW_KEY_LEFT_CONTROL == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.slowDown = true;
					}
					else if (GLFW_RELEASE == aAction) {
						state->camControl.slowDown = false;
					}
				}
				// space ship launch button!
				else if (GLFW_KEY_F == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.shoot = true;
					}
				}
				// Reset button
				else if (GLFW_KEY_R == aKey) {
					if (GLFW_PRESS == aAction) {
						state->camControl.shoot = false;
						state->camControl.shootTime = 0.f;
					}
				}
				//split button
				else if (GLFW_KEY_V == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.start_split = !state->camControl.start_split;
				}
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive)
			{
				auto const dx = float(aX - state->camControl.lastX);
				auto const dy = float(aY - state->camControl.lastY);

				state->camControl.phi += dx * kMouseSensitivity_;

				state->camControl.theta += dy * kMouseSensitivity_;
				if (state->camControl.theta > kPi_ / 2.f)
					state->camControl.theta = kPi_ / 2.f;
				else if (state->camControl.theta < -kPi_ / 2.f)
					state->camControl.theta = -kPi_ / 2.f;
			}

			state->camControl.lastX = float(aX);
			state->camControl.lastY = float(aY);
		}
	}

	// Task 1.11: implement UI controls of rocket shoot and reset

	void mouseButtonCallback(GLFWwindow* aWindow, int button, int action, int mods) {
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			double mouseX, mouseY;
			glfwGetCursorPos(aWindow, &mouseX, &mouseY);

			// Turn into OpenGL coordinates
			float normalizedX = (2.0f * static_cast<float> (mouseX)) / state->windowWidth - 1.0f;
			float normalizedY = 1.0f - (2.0f * static_cast<float> (mouseY)) / state->windowHeight;

			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				if (state->camControl.showStartupPage)
				{
					if (normalizedX >= x1 && normalizedX <= (x1 + width1) && normalizedY <= y11 && normalizedY >= (y11 - height1))
						state->camControl.showStartupPage = false;
					else if (normalizedX >= x2 && normalizedX <= (x2 + width1) && normalizedY <= y2 && normalizedY >= (y2 - height1))
						glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
					return;
				}
				mods = 1;
				if (normalizedX >= x1 && normalizedX <= (x1 + width1) && normalizedY <= y11 && normalizedY >= (y11 - height1)) {
					if (!state->camControl.shoot)
						state->camControl.shoot = true;
				}
				else if (normalizedX >= x2 && normalizedX <= (x2 + width1) && normalizedY <= y2 && normalizedY >= (y2 - height1)) {
					mods = 2;
					state->camControl.shoot = false;
					state->camControl.shootTime = 0.0f;
				}
			}
			/*else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
				
			}*/
		}
	}
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if (window)
			glfwDestroyWindow(window);
	}
}