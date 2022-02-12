#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    struct phIntersectionPoint;

    // External declarations


    // Class definitions

    struct phIntersectionPoint {
    public:
        Vector3 Point;
        Vector3 Normal;
        float NormalizedDistance;
        float Penetration;
        byte IntersectResult;
    public:
        void Set(const Vector3& point, const Vector3& normal, float normalizedDistance, float penetration, bool result)
        {
            this->Point = point;
            this->Normal = normal;
            this->NormalizedDistance = normalizedDistance;
            this->Penetration = penetration;
            this->IntersectResult = result;
        }
    };
    ASSERT_SIZEOF(phIntersectionPoint, 0x24);


    // Lua initialization

}