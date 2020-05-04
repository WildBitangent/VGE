#include "GJK.hpp"
#include <algorithm>
#include <memory>

using namespace glm;

namespace
{
	vec2 trippleProd(const vec2& a, const vec2& b, const vec2& c)
	{
		return b * dot(a, c) - a * dot(b, c);
	}

	std::vector<vec2> createConvexEnvelope(std::vector<vec2> points)
	{
		vec2 pivot;

		// returns -1 if a -> b -> c forms a counter-clockwise turn,
		// +1 for a clockwise turn, 0 if they are collinear
		auto ccw = [](const vec2& a, const vec2& b, const vec2& c)
		{
			auto area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
			if (area > 0.f) return -1;
			else if (area < 0.f) return 1;
			return 0;
		};

		// returns square of Euclidean distance between two points
		auto sqrDist = [](const vec2& a, const vec2& b)
		{
			auto d = a - b;
			return dot(d, d);
		};

		// used for sorting points according to polar order w.r.t the pivot
		auto polar_order = [&](const vec2& a, const vec2& b)
		{
			int order = ccw(pivot, a, b);
			if (order == 0)
				return sqrDist(pivot, a) < sqrDist(pivot, b);
			return (order == -1);
		};


		// find the point having the least y coordinate (pivot),
		// ties are broken in favor of lower x coordinate
		for (size_t i = 1; i < points.size(); i++)
			if (points[i].y > points[0].y || (points[i].y == points[0].y && points[i].x < points[0].x))
				std::swap(points[0], points[i]);


		// sort the remaining point according to polar order about the pivot
		pivot = points[0];
		std::sort(points.begin() + 1, points.end(), polar_order);

		std::vector<vec2> hull = {
			points[0],
			points[1],
			points[2],
		};

		for (size_t i = 3; i < points.size(); ++i) 
		{
			auto top = hull.back();
			hull.pop_back();

			while (!hull.empty() && ccw(hull.back(), top, points[i]) != -1)
				top = hull.back(), hull.pop_back();

			hull.emplace_back(top);
			hull.emplace_back(points[i]);
		}

		return hull;
	}
}

GJK::GJK(const Hull& a, const Hull& b)
	: NarrowPhaseDetector(a, b)
{
}

GJK::operator bool()
{
	vec2 direction = { 1, 0 }; // TODO pick better direction ??
	vec2 simplex[3];

	// simplex 0 - point (edge case)
	{
		simplex[0] = support(direction);
		if (dot(simplex[0], direction) <= 0.f)
			return false;

		direction = -simplex[0];
	}

	// simplex 1 - line (edge case)
	{
		simplex[1] = support(direction);
		if (dot(simplex[1], direction) <= 0.f)
			return false;

		vec2 ab = simplex[0] - simplex[1]; // from point A to B

		direction = trippleProd(ab, -simplex[1], ab); // normal to AB towards Origin
		if (dot(direction, direction) == 0.f)
			direction = { ab.y, -ab.x }; // perpendicular vector
	}

	while (true) 
	{
		simplex[2] = support(direction);
		if (dot(simplex[2], direction) <= 0.f)
			return false;

		vec2 ao = -simplex[2];
		vec2 ab = simplex[1] - simplex[2];
		vec2 ac = simplex[0] - simplex[2];
		vec2 acn = trippleProd(ab, ac, ac); // normal to AC

		if (dot(acn, ao) >= 0)
			direction = acn; // new direction is normal to AC towards Origin
		else 
		{
			vec2 abn = trippleProd(ac, ab, ab); // normal to AB
			if (dot(abn, ao) < 0.f)
				return true;

			simplex[0] = simplex[1];
			direction = abn; // new direction is normal to AB towards Origin
		}

		simplex[1] = simplex[2]; // swap element in the middle (point B)
	}

	return false;
}

vec2 GJK::support(const vec2& direction)
{
	const auto furthestPoint = [](const vec2& direction, const Hull& hull)
	{
		float max = dot(direction, hull.front());
		size_t index = 0;

		for (size_t i = 1; i < hull.size(); i++)
		{
			if (float product = dot(direction, hull[i]); product > max)
			{
				max = product;
				index = i;
			}
		}
		return index;
	};

	size_t i = furthestPoint(direction, mHullA);
	size_t j = furthestPoint(-direction, mHullB);
	return mHullA[i] - mHullB[j];
}

GJKVisualizer::GJKVisualizer()
	: GJK(Hull(), Hull())
{
}

