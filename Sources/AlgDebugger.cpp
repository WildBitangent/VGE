#include "AlgDebugger.hpp"
#include "Constants.hpp"

#include "GJK.hpp"

using namespace glm;

AlgDebugger::AlgDebugger()
	: mPolygons(512)
{
	vec2 offset = WINSIZE / 8.f;
	mPolygons.setPolygonArea(-offset, offset);
	mPolygons.setPolygonSize(120.f);
	mPolygons.setMaxVertices(5); // TODO tweak
	mPolygons.generatePolygons(2);

	mVisualizers.emplace_back(std::make_unique<GJKVisualizer>()); // TODO maybe use hash map with string key?
	// TODO add SAT visualizer

	mVisualizers[mVisualizerIndex]->simulate(mPolygons[0], mPolygons[1]);
}

void AlgDebugger::update(float dt)
{
}

void AlgDebugger::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// TODO GUI for visualizer info

	// Move render area so the origin is at the center of screen
	states.transform.translate(640, 360);

	// Draw simulation step count
	Text info(std::to_string(mSimulationIndex + 1) + "/" + std::to_string(mVisualizers[mVisualizerIndex]->size()));
	info.mText.setPosition(350, -270);
	info.mText.setCharacterSize(17);
	target.draw(info, states);

	for (const auto& x : (*mVisualizers[mVisualizerIndex])[mSimulationIndex])
		target.draw(*x, states);
}


void AlgDebugger::onEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
			case sf::Keyboard::Key::R:
			{
				mPolygons.generatePolygons(2);
				mSimulationIndex = 0;
				mVisualizers[mVisualizerIndex]->simulate(mPolygons[0], mPolygons[1]);
				break;
			}

			case sf::Keyboard::Key::Num1: // GJK Visualizer
			{
				mSimulationIndex = 0; 
				mSimulationIndex = 0;
				mVisualizers[mVisualizerIndex]->simulate(mPolygons[0], mPolygons[1]);
				break;
			}

			case sf::Keyboard::Key::Num2: // TODO SAT Visualizer
			{
				//mVisualizerIndex = 1;
				//mSimulationIndex = 0;
				//mVisualizers[mVisualizerIndex]->simulate(mPolygons[0], mPolygons[1]);
				break;
			}
			case sf::Keyboard::Key::Left:
			{
				mSimulationIndex = std::max(0, mSimulationIndex - 1);
				break;
			}
			case sf::Keyboard::Key::Right:
			{
				auto size = static_cast<int>(mVisualizers[mVisualizerIndex]->size());
				mSimulationIndex = std::min(size - 1, mSimulationIndex + 1);
				break;
			}
		}
	}
}
