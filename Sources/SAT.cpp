#include "SAT.hpp"

SAT::SAT(const Hull& a, const Hull& b)
    : mHullA(a)
    , mHullB(b)
{
}

SAT::operator bool()
{
    auto normalsHullA = getNormals(mHullA);
    auto normalsHullB = getNormals(mHullB);
    bool separated = false;

    for (size_t i = 0; i < normalsHullA.size() && !separated; i++)
    {
        auto result1 = getMinMax(mHullA, normalsHullA[i]);
        auto result2 = getMinMax(mHullB, normalsHullA[i]);
        separated = result1.max < result2.min || result2.max < result1.min;
    }

    if (!separated)
    {
        for (size_t i = 0; i < normalsHullB.size() && !separated; i++)
        {
            auto result1 = getMinMax(mHullA, normalsHullB[i]);
            auto result2 = getMinMax(mHullB, normalsHullB[i]);
            separated = result1.max < result2.min || result2.max < result1.min;
        }
    }

    return !separated;
}

std::vector<glm::vec2> SAT::getNormals(const Hull& hull)
{
    static auto normalFactor = glm::vec2(-1.0, 1.0);

    std::vector<glm::vec2> normals(hull.size());
    for (size_t i = 0; i < hull.size(); i++)
    {
        auto direction = hull[(i + 1) % hull.size()] - hull[i];
        normals[i] = glm::normalize(direction * normalFactor);
    }
    return normals;
}

SAT::MinMaxResult SAT::getMinMax(const Hull& hull, const glm::vec2& axis)
{
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();

    for (size_t i = 0; i < hull.size(); i++)
    {
        auto currentProjection = glm::dot(hull[i], axis);

        //select the maximum projection on axis to corresponding box corners
        if (currentProjection < min)
            min = currentProjection;
        //select the minimum projection on axis to corresponding box corners
        if (currentProjection > max)
            max = currentProjection;
    }

    return { min, max };
}
