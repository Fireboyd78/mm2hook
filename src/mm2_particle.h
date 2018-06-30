#pragma once
#include "mm2_common.h"
#include "mm2_level.h"
#include "mm2_base.h"
#include "mm2_vector.h"
#include "mm2_rgl.h"

namespace MM2
{
    // Forward declarations
    class asSparkPos;
    class asMeshCardInfo;

    struct asSparkPos
    {
        byte TexCoordOffset;
        byte OffsetMask;
        byte SomeFlag;
        float Scale;
        uint Color;
        float ShadowHeight;
        Vector3 Pos;
    };

    struct asMeshCardInfo
    {
        int Count;
        uint PositionsIndex;
        Vector2 *Positions;
        Vector2 *TexCoords;

        void asMeshCardInfo::Draw(asSparkPos *sparks, int sparkCount)
        {
            byte currentCullMode = *(byte*)0x6856BB;

            for (int i = 0; i < sparkCount; ++i)
            {
                asSparkPos* spark = &sparks[i];

                Vector2* positions = &this->Positions[4 * ((this->PositionsIndex - 1) & spark->OffsetMask)];
                Vector2* texcoords = &this->TexCoords[4 * spark->TexCoordOffset];

                vglCurrentColor = spark->Color;

                vglBegin(gfxDrawMode::DRAWMODE_TRIANGLEFAN, 0);
                for (int j = 0; j < this->Count; ++j)
                {
                    int aj = currentCullMode != 2 ? this->Count - j - 1 : j;

                    vglTexCoord2f(texcoords[aj].X, texcoords[aj].Y);

                    float scaleX = spark->Scale * positions[aj].X;
                    float scaleY = spark->Scale * positions[aj].Y;

                    float z = gfxRenderState::sm_Camera->m23 * scaleY + gfxRenderState::sm_Camera->m13 * scaleX + spark->Pos.Z;
                    float y = gfxRenderState::sm_Camera->m22 * scaleY + gfxRenderState::sm_Camera->m12 * scaleX + spark->Pos.Y;
                    float x = gfxRenderState::sm_Camera->m21 * scaleY + gfxRenderState::sm_Camera->m11 * scaleX + spark->Pos.X;

                    vglVertex(x, y, z);
                }
                vglEnd();
            }
        }
    };
}