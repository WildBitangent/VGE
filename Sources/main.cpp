#include <iostream>
#include <omp.h>
#include <glm/glm.hpp>
#include <chrono>
#include <string>


#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Constants.hpp"
#include "PerfBench.hpp"
#include "AlgDebugger.hpp"

using namespace glm;
using namespace std::chrono_literals;

int main()
{
	using clock = std::chrono::high_resolution_clock;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(WINSIZE.x, WINSIZE.y), "VGE proj", 7, settings);
	
	size_t selectedlevel = 0;
	std::vector<std::unique_ptr<Level>> levels;
	levels.emplace_back(std::make_unique<PerfBench>(window));
	levels.emplace_back(std::make_unique<AlgDebugger>());

	std::string baseText(
		"F1 - Performance benchmark\n"
		"F2 - Algorithm debugger\n\n"
	);

	std::string level0Text(
		"WASD - Move in scene\n"
	);

	std::string level1Text(
		"Left/Right - Previous/next step\n"
		"Num 1 - GJK\n"
		"Num 2 - SAT\n"
		"R - New polygon pair\n"
	);

	Text overlayText(baseText + level0Text);
	overlayText.mText.setCharacterSize(15);
	//overlayText.mText.setFillColor(sf::Color::Green);
	overlayText.mText.setPosition(10, 10);
	
	auto timeStart = clock::now();

	while (window.isOpen())
	{
		for (sf::Event e; window.pollEvent(e); )
		{
			if (e.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
				window.close();
			else if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Key::F1)
			{
				selectedlevel = 0;
				overlayText.mText.setString(baseText + level0Text);
			}
			else if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Key::F2)
			{
				selectedlevel = 1;
				overlayText.mText.setString(baseText + level1Text);
			}
			else
				levels[selectedlevel]->onEvent(e);
		}

		auto currentTime = clock::now();
		auto deltaTime = currentTime - timeStart;
		timeStart = currentTime;

		auto delta = std::chrono::duration<float, std::milli>(deltaTime).count();
		//if (delta > 100.f / 144.f)
		//	delta = 100.f / 144.f;

		levels[selectedlevel]->update(delta);

		window.clear();
		window.draw(*levels[selectedlevel]);
		window.draw(overlayText);
		window.display();
	}

	return EXIT_SUCCESS;
}