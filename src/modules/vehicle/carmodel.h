#pragma once
#include <modules\vehicle.h>
#include <mm2_gfx.h>
#include <mm2_effects.h>
#include <mm2_city.h>

#include "car.h"
#include "breakable.h"
#include "siren.h"
#include "cardamage.h"

namespace MM2
{
    // Forward declarations
    class vehCarModel;

    // External declarations
    extern class dgPhysEntity;
    extern class lvlInstance;
    extern struct fxTexelDamage;
    extern class vehSiren;
    extern class vehCarDamage;

    // Class definitions

    class vehCarModel : public lvlInstance {
    public:
        static bool EnableSpinningWheels;
        static bool EnableHeadlightFlashing;
        static bool EnableLEDSiren;
        static bool MWStyleTotaledCar;
        static int SirenType;
        static int HeadlightType;
        static float SirenCycle;
        static float HeadlightFlashingSpeed;
        static bool PartReflections;
        static bool WheelReflections;
        static bool MM1StyleTransmission;
        static bool BreakableRenderTweak;
        static bool WheelWobble;
        static bool MM1StyleWobble;
    private:
        bool enabledElectrics[4]; //0/1 are back, 2/3 are front
        vehCar* car;
        vehCarSim* carSim;
        fxTexelDamage* texelDamage;
        byte variant;
        byte char_25;
        byte char_26;
        byte char_27;
        int dword_28;
        float dummyWheelOffset;
        Vector3 fndr0offset;
        Vector3 fndr1offset;
        Vector3 trailerHitchPosition;
        int dword_54;
        int dword_58;
        int dword_5c;
        int dword_60;
        int dword_64;
        int dword_68;
        int dword_6c;
        int dword_70;
        int dword_74;
        int dword_78;
        int dword_7c;
        int dword_80;
        int dword_84;
        int dword_88;
        int dword_8c;
        Vector3 weirdOffsetPos;
        int dword_9c;
        vehBreakableMgr* genBreakableMgr;
        int dword_a4;
        int wheelBrokenStatus;
        vehBreakableMgr* wheelBreakableMgr;
        ltLight* headlights;
        Vector3 headlightPositions[2];
        //EXTRA FIELDS. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        //These are the position differences from (FNDR2-WHL2) and (FNDR3-WHL3) / (FNDR4-WHL4) and (FNDR5-WHL5)
        Vector3 fndr2offset;
        Vector3 fndr3offset;
        Vector3 fndr4offset;
        Vector3 fndr5offset;
        ltLight* extraHeadlights; //HEADLIGHT2-3
        Vector3 extraHeadlightPositions[2];
        ltLight* foglights; //FOGLIGHT0-3
        Vector3 foglightPositions[4];
        bool HeadlightsState;
        bool HazardLightsState;
        bool LeftSignalLightState;
        bool RightSignalLightState;
        bool FoglightsState;
    public:
        AGE_API vehCarModel() {
            scoped_vtable x(this);
            hook::Thunk<0x4CCF20>::Call<void>(this);
        }

        AGE_API ~vehCarModel() {
            scoped_vtable x(this);
            hook::Thunk<0x4CCF800>::Call<void>(this);
        }

        inline vehBreakableMgr * getGenBreakableMgr(void) const {
            return this->genBreakableMgr;
        }
        
        inline vehBreakableMgr * getMechBreakableMgr(void) const {
            return this->wheelBreakableMgr;
        }

        inline vehCar * getCar(void) const {
            return this->car;
        }

        inline vehCarSim * getCarSim(void) const {
            return this->carSim;
        }

        inline int getVariant(void) const {
            return this->variant;
        }

        inline void setVariant(int variant) {
            this->PreLoadShader(variant);
            this->variant = variant;
        }

        inline ltLight* getHeadlight(int index) {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 2)
                index = 1;
            
            return &this->headlights[index];
        }

        inline int getWheelBrokenStatus(void) const {
            return this->wheelBrokenStatus;
        }

        inline bool getEnabledElectrics(int num)
        {
            return this->enabledElectrics[num];
        }

        inline Vector3 getHeadlightPosition(int num)
        {
            return this->headlightPositions[num];
        }

        inline ltLight* getExtraHeadlight(int index) {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 2)
                index = 1;

