#pragma once
#include <modules\vehicle.h>

#include "policecaraudio.h"
#include "nitrocaraudio.h"
#include "semicaraudio.h"
#include "caraudio.h"

namespace MM2
{
    // Forward declarations
    class vehCarAudioContainer;

    // External declarations
    extern class AudImpact;
    extern class Stream;

    // Class definitions

    class vehCarAudioContainer {
    private:
        int field_0;
        int VehType;
        vehCarAudio* CarAudioPtr;
        vehPoliceCarAudio* PoliceCarAudioPtr;
        vehSemiCarAudio* SemiCarAudioPtr;
        vehNitroCarAudio* NitroCarAudioPtr;
    public:
        inline int GetVehType() {
            return this->VehType;
        }

        inline void SetVehType(int type) {
            this->VehType = type;
        }

        inline vehCarAudio * GetCarAudioPtr() {
            return this->CarAudioPtr;
        }

        inline vehPoliceCarAudio * GetPoliceCarAudioPtr() {
            return this->PoliceCarAudioPtr;
        }

        inline vehSemiCarAudio * GetSemiCarAudioPtr() {
            return this->SemiCarAudioPtr;
        }

        inline vehNitroCarAudio * GetNitroCarAudio() {
            return this->NitroCarAudioPtr;
        }

        inline bool IsPlayer() {
            return this->VehType == 2;
        }

        AGE_API static bool IsPolice(const char* name) {
            return hook::StaticThunk<0x4D1A70>::Call<bool>(name);
        }

        AGE_API static bool IsSemiOrBus(const char* name) {
            return hook::StaticThunk<0x4D1A00>::Call<bool>(name);
        }
        
        AGE_API static void RegisterSemiNames(Stream* a1, LPCSTR a2) {
            hook::StaticThunk<0x4D0E30>::Call<void>(a1, a2);
        }

        AGE_API static void RegisterPoliceNames(Stream* a1, LPCSTR a2) {
            hook::StaticThunk<0x4D0F10>::Call<void>(a1, a2);
        }

        AGE_API static void SetSirenCSVName(LPCSTR name) {
            hook::StaticThunk<0x4D0C80>::Call<void>(name);
        };

        AGE_API void Set3D(bool a1) {
            hook::Thunk<0x4D1840>::Call<void>(this, a1);
        }

        AGE_API void StartSiren() {
            hook::Thunk<0x4D18B0>::Call<void>(this);
        }

        AGE_API void StopSiren() {
            hook::Thunk<0x4D18C0>::Call<void>(this);
        }

        AGE_API void PlayHorn()
        {
            if (!this->field_0)
            {
                auto carAudio = this->CarAudioPtr;
                if (carAudio != nullptr)
                    carAudio->PlayHorn();

                auto policeAudio = this->PoliceCarAudioPtr;
                if (policeAudio != nullptr)
                    policeAudio->PlayHorn();

                auto semiAudio = this->SemiCarAudioPtr;
                if (semiAudio != nullptr)
                    semiAudio->PlayHorn();

                auto nitroAudio = this->NitroCarAudioPtr;
                if (nitroAudio != nullptr)
                    nitroAudio->PlayHorn();

                this->field_0 = 1;
            }
        }

        AGE_API void StopHorn() {
            if (this->field_0)
            {
                auto carAudio = this->CarAudioPtr;
                if (carAudio != nullptr)
                    carAudio->StopHorn();

                auto policeAudio = this->PoliceCarAudioPtr;
                if (policeAudio != nullptr)
                    policeAudio->StopHorn();

                auto semiAudio = this->SemiCarAudioPtr;
                if (semiAudio != nullptr)
                    semiAudio->StopHorn();

                auto nitroAudio = this->NitroCarAudioPtr;
                if (nitroAudio != nullptr)
                    nitroAudio->StopHorn();

                this->field_0 = 0;
            }
        }

        AGE_API void SilenceEngine(int a1) {
            hook::Thunk<0x4D1910>::Call<void>(this, a1);
        }

        AGE_API AudImpact * GetAudImpactPtr() {
            return hook::Thunk<0x4D1730>::Call<AudImpact *>(this);
        };
    };
    ASSERT_SIZEOF(vehCarAudioContainer, 0x18);

    // Lua initialization

}