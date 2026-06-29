#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <array>
#include <vector>
#include <cstring>
#include <cctype>

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
#include "level_data.hpp"
#include "text_renderer.hpp"
#include "ship_builder.hpp"
#include "game_mechanics.hpp"
#include "hud_renderer.hpp"


namespace
{
	using namespace level_data;
	using text_renderer::Vertex2D;
	using text_renderer::push_rect;

	constexpr char const* kWindowTitle = "Rocket 3D";

	constexpr float kPi_ = kPi;
	constexpr float kMovementPerSecond_ = kMovementPerSecond;
	constexpr float kMouseSensitivity_ = kMouseSensitivity;
	constexpr float kRocketSteerPerSecond_ = kRocketSteerPerSecond;
	constexpr float kLaunchSpeedMin_ = kLaunchSpeedMin;
	constexpr float kLaunchSpeedMax_ = kLaunchSpeedMax;
	constexpr float kLevelAdvanceDelay_ = kLevelAdvanceDelay;
	constexpr float kFollowCamSmoothing_ = kFollowCamSmoothing;
	constexpr float kFollowCamRecentering_ = kFollowCamRecentering;
	constexpr float kMaxFuel_ = kMaxFuel;
	constexpr float kFuelBurnRate_ = kFuelBurnRate;
	constexpr float kSteerFuelCost_ = kSteerFuelCost;
	constexpr float kCountdownDuration_ = kCountdownDuration;

	struct UiBatch
	{
		GLuint vao = 0;
		GLuint vbo = 0;
	};