void GJKVisualizer::simulate(const Hull& a, const Hull& b)
{
	mHullA = a; 
	mHullB = b;

	mDrawStack.clear();
	DrawCall drawCall;

	auto normalSize = 180.f;
	vec2 direction = { 1, 0 };
	vec2 simplex[3];

	std::vector<vec2> minkDiff;
	for (const auto& a : mHullA)
		for (const auto& b : mHullB)
			minkDiff.emplace_back(a - b);

	auto envelope = createConvexEnvelope(minkDiff);

	auto drawCross = [&]()
	{
		auto horizontal = std::make_unique<Line>(sf::Color::White);
		horizontal->add({ { -10, 0 }, { 10, 0 } });
		drawCall.emplace_back(std::move(horizontal));

		auto vertical = std::make_unique<Line>(sf::Color::White);
		vertical->add({ { 0, -10 }, { 0, 10 } });
		drawCall.emplace_back(std::move(vertical));
	};

	auto drawHulls = [&]()
	{
		auto h1 = std::make_unique<Line>(sf::Color::White);
		h1->add(mHullA, true);
		drawCall.emplace_back(std::move(h1));

		auto h2 = std::make_unique<Line>(sf::Color::White);
		h2->add(mHullB, true);
		drawCall.emplace_back(std::move(h2));
	};

	auto drawEnvelope = [&]()
	{
		auto envline = std::make_unique<Line>(sf::Color::Green);
		envline->add(envelope, true);
		drawCall.emplace_back(std::move(envline));

		for (const auto& p : minkDiff)
		{
			auto point = std::make_unique<Circle>(3.f);
			point->setPosition(p.x, p.y);
			drawCall.emplace_back(std::move(point));
		}
	};

	auto drawNormal = [&](vec2 A, vec2 B)
	{
		auto line = std::make_unique<Line>(sf::Color::Red);
		line->add(A);
		line->add(B);
		drawCall.emplace_back(std::move(line));
	};

	auto drawHighlightVertex = [&](const vec2& v)
	{
		auto vert = std::make_unique<Circle>(4.5f, sf::Color::Cyan);
		vert->setPosition(v.x, v.y);
		drawCall.emplace_back(std::move(vert));
	};

	auto drawSimplex = [&](size_t simplexSize = 3)
	{
		for (size_t i = 0; i < simplexSize; ++i)
		{
			auto vert = std::make_unique<Circle>(3.5f, sf::Color::Yellow);
			vert->setPosition(simplex[i].x, simplex[i].y);
			drawCall.emplace_back(std::move(vert));
		}
		
		auto line = std::make_unique<Line>(sf::Color::Yellow);
		line->add(std::span(simplex, simplexSize), true);
		drawCall.emplace_back(std::move(line));
	};

	auto drawText = [&](const std::string& text)
	{
		drawCall.emplace_back(std::make_unique<Text>(text));
	};

	// 1st step - create minkowsky diff vizual
	{
		drawCross();
		drawHulls();
		drawEnvelope();
		drawText(
			"                               GJK                \n\n"

			"Visualization of Minkowski difference (green area)\n"
			"of 2 convex shapes. White vertices are calculated\n"
			"by Minkowski difference - every vertex of shape B\n"
			"is substracted from every vertex of shape A\n\n"

			"Idea of GJK algorithm is, that 2 convex shapes\n"
			"collide, only if origin (point [0, 0] is inside\n"
			"their minkowski difference convex envelope."
		);

		mDrawStack.emplace_back(std::move(drawCall));
	}

	// Simplex 0 case
	{
		drawCross();
		drawEnvelope();
		drawNormal({ 0.f, 0.f }, direction * normalSize);
		//drawHighlightVertex(simplex[0]);
		drawText(
			"With support function find furthest\n"
			"vertex along direction (red)."
		);
		mDrawStack.emplace_back(std::move(drawCall));

		simplex[0] = support(direction);
		drawCross();
		drawEnvelope();
		drawNormal({ 0.f, 0.f }, direction * normalSize);
		drawHighlightVertex(simplex[0]);
		drawText("The furthest point is found.");
		mDrawStack.emplace_back(std::move(drawCall));
		
		if (dot(simplex[0], direction) <= 0.f)
		{
			drawCross();
			drawEnvelope();
			drawSimplex(1);
			//drawNormal({ 0.f, 0.f }, direction * normalSize);
			drawText(
				"At this stage is determined that\n"
				"every point lies behind origin\n"
				"and collision is not possible"
			);
			
			mDrawStack.emplace_back(std::move(drawCall));
			return;
		}

		direction = -simplex[0];
	}

	// Simplex 1 case - 
	{
		simplex[1] = support(direction);
		
		drawCross();
		drawEnvelope();
		drawNormal({ 0.f, 0.f }, normalize(direction) * normalSize);
		drawSimplex(1);
		drawText(
			"Pick -direction of the selected point.\n"
			"Using support function, find furthest vertex\n"
			"along direction."
		);
		mDrawStack.emplace_back(std::move(drawCall));

		drawCross();
		drawEnvelope();
		drawNormal({ 0.f, 0.f }, normalize(direction) * normalSize);
		drawSimplex(2);
		drawHighlightVertex(simplex[1]);
		drawSimplex(1);
		drawText("New support point is found, creating 1-simplex.");
		mDrawStack.emplace_back(std::move(drawCall));

		if (dot(simplex[1], direction) <= 0.f)
		{
			drawCross();
			drawEnvelope();
			drawSimplex(2);
			drawText(
				"At this stage is determined that\n"
				"every point lies behind origin\n"
				"and collision is not possible"
			);

			mDrawStack.emplace_back(std::move(drawCall));
			return;
		}
		
		vec2 ab = simplex[0] - simplex[1]; // from point A to B

		direction = trippleProd(ab, -simplex[1], ab); // normal to AB towards Origin
		if (dot(direction, direction) == 0.f)
			direction = { ab.y, -ab.x }; // perpendicular vector

		drawCross();
		drawEnvelope();
		drawSimplex(2);
		drawNormal({ 0.f, 0.f }, normalize(direction) * normalSize);
		drawText(
			"New direction is computed as perpendicular vector\n"
			"to 1-simplex and is pointing towards origin.\n"
			"Now with support function find another\n"
			"furthest vertex along given direction"
		);
		mDrawStack.emplace_back(std::move(drawCall));
	}
	
	// Simplex 2 case -
	{
		while (true)
		{
			simplex[2] = support(direction);

			drawCross();
			drawEnvelope();
			drawSimplex(3);
			drawNormal({ 0.f, 0.f }, normalize(direction) * normalSize);
			drawHighlightVertex(simplex[2]);
			drawText(
				"The support vertex is found.\n"
				"We know, that other 2 support points lie behind origin\n"
				"so if this one lies in front of it, intersection will be\n"
				"possible."
			);
			mDrawStack.emplace_back(std::move(drawCall));

			if (dot(simplex[2], direction) <= 0.f)
			{
				drawCross();
				drawEnvelope();
				drawSimplex();
				drawText(
					"Found point lies behind origin\n"
					"means that every point is behind origin\n"
					"and collision is not possible."
				);
				mDrawStack.emplace_back(std::move(drawCall));
				
				return;
			}

			vec2 ao = -simplex[2];
			vec2 bc = simplex[1] - simplex[2];
			vec2 ac = simplex[0] - simplex[2];
			vec2 acn = trippleProd(bc, ac, ac); // normal to AC

			drawCross();
			drawEnvelope();
			drawSimplex(3);
			drawText(
				"Vertex indeed lies in front of origin. Now check if\n"
				"origin is inside the 2-simplex by performing dot product\n"
				"over normal of AC and BC 1-simplices."
			);
			mDrawStack.emplace_back(std::move(drawCall));

			if (dot(acn, ao) >= 0)
				direction = acn; // new direction is normal to AC towards Origin
			else
			{
				vec2 abn = trippleProd(ac, bc, bc); // normal to BC
				if (dot(abn, ao) < 0.f)
				{
					drawCross();
					drawEnvelope();
					drawSimplex(3);
					drawText("Collision is found - origin point is inside 2-simplex.");
					mDrawStack.emplace_back(std::move(drawCall));

					return;
				}

				simplex[0] = simplex[1];
				direction = abn; // new direction is normal to AB towards Origin
			}


			simplex[1] = simplex[2]; // swap element in the middle (point B)

			drawCross();
			drawEnvelope();
			drawSimplex(2);
			drawNormal({ 0.f, 0.f }, normalize(direction) * normalSize);
			//drawHighlightVertex(simplex[2]);
			std::string text =
				"Origin is not inside 2-simplex, but there are still some\n"
				"vertices left. Continuing to next iteration of algorithm.\n"
				"New direction is picked from the ";
			text += (direction == acn ? "AC" : "BC");
			text +=
				" 1-simplex,\n"
				"is perpedicular to it and pointing towards origin.";
			drawText(text);
			mDrawStack.emplace_back(std::move(drawCall));
		}
	}
}
