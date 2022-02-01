#pragma once
#include <modules\vehicle.h>
#include <modules\phys.h>
#include "carsim.h"
#include "siren.h"

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

    class vehTrailerInstance : public lvlInstance {
    private:
        vehTrailer* Trailer;
        int Variant;
        Vector3 TrailerHitchPosition;
    public:
        inline vehTrailer * getTrailer(void) {
            return this->Trailer;
        }

        inline int getVariant(void) {
            return this->Variant;
        }

        inline void setVariant(int variant) {
            this->Variant = variant;
        }

        inline Vector3 getTrailerHitchPosition(void) {
            return this->TrailerHitchPosition;
        }

        AGE_API void Init(const char* basename, const Vector3& vector, int variant)
        {
            this->setFlag(0xC8);

            bool hasGeometry = false;

            if (this->BeginGeom(basename, "trailer", 0xD))
            {
                lvlInstance::AddGeom(basename, "shadow", 0);
                lvlInstance::AddGeom(basename, "tlight", 0);
                lvlInstance::AddGeom(basename, "twhl0", 5);
                lvlInstance::AddGeom(basename, "twhl1", 4);
                lvlInstance::AddGeom(basename, "twhl2", 0);
                lvlInstance::AddGeom(basename, "twhl3", 0);
                lvlInstance::AddGeom(basename, "trailer_hitch", 0);

                //NEW MM2HOOK OBJECTS
                lvlInstance::AddGeom(basename, "rlight", 0);
                lvlInstance::AddGeom(basename, "blight", 0);
                lvlInstance::AddGeom(basename, "hlight", 0);
                lvlInstance::AddGeom(basename, "slight0", 0);
                lvlInstance::AddGeom(basename, "slight1", 0);
                lvlInstance::AddGeom(basename, "siren0", 0);
                lvlInstance::AddGeom(basename, "siren1", 0);

                lvlInstance::AddGeom(basename, "twhl4", 0);
                lvlInstance::AddGeom(basename, "twhl5", 0);

                lvlInstance::AddGeom(basename, "tswhl0", 0);
                lvlInstance::AddGeom(basename, "tswhl1", 0);
                lvlInstance::AddGeom(basename, "tswhl2", 0);
                lvlInstance::AddGeom(basename, "tswhl3", 0);
                lvlInstance::AddGeom(basename, "tswhl4", 0);
                lvlInstance::AddGeom(basename, "tswhl5", 0);

                lvlInstance::AddGeom(basename, "tslight0", 0);
                lvlInstance::AddGeom(basename, "tslight1", 0);

                this->EndGeom();
                hasGeometry = true;
            }

            //clamp variant value
            int shadersPerVariant = 1;
            if (this->getGeomSetId() != 0)
                shadersPerVariant = lvlInstance::GetGeomTableEntry(this->getGeomSetId())->numShadersPerPaintjob;
            this->Variant = variant % shadersPerVariant;

            //get our geometry id
            int geomSetId = this->getGeomSetId();
            int geomSetIdOffset = geomSetId - 1;

            //pre-load our variant
            this->PreLoadShader(this->Variant);

            //optimize this instance
            if (hasGeometry)
                this->Optimize(this->Variant);

            //load trailer hitch offset
            if (this->getGeomSetId() != 0)
            {
                auto hitchEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 7);
                if (hitchEntry->getHighLOD() != nullptr)
                {
                    Matrix34 outMatrix;
                    GetPivot(outMatrix, basename, "trailer_hitch");
                    this->TrailerHitchPosition = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }
        }

        /*
            lvlInstance virtuals
        */

        virtual AGE_API Vector3 const & GetPosition() override                         { return hook::Thunk<0x4D7810>::Call<Vector3 const&>(this); }
        virtual AGE_API Matrix34 const & GetMatrix(Matrix34 *a1) override              { return hook::Thunk<0x4D77F0>::Call<Matrix34 const&>(this, a1); }
        virtual AGE_API void SetMatrix(const Matrix34 *a1) override                    { hook::Thunk<0x4D77D0>::Call<void>(this, a1); }
        virtual AGE_API dgPhysEntity * GetEntity() override                            { return hook::Thunk<0x4D7820>::Call<dgPhysEntity*>(this); };
        virtual AGE_API dgPhysEntity * AttachEntity() override                         { return hook::Thunk<0x4D7830>::Call<dgPhysEntity*>(this); };
        virtual AGE_API Vector3 const & GetVelocity() override                         { return hook::Thunk<0x4D7840>::Call<const Vector3 &>(this); }
        virtual AGE_API void Draw(int lod) override                                    { hook::Thunk<0x4D7F20>::Call<void>(this, lod); }
        virtual AGE_API void DrawShadow() override                                     { hook::Thunk<0x4D81F0>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap() override                                  { hook::Thunk<0x4D8320>::Call<void>(this); }
        virtual AGE_API unsigned int SizeOf() override                                 { return hook::Thunk<0x4D7850>::Call<int>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailerInstance, lvlInstance>("vehTrailerInstance")
                //properties
                .addProperty("Trailer", &getTrailer)
                .addProperty("TrailerHitchPosition", &getTrailerHitchPosition)
            .endClass();
        }
    };
    ASSERT_SIZEOF(vehTrailerInstance, 0x28);

    class vehTrailer : public dgPhysEntity, public asNode {
    private:
        dgTrailerJoint TrailerJoint;
        vehTrailerInstance TrailerInstance;
        phInertialCS ICS;
        float Mass;
        Vector3 InertiaBox;
        Vector3 Center;
        Vector3 CarHitchOffset;
        Vector3 TrailerHitchOffset;
        vehDrivetrain DrivetrainFL;
        vehDrivetrain DrivetrainFR;
        vehDrivetrain DrivetrainBL;
        vehDrivetrain DrivetrainBR;
        vehWheel WheelFL;
        vehWheel WheelFR;
        vehWheel WheelBL;
        vehWheel WheelBR;
        lvlTrackManager TrackFL;
        lvlTrackManager TrackFR;
        lvlTrackManager TrackBL;
        lvlTrackManager TrackBR;
        //EXTRA FIELDS. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        int VehType;
        vehSiren Siren;
    public:
        inline dgTrailerJoint * getTrailerJoint(void) {
            return &this->TrailerJoint;
        }

        inline vehTrailerInstance * getTrailerInstance(void) {
            return &this->TrailerInstance;
        }

        inline phInertialCS * getICS(void) {
            return &this->ICS;
        }

        inline float getMass(void) {
            return this->Mass;
        }

        inline void setMass(float mass) {
            this->Mass = mass;
        }

        inline void setAndApplyMass(float mass) {
            this->setMass(mass);

            auto box = this->getInertiaBox();
            this->getICS()->InitBoxMass(this->getMass(), box.X, box.Y, box.Z);
        }

        inline Vector3 getInertiaBox(void) {
            return this->InertiaBox;
        }

        inline void setInertiaBox(Vector3 inertiaBox) {
            this->InertiaBox = inertiaBox;
        }

        inline void setAndApplyInertiaBox(Vector3 inertiaBox) {
            this->setInertiaBox(inertiaBox);

            auto box = this->getInertiaBox();
            this->getICS()->InitBoxMass(this->getMass(), box.X, box.Y, box.Z);
        }

        inline Vector3 getCenter(void) {
            return this->Center;
        }

        inline Vector3 getCarHitchOffset(void) {
            return this->CarHitchOffset;
        }

        inline Vector3 getTrailerHitchOffset(void) {
            return this->TrailerHitchOffset;
        }

        inline vehDrivetrain * getDrivetrain(int num) {
            switch (num) {
            case 0:
                return &this->DrivetrainFL;
            case 1:
                return &this->DrivetrainFR;
            case 2:
                return &this->DrivetrainBL;
            case 3:
                return &this->DrivetrainBR;
            }
            return nullptr;
        }

        inline vehWheel * getWheel(int num) {
            switch (num) {
            case 0:
                return &this->WheelFL;
            case 1:
                return &this->WheelFR;
            case 2:
                return &this->WheelBL;
            case 3:
                return &this->WheelBR;
            }
            return nullptr;
        }

        inline lvlTrackManager * getTrackManager(int num) {
            switch (num) {
            case 0:
                return &this->TrackFL;
            case 1:
                return &this->TrackFR;
            case 2:
                return &this->TrackBL;
            case 3:
                return &this->TrackBR;
            }
            return nullptr;
        }

        inline int getVehType() {
            return this->VehType;
        }

        inline void setVehType(int type) {
            this->VehType = type;
        }

        inline vehSiren * getSiren() {
            return &this->Siren;
        }

        AGE_API vehTrailer()                                            { hook::Thunk<0x4D6F40>::Call<void>(this); }
        AGE_API ~vehTrailer()                                           { hook::Thunk<0x4D71A0>::Call<void>(this); }

        AGE_API void Init(const char* basename, const Vector3* a2, vehCarSim* a3, int a4)
        {
            hook::Thunk<0x4D72F0>::Call<void>(this, basename, a2, a3, a4);

            Matrix34 diffMatrix;

            if (GetPivot(diffMatrix, basename, "trailer_twhl4")) {
                this->getTrailerJoint()->getCarSim()->TrailerBackBackLeftWheelPosDiff.X = diffMatrix.m30 - getWheel(2)->getCenter().X;
                this->getTrailerJoint()->getCarSim()->TrailerBackBackLeftWheelPosDiff.Y = diffMatrix.m31 - getWheel(2)->getCenter().Y;
                this->getTrailerJoint()->getCarSim()->TrailerBackBackLeftWheelPosDiff.Z = diffMatrix.m32 - getWheel(2)->getCenter().Z;
            }

            if (GetPivot(diffMatrix, basename, "trailer_twhl5")) {
                this->getTrailerJoint()->getCarSim()->TrailerBackBackRightWheelPosDiff.X = diffMatrix.m30 - getWheel(3)->getCenter().X;
                this->getTrailerJoint()->getCarSim()->TrailerBackBackRightWheelPosDiff.Y = diffMatrix.m31 - getWheel(3)->getCenter().Y;
                this->getTrailerJoint()->getCarSim()->TrailerBackBackRightWheelPosDiff.Z = diffMatrix.m32 - getWheel(3)->getCenter().Z;
            }
        }

        AGE_API int BottomedOut()                                       { return hook::Thunk<0x4D7980>::Call<int>(this); }

        /*
            dgPhysEntity virtuals
        */

        virtual AGE_API bool RequiresTerrainCollision() override        { return hook::Thunk<0x4D7870>::Call<bool>(this); }
        virtual AGE_API void Update() override                          { hook::Thunk<0x4D7B00>::Call<void>(this); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Reset() override                           { hook::Thunk<0x4D79C0>::Call<void>(this); }
        virtual AGE_API void FileIO(datParser &parser) override         { hook::Thunk<0x4D7CB0>::Call<void>(this, &parser); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailer, dgPhysEntity>("vehTrailer")
                .addProperty("Mass", &getMass, &setAndApplyMass)
                .addProperty("InertiaBox", &getInertiaBox, &setAndApplyInertiaBox)
                .addFunction("GetDrivetrain", &getDrivetrain)
                .addFunction("GetWheel", &getWheel)
                .addFunction("GetTrackManager", &getTrackManager)
                .addFunction("BottomedOut", &BottomedOut)
                .endClass();
        }
    };
    ASSERT_SIZEOF(vehTrailer, 0x1038 + 0x4 + 0x164); //+2 extra fields

    // Lua initialization

}