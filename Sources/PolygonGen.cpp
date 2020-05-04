#include "PolygonGen.hpp"

using namespace glm;

PolygonGen::PolygonGen(size_t memSize)
	: mPool(memSize) // 4 MB // TODO SIZE
	//, mDirPool(1 * 1024 * 1024)
	, mGenerator(mRandomDevice())
{
	setPolygonSize(50);
	setPolygonArea({ 0, 0 }, { 800, 600 });
	setMaxVertices(15);

	mDirections.reserve(memSize);
}

void PolygonGen::setPolygonArea(glm::vec2 areaMin, glm::vec2 areaMax)
{
	mAreaMin = areaMin;
	mAreaMax = areaMax;

	mDistX = std::uniform_real_distribution<float>(areaMin.x, areaMax.x);
	mDistY = std::uniform_real_distribution<float>(areaMin.y, areaMax.y);
}

void PolygonGen::setPolygonSize(float size)
{
	mMaxPolygonSize = size;
	mDistPolySize = std::uniform_real_distribution<float>(25, size);
}

void PolygonGen::setMaxVertices(size_t size)
{
	if (size < 3)
		return;

	mMaxVertices = size;
	mDistMaxVert = std::uniform_int_distribution<size_t>(3, size);
}

PolygonGen::PolyArray& PolygonGen::generatePolygons(size_t count)
{
	mPolygons.clear();
	mPool.clear();
	//mDirPool.clear();
	return resizePolygons(count);
}

PolygonGen::PolyArray& PolygonGen::resizePolygons(size_t count)
{
	mDirections.resize(count);

	if (mPolygons.size() > count)
	{
		mPool.clearFromAdr(mPolygons[count].data());
		//mDirPool.clearFromAdr(&mDirections[count]);
		mPolygons.resize(count);
	}
	else
	{
		for (size_t i = mPolygons.size(); i < count; ++i)
		{
			mPolygons.emplace_back(generatePolygon());
			mDirections[i] = { mDistDir(mGenerator), mDistDir(mGenerator) };
		}
	}

	return mPolygons;
}

PolygonGen::PolyArray& PolygonGen::regenerateLastPolygons(size_t count)
{
	auto start = mPolygons.size() > count ? mPolygons.size() - count : 0;
	
	mDirections.resize(count);
	mPolygons.resize(count);
	mPool.clearFromAdr(mPolygons[count].data());

	for (size_t i = 0; i < count; ++i)
	{
		mPolygons.emplace_back(generatePolygon());
		mDirections[i + start] = { mDistDir(mGenerator), mDistDir(mGenerator) };
	}

	return mPolygons;
}

PolygonGen::PolyArray& PolygonGen::data()
{
	return mPolygons;
}

std::vector<glm::vec2>& PolygonGen::getDirections()
{
	return mDirections;
}

std::span<glm::vec2> PolygonGen::operator[](size_t i) const
{
	return mPolygons[i];
}

size_t PolygonGen::size() const
{
	return mPolygons.size();
}

std::span<glm::vec2> PolygonGen::generatePolygon()
{
	std::vector<float> angles;
	angles.reserve(mMaxVertices);
	angles.clear();

	// generate circle
	auto radius = mDistPolySize(mGenerator);
	vec2 pos = { mDistX(mGenerator), mDistY(mGenerator) };

	// Number of vertices polygon's gonna have
	auto vertCount = mDistMaxVert(mGenerator);
	auto mem = mPool.alloc<vec2>(vertCount);
	std::span<vec2> vertexArray(mem, vertCount);

	// Generate angles on the circle
	for (size_t i = 0; i < vertCount; ++i)
		angles.emplace_back(mDistAngle(mGenerator));

	std::sort(angles.begin(), angles.end());

	// create vertices from angles
	for (size_t i = 0; i < vertCount; ++i)
		vertexArray[i] = pos + vec2{ glm::cos(angles[i]) * radius, glm::sin(angles[i]) * radius };

	return vertexArray;
}
