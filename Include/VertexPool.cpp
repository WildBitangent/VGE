#include "VertexPool.hpp"

VertexPool::VertexPool(size_t size)
	: mPoolSize(size * sizeof(float))
{
	mPool = reinterpret_cast<float*>(_aligned_malloc(mPoolSize, GLOBAL_ALIGNMENT));
	//mPool = reinterpret_cast<float*>(_mm_malloc(mPoolSize, GLOBAL_ALIGNMENT));
}

VertexPool::~VertexPool()
{
	_aligned_free(mPool);
	//_mm_free(mPool);
}

void VertexPool::clear()
{
	mCurrentOffset = 0;
}