	void init_batch(UiBatch& b)
	{
		glGenVertexArrays(1, &b.vao);
		glGenBuffers(1, &b.vbo);
		glBindVertexArray(b.vao);
		glBindBuffer(GL_ARRAY_BUFFER, b.vbo);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, x)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, r)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}

	void draw_vertices(ShaderProgram& uiProg, UiBatch& b, std::vector<Vertex2D> const& verts)
	{
		if (verts.empty())
			return;
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glUseProgram(uiProg.programId());
		float const white[] = { 1.f, 1.f, 1.f };
		glUniform3fv(0, 1, white);
		glBindVertexArray(b.vao);
		glBindBuffer(GL_ARRAY_BUFFER, b.vbo);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex2D), verts.data(), GL_STREAM_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));
		glBindVertexArray(0);
		glUseProgram(0);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void draw_text(ShaderProgram& uiProg, UiBatch& batch, int windowWidth, int windowHeight, float xPx, float yPx, float sizePx, float r, float g, float b, char const* text)
	{
		auto verts = text_renderer::build_text_vertices(windowWidth, windowHeight, xPx, yPx, sizePx, r, g, b, text);
		draw_vertices(uiProg, batch, verts);
	}

	void draw_text_centered_in_ndc_rect(
		ShaderProgram& uiProg,
		UiBatch& batch,
		int windowWidth,
		int windowHeight,
		float x,
		float y,
		float w,
		float h,
		float sizePx,
		float r,
		float g,
		float b,
		char const* text
	)
	{
		float const left = (x + 1.f) * 0.5f * float(windowWidth);
		float const right = (x + w + 1.f) * 0.5f * float(windowWidth);
		float const top = (1.f - y) * 0.5f * float(windowHeight);
		float const bottom = (1.f - (y - h)) * 0.5f * float(windowHeight);
		float const rectWidth = right - left;
		float const rectHeight = bottom - top;
		float const textWidth = text_renderer::measure_text_width_px(text, sizePx);
		float const tx = left + (rectWidth - textWidth) * 0.5f;
		float const ty = top + (rectHeight - sizePx) * 0.5f;
		draw_text(uiProg, batch, windowWidth, windowHeight, tx, ty, sizePx, r, g, b, text);
	}

	struct State_
	{
		ShaderProgram* prog;
		int windowWidth; 
		int windowHeight; 
		int score;
		int successfulMissions;
		int launchCount;
		int currentLevel;
		float missionTimer;
		float launchSpeedScale;
		float gameplayTime;
		float levelTransitionTimer;
		float followCamSmoothing;
		float followCamRecentering;
		float fuel;
		float countdownTimer;
		int followCamPreset;
		GamePhase phase;
		bool followCamera;
		bool campaignCleared;
		bool missionComplete;
		bool missionFailed;
		std::array<bool, 3> targetCollected;
		std::array<bool, 3> bossGatePassed;
		struct CamCtrl_
		{
			// act
			bool cameraActive;
			bool zoomIn, zoomOut;
			bool moveLeft, moveRight;
			bool moveUp, moveDown;
			bool speedUp, slowDown;
			bool shoot;
			bool steerLeft, steerRight;
			bool steerUp, steerDown;
			bool is_split, start_split;
			bool split_shift;
			bool showStartupPage;
			bool testMode;

			// zoom
			float phi, theta;
			float radius;
			float Pos_x, Pos_y;
			float shootTime, height;
			float flightOffsetY, flightOffsetZ;
			float followPhi, followTheta;
			float lastX, lastY;
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

// In-game control button layout (NDC)
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
	std::printf("\n===== START MENU =====\n");
	std::printf("Select mode from startup menu: GAME or TEST\n");
	std::printf("G/T keys can also select mode\n");
	std::printf("Camera: SPACE + mouse, movement with W/A/S/D/Q/E\n");
	std::printf("Rocket: F launch, R reset, C camera mode, V split view\n");
	std::printf("======================\n\n");

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

	auto ship = ship_builder::build_rocket_body();
	GLuint ship_vao = bind_vao(ship);
	std::size_t vertexCount_ship = ship.vertex_positions.size();

	auto exhaustTrail = ship_builder::build_exhaust_trail(6);
	GLuint ship_vao11 = bind_vao(exhaustTrail);
	std::size_t vertexCount_ship11 = exhaustTrail.vertex_positions.size();

	auto missionBeacon3 = ship_builder::build_mission_beacon();
	GLuint missionBeaconVao = bind_vao(missionBeacon3);
	std::size_t missionBeaconVertices = missionBeacon3.vertex_positions.size();

	auto obstacleMesh = ship_builder::build_obstacle_mesh();
	GLuint obstacleVao = bind_vao(obstacleMesh);
	std::size_t obstacleVertices = obstacleMesh.vertex_positions.size();

	auto bossMesh = ship_builder::build_boss_gate_mesh();
	GLuint bossVao = bind_vao(bossMesh);
	std::size_t bossVertices = bossMesh.vertex_positions.size();

	auto fuelCanisterMesh = ship_builder::build_fuel_canister();
	GLuint fuelCanisterVao = bind_vao(fuelCanisterMesh);
	std::size_t fuelCanisterVertices = fuelCanisterMesh.vertex_positions.size();

// build the shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert"},
		{GL_FRAGMENT_SHADER, "assets/default.frag"}
		});
	
	ShaderProgram prog_landingpad({
		{GL_VERTEX_SHADER, "assets/landingpad.vert"},
		{GL_FRAGMENT_SHADER, "assets/landingpad.frag"}
		});
	// UI shader
	ShaderProgram prog_ui({
		{GL_VERTEX_SHADER, "assets/button.vert"},
		{GL_FRAGMENT_SHADER, "assets/button.frag"}
		});

	ShaderProgram prog_startup_bg({
		{GL_VERTEX_SHADER, "assets/startup_bg.vert"},
		{GL_FRAGMENT_SHADER, "assets/startup_bg.frag"}
		});

	GLuint startupBackgroundTex = load_texture("assets/L4343A-4k.jpeg");


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
		3, GL_FLOAT, GL_FALSE, // 3 floats, not normalized to [0..1] (GL_FALSE)
		0, // see above
		0 // see above
	);
	glEnableVertexAttribArray(1);

	static float const kStartupBgVerts[] = {
		-1.f, -1.f, 0.f, 0.f,
		 1.f, -1.f, 1.f, 0.f,
		-1.f,  1.f, 0.f, 1.f,
		 1.f,  1.f, 1.f, 1.f
	};

	GLuint startupBgVao = 0;
	GLuint startupBgVbo = 0;
	glGenVertexArrays(1, &startupBgVao);
	glGenBuffers(1, &startupBgVbo);
	glBindVertexArray(startupBgVao);
	glBindBuffer(GL_ARRAY_BUFFER, startupBgVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kStartupBgVerts), kStartupBgVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(sizeof(float) * 2));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	UiBatch textBatch{};
	init_batch(textBatch);

	state.prog = &prog;
	state.camControl.showStartupPage = true;
	state.camControl.testMode = false;
	state.score = 0;
	state.successfulMissions = 0;
	state.launchCount = 0;
	state.currentLevel = 0;
	state.launchSpeedScale = 0.75f;
	state.gameplayTime = 0.f;
	state.levelTransitionTimer = 0.f;
	state.followCamPreset = 1;
	state.followCamSmoothing = kFollowCamPresets[state.followCamPreset].smoothing;
	state.followCamRecentering = kFollowCamPresets[state.followCamPreset].recenter;
	state.followCamera = true;
	state.fuel = kMaxFuel_;
	state.countdownTimer = 0.f;
	state.phase = GamePhase::STARTUP;
	state.campaignCleared = false;
	state.missionTimer = kLevels[state.currentLevel].missionTime;
	state.missionComplete = false;
	state.missionFailed = false;
	state.targetCollected = { false, false, false };
	state.bossGatePassed = { false, false, false };
	state.camControl.Pos_x = 0.0f;
	state.camControl.Pos_y = 0.0f;
	state.camControl.state_Left = 0;
	state.camControl.state_Right = 0;
	state.camControl.radius = 10.f;
	state.camControl.height = 0.f;
	state.camControl.flightOffsetY = 0.f;
	state.camControl.flightOffsetZ = 0.f;
	state.camControl.followPhi = 0.f;
	state.camControl.followTheta = 0.f;
	state.camControl.steerLeft = false;
	state.camControl.steerRight = false;
	state.camControl.steerUp = false;
	state.camControl.steerDown = false;
	state.camControl.is_split = false;
	state.camControl.split_shift = false;
	state.windowWidth = iwidth;
	state.windowHeight = iheight;

	FuelPickupManager fuelPickups;
	fuelPickups.addCanister(FuelCanister(Vec3f{ 22.f, 7.5f, -1.f }, 20.f));
	fuelPickups.addCanister(FuelCanister(Vec3f{ 42.f, 9.0f, -1.f }, 25.f));

	ComboTracker comboTracker(3.0f, 50.0f);
	WindSystem windSystem(0.0f, 1.0f, 0.0f);
	AchievementTracker achievements;

	auto last = Clock::now();

	float angle = 0.f;
	// TODO: global GL setup goes here

	OGL_CHECKPOINT_ALWAYS();

	// GPU timing queries
	
	GLuint queryID[4];
	int queryIndex = 0;
	
	glGenQueries(4, queryID);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Frame timing start
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

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glUseProgram(prog_startup_bg.programId());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, startupBackgroundTex);
			glUniform1i(0, 0);
			glBindVertexArray(startupBgVao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
			glUseProgram(0);

			std::vector<Vertex2D> startupButtons;
			push_rect(startupButtons, kStartupGameButtonX, kStartupGameButtonY, kStartupButtonWidth, kStartupButtonHeight, 0.20f, 0.62f, 0.95f);
			push_rect(startupButtons, kStartupTestButtonX, kStartupTestButtonY, kStartupButtonWidth, kStartupButtonHeight, 0.28f, 0.80f, 0.46f);
			draw_vertices(prog_ui, textBatch, startupButtons);

			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 60.f, 40.f, 0.98f, 0.98f, 0.98f, "ROCKET 3D");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 108.f, 20.f, 0.92f, 0.96f, 1.0f, "6-LEVEL CAMPAIGN FLIGHT GAME");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 148.f, 15.f, 0.80f, 0.90f, 0.90f, "--- CONTROLS ---");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 172.f, 15.f, 0.90f, 0.96f, 0.96f, "SPACE: CAMERA ON/OFF   WASDQE: MOVE   MOUSE: LOOK");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 194.f, 15.f, 0.90f, 0.96f, 0.96f, "F: LAUNCH   R: RESET   C: CYCLE VIEW   V: SPLIT SCREEN");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 216.f, 15.f, 0.90f, 0.96f, 0.96f, "ARROWS: STEER   [ ]: SPEED   B: FOLLOW CAM   P: CAM PRESET");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 238.f, 15.f, 0.90f, 0.96f, 0.96f, ",/. ;/': TUNE SMOOTH/RECENTER   SHIFT/CTRL: MOVE SPEED");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 268.f, 15.f, 0.80f, 0.90f, 0.90f, "--- GAMEPLAY ---");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 290.f, 15.f, 0.90f, 0.96f, 0.96f, "COLLECT COINS -> FLY THROUGH BOSS RINGS -> LAND ON FINAL PAD");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 312.f, 15.f, 0.90f, 0.96f, 0.96f, "STEERING COSTS FUEL - WATCH THE FUEL BAR");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 334.f, 15.f, 0.90f, 0.96f, 0.96f, "COMBO: COLLECT QUICKLY FOR BONUS   AVOID RED OBSTACLES");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 356.f, 15.f, 0.90f, 0.96f, 0.96f, "6 LEVELS: EASY -> NORMAL -> HARD   AUTO ADVANCE ON SUCCESS");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 386.f, 15.f, 0.80f, 0.90f, 0.90f, "--- SELECT MODE ---");
			draw_text(prog_ui, textBatch, nwidth, nheight, 70.f, 408.f, 15.f, 0.90f, 0.96f, 0.96f, "GAME: 6-LEVEL CAMPAIGN   TEST: FREE FLIGHT + FRAME TIMING");
			draw_text_centered_in_ndc_rect(prog_ui, textBatch, nwidth, nheight, kStartupGameButtonX, kStartupGameButtonY, kStartupButtonWidth, kStartupButtonHeight, 22.f, 0.07f, 0.07f, 0.07f, "GAME MODE");
			draw_text_centered_in_ndc_rect(prog_ui, textBatch, nwidth, nheight, kStartupTestButtonX, kStartupTestButtonY, kStartupButtonWidth, kStartupButtonHeight, 22.f, 0.07f, 0.07f, 0.07f, "TEST MODE");

			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glfwSwapBuffers(window);
			continue;
		}

		// Update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;

		// Dynamic window title
		{
			char titleBuf[128];
			std::snprintf(titleBuf, sizeof(titleBuf), "Rocket 3D - %s [Score: %d]", kLevels[state.currentLevel].name, state.score);
			glfwSetWindowTitle(window, titleBuf);
		}


		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;
		state.gameplayTime += dt;
		auto const& level = kLevels[state.currentLevel];

		windSystem.setParameters(level.hard ? 0.8f : 0.0f, 1.0f + level.obstacleMotionScale * 0.5f, level.obstacleMotionScale * 0.4f);
		windSystem.update(state.gameplayTime);

		if (state.missionComplete && !state.campaignCleared)
		{
			state.levelTransitionTimer += dt;
			if (state.levelTransitionTimer >= kLevelAdvanceDelay_)
			{
				state.levelTransitionTimer = 0.f;
				if (state.currentLevel + 1 < static_cast<int>(kLevels.size()))
				{
					state.currentLevel += 1;
					state.missionTimer = kLevels[state.currentLevel].missionTime;
					state.missionComplete = false;
					state.missionFailed = false;
					state.targetCollected = { false, false, false };
					state.bossGatePassed = { false, false, false };
					state.camControl.shoot = false;
					state.camControl.shootTime = 0.f;
					state.camControl.flightOffsetY = 0.f;
					state.camControl.flightOffsetZ = 0.f;
					fuelPickups.resetAll();
					comboTracker.reset();
				}
				else
				{
					state.campaignCleared = true;
						achievements.unlock(AchievementId::CAMPAIGN_COMPLETE);
					}
			}
		}

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
			achievements.unlock(AchievementId::FIRST_LAUNCH);
			if (state.fuel > 0.f)
				state.fuel -= kFuelBurnRate_ * dt;
			if (state.fuel < 0.f) state.fuel = 0.f;

			if (state.camControl.shootTime < 30.f) {
				state.camControl.shootTime += dt * 6.f * state.launchSpeedScale;
				if (state.camControl.shootTime > 30.f)
					state.camControl.shootTime = 30.f;
			}
			if (state.fuel > 0.f) {
				if (state.camControl.steerLeft) { state.camControl.flightOffsetZ -= kRocketSteerPerSecond_ * dt; state.fuel -= kSteerFuelCost_ * dt; }
				if (state.camControl.steerRight) { state.camControl.flightOffsetZ += kRocketSteerPerSecond_ * dt; state.fuel -= kSteerFuelCost_ * dt; }
				if (state.camControl.steerUp) { state.camControl.flightOffsetY += (kRocketSteerPerSecond_ * 0.7f) * dt; state.fuel -= kSteerFuelCost_ * dt; }
				if (state.camControl.steerDown) { state.camControl.flightOffsetY -= (kRocketSteerPerSecond_ * 0.7f) * dt; state.fuel -= kSteerFuelCost_ * dt; }
			}
			state.camControl.flightOffsetZ += windSystem.getCurrentOffset() * dt;
			if (state.camControl.flightOffsetZ < -6.f) state.camControl.flightOffsetZ = -6.f;
			if (state.camControl.flightOffsetZ > 6.f) state.camControl.flightOffsetZ = 6.f;
			if (state.camControl.flightOffsetY < -2.5f) state.camControl.flightOffsetY = -2.5f;
			if (state.camControl.flightOffsetY > 4.0f) state.camControl.flightOffsetY = 4.0f;

			// angle rely on the shoot time
			float th = 0.2f * kPi_ * state.camControl.shootTime * state.camControl.shootTime / 180.0f;
			// change the position of the rocket
			remote_y = 25.0f * sin(th) + state.camControl.flightOffsetY;
			// move camera to follow the rocket
			state.camControl.height = remote_y;
			remote_x = 35.f - 35.f * cos(th);
			remote_z = state.camControl.flightOffsetZ;
			// Translating the rocket with matrix
			T_trans = make_rotation_x(kPi_ / 2.f - th - 3.141592f / 2.f);
			T1 = make_translation({ remote_x, remote_y, remote_z }) * T_trans;
			T2 = make_rotation_z(kPi_ / 2.f - th - kPi_ / 2.f);
		}

		auto const obstaclePositions = compute_obstacle_positions(state.gameplayTime, level.obstacleMotionScale);
		Vec3f const rocketWorldPos = { 1.f + remote_x, -0.95f + remote_y, -1.f + remote_z };
		if (!state.missionComplete && !state.missionFailed && !state.campaignCleared)
		{
			state.missionTimer -= dt;
			if (state.missionTimer <= 0.f)
			{
				state.missionTimer = 0.f;
				state.missionFailed = true;
				state.camControl.shoot = false;
			}

			if (state.camControl.shoot)
			{
				for (auto const& p : obstaclePositions)
				{
					Vec3f const d = rocketWorldPos - p;
					if (dot(d, d) < level.obstacleRadius * level.obstacleRadius)
					{
						state.missionFailed = true;
						state.camControl.shoot = false;
						state.phase = GamePhase::FAILED;
						break;
					}
				}

				int prevCollected = 0;
					for (bool g : state.targetCollected) if (g) ++prevCollected;

					for (std::size_t i = 0; i < kMissionTargets.size(); ++i)
					{
						if (state.targetCollected[i])
							continue;
						Vec3f const d = rocketWorldPos - kMissionTargets[i];
						if (dot(d, d) < 2.8f)
						{
							state.targetCollected[i] = true;
							int comboBonus = comboTracker.collect(state.gameplayTime);
							state.score += 100 + comboBonus;
							prevCollected++;
						}
					}

					float fuelRestored = fuelPickups.checkCollection(rocketWorldPos, 1.8f);
					if (fuelRestored > 0.f)
					{
						state.fuel += fuelRestored;
						if (state.fuel > kMaxFuel_) state.fuel = kMaxFuel_;
					}
					if (fuelPickups.getTotalCount() > 0 && fuelPickups.getRemainingCount() == 0)
						achievements.unlock(AchievementId::ALL_FUEL_COLLECTED);
					fuelPickups.update(dt);
					comboTracker.update(state.gameplayTime);
					if (comboTracker.getComboCount() >= 3)
						achievements.unlock(AchievementId::COMBO_3);
					if (comboTracker.getComboCount() >= 5)
						achievements.unlock(AchievementId::COMBO_5);

				bool allCollected = true;
				for (bool got : state.targetCollected)
					allCollected = allCollected && got;

				if (allCollected)
				{
					for (std::size_t i = 0; i < kBossGateCenters.size(); ++i)
					{
						if (state.bossGatePassed[i])
							continue;
						Vec3f const d = rocketWorldPos - kBossGateCenters[i];
						if (dot(d, d) < level.bossGateRadius * level.bossGateRadius)
						{
							state.bossGatePassed[i] = true;
							state.score += 150;
						}
					}
				}

				bool allBossPassed = true;
				for (bool passed : state.bossGatePassed)
					allBossPassed = allBossPassed && passed;

				if (allCollected && allBossPassed && state.camControl.shootTime >= 29.8f)
				{
					if (std::fabs(rocketWorldPos.x - 71.f) < 1.8f && std::fabs(rocketWorldPos.y + 0.97f) < 1.2f && std::fabs(rocketWorldPos.z + 1.f) < 1.5f)
					{
						state.missionComplete = true;
								state.phase = GamePhase::LEVEL_CLEAR;
								state.levelTransitionTimer = 0.f;
								state.successfulMissions += 1;
								int timeBonus = static_cast<int>(state.missionTimer * 10.f);
								int fuelBonus = static_cast<int>(state.fuel * 5.f);
								state.score += 500 + timeBonus + fuelBonus;
								achievements.unlock(AchievementId::FIRST_LANDING);
					}
				}
			}
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
		
			if (state.camControl.radius <= 0.1f) {
				state.camControl.radius = 0.1f;
			}

			if (state.followCamera && state.camControl.shoot)
			{
				float followAlpha = state.followCamSmoothing * dt;
				if (followAlpha > 1.f) followAlpha = 1.f;
				float recenterAlpha = state.followCamRecentering * dt;
				if (recenterAlpha > 1.f) recenterAlpha = 1.f;

				float targetPhi = state.camControl.phi;
				float targetTheta = state.camControl.theta * 0.6f;
				state.camControl.followPhi += (targetPhi - state.camControl.followPhi) * followAlpha;
				state.camControl.followTheta += (targetTheta - state.camControl.followTheta) * followAlpha;
				state.camControl.followPhi += (0.f - state.camControl.followPhi) * recenterAlpha;
				state.camControl.followTheta += (0.f - state.camControl.followTheta) * recenterAlpha;

				Rx = make_rotation_x(state.camControl.followTheta);
				Ry = make_rotation_y(state.camControl.followPhi);
				T = make_translation({ -(remote_x - 7.f), -(remote_y + 1.2f), -(remote_z + 10.f) });
			}
			else
			{
				state.camControl.followPhi = state.camControl.phi;
				state.camControl.followTheta = state.camControl.theta * 0.6f;
				Rx = make_rotation_x(state.camControl.theta);
				Ry = make_rotation_y(state.camControl.phi);
				T = make_translation({ -state.camControl.Pos_x, -state.camControl.Pos_y, -state.camControl.radius });
			}
		}
		//split model 1
		else if (state.camControl.state_Left == 1) {
			Rx = make_rotation_x(0.f);
			Ry = make_rotation_y(0.f);
			T = make_translation({ -remote_x, -remote_y + 0.95f, -5.f });
		}
		// alternate camera preset
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
		
		// Timing start: environment pass
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

		// Timing end: environment pass
		glQueryCounter(queryID[queryIndex + 1], GL_TIMESTAMP);

		// Timing start: landing pad pass
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

		for (std::size_t i = 0; i < kMissionTargets.size(); ++i)
		{
			if (state.targetCollected[i])
				continue;
			float bob = 0.35f * std::sin(angle * 2.5f + static_cast<float>(i));
			Mat44f beaconModel = make_translation({ kMissionTargets[i].x, kMissionTargets[i].y + bob, kMissionTargets[i].z }) *
				make_rotation_y(state.gameplayTime * 2.4f + static_cast<float>(i));
			glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * beaconModel).v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, beaconModel.v);
			glBindVertexArray(missionBeaconVao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(missionBeaconVertices));
		}

		for (std::size_t i = 0; i < obstaclePositions.size(); ++i)
		{
			float pulse = 0.5f + 0.5f * std::sin(state.gameplayTime * (2.2f + level.obstacleMotionScale) + static_cast<float>(i));
			Mat44f obstacleModel = make_translation(obstaclePositions[i]) *
				make_rotation_y(state.gameplayTime * (3.0f + level.obstacleMotionScale)) *
				make_scaling(0.55f + 0.18f * pulse, 0.55f + 0.18f * pulse, 0.55f + 0.18f * pulse);
			glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * obstacleModel).v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, obstacleModel.v);
			glBindVertexArray(obstacleVao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(obstacleVertices));
		}

		for (std::size_t i = 0; i < kBossGateCenters.size(); ++i)
		{
			if (state.bossGatePassed[i])
				continue;
			float glow = 0.5f + 0.5f * std::sin(state.gameplayTime * (level.hard ? 6.f : 3.f) + static_cast<float>(i) * 1.7f);
			Mat44f bossModel = make_translation(kBossGateCenters[i]) *
				make_rotation_z(state.gameplayTime * (0.3f + 0.2f * static_cast<float>(i + 1))) *
				make_scaling(0.8f + 0.25f * glow, 0.22f, 0.8f + 0.25f * glow);
			glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * bossModel).v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, bossModel.v);
			glBindVertexArray(bossVao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(bossVertices));
		}

		for (auto const& canister : fuelPickups.getCanisters())
		{
			if (canister.collected)
				continue;
			float bob = 0.25f * std::sin(state.gameplayTime * 2.0f);
			Mat44f canisterModel = make_translation({ canister.position.x, canister.position.y + bob, canister.position.z }) *
				make_rotation_y(canister.rotation);
			glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * canisterModel).v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, canisterModel.v);
			glBindVertexArray(fuelCanisterVao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(fuelCanisterVertices));
		}

		glBindVertexArray(0);
		glUseProgram(0);

		// Timing end: landing pad pass
		glQueryCounter(queryID[queryIndex + 2], GL_TIMESTAMP);

		// Timing start: rocket pass
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

		// Timing end: rocket pass
		glQueryCounter(queryID[queryIndex + 3], GL_TIMESTAMP);

		// Frame timing end
		glQueryCounter(queryID[queryIndex], GL_TIMESTAMP);

		// Speed lines overlay during high-speed flight
		if (state.camControl.shoot && state.camControl.shootTime > 2.f)
		{
			float speed = state.camControl.shootTime * state.launchSpeedScale;
			float intensity = std::min(speed / 20.f, 1.0f) * 0.35f;
			if (intensity > 0.05f)
			{
				std::vector<Vertex2D> speedLines;
				int lineCount = static_cast<int>(intensity * 16.f);
				for (int s = 0; s < lineCount; ++s)
				{
					float frac = static_cast<float>(s) / static_cast<float>(lineCount);
					float yp = -0.9f + frac * 1.8f;
					float lineW = 0.005f + intensity * 0.008f;
					float xOff = (frac * 7.13f) - static_cast<int>(frac * 7.13f); // pseudo-random
					xOff = xOff * 0.6f + 0.35f;
					push_rect(speedLines, xOff, yp, lineW, 0.45f * intensity, 1.f, 1.f, 1.f);
					push_rect(speedLines, -xOff - lineW, yp - 0.1f, lineW, 0.35f * intensity, 0.9f, 0.95f, 1.f);
				}
				draw_vertices(prog_ui, textBatch, speedLines);
			}
		}

		// Draw button
		glUseProgram(prog_ui.programId());
		static float const baseColor[] = { 0.2f, 1.f, 1.f };
		glUniform3fv(0, 1, baseColor);

		// Draw first rectangle
		glBindVertexArray(vao1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Use GL_TRIANGLE_FAN for a rectangle

		// // Draw second rectangle
		glBindVertexArray(vao2);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindVertexArray(0);
		glUseProgram(0);

		draw_text_centered_in_ndc_rect(prog_ui, textBatch, nwidth, nheight, x1, y11, width1, height1, 18.f, 0.08f, 0.08f, 0.08f, "LAUNCH");
		draw_text_centered_in_ndc_rect(prog_ui, textBatch, nwidth, nheight, x2, y2, width1, height1, 18.f, 0.08f, 0.08f, 0.08f, "RESET");

		{
			int collectedCount = 0;
			for (bool got : state.targetCollected) if (got) ++collectedCount;
			int bossPassedCount = 0;
			for (bool p : state.bossGatePassed) if (p) ++bossPassedCount;
			char const* followPresetName = (state.followCamPreset >= 0 && state.followCamPreset < static_cast<int>(kFollowCamPresets.size()))
				? kFollowCamPresets[state.followCamPreset].name : "CUSTOM";

			hud_renderer::HudInput hi{};
			hi.testMode = state.camControl.testMode;
			hi.levelName = kLevels[state.currentLevel].name;
			hi.score = state.score;
			hi.successfulMissions = state.successfulMissions;
			hi.launchCount = state.launchCount;
			hi.collectedCount = collectedCount;
			hi.bossPassedCount = bossPassedCount;
			hi.missionTimer = state.missionTimer;
			hi.launchSpeedScale = state.launchSpeedScale;
			hi.followCamera = state.followCamera;
			hi.fuel = state.fuel;
			hi.maxFuel = kMaxFuel_;
			hi.camPresetName = followPresetName;
			hi.followCamSmoothing = state.followCamSmoothing;
			hi.followCamRecentering = state.followCamRecentering;
			hi.comboCount = comboTracker.getComboCount();
			hi.fuelRemaining = fuelPickups.getRemainingCount();
			hi.fuelTotal = fuelPickups.getTotalCount();
			hi.windOffset = windSystem.getCurrentOffset();
			hi.achieveUnlocked = achievements.getUnlockedCount();
			hi.achieveTotal = achievements.getTotalCount();
			hi.gameplayTime = state.gameplayTime;
			hi.inFlight = state.camControl.shoot;
			hi.currentLevel = state.currentLevel;
			hi.totalLevels = static_cast<int>(kLevels.size());
			hi.campaignCleared = state.campaignCleared;
			hi.missionComplete = state.missionComplete;
			hi.missionFailed = state.missionFailed;

			auto hud = hud_renderer::compute_hud(hi);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 20.f, 18.f, 0.94f, 0.94f, 0.94f, hud.line1);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 44.f, 18.f, 0.94f, 0.94f, 0.94f, hud.line2);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 68.f, 18.f, 0.94f, 0.94f, 0.94f, hud.line3);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 92.f, 18.f, 0.84f, 0.93f, 1.0f, hud.line4);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 116.f, 17.f, 0.84f, 0.93f, 1.0f, hud.line5);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 140.f, 17.f, 0.30f, 0.95f, 0.45f, hud.line6);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 164.f, 17.f, 0.65f, 0.80f, 1.0f, hud.line7);
			draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 188.f, 17.f, 0.75f, 0.75f, 0.75f, hud.line8);

			// Fuel bar (top-right)
			float fuelR = 1.0f - hud.fuelFraction;
			float fuelG = hud.fuelFraction;
			std::vector<Vertex2D> fuelBar;
			push_rect(fuelBar, 0.68f, 0.96f, 0.28f, 0.025f, 0.15f, 0.15f, 0.15f);
			push_rect(fuelBar, 0.68f, 0.96f, 0.28f * hud.fuelFraction, 0.025f, fuelR, fuelG, 0.1f);
			draw_vertices(prog_ui, textBatch, fuelBar);
			draw_text(prog_ui, textBatch, nwidth, nheight, float(nwidth) - 210.f, 12.f, 14.f, 0.94f, 0.94f, 0.94f, "FUEL");

			if (state.campaignCleared)
			{
				draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 218.f, 18.f, 0.75f, 1.0f, 0.75f, "CAMPAIGN CLEARED - PRESS R TO RESTART");
				draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 244.f, 17.f, 0.85f, 0.95f, 0.85f, hud.statusLine);
			}
			else if (state.missionComplete)
				draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 218.f, 18.f, 0.75f, 1.0f, 0.75f, "MISSION COMPLETE - AUTO NEXT LEVEL");
			else if (state.missionFailed)
				draw_text(prog_ui, textBatch, nwidth, nheight, 20.f, 218.f, 18.f, 1.0f, 0.70f, 0.70f, "MISSION FAILED - PRESS R TO RETRY");

			// Level progress bar (bottom center)
			std::vector<Vertex2D> progBar;
			push_rect(progBar, -0.25f, -0.72f, 0.50f, 0.018f, 0.12f, 0.12f, 0.18f);
			push_rect(progBar, -0.25f, -0.72f, 0.50f * hud.levelProgress, 0.018f, 0.25f, 0.75f, 0.95f);
			draw_vertices(prog_ui, textBatch, progBar);
		}

