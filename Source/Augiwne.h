#pragma once

#ifndef AUGIWNE_H

#define AUGIWNE_H
#include <System.h>
#include <Maths\Maths.h>
#include <Utilities\StopWatch.h>
#include <Graphics\window.h>
#include <Graphics\Sprite.h>
#include <Graphics\Renderers\2D\SimpleRenderer2D.h>
#include <Graphics\Renderers\2D\BatchRenderer2D.h>
#include <Graphics\Layers\Layer.h>
#include <Graphics\Layers\TileLayer.h>
#include <Graphics\Layers\Group.h>
#include <Graphics\Camera.h>
#include <Network\Client.h>

#endif

namespace Augiwne {
	class Augiwne {
	private:
		Graphics::Window* m_Window;
		float m_Timer;
		unsigned int m_FramesPerSecond;
		unsigned int m_UpdatesPerSecond;
	public:
		StopWatch* m_Watcher;
		void Run()
		{
			float timer = 0.0f;
			float updateTimer = 0.0f;
			float updateTick = 1.0f / 128.0f;
			m_Watcher = new StopWatch();
			unsigned int frames = 0;
			unsigned int updates = 0;
			while (!m_Window->Closed())
			{
				m_Window->Clear();
				if (m_Watcher->Elapsed() - updateTimer > updateTick)
				{
					updates++;
					Update();
					updateTimer += updateTick;
				}
				Render();
				frames ++;
				m_Window->Update();
				if (m_Watcher->Elapsed() - m_Timer > 1.0f)
				{
					Tick();
					m_Timer += 1.0f;
					m_FramesPerSecond = frames;
					m_UpdatesPerSecond = updates;
					frames = 0; updates = 0;
				}
			}
		}
		Augiwne()
			: m_FramesPerSecond(0), m_UpdatesPerSecond(0) {

		}
		virtual ~Augiwne () {
			delete m_Watcher;
			delete m_Window;
		}
		Graphics::Window* CreateRenderWindow(const char* name, int width, int height, int interval) {
			m_Window = new Graphics::Window(name, width, height, interval);
			return m_Window;
		}

		void Start() {
			Init();
			Run();
		}
	protected:
		virtual void Init() = 0;
		virtual void Tick() { }
		virtual void Update() { };
		virtual void Render() = 0;
	public:
		const unsigned int GetFPS() const { return m_FramesPerSecond; }
		const unsigned int GetUPS() const { return m_UpdatesPerSecond; }
	};
}