#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    class cityTimeWeatherLighting;
    class sdlPage16;
    class mmCityInfo;
    class mmCityList;

    // External declarations
    extern class gfxTexture;
    extern class Stream;

    namespace $
    {
        declhook(0x6299A8, _Type<cityTimeWeatherLighting[16]>, timeWeathers);

        namespace cityLevel
        {
            declhook(0x445820, _Func<void>::ThisCall, DrawRooms);
            declhook(0x443E50, _Func<void>::ThisCall, SetObjectDetail);
        }
        namespace sdlCommon
        {
            declhook(0x45CBC0, _Func<bool>, BACKFACE);
            declhook(0x448090, _Func<void>, UpdateLighting);
        }
        namespace sdlPage16
        {
            declhook(0x45A4E0, _MemberFunc<void>, $$ctor);

            declhook(0x448330, _MemberFunc<void>, Draw);

            declhook(0x45A560, _MemberFunc<void>, ArcMap);
            declhook(0x45A760, _MemberFunc<void>, WallMap);
            declhook(0x45A900, _MemberFunc<bool>, PointInPerimeter);

            declhook(0x45A9F0, _MemberFunc<void>, GetCentroid);
            declhook(0x45D110, _MemberFunc<int>, GetPerimeterCount);
            declhook(0x45D120, _MemberFunc<int>, GetPerimeterVertexIndex);
            declhook(0x45D140, _MemberFunc<const MM2::Vector3 &>, GetCodedVertex);
            declhook(0x45D160, _MemberFunc<float>, GetFloat);
            declhook(0x45D170, _MemberFunc<MM2::gfxTexture *>, GetTexture);

            declhook(0x450880, _Func<uint>, GetShadedColor$1);
            declhook(0x450910, _Func<uint>, GetShadedColor$2);

            declhook(0x45BF90, _Func<MM2::sdlPage16 *>, LoadBinary);
        }
    }

    class cityTimeWeatherLighting {
    public:
        /*0x00*/float KeyHeading;
        /*0x04*/float KeyPitch;
        /*0x08*/Vector3 KeyColor;

        /*0x14*/float Fill1Heading;
        /*0x18*/float Fill1Pitch;
        /*0x1C*/Vector3 Fill1Color;

        /*0x28*/float Fill2Heading;
        /*0x2C*/float Fill2Pitch;
        /*0x30*/Vector3 Fill2Color;

        // "ambient light levels" that get calculated using the Ambient color?
        // doesn't care for any of the Key/Pitch values
        /*0x3C*/int AmbientLevel3; // fullbright
        /*0x40*/int AmbientLevel2;
        /*0x44*/int AmbientLevel1;

        /*0x48*/int Ambient; // looks like the alpha is ignored
    };

    typename typedef void(*SDLIteratorCB)(const void *, int, int, int, const ushort *, void *);

    class sdlCommon {
    public:
        static ageHook::Type<Vector3> sm_CamPos;
        static ageHook::Type<uint> sm_LightTable;

        AGE_API static void UpdateLighting(void) {
            $::sdlCommon::UpdateLighting();
        }

        AGE_API static bool BACKFACE(const Vector3 &vec1, const Vector3 &vec2) {
            return $::sdlCommon::BACKFACE(&vec1, &vec2);
        }
    };

    class sdlPage16 {
        char unk_00;
        char PerimeterCount;
        char unk_02;
        char unk_03;
        Vector3 *CodedVertices;
        float *Floats;
        int unk_0C;
        unsigned short *PerimeterVertices;
        unsigned short *Attributes;
        gfxTexture **Textures;
        uint unk_1C;
    public:
        AGE_API sdlPage16(int p1, int p2) {
            $::sdlPage16::$$ctor(this, p1, p2);
        }

        AGE_API static sdlPage16 * LoadBinary(Stream *stream) {
            return $::sdlPage16::LoadBinary(stream);
        }

        AGE_API void Draw(int lod, uint baseColor) const {
            $::sdlPage16::Draw(this, lod, baseColor);
        }

        AGE_API void ArcMap(float *p1, const ushort *p2, int p3, int p4, int p5) const {
            $::sdlPage16::ArcMap(this, p1, p2, p3, p4, p5);
        }

        AGE_API void WallMap(float *p1, const ushort *p2, float p3, int p4, int p5) const {
            $::sdlPage16::WallMap(this, p1, p2, p3, p4, p5);
        }

        AGE_API bool PointInPerimeter(float p1, float p2) const {
            return $::sdlPage16::PointInPerimeter(this, p1, p2);
        }

        AGE_API void GetCentroid(Vector3 &p1) const {
            $::sdlPage16::GetCentroid(this, &p1);
        }

        AGE_API int GetPerimeterCount(void) const {
            return $::sdlPage16::GetPerimeterCount(this);
        }

        AGE_API int GetPerimeterVertexIndex(int p1) const {
            return $::sdlPage16::GetPerimeterVertexIndex(this, p1);
        }

        AGE_API const Vector3 & GetCodedVertex(int p1) const {
            return $::sdlPage16::GetCodedVertex(this, p1);
        }

        AGE_API float GetFloat(int p1) const {
            return $::sdlPage16::GetFloat(this, p1);
        }

        AGE_API gfxTexture * GetTexture(int p1) const {
            return $::sdlPage16::GetTexture(this, p1);
        }

        /* these are originally private, but they're pretty helpful */

        AGE_API static uint GetShadedColor(uint p1, uint p2) {
            return $::sdlPage16::GetShadedColor$1(p1, p2);
        }

        AGE_API static uint GetShadedColor(uint p1, uint p2, uint p3) {
            return $::sdlPage16::GetShadedColor$2(p1, p2, p3);
        }
    };

    class mmCityInfo {
    private:
        char localisedName[40];
        char mapName[40];
        char raceDir[40];

        char *blitzNames;
        char *checkpointNames;
        char *circuitNames;
        
        BOOL isValid;

        int blitzCount;
        int checkpointCount;
        int circuitCount;

        int _unk[2]; // ???

        inline char * getRaceNamesPtr(int mode) {
            switch (mode) {
                case 1: return checkpointNames;
                case 3: return circuitNames;
                case 4: return blitzNames;
            }
            return NULL;
        }

        inline int getRaceNamesCount(int mode) {
            switch (mode) {
                case 1: return checkpointCount;
                case 3: return circuitCount;
                case 4: return blitzCount;
            }
            return -1;
        }
    public:
        AGE_API mmCityInfo(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x52A540>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~mmCityInfo(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x52A560>::Call<void>(this);
            POP_VTABLE();
        }

        inline char * GetLocalisedName(void) {
            return localisedName;
        }

        inline char * GetMapName(void) {
            return mapName;
        }

        inline char * GetRaceDir(void) {
            return raceDir;
        }

        inline int GetRaceNames(int mode, char *buffer) {
            char *names = getRaceNamesPtr(mode);

            if (names != NULL) {
                strcpy(buffer, names);
                return getRaceNamesCount(mode);
            }

            return -1;
        }
    };

    class mmCityList {
    private:
        mmCityInfo **cityInfos;
        int numCities;
        int curCity;
    public:
        AGE_API mmCityList(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x524160>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~mmCityList(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x524180>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API int GetCityID(char *city)                   { return ageHook::Thunk<0x524270>::Call<int>(this, city); }

        AGE_API mmCityInfo * GetCityInfo(int city)          { return ageHook::Thunk<0x5241F0>::Call<mmCityInfo *>(this, city); }
        AGE_API mmCityInfo * GetCityInfo(char *city)        { return ageHook::Thunk<0x524220>::Call<mmCityInfo *>(this, city); }

        AGE_API mmCityInfo * GetCurrentCity(void)           { return ageHook::Thunk<0x524320>::Call<mmCityInfo *>(this); }
    };

    declhook(0x6B1CA0, _Type<mmCityList *>, CityListPtr);
}