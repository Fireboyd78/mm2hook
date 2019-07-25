#pragma once
#include <modules\vehicle.h>
#include <modules\phys.h>
#include "carsim.h"

namespace MM2
{
    // Forward declarations
    class vehTrailer;
    class vehTrailerInstance;

    // External declarations
    extern class asNode;
    extern class dgPhysEntity;
    extern class dgTrailerJoint;

    // Class definitions

    class vehTrailer : public dgPhysEntity, public asNode {
    protected:
        ageHook::Field<0x1E4, vehCarSim *> _sim;
        ageHook::Field<0xCC, dgTrailerJoint> _joint;
    public:
        inline vehCarSim* getCarSim(void) const {
            return _sim.get(this);
        }

        inline dgTrailerJoint * getJoint(void) const {
            return _joint.ptr(this);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailer, dgPhysEntity>("vehTrailer")
                .addPropertyReadOnly("vehCarSim", &getCarSim)
                .addPropertyReadOnly("Joint", &getJoint)
            .endClass();
        }
    };

    class vehTrailerInstance : public lvlInstance {
    protected:
        ageHook::Field<0x14, vehTrailer *> _trailer;
    public:
        inline vehTrailer * getTrailer(void) const {
            return _trailer.get(this);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailerInstance, lvlInstance>("vehTrailerInstance")
                //properties
                .addPropertyReadOnly("Trailer", &getTrailer)
            .endClass();
        }
    };

    // Lua initialization

}