#pragma once

#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <span>

using Hull = std::span<glm::vec2>;
using DrawCall = std::vector<sf::Vertex>;
using DrawStack = std::vector<DrawCall>;

class SAT
{
public:
	explicit SAT(const Hull& a, const Hull& b);
	operator bool();

private:
	struct MinMaxResult
	{
		float min;
		float max;
	};

	std::vector<glm::vec2> getNormals(const Hull& hull);
	MinMaxResult getMinMax(const Hull& hull, const glm::vec2& axis);

private:

	const Hull& mHullA; // TODO replace with aligned pool AKA vector view
	const Hull& mHullB;
};
