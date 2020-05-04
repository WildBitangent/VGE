#include "Shapes.hpp"

Line::Line(const sf::Color color) // TODO insert vertices
	: mColor(color)
{

}

void Line::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	for (size_t i = 0; i < mVertices.size() - 1; ++i)
		target.draw(&mVertices[i], 2, sf::Lines, states);
}

void Line::add(const glm::vec2& v)
{
	sf::Vertex x;
	x.color = mColor;
	x.position = { v.x, v.y };
	mVertices.emplace_back(x);
}

void Line::add(const std::span<glm::vec2>& vertices, bool connect)
{
	for (const auto& v : vertices)
		mVertices.emplace_back(sf::Vertex(sf::Vector2f{ v.x, v.y }, mColor));
	if (connect)
		mVertices.emplace_back(mVertices.front());
}

void Line::add(const std::vector<glm::vec2>& vertices, bool connect)
{
	for (const auto& v : vertices)
		mVertices.emplace_back(sf::Vertex(sf::Vector2f{ v.x, v.y }, mColor));
	if (connect)
		mVertices.emplace_back(mVertices.front());
}
