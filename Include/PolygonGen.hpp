#pragma once
#include <glm/glm.hpp>
#include <random>
#include <vector>
#include <span>

#include "VertexPool.hpp"

class PolygonGen
{
public:
	using PolyArray = std::vector<std::span<glm::vec2>>;
public:
	PolygonGen();

	void setPolygonArea(glm::uvec2 areaMin, glm::uvec2 areaMax);
	void setPolygonSize(float size);
	void setMaxVertices(size_t size);

	PolyArray& generatePolygons(size_t count);
	PolyArray& resizePolygons(size_t count);
	PolyArray& get();
	std::span<glm::vec2> getDirections();

private:
	std::span<glm::vec2> generatePolygon();

private:
	VertexPool mPool;
	VertexPool mDirPool;
	PolyArray mPolygons;
	std::span<glm::vec2> mDirections;

	glm::uvec2 mAreaMin;
	glm::uvec2 mAreaMax;

	float mMaxPolygonSize;
	size_t mMaxVertices;

	std::random_device mRandomDevice;
	std::mt19937 mGenerator;

	std::uniform_real_distribution<float> mDistDir{ -1.f, 1.f };
	std::uniform_real_distribution<float> mDistAngle{ 0.f, 2 * 3.14159265358979f };
	std::uniform_real_distribution<float> mDistX;
	std::uniform_real_distribution<float> mDistY;
	std::uniform_real_distribution<float> mDistPolySize;
	std::uniform_int_distribution<size_t> mDistMaxVert;
};