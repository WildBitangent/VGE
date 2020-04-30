#pragma once
#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include "PolygonGen.hpp"
#include "Collision.hpp"


class Logic : public sf::Drawable, public sf::Transformable
{
	static constexpr float MOVESPEED = 0.5f;

public:
	Logic(sf::Window& window);

	void update(float dt);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	bool checkCollisions();
	void updatePolygons(float dt);

private:
	sf::Window& mWindow;
	PolygonGen mPolygons;
	CollisionDetector mColliDetector;

	std::vector<sf::ConvexShape> mShapes;

	//glm::vec2 mCameraPos = { 0, 0 }; // TODO this is center
};