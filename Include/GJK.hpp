#pragma once
#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <span>

using Hull = std::span<glm::vec2>;
using DrawCall = std::vector<sf::Vertex>;
using DrawStack = std::vector<DrawCall>;

class GJK
{
public:
	explicit GJK(const Hull& a, const Hull& b);
	operator bool();

private:
	glm::vec2 support(const glm::vec2& direction);

private:
	const Hull& mHullA; // TODO replace with aligned pool AKA vector view
	const Hull& mHullB;
};

class GJKVisualizer
{
public:
	DrawStack simulate();

	DrawStack& operator()() { return mDrawStack; }
	DrawCall& operator[](size_t i) { return mDrawStack[i]; }

private:



private:
	DrawStack mDrawStack;
	const Hull& mHullA;
	const Hull& mHullB;
};