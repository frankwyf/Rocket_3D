#include "defaults.hpp"
#include "loadobj.hpp"
#include "load_texture.hpp"
#include "simple_mesh.hpp"
#include "rocket.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

#include <glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cmath>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/debug_output.hpp"
#include "defaults.hpp"

namespace
{
	constexpr char const* kWindowTitle = "SkyRocket 3D - Arcade Mission";

	enum class GameMode
	{
		Campaign,
		TimeAttack,
		Collector
	};

	struct Vertex2D
	{
		float x, y;
		float r, g, b;
	};

	struct UiBatch
	{
		GLuint vao = 0;
		GLuint vbo = 0;
	};

	struct State
	{
		ShaderProgram* uiProgram = nullptr;
		ShaderProgram* threeDProgram = nullptr;
		int windowWidth = 1280;
		int windowHeight = 720;

		bool menuActive = true;
		bool showHelp = false;
		GameMode mode = GameMode::Campaign;

		int round = 1;
		int maxRounds = 3;
		int score = 0;
		float roundTime = 0.f;
		float roundTimeLimit = 35.f;
		bool roundComplete = false;

		bool launched = false;
		float launchT = 0.f;
		float rocketX = -0.75f;
		float rocketY = -0.55f;

		bool usedSplit = false;
		bool switchedCamera = false;
		bool splitEnabled = false;
		bool cameraAlt = false;

		std::array<bool, 3> bonusCollected{ false, false, false };

		bool prevLeftMouse = false;
		std::array<bool, 512> prevKeys{};
	};

	char const* mode_name(GameMode mode)
	{
		switch (mode)
		{
		case GameMode::TimeAttack: return "TIME ATTACK";
		case GameMode::Collector: return "COLLECTOR";
		default: return "CAMPAIGN";
		}
	}

	void apply_mode(State& s)
	{
		switch (s.mode)
		{
		case GameMode::TimeAttack:
			s.maxRounds = 5;
			s.roundTimeLimit = 20.f;
			break;
		case GameMode::Collector:
			s.maxRounds = 3;
			s.roundTimeLimit = 30.f;
			break;
		case GameMode::Campaign:
		default:
			s.maxRounds = 3;
			s.roundTimeLimit = 35.f;
			break;
		}
		s.roundTime = s.roundTimeLimit;
	}

	void reset_round(State& s)
	{
		s.launched = false;
		s.launchT = 0.f;
		s.rocketX = -0.75f;
		s.rocketY = -0.55f;
		s.usedSplit = false;
		s.switchedCamera = false;
		s.splitEnabled = false;
		s.cameraAlt = false;
		s.bonusCollected = { false, false, false };
		s.roundComplete = false;
		apply_mode(s);
	}

	void reset_game(State& s)
	{
		s.round = 1;
		s.score = 0;
		reset_round(s);
	}

	bool key_pressed_once(GLFWwindow* window, State& s, int key)
	{
		bool down = glfwGetKey(window, key) == GLFW_PRESS;
		bool pressed = down && !s.prevKeys[static_cast<std::size_t>(key)];
		s.prevKeys[static_cast<std::size_t>(key)] = down;
		return pressed;
	}

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

