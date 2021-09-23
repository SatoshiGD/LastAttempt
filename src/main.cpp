#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gd.h>
#include <MinHook.h>
#include "utils.hpp"

using namespace cocos2d;
using namespace gd;

struct PlayerObjectData {
    float m_x = 0.0f, 
        m_y = 0.0f, 
        m_rotation = 0.0f;
    int m_mode = 0;
    PlayerObjectData(PlayerObject* player) {
        m_x = player->getPosition().x;
        m_y = player->getPosition().y;
        m_rotation = player->getRotation();
        // to-do : mode, color
    }
};

struct PlayerReplay { 
    SimplePlayer* m_sprite = nullptr;
    std::vector<PlayerObjectData> m_data;
    std::vector<PlayerObjectData> m_playData;
    void setData(const int frame) {
        if (!m_sprite || frame >= m_playData.size()) return;
        m_sprite->setPositionX(m_playData.at(frame).m_x);
        m_sprite->setPositionY(m_playData.at(frame).m_y);
        m_sprite->setRotation(m_playData.at(frame).m_rotation);
    }
    void newAttempt() {
        m_playData = m_data;
        m_data.clear();
    }
    void createNew(CCLayer* objectLayer) {
        auto manager = GameManager::sharedState();
        m_sprite = SimplePlayer::create(manager->m_nPlayerFrame);
        m_sprite->setColor(manager->colorForIdx(manager->getPlayerColor()));
        m_sprite->setSecondColor(manager->colorForIdx(manager->getPlayerColor2()));
        m_sprite->setOpacity(100);
        objectLayer->addChild(m_sprite);
    }
};

class ext_PlayLayer : public PlayLayer {
public:
    // extra
    int m_frame = 0;
    static inline PlayerReplay s_sPlayer1;
    static inline PlayerReplay s_sPlayer2;

    // hooks
    static inline bool (__thiscall* o_init) (PlayLayer* self, GJGameLevel* level) = nullptr;
    bool h_init(GJGameLevel* level) {
        auto returnValue = ext_PlayLayer::o_init(this, level);
        s_sPlayer1.createNew(m_pObjectLayer);
        s_sPlayer2.createNew(m_pObjectLayer);
        return returnValue;
    }

    static inline void (__thiscall* o_update) (PlayLayer* self, const float delta) = nullptr;
    void h_update(const float delta) {
        if (m_pPlayer1->getPosition().x != 0.0f) { 
            s_sPlayer1.m_data.push_back(m_pPlayer1);
            s_sPlayer2.m_data.push_back(m_pPlayer2);
            m_frame++;
        }
        s_sPlayer1.setData(m_frame);
        s_sPlayer2.setData(m_frame);
        return ext_PlayLayer::o_update(this, CCDirector::sharedDirector()->getAnimationInterval());
    }

    static inline int (__thiscall* o_updateAttempts) (PlayLayer* self) = nullptr;
    int h_updateAttempts() {
        m_frame = 0;
        s_sPlayer1.newAttempt();
        s_sPlayer2.newAttempt();
		return ext_PlayLayer::o_updateAttempts(this);
	}

    static inline int(__thiscall* o_onExit) (PlayLayer* self) = nullptr;
    int h_onExit() {
        s_sPlayer1.m_sprite = nullptr;
        s_sPlayer2.m_sprite = nullptr;
        return ext_PlayLayer::o_onExit(this);
    }
};

DWORD WINAPI mainThread(LPVOID lpParam) {
    MH_Initialize();
    createHookOf(getFunctionPointer(0x1FB780), union_cast(&ext_PlayLayer::h_init), &ext_PlayLayer::o_init);
    createHookOf(getFunctionPointer(0x2029C0), union_cast(&ext_PlayLayer::h_update), &ext_PlayLayer::o_update); 
    createHookOf(getFunctionPointer(0x20CED0), union_cast(&ext_PlayLayer::h_updateAttempts), &ext_PlayLayer::o_updateAttempts);
    createHookOf(getFunctionPointer(0x20D810), union_cast(&ext_PlayLayer::h_onExit), &ext_PlayLayer::o_onExit);
    MH_EnableHook(MH_ALL_HOOKS);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, mainThread, hModule, NULL, NULL);
    }
    return TRUE;
}
