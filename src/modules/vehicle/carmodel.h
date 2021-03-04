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
        static bool EnableFlashingHeadlights;
        static bool EnableSignals;
        static bool MWStyleTotaledCar;
        static int SirenType;
        static int HeadlightType;
        static float SirenCycle;
        static bool PartReflections;
        static bool WheelReflections;
        static bool mm1StyleTransmission; //god this is horrible...

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

        AGE_API void GetSurfaceColor(modStatic* model, Vector3* outVector)
                                                            { hook::Thunk<0x4CDF00>::Call<void>(this, model, outVector); }
        AGE_API void InitBreakable(vehBreakableMgr* manager, const char* basename, const char* breakableName, int geomId, int someId)
                                                            { hook::Thunk<0x4CDC50>::Call<void>(this, manager, basename, breakableName, geomId, someId); }
        AGE_API void InitSirenLight(const char* basename, const char* mtxName, int geomId)
                                                            { hook::Thunk<0x4CDF60>::Call<void>(this, basename, mtxName, geomId); }
        AGE_API void BreakElectrics(Vector3* a1)            { hook::Thunk<0x4CEFE0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { hook::Thunk<0x4CDFF0>::Call<void>(this); }
        AGE_API void EjectOneshot()                         { hook::Thunk<0x4CDCA0>::Call<void>(this); }
        AGE_API bool GetVisible()                           { return hook::Thunk<0x4CF070>::Call<bool>(this); }
        AGE_API void SetVisible(bool a1)                    { hook::Thunk<0x4CF050>::Call<void>(this, a1); }
        
        AGE_API void DrawHeadlights(bool rotate)
        {
            if (this->headlights == nullptr)
                return;

            if (rotate)
            {
                this->headlights[0].Direction.RotateY(datTimeManager::Seconds * 42.411503f);
                this->headlights[1].Direction.RotateY(datTimeManager::Seconds * -42.411503f);
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
            }

            //load headlights
            if (this->getGeomSetId() != 0) 
            {
                auto headlight0entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 47);
                auto headlight1entry = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 48);

                if (headlight0entry != nullptr && headlight1entry != nullptr)
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
            
            int variantGeomId = this->variant + 61;
            string_buf<16> buffer("variant%d", this->variant);
            InitBreakable(this->genBreakableMgr, basename, buffer, variantGeomId, 0);

            //create wheel breakables
            this->wheelBreakableMgr = new vehBreakableMgr();
            this->wheelBreakableMgr->Init(this->carSim->getWorldMatrix());
            this->wheelBreakableMgr->setVariant(this->variant);
            
            InitBreakable(this->wheelBreakableMgr, basename, "whl0", 26, 1 << 0);
            InitBreakable(this->wheelBreakableMgr, basename, "hub0", 38, 1 << 1);
            InitBreakable(this->wheelBreakableMgr, basename, "whl1", 27, 1 << 2);
            InitBreakable(this->wheelBreakableMgr, basename, "hub1", 39, 1 << 3);
            InitBreakable(this->wheelBreakableMgr, basename, "whl2", 28, 1 << 4);
            InitBreakable(this->wheelBreakableMgr, basename, "hub2", 40, 1 << 5);
            InitBreakable(this->wheelBreakableMgr, basename, "whl3", 29, 1 << 6);
            InitBreakable(this->wheelBreakableMgr, basename, "hub3", 41, 1 << 7);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr0", 49, 1 << 8);
            InitBreakable(this->wheelBreakableMgr, basename, "fndr1", 50, 1 << 9);
            InitBreakable(this->wheelBreakableMgr, basename, "engine", 25, 1 << 10);

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
            
            //draw BREAK objects
            this->genBreakableMgr->Draw(this->carSim->getWorldMatrix(), shaders, lod);

            //setup renderer
            Matrix44::Convert(gfxRenderState::sm_World, this->carSim->getWorldMatrix());
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            //draw the body
            auto bodyModel = mainGeomEntry->getLOD(lod);
            if (bodyModel != nullptr)
                bodyModel->Draw(shaders);

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

                decalModel->Draw(shaders);

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
                if (fndr0model != nullptr && (wheelBrokenStatus & 0x100) != 0)
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
                if (fndr1model != nullptr && (wheelBrokenStatus & 0x200) != 0)
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

                    int wheelStatusFlag = 1 << (i * 2);
                    int hubStatusFlag = 1 << ((i + 1) * 2);

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

                    //hub
                    if ((this->wheelBrokenStatus & hubStatusFlag) != 0)
                    {
                        DrawPart(lod, hubId, &wheel->getMatrix(), shaders, vehCarModel::PartReflections);
                    }
                }
            }

            //draw plight
            if (lod >= 1)
            {
                if (vehCar::sm_DrawHeadlights)
                    //plighton
                    DrawPart(lod, 53, this->carSim->getWorldMatrix(), shaders, vehCarModel::PartReflections);
                else
                    //plightoff
                    DrawPart(lod, 54, this->carSim->getWorldMatrix(), shaders, vehCarModel::PartReflections);
            }

            //draw suspension, engine, extra wheels
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
                if ((this->wheelBrokenStatus & 0x400) != 0)
                {
                    auto engineMatrixPtr = this->carSim->getEngine()->getVisualMatrixPtr();
                    if(engineMatrixPtr != nullptr)
                        DrawPart(lod, 25, engineMatrixPtr, shaders, vehCarModel::PartReflections);
                }

                //extra wheels
                auto whl4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 51)->getLOD(lod);
                auto swhl4model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 59)->getLOD(lod);
                if (whl4model != nullptr)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(2);
                    Matrix34 dummyWhlMatrix = Matrix34();
                    dummyWhlMatrix.Set(&refWheel->getMatrix());

                    float offsetX = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackLeftWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackLeftWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackLeftWheelPosDiff.X * carMatrix->m02;
                    dummyWhlMatrix.m30 += offsetX;
                    dummyWhlMatrix.m31 += offsetY;
                    dummyWhlMatrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && swhl4model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 59, &dummyWhlMatrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 51, &dummyWhlMatrix, shaders, vehCarModel::WheelReflections);
                    }
                }

                auto whl5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 52)->getLOD(lod);
                auto swhl5model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 60)->getLOD(lod);
                if (whl5model != nullptr)
                {
                    auto carMatrix = this->carSim->getWorldMatrix();
                    auto refWheel = this->carSim->getWheel(3);
                    Matrix34 dummyWhlMatrix = Matrix34();
                    dummyWhlMatrix.Set(&refWheel->getMatrix());

                    float offsetX = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m10 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m20 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m00;
                    float offsetY = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m11 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m21 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m01;
                    float offsetZ = carSim->BackBackRightWheelPosDiff.Y * carMatrix->m12 + carSim->BackBackRightWheelPosDiff.Z * carMatrix->m22 + carSim->BackBackRightWheelPosDiff.X * carMatrix->m02;
                    dummyWhlMatrix.m30 += offsetX;
                    dummyWhlMatrix.m31 += offsetY;
                    dummyWhlMatrix.m32 += offsetZ;

                    if (fabs(refWheel->getRotationRate()) > 26.f && swhl5model != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(lod, 60, &dummyWhlMatrix, shaders, vehCarModel::WheelReflections);
                    }
                    else {
                        DrawPart(lod, 52, &dummyWhlMatrix, shaders, vehCarModel::WheelReflections);
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

            if (vehCarModel::EnableSignals && car->IsPlayer()) {
                //check signal clock
                bool drawSignal = fmod(datTimeManager::ElapsedTime, 1.f) > 0.5f;

                //draw stuff!
                if (drawSignal) {
                    if (LeftSignalLightState || HazardLightsState) {
                        if (slight0 != nullptr)
                            slight0->Draw(shaders);
                    }
                    if (RightSignalLightState || HazardLightsState) {
                        if (slight1 != nullptr)
                            slight1->Draw(shaders);
                    }
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
                    if (vehCarModel::EnableFlashingHeadlights) {
                        if (siren != nullptr && siren->Active)
                        {
                            this->DrawHeadlights(true);
                        }
                        else if (car->IsPlayer() && vehCarModel::HeadlightsState || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                        {
                            this->DrawHeadlights(false);
                        }
                    }
                    else {
                        if (car->IsPlayer() && vehCarModel::HeadlightsState || !car->IsPlayer() && vehCar::sm_DrawHeadlights)
                        {
                            this->DrawHeadlights(false);
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

            if (vehCarModel::SirenType < 3) {
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

    ASSERT_SIZEOF(vehCarModel, 0xCC);

    // Lua initialization

}