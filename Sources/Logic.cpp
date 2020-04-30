#include "Logic.hpp"
#include "Constants.hpp"
#include <SFML/Window/Keyboard.hpp>


#include "GJK.hpp"

using namespace glm;


Logic::Logic(sf::Window& window)
	: mWindow(window)
{
	size_t defaultPolygonCount = 700;
	mPolygons.generatePolygons(defaultPolygonCount);

	// create shapes
	for (size_t i = 0; i < defaultPolygonCount; ++i)
	{
		const auto& polygon = mPolygons.get()[i];
		
		sf::ConvexShape shape;
		shape.setPointCount(polygon.size());
		
		for (size_t j = 0; j < polygon.size(); ++j)
			shape.setPoint(j, reinterpret_cast<sf::Vector2f&>(polygon[j]));

		mShapes.emplace_back(shape);
	}
	//mShapes.emplace_back(); // Mouse hull

	mColliDetector.setBroadPhaseDetector(std::make_unique<SpatialGrid>(225));
	for (auto& p : mPolygons.get())
		mColliDetector.addCollider(p);
}

void Logic::update(float dt)
{
	// handle camera movement
	vec2 movedir = { 0, 0 };

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		movedir.x += 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		movedir.x -= 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		movedir.y += 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		movedir.y -= 1;

	movedir *= MOVESPEED;
	//if (movedir.x != 0 && movedir.y != 0)
	//{
	//	if (movedir < 0) movedir.x - sqrt(abs(movedir))
	//}
		//movedir = (movedir.x < 0) ?  : sqrt(movedir);

	auto delta = movedir * dt;
	move(delta.x, delta.y);
	//mCameraPos += delta;

	updatePolygons(dt);
	mColliDetector.update();

	//sf::Vector2f halfArea = { (MAX_AREA.x - mWindow.getSize().x) / 2, (MAX_AREA.y - mWindow.getSize().y) / 2 };
	auto position = getPosition();
	position.x = std::min(std::max(position.x, -AREA_SIZE.x + mWindow.getSize().x), AREA_SIZE.x);
	position.y = std::min(std::max(position.y, -AREA_SIZE.y + mWindow.getSize().y), AREA_SIZE.y);
	setPosition(position.x, position.y);

	for (size_t i = 0; i < mShapes.size(); ++i)
		mShapes[i].setFillColor(mColliDetector.queryIsColliding(i) ? sf::Color::Cyan : sf::Color::White);
}

void Logic::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// draw gridlines
	constexpr int stride = 225;

	std::vector<sf::Vertex> lines;
	for (size_t i = 0; i < mWindow.getSize().y / stride + 2; ++i)
	{
		float y = std::fmod(getPosition().y, stride) + i * stride;
		sf::Vertex a, b;

		a.position = { 0.f, y };
		b.position = { static_cast<float>(mWindow.getSize().x), y };
	
		lines.emplace_back(a);
		lines.emplace_back(b);
	}

	for (size_t i = 0; i < mWindow.getSize().x / stride + 2; ++i)
	{
		float x = std::fmod(getPosition().x, stride) + i * stride;
		sf::Vertex a, b;

		a.position = { x, 0.f };
		b.position = { x, static_cast<float>(mWindow.getSize().y) };

		lines.emplace_back(a);
		lines.emplace_back(b);
	}

	//sf::CircleShape circle; 
	//circle.setFillColor(sf::Color::Red); 
	//circle.setPosition(getPosition().x + 50, getPosition().y + 50); 
	//circle.setRadius(100);


	target.draw(lines.data(), lines.size(), sf::Lines, states);

	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable
	for (auto& s : mShapes)
		target.draw(s, states);
}

bool Logic::checkCollisions()
{
	//auto mouse = static_cast<sf::Vector2f>(sf::Mouse::getPosition(mWindow)) - getPosition();
	//vec2 pos = { mouse.x, mouse.y };

	//std::vector<vec2> mouseHull = {
	//	pos + vec2(-20, 20),
	//	pos + vec2(-10, -10),
	//	pos + vec2(10, -10)
	//};

	//sf::ConvexShape& mouseShape = mShapes.back();
	//mouseShape.setPointCount(mouseHull.size());

	//for (size_t i = 0; i < mouseHull.size(); i++)
	//	mouseShape.setPoint(i, { mouseHull[i].x, mouseHull[i].y });

	//for (auto& e : mPolygons.get())
	//	if (GJK(std::span(mouseHull.data(), mouseHull.size()), e))
	//		return true;

	return false; 
}

void Logic::updatePolygons(float dt)
{
	//vec2 halfArea = { (MAX_AREA.x - mWindow.getSize().x) / 2, (MAX_AREA.y - mWindow.getSize().y) / 2 };
	
	#pragma omp parallel for schedule(static)
	for (size_t i = 0; i < mPolygons.get().size(); ++i)
	{
		auto& polygon = mPolygons.get()[i];
		auto& direction = mPolygons.getDirections()[i];
		auto speed = direction * dt;
		
		bool pushX = false;
		bool pushY = false;
		for (auto& v : polygon)
		{
			v += speed;

			if (v.x < -AREA_SIZE.x || v.x > AREA_SIZE.x) pushX = true;
			if (v.y < -AREA_SIZE.y || v.y > AREA_SIZE.y) pushY = true;
		}

		if (pushX || pushY)
		{
			if (pushX) direction.x = -direction.x;
			if (pushY) direction.y = -direction.y;

			speed = direction * dt;
			for (auto& v : polygon)
				v += speed;
		}

		for (size_t j = 0; j < polygon.size(); ++j)
			mShapes[i].setPoint(j, reinterpret_cast<sf::Vector2f&>(polygon[j]));
	}
}
