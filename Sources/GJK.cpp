#include "GJK.hpp"

using namespace glm;

namespace
{
    vec2 trippleProd(const vec2& a, const vec2& b, const vec2& c)
    {
        return b * dot(a, c) - a * dot(b, c);
    }

    //std::vector<vec2> createConvexEnvelope(const std::vector<vec2>& points)
    //{
    //    vec2 p0;

    //    // A utility function to find next to top in a stack 
    //    Point nextToTop(stack<Point> & S)
    //    {
    //        Point p = S.top();
    //        S.pop();
    //        Point res = S.top();
    //        S.push(p);
    //        return res;
    //    }

    //    // A utility function to swap two points 
    //    int swap(Point & p1, Point & p2)
    //    {
    //        Point temp = p1;
    //        p1 = p2;
    //        p2 = temp;
    //    }

    //    // A utility function to return square of distance 
    //    // between p1 and p2 
    //    int distSq(Point p1, Point p2)
    //    {
    //        return (p1.x - p2.x) * (p1.x - p2.x) +
    //            (p1.y - p2.y) * (p1.y - p2.y);
    //    }

    //    // To find orientation of ordered triplet (p, q, r). 
    //    // The function returns following values 
    //    // 0 --> p, q and r are colinear 
    //    // 1 --> Clockwise 
    //    // 2 --> Counterclockwise 
    //    int orientation(Point p, Point q, Point r)
    //    {
    //        int val = (q.y - p.y) * (r.x - q.x) -
    //            (q.x - p.x) * (r.y - q.y);

    //        if (val == 0) return 0;  // colinear 
    //        return (val > 0) ? 1 : 2; // clock or counterclock wise 
    //    }

    //    // A function used by library function qsort() to sort an array of 
    //    // points with respect to the first point 
    //    int compare(const void* vp1, const void* vp2)
    //    {
    //        Point* p1 = (Point*)vp1;
    //        Point* p2 = (Point*)vp2;

    //        // Find orientation 
    //        int o = orientation(p0, *p1, *p2);
    //        if (o == 0)
    //            return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;

    //        return (o == 2) ? -1 : 1;
    //    }
    //}
}

GJK::GJK(const Hull& a, const Hull& b)
    : mHullA(a)
    , mHullB(b)
{
}

GJK::operator bool()
{
    vec2 direction = { 1, 0 }; // TODO pick better direction
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

DrawStack GJKVisualizer::simulate()
{
    DrawStack simulationStack;



}
