#include <memory>
#include <atomic>
#include <vector>
#include <cstdint>

#include "../activity/NeoActivityManager.hpp"

class NeoBackendLayer : public cocos2d::CCLayer {
protected:
    std::shared_ptr<std::atomic<bool>> m_dirtySignal =
        std::make_shared<std::atomic<bool>>(true);

    uint64_t m_listenerID = 0;
    uint64_t m_selectedID = 0;

    int m_filter = 0;

    std::vector<uint64_t> m_visibleIDs;

    cocos2d::CCLabelTTF* m_statusLabel = nullptr;
    cocos2d::CCLabelTTF* m_detailsLabel = nullptr;

    bool init() override;
    void update(float dt) override;
    void onExit() override;

    void refreshList();
    void refreshDetails();

    void onClose(cocos2d::CCObject*);
    void onSelectRequest(cocos2d::CCObject*);

    void onFilterAll(cocos2d::CCObject*);
    void onFilterNeo(cocos2d::CCObject*);
    void onFilterGD(cocos2d::CCObject*);
    void onFilterGeode(cocos2d::CCObject*);

public:
    ~NeoBackendLayer() override;

    static NeoBackendLayer* create();
    static void open();
};
