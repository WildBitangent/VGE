#include "SAT.hpp"

SAT::SAT(const Hull& a, const Hull& b)
	: NarrowPhaseDetector(a, b)
{
}

SAT::operator bool()
{
	bool separated = false;
	
	auto hullNormals = getNormals(mHullA);
	for (size_t i = 0; i < hullNormals.size() && !separated; i++)
	{
		auto result1 = getMinMax(mHullA, hullNormals[i]);
		auto result2 = getMinMax(mHullB, hullNormals[i]);
		separated = result1.max < result2.min || result2.max < result1.min;
	}

	if (!separated)
	{
		hullNormals = getNormals(mHullB);
		for (size_t i = 0; i < hullNormals.size() && !separated; i++)
		{
			auto result1 = getMinMax(mHullA, hullNormals[i]);
			auto result2 = getMinMax(mHullB, hullNormals[i]);
			separated = result1.max < result2.min || result2.max < result1.min;
		}
	}

	return !separated;
}

std::vector<glm::vec2> SAT::getNormals(const Hull& hull)
{
	static auto normalFactor = glm::vec2(1.0, -1.0);

	std::vector<glm::vec2> normals(hull.size());
	for (size_t i = 0; i < hull.size(); i++)
	{
		auto direction = hull[(i + 1) % hull.size()] - hull[i];
		normals[i] = glm::normalize(glm::vec2(direction.y, direction.x) * normalFactor);
	}
	return normals;
}

SAT::MinMaxResult SAT::getMinMax(const Hull& hull, const glm::vec2& axis)
{
	auto min = std::numeric_limits<float>::max();
	auto max = std::numeric_limits<float>::lowest();
	size_t minIndex = 0;
	size_t maxIndex = 0;

	for (size_t i = 0; i < hull.size(); i++)
	{
		auto currentProjection = glm::dot(hull[i], axis);
		//select the maximum projection on axis to corresponding box corners
		if (currentProjection < min)
		{
			min = currentProjection;
			minIndex = i;
		}
		//select the minimum projection on axis to corresponding box corners
		if (currentProjection > max)
		{
			max = currentProjection;
			maxIndex = i;
		}
		max = glm::max(max, currentProjection);
	}

	return { min, minIndex, max, maxIndex };
}

SATVisualizer::SATVisualizer()
	: SAT(Hull(), Hull())
{
}

