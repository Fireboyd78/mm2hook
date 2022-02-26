#pragma once
#include <modules\ai.h>
#include <modules\ai\aiVehicleData.h>

#include "aiVehicleSpline.h"
#include "aiRailSet.h"

namespace MM2
{
    // Forward declarations
    class aiVehicleActive;
    class aiVehicleManager;

    // External declarations
    extern class asNode;
    extern class ltLight;

    // Class definitions
    class aiVehicleActive : dgPhysEntity {
    private:
        datCallback* CallBack;
        asParticles Particles;
        asParticles Particles2;
        asBirthRule BirthRule;
        asBirthRule BirthRule2;
        float CurDamage;
        float MaxDamage;
        int field_2D0;
        lvlInstance* Instance;
        phInertialCS ICS;
        phSleep Sleep;
        vehWeelCheap WheelFrontLeft;
        vehWeelCheap WheelFrontRight;
        vehWeelCheap WheelBackLeft;
        vehWeelCheap WheelBackRight;
    public:
        inline vehWeelCheap * getWheel(int num) {
            switch (num) {
            case 0:
                return &WheelFrontLeft;
            case 1:
                return &WheelFrontRight;
            case 2:
                return &WheelBackLeft;
            case 3:
                return &WheelBackRight;
            }
            return nullptr;
        }

        /*
            dgPhysEntity virtuals
        */
        virtual AGE_API void Update() override                    { hook::Thunk<0x553890>::Call<void>(this); }
        virtual AGE_API void PostUpdate() override                { hook::Thunk<0x553960>::Call<void>(this); }
        virtual AGE_API phInertialCS* GetICS() override           { return hook::Thunk<0x5543B0>::Call<phInertialCS*>(this); }
        virtual AGE_API lvlInstance* GetInst() override           { return hook::Thunk<0x553430>::Call<lvlInstance*>(this); }
        virtual AGE_API void DetachMe() override                  { hook::Thunk<0x553690>::Call<void>(this); }
    };
    ASSERT_SIZEOF(aiVehicleActive, 0xAFC);

    class aiVehicleManager : public asNode {
    private:
        aiVehicleData VehicleDatas[32];
        int NumVehicleDatas;
        aiVehicleActive* ActiveActives[32];
        aiVehicleActive AiVehicleActives[32];
        short AttachedCount;
        short Gap;
        ltLight* SharedLight;
    public:
        static hook::Type<aiVehicleManager *> Instance;
        static hook::Type<int> SignalClock;

        AGE_API aiVehicleManager(void) {
            scoped_vtable x(this);
            hook::Thunk<0x553B30>::Call<void>(this);
        }

        AGE_API virtual ~aiVehicleManager(void) {
            scoped_vtable x(this);
            hook::Thunk<0x553C2>::Call<void>(this);
        }

        //members
        AGE_API void Init(char *unused)                           { hook::Thunk<0x553CE0>::Call<void>(this, unused); }
        AGE_API int AddVehicleDataEntry(LPCSTR name)              { return hook::Thunk<0x553FA0>::Call<int>(this, name); }
        AGE_API void SaveEntry()                                  { hook::Thunk<0x5541E0>::Call<void>(this); }
        /*
        AGE_API aiVehicleActive Attach
        AGE_API aiVehicleActive Detach
        */

        //asNode overrides
        AGE_API void Reset() override                             { hook::Thunk<0x553D60>::Call<void>(this); }
        AGE_API void Update() override                            { hook::Thunk<0x553EA0>::Call<void>(this); }

        //helpers
        int getDataCount() {
            return this->NumVehicleDatas;
        }

        aiVehicleData * getData(int num) {
            //clamp
            int max = getDataCount();
            if (num >= max)
                num = max - 1;

            //return data
            return &this->VehicleDatas[num];
        }

        inline ltLight * getSharedLight()
        {
            return this->SharedLight;
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleManager, asNode>("aiVehicleManager")
                .addStaticProperty("Instance", [] { return Instance.get(); })
                
                .addFunction("AddVehicleDataEntry", &AddVehicleDataEntry)
                .addFunction("SaveEntry", &SaveEntry)

                .addPropertyReadOnly("DataCount", &getDataCount)
                .addFunction("GetData", &getData)
            .endClass();
        }
    };
    ASSERT_SIZEOF(aiVehicleManager, 0x177A4);

