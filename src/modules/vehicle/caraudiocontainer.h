#pragma once
#include <modules\vehicle.h>

#include "policecaraudio.h"
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
    public:
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

        AGE_API void PlayHorn() {
            hook::Thunk<0x4D17C0>::Call<void>(this);
        }

        AGE_API void StopHorn() {
            hook::Thunk<0x4D1800>::Call<void>(this);
        }

        AGE_API void SilenceEngine(int a1) {
            hook::Thunk<0x4D1910>::Call<void>(this, a1);
        }

        AGE_API vehPoliceCarAudio * GetPoliceCarAudioPtr() {
            return hook::Thunk<0x4D1790>::Call<vehPoliceCarAudio *>(this);
        };

        AGE_API vehCarAudio * GetCarAudioPtr() {
            return hook::Thunk<0x4D1770>::Call<vehCarAudio*>(this);
        };

        AGE_API AudImpact * GetAudImpactPtr() {
            return hook::Thunk<0x4D1730>::Call<AudImpact *>(this);
        };
    };

    // Lua initialization

}