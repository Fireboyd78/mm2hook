#include "mm2_game.h"
using namespace MM2;

/*
    mmGame
*/
HOOK_SET(0x412550, _MemberFunc<void>, $mmGame_$$ctor);
HOOK_SET(0x413940, _MemberFunc<void>, $mmGame_$$dtor);
HOOK_SET(0x413E90, _MemberFunc<void>, $mmGame_Update);
HOOK_SET(0x4144A0, _MemberFunc<void>, $mmGame_UpdatePaused);
HOOK_SET(0x413D40, _MemberFunc<void>, $mmGame_Reset);
HOOK_SET(0x412710, _MemberFunc<int>, $mmGame_Init);
HOOK_SET(0x413650, _MemberFunc<void>, $mmGame_InitGameStrings);
HOOK_SET(0x4133F0, _MemberFunc<void>, $mmGame_InitOtherPlayers);
HOOK_SET(0x414290, _MemberFunc<void>, $mmGame_HitWaterHandler);
HOOK_SET(0x414280, _MemberFunc<void>, $mmGame_DropThruCityHandler);
HOOK_SET(0x414E50, _MemberFunc<void>, $mmGame_SendChatMessage);
HOOK_SET(0x414D30, _MemberFunc<void>, $mmGame_BeDone);

mmGame::mmGame(void) {
    PUSH_VTABLE();
    $mmGame_$$ctor(this);
    POP_VTABLE();
}

mmGame::~mmGame(void) {
    PUSH_VTABLE();
    $mmGame_$$dtor(this);
    POP_VTABLE();
};

int mmGame::Init(void) {
    return $mmGame_Init(this);
};

void mmGame::InitGameStrings(void) {
    $mmGame_InitGameStrings(this);
};

void mmGame::InitOtherPlayers(void) {
    $mmGame_InitOtherPlayers(this);
}

void mmGame::HitWaterHandler(void) {
    $mmGame_HitWaterHandler(this);
}

void mmGame::DropThruCityHandler(void) {
    $mmGame_DropThruCityHandler(this);
}

void mmGame::SendChatMessage(char * message) {
    $mmGame_SendChatMessage(this, message);
}

void mmGame::BeDone(int p1) {
    $mmGame_BeDone(this, p1);
}

/*
    mmGameManager
*/
HOOK_SET(0x5E0D08, _Type<MM2::mmGameManager *>, $mmGameManager_Instance);

mmGameManager * mmGameManager::Instance(void) {
    return $mmGameManager_Instance;
}

/*
    mmGameMusicData
*/
HOOK_SET(0x434060, _MemberFunc<bool>, $mmGameMusicData_LoadAmbientSFX);

bool mmGameMusicData::LoadAmbientSFX(LPCSTR name) {
    return $mmGameMusicData_LoadAmbientSFX(this, name);
}

/*
    mmHUD
*/
HOOK_SET(0x42E1F0, _MemberFunc<void>, $mmHUD_SetMessage$1);
HOOK_SET(0x42E240, _MemberFunc<void>, $mmHUD_SetMessage$2);
HOOK_SET(0x42D280, _MemberFunc<void>, $mmHUD_PostChatMessage);

void mmHUD::SetMessage(LPCSTR message, float duration, int p2) {
    $mmHUD_SetMessage$1(this, message, duration, p2);
}

void mmHUD::SetMessage(LPCSTR message) {
    $mmHUD_SetMessage$2(this, message);
}

void mmHUD::PostChatMessage(LPCSTR message) {
    $mmHUD_PostChatMessage(this, message);
}

/*
    mmPopup
*/
HOOK_SET(0x42A280, _MemberFunc<int>,  $mmPopup_IsEnabled);
HOOK_SET(0x42B4F0, _MemberFunc<void>, $mmPopup_Lock);
HOOK_SET(0x42B500, _MemberFunc<void>, $mmPopup_Unlock);
HOOK_SET(0x42A400, _MemberFunc<void>, $mmPopup_ProcessChat);

int mmPopup::IsEnabled(void) {
    return $mmPopup_IsEnabled(this);
}

void mmPopup::Lock(void) {
    $mmPopup_Lock(this);
}

void mmPopup::Unlock(void) {
    $mmPopup_Unlock(this);
}

void mmPopup::ProcessChat(void) {
    $mmPopup_ProcessChat(this);
}