    class aiVehicleInstance : public lvlInstance {
    public:
        static int AmbientHeadlightStyle;
        static int MM1StyleShadows;
        static bool ShadowTransparency;
    private:
        aiVehicleSpline* Spline;
        short SignalFrequency;
        byte SignalState;
        short LOD;
        short Variant;
        vehBreakableMgr* BreakableMgr;
        Vector3 HeadlightPosition;
        Vector3 VehiclePosition;
    public:
        AGE_API aiVehicleInstance(aiVehicleSpline* spline, char* basename) : lvlInstance()
        {
            this->Spline = spline;

            this->BreakableMgr = new vehBreakableMgr();
            this->BreakableMgr->setImpactThreshold(2500.f);
            this->BreakableMgr->setField_10(11.f);
            this->BreakableMgr->setField_18(4.f);

            if (lvlInstance::BeginGeom(basename, "BODY", 5))
            {
                gfxForceLVERTEX = true;
                lvlInstance::AddGeom(basename, "SHADOW", 0);
                lvlInstance::AddGeom(basename, "HLIGHT", 0);
                lvlInstance::AddGeom(basename, "TLIGHT", 0);
                lvlInstance::AddGeom(basename, "SLIGHT0", 0);
                lvlInstance::AddGeom(basename, "SLIGHT1", 0);
                lvlInstance::AddGeom(basename, "BLIGHT", 0); //NEW MM2HOOK OBJECT
                gfxForceLVERTEX = false;

                lvlInstance::AddGeom(basename, "WHL0", 4);
                lvlInstance::AddGeom(basename, "WHL1", 4);
                lvlInstance::AddGeom(basename, "WHL2", 4);
                lvlInstance::AddGeom(basename, "WHL3", 4);

                lvlInstance::AddGeom(basename, "break0", 2);
                lvlInstance::AddGeom(basename, "break1", 2);
                lvlInstance::AddGeom(basename, "break2", 2);
                lvlInstance::AddGeom(basename, "break3", 2);

                //NEW MM2HOOK OBJECTS
                lvlInstance::AddGeom(basename, "break01", 2);
                lvlInstance::AddGeom(basename, "break12", 2);
                lvlInstance::AddGeom(basename, "break23", 2);
                lvlInstance::AddGeom(basename, "break03", 2);
                //NEW MM2HOOK OBJECTS

                gfxForceLVERTEX = true;
                lvlInstance::AddGeom(basename, "headlight0", 0);
                lvlInstance::AddGeom(basename, "headlight1", 0);
                gfxForceLVERTEX = false;

                lvlInstance::AddGeom(basename, "WHL4", 0);
                lvlInstance::AddGeom(basename, "WHL5", 0);

                //NEW MM2HOOK OBJECTS
                lvlInstance::AddGeom(basename, "PLIGHTON", 0);
                lvlInstance::AddGeom(basename, "PLIGHTOFF", 0);

                lvlInstance::AddGeom(basename, "SWHL0", 0);
                lvlInstance::AddGeom(basename, "SWHL1", 0);
                lvlInstance::AddGeom(basename, "SWHL2", 0);
                lvlInstance::AddGeom(basename, "SWHL3", 0);
                lvlInstance::AddGeom(basename, "SWHL4", 0);
                lvlInstance::AddGeom(basename, "SWHL5", 0);

                gfxForceLVERTEX = true;
                lvlInstance::AddGeom(basename, "TSLIGHT0", 0);
                lvlInstance::AddGeom(basename, "TSLIGHT1", 0);
                gfxForceLVERTEX = false;

                lvlInstance::EndGeom();
            }

            this->SetFlag(0xD8);
            this->SignalFrequency = irand2(this);
            this->SignalState = 0;
            this->SetSubType(aiVehicleManager::Instance->AddVehicleDataEntry(basename));

            this->SetColor();
            this->InitBreakable(basename, "break0", 11);
            this->InitBreakable(basename, "break1", 12);
            this->InitBreakable(basename, "break2", 13);
            this->InitBreakable(basename, "break3", 14);
            this->InitBreakable(basename, "break01", 15);
            this->InitBreakable(basename, "break12", 16);
            this->InitBreakable(basename, "break23", 17);
            this->InitBreakable(basename, "break03", 18);

            this->BreakableMgr->setVariant(this->Variant);
            this->BreakableMgr->Init(&this->Spline->GetMatrix());

            //get our geometry id
            int geomSetId = this->GetGeomIndex();
            int geomSetIdOffset = geomSetId - 1;

            if (this->GetGeomIndex() != 0)
            {
                auto headlight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 19);
                if (headlight0->GetHighLOD() != nullptr)
                {
                    Matrix34 outMatrix;
                    GetPivot(outMatrix, basename, "headlight0");
                    this->HeadlightPosition = Vector3(outMatrix.m30, outMatrix.m31, outMatrix.m32);
                }
            }
        }

