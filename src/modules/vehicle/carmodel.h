#pragma once
#include <modules\vehicle.h>
#include <mm2_gfx.h>
#include <mm2_effects.h>

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
        static bool MWStyleTotaledCar;
        static int SirenType;
        static int HeadlightType;
        static float SirenCycle;
        static float HeadlightFlashingSpeed;
        static bool PartReflections;
        static bool WheelReflections;
        static bool mm1StyleTransmission; //god this is horrible...
        static bool breakableRenderTweak;

        //light states
        static bool HeadlightsState;
        static bool HazardLightsState;
        static bool LeftSignalLightState;
        static bool RightSignalLightState;
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
        ltLight* extraHeadlights[6]; //HEADLIGHT2-7
        Vector3 extraHeadlightPositions[6];
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
            
            auto headlightLightsArray = *getPtr<ltLight*>(this, 0xB0);
            if (headlightLightsArray == nullptr)
                return NULL;
            return &headlightLightsArray[index];
        }

        inline int getWheelBrokenStatus(void) const {
            return this->wheelBrokenStatus;
        }

        AGE_API void GetSurfaceColor(modStatic* model, Vector3* outVector)
                                                            { hook::Thunk<0x4CDF00>::Call<void>(this, model, outVector); }
        AGE_API void InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int geomId, int someId)
                                                            { hook::Thunk<0x4CDC50>::Call<void>(this, manager, basename, breakableName, geomId, someId); }
        AGE_API void InitSirenLight(const char* basename, const char* mtxName, int geomId)
        {
            if (this->getGeomSetId() != 0)
            {
                auto sirenEntry = lvlInstance::GetGeomTableEntry((this->getGeomSetId() - 1) + geomId);

                if (sirenEntry->getHighLOD() != nullptr)
                {
                    auto siren = this->car->getSiren();
                    Matrix34 outMatrix;

                    GetPivot(outMatrix, basename, mtxName);
                    this->GetSurfaceColor(sirenEntry->getHighLOD(), &siren->ltLightPool[siren->LightCount].Color);
                    siren->AddLight(&Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32), &siren->ltLightPool[siren->LightCount].Color);
                }
            }
        }

        AGE_API void BreakElectrics(Vector3* a1)            { hook::Thunk<0x4CEFE0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { hook::Thunk<0x4CDFF0>::Call<void>(this); }
        AGE_API void EjectOneshot()
        { 
            if (!(byte)this->dword_a4) {
                if (this->carSim->getSpeedMPH() > 100.f) {
                    this->wheelBreakableMgr->EjectAll(this->getRoomId());
                    this->wheelBrokenStatus = 0;
                    this->dword_a4 = 1;
                    return;
                }
                if (this->carSim->getSpeedMPH() <= 75.f) {
                    if (this->carSim->getSpeedMPH() <= 50.f)
                        return;

                    int i = 3 * (rand() % 4);

                    int wheelStatusFlag = 1 << i;
                    int hubStatusFlag = 1 << (i + 1);
                    int fenderStatusFlag = 1 << (i + 2);

                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(wheelStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(hubStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(fenderStatusFlag), this->getRoomId());

                    int ejectPackage = (wheelStatusFlag | hubStatusFlag | fenderStatusFlag);

                    int ii = 3 * (rand() % 2);

                    int extraWheelStatusFlag = 1 << (ii + 12);
                    int extraHubStatusFlag = 1 << (ii + 13);
                    int extraFenderStatusFlag = 1 << (ii + 14);

                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraWheelStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraHubStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraFenderStatusFlag), this->getRoomId());

                    int ejectPackage2 = (extraWheelStatusFlag | extraHubStatusFlag | extraFenderStatusFlag);

                    this->wheelBrokenStatus &= ~(ejectPackage | ejectPackage2);
                    this->dword_a4 = 1;
                    return;
                }
                else {
                    int i = 3 * (rand() % 4);

                    int wheelStatusFlag = 1 << i;
                    int hubStatusFlag = 1 << (i + 1);
                    int fenderStatusFlag = 1 << (i + 2);

                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(wheelStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(hubStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(fenderStatusFlag), this->getRoomId());

                    int ejectPackage = (wheelStatusFlag | hubStatusFlag | fenderStatusFlag);

                    int ii = 3 * (rand() % 4);

                    int wheelStatusFlag2 = 1 << ii;
                    int hubStatusFlag2 = 1 << (ii + 1);
                    int fenderStatusFlag2 = 1 << (ii + 2);

                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(wheelStatusFlag2), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(hubStatusFlag2), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(fenderStatusFlag2), this->getRoomId());

                    int ejectPackage2 = (wheelStatusFlag2 | hubStatusFlag2 | fenderStatusFlag2);

                    int iii = 3 * (rand() % 2);

                    int extraWheelStatusFlag = 1 << (iii + 12);
                    int extraHubStatusFlag = 1 << (iii + 13);
                    int extraFenderStatusFlag = 1 << (iii + 14);

                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraWheelStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraHubStatusFlag), this->getRoomId());
                    this->wheelBreakableMgr->Eject(this->wheelBreakableMgr->Get(extraFenderStatusFlag), this->getRoomId());

                    int ejectPackage3 = (extraWheelStatusFlag | extraHubStatusFlag | extraFenderStatusFlag);

                    this->wheelBrokenStatus &= ~(ejectPackage | ejectPackage2 | ejectPackage3);
                    this->dword_a4 = 1;
                    return;
                }
            }
        }

        AGE_API bool GetVisible()                           { return hook::Thunk<0x4CF070>::Call<bool>(this); }
        AGE_API void SetVisible(bool a1)                    { hook::Thunk<0x4CF050>::Call<void>(this, a1); }
        
        AGE_API void DrawHeadlights(bool rotate)
        {
            if (this->headlights == nullptr)
                return;

            if (rotate)
            {
                this->headlights[0].Direction.RotateY(datTimeManager::Seconds * vehCarModel::HeadlightFlashingSpeed);
                this->headlights[1].Direction.RotateY(datTimeManager::Seconds * -vehCarModel::HeadlightFlashingSpeed);
            }
            else
            {
                auto carMatrix = this->carSim->getWorldMatrix();
                this->headlights[0].Direction = Vector3(-carMatrix->m20, -carMatrix->m21, -carMatrix->m22);
                this->headlights[1].Direction = Vector3(-carMatrix->m20, -carMatrix->m21, -carMatrix->m22);
            }

            bool bothLightsBroken = !(enabledElectrics[2] || enabledElectrics[3]);
            if (bothLightsBroken)
                return;

            ltLight::DrawGlowBegin();
            for (int i = 0; i < 2; i++)
            {
                bool isHeadlightBroken = !(enabledElectrics[i + 2]);
                if (isHeadlightBroken)
                    continue;

                auto light = &this->headlights[i];
                auto lightPos = this->headlightPositions[i];
                auto carMatrix = this->carSim->getWorldMatrix();

                float lX = lightPos.Y * carMatrix->m10 + lightPos.Z * carMatrix->m20 + lightPos.X * carMatrix->m00 + carMatrix->m30;
                float lY = lightPos.Y * carMatrix->m11 + lightPos.Z * carMatrix->m21 + lightPos.X * carMatrix->m01 + carMatrix->m31;
                float lZ = lightPos.Y * carMatrix->m12 + lightPos.Z * carMatrix->m22 + lightPos.X * carMatrix->m02 + carMatrix->m32;
                light->Position = Vector3(lX, lY, lZ);

                Vector3* someCameraThing = (Vector3*)0x685490;
                light->DrawGlow(someCameraThing);
            }
            ltLight::DrawGlowEnd();
        }

        AGE_API void DrawExtraHeadlights(bool rotate)
        {
            int geomSetId = this->getGeomSetId();
            int geomSetIdOffset = geomSetId - 1;
            float rotationAmount = vehCarModel::HeadlightFlashingSpeed;

            ltLight::DrawGlowBegin();
            for (int i = 0; i < 6; i++)
            {
                auto headlightEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 73 + i);
                if (headlightEntry->getHighLOD() == nullptr)
                    continue;

                if (rotate)
                {
                    this->extraHeadlights[i]->Direction.RotateY(datTimeManager::Seconds * rotationAmount);
                    rotationAmount *= -1.f;
                }
                else
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    this->extraHeadlights[i]->Direction = Vector3(-carMatrix->m20, -carMatrix->m21, -carMatrix->m22);
                }

                auto light = this->extraHeadlights[i];
                auto lightPos = this->extraHeadlightPositions[i];
                auto carMatrix = this->carSim->getWorldMatrix();

                float lX = lightPos.Y * carMatrix->m10 + lightPos.Z * carMatrix->m20 + lightPos.X * carMatrix->m00 + carMatrix->m30;
                float lY = lightPos.Y * carMatrix->m11 + lightPos.Z * carMatrix->m21 + lightPos.X * carMatrix->m01 + carMatrix->m31;
                float lZ = lightPos.Y * carMatrix->m12 + lightPos.Z * carMatrix->m22 + lightPos.X * carMatrix->m02 + carMatrix->m32;
                light->Position = Vector3(lX, lY, lZ);

                Vector3* someCameraThing = (Vector3*)0x685490;
                light->DrawGlow(someCameraThing);
            }
            ltLight::DrawGlowEnd();
        }
        
        AGE_API void DrawPart(modStatic* model, const Matrix34* matrix, modShader* shaders)
                                                            { hook::Thunk<0x4CE880>::Call<void>(this, model, matrix, shaders); }

        AGE_API void DrawPart(int lod, int geomId, const Matrix34* matrix, modShader* shaders)
        {
            if (this->getGeomSetId() == 0)
                return;

            auto model = lvlInstance::GetGeomTableEntry(this->getGeomSetId() + geomId - 1)->getLOD(lod);
            if (model != nullptr)
                DrawPart(model, matrix, shaders);
        }

        void DrawPartReflected(int lod, int geomId, const Matrix34* matrix, modShader* shaders)
        {
            if (this->getGeomSetId() == 0)
                return;

            auto model = lvlInstance::GetGeomTableEntry(this->getGeomSetId() + geomId - 1)->getLOD(lod);
            if (model != nullptr) {
                DrawPart(model, matrix, shaders);

                hook::Type<gfxTexture*> g_ReflectionMap = 0x628914;
                bool isSoftware = *(bool*)0x6830D4;

                auto state = &MMSTATE;
                if (lod == 3 && (g_ReflectionMap != nullptr && !isSoftware && state->EnableReflections))
                {
                    modShader::BeginEnvMap(g_ReflectionMap, *matrix);
                    model->DrawEnvMapped(shaders, g_ReflectionMap, 1.f);
                    modShader::EndEnvMap();
                }
            }
        }
                                                            
        void DrawPart(int lod, int geomId, const Matrix34* matrix, modShader* shaders, bool reflected)
        {
            if (reflected)
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
                //gfxForceLVERTEX = 1;
                lvlInstance::AddGeom(basename, "shadow", 0);
                lvlInstance::AddGeom(basename, "hlight", 0);
                lvlInstance::AddGeom(basename, "tlight", 0);
                lvlInstance::AddGeom(basename, "rlight", 0);
                lvlInstance::AddGeom(basename, "slight0", 0);
                lvlInstance::AddGeom(basename, "slight1", 0);
                lvlInstance::AddGeom(basename, "blight", 0);
                //gfxForceLVERTEX = 0;

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

                //gfxForceLVERTEX = 1;
                lvlInstance::AddGeom(basename, "headlight0", 0);
                lvlInstance::AddGeom(basename, "headlight1", 0);
                //gfxForceLVERTEX = 0;

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

                //gfxForceLVERTEX = 1;
                lvlInstance::AddGeom(basename, "headlight2", 0);
                lvlInstance::AddGeom(basename, "headlight3", 0);
                lvlInstance::AddGeom(basename, "headlight4", 0);
                lvlInstance::AddGeom(basename, "headlight5", 0);
                lvlInstance::AddGeom(basename, "headlight6", 0);
                lvlInstance::AddGeom(basename, "headlight7", 0);
                //gfxForceLVERTEX = 0;

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

                //gfxForceLVERTEX = 1;
                lvlInstance::AddGeom(basename, "tslight0", 0);
                lvlInstance::AddGeom(basename, "tslight1", 0);
                //gfxForceLVERTEX = 0;

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
            if (this->getGeomSetId() != 0)
                shadersPerVariant = lvlInstance::GetGeomTableEntry(this->getGeomSetId())->numShadersPerPaintjob;
            this->variant = this->variant % shadersPerVariant;

            //get our geometry id
            int geomSetId = this->getGeomSetId();
            int geomSetIdOffset = geomSetId - 1;

            //pre-load our variant
            lvlInstance::PreLoadShader(this->variant);

            //init fxTexelDamage
            if (this->getGeomSetId() != 0) 
            {
                auto bodyEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset);
                if (bodyEntry->getHighLOD() != nullptr)
                {
                    this->texelDamage = new fxTexelDamage();

                    if (!texelDamage->Init(bodyEntry->getHighLOD(), bodyEntry->pShaders[this->getVariant()], bodyEntry->numShaders)) 
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
            if (this->getGeomSetId() != 0) 
            {
                auto headlight0entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 47);
                auto headlight1entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 48);

                if (headlight0entry->getHighLOD() != nullptr && headlight1entry->getHighLOD() != nullptr)
                {
                    this->headlights = new ltLight[2];
                    Matrix34 outMatrix;

                    GetPivot(outMatrix, basename, "headlight0");
                    headlights[0].Color = Vector3(1.f, 1.f, 1.f);
                    headlights[0].Type = 1;
                    headlights[0].SpotExponent = 3.f;
                    this->GetSurfaceColor(headlight0entry->getHighLOD(), &headlights[0].Color);
                    this->headlightPositions[0] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);

                    GetPivot(outMatrix, basename, "headlight1");
                    headlights[1].Color = Vector3(1.f, 1.f, 1.f);
                    headlights[1].Type = 1;
                    headlights[1].SpotExponent = 3.f;
                    this->GetSurfaceColor(headlight1entry->getHighLOD(), &headlights[1].Color);
                    this->headlightPositions[1] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }

            //load extra headlights
            if (this->getGeomSetId() != 0)
            {
                for (int i = 0; i < 6; i++)
                {
                    auto headlightEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 73 + i);
                    if (headlightEntry->getHighLOD() == nullptr)
                        continue;

                    Matrix34 outMatrix;

                    string_buf<16> buffer("headlight%d", i + 2);
                    GetPivot(outMatrix, basename, buffer);
                    extraHeadlights[i] = new ltLight();
                    extraHeadlights[i]->Color = Vector3(1.f, 1.f, 1.f);
                    extraHeadlights[i]->Type = 1;
                    extraHeadlights[i]->SpotExponent = 3.f;
                    this->GetSurfaceColor(headlightEntry->getHighLOD(), &extraHeadlights[i]->Color);
                    this->extraHeadlightPositions[i] = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }

            //load FNDR offsets
            if (this->getGeomSetId() != 0)
            {
                auto fndrEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 49);
                if (fndrEntry->getHighLOD() != nullptr)
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
                if (fndr2Entry->getHighLOD() != nullptr)
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
                if (fndr4Entry->getHighLOD() != nullptr)
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
            
            int variantGeomId = this->variant + 103;
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
            if (this->getGeomSetId() != 0)
            {
                auto hitchEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 42);
                if (hitchEntry->getHighLOD() != nullptr)
                {
                    Matrix34 outMatrix;
                    GetPivot(outMatrix, basename, "trailer_hitch");
                    this->trailerHitchPosition = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }
        }
        
        /*
            lvlInstance virtuals
        */

        AGE_API void Reset()  override                      { hook::Thunk<0x4CDFD0>::Call<void>(this); }
        AGE_API const Vector3 & GetPosition() override
                                                            { return hook::Thunk<0x4CEF50>::Call<const Vector3 &>(this); }
        AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            { return hook::Thunk<0x4CEF90>::Call<const Matrix34 &>(this, a1); }
        AGE_API void SetMatrix(const Matrix34 *a1) override 
                                                            { hook::Thunk<0x4CEFA0>::Call<void>(this, a1); }
        AGE_API dgPhysEntity * GetEntity() override         { return hook::Thunk<0x4CEFC0>::Call<dgPhysEntity*>(this); }
        AGE_API dgPhysEntity * AttachEntity() override 
                                                            { return hook::Thunk<0x4CEFD0>::Call<dgPhysEntity*>(this); }
        AGE_API const Vector3 * GetVelocity() override
                                                            { return hook::Thunk<0x4CEF80>::Call<Vector3 const*>(this); }

        AGE_API void Draw(int lod) override
        {
            if (lod < 0 || lod > 3)
                return;
            if (this->getGeomSetId() == 0)
                return;

            //get our geometry id
            int geomSetId = this->getGeomSetId();
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
            if (breakableRenderTweak)
                this->genBreakableMgr->Draw(this->carSim->getWorldMatrix(), shaders, lod);

            //setup renderer
            Matrix44::Convert(gfxRenderState::sm_World, this->carSim->getWorldMatrix());
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            //draw the body
            auto bodyModel = mainGeomEntry->getLOD(lod);
            if (bodyModel != nullptr)
                bodyModel->Draw(shaders);

            //draw BREAK objects below the body
            if (!breakableRenderTweak)
                this->genBreakableMgr->Draw(this->carSim->getWorldMatrix(), shaders, lod);

            //draw decal
            auto decalModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 11)->getLOD(lod);
            if (decalModel != nullptr)
            {
                int oldAlphaRef2 = (&RSTATE->Data)->AlphaRef;
                if ((&RSTATE->Data)->AlphaRef != 0)
                {
                    (&RSTATE->Data)->AlphaRef = 0;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }

                DrawPart(decalModel, this->carSim->getWorldMatrix(), shaders);

                if ((&RSTATE->Data)->AlphaRef != oldAlphaRef2)
                {
                    (&RSTATE->Data)->AlphaRef = oldAlphaRef2;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }
            }

            //draw reflection (only in H LOD)
            hook::Type<gfxTexture*> g_ReflectionMap = 0x628914;
            bool isSoftware = *(bool*)0x6830D4;
            auto state = &MMSTATE;

            if (bodyModel != nullptr && lod == 3 && (g_ReflectionMap != nullptr && !isSoftware && state->EnableReflections))
            {
                modShader::BeginEnvMap(g_ReflectionMap, *this->carSim->getWorldMatrix());
                bodyModel->DrawEnvMapped(shaders, g_ReflectionMap, 1.f);
                modShader::EndEnvMap();
            }

            //draw FNDR
            if (lod == 3)
            {
                Matrix34 fndrMatrix = Matrix34();
                auto carMatrix = *this->carSim->getWorldMatrix();
                fndrMatrix.Identity();

                fndrMatrix.m10 = carMatrix.m10;
                fndrMatrix.m11 = carMatrix.m11;
                fndrMatrix.m12 = carMatrix.m12;

                auto fndr0model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 49)->getHighLOD();
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

                    DrawPart(3, 49, &fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                auto fndr1model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 50)->getHighLOD();
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

                    DrawPart(3, 50, &fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                //draw FNDR2/3, we have to draw them here to be rendered over WHL2/3
                auto fndr2model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 63)->getHighLOD();
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

                    DrawPart(3, 63, &fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                auto fndr3model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 64)->getHighLOD();
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

                    DrawPart(3, 64, &fndrMatrix, shaders, vehCarModel::PartReflections);
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
                        auto shubModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + shubId)->getLOD(lod);
                        if (fabs(wheel->getRotationRate()) > 26.f && shubModel != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPart(lod, shubId, &wheel->getMatrix(), shaders, vehCarModel::PartReflections);
                        }
                        else {
                            DrawPart(lod, hubId, &wheel->getMatrix(), shaders, vehCarModel::PartReflections);
                        }
                    }

                    //wheel
                    if ((this->wheelBrokenStatus & wheelStatusFlag) != 0)
                    {
                        auto swhlModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + swhlId)->getLOD(lod);
                        if (fabs(wheel->getRotationRate()) > 26.f && swhlModel != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPart(lod, swhlId, &wheel->getMatrix(), shaders, vehCarModel::WheelReflections);
                        }
                        else {
                            DrawPart(lod, whlId, &wheel->getMatrix(), shaders, vehCarModel::WheelReflections);
                        }
                    }
                }
            }

            //draw plight
            if (lod >= 1)
            {
                if (car->IsPlayer() && vehCarModel::HeadlightsState || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                    //plighton
                    DrawPart(lod, 53, this->carSim->getWorldMatrix(), shaders, vehCarModel::PartReflections);
                else
                    //plightoff
                    DrawPart(lod, 54, this->carSim->getWorldMatrix(), shaders, vehCarModel::PartReflections);
            }

            Matrix34 dummyWhl4Matrix = Matrix34();
            Matrix34 dummyWhl5Matrix = Matrix34();

            //draw suspension, engine, extra wheels, extra fenders
            if (lod >= 1)
            {
                //suspension
                for (int i = 0; i < 4; i++)
                {
                    DrawPart(lod, 13 + i, &this->carSim->ShockSuspensions[i].getSuspensionMatrix(), shaders, vehCarModel::PartReflections);
                }
                for (int i = 0; i < 4; i++)
                {
                    DrawPart(lod, 17 + i, &this->carSim->ArmSuspensions[i].getSuspensionMatrix(), shaders, vehCarModel::PartReflections);
                }
                for (int i = 0; i < 2; i++)
                {
                    DrawPart(lod, 21 + i, &this->carSim->ShaftSuspensions[i].getSuspensionMatrix(), shaders, vehCarModel::PartReflections);
                }

                DrawPart(lod, 23, &this->carSim->AxleFront.getAxleMatrix(), shaders, vehCarModel::PartReflections);
                DrawPart(lod, 24, &this->carSim->AxleRear.getAxleMatrix(), shaders, vehCarModel::PartReflections);
                
                //engine
                if ((this->wheelBrokenStatus & 0x40000) != 0)
                {
                    auto engineMatrixPtr = this->carSim->getEngine()->getVisualMatrixPtr();
                    if(engineMatrixPtr != nullptr)
                        DrawPart(lod, 25, engineMatrixPtr, shaders, vehCarModel::PartReflections);
                }

                //extra hubs
                auto hub4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 61)->getLOD(lod);
                auto shub4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 71)->getLOD(lod);
                if (hub4model != nullptr && (this->wheelBrokenStatus & 0x2000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(2);
                    dummyWhl4Matrix.Set(&refWheel->getMatrix());

                    float offsetX = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m02;
                    dummyWhl4Matrix.m30 += offsetX;
                    dummyWhl4Matrix.m31 += offsetY;
                    dummyWhl4Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && shub4model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 71, &dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 61, &dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                }

                auto hub5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 62)->getLOD(lod);
                auto shub5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 72)->getLOD(lod);
                if (hub5model != nullptr && (this->wheelBrokenStatus & 0x10000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(3);
                    dummyWhl5Matrix.Set(&refWheel->getMatrix());

                    float offsetX = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m02;
                    dummyWhl5Matrix.m30 += offsetX;
                    dummyWhl5Matrix.m31 += offsetY;
                    dummyWhl5Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && shub5model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 72, &dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 62, &dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
                    }
                }

                Matrix34 fndrMatrix = Matrix34();
                auto carMatrix = *this->carSim->getWorldMatrix();
                fndrMatrix.Identity();

                fndrMatrix.m10 = carMatrix.m10;
                fndrMatrix.m11 = carMatrix.m11;
                fndrMatrix.m12 = carMatrix.m12;

                //extra fenders
                auto fndr4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 65)->getHighLOD();
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

                    DrawPart(3, 65, &fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                auto fndr5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 66)->getHighLOD();
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

                    DrawPart(3, 66, &fndrMatrix, shaders, vehCarModel::PartReflections);
                }

                //extra wheels
                auto whl4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 51)->getLOD(lod);
                auto swhl4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 59)->getLOD(lod);
                if (whl4model != nullptr && (this->wheelBrokenStatus & 0x1000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(2);
                    dummyWhl4Matrix.Set(&refWheel->getMatrix());

                    float offsetX = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m02;
                    dummyWhl4Matrix.m30 += offsetX;
                    dummyWhl4Matrix.m31 += offsetY;
                    dummyWhl4Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && swhl4model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 59, &dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 51, &dummyWhl4Matrix, shaders, vehCarModel::WheelReflections);
                    }
                }

                auto whl5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 52)->getLOD(lod);
                auto swhl5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 60)->getLOD(lod);
                if (whl5model != nullptr && (this->wheelBrokenStatus & 0x8000) != 0)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(3);
                    dummyWhl5Matrix.Set(&refWheel->getMatrix());

                    float offsetX = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m02;
                    dummyWhl5Matrix.m30 += offsetX;
                    dummyWhl5Matrix.m31 += offsetY;
                    dummyWhl5Matrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && swhl5model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 60, &dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 52, &dummyWhl5Matrix, shaders, vehCarModel::WheelReflections);
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
        
        AGE_API void DrawGlow() override
        {
            if (!this->GetVisible())
                return;

            //get our geometry id
            int geomSetId = this->getGeomSetId();
            int geomSetIdOffset = geomSetId - 1;

            //get shaders
            auto mainGeomEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset);
            auto shaders = mainGeomEntry->pShaders[this->getVariant()];

            //get car stuff we use to determine what to darw
            auto car = this->getCar();
            auto carsim = this->carSim;
            auto siren = car->getSiren();
            auto curDamage = car->getCarDamage()->getCurDamage();
            auto maxDamage = car->getCarDamage()->getMaxDamage();
            int gear = carsim->getTransmission()->getGear();
            if (curDamage >= maxDamage && vehCarModel::MWStyleTotaledCar)
                return;

            //setup renderer
            Matrix44::Convert(gfxRenderState::sm_World, this->carSim->getWorldMatrix());
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;


            //draw signals
            modStatic* slight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 5)->getHighestLOD();
            modStatic* slight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 6)->getHighestLOD();

            //draw brake signals
            modStatic* tslight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 101)->getHighestLOD();
            modStatic* tslight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 102)->getHighestLOD();

            //check signal clock
            bool drawSignal = fmod(datTimeManager::ElapsedTime, 1.f) > 0.5f;

            //draw stuff!
            if (drawSignal && car->IsPlayer()) {
                if (LeftSignalLightState || HazardLightsState) {
                    if (slight0 != nullptr)
                        slight0->Draw(shaders);
                    if (tslight0 != nullptr)
                        tslight0->Draw(shaders);
                }
                if (RightSignalLightState || HazardLightsState) {
                    if (slight1 != nullptr)
                        slight1->Draw(shaders);
                    if (tslight1 != nullptr)
                        tslight1->Draw(shaders);
                }
            }

            //draw brake signals for player
            if (car->IsPlayer()) {
                if (!LeftSignalLightState && !HazardLightsState) {
                    if (tslight0 != nullptr) {
                        //draw brake copy
                        if (carsim->getBrake() > 0.1)
                            tslight0->Draw(shaders);
                        //draw headlight copy
                        if (vehCarModel::HeadlightsState)
                            tslight0->Draw(shaders);
                    }
                }
                if (!RightSignalLightState && !HazardLightsState) {
                    if (tslight1 != nullptr) {
                        //draw brake copy
                        if (carsim->getBrake() > 0.1)
                            tslight1->Draw(shaders);
                        //draw headlight copy
                        if (vehCarModel::HeadlightsState)
                            tslight1->Draw(shaders);
                    }
                }
            }

            //draw brake signals for cops and opponents
            if (!car->IsPlayer()) {
                if (tslight0 != nullptr) {
                    //draw brake copy
                    if (carsim->getBrake() > 0.1)
                        tslight0->Draw(shaders);
                    //draw headlight copy
                    if (vehCar::sm_DrawHeadlights)
                        tslight0->Draw(shaders);
                }
                if (tslight1 != nullptr) {
                    //draw brake copy
                    if (carsim->getBrake() > 0.1)
                        tslight1->Draw(shaders);
                    //draw headlight copy
                    if (vehCar::sm_DrawHeadlights)
                        tslight1->Draw(shaders);
                }
            }

            //only draw rear lights if the electrics allow it
            if (enabledElectrics[0] || enabledElectrics[1])
            {   
                //draw tlight
                modStatic* tlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 3)->getHighestLOD();
                if (tlight != nullptr) {
                    //draw brake copy
                    if (carsim->getBrake() > 0.1)
                        tlight->Draw(shaders);
                    //draw headlight copy
                    if (car->IsPlayer() && vehCarModel::HeadlightsState || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                        tlight->Draw(shaders);
                }

                //draw blight
                modStatic* blight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 7)->getHighestLOD();
                if (blight != nullptr) {
                    //draw brake copy
                    if (carsim->getBrake() > 0.1)
                        blight->Draw(shaders);
                }

                //draw rlight
                modStatic* rlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 4)->getHighestLOD();
                if (mm1StyleTransmission) {
                    auto throttle = carsim->getEngine()->getThrottleInput();
                    auto speedMPH = carsim->getSpeedMPH();
                    auto transmission = carsim->getTransmission();

                    if (rlight != nullptr && gear == 0) {
                        if (transmission->IsAuto()) {
                            if (throttle > 0.f || speedMPH >= 1.f)
                                rlight->Draw(shaders);
                        }
                        else {
                            rlight->Draw(shaders);
                        }
                    }
                }
                else {
                    if (rlight != nullptr && gear == 0) {
                        rlight->Draw(shaders);
                    }
                }
            }

            //Draw siren and headlights
            modStatic* hlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 2)->getHighestLOD();
            modStatic* siren0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 9)->getHighestLOD();
            modStatic* siren1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 10)->getHighestLOD();

            if (vehCarModel::HeadlightType < 3) {
                if (vehCarModel::HeadlightType == 0 || vehCarModel::HeadlightType == 2) {
                    //MM2 headlights
                    if (vehCarModel::EnableHeadlightFlashing) {
                        if (siren != nullptr && siren->Active)
                        {
                            this->DrawHeadlights(true);
                            this->DrawExtraHeadlights(true);
                        }
                        else if (car->IsPlayer() && vehCarModel::HeadlightsState || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                        {
                            this->DrawHeadlights(false);
                            this->DrawExtraHeadlights(false);
                        }
                    }
                    else {
                        if (car->IsPlayer() && vehCarModel::HeadlightsState || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                        {
                            this->DrawHeadlights(false);
                            this->DrawExtraHeadlights(false);
                        }
                    }
                }
                if (vehCarModel::HeadlightType == 1 || vehCarModel::HeadlightType == 2) {
                    //MM1 headlights
                    Matrix44::Convert(gfxRenderState::sm_World, this->carSim->getWorldMatrix());
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                    if (enabledElectrics[2] || enabledElectrics[3])
                    {
                        if (hlight != nullptr) {
                            if (car->IsPlayer() && vehCarModel::HeadlightsState || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                                hlight->Draw(shaders);
                        }
                    }
                }
            }

            if (vehCarModel::SirenType < 3 && !car->IsPlayer()) {
                if (vehCarModel::SirenType == 0 || vehCarModel::SirenType == 2) {
                    //MM2 siren
                    if (siren != nullptr && siren->HasLights && siren->Active)
                    {
                        siren->Draw(this->carSim->getWorldMatrix());
                    }
                }
                if (vehCarModel::SirenType == 1 || vehCarModel::SirenType == 2) {
                    //MM1 siren
                    Matrix44::Convert(gfxRenderState::sm_World, this->carSim->getWorldMatrix());
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                    if (siren != nullptr && siren->Active) {
                        int sirenStage = fmod(datTimeManager::ElapsedTime, 2 * vehCarModel::SirenCycle) >= vehCarModel::SirenCycle ? 1 : 0;
                        if (sirenStage == 0 && siren0 != nullptr) {
                            siren0->Draw(shaders);
                        }
                        else if (sirenStage == 1 && siren1 != nullptr) {
                            siren1->Draw(shaders);
                        }
                    }
                }
            }

            if (car->IsPlayer()) {
                if (vehSiren::SirenLightStyle >= 0 && vehSiren::SirenLightStyle < 3) {
                    //MM2 siren
                    if (siren != nullptr && siren->HasLights && siren->Active)
                    {
                        siren->Draw(this->carSim->getWorldMatrix());
                    }
                }
                if (vehSiren::SirenLightStyle >= 0 && vehSiren::SirenLightStyle != 2) {
                    //MM1 siren
                    Matrix44::Convert(gfxRenderState::sm_World, this->carSim->getWorldMatrix());
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                    if (siren != nullptr && siren->Active) {
                        int sirenStage = fmod(datTimeManager::ElapsedTime, 2 * vehCarModel::SirenCycle) >= vehCarModel::SirenCycle ? 1 : 0;
                        if (sirenStage == 0 && siren0 != nullptr) {
                            siren0->Draw(shaders);
                        }
                        else if (sirenStage == 1 && siren1 != nullptr) {
                            siren1->Draw(shaders);
                        }
                    }
                }
            }
        }

        AGE_API void DrawReflected(float a1) override
                                                            { hook::Thunk<0x4CF080>::Call<void>(this, a1); }
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

    ASSERT_SIZEOF(vehCarModel, 0xCC + 0xC + 0xC + 0xC + 0xC + 0x18 + 0x48); //+6 extra fields

    // Lua initialization

}