#include <iostream>
#include <omp.h>
#include <glm/glm.hpp>
#include <chrono>


#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Logic.hpp"


using namespace glm;
using namespace std::chrono_literals;

int main()
{
	using clock = std::chrono::high_resolution_clock;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "VGE proj");
	Logic logic(window);
	
	auto timeStart = clock::now();
	std::chrono::nanoseconds accumulator;

	while (window.isOpen())
	{
		for (sf::Event e; window.pollEvent(e); )
			if (e.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
				window.close();

		auto currentTime = clock::now();
		auto deltaTime = currentTime - timeStart;
		timeStart = currentTime;

		auto delta = std::chrono::duration<float, std::milli>(deltaTime).count();
		if (delta > 1000.f / 144.f)
			delta = 1000.f / 144.f;
		logic.update(delta);

		window.clear();
		window.draw(logic);
		window.display();
	}

	return EXIT_SUCCESS;
}