        aiVehicleInstance * Construct(aiVehicleSpline* spline, char* basename)
        {
            return new(this) aiVehicleInstance(spline, basename);
        }

        //properties
        inline aiVehicleSpline * getSpline()
        {
            return this->Spline;
        }

        //members
        AGE_API aiVehicleData * GetData()                          { return hook::Thunk<0x553F80>::Call<aiVehicleData*>(this); }
        AGE_API void SetColor()                                    { hook::Thunk<0x552110>::Call<void>(this); }
        AGE_API void InitBreakable(const char* basename, const char* breakableName, int geomId)
                                                                   { hook::Thunk<0x552010>::Call<void>(this, basename, breakableName, geomId); }

        void DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, bool reflected)
        {
            //setup renderer
            Matrix44::Convert(gfxRenderState::sm_World, matrix);
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            //draw model
            model->Draw(shaders);

            //draw reflections
            if (reflected && !(lvlLevel::Singleton->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean)))
            {
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

        //overrides
        AGE_API void Reset() override
        {
            this->SignalState = 0;

            if (this->BreakableMgr != nullptr)
                this->BreakableMgr->Reset();
        }

        AGE_API Vector3 const& GetPosition() override              { return hook::Thunk<0x553030>::Call<Vector3 const&>(this); }
        AGE_API Matrix34 const& GetMatrix(Matrix34* a1) override   { return hook::Thunk<0x553020>::Call<Matrix34 const&>(this, a1); }
        AGE_API void SetMatrix(Matrix34 const &a1) override        { hook::Thunk<0x553010>::Call<void>(this, &a1); }
        AGE_API dgPhysEntity* GetEntity() override                 { return hook::Thunk<0x552F50>::Call<dgPhysEntity*>(this); }
        AGE_API dgPhysEntity* AttachEntity() override              { return hook::Thunk<0x552FB0>::Call<dgPhysEntity*>(this); }
        AGE_API void Detach() override                             { hook::Thunk<0x552F80>::Call<void>(this); }

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
            auto shaders = mainGeomEntry->pShaders[this->Variant];

            //draw the body
            auto bodyModel = mainGeomEntry->GetLOD(lod);
            if (bodyModel != nullptr)
                DrawPart(bodyModel, this->Spline->GetMatrix(), shaders, lod == 3);

            //draw breakables
            this->BreakableMgr->Draw(this->Spline->GetMatrix(), shaders, 3);

            //draw wheels (only in H LOD)
            if (lod == 3)
            {
                if (this->GetEntity() != nullptr)
                {
                    auto vehicleActive = (aiVehicleActive*)this->GetEntity();

                    for (int i = 0; i < 4; i++)
                    {
                        modStatic* wheelModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 7 + i)->GetHighLOD();
                        if (wheelModel != nullptr)
                            DrawPart(wheelModel, vehicleActive->getWheel(i)->getMatrix(), shaders, vehCarModel::PartReflections);
                    }

                    modStatic* whl4Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 21)->GetHighLOD();
                    if (whl4Model != nullptr)
                    {
                        Matrix34 whl4Matrix;

                        whl4Matrix.Identity();
                        whl4Matrix.SetRow(3, *this->GetData()->GetWheelPosition(4));
                        whl4Matrix.Dot(this->Spline->GetMatrix());

                        DrawPart(whl4Model, whl4Matrix, shaders, vehCarModel::PartReflections);
                    }

                    modStatic* whl5Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 22)->GetHighLOD();
                    if (whl5Model != nullptr)
                    {
                        Matrix34 whl5Matrix;

                        whl5Matrix.Identity();
                        whl5Matrix.SetRow(3, *this->GetData()->GetWheelPosition(5));
                        whl5Matrix.Dot(this->Spline->GetMatrix());

                        DrawPart(whl5Model, whl5Matrix, shaders, vehCarModel::PartReflections);
                    }
                }
                else {
                    for (int i = 0; i < 4; i++)
                    {
                        int swhlId = 55 + i;

                        Matrix34 wheelMatrix;

                        wheelMatrix.Identity();
                        wheelMatrix.SetRow(3, *this->GetData()->GetWheelPosition(i));
                        wheelMatrix.MakeRotateX(-this->Spline->getWheelRotation());
                        wheelMatrix.Dot(this->Spline->GetMatrix());

                        modStatic* wheelModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 7 + i)->GetHighLOD();
                        if (wheelModel != nullptr)
                        {
                            modStatic* sWheelModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 25 + i)->GetHighLOD();
                            if (this->Spline->getCurSpeed() > 20.f && sWheelModel != nullptr && vehCarModel::EnableSpinningWheels)
                            {
                                DrawPart(sWheelModel, wheelMatrix, shaders, vehCarModel::PartReflections);
                            }
                            else {
                                DrawPart(wheelModel, wheelMatrix, shaders, vehCarModel::PartReflections);
                            }
                        }
                    }

                    modStatic* whl4Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 21)->GetHighLOD();
                    if (whl4Model != nullptr)
                    {
                        Matrix34 whl4Matrix;

                        whl4Matrix.Identity();
                        whl4Matrix.SetRow(3, *this->GetData()->GetWheelPosition(4));
                        whl4Matrix.MakeRotateX(-this->Spline->getWheelRotation());
                        whl4Matrix.Dot(this->Spline->GetMatrix());

                        modStatic* swhl4Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 29)->GetHighLOD();
                        if (this->Spline->getCurSpeed() > 20.f && swhl4Model != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPart(swhl4Model, whl4Matrix, shaders, vehCarModel::PartReflections);
                        }
                        else {
                            DrawPart(whl4Model, whl4Matrix, shaders, vehCarModel::PartReflections);
                        }
                    }

                    modStatic* whl5Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 22)->GetHighLOD();
                    if (whl5Model != nullptr)
                    {
                        Matrix34 whl5Matrix;

                        whl5Matrix.Identity();
                        whl5Matrix.SetRow(3, *this->GetData()->GetWheelPosition(5));
                        whl5Matrix.MakeRotateX(-this->Spline->getWheelRotation());
                        whl5Matrix.Dot(this->Spline->GetMatrix());

                        modStatic* swhl5Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 30)->GetHighLOD();
                        if (this->Spline->getCurSpeed() > 20.f && swhl5Model != nullptr && vehCarModel::EnableSpinningWheels)
                        {
                            DrawPart(swhl5Model, whl5Matrix, shaders, vehCarModel::PartReflections);
                        }
                        else {
                            DrawPart(whl5Model, whl5Matrix, shaders, vehCarModel::PartReflections);
                        }
                    }
                }
            }

            //draw pop-up headlights
            if (lod >= 2)
            {
                modStatic* plighton = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 23)->GetHighLOD();
                if (plighton != nullptr)
                {
                    if (aiMap::Instance->drawHeadlights)
                        DrawPart(plighton, this->Spline->GetMatrix(), shaders, vehCarModel::PartReflections);
                }

                modStatic* plightoff = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 24)->GetHighLOD();
                if (plightoff != nullptr)
                {
                    if (!aiMap::Instance->drawHeadlights)
                        DrawPart(plightoff, this->Spline->GetMatrix(), shaders, vehCarModel::PartReflections);
                }
            }

            this->LOD = lod + 1;
        }

        AGE_API void DrawShadow() override
        {
            //get our geometry id
            int geomSetId = this->GetGeomIndex();
            int geomSetIdOffset = geomSetId - 1;

            auto vehicleMatrix = this->Spline->GetMatrix();
            auto timeWeather = $::timeWeathers.ptr(cityLevel::timeOfDay);

            //get shaders
            auto shaders = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->pShaders[this->Variant];

            //get shadow
            modStatic* shadow = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 1)->GetHighLOD();

            if (shadow != nullptr)
            {
                Matrix34 shadowMatrix;

                if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, this->GetRoomId(), &vehicleMatrix))
                {
                    RSTATE->SetBlendSet(0, 0x80);

                    Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                    shadow->Draw(shaders);
                }
            }

            if (MMSTATE->TimeOfDay == 3 || MMSTATE->WeatherType != 0 || aiVehicleInstance::MM1StyleShadows < 4 ||
                lvlLevel::Singleton->GetRoomInfo(this->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
                return;

            modStatic* body = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->GetHighLOD();

            if (body != nullptr)
            {
                int srcBlend = (&RSTATE->Data)->SrcBlend;
                if ((&RSTATE->Data)->SrcBlend != 0)
                {
                    (&RSTATE->Data)->SrcBlend = 0;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }

                int destBlend = (&RSTATE->Data)->DestBlend;
                if (aiVehicleInstance::ShadowTransparency)
                {
                    if ((&RSTATE->Data)->DestBlend != 9)
                    {
                        (&RSTATE->Data)->DestBlend = 9;
                        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                    }
                }
                else {
                    if ((&RSTATE->Data)->DestBlend != 0)
                    {
                        (&RSTATE->Data)->DestBlend = 0;
                        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                    }
                }

                Matrix34 shadowMatrix;

                if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, this->GetRoomId(), &vehicleMatrix))
                {
                    float posDiffY = vehicleMatrix.m31 - shadowMatrix.m31;

                    shadowMatrix.m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
                    shadowMatrix.m11 = 0.f;
                    shadowMatrix.m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

                    shadowMatrix.m30 += shadowMatrix.m10 * posDiffY;
                    shadowMatrix.m32 += shadowMatrix.m12 * posDiffY;

                    Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                    body->Draw(shaders);
                }

                if ((&RSTATE->Data)->DestBlend != destBlend)
                {
                    (&RSTATE->Data)->DestBlend = destBlend;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }

                if ((&RSTATE->Data)->SrcBlend != srcBlend)
                {
                    (&RSTATE->Data)->SrcBlend = srcBlend;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }
            }
        }

        AGE_API void DrawShadowMap() override                      { hook::Thunk<0x552F30>::Call<void>(this); }

        AGE_API void DrawGlow() override
        {
            //setup renderer
            Matrix44::Convert(gfxRenderState::sm_World, this->Spline->GetMatrix());
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            //get our geometry id
            int geomSetId = this->GetGeomIndex();
            int geomSetIdOffset = geomSetId - 1;

            //get shaders
            auto shaders = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->pShaders[this->Variant];

            //get lights
            modStatic* hlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 2)->GetHighestLOD();
            modStatic* tlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 3)->GetHighestLOD();
            modStatic* slight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 4)->GetHighestLOD();
            modStatic* slight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 5)->GetHighestLOD();
            modStatic* blight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 6)->GetHighestLOD();
            modStatic* tslight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 31)->GetHighestLOD();
            modStatic* tslight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 32)->GetHighestLOD();

            //draw brake light
            if (blight != nullptr)
            {
                if (this->Spline->getRailSet()->getCurAccelFactor() < 0.f || this->Spline->getCurSpeed() == 0.f)
                    blight->Draw(shaders);
            }

            //draw taillight
            if (tlight != nullptr)
            {
                //draw brake copy
                if (this->Spline->getRailSet()->getCurAccelFactor() < 0.f || this->Spline->getCurSpeed() == 0.f)
                    tlight->Draw(shaders);
                //draw headlight copy
                if (aiMap::Instance->drawHeadlights)
                    tlight->Draw(shaders);
            }

            //draw left signal
            if (this->SignalState & 1)
            {
                if ((aiVehicleManager::SignalClock + (int)this->SignalFrequency) & 8)
                {
                    if (slight0 != nullptr)
                        slight0->Draw(shaders);
                    if (tslight0 != nullptr)
                        tslight0->Draw(shaders);
                }
            }
            else {
                if (tslight0 != nullptr) {
                    //draw brake copy
                    if (this->Spline->getRailSet()->getCurAccelFactor() < 0.f || this->Spline->getCurSpeed() == 0.f)
                        tslight0->Draw(shaders);
                    //draw headlight copy
                    if (aiMap::Instance->drawHeadlights)
                        tslight0->Draw(shaders);
                }
            }

            //draw right signal
            if (this->SignalState & 2)
            {
                if ((aiVehicleManager::SignalClock + (int)this->SignalFrequency) & 8)
                {
                    if (slight1 != nullptr)
                        slight1->Draw(shaders);
                    if (tslight1 != nullptr)
                        tslight1->Draw(shaders);
                }
            }
            else {
                if (tslight1 != nullptr) {
                    //draw brake copy
                    if (this->Spline->getRailSet()->getCurAccelFactor() < 0.f || this->Spline->getCurSpeed() == 0.f)
                        tslight1->Draw(shaders);
                    //draw headlight copy
                    if (aiMap::Instance->drawHeadlights)
                        tslight1->Draw(shaders);
                }
            }

            //draw headlights
            if (aiVehicleInstance::AmbientHeadlightStyle < 3)
            {
                if (aiVehicleInstance::AmbientHeadlightStyle == 0 || aiVehicleInstance::AmbientHeadlightStyle == 2)
                {
                    //MM2 headlights
                    if (aiMap::Instance->drawHeadlights)
                    {
                        ltLight::DrawGlowBegin();

                        Vector3 someCameraThing = *(Vector3*)0x685490;

                        auto light = aiVehicleManager::Instance->getSharedLight();
                        auto lightPos = this->HeadlightPosition;
                        auto carMatrix = this->Spline->GetMatrix();

                        float posDiffX = carMatrix.m30 - someCameraThing.X;
                        float posDiffY = carMatrix.m31 - someCameraThing.Y;
                        float posDiffZ = carMatrix.m32 - someCameraThing.Z;

                        float invGlowScale = -ltLight::GlowScale;
                        posDiffX *= invGlowScale * 0.1f;
                        posDiffY *= invGlowScale * 0.1f;
                        posDiffZ *= invGlowScale * 0.1f;

                        light->Direction = Vector3(-carMatrix.m20, -carMatrix.m21, -carMatrix.m22);

                        modStatic* headlight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 19)->GetHighLOD();
                        if (headlight0 != nullptr)
                        {
                            float lX = lightPos.Y * carMatrix.m10 + lightPos.Z * carMatrix.m20 + lightPos.X * carMatrix.m00 + carMatrix.m30 + posDiffX;
                            float lY = lightPos.Y * carMatrix.m11 + lightPos.Z * carMatrix.m21 + lightPos.X * carMatrix.m01 + carMatrix.m31 + posDiffY;
                            float lZ = lightPos.Y * carMatrix.m12 + lightPos.Z * carMatrix.m22 + lightPos.X * carMatrix.m02 + carMatrix.m32 + posDiffZ;

                            light->Position = Vector3(lX, lY, lZ);
                            light->DrawGlow(someCameraThing);
                        }

                        modStatic* headlight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 20)->GetHighLOD();
                        if (headlight1 != nullptr)
                        {
                            float lX = lightPos.Y * carMatrix.m10 + lightPos.Z * carMatrix.m20 + -lightPos.X * carMatrix.m00 + carMatrix.m30 + posDiffX;
                            float lY = lightPos.Y * carMatrix.m11 + lightPos.Z * carMatrix.m21 + -lightPos.X * carMatrix.m01 + carMatrix.m31 + posDiffY;
                            float lZ = lightPos.Y * carMatrix.m12 + lightPos.Z * carMatrix.m22 + -lightPos.X * carMatrix.m02 + carMatrix.m32 + posDiffZ;

                            light->Position = Vector3(lX, lY, lZ);
                            light->DrawGlow(someCameraThing);
                        }

                        ltLight::DrawGlowEnd();
                    }
                }

                if (aiVehicleInstance::AmbientHeadlightStyle == 1 || aiVehicleInstance::AmbientHeadlightStyle == 2)
                {
                    //MM1 headlights
                    Matrix44::Convert(gfxRenderState::sm_World, this->Spline->GetMatrix());
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                    if (hlight != nullptr && aiMap::Instance->drawHeadlights)
                        hlight->Draw(shaders);
                }
            }
        }

        AGE_API void DrawReflected(float a1) override              { hook::Thunk<0x552CB0>::Call<void>(this, a1); }
        AGE_API unsigned int SizeOf() override                     { return hook::Thunk<0x553060>::Call<int>(this); }
        AGE_API phBound* GetBound(int a1) override                 { return hook::Thunk<0x552F40>::Call<phBound*>(this, a1); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleInstance, lvlInstance>("aiVehicleInstance")
                //members
                .addFunction("GetData", &GetData)
                .endClass();
        }
    };
    ASSERT_SIZEOF(aiVehicleInstance, 0x3C);

    // Lua initialization

}