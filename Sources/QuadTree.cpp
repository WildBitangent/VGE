#include "QuadTree.hpp"
#include "Constants.hpp"

namespace
{
    float approxMinLeafSize;
}

QuadTreeDetector::QuadTreeDetector(float minLeafSize) :
    mQuadTree(glm::vec2(-AREA_SIZE.x * 0.5f), glm::vec2(AREA_SIZE.x * 0.5f)),
    mTopLeft(-AREA_SIZE.x * 0.5f),
    mBotRight(AREA_SIZE.x * 0.5f)
{
    approxMinLeafSize = minLeafSize;
}

Pairs QuadTreeDetector::generatePairs()
{
    mQuadTree.reset();

    for (CollisionID i = 0; i < mObjects.size(); i++)
    {
        mTreeObjects[i].update();
        if (mQuadTree.inBounds(mTreeObjects[i]))
            mQuadTree.insert(mTreeObjects[i]);
    }

    Pairs pairs;
    
    #pragma omp parallel for shared(pairs) // Nothing 124
    for (CollisionID i = 0; i < mTreeObjects.size() - 1; i++)
    {
        auto collidingObjects = std::set<CollisionID>();
        if (mQuadTree.inBounds(mTreeObjects[i]))
        {
            mQuadTree.getCollidingObjects(mTreeObjects[i], collidingObjects);
            for (auto& collidingObject : collidingObjects)
            {
                #pragma omp critical  // 108
                pairs.emplace_back(i, collidingObject);
            }
        }
    }
    
    return pairs;
}

bool QuadTree::inBounds(QuadTreeObject& object)
{
    return object.minBound.x >= mTopLeft.x && object.minBound.y >= mTopLeft.y && object.maxBound.x <= mBotRight.x && object.maxBound.y <= mBotRight.y;
}

void QuadTreeDetector::onColliderAddition()
{
    mTreeObjects.push_back(QuadTree::QuadTreeObject(mObjects.back(), mObjects.size() - 1));
}

QuadTree::QuadTreeObject::QuadTreeObject(Object object, CollisionID objectID) :
    objectID(objectID),
    object(object)
{}

void QuadTree::QuadTreeObject::update()
{
    minBound = glm::vec2(std::numeric_limits<float>::max());
    maxBound = glm::vec2(std::numeric_limits<float>::lowest());
    for (auto& vertex : object) // Compute AABB
    {
        minBound.x = glm::min(minBound.x, vertex.x);
        minBound.y = glm::min(minBound.y, vertex.y);
        maxBound.x = glm::max(maxBound.x, vertex.x);
        maxBound.y = glm::max(maxBound.y, vertex.y);
    }
}

QuadTree::QuadTree(glm::vec2 topLeft, glm::vec2 botRight) :
    mTopLeft(topLeft),
    mBotRight(botRight),
    mCenter((topLeft + botRight) * 0.5f)
{
}

void QuadTree::reset()
{
    mTopLeftTree.reset(nullptr);
    mBotLeftTree.reset(nullptr);
    mTopRightTree.reset(nullptr);
    mBotRightTree.reset(nullptr);
}

void QuadTree::insert(QuadTreeObject& object)
{
    if (mBotRight.x - mTopLeft.x <= approxMinLeafSize && mBotRight.y - mTopLeft.y <= approxMinLeafSize)
    {
        mQuadObjects.push_back(object.objectID);
        return;
    }

    bool atTop = object.minBound.y <= mCenter.y;
    bool atBot = object.maxBound.y >= mCenter.y;
    bool atLeft = object.minBound.x <= mCenter.x;
    bool atRight = object.maxBound.x >= mCenter.x;
    bool inTopLeft = atTop && atLeft;
    bool inBotLeft = atBot && atLeft;
    bool inTopRight = atTop && atRight;
    bool inBotRight = atBot && atRight;

    if (inTopLeft)
    {
        if (!mTopLeftTree)
            mTopLeftTree.reset(new QuadTree(mTopLeft, mCenter));
        mTopLeftTree->insert(object);
    }

    if (inBotLeft)
    {
        if (!mBotLeftTree)
            mBotLeftTree.reset(new QuadTree(glm::vec2(mTopLeft.x, mCenter.y), glm::vec2(mCenter.x, mBotRight.y)));
        mBotLeftTree->insert(object);
    }

    if (inTopRight)
    {
        if (!mTopRightTree)
            mTopRightTree.reset(new QuadTree(glm::vec2(mCenter.x, mTopLeft.y), glm::vec2(mBotRight.x, mCenter.y)));
        mTopRightTree->insert(object);
    }

    if (inBotRight)
    {
        if (!mBotRightTree)
            mBotRightTree.reset(new QuadTree(mCenter, mBotRight));
        mBotRightTree->insert(object);
    }
}

void QuadTree::getCollidingObjects(QuadTreeObject& object, std::set<CollisionID>& collisions)
{
    if (mQuadObjects.size())
    {
        for (auto quadObject : mQuadObjects)
            if (quadObject > object.objectID)
                collisions.emplace(quadObject);
        return;
    }

    bool atTop = object.minBound.y <= mCenter.y;
    bool atBot = object.maxBound.y >= mCenter.y;
    bool atLeft = object.minBound.x <= mCenter.x;
    bool atRight = object.maxBound.x >= mCenter.x;
    bool inTopLeft = atTop && atLeft;
    bool inBotLeft = atBot && atLeft;
    bool inTopRight = atTop && atRight;
    bool inBotRight = atBot && atRight;

    if (inTopLeft && mTopLeftTree)
        mTopLeftTree->getCollidingObjects(object, collisions);

    if (inBotLeft && mBotLeftTree)
       mBotLeftTree->getCollidingObjects(object, collisions);

    if (inTopRight && mTopRightTree)
        mTopRightTree->getCollidingObjects(object, collisions);

    if (inBotRight && mBotRightTree)
        mBotRightTree->getCollidingObjects(object, collisions);
}
