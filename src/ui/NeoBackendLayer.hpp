
#pragma once

#include <Geode/Geode.hpp>

#include "../activity/NeoActivityManager.hpp"

#include <atomic>
#include <vector>
#include <cstdint>

class NeoBackendLayer : public cocos2d::CCLayer {
protected:
    std::atomic<bool> m_dirty{true};

    uint64_t m_listenerID = 0;
    uint64_t m_selectedID = 0;

    int m_filter = 0;

    std::vector<uint64_t> m_visibleIDs;

    cocos2d::CCLabelTTF* m_statusLabel = nullptr;
    cocos2d::CCLabelTTF* m_detailsLabel = nullptr;

    bool init() override;

    void refreshList();
    void refreshDetails();

    void onClose(cocos2d::CCObject*);
    void onFilterAll(cocos2d::CCObject*);
    void onFilterNeo(cocos2d::CCObject*);
    void onFilterGD(cocos2d::CCObject*);
    void onFilterGeode(cocos2d::CCObject*);
    void onSelectRequest(cocos2d::CCObject*);

    void update(float dt) override;

public:
    ~NeoBackendLayer() override;

    static NeoBackendLayer* create();
    static void open();

    void onExit() override;
};
