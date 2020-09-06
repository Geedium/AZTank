#pragma once
#include "System.h"
#include <iostream>
#include <glew.h>
#include <GLFW/glfw3.h>

#define MAX_KEYS	1024
#define MAX_BUTTONS	32

namespace Augiwne { namespace Graphics {
	class Window
	{
	private:
		const char *m_Title;
		int m_Width, m_Height, m_Interval;
		GLFWwindow *m_Window;
		bool m_Closed;
		static bool m_Keys[MAX_KEYS];
		static bool m_KeyState[MAX_KEYS];
		static bool m_KeyTyped[MAX_KEYS];
		static bool m_MouseButtons[MAX_BUTTONS];
		static bool m_MouseState[MAX_BUTTONS];
		static bool m_MouseClicked[MAX_BUTTONS];
		static double mx, my;
	public:
		Window(const char *name, int width, int height, int interval = 1);
		~Window();
		void Clear() const;
		void Update() const;
		bool Closed() const;

		bool IsKeyPressed(unsigned int keycode) const;
		bool IsKeyTyped(unsigned int keycode) const;
		bool IsMouseButtonPressed(unsigned int button) const;
		bool IsMouseButtonClicked(unsigned int button) const;
		void GetMousePosition(double& x, double& y);
	private:
		bool Init();
	public:
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
	private:
		friend static void WindowSize_Callback(GLFWwindow* window, int width, int height);
		friend static void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend static void MouseInput_Callback(GLFWwindow* window, int button, int action, int mods);
		friend static void CursorPosition_Callback(GLFWwindow* window, double xpos, double ypos);
	};
}}