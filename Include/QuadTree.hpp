#pragma once

#include "Collision.hpp"
#include <set>
#include <vector>

class QuadTree
{
public:
    QuadTree(glm::vec2 topLeft, glm::vec2 botRight);

    struct QuadTreeObject
    {
        glm::vec2 minBound;
        glm::vec2 maxBound;
        CollisionID objectID;
        Object object;
        QuadTreeObject(Object object, CollisionID objectID);
        void update();
    };

    void reset();
    bool inBounds(QuadTreeObject& object);
    void insert(QuadTreeObject& object);
    void getCollidingObjects(QuadTreeObject& object, std::set<CollisionID>& collisions);

private:
    glm::vec2 mTopLeft;
    glm::vec2 mBotRight;
    glm::vec2 mCenter;

    std::vector<CollisionID> mQuadObjects;

    std::unique_ptr<QuadTree> mTopLeftTree;
    std::unique_ptr<QuadTree> mTopRightTree;
    std::unique_ptr<QuadTree> mBotLeftTree;
    std::unique_ptr<QuadTree> mBotRightTree;
};

class QuadTreeDetector : public BroadPhaseDetector
{
public:
    QuadTreeDetector(float minLeafSize);

    virtual Pairs generatePairs() override;
    virtual void onColliderAddition() override;

private:
    QuadTree mQuadTree;
    glm::vec2 mTopLeft;
    glm::vec2 mBotRight;
    std::vector<QuadTree::QuadTreeObject> mTreeObjects;
};

