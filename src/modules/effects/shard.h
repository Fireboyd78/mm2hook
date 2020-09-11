#pragma once
#include <modules\effects.h>

namespace MM2
{
    // Forward declarations
    class fxShard;
    class fxShardManager;

    // External declarations


    // Class definitions
    class fxShard {
    private:
        Vector3 Velocity;
        Vector3 RotationAxis;
        float AliveTime;
        float RotationSpeed;
        Matrix34 Matrix;
        float TexUCoord;
        float TexVCoord;
        int dword_58;
    public:
        AGE_API void Draw(modShader* shader)                { hook::Thunk<0x45FE30>::Call<void>(this, shader); }
    };

    class fxShardManager : public asNode {
    private:
        fxShard* Shards;
        int ShardCount;
        int LastShardIndex;
        float ImpulseThreshold;
        float ImpulseEmitRatio;
        float XDamp;
        float YDamp;
        float ZDamp;
        float RotationSpeedBase;
        float SpeedThreshold;
        int InstanceNumber;
        modShader* ShaderSet;
        int ShaderCount;
    public:
        void Draw() {
            if (this->ShaderCount == 0)
                return;

            //shader to draw with
            int curShader = 0;
            int shaderSkipAmount = 2;
            int maxShader = this->ShaderCount;
            
            auto renderState = &RSTATE;
            auto renderStateData = &renderState->Data;
            
            //set cull mode
            byte oldCullMode = renderStateData->CullMode;
            if (renderStateData->CullMode != 1) {
                renderStateData->CullMode = 1;
                *(int*)0x685778 |= 1; //set m_Touched
            }
            
            //draw shards
            for (int i = 0; i < this->ShardCount; i++) {
                auto shard = &this->Shards[i];
                auto shader = &this->ShaderSet[curShader];
                shard->Draw(shader);

                //move to next shader, and loop back to 0 if needed
                curShader += shaderSkipAmount;
                if (curShader >= maxShader)
                    curShader = 0;
            }

            //reset cull mode
            if (renderStateData->CullMode != oldCullMode) {
                renderStateData->CullMode = oldCullMode;
                *(int*)0x685778 |= 1; //set m_Touched
            }

            //finish off
            memcpy(&gfxRenderState::sm_World, (Matrix44*)0x6A3B48, sizeof(Matrix44));
            *(int*)0x685778 |= 0x88; //set m_Touched
        }
    };

    // Lua initialization

}