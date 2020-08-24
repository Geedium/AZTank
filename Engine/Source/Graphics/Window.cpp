#include "System.h"
#include "window.h"

#define ENGINE_VERSION "0.5"

namespace Augiwne { namespace Graphics {
	bool Window::m_Keys[MAX_KEYS];
	bool Window::m_KeyState[MAX_KEYS];
	bool Window::m_KeyTyped[MAX_KEYS];
	bool Window::m_MouseButtons[MAX_BUTTONS];
	bool Window::m_MouseState[MAX_BUTTONS];
	bool Window::m_MouseClicked[MAX_BUTTONS];
	double Window::mx;
	double Window::my;

	Window::Window(const char *title, int width, int height, int interval)
	{
		m_Title = title;
		m_Width = width;
		m_Height = height;
		m_Interval = interval;

		if (!Init())
		{
			std::cout << "Failed to init!" << std::endl;
			glfwTerminate();
		}

		for (int i = 0; i < MAX_KEYS; i++)
		{
			m_Keys[i] = false;
			m_KeyState[i] = false;
			m_KeyTyped[i] = false;
		}
		for (int i = 0; i < MAX_BUTTONS; i++)
		{
			m_MouseButtons[i] = false;
			m_MouseState[i] = false;
			m_MouseClicked[i] = false;
		}
	}
	Window::~Window()
	{
		glfwTerminate();
	}
	bool Window::Init()
	{
		if (!glfwInit())
		{
			std::cout << "Error! GLFW" << std::endl;
			return false;
		}
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
		if (!m_Window)
		{
			glfwTerminate();
			std::cout << "Failed to create GLFW Window!";
			return false;
		}
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, WindowSize_Callback);
		glfwSetKeyCallback(m_Window, Key_Callback);
		glfwSetMouseButtonCallback(m_Window, MouseInput_Callback);
		glfwSetCursorPosCallback(m_Window, CursorPosition_Callback);
		glfwSwapInterval(m_Interval);
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Error! GLEW" << std::endl;
			return false;
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		printf("Window: %s\n", m_Title);
		printf("Graphics Processor Unit: %s\n", glGetString(GL_RENDERER));
		printf("Open GL Version: %s\n", glGetString(GL_VERSION));
		printf("Engine Version: %s\n", ENGINE_VERSION);
		printf("V-Sync: %s\n\n", m_Interval == 1 ? "On" : "Off");
		return true;
	}
	void Window::Clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void Window::Update() const
	{
		for (int i = 0; i < MAX_KEYS; i++)
			m_KeyTyped[i] = m_Keys[i] && !m_KeyState[i];
		for (int i = 0; i < MAX_BUTTONS; i++)
			m_MouseClicked[i] = m_MouseButtons[i] && !m_MouseState[i];

		memcpy(m_KeyState, m_Keys, MAX_KEYS);
		memcpy(m_MouseState, m_MouseButtons, MAX_BUTTONS);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			std::cout << "Open GL Error: " << error << std::endl;

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
	bool Window::Closed() const
	{
		return glfwWindowShouldClose(m_Window) == 1;
	}
	bool Window::IsKeyPressed(unsigned int keycode) const
	{
		if (keycode >= MAX_KEYS)
			return false;
		return m_Keys[keycode];
	}
	bool Window::IsKeyTyped(unsigned int keycode) const
	{
		if (keycode >= MAX_KEYS)
			return false;
		return m_KeyTyped[keycode];
	}
	bool Window::IsMouseButtonClicked(unsigned int button) const
	{
		if (button >= MAX_BUTTONS)
			return false;
		return m_MouseClicked[button];
	}
	bool Window::IsMouseButtonPressed(unsigned int button) const
	{
		if (button >= MAX_BUTTONS)
			return false;
		return m_MouseButtons[button];
	}
	void Window::GetMousePosition(double& x, double& y)
	{
		x = mx;
		y = my;
	}

	void WindowSize_Callback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_Width = width;
		win->m_Height = height;
	}
	void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_Keys[key] = action != GLFW_RELEASE;
	}
	void MouseInput_Callback(GLFWwindow* window, int button, int action, int mods)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_MouseButtons[button] = action != GLFW_RELEASE;
	}
	void CursorPosition_Callback(GLFWwindow* window, double xpos, double ypos)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->mx = xpos;
		win->my = ypos;
	}
}}