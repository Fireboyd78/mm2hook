#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehWeelCheap;

    // External declarations


    // Class definitions
    class vehWeelCheap : public asNode {
    private:
        phInertialCS* InertialCS;
        float Spring;
        float Damping;
        lvlSegment Segment;
        lvlIntersection Intersection;
        lvlSegmentInfo SegmentInfo;
        BOOL Grounded;
        Vector3 Position;
        float Radius;
        float Gap;
        float field_10C;
        float field_110;
        float field_114;
        float RubberSpring;
        float RubberDamp;
        float Limit;
        float field_124;
        Matrix34 Matrix;
        Matrix34 Matrix2;
    public:
        inline Matrix34 getMatrix()
        {
            return this->Matrix;
        }
    };
    ASSERT_SIZEOF(vehWeelCheap, 0x188);

    // Lua initialization

}