// Poll results without stalling the CPU/GPU pipeline
GLuint64 timestamp[4]{};
GLuint available = 0;
		glGetQueryObjectuiv(queryID[queryIndex], GL_QUERY_RESULT_AVAILABLE, &available);
		if (available)
		{
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

			float time[4];
			for (int i = 0; i < 4; i++) {
				time[i] = static_cast<float>(elapsedTime[i]) / 1000000.0f;
			}

			// Print the elapsed time between each section
			if (state.camControl.testMode)
			{
				std::printf("============== Frame Timing ==============\n\n");
				std::printf("Frame total: %.2f ms\n", time[3]);
			 std::printf("Environment pass: %.2f ms\n", time[0]);
			 std::printf("Landing pad pass: %.2f ms\n", time[1]);
			 std::printf("Rocket pass: %.2f ms\n", time[2]);
			 std::printf("\n==========================================\n\n");
			}
		}

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

			for (std::size_t i = 0; i < kMissionTargets.size(); ++i)
			{
				if (state.targetCollected[i])
					continue;
				float bob = 0.35f * std::sin(angle * 2.5f + static_cast<float>(i));
				Mat44f beaconModel = make_translation({ kMissionTargets[i].x, kMissionTargets[i].y + bob, kMissionTargets[i].z }) *
					make_rotation_y(state.gameplayTime * 2.4f + static_cast<float>(i));
				glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * beaconModel).v);
				glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
				glUniformMatrix4fv(11, 1, GL_TRUE, beaconModel.v);
				glBindVertexArray(missionBeaconVao);
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(missionBeaconVertices));
			}

			for (std::size_t i = 0; i < obstaclePositions.size(); ++i)
			{
				float pulse = 0.5f + 0.5f * std::sin(state.gameplayTime * (2.2f + level.obstacleMotionScale) + static_cast<float>(i));
				Mat44f obstacleModel = make_translation(obstaclePositions[i]) *
					make_rotation_y(state.gameplayTime * (3.0f + level.obstacleMotionScale)) *
					make_scaling(0.55f + 0.18f * pulse, 0.55f + 0.18f * pulse, 0.55f + 0.18f * pulse);
				glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * obstacleModel).v);
				glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
				glUniformMatrix4fv(11, 1, GL_TRUE, obstacleModel.v);
				glBindVertexArray(obstacleVao);
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(obstacleVertices));
			}

			for (std::size_t i = 0; i < kBossGateCenters.size(); ++i)
			{
				if (state.bossGatePassed[i])
					continue;
				float glow = 0.5f + 0.5f * std::sin(state.gameplayTime * (level.hard ? 6.f : 3.f) + static_cast<float>(i) * 1.7f);
				Mat44f bossModel = make_translation(kBossGateCenters[i]) *
					make_rotation_z(state.gameplayTime * (0.3f + 0.2f * static_cast<float>(i + 1))) *
					make_scaling(0.8f + 0.25f * glow, 0.22f, 0.8f + 0.25f * glow);
				glUniformMatrix4fv(0, 1, GL_TRUE, (projCameraWorld * bossModel).v);
				glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
				glUniformMatrix4fv(11, 1, GL_TRUE, bossModel.v);
				glBindVertexArray(bossVao);
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(bossVertices));
			}

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

	// Cleanup GPU timing queries
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
				auto activate_mode = [&](bool testMode) {
					state->camControl.testMode = testMode;
					state->camControl.showStartupPage = false;
					state->camControl.shoot = false;
					state->camControl.shootTime = 0.f;
					state->camControl.flightOffsetY = 0.f;
					state->camControl.flightOffsetZ = 0.f;
					state->camControl.followPhi = 0.f;
					state->camControl.followTheta = 0.f;
					state->currentLevel = 0;
					state->campaignCleared = false;
					state->followCamPreset = 1;
					state->followCamSmoothing = kFollowCamPresets[state->followCamPreset].smoothing;
					state->followCamRecentering = kFollowCamPresets[state->followCamPreset].recenter;
					state->missionTimer = kLevels[state->currentLevel].missionTime;
					state->missionComplete = false;
					state->missionFailed = false;
					state->targetCollected = { false, false, false };
					state->bossGatePassed = { false, false, false };
				};
				if (GLFW_KEY_ENTER == aKey && GLFW_PRESS == aAction)
					activate_mode(false);
				else if (GLFW_KEY_G == aKey && GLFW_PRESS == aAction)
					activate_mode(false);
				else if (GLFW_KEY_T == aKey && GLFW_PRESS == aAction)
					activate_mode(true);
				return;
			}

			if (aAction == GLFW_PRESS && GLFW_KEY_LEFT_BRACKET == aKey)
			{
				state->launchSpeedScale -= 0.10f;
				if (state->launchSpeedScale < kLaunchSpeedMin_)
					state->launchSpeedScale = kLaunchSpeedMin_;
			}
			else if (aAction == GLFW_PRESS && GLFW_KEY_RIGHT_BRACKET == aKey)
			{
				state->launchSpeedScale += 0.10f;
				if (state->launchSpeedScale > kLaunchSpeedMax_)
					state->launchSpeedScale = kLaunchSpeedMax_;
			}
			else if (aAction == GLFW_PRESS && GLFW_KEY_COMMA == aKey)
			{
				state->followCamSmoothing -= 0.5f;
				if (state->followCamSmoothing < 2.0f)
					state->followCamSmoothing = 2.0f;
				state->followCamPreset = -1;
			}
			else if (aAction == GLFW_PRESS && GLFW_KEY_PERIOD == aKey)
			{
				state->followCamSmoothing += 0.5f;
				if (state->followCamSmoothing > 20.0f)
					state->followCamSmoothing = 20.0f;
				state->followCamPreset = -1;
			}
			else if (aAction == GLFW_PRESS && GLFW_KEY_SEMICOLON == aKey)
			{
				state->followCamRecentering -= 0.2f;
				if (state->followCamRecentering < 0.0f)
					state->followCamRecentering = 0.0f;
				state->followCamPreset = -1;
			}
			else if (aAction == GLFW_PRESS && GLFW_KEY_APOSTROPHE == aKey)
			{
				state->followCamRecentering += 0.2f;
				if (state->followCamRecentering > 6.0f)
					state->followCamRecentering = 6.0f;
				state->followCamPreset = -1;
			}
			else if (aAction == GLFW_PRESS && GLFW_KEY_P == aKey)
			{
				if (state->followCamPreset < 0)
					state->followCamPreset = 0;
				else
					state->followCamPreset = (state->followCamPreset + 1) % static_cast<int>(kFollowCamPresets.size());
				state->followCamSmoothing = kFollowCamPresets[state->followCamPreset].smoothing;
				state->followCamRecentering = kFollowCamPresets[state->followCamPreset].recenter;
			}
			else if (aAction == GLFW_PRESS && GLFW_KEY_B == aKey)
			{
				state->followCamera = !state->followCamera;
			}

			if (GLFW_KEY_LEFT == aKey)
				state->camControl.steerLeft = (aAction != GLFW_RELEASE);
			else if (GLFW_KEY_RIGHT == aKey)
				state->camControl.steerRight = (aAction != GLFW_RELEASE);
			else if (GLFW_KEY_UP == aKey)
				state->camControl.steerUp = (aAction != GLFW_RELEASE);
			else if (GLFW_KEY_DOWN == aKey)
				state->camControl.steerDown = (aAction != GLFW_RELEASE);

			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;
				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			if (!state->camControl.cameraActive)
				return;

			if (GLFW_KEY_W == aKey)
				state->camControl.zoomIn = (GLFW_PRESS == aAction) || (GLFW_REPEAT == aAction);
			else if (GLFW_KEY_E == aKey)
				state->camControl.moveDown = (GLFW_PRESS == aAction) || (GLFW_REPEAT == aAction);
			else if (GLFW_KEY_A == aKey)
				state->camControl.moveLeft = (GLFW_PRESS == aAction) || (GLFW_REPEAT == aAction);
			else if (GLFW_KEY_S == aKey)
				state->camControl.zoomOut = (GLFW_PRESS == aAction) || (GLFW_REPEAT == aAction);
			else if (GLFW_KEY_D == aKey)
				state->camControl.moveRight = (GLFW_PRESS == aAction) || (GLFW_REPEAT == aAction);
			else if (GLFW_KEY_Q == aKey)
				state->camControl.moveUp = (GLFW_PRESS == aAction) || (GLFW_REPEAT == aAction);
			else if (GLFW_KEY_C == aKey && GLFW_RELEASE == aAction)
			{
				state->camControl.state_Left = (state->camControl.state_Left + 1) % 3;
				if (state->camControl.start_split && state->camControl.split_shift)
					state->camControl.state_Right = (state->camControl.state_Right + 1) % 3;
			}
			else if (GLFW_KEY_LEFT_SHIFT == aKey)
			{
				if (GLFW_PRESS == aAction)
					state->camControl.speedUp = true;
				else if (GLFW_RELEASE == aAction)
					state->camControl.speedUp = false;
			}
			else if (GLFW_KEY_LEFT_CONTROL == aKey)
			{
				if (GLFW_PRESS == aAction)
				state->camControl.slowDown = true;
				else if (GLFW_RELEASE == aAction)
				state->camControl.slowDown = false;
			}
			else if (GLFW_KEY_F == aKey && GLFW_PRESS == aAction)
			{
				if (!state->camControl.shoot)
						state->launchCount += 1;
					if (state->camControl.shootTime <= 0.f)
					{
						state->targetCollected = { false, false, false };
						state->bossGatePassed = { false, false, false };
						state->missionTimer = kLevels[state->currentLevel].missionTime;
					}
					state->camControl.flightOffsetY = 0.f;
					state->camControl.flightOffsetZ = 0.f;
					state->camControl.followPhi = 0.f;
					state->camControl.followTheta = 0.f;
						state->camControl.shoot = true;
						state->fuel = kMaxFuel_;
						state->phase = GamePhase::FLYING;
						state->missionComplete = false;
						state->missionFailed = false;
					}
			else if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.shoot = false;
				state->camControl.shootTime = 0.f;
				state->camControl.flightOffsetY = 0.f;
				state->camControl.flightOffsetZ = 0.f;
				state->camControl.followPhi = 0.f;
				state->camControl.followTheta = 0.f;
				state->campaignCleared = false;
				state->missionTimer = kLevels[state->currentLevel].missionTime;
				state->missionComplete = false;
				state->missionFailed = false;
				state->targetCollected = { false, false, false };
				state->bossGatePassed = { false, false, false };
			}
			else if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.start_split = !state->camControl.start_split;
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive || (state->followCamera && state->camControl.shoot))
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

	// Mouse controls for startup and in-game action buttons
	void mouseButtonCallback(GLFWwindow* aWindow, int button, int action, int mods)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			double mouseX, mouseY;
			glfwGetCursorPos(aWindow, &mouseX, &mouseY);

			float normalizedX = (2.0f * static_cast<float>(mouseX)) / state->windowWidth - 1.0f;
			float normalizedY = 1.0f - (2.0f * static_cast<float>(mouseY)) / state->windowHeight;

			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				if (state->camControl.showStartupPage)
				{
					auto activate_mode = [&](bool testMode) {
						state->camControl.testMode = testMode;
						state->camControl.showStartupPage = false;
						state->camControl.shoot = false;
						state->camControl.shootTime = 0.f;
						state->camControl.flightOffsetY = 0.f;
						state->camControl.flightOffsetZ = 0.f;
						state->camControl.followPhi = 0.f;
						state->camControl.followTheta = 0.f;
						state->currentLevel = 0;
						state->campaignCleared = false;
						state->followCamPreset = 1;
						state->followCamSmoothing = kFollowCamPresets[state->followCamPreset].smoothing;
						state->followCamRecentering = kFollowCamPresets[state->followCamPreset].recenter;
						state->missionTimer = kLevels[state->currentLevel].missionTime;
						state->missionComplete = false;
						state->missionFailed = false;
						state->targetCollected = { false, false, false };
						state->bossGatePassed = { false, false, false };
					};
					if (normalizedX >= kStartupGameButtonX && normalizedX <= (kStartupGameButtonX + kStartupButtonWidth)
						&& normalizedY <= kStartupGameButtonY && normalizedY >= (kStartupGameButtonY - kStartupButtonHeight))
					{
						activate_mode(false);
						std::printf("Mode selected: GAME\n");
					}
					else if (normalizedX >= kStartupTestButtonX && normalizedX <= (kStartupTestButtonX + kStartupButtonWidth)
						&& normalizedY <= kStartupTestButtonY && normalizedY >= (kStartupTestButtonY - kStartupButtonHeight))
					{
						activate_mode(true);
						std::printf("Mode selected: TEST\n");
					}
					return;
				}

				mods = 1;
				if (normalizedX >= x1 && normalizedX <= (x1 + width1) && normalizedY <= y11 && normalizedY >= (y11 - height1))
				{
					if (!state->camControl.shoot)
						state->launchCount += 1;
					if (state->camControl.shootTime <= 0.f)
					{
						state->targetCollected = { false, false, false };
						state->bossGatePassed = { false, false, false };
						state->missionTimer = kLevels[state->currentLevel].missionTime;
					}
					state->camControl.flightOffsetY = 0.f;
					state->camControl.flightOffsetZ = 0.f;
					state->camControl.followPhi = 0.f;
					state->camControl.followTheta = 0.f;
					state->camControl.shoot = true;
					state->missionComplete = false;
					state->missionFailed = false;
				}
				else if (normalizedX >= x2 && normalizedX <= (x2 + width1) && normalizedY <= y2 && normalizedY >= (y2 - height1))
				{
					mods = 2;
					state->camControl.shoot = false;
					state->camControl.shootTime = 0.0f;
					state->camControl.flightOffsetY = 0.f;
					state->camControl.flightOffsetZ = 0.f;
					state->camControl.followPhi = 0.f;
					state->camControl.followTheta = 0.f;
					state->campaignCleared = false;
					state->missionTimer = kLevels[state->currentLevel].missionTime;
					state->missionComplete = false;
					state->missionFailed = false;
					state->targetCollected = { false, false, false };
					state->bossGatePassed = { false, false, false };
				}
			}
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