	void draw_vertices(State const& s, UiBatch& b, std::vector<Vertex2D> const& verts)
	{
		if (verts.empty())
			return;
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glUseProgram(s.uiProgram->programId());
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

	void push_rect(std::vector<Vertex2D>& v, float x, float y, float w, float h, float r, float g, float b)
	{
		Vertex2D v0{ x, y, r, g, b };
		Vertex2D v1{ x + w, y, r, g, b };
		Vertex2D v2{ x + w, y - h, r, g, b };
		Vertex2D v3{ x, y - h, r, g, b };
		v.push_back(v0); v.push_back(v1); v.push_back(v2);
		v.push_back(v0); v.push_back(v2); v.push_back(v3);
	}

	std::array<unsigned char, 7> glyph_rows(char c)
	{
		auto rows = std::array<unsigned char, 7>{ 0,0,0,0,0,0,0 };
		switch (static_cast<char>(std::toupper(static_cast<unsigned char>(c))))
		{
		case 'A': rows = { 0x0E,0x11,0x11,0x1F,0x11,0x11,0x11 }; break;
		case 'B': rows = { 0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E }; break;
		case 'C': rows = { 0x0E,0x11,0x10,0x10,0x10,0x11,0x0E }; break;
		case 'D': rows = { 0x1E,0x11,0x11,0x11,0x11,0x11,0x1E }; break;
		case 'E': rows = { 0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F }; break;
		case 'F': rows = { 0x1F,0x10,0x10,0x1E,0x10,0x10,0x10 }; break;
		case 'G': rows = { 0x0E,0x11,0x10,0x13,0x11,0x11,0x0E }; break;
		case 'H': rows = { 0x11,0x11,0x11,0x1F,0x11,0x11,0x11 }; break;
		case 'I': rows = { 0x1F,0x04,0x04,0x04,0x04,0x04,0x1F }; break;
		case 'J': rows = { 0x07,0x02,0x02,0x02,0x12,0x12,0x0C }; break;
		case 'K': rows = { 0x11,0x12,0x14,0x18,0x14,0x12,0x11 }; break;
		case 'L': rows = { 0x10,0x10,0x10,0x10,0x10,0x10,0x1F }; break;
		case 'M': rows = { 0x11,0x1B,0x15,0x15,0x11,0x11,0x11 }; break;
		case 'N': rows = { 0x11,0x19,0x15,0x13,0x11,0x11,0x11 }; break;
		case 'O': rows = { 0x0E,0x11,0x11,0x11,0x11,0x11,0x0E }; break;
		case 'P': rows = { 0x1E,0x11,0x11,0x1E,0x10,0x10,0x10 }; break;
		case 'Q': rows = { 0x0E,0x11,0x11,0x11,0x15,0x12,0x0D }; break;
		case 'R': rows = { 0x1E,0x11,0x11,0x1E,0x14,0x12,0x11 }; break;
		case 'S': rows = { 0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E }; break;
		case 'T': rows = { 0x1F,0x04,0x04,0x04,0x04,0x04,0x04 }; break;
		case 'U': rows = { 0x11,0x11,0x11,0x11,0x11,0x11,0x0E }; break;
		case 'V': rows = { 0x11,0x11,0x11,0x11,0x11,0x0A,0x04 }; break;
		case 'W': rows = { 0x11,0x11,0x11,0x15,0x15,0x15,0x0A }; break;
		case 'X': rows = { 0x11,0x11,0x0A,0x04,0x0A,0x11,0x11 }; break;
		case 'Y': rows = { 0x11,0x11,0x0A,0x04,0x04,0x04,0x04 }; break;
		case 'Z': rows = { 0x1F,0x01,0x02,0x04,0x08,0x10,0x1F }; break;
		case '0': rows = { 0x0E,0x11,0x13,0x15,0x19,0x11,0x0E }; break;
		case '1': rows = { 0x04,0x0C,0x04,0x04,0x04,0x04,0x0E }; break;
		case '2': rows = { 0x0E,0x11,0x01,0x02,0x04,0x08,0x1F }; break;
		case '3': rows = { 0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E }; break;
		case '4': rows = { 0x02,0x06,0x0A,0x12,0x1F,0x02,0x02 }; break;
		case '5': rows = { 0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E }; break;
		case '6': rows = { 0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E }; break;
		case '7': rows = { 0x1F,0x01,0x02,0x04,0x08,0x08,0x08 }; break;
		case '8': rows = { 0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E }; break;
		case '9': rows = { 0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E }; break;
		case ' ': rows = { 0,0,0,0,0,0,0 }; break;
		case '-': rows = { 0,0,0,0x1F,0,0,0 }; break;
		case ':': rows = { 0,0x04,0x04,0,0x04,0x04,0 }; break;
		case '.': rows = { 0,0,0,0,0,0x06,0x06 }; break;
		case ',': rows = { 0,0,0,0,0x06,0x06,0x02 }; break;
		case '+': rows = { 0,0x04,0x04,0x1F,0x04,0x04,0 }; break;
		case '/': rows = { 0x01,0x02,0x04,0x08,0x10,0,0 }; break;
		case '>': rows = { 0x10,0x08,0x04,0x02,0x04,0x08,0x10 }; break;
		case '<': rows = { 0x02,0x04,0x08,0x10,0x08,0x04,0x02 }; break;
		default: rows = { 0x1F,0x11,0x02,0x04,0x08,0x11,0x1F }; break;
		}
		return rows;
	}

	void draw_text(State const& s, UiBatch& batch, float xPx, float yPx, float sizePx, float r, float g, float b, char const* text)
	{
		if (!text || s.windowWidth <= 0 || s.windowHeight <= 0)
			return;
		float pixel = sizePx / 7.f;
		float advance = pixel * 6.f;
		std::vector<Vertex2D> verts;
		verts.reserve(std::strlen(text) * 5 * 7 * 6);

		auto to_ndc_x = [&](float px) { return (px / float(s.windowWidth)) * 2.f - 1.f; };
		auto to_ndc_y = [&](float py) { return 1.f - (py / float(s.windowHeight)) * 2.f; };

		for (std::size_t i = 0; text[i] != '\0'; ++i)
		{
			auto rows = glyph_rows(text[i]);
			float gx = xPx + static_cast<float>(i) * advance;
			for (int row = 0; row < 7; ++row)
			{
				for (int col = 0; col < 5; ++col)
				{
					if ((rows[row] & (1u << (4 - col))) == 0)
						continue;
					float x0 = to_ndc_x(gx + col * pixel);
					float y0 = to_ndc_y(yPx + row * pixel);
					float x1 = to_ndc_x(gx + (col + 1) * pixel);
					float y1 = to_ndc_y(yPx + (row + 1) * pixel);
					push_rect(verts, x0, y0, x1 - x0, y0 - y1, r, g, b);
				}
			}
		}
		draw_vertices(s, batch, verts);
	}

	bool inside_rect(float nx, float ny, float x, float y, float w, float h)
	{
		return nx >= x && nx <= (x + w) && ny <= y && ny >= (y - h);
	}
}

int main(int argc, char** argv) try
{
	bool profileMode = false;
	for (int i = 1; i < argc; ++i)
	{
		if (0 == std::strcmp(argv[i], "--profile") || 0 == std::strcmp(argv[i], "--mode=profile"))
			profileMode = true;
	}
	std::printf("Running in %s mode.\n", profileMode ? "PROFILE" : "PLAY");

	if (GLFW_TRUE != glfwInit())
		throw Error("glfwInit() failed");

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	GLFWwindow* window = glfwCreateWindow(1280, 720, kWindowTitle, nullptr, nullptr);
	if (!window)
		throw Error("glfwCreateWindow() failed");
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
		throw Error("gladLoadGLLoader() failed");

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	ShaderProgram prog_ui({
		{GL_VERTEX_SHADER, "assets/button.vert"},
		{GL_FRAGMENT_SHADER, "assets/button.frag"}
		});

	ShaderProgram prog_default({
		{GL_VERTEX_SHADER, "assets/default.vert"},
		{GL_FRAGMENT_SHADER, "assets/default.frag"}
		});

	ShaderProgram prog_3d({
		{GL_VERTEX_SHADER, "assets/landingpad.vert"},
		{GL_FRAGMENT_SHADER, "assets/landingpad.frag"}
		});

	std::unordered_map<std::string, Material> parlahti_materials = load_mtl("assets/parlahti.obj");
	SimpleMeshData parlahti_obj = load_obj_file("assets/parlahti.obj");
	GLuint parlahti_vao = bind_vao(parlahti_obj);
	std::size_t parlahti_vertex = parlahti_obj.vertex_positions.size();
	GLuint parlahti_tex = parlahti_materials["Material.001"].texture;

	SimpleMeshData launchpad_obj = load_obj_file("assets/landingpad.obj");
	GLuint padVao = bind_vao(launchpad_obj);
	std::size_t padVertexCount = launchpad_obj.vertex_positions.size();

	auto cyl = make_cylinder(true, 8, { 1.f, 0.4f, 0.f },
		make_rotation_z(3.141592f / 2.f) * make_scaling(0.3f, 0.3f, 0.3f));
	auto cy2 = make_cylinder(true, 6, { 0.0f, 0.3f, 0.6f },
		make_rotation_z(3.141592f / 2.f) * make_scaling(0.6f, 0.2f, 0.2f) * make_translation({ 0.5f, 0.f, 0.f }));
	auto cy3 = make_cylinder(true, 10, { 0.0f, 0.6f, 0.3f },
		make_rotation_z(3.141592f / 2.f) * make_scaling(0.5f, 0.15f, 0.15f) * make_translation({ 1.8f, 0.f, 0.f }));
	auto cy4 = make_cylinder(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) * make_scaling(0.6f, 0.1f, 0.1f) * make_translation({ 2.3f, 0.f, 0.f }));
	auto cy5 = make_cylinder(true, 4, { 0.3f, 0.8f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 1.f) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.1f, 0.1f, 0.1f) * make_translation({ 5.f, 2.8f, 0.f }));
	auto cy6 = make_cylinder(true, 4, { 0.3f, 0.4f, 0.2f },
		make_rotation_y(3.141592f / 6.f * 3.f) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.1f, 0.1f, 0.1f) * make_translation({ 5.f, 2.8f, 0.f }));
	auto cy7 = make_cylinder(true, 4, { 0.3f, 0.8f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 5.f) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.1f, 0.1f, 0.1f) * make_translation({ 5.f, 2.8f, 0.f }));
	auto cy8 = make_cylinder(true, 4, { 0.3f, 0.2f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 7.f) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.1f, 0.1f, 0.1f) * make_translation({ 5.f, 2.8f, 0.f }));
	auto cy9 = make_cylinder(true, 4, { 0.3f, 0.8f, 0.6f },
		make_rotation_y(3.141592f / 6.f * 9.f) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.1f, 0.1f, 0.1f) * make_translation({ 5.f, 2.8f, 0.f }));
	auto cy10 = make_cylinder(true, 4, { 0.3f, 0.4f, 0.2f },
		make_rotation_y(3.141592f / 6.f * 11.f) * make_rotation_z(3.141592f / 2.f) * make_scaling(0.1f, 0.1f, 0.1f) * make_translation({ 5.f, 2.8f, 0.f }));
	auto cy11 = make_cone(true, 8, { 1.f, 0.f, 0.6f },
		make_rotation_z(3.141592f / 2.f) * make_scaling(0.3f, 0.1f, 0.1f) * make_translation({ -1.f, 0.f, 0.f }));
	auto ship = concatenate(concatenate(concatenate(concatenate(concatenate(concatenate(concatenate(concatenate(concatenate(concatenate(std::move(cyl), cy2), cy3), cy4), cy5), cy6), cy7), cy8), cy9), cy10), cy11);
	GLuint rocketVao = bind_vao(ship);
	std::size_t rocketVertexCount = ship.vertex_positions.size();

	State s{};
	s.uiProgram = &prog_ui;
	reset_game(s);

	UiBatch uiBatch{};
	init_batch(uiBatch);

	bool lastLeft = false;
	auto last = Clock::now();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		int w = 0, h = 0;
		glfwGetFramebufferSize(window, &w, &h);
		if (w <= 0 || h <= 0)
			continue;
		s.windowWidth = w;
		s.windowHeight = h;

		auto now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;

		if (key_pressed_once(window, s, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		if (key_pressed_once(window, s, GLFW_KEY_H))
			s.showHelp = !s.showHelp;

		double mx = 0.0, my = 0.0;
		glfwGetCursorPos(window, &mx, &my);
		float nx = (2.f * float(mx)) / float(w) - 1.f;
		float ny = 1.f - (2.f * float(my)) / float(h);
		bool leftDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		bool leftPressed = leftDown && !lastLeft;
		lastLeft = leftDown;

		if (s.menuActive)
		{
			if (key_pressed_once(window, s, GLFW_KEY_1)) { s.mode = GameMode::Campaign; reset_game(s); }
			if (key_pressed_once(window, s, GLFW_KEY_2)) { s.mode = GameMode::TimeAttack; reset_game(s); }
			if (key_pressed_once(window, s, GLFW_KEY_3)) { s.mode = GameMode::Collector; reset_game(s); }
			if (key_pressed_once(window, s, GLFW_KEY_ENTER)) { s.menuActive = false; reset_game(s); }

			if (leftPressed)
			{
				if (inside_rect(nx, ny, -0.78f, 0.20f, 0.48f, 0.16f)) { s.mode = GameMode::Campaign; reset_game(s); }
				else if (inside_rect(nx, ny, -0.24f, 0.20f, 0.48f, 0.16f)) { s.mode = GameMode::TimeAttack; reset_game(s); }
				else if (inside_rect(nx, ny, 0.30f, 0.20f, 0.48f, 0.16f)) { s.mode = GameMode::Collector; reset_game(s); }
				else if (inside_rect(nx, ny, -0.22f, -0.28f, 0.44f, 0.20f)) { s.menuActive = false; reset_game(s); }
			}
		}
		else
		{
			if (key_pressed_once(window, s, GLFW_KEY_R))
				reset_game(s);
			if (key_pressed_once(window, s, GLFW_KEY_F) && !s.launched)
			{
				s.launched = true;
				s.launchT = 0.f;
			}
			if (key_pressed_once(window, s, GLFW_KEY_V))
			{
				s.splitEnabled = !s.splitEnabled;
				s.usedSplit = true;
			}
			if (key_pressed_once(window, s, GLFW_KEY_C))
			{
				s.cameraAlt = !s.cameraAlt;
				s.switchedCamera = true;
			}

			if (leftPressed)
			{
				if (inside_rect(nx, ny, -0.5f, -0.85f, 0.2f, 0.15f) && !s.launched)
				{
					s.launched = true;
					s.launchT = 0.f;
				}
				else if (inside_rect(nx, ny, 0.3f, -0.85f, 0.2f, 0.15f))
				{
					reset_game(s);
				}
			}

			if (s.launched)
			{
				s.launchT += dt * 0.35f;
				s.rocketX = -0.75f + 1.60f * s.launchT;
				s.rocketY = -0.55f + std::sin(3.1415926f * s.launchT) * 0.55f;

				std::array<float, 3> bx{ -0.30f, 0.05f, 0.42f };
				std::array<float, 3> by{ -0.05f, 0.24f, -0.02f };
				for (int i = 0; i < 3; ++i)
				{
					if (!s.bonusCollected[i] && std::abs(s.rocketX - bx[i]) < 0.06f && std::abs(s.rocketY - by[i]) < 0.06f)
					{
					 s.bonusCollected[i] = true;
					 s.score += 25;
					}
				}

				s.roundTime -= dt;
				if (s.roundTime <= 0.f)
				{
					s.score = std::max(0, s.score - 30);
					reset_round(s);
				}

				if (s.launchT >= 1.0f)
				{
					int collected = int(s.bonusCollected[0]) + int(s.bonusCollected[1]) + int(s.bonusCollected[2]);
					bool landed = std::abs(s.rocketX - 0.75f) < 0.08f && std::abs(s.rocketY + 0.55f) < 0.08f;
					bool ok = landed;
					if (s.mode == GameMode::Campaign)
					{
						if (s.round >= 2) ok = ok && s.usedSplit;
						if (s.round >= 3) ok = ok && s.switchedCamera;
					}
					else if (s.mode == GameMode::TimeAttack)
						ok = ok && collected >= 1;
					else if (s.mode == GameMode::Collector)
						ok = ok && collected == 3;

					if (ok)
					{
					 s.score += 100 + int(std::max(0.f, s.roundTime) * 5.f);
					 s.round += 1;
					 if (s.round > s.maxRounds)
					 {
						 s.menuActive = true;
						 s.round = 1;
					 }
					}
					else
					{
					 s.score = std::max(0, s.score - 20);
					}
					reset_round(s);
				}
			}
		}

		glViewport(0, 0, w, h);
		if (s.cameraAlt) glClearColor(0.06f, 0.08f, 0.14f, 0.f);
		else glClearColor(0.12f, 0.14f, 0.18f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float camZ = s.cameraAlt ? 55.0f : 65.0f;
		Mat44f proj = make_perspective_projection(60.f * 3.1415926f / 180.f, float(w) / float(h), 0.1f, 200.f);
		Mat44f view = make_translation({ -35.f, -8.f, -camZ }) * make_rotation_x(-0.35f);
		Vec3f eyePos{ 35.f, 8.f, camZ };

		glUseProgram(prog_default.programId());
		{
			Mat44f mapWorld = kIdentity44f;
			Mat44f pcw = proj * view * mapWorld;
			Mat33f normal = mat44_to_mat33(transpose(invert(mapWorld)));
			glUniformMatrix4fv(0, 1, GL_TRUE, pcw.v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normal.v);
			glUniform3f(2, 0.2f, 0.8f, -0.5f);
			glUniform3f(3, 0.9f, 0.9f, 0.9f);
			glUniform3f(4, 0.45f, 0.45f, 0.45f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, parlahti_tex);
			glBindVertexArray(parlahti_vao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(parlahti_vertex));
		}
		glBindVertexArray(0);
		glUseProgram(0);

		glUseProgram(prog_3d.programId());
		glUniform3f(2, 30.f, 22.f, 20.f);
		glUniform3f(3, 0.08f, 0.08f, 0.08f);
		glUniform3f(4, 1.f, 1.f, 1.f);
		glUniform3f(5, eyePos.x, eyePos.y, eyePos.z);
		glUniform1f(6, 40.f);
		glUniform3f(7, -4.f, 4.f, 3.f);
		glUniform3f(8, 0.4f, 0.45f, 0.6f);
		glUniform3f(9, 4.f, 3.f, -2.f);
		glUniform3f(10, 0.35f, 0.35f, 0.45f);

		auto draw3d = [&](Mat44f const& world, GLuint vao, std::size_t count)
		{
			Mat44f pcw = proj * view * world;
			Mat33f normal = mat44_to_mat33(transpose(invert(world)));
			glUniformMatrix4fv(0, 1, GL_TRUE, pcw.v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normal.v);
			glUniformMatrix4fv(11, 1, GL_TRUE, world.v);
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));
		};

		Mat44f startPadWorld = make_translation({ 1.f, -0.95f, -1.0f });
		Mat44f targetPadWorld = make_translation({ 71.f, -0.95f, -1.0f });
		draw3d(startPadWorld, padVao, padVertexCount);
		draw3d(targetPadWorld, padVao, padVertexCount);

		float rocketWorldX = ((s.rocketX + 0.75f) / 1.60f) * 70.f + 1.f;
		float rocketWorldY = (s.rocketY + 0.55f) * 45.f;
		Mat44f rocketWorld = make_translation({ rocketWorldX, rocketWorldY, -1.0f });
		draw3d(rocketWorld, rocketVao, rocketVertexCount);
		glBindVertexArray(0);
		glUseProgram(0);

		std::vector<Vertex2D> ui;
		push_rect(ui, -0.95f, 0.92f, 1.90f, 0.26f, 0.11f, 0.12f, 0.14f);

		if (s.menuActive)
		{
			push_rect(ui, -0.85f, 0.90f, 1.70f, 1.45f, 0.10f, 0.10f, 0.16f);
			push_rect(ui, -0.78f, 0.20f, 0.48f, 0.16f, 0.78f, 0.28f, 0.28f);
			push_rect(ui, -0.24f, 0.20f, 0.48f, 0.16f, 0.24f, 0.62f, 0.26f);
			push_rect(ui, 0.30f, 0.20f, 0.48f, 0.16f, 0.95f, 0.62f, 0.22f);
			push_rect(ui, -0.22f, -0.28f, 0.44f, 0.20f, 0.94f, 0.85f, 0.28f);
		}
		else
		{
			push_rect(ui, -0.80f, -0.62f, 0.12f, 0.12f, 0.20f, 0.60f, 1.0f); // rocket
			push_rect(ui, 0.72f, -0.50f, 0.14f, 0.14f, 0.95f, 0.95f, 0.25f); // target

			std::array<float, 3> bx{ -0.30f, 0.05f, 0.42f };
			std::array<float, 3> by{ -0.05f, 0.24f, -0.02f };
			for (int i = 0; i < 3; ++i)
			{
				float cr = s.bonusCollected[i] ? 0.30f : 0.95f;
				float cg = s.bonusCollected[i] ? 0.80f : 0.75f;
				float cb = s.bonusCollected[i] ? 0.35f : 0.20f;
				push_rect(ui, bx[i], by[i], 0.05f, 0.05f, cr, cg, cb);
			}

			push_rect(ui, -0.50f, -0.85f, 0.20f, 0.15f, 0.85f, 0.15f, 0.15f);
			push_rect(ui, 0.30f, -0.85f, 0.20f, 0.15f, 0.20f, 0.85f, 0.20f);
			push_rect(ui, s.rocketX, s.rocketY, 0.06f, 0.06f, 0.20f, 0.65f, 1.0f);
		}

		draw_vertices(s, uiBatch, ui);

		if (s.menuActive)
		{
			draw_text(s, uiBatch, 50.f, 40.f, 74.f, 1.f, 1.f, 1.f, "SKYROCKET 3D");
			draw_text(s, uiBatch, 50.f, 126.f, 30.f, 0.86f, 0.90f, 1.f, "GAME PLAY BRIEFING");
			draw_text(s, uiBatch, 50.f, 170.f, 26.f, 1.f, 0.82f, 0.82f, "1 CAMPAIGN  - PROGRESSIVE OBJECTIVES");
			draw_text(s, uiBatch, 50.f, 205.f, 26.f, 0.82f, 1.f, 0.82f, "2 TIME ATTACK - 5 ROUNDS, >=1 BONUS");
			draw_text(s, uiBatch, 50.f, 240.f, 26.f, 1.f, 0.92f, 0.76f, "3 COLLECTOR  - GET ALL 3 BONUSES");
			draw_text(s, uiBatch, 50.f, 286.f, 22.f, 0.92f, 0.92f, 0.92f, "CONTROLS WASDQE MOUSE F V C R H");
			draw_text(s, uiBatch, 50.f, 324.f, 26.f, 1.f, 1.f, 0.78f, "CURRENT MODE");
			draw_text(s, uiBatch, 300.f, 324.f, 26.f, 1.f, 1.f, 1.f, mode_name(s.mode));
			draw_text(s, uiBatch, w * 0.42f, h * 0.44f, 42.f, 0.08f, 0.08f, 0.08f, "START");
			draw_text(s, uiBatch, 160.f, h - 80.f, 20.f, 0.90f, 0.90f, 0.90f, "CLICK MODE OR PRESS 1 2 3  THEN START");
		}
		else
		{
			char hud[160] = {};
			std::snprintf(hud, sizeof(hud), "%s   ROUND %d/%d   SCORE %d   TIME %.1f", mode_name(s.mode), s.round, s.maxRounds, s.score, s.roundTime);
			draw_text(s, uiBatch, 16.f, 16.f, 22.f, 1.f, 1.f, 0.85f, hud);

			char const* obj = (s.mode == GameMode::Campaign)
				? ((s.round == 1) ? "OBJECTIVE LAND ON FAR PAD" : (s.round == 2 ? "OBJECTIVE PRESS V THEN LAND" : "OBJECTIVE PRESS C THEN LAND"))
				: (s.mode == GameMode::TimeAttack ? "OBJECTIVE GET >=1 BONUS THEN LAND" : "OBJECTIVE GET ALL 3 BONUSES THEN LAND");
			draw_text(s, uiBatch, 16.f, 46.f, 20.f, 0.85f, 0.95f, 1.f, obj);
			draw_text(s, uiBatch, 18.f, h - 64.f, 24.f, 1.f, 1.f, 1.f, "LAUNCH");
			draw_text(s, uiBatch, w - 220.f, h - 64.f, 24.f, 1.f, 1.f, 1.f, "RESET");
			if (s.showHelp)
				draw_text(s, uiBatch, 16.f, 78.f, 18.f, 1.f, 1.f, 0.8f, "HELP H TOGGLE  F LAUNCH  V SPLIT  C CAMERA");
		}

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
catch (std::exception const& e)
{
	std::fprintf(stderr, "Fatal error: %s\n", e.what());
	return 1;
}
