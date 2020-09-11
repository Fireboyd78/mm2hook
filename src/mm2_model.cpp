#include "mm2_model.h"
using namespace MM2;

modStatic * modGetStatic(LPCSTR file, float &a2, bool a3) {
    return hook::StaticThunk<0x4A52E0>::Call<modStatic *>(file, &a2, a3);
}

modStatic * modGetStatic(LPCSTR file, void(*a2)(Vector3 &, void *), void *a3, bool a4) {
    return hook::StaticThunk<0x4A4E30>::Call<modStatic *>(file, a2, a3, a4);
}

modModel * modGetModel(LPCSTR filename, uint a2, bool a3, bool a4) {
    return hook::StaticThunk<0x597A70>::Call<modModel *>(filename, a2, a3, a4);
}

void modConvertModel(LPCSTR filename, LPCSTR newFilename, bool a3) {
    return hook::StaticThunk<0x597BD0>::Call<void>(filename, newFilename, a3);
}

bool MM2::GetPivot(Matrix34 &mtx, LPCSTR basename, LPCSTR file) {
    return hook::StaticThunk<0x4A5280>::Call<bool>(&mtx, basename, file);
}

/*
    modShader
*/
declfield(modShader::sm_Materials)(0x66191C);
declfield(modShader::sm_StaticMaterialCount)(0x661920);