            return &this->extraHeadlights[index];
        }

        inline Vector3 getExtraHeadlightPosition(int num)
        {
            return this->extraHeadlightPositions[num];
        }

        inline ltLight* getFoglight(int index)
        {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 4)
                index = 3;

            return &this->foglights[index];
        }

        inline Vector3 getFoglightPosition(int num)
        {
            return this->foglightPositions[num];
        }

        inline bool getHeadlightsState()
        {
            return this->HeadlightsState;
        }

        inline void setHeadlightsState(bool state)
        {
            this->HeadlightsState = state;
        }

        inline bool getHazardLightsState()
        {
            return this->HazardLightsState;
        }

        inline void setHazardLightsState(bool state)
        {
            this->HazardLightsState = state;
        }

        inline bool getLeftSignalLightState()
        {
            return this->LeftSignalLightState;
        }

        inline void setLeftSignalLightState(bool state)
        {
            this->LeftSignalLightState = state;
        }

        inline bool getRightSignalLightState()
        {
            return this->RightSignalLightState;
        }

        inline void setRightSignalLightState(bool state)
        {
            this->RightSignalLightState = state;
        }

        inline bool getFoglightsState()
        {
            return this->FoglightsState;
        }

        inline void setFoglightsState(bool state)
        {
            this->FoglightsState = state;
        }

        AGE_API void GetSurfaceColor(modStatic* model, Vector3* outVector)
                                                            { hook::Thunk<0x4CDF00>::Call<void>(this, model, outVector); }
        AGE_API void InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int geomId, int someId)
                                                            { hook::Thunk<0x4CDC50>::Call<void>(this, manager, basename, breakableName, geomId, someId); }
        AGE_API void InitSirenLight(const char* basename, const char* mtxName, int geomId)
        {
            if (this->GetGeomIndex() != 0)
            {
                auto sirenEntry = lvlInstance::GetGeomTableEntry((this->GetGeomIndex() - 1) + geomId);

                if (sirenEntry->GetHighLOD() != nullptr)
                {
                    auto siren = this->car->getSiren();
                    Matrix34 outMatrix;

                    GetPivot(outMatrix, basename, mtxName);
                    this->GetSurfaceColor(sirenEntry->GetHighLOD(), &siren->getLight(siren->getLightCount())->Color);
                    siren->AddLight(Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32), siren->getLight(siren->getLightCount())->Color);
                }
            }
        }

        AGE_API void BreakElectrics(Vector3* a1)            { hook::Thunk<0x4CEFE0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { hook::Thunk<0x4CDFF0>::Call<void>(this); }
        AGE_API void EjectOneshot()
        {
            auto impactPos = car->getCarDamage()->getLastImpactPos();

            if (!(byte)this->dword_a4) {
                if (this->carSim->getSpeedMPH() > 100.f) {
                    this->wheelBreakableMgr->EjectAll(this->GetRoomId());
                    this->wheelBrokenStatus = 0;
                    this->dword_a4 = 1;
                    return;
                }
                if (this->carSim->getSpeedMPH() <= 75.f) {
                    if (this->carSim->getSpeedMPH() <= 50.f)
                        return;

                    int id = -1;

                    float maxDist = 1.8f;

                    for (int i = 0; i < 4; i++)
                    {
                        auto wheelPos = this->carSim->getWheel(i)->getCenter();

                        float dist = wheelPos.Dist(impactPos);

                        if (dist < maxDist)
                        {
                            maxDist = dist;
                            id = i;
                        }
                    }

                    vehCarModel::EjectWheelsAndParts(id);

                    if (id == 2)
                        vehCarModel::EjectWheelsAndParts(4);

                    if (id == 3)
                        vehCarModel::EjectWheelsAndParts(5);

                    this->dword_a4 = 1;
                    return;
                }
                else {
                    float whl0MaxDist = 1.8f;
                    auto whl0Pos = this->carSim->getWheel(0)->getCenter();
                    float whl0Dist = whl0Pos.Dist(impactPos);

                    if (whl0Dist < whl0MaxDist)
                    {
                        whl0MaxDist = whl0Dist;
                        vehCarModel::EjectWheelsAndParts(0);
                    }

                    float whl1MaxDist = 1.8f;
                    auto whl1Pos = this->carSim->getWheel(1)->getCenter();
                    float whl1Dist = whl1Pos.Dist(impactPos);

                    if (whl1Dist < whl1MaxDist)
                    {
                        whl1MaxDist = whl1Dist;
                        vehCarModel::EjectWheelsAndParts(1);
                    }

                    float whl2MaxDist = 1.8f;
                    auto whl2Pos = this->carSim->getWheel(2)->getCenter();
                    float whl2Dist = whl2Pos.Dist(impactPos);

                    if (whl2Dist < whl2MaxDist)
                    {
                        whl2MaxDist = whl2Dist;
                        vehCarModel::EjectWheelsAndParts(2);
                        vehCarModel::EjectWheelsAndParts(4);
                    }

                    float whl3MaxDist = 1.8f;
                    auto whl3Pos = this->carSim->getWheel(3)->getCenter();
                    float whl3Dist = whl3Pos.Dist(impactPos);

                    if (whl3Dist < whl3MaxDist)
                    {
                        whl3MaxDist = whl3Dist;
                        vehCarModel::EjectWheelsAndParts(3);
                        vehCarModel::EjectWheelsAndParts(5);
                    }

                    this->dword_a4 = 1;
                    return;
                }
            }
        }

        // Custom addition
        void EjectWheelsAndParts(int id)
        {
            if (id < 0 || id > 5)
                return;

            int wheelStatusFlag = 1 << (id * 3);
            int hubStatusFlag = 1 << ((id * 3) + 1);
            int fenderStatusFlag = 1 << ((id * 3) + 2);

            this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(wheelStatusFlag), this->GetRoomId());
            this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(hubStatusFlag), this->GetRoomId());
            this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(fenderStatusFlag), this->GetRoomId());

            this->wheelBrokenStatus &= ~(wheelStatusFlag | hubStatusFlag | fenderStatusFlag);
        }

        AGE_API bool GetVisible()                           { return hook::Thunk<0x4CF070>::Call<bool>(this); }
        AGE_API void SetVisible(bool a1)                    { hook::Thunk<0x4CF050>::Call<void>(this, a1); }
        AGE_API void DrawHeadlights(bool rotate)            { hook::Thunk<0x4CED50>::Call<void>(this, rotate); }
        
        AGE_API void DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders)
                                                            { hook::Thunk<0x4CE880>::Call<void>(this, model, &matrix, shaders); }

        AGE_API void DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders)
        {
            if (this->GetGeomIndex() == 0)
                return;

            auto model = lvlInstance::GetGeomTableEntry(this->GetGeomIndex() + geomId - 1)->GetLOD(lod);
            if (model != nullptr)
                DrawPart(model, matrix, shaders);
        }

        void DrawPartReflected(int lod, int geomId, const Matrix34& matrix, modShader* shaders)
        {
            if (this->GetGeomIndex() == 0)
                return;

            auto model = lvlInstance::GetGeomTableEntry(this->GetGeomIndex() + geomId - 1)->GetLOD(lod);
            if (model != nullptr) {
                DrawPart(model, matrix, shaders);

                float reflectionIntensity = 1.f;
                auto reflectionMap = lvlLevel::Singleton->GetEnvMap(this->GetRoomId(), this->GetPosition(), &reflectionIntensity);
                if (reflectionMap != nullptr)
                {
                    modShader::BeginEnvMap(reflectionMap, matrix);
                    model->DrawEnvMapped(shaders, reflectionMap, reflectionIntensity);
                    modShader::EndEnvMap();
                }
            }
        }
                                                            
        void DrawPart(int lod, int geomId, const Matrix34& matrix, modShader* shaders, bool reflected)
        {
            if (reflected && !(lvlLevel::Singleton->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean)))
                DrawPartReflected(lod, geomId, matrix, shaders);
            else
                DrawPart(lod, geomId, matrix, shaders);
        }

        //new init
        void Init(vehCar* car, const char* basename, int variant) 
        {
            this->car = car;
            this->carSim = car->getCarSim();
            this->variant = (variant > 255) ? 0 : variant;
            bool hasGeometry = false;

            if (lvlInstance::BeginGeomWithGroup(basename, "body", "player", 0xC)) 
            {
                gfxForceLVERTEX = true;
                lvlInstance::AddGeom(basename, "shadow", 0);
                lvlInstance::AddGeom(basename, "hlight", 0);
                lvlInstance::AddGeom(basename, "tlight", 0);
                lvlInstance::AddGeom(basename, "rlight", 0);
                lvlInstance::AddGeom(basename, "slight0", 0);
                lvlInstance::AddGeom(basename, "slight1", 0);
                lvlInstance::AddGeom(basename, "blight", 0);
                gfxForceLVERTEX = false;

                lvlInstance::AddGeom(basename, "bodydamage", 0);
                lvlInstance::AddGeom(basename, "siren0", 0);
                lvlInstance::AddGeom(basename, "siren1", 0);
                lvlInstance::AddGeom(basename, "decal", 0);
                lvlInstance::AddGeom(basename, "driver", 0);
                lvlInstance::AddGeom(basename, "shock0", 0);
                lvlInstance::AddGeom(basename, "shock1", 0);
                lvlInstance::AddGeom(basename, "shock2", 0);
                lvlInstance::AddGeom(basename, "shock3", 0);
                lvlInstance::AddGeom(basename, "arm0", 0);
                lvlInstance::AddGeom(basename, "arm1", 0);
                lvlInstance::AddGeom(basename, "arm2", 0);
                lvlInstance::AddGeom(basename, "arm3", 0);
                lvlInstance::AddGeom(basename, "shaft2", 0);
                lvlInstance::AddGeom(basename, "shaft3", 0);
                lvlInstance::AddGeom(basename, "axle0", 0);
                lvlInstance::AddGeom(basename, "axle1", 0);
                lvlInstance::AddGeom(basename, "engine", 2);

                lvlInstance::AddGeom(basename, "whl0", 6);
                lvlInstance::AddGeom(basename, "whl1", 4);
                lvlInstance::AddGeom(basename, "whl2", 4);
                lvlInstance::AddGeom(basename, "whl3", 4);

                lvlInstance::AddGeom(basename, "break0", 2);
                lvlInstance::AddGeom(basename, "break1", 2);
                lvlInstance::AddGeom(basename, "break2", 2);
                lvlInstance::AddGeom(basename, "break3", 2);
                lvlInstance::AddGeom(basename, "break01", 2);
                lvlInstance::AddGeom(basename, "break12", 2);
                lvlInstance::AddGeom(basename, "break23", 2);
                lvlInstance::AddGeom(basename, "break03", 2);

                lvlInstance::AddGeom(basename, "hub0", 0);
                lvlInstance::AddGeom(basename, "hub1", 0);
                lvlInstance::AddGeom(basename, "hub2", 0);
                lvlInstance::AddGeom(basename, "hub3", 0);

                lvlInstance::AddGeom(basename, "trailer_hitch", 0);

                lvlInstance::AddGeom(basename, "srn0", 0);
                lvlInstance::AddGeom(basename, "srn1", 0);
                lvlInstance::AddGeom(basename, "srn2", 0);
                lvlInstance::AddGeom(basename, "srn3", 0);

                gfxForceLVERTEX = true;
                lvlInstance::AddGeom(basename, "headlight0", 0);
                lvlInstance::AddGeom(basename, "headlight1", 0);
                gfxForceLVERTEX = false;

                lvlInstance::AddGeom(basename, "fndr0", 0);
                lvlInstance::AddGeom(basename, "fndr1", 0);
                lvlInstance::AddGeom(basename, "whl4", 0);
                lvlInstance::AddGeom(basename, "whl5", 0);

                //NEW MM2HOOK OBJECTS
                //ADD +8 to any ID after this
                lvlInstance::AddGeom(basename, "plighton", 0);
                lvlInstance::AddGeom(basename, "plightoff", 0);

                lvlInstance::AddGeom(basename, "swhl0", 0);
                lvlInstance::AddGeom(basename, "swhl1", 0);
                lvlInstance::AddGeom(basename, "swhl2", 0);
                lvlInstance::AddGeom(basename, "swhl3", 0);
                lvlInstance::AddGeom(basename, "swhl4", 0);
                lvlInstance::AddGeom(basename, "swhl5", 0);

                lvlInstance::AddGeom(basename, "hub4", 0);
                lvlInstance::AddGeom(basename, "hub5", 0);

                lvlInstance::AddGeom(basename, "fndr2", 0);
                lvlInstance::AddGeom(basename, "fndr3", 0);
                lvlInstance::AddGeom(basename, "fndr4", 0);
                lvlInstance::AddGeom(basename, "fndr5", 0);

                lvlInstance::AddGeom(basename, "shub0", 0);
                lvlInstance::AddGeom(basename, "shub1", 0);
                lvlInstance::AddGeom(basename, "shub2", 0);
                lvlInstance::AddGeom(basename, "shub3", 0);
                lvlInstance::AddGeom(basename, "shub4", 0);
                lvlInstance::AddGeom(basename, "shub5", 0);

                gfxForceLVERTEX = true;
                lvlInstance::AddGeom(basename, "headlight2", 0);
                lvlInstance::AddGeom(basename, "headlight3", 0);

                lvlInstance::AddGeom(basename, "foglight0", 0);
                lvlInstance::AddGeom(basename, "foglight1", 0);
                lvlInstance::AddGeom(basename, "foglight2", 0);
                lvlInstance::AddGeom(basename, "foglight3", 0);
                gfxForceLVERTEX = false;

                lvlInstance::AddGeom(basename, "srn4", 0);
                lvlInstance::AddGeom(basename, "srn5", 0);
                lvlInstance::AddGeom(basename, "srn6", 0);
                lvlInstance::AddGeom(basename, "srn7", 0);
                lvlInstance::AddGeom(basename, "srn8", 0);
                lvlInstance::AddGeom(basename, "srn9", 0);
                lvlInstance::AddGeom(basename, "srn10", 0);
                lvlInstance::AddGeom(basename, "srn11", 0);
                lvlInstance::AddGeom(basename, "srn12", 0);
                lvlInstance::AddGeom(basename, "srn13", 0);
                lvlInstance::AddGeom(basename, "srn14", 0);
                lvlInstance::AddGeom(basename, "srn15", 0);
                lvlInstance::AddGeom(basename, "srn16", 0);
                lvlInstance::AddGeom(basename, "srn17", 0);
                lvlInstance::AddGeom(basename, "srn18", 0);
                lvlInstance::AddGeom(basename, "srn19", 0);
                lvlInstance::AddGeom(basename, "srn20", 0);
                lvlInstance::AddGeom(basename, "srn21", 0);
                lvlInstance::AddGeom(basename, "srn22", 0);
                lvlInstance::AddGeom(basename, "srn23", 0);

                lvlInstance::AddGeom(basename, "lightbar0", 0);
                lvlInstance::AddGeom(basename, "lightbar1", 0);

                gfxForceLVERTEX = true;
                lvlInstance::AddGeom(basename, "tslight0", 0);
                lvlInstance::AddGeom(basename, "tslight1", 0);

                lvlInstance::AddGeom(basename, "flight0", 0);
                lvlInstance::AddGeom(basename, "flight1", 0);
                lvlInstance::AddGeom(basename, "flight2", 0);
                lvlInstance::AddGeom(basename, "flight3", 0);
                gfxForceLVERTEX = false;

                lvlInstance::AddGeom(basename, "breaklt0", 0);
                lvlInstance::AddGeom(basename, "breaklt1", 0);
                lvlInstance::AddGeom(basename, "breaklt2", 0);
                lvlInstance::AddGeom(basename, "breaklt3", 0);

                //add variants
                //supports up to 32 paintjobs
                for (int i = 0; i < 32; i++)
                {
                    string_buf<16> buffer("variant%d", i);
                    lvlInstance::AddGeom(basename, buffer, (i == this->variant) ? 2 : 8);
                }

                lvlInstance::EndGeom();
                hasGeometry = true;
            }

            //clamp variant value
            int shadersPerVariant = 1;
            if (this->GetGeomIndex() != 0)
                shadersPerVariant = lvlInstance::GetGeomTableEntry(this->GetGeomIndex())->numShadersPerPaintjob;
            this->variant = this->variant % shadersPerVariant;

            //get our geometry id
            int geomSetId = this->GetGeomIndex();
            int geomSetIdOffset = geomSetId - 1;

            //pre-load our variant
            lvlInstance::PreLoadShader(this->variant);

            //init fxTexelDamage
            if (this->GetGeomIndex() != 0)
            {
                auto bodyEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset);
                if (bodyEntry->GetHighLOD() != nullptr)
                {
                    this->texelDamage = new fxTexelDamage();

                    if (!texelDamage->Init(bodyEntry->GetHighLOD(), bodyEntry->pShaders[this->getVariant()], bodyEntry->numShaders)) 
                    {
                        delete texelDamage;
                        texelDamage = nullptr;
                        Warningf("Vehicle '%s': No damage textures found, disabling texel damage.", basename);
                    }
                }
            }

            //optimize this instance
            if (hasGeometry)
                lvlInstance::Optimize(this->variant);

            //init siren lights
            auto siren = this->car->getSiren();
            if (siren != nullptr) 
            {
                siren->Init();
                InitSirenLight(basename, "srn0", 43);
                InitSirenLight(basename, "srn1", 44);
                InitSirenLight(basename, "srn2", 45);
                InitSirenLight(basename, "srn3", 46);
                InitSirenLight(basename, "srn4", 79);
                InitSirenLight(basename, "srn5", 80);
                InitSirenLight(basename, "srn6", 81);
                InitSirenLight(basename, "srn7", 82);
                InitSirenLight(basename, "srn8", 83);
                InitSirenLight(basename, "srn9", 84);
                InitSirenLight(basename, "srn10", 85);
                InitSirenLight(basename, "srn11", 86);
                InitSirenLight(basename, "srn12", 87);
                InitSirenLight(basename, "srn13", 88);
                InitSirenLight(basename, "srn14", 89);
                InitSirenLight(basename, "srn15", 90);
                InitSirenLight(basename, "srn16", 91);
                InitSirenLight(basename, "srn17", 92);
                InitSirenLight(basename, "srn18", 93);
                InitSirenLight(basename, "srn19", 94);
                InitSirenLight(basename, "srn20", 95);
                InitSirenLight(basename, "srn21", 96);
                InitSirenLight(basename, "srn22", 97);
                InitSirenLight(basename, "srn23", 98);
            }

            //load headlights
            if (this->GetGeomIndex() != 0)
            {
                auto headlight0entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 47);
                auto headlight1entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 48);

                if (headlight0entry->GetHighLOD() != nullptr && headlight1entry->GetHighLOD() != nullptr)
                {
                    this->headlights = new ltLight[2];
                    Matrix34 outMatrix;

                    GetPivot(outMatrix, basename, "headlight0");
                    headlights[0].Color = Vector3(1.f, 1.f, 1.f);
                    headlights[0].Type = 1;
                    headlights[0].SpotExponent = 3.f;
                    this->GetSurfaceColor(headlight0entry->GetHighLOD(), &headlights[0].Color);
                    this->headlightPositions[0] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);

                    GetPivot(outMatrix, basename, "headlight1");
                    headlights[1].Color = Vector3(1.f, 1.f, 1.f);
                    headlights[1].Type = 1;
                    headlights[1].SpotExponent = 3.f;
                    this->GetSurfaceColor(headlight1entry->GetHighLOD(), &headlights[1].Color);
                    this->headlightPositions[1] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }

            //load extra headlights
            if (this->GetGeomIndex() != 0)
            {
                auto headlight2entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 73);
                auto headlight3entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 74);

                if (headlight2entry->GetHighLOD() != nullptr && headlight3entry->GetHighLOD() != nullptr)
                {
                    this->extraHeadlights = new ltLight[2];
                    Matrix34 outMatrix;

                    GetPivot(outMatrix, basename, "headlight2");
                    extraHeadlights[0].Color = Vector3(1.f, 1.f, 1.f);
                    extraHeadlights[0].Type = 1;
                    extraHeadlights[0].SpotExponent = 3.f;
                    this->GetSurfaceColor(headlight2entry->GetHighLOD(), &extraHeadlights[0].Color);
                    this->extraHeadlightPositions[0] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);

                    GetPivot(outMatrix, basename, "headlight3");
                    extraHeadlights[1].Color = Vector3(1.f, 1.f, 1.f);
                    extraHeadlights[1].Type = 1;
                    extraHeadlights[1].SpotExponent = 3.f;
                    this->GetSurfaceColor(headlight3entry->GetHighLOD(), &extraHeadlights[1].Color);
                    this->extraHeadlightPositions[1] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }

            //load foglights
            if (this->GetGeomIndex() != 0)
            {
                auto foglight0Entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 75);
                auto foglight1Entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 76);
                auto foglight2Entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 77);
                auto foglight3Entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 78);

                if (foglight0Entry->GetHighLOD() != nullptr)
                {
                    foglights = new ltLight[4];
                    Matrix34 outMatrix;

                    GetPivot(outMatrix, basename, "foglight0");
                    foglights[0].Color = Vector3(1.f, 1.f, 1.f);
                    foglights[0].Type = 1;
                    foglights[0].SpotExponent = 3.f;
                    this->GetSurfaceColor(foglight0Entry->GetHighLOD(), &foglights[0].Color);
                    this->foglightPositions[0] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);

                    GetPivot(outMatrix, basename, "foglight1");
                    foglights[1].Color = Vector3(1.f, 1.f, 1.f);
                    foglights[1].Type = 1;
                    foglights[1].SpotExponent = 3.f;
                    this->GetSurfaceColor(foglight1Entry->GetHighLOD(), &foglights[1].Color);
                    this->foglightPositions[1] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);

                    GetPivot(outMatrix, basename, "foglight2");
                    foglights[2].Color = Vector3(1.f, 1.f, 1.f);
                    foglights[2].Type = 1;
                    foglights[2].SpotExponent = 3.f;
                    this->GetSurfaceColor(foglight2Entry->GetHighLOD(), &foglights[2].Color);
                    this->foglightPositions[2] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);

                    GetPivot(outMatrix, basename, "foglight3");
                    foglights[3].Color = Vector3(1.f, 1.f, 1.f);
                    foglights[3].Type = 1;
                    foglights[3].SpotExponent = 3.f;
                    this->GetSurfaceColor(foglight3Entry->GetHighLOD(), &foglights[3].Color);
                    this->foglightPositions[3] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }

            //load FNDR offsets
            if (this->GetGeomIndex() != 0)
            {
                auto fndrEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 49);
                if (fndrEntry->GetHighLOD() != nullptr)
                {
                    Matrix34 outMatrix;
                    auto carsim = this->carSim;
                    
                    //note about the 0.025y offset
                    //the game has this weird offset by default
                    //removing it appears to have no functional issues, but visually
                    //mods will be broken if removed because they compensate
                    GetPivot(outMatrix, basename, "fndr0");
                    this->fndr0offset.X = outMatrix.m30 - carsim->getWheel(0)->getCenter().X;
                    this->fndr0offset.Y = (outMatrix.m31 - carsim->getWheel(0)->getCenter().Y) + 0.025f;
                    this->fndr0offset.Z = outMatrix.m32 - carsim->getWheel(0)->getCenter().Z;           

                    GetPivot(outMatrix, basename, "fndr1");
                    this->fndr1offset.X = outMatrix.m30 - carsim->getWheel(1)->getCenter().X;
                    this->fndr1offset.Y = (outMatrix.m31 - carsim->getWheel(1)->getCenter().Y) + 0.025f;
                    this->fndr1offset.Z = outMatrix.m32 - carsim->getWheel(1)->getCenter().Z;
                }

                //extra FNDR2/3 offsets
                auto fndr2Entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 63);
                if (fndr2Entry->GetHighLOD() != nullptr)
                {
                    Matrix34 outMatrix;
                    auto carsim = this->carSim;

                    GetPivot(outMatrix, basename, "fndr2");
                    this->fndr2offset.X = outMatrix.m30 - carsim->getWheel(2)->getCenter().X;
                    this->fndr2offset.Y = outMatrix.m31 - carsim->getWheel(2)->getCenter().Y;
                    this->fndr2offset.Z = outMatrix.m32 - carsim->getWheel(2)->getCenter().Z;

                    GetPivot(outMatrix, basename, "fndr3");
                    this->fndr3offset.X = outMatrix.m30 - carsim->getWheel(3)->getCenter().X;
                    this->fndr3offset.Y = outMatrix.m31 - carsim->getWheel(3)->getCenter().Y;
                    this->fndr3offset.Z = outMatrix.m32 - carsim->getWheel(3)->getCenter().Z;
                }

                //extra FNDR4/5 offsets
                auto fndr4Entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 65);
                if (fndr4Entry->GetHighLOD() != nullptr)
                {
                    Matrix34 outMatrix;
                    auto carsim = this->carSim;

                    GetPivot(outMatrix, basename, "fndr4");
                    this->fndr4offset.X = outMatrix.m30 - carsim->getWheel(2)->getCenter().X;
                    this->fndr4offset.Y = outMatrix.m31 - carsim->getWheel(2)->getCenter().Y;
                    this->fndr4offset.Z = (outMatrix.m32 - carsim->getWheel(2)->getCenter().Z) + 1.10738f;

                    GetPivot(outMatrix, basename, "fndr5");
                    this->fndr5offset.X = outMatrix.m30 - carsim->getWheel(3)->getCenter().X;
                    this->fndr5offset.Y = outMatrix.m31 - carsim->getWheel(3)->getCenter().Y;
                    this->fndr5offset.Z = (outMatrix.m32 - carsim->getWheel(3)->getCenter().Z) + 1.10738f;
                }
            }

            //create gen breakables
            this->genBreakableMgr = new vehBreakableMgr();
            this->genBreakableMgr->Init(this->carSim->getWorldMatrix());
            this->genBreakableMgr->setVariant(this->variant);

            InitBreakable(this->genBreakableMgr, basename, "break0", 30, 0);
            InitBreakable(this->genBreakableMgr, basename, "break1", 31, 0);
            InitBreakable(this->genBreakableMgr, basename, "break2", 32, 0);
            InitBreakable(this->genBreakableMgr, basename, "break3", 33, 0);
            InitBreakable(this->genBreakableMgr, basename, "break01", 34, 0);
            InitBreakable(this->genBreakableMgr, basename, "break12", 35, 0);
            InitBreakable(this->genBreakableMgr, basename, "break23", 36, 0);
            InitBreakable(this->genBreakableMgr, basename, "break03", 37, 0);
            InitBreakable(this->genBreakableMgr, basename, "lightbar0", 99, 1);
            InitBreakable(this->genBreakableMgr, basename, "lightbar1", 100, 2);
            InitBreakable(this->genBreakableMgr, basename, "breaklt0", 107, 3);
            InitBreakable(this->genBreakableMgr, basename, "breaklt1", 108, 4);
            InitBreakable(this->genBreakableMgr, basename, "breaklt2", 109, 5);
            InitBreakable(this->genBreakableMgr, basename, "breaklt3", 110, 6);
            
            int variantGeomId = this->variant + 111;
            string_buf<16> buffer("variant%d", this->variant);
            InitBreakable(this->genBreakableMgr, basename, buffer, variantGeomId, 0);

            //create wheel breakables
            this->wheelBreakableMgr = new vehBreakableMgr();
            this->wheelBreakableMgr->Init(this->carSim->getWorldMatrix());
            this->wheelBreakableMgr->setVariant(this->variant);
            
            InitBreakable(this->wheelBreakableMgr, basename, "whl0", 26, 1 << 0);
            InitBreakable(this->wheelBreakableMgr, basename, "hub0", 38, 1 << 1);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr0", 49, 1 << 2);
            InitBreakable(this->wheelBreakableMgr, basename, "whl1", 27, 1 << 3);
            InitBreakable(this->wheelBreakableMgr, basename, "hub1", 39, 1 << 4);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr1", 50, 1 << 5);
            InitBreakable(this->wheelBreakableMgr, basename, "whl2", 28, 1 << 6);
            InitBreakable(this->wheelBreakableMgr, basename, "hub2", 40, 1 << 7);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr2", 63, 1 << 8);
            InitBreakable(this->wheelBreakableMgr, basename, "whl3", 29, 1 << 9);
            InitBreakable(this->wheelBreakableMgr, basename, "hub3", 41, 1 << 10);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr3", 64, 1 << 11);
            InitBreakable(this->wheelBreakableMgr, basename, "whl4", 51, 1 << 12);
            InitBreakable(this->wheelBreakableMgr, basename, "hub4", 61, 1 << 13);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr4", 65, 1 << 14);
            InitBreakable(this->wheelBreakableMgr, basename, "whl5", 52, 1 << 15);
            InitBreakable(this->wheelBreakableMgr, basename, "hub5", 62, 1 << 16);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr5", 66, 1 << 17);
            InitBreakable(this->wheelBreakableMgr, basename, "engine", 25, 1 << 18);

            //load trailer hitch offset
            if (this->GetGeomIndex() != 0)
            {
                auto hitchEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 42);
                if (hitchEntry->GetHighLOD() != nullptr)
                {
                    Matrix34 outMatrix;
                    GetPivot(outMatrix, basename, "trailer_hitch");
                    this->trailerHitchPosition = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }
        }
        
        AGE_API void InitBound(char const* vehName, bool useGeometry)               { hook::Thunk<0x4CDA20>::Call<void>(this, vehName, useGeometry); }

        // We made this matrix to draw car body with wobble angling
        Matrix34 getCarMatrix()
        {
            if (!vehCarModel::WheelWobble || !this->carSim->OnGround())
                return *this->carSim->getWorldMatrix();

            Matrix34 matrix;
            Vector3 vec;

            memcpy(&matrix, this->carSim->getWorldMatrix(), sizeof(Matrix34));

            float rotation = (fabs(this->carSim->getWheel(1)->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

            float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
            float invRotationAmount = 1.f - rotationAmount;

            vec.X = matrix.m20 + matrix.m00;
            vec.Y = matrix.m21 + matrix.m01;
            vec.Z = matrix.m22 + matrix.m02;

            auto carDamage = this->car->getCarDamage();
            float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

            damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

            float angle = sin(this->carSim->getWheel(1)->getAccumulatedRotation()) * damagePercent * invRotationAmount * 0.02f;

            if (!vehCarModel::MM1StyleWobble)
            {
                auto wheelWobbleLimit = this->carSim->getWheel(1)->getWobbleLimit();
                auto wheelWobbleAmount = this->carSim->getWheel(1)->getWobbleAmount();

                angle = sin(this->carSim->getWheel(1)->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.2f;
            }

            matrix.Rotate(vec, angle);

            return matrix;
        }

        /*
            lvlInstance virtuals
        */

        AGE_API void Reset()  override                      { hook::Thunk<0x4CDFD0>::Call<void>(this); }
        AGE_API const Vector3 & GetPosition() override
                                                            { return hook::Thunk<0x4CEF50>::Call<const Vector3 &>(this); }
        AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            { return hook::Thunk<0x4CEF90>::Call<const Matrix34 &>(this, a1); }
        AGE_API void SetMatrix(const Matrix34 &a1) override 
                                                            { hook::Thunk<0x4CEFA0>::Call<void>(this, &a1); }
        AGE_API dgPhysEntity * GetEntity() override         { return hook::Thunk<0x4CEFC0>::Call<dgPhysEntity*>(this); }
        AGE_API dgPhysEntity * AttachEntity() override 
                                                            { return hook::Thunk<0x4CEFD0>::Call<dgPhysEntity*>(this); }
        AGE_API const Vector3 & GetVelocity() override
                                                            { return hook::Thunk<0x4CEF80>::Call<const Vector3 &>(this); }

        AGE_API void Draw(int lod) override
        {
            if (lod < 0 || lod > 3)
                return;
            if (this->GetGeomIndex() == 0)
                return;

            //get our geometry id
            int geomSetId = this->GetGeomIndex();
            int geomSetIdOffset = geomSetId - 1;

            //get shaders
            auto mainGeomEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset);
            auto shaders = mainGeomEntry->pShaders[this->getVariant()];

            //use texel damage
            if (lod >= 2 && this->texelDamage != nullptr)
            {
                shaders = this->texelDamage->CurrentShaders;
            }

            //vppanozgt hack... use full alpha for paintjob 4
            auto bodyGeomName = lvlInstance::GetGeomName(geomSetIdOffset);
            int oldAlphaRef = (&RSTATE->Data)->AlphaRef;
            if (!strcmp(bodyGeomName, "vppanozgt_body") && this->getVariant() == 4)
            {
                if ((&RSTATE->Data)->AlphaRef != 0)
                {
                    (&RSTATE->Data)->AlphaRef = 0;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }
            }

            //draw BREAK objects above the body
            if (BreakableRenderTweak)
                this->genBreakableMgr->Draw(this->getCarMatrix(), shaders, lod);

            //setup renderer
            Matrix44::Convert(gfxRenderState::sm_World, this->getCarMatrix());
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            //draw the body
            auto bodyModel = mainGeomEntry->GetLOD(lod);
            if (bodyModel != nullptr)
                bodyModel->Draw(shaders);

            //draw BREAK objects below the body
            if (!BreakableRenderTweak)
                this->genBreakableMgr->Draw(this->getCarMatrix(), shaders, lod);

            //draw decal
            auto decalModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 11)->GetLOD(lod);
            if (decalModel != nullptr)
            {
                int oldAlphaRef2 = (&RSTATE->Data)->AlphaRef;
                if ((&RSTATE->Data)->AlphaRef != 0)
                {
                    (&RSTATE->Data)->AlphaRef = 0;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }

                DrawPart(decalModel, this->getCarMatrix(), shaders);

                if ((&RSTATE->Data)->AlphaRef != oldAlphaRef2)
                {
                    (&RSTATE->Data)->AlphaRef = oldAlphaRef2;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }
            }

            //draw reflection (only in H LOD)
            float reflectionIntensity = 1.f;
            auto reflectionMap = lvlLevel::Singleton->GetEnvMap(this->GetRoomId(), this->GetPosition(), &reflectionIntensity);
            if (lod == 3 && reflectionMap != nullptr && bodyModel != nullptr &&
                !(lvlLevel::Singleton->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean)))
            {
                modShader::BeginEnvMap(reflectionMap, this->getCarMatrix());
                bodyModel->DrawEnvMapped(shaders, reflectionMap, reflectionIntensity);
                modShader::EndEnvMap();
            }

            //draw FNDR
            if (lod == 3)
            {
                Matrix34 fndrMatrix = Matrix34();
                auto carMatrix = this->getCarMatrix();
                fndrMatrix.Identity();

                fndrMatrix.m10 = carMatrix.m10;
                fndrMatrix.m11 = carMatrix.m11;
                fndrMatrix.m12 = carMatrix.m12;

                auto fndr0model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 49)->GetHighLOD();
                if (fndr0model != nullptr && (wheelBrokenStatus & 0x4) != 0)
                {
                    auto whlMatrix = this->carSim->getWheel(0)->getMatrix();

                    fndrMatrix.m00 = whlMatrix.m00;
                    fndrMatrix.m01 = whlMatrix.m01;
                    fndrMatrix.m02 = whlMatrix.m02;

                    fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
                    fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
                    fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

                    fndrMatrix.m30 = fndr0offset.Y * fndrMatrix.m10 + fndr0offset.Z * fndrMatrix.m20 + fndr0offset.X * fndrMatrix.m00 + whlMatrix.m30;
                    fndrMatrix.m31 = fndr0offset.Y * fndrMatrix.m11 + fndr0offset.Z * fndrMatrix.m21 + fndr0offset.X * fndrMatrix.m01 + whlMatrix.m31;
                    fndrMatrix.m32 = fndr0offset.Y * fndrMatrix.m12 + fndr0offset.Z * fndrMatrix.m22 + fndr0offset.X * fndrMatrix.m02 + whlMatrix.m32;

                    DrawPart(3, 49, fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                auto fndr1model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 50)->GetHighLOD();
                if (fndr1model != nullptr && (wheelBrokenStatus & 0x20) != 0)
                {
                    auto whlMatrix = this->carSim->getWheel(1)->getMatrix();

                    fndrMatrix.m00 = whlMatrix.m00;
                    fndrMatrix.m01 = whlMatrix.m01;
                    fndrMatrix.m02 = whlMatrix.m02;

                    fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
                    fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
                    fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

                    fndrMatrix.m30 = fndr1offset.Y * fndrMatrix.m10 + fndr1offset.Z * fndrMatrix.m20 + fndr1offset.X * fndrMatrix.m00 + whlMatrix.m30;
                    fndrMatrix.m31 = fndr1offset.Y * fndrMatrix.m11 + fndr1offset.Z * fndrMatrix.m21 + fndr1offset.X * fndrMatrix.m01 + whlMatrix.m31;
                    fndrMatrix.m32 = fndr1offset.Y * fndrMatrix.m12 + fndr1offset.Z * fndrMatrix.m22 + fndr1offset.X * fndrMatrix.m02 + whlMatrix.m32;

                    DrawPart(3, 50, fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                //draw FNDR2/3, we have to draw them here to be rendered over WHL2/3
                auto fndr2model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 63)->GetHighLOD();
                if (fndr2model != nullptr && (wheelBrokenStatus & 0x100) != 0)
                {
                    auto whlMatrix = this->carSim->getWheel(2)->getMatrix();

                    fndrMatrix.m00 = whlMatrix.m00;
                    fndrMatrix.m01 = whlMatrix.m01;
                    fndrMatrix.m02 = whlMatrix.m02;

                    fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
                    fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
                    fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

                    fndrMatrix.m30 = fndr2offset.Y * fndrMatrix.m10 + fndr2offset.Z * fndrMatrix.m20 + fndr2offset.X * fndrMatrix.m00 + whlMatrix.m30;
                    fndrMatrix.m31 = fndr2offset.Y * fndrMatrix.m11 + fndr2offset.Z * fndrMatrix.m21 + fndr2offset.X * fndrMatrix.m01 + whlMatrix.m31;
                    fndrMatrix.m32 = fndr2offset.Y * fndrMatrix.m12 + fndr2offset.Z * fndrMatrix.m22 + fndr2offset.X * fndrMatrix.m02 + whlMatrix.m32;

                    DrawPart(3, 63, fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                auto fndr3model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 64)->GetHighLOD();
                if (fndr3model != nullptr && (wheelBrokenStatus & 0x800) != 0)
                {
                    auto whlMatrix = this->carSim->getWheel(3)->getMatrix();

                    fndrMatrix.m00 = whlMatrix.m00;
                    fndrMatrix.m01 = whlMatrix.m01;
                    fndrMatrix.m02 = whlMatrix.m02;

                    fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
                    fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
                    fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

                    fndrMatrix.m30 = fndr3offset.Y * fndrMatrix.m10 + fndr3offset.Z * fndrMatrix.m20 + fndr3offset.X * fndrMatrix.m00 + whlMatrix.m30;
                    fndrMatrix.m31 = fndr3offset.Y * fndrMatrix.m11 + fndr3offset.Z * fndrMatrix.m21 + fndr3offset.X * fndrMatrix.m01 + whlMatrix.m31;
                    fndrMatrix.m32 = fndr3offset.Y * fndrMatrix.m12 + fndr3offset.Z * fndrMatrix.m22 + fndr3offset.X * fndrMatrix.m02 + whlMatrix.m32;

                    DrawPart(3, 64, fndrMatrix, shaders, vehCarModel::PartReflections);
                }
            }

            //draw wheels and hubs
            if (lod >= 1)
            {
                //draw wheels
                for (int i = 0; i < 4; i++) 
                {
                    auto wheel = this->carSim->getWheel(i);
                    int swhlId = 55 + i;
                    int whlId = 26 + i;
                    int hubId = 38 + i;
                    int shubId = 67 + i;

                    int wheelStatusFlag = 1 << (i * 3);
                    int hubStatusFlag = 1 << ((i * 3) + 1);

                    //hub
                    if ((this->wheelBrokenStatus & hubStatusFlag) != 0)
                    {
                        auto shubModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + shubId)->GetLOD(lod);
                        if (fabs(wheel->getRotationRate()) > 26.f && shubModel != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPart(lod, shubId, wheel->getMatrix(), shaders, vehCarModel::PartReflections);
                        }
                        else {
                            DrawPart(lod, hubId, wheel->getMatrix(), shaders, vehCarModel::PartReflections);
                        }
                    }

                    //wheel
                    if ((this->wheelBrokenStatus & wheelStatusFlag) != 0)
                    {
                        auto swhlModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + swhlId)->GetLOD(lod);
                        if (fabs(wheel->getRotationRate()) > 26.f && swhlModel != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPart(lod, swhlId, wheel->getMatrix(), shaders, vehCarModel::WheelReflections);
                        }
                        else {
                            DrawPart(lod, whlId, wheel->getMatrix(), shaders, vehCarModel::WheelReflections);
                        }
                    }
                }
            }

            //draw plight
            if (lod >= 1)
            {
                if (this->HeadlightsState)
                    //plighton
                    DrawPart(lod, 53, this->getCarMatrix(), shaders, vehCarModel::PartReflections);
                else
                    //plightoff
                    DrawPart(lod, 54, this->getCarMatrix(), shaders, vehCarModel::PartReflections);
            }

            Matrix34 dummyWhl4Matrix = Matrix34();
            Matrix34 dummyWhl5Matrix = Matrix34();

            //draw suspension, engine, extra wheels, extra fenders
            if (lod >= 1)
            {
                //suspension
                for (int i = 0; i < 4; i++)
                {
                    if (vehCarModel::WheelWobble)
                    {
                        Matrix34 matrix;
                        Vector3 vec;

                        memcpy(&matrix, &this->carSim->ShockSuspensions[i].getSuspensionMatrix(), sizeof(Matrix34));

                        float rotation = (fabs(this->carSim->ShockSuspensions[1].getWheel()->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

                        float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
                        float invRotationAmount = 1.f - rotationAmount;

                        vec.X = this->getCarMatrix().m20 + this->getCarMatrix().m00;
                        vec.Y = this->getCarMatrix().m21 + this->getCarMatrix().m01;
                        vec.Z = this->getCarMatrix().m22 + this->getCarMatrix().m02;

                        auto carDamage = this->car->getCarDamage();
                        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

                        damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

                        float angle = sin(this->carSim->ShockSuspensions[1].getWheel()->getAccumulatedRotation()) * damagePercent * invRotationAmount * 0.02f;

                        if (!vehCarModel::MM1StyleWobble)
                        {
                            auto wheelWobbleLimit = this->carSim->ShockSuspensions[1].getWheel()->getWobbleLimit();
                            auto wheelWobbleAmount = this->carSim->ShockSuspensions[1].getWheel()->getWobbleAmount();

                            angle = sin(this->carSim->ShockSuspensions[1].getWheel()->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.2f;
                        }

                        matrix.Set(this->carSim->ShockSuspensions[i].getSuspensionPivot());

                        matrix.m01 += angle;
                        matrix.m11 += angle;
                        matrix.m21 += angle;

                        matrix.Dot(this->getCarMatrix());

                        matrix.Rotate(vec, angle);

                        DrawPart(lod, 13 + i, matrix, shaders, vehCarModel::PartReflections);
                    }
                    else {
                        DrawPart(lod, 13 + i, this->carSim->ShockSuspensions[i].getSuspensionMatrix(), shaders, vehCarModel::PartReflections);
                    }
                }
                for (int i = 0; i < 4; i++)
                {
                    if (vehCarModel::WheelWobble)
                    {
                        Matrix34 matrix;

                        memcpy(&matrix, &this->carSim->ArmSuspensions[i].getSuspensionMatrix(), sizeof(Matrix34));

                        float rotation = (fabs(this->carSim->ArmSuspensions[i].getWheel()->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

                        float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
                        float invRotationAmount = 1.f - rotationAmount;

                        auto carDamage = this->car->getCarDamage();
                        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

                        damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

                        float angle = sin(this->carSim->ArmSuspensions[i].getWheel()->getMatrix().m11) * damagePercent * invRotationAmount * 0.02f;

                        if (!vehCarModel::MM1StyleWobble)
                        {
                            auto wheelWobbleLimit = this->carSim->ArmSuspensions[1].getWheel()->getWobbleLimit();
                            auto wheelWobbleAmount = this->carSim->ArmSuspensions[1].getWheel()->getWobbleAmount();

                            angle = sin(this->carSim->ArmSuspensions[1].getWheel()->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.2f;
                        }

                        matrix.Set(this->carSim->ArmSuspensions[i].getSuspensionPivot());

                        matrix.m01 += angle;
                        matrix.m11 += angle;
                        matrix.m21 += angle;

                        matrix.Dot(this->getCarMatrix());

                        DrawPart(lod, 17 + i, matrix, shaders, vehCarModel::PartReflections);
                    }
                    else {
                        DrawPart(lod, 17 + i, this->carSim->ArmSuspensions[i].getSuspensionMatrix(), shaders, vehCarModel::PartReflections);
                    }
                }
                for (int i = 0; i < 2; i++)
                {
                    if (vehCarModel::WheelWobble)
                    {
                        Matrix34 matrix;
                        Vector3 vec;

                        memcpy(&matrix, &this->carSim->ShaftSuspensions[i].getSuspensionMatrix(), sizeof(Matrix34));

                        float rotation = (fabs(this->carSim->ShaftSuspensions[1].getWheel()->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

                        float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
                        float invRotationAmount = 1.f - rotationAmount;

                        vec.X = this->getCarMatrix().m20 + this->getCarMatrix().m00;
                        vec.Y = this->getCarMatrix().m21 + this->getCarMatrix().m01;
                        vec.Z = this->getCarMatrix().m22 + this->getCarMatrix().m02;

                        auto carDamage = this->car->getCarDamage();
                        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

                        damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

                        float angle = sin(this->carSim->ShaftSuspensions[1].getWheel()->getAccumulatedRotation()) * damagePercent * invRotationAmount * 0.02f;

                        if (!vehCarModel::MM1StyleWobble)
                        {
                            auto wheelWobbleLimit = this->carSim->ShaftSuspensions[1].getWheel()->getWobbleLimit();
                            auto wheelWobbleAmount = this->carSim->ShaftSuspensions[1].getWheel()->getWobbleAmount();

                            angle = sin(this->carSim->ShaftSuspensions[1].getWheel()->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.2f;
                        }

                        matrix.Set(this->carSim->ShaftSuspensions[i].getSuspensionPivot());

                        matrix.m01 += angle;
                        matrix.m11 += angle;
                        matrix.m21 += angle;

                        matrix.Dot(this->getCarMatrix());

                        matrix.Rotate(vec, angle);

                        DrawPart(lod, 21 + i, matrix, shaders, vehCarModel::PartReflections);
                    }
                    else {
                        DrawPart(lod, 21 + i, this->carSim->ShaftSuspensions[i].getSuspensionMatrix(), shaders, vehCarModel::PartReflections);
                    }
                }

                auto axleFrontmodel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 23)->GetLOD(lod);
                if (axleFrontmodel != nullptr)
                {
                    if (vehCarModel::WheelWobble)
                    {
                        Matrix34 matrix;
                        Vector3 vec;

                        memcpy(&matrix, &this->carSim->AxleFront.getAxleMatrix(), sizeof(Matrix34));

                        float rotation = (fabs(this->carSim->AxleFront.getRightWheel()->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

                        float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
                        float invRotationAmount = 1.f - rotationAmount;

                        vec.X = this->getCarMatrix().m20 + this->getCarMatrix().m00;
                        vec.Y = this->getCarMatrix().m21 + this->getCarMatrix().m01;
                        vec.Z = this->getCarMatrix().m22 + this->getCarMatrix().m02;

                        auto carDamage = this->car->getCarDamage();
                        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

                        damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

                        float angle = sin(this->carSim->AxleFront.getRightWheel()->getAccumulatedRotation()) * damagePercent * invRotationAmount * 0.02f;

                        if (!vehCarModel::MM1StyleWobble)
                        {
                            auto wheelWobbleLimit = this->carSim->AxleFront.getRightWheel()->getWobbleLimit();
                            auto wheelWobbleAmount = this->carSim->AxleFront.getRightWheel()->getWobbleAmount();

                            angle = sin(this->carSim->AxleFront.getRightWheel()->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.2f;
                        }

                        matrix.Rotate(vec, angle);

                        DrawPart(lod, 23, matrix, shaders, vehCarModel::PartReflections);
                    }
                    else {
                        DrawPart(lod, 23, this->carSim->AxleFront.getAxleMatrix(), shaders, vehCarModel::PartReflections);
                    }
                }

                auto axleRearmodel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 24)->GetLOD(lod);
                if (axleRearmodel != nullptr)
                {
                    if (vehCarModel::WheelWobble)
                    {
                        Matrix34 matrix;
                        Vector3 vec;

                        memcpy(&matrix, &this->carSim->AxleRear.getAxleMatrix(), sizeof(Matrix34));

                        float rotation = (fabs(this->carSim->AxleRear.getRightWheel()->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

                        float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
                        float invRotationAmount = 1.f - rotationAmount;

                        vec.X = this->getCarMatrix().m20 + this->getCarMatrix().m00;
                        vec.Y = this->getCarMatrix().m21 + this->getCarMatrix().m01;
                        vec.Z = this->getCarMatrix().m22 + this->getCarMatrix().m02;

                        auto carDamage = this->car->getCarDamage();
                        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

                        damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

                        float angle = sin(this->carSim->AxleRear.getRightWheel()->getAccumulatedRotation()) * damagePercent * invRotationAmount * 0.02f;

                        if (!vehCarModel::MM1StyleWobble)
                        {
                            auto wheelWobbleLimit = this->carSim->AxleRear.getRightWheel()->getWobbleLimit();
                            auto wheelWobbleAmount = this->carSim->AxleRear.getRightWheel()->getWobbleAmount();

                            angle = sin(this->carSim->AxleRear.getRightWheel()->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.2f;
                        }

                        matrix.Rotate(vec, angle);

                        DrawPart(lod, 24, matrix, shaders, vehCarModel::PartReflections);
                    }
                    else {
                        DrawPart(lod, 24, this->carSim->AxleRear.getAxleMatrix(), shaders, vehCarModel::PartReflections);
                    }
                }

                //engine
                if ((this->wheelBrokenStatus & 0x40000) != 0)
                {
                    auto engineMatrixPtr = this->carSim->getEngine()->getVisualMatrixPtr();
                    if(engineMatrixPtr != nullptr)
                        DrawPart(lod, 25, *engineMatrixPtr, shaders, vehCarModel::PartReflections);
                }

                //extra hubs
                auto hub4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 61)->GetLOD(lod);
                auto shub4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 71)->GetLOD(lod);
                if (hub4model != nullptr && (this->wheelBrokenStatus & 0x2000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(2);
                    dummyWhl4Matrix.Set(refWheel->getMatrix());

                    float offsetX = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m02;
                    dummyWhl4Matrix.m30 += offsetX;
                    dummyWhl4Matrix.m31 += offsetY;
                    dummyWhl4Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && shub4model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 71, dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 61, dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                }

                auto hub5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 62)->GetLOD(lod);
                auto shub5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 72)->GetLOD(lod);
                if (hub5model != nullptr && (this->wheelBrokenStatus & 0x10000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(3);
                    dummyWhl5Matrix.Set(refWheel->getMatrix());

                    float offsetX = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m02;
                    dummyWhl5Matrix.m30 += offsetX;
                    dummyWhl5Matrix.m31 += offsetY;
                    dummyWhl5Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && shub5model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 72, dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 62, dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
                    }
                }

                Matrix34 fndrMatrix = Matrix34();
                auto carMatrix = *this->carSim->getWorldMatrix();
                fndrMatrix.Identity();

                fndrMatrix.m10 = carMatrix.m10;
                fndrMatrix.m11 = carMatrix.m11;
                fndrMatrix.m12 = carMatrix.m12;

                //extra fenders
                auto fndr4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 65)->GetHighLOD();
                if (fndr4model != nullptr && (wheelBrokenStatus & 0x4000) != 0)
                {
                    fndrMatrix.m00 = dummyWhl4Matrix.m00;
                    fndrMatrix.m01 = dummyWhl4Matrix.m01;
                    fndrMatrix.m02 = dummyWhl4Matrix.m02;

                    fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
                    fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
                    fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

                    fndrMatrix.m30 = fndr4offset.Y * fndrMatrix.m10 + fndr4offset.Z * fndrMatrix.m20 + fndr4offset.X * fndrMatrix.m00 + dummyWhl4Matrix.m30;
                    fndrMatrix.m31 = fndr4offset.Y * fndrMatrix.m11 + fndr4offset.Z * fndrMatrix.m21 + fndr4offset.X * fndrMatrix.m01 + dummyWhl4Matrix.m31;
                    fndrMatrix.m32 = fndr4offset.Y * fndrMatrix.m12 + fndr4offset.Z * fndrMatrix.m22 + fndr4offset.X * fndrMatrix.m02 + dummyWhl4Matrix.m32;

                    DrawPart(3, 65, fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                auto fndr5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 66)->GetHighLOD();
                if (fndr5model != nullptr && (wheelBrokenStatus & 0x20000) != 0)
                {
                    fndrMatrix.m00 = dummyWhl5Matrix.m00;
                    fndrMatrix.m01 = dummyWhl5Matrix.m01;
                    fndrMatrix.m02 = dummyWhl5Matrix.m02;

                    fndrMatrix.m20 = fndrMatrix.m12 * fndrMatrix.m01 - fndrMatrix.m11 * fndrMatrix.m02;
                    fndrMatrix.m21 = fndrMatrix.m02 * fndrMatrix.m10 - fndrMatrix.m12 * fndrMatrix.m00;
                    fndrMatrix.m22 = fndrMatrix.m11 * fndrMatrix.m00 - fndrMatrix.m01 * fndrMatrix.m10;

                    fndrMatrix.m30 = fndr5offset.Y * fndrMatrix.m10 + fndr5offset.Z * fndrMatrix.m20 + fndr5offset.X * fndrMatrix.m00 + dummyWhl5Matrix.m30;
                    fndrMatrix.m31 = fndr5offset.Y * fndrMatrix.m11 + fndr5offset.Z * fndrMatrix.m21 + fndr5offset.X * fndrMatrix.m01 + dummyWhl5Matrix.m31;
                    fndrMatrix.m32 = fndr5offset.Y * fndrMatrix.m12 + fndr5offset.Z * fndrMatrix.m22 + fndr5offset.X * fndrMatrix.m02 + dummyWhl5Matrix.m32;

                    DrawPart(3, 66, fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                //extra wheels
                auto whl4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 51)->GetLOD(lod);
                auto swhl4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 59)->GetLOD(lod);
                if (whl4model != nullptr && (this->wheelBrokenStatus & 0x1000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(2);
                    dummyWhl4Matrix.Set(refWheel->getMatrix());

                    float offsetX = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m02;
                    dummyWhl4Matrix.m30 += offsetX;
                    dummyWhl4Matrix.m31 += offsetY;
                    dummyWhl4Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && swhl4model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 59, dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 51, dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                }

                auto whl5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 52)->GetLOD(lod);
                auto swhl5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 60)->GetLOD(lod);
                if (whl5model != nullptr && (this->wheelBrokenStatus & 0x8000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(3);
                    dummyWhl5Matrix.Set(refWheel->getMatrix());

                    float offsetX = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m02;
                    dummyWhl5Matrix.m30 += offsetX;
                    dummyWhl5Matrix.m31 += offsetY;
                    dummyWhl5Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && swhl5model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 60, dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 52, dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
                    }
                }
            }

            //set alpha back if requireed
            if (oldAlphaRef != (&RSTATE->Data)->AlphaRef)
            {
                (&RSTATE->Data)->AlphaRef = oldAlphaRef;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }

        AGE_API void DrawShadow() override                  { hook::Thunk<0x4CE940>::Call<void>(this); }
        AGE_API void DrawShadowMap() override               { hook::Thunk<0x4CEA90>::Call<void>(this); }
        AGE_API void DrawGlow() override                    { hook::Thunk<0x4CEB90>::Call<void>(this); }
        AGE_API void DrawReflected(float a1) override       { hook::Thunk<0x4CF080>::Call<void>(this, a1); }
        AGE_API unsigned int SizeOf() override              { return hook::Thunk<0x4CDFE0>::Call<int>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCarModel, lvlInstance>("vehCarModel")
                //properties
                .addPropertyReadOnly("Breakables", &getGenBreakableMgr)
                .addPropertyReadOnly("WheelBreakables", &getMechBreakableMgr)
                .addProperty("Variant", &getVariant, &setVariant)
                .addProperty("Visible", &GetVisible, &SetVisible)

                //lua functions
                .addFunction("GetHeadlight", &getHeadlight)

                //functions
                .addFunction("Reset", &Reset)
                .addFunction("BreakElectrics", &BreakElectrics)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("EjectOneshot", &EjectOneshot)


            .endClass();
        }
    };

    ASSERT_SIZEOF(vehCarModel, 0xCC + 0x30 + 0x4 + 0x18 + 0x4 + 0x30 + 0x8); //+13 extra fields

    // Lua initialization

}