void SATVisualizer::simulate(const Hull& a, const Hull& b)
{
	mHullA = a;
	mHullB = b;

	mDrawStack.clear();
	DrawCall drawCall;

	glm::vec2 topLeft(std::numeric_limits<float>::max());
	glm::vec2 botRight(std::numeric_limits<float>::lowest());

	for (auto& point : mHullA)
	{
		topLeft.x = glm::min(topLeft.x, point.x);
		topLeft.y = glm::min(topLeft.y, point.y);
		botRight.x = glm::max(botRight.x, point.x);
		botRight.y = glm::max(botRight.y, point.y);
	}

	for (auto& point : mHullB)
	{
		topLeft.x = glm::min(topLeft.x, point.x);
		topLeft.y = glm::min(topLeft.y, point.y);
		botRight.x = glm::max(botRight.x, point.x);
		botRight.y = glm::max(botRight.y, point.y);
	}

	auto radius = glm::abs(glm::length(topLeft - botRight)) * 0.5f;
	auto center = (topLeft + botRight) * 0.5f;

	auto drawLine = [&](glm::vec2 A, glm::vec2 B, sf::Color color)
	{
		auto line = std::make_unique<Line>(color);
		line->add(A);
		line->add(B);
		drawCall.emplace_back(std::move(line));
	};

	auto drawDottedLine = [&](glm::vec2 A, glm::vec2 B, sf::Color color)
	{
		auto direction = B - A;
		auto length = glm::length(direction);
		auto actualLength = 0.0f;
		auto step = 10.0f;
		auto pause = step / 2.0f;
		direction = glm::normalize(direction);
		auto start = A;
		while (true)
		{
			if (actualLength + step + pause < length)
			{
				drawLine(start, start + direction * step, color);
				start = start + direction * (step + pause);
				actualLength += step + pause;
			}
			else
			{
				drawLine(start, start + direction * (length - actualLength), color);
				break;
			}
		}
	};

	auto drawText = [&](const std::string& text)
	{
		drawCall.emplace_back(std::make_unique<Text>(text));
	};

	auto introduction = [&]()
	{
		drawText(
			"                               SAT                \n\n"

			"Visualization of Separating Axis Theorem algorithm.\n"
			"of 2 convex shapes.\n\n"

			"Idea of SAT algorithm is, that 2 convex shapes\n"
			"collide, only if axis separating these two\n"
			"shapes cannot be found.\n\n"

			"Firstly one shape (green) is selected and then\n"
			"iteration through its edge normals (red) begins."
		);
	};

	auto explanation = [&]()
	{
		drawText(
			"In each cycle shapes are projected to line which\n"
			"is parallel to actual selected normal. Projection\n"
			"is done as dot product of all points with\n"
			"normal and selecting minimum and maximum value.\n"
			"This is done for both shapes.\n\n"

			"In visualisation these projections are shown as\n"
			"lines corresponding to its shapes by their color.\n"
			"Line is purple on segment where projections overlap.\n"
			"If no overlap can be found solution is found.\n"
		);
	};

	auto nextShape = [&]()
	{
		drawText(
			"All normals was tested in first (green) shape so\n"
			"next (blue)shape must by tested for finding the\n"
			"axis same using same process as first shape. So\n"
			"first normal is taken for test.\n"
		);
	};

	auto finishSeparated = [&]()
	{
		drawText(
			"Shapes are not overlapping in projection and\n"
			"axis separating both shapes is found which\n"
			"means that shapes are not colliding.\n"
		);
	};

	auto finishCollision = [&]()
	{
		drawText(
			"Axis separating both shapes cannot be found\n"
			"after testing all normals, therefore shapes are\n"
			"are colliding.\n"
		);
	};

	auto nextNormal = [&]()
	{
		drawText(
			"Another normal is taken because overlap was\n"
			"was found in last step. Shapes are projected\n"
			"to normal using dot product.\n"
		);
	};

	auto drawNormal = [&](glm::vec2 A, glm::vec2 normal)
	{
		drawLine(A, A + normal * 100.0f, sf::Color::Red);
	};

	auto drawNormalBackground = [&](glm::vec2 A, glm::vec2 normal)
	{
		auto color = sf::Color(255, 255, 255, 100);
		drawLine(A - normal * 3000.0f, A + normal * 3000.0f, color);
	};

	auto drawProjection = [&](glm::vec2 start, glm::vec2 normal, MinMaxResult hullAProj, MinMaxResult hullBProj, bool axis)
	{
		auto dottedHullACol = sf::Color::Green;
		dottedHullACol.a = 150;
		auto dottedHullBCol = sf::Color::Blue;
		dottedHullBCol.a = 150;
		auto dir = glm::vec2(normal.y, -normal.x);

		start = center + normal * glm::dot(normal, -center) + dir * radius;

		drawNormalBackground(start, normal);

		drawLine(start + normal * hullAProj.min, start + normal * hullAProj.max, sf::Color::Green);
		drawLine(start + normal * hullBProj.min, start + normal * hullBProj.max, sf::Color::Blue);

		drawDottedLine(mHullA[hullAProj.minIndex], start + normal * hullAProj.min, dottedHullACol);
		drawDottedLine(mHullA[hullAProj.maxIndex], start + normal * hullAProj.max, dottedHullACol);

		drawDottedLine(mHullB[hullBProj.minIndex], start + normal * hullBProj.min, dottedHullBCol);
		drawDottedLine(mHullB[hullBProj.maxIndex], start + normal * hullBProj.max, dottedHullBCol);

		if (hullAProj.max > hullBProj.min && hullAProj.min < hullBProj.max)
			drawLine(start + normal * glm::max(hullAProj.min, hullBProj.min), start + normal * glm::min(hullAProj.max, hullBProj.max), sf::Color::Magenta);
		else if (axis)
		{
			auto color = sf::Color(255, 255, 255, 150);
			auto midPoint = (start + normal * glm::max(hullAProj.min, hullBProj.min) + start + normal * glm::min(hullAProj.max, hullBProj.max)) * 0.5f;
			drawDottedLine(midPoint + dir * 3000.0f, midPoint - dir * 3000.0f, color);
		}
	};

	auto drawHulls = [&]()
	{
		auto h1 = std::make_unique<Line>(sf::Color::Green);
		h1->add(mHullA, true);
		drawCall.emplace_back(std::move(h1));

		auto h2 = std::make_unique<Line>(sf::Color::Blue);
		h2->add(mHullB, true);
		drawCall.emplace_back(std::move(h2));
	};

	bool first = true;
	bool separated = false;

	auto hullNormals = getNormals(mHullA);
	for (size_t i = 0; i < hullNormals.size() && !separated; i++)
	{
		auto result1 = getMinMax(mHullA, hullNormals[i]);
		auto result2 = getMinMax(mHullB, hullNormals[i]);
		separated = result1.max < result2.min || result2.max < result1.min;

		auto start = (mHullA[(i + 1) % hullNormals.size()] + mHullA[i]) * 0.5f;

		if (first)
		{
			first = false;

			drawHulls();
			drawNormal(start, hullNormals[i]);
			introduction();
			mDrawStack.emplace_back(std::move(drawCall));

			drawHulls();
			drawNormal(start, hullNormals[i]);
			drawProjection(start, hullNormals[i], result1, result2, false);
			explanation();
			mDrawStack.emplace_back(std::move(drawCall));
		}
		else
		{
			drawHulls();
			drawNormal(start, hullNormals[i]);
			drawProjection(start, hullNormals[i], result1, result2, false);
			nextNormal();
			mDrawStack.emplace_back(std::move(drawCall));
		}

		if (separated)
		{
			drawHulls();
			drawNormal(start, hullNormals[i]);
			drawProjection(start, hullNormals[i], result1, result2, true);
			finishSeparated();
			mDrawStack.emplace_back(std::move(drawCall));
		}
	}

	first = true;

	if (!separated)
	{
		hullNormals = getNormals(mHullB);
		for (size_t i = 0; i < hullNormals.size() && !separated; i++)
		{
			auto result1 = getMinMax(mHullA, hullNormals[i]);
			auto result2 = getMinMax(mHullB, hullNormals[i]);
			separated = result1.max < result2.min || result2.max < result1.min;

			auto start = (mHullB[(i + 1) % hullNormals.size()] + mHullB[i]) * 0.5f;

			if (first)
			{
				first = false;

				drawHulls();
				drawNormal(start, hullNormals[i]);
				drawProjection(start, hullNormals[i], result1, result2, false);
				nextShape();
				mDrawStack.emplace_back(std::move(drawCall));
			}
			else
			{
				drawHulls();
				drawNormal(start, hullNormals[i]);
				drawProjection(start, hullNormals[i], result1, result2, false);
				nextNormal();
				mDrawStack.emplace_back(std::move(drawCall));
			}

			if (separated)
			{
				drawHulls();
				drawNormal(start, hullNormals[i]);
				drawProjection(start, hullNormals[i], result1, result2, true);
				finishSeparated();
				mDrawStack.emplace_back(std::move(drawCall));
			}
			else if (i == hullNormals.size() - 1)
			{
				drawHulls();
				drawNormal(start, hullNormals[i]);
				drawProjection(start, hullNormals[i], result1, result2, false);
				finishCollision();
				mDrawStack.emplace_back(std::move(drawCall));
			}
		}
	}
}
