#include "NeoBackendLayer.hpp"

#include <algorithm>
#include <sstream>
#include <string>

using namespace cocos2d;
using namespace neo::intercept;

namespace {

    std::string getSourceName(RequestSource source) {
        switch (source) {
            case RequestSource::NeoBackend:
                return "NEO";

            case RequestSource::GeometryDash:
                return "GD";

            case RequestSource::Geode:
                return "GEODE";
        }

        return "UNKNOWN";
    }

    std::string getStateName(RequestState state) {
        switch (state) {
            case RequestState::Pending:
                return "PENDING";

            case RequestState::Completed:
                return "COMPLETED";

            case RequestState::Failed:
                return "FAILED";
        }

        return "UNKNOWN";
    }

    std::string shorten(
        const std::string& value,
        size_t maxLength
    ) {
        if (value.size() <= maxLength) {
            return value;
        }

        return value.substr(0, maxLength) + "...";
    }

    CCLabelTTF* makeLabel(
        const std::string& text,
        float size
    ) {
        return CCLabelTTF::create(
            text.c_str(),
            "Arial",
            size
        );
    }
}

// ==========================================
// CREATE
// ==========================================

NeoBackendLayer* NeoBackendLayer::create() {
    auto ret = new NeoBackendLayer();

    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

// ==========================================
// OPEN
// ==========================================

void NeoBackendLayer::open() {
    auto layer = NeoBackendLayer::create();

    if (!layer) {
        return;
    }

    auto scene = CCScene::create();

    if (!scene) {
        return;
    }

    scene->addChild(layer);

    CCDirector::sharedDirector()->pushScene(
        CCTransitionFade::create(0.2f, scene)
    );
}

// ==========================================
// INIT
// ==========================================

bool NeoBackendLayer::init() {
    if (!CCLayer::init()) {
        return false;
    }

    auto winSize =
        CCDirector::sharedDirector()->getWinSize();

    auto background = CCLayerColor::create(
        ccc4(13, 18, 32, 245)
    );

    addChild(background);

    // Title

    auto title = makeLabel(
        "NEO BACKEND INSPECTOR",
        23.0f
    );

    title->setPosition(
        ccp(winSize.width / 2, winSize.height - 35)
    );

    addChild(title);

    // Status

    m_statusLabel = makeLabel(
        "LIVE MONITOR | READY",
        12.0f
    );

    m_statusLabel->setPosition(
        ccp(winSize.width / 2, winSize.height - 62)
    );

    addChild(m_statusLabel);

    // Close button

    auto closeItem = CCMenuItemFont::create(
        "CLOSE",
        this,
        menu_selector(NeoBackendLayer::onClose)
    );

    auto closeMenu = CCMenu::create(
        closeItem,
        nullptr
    );

    closeMenu->setPosition(
        ccp(winSize.width - 55, winSize.height - 35)
    );

    addChild(closeMenu);

    // Filters

    struct FilterButton {
        const char* name;
        SEL_MenuHandler callback;
    };

    FilterButton filters[] = {
        {"ALL", menu_selector(NeoBackendLayer::onFilterAll)},
        {"NEO", menu_selector(NeoBackendLayer::onFilterNeo)},
        {"GD", menu_selector(NeoBackendLayer::onFilterGD)},
        {"GEODE", menu_selector(NeoBackendLayer::onFilterGeode)}
    };

    auto filterMenu = CCMenu::create();

    float filterX = 0.0f;

    for (const auto& filter : filters) {
        auto item = CCMenuItemFont::create(
            filter.name,
            this,
            filter.callback
        );

        item->setPosition(
            ccp(filterX, 0)
        );

        filterX += 75.0f;

        filterMenu->addChild(item);
    }

    filterMenu->setPosition(
        ccp(
            winSize.width / 2 - 110,
            winSize.height - 100
        )
    );

    addChild(filterMenu);

    // Details panel

    m_detailsLabel = makeLabel(
        "Select a request to inspect its details.",
        13.0f
    );

    m_detailsLabel->setAnchorPoint(
        ccp(0, 1)
    );

    m_detailsLabel->setPosition(
        ccp(
            winSize.width / 2 + 15,
            winSize.height - 135
        )
    );

    addChild(m_detailsLabel);

    // Initial history

// Initial history

std::weak_ptr<std::atomic<bool>> weakDirty = m_dirtySignal;

m_listenerID =
    NeoActivityManager::get().addListener(
        [weakDirty]() {
            if (auto dirty = weakDirty.lock()) {
                dirty->store(true);
            }
        }
    );

m_dirtySignal->store(true);

scheduleUpdate();

return true;
}
    
// ==========================================
// UPDATE
// ==========================================

void NeoBackendLayer::update(float dt) {
    CCLayer::update(dt);

    if (m_dirtySignal && m_dirtySignal->exchange(false)) {
        refreshList();
        refreshDetails();
    }
}

// ==========================================
// REFRESH REQUEST LIST
// ==========================================

void NeoBackendLayer::refreshList() {
    auto winSize =
        CCDirector::sharedDirector()->getWinSize();

    // Remove previous rows.

    removeChildByTag(1000, true);

    auto listContainer = CCNode::create();

    listContainer->setTag(1000);

    addChild(listContainer);

    auto history =
        NeoActivityManager::get().getHistory();

    std::reverse(history.begin(), history.end());

    m_visibleIDs.clear();

    float y = winSize.height - 145.0f;

    int displayed = 0;

    for (const auto& request : history) {

        bool matches = false;

        switch (m_filter) {
            case 0:
                matches = true;
                break;

            case 1:
                matches =
                    request.source == RequestSource::NeoBackend;
                break;

            case 2:
                matches =
                    request.source == RequestSource::GeometryDash;
                break;

            case 3:
                matches =
                    request.source == RequestSource::Geode;
                break;
        }

        if (!matches) {
            continue;
        }

        if (displayed >= 10) {
            break;
        }

        m_visibleIDs.push_back(request.id);

        std::stringstream text;

        text << "#"
             << request.id
             << " | "
             << getSourceName(request.source)
             << " | "
             << request.method
             << " | "
             << shorten(request.url, 28)
             << " | ";

        if (request.responseCode > 0) {
            text << request.responseCode;
        }
        else {
            text << getStateName(request.state);
        }

        auto item = CCMenuItemFont::create(
            text.str().c_str(),
            this,
            menu_selector(NeoBackendLayer::onSelectRequest)
        );

        item->setTag(displayed);

        item->setAnchorPoint(ccp(0, 0.5f));

        auto menu = CCMenu::create(
            item,
            nullptr
        );

        menu->setPosition(
            ccp(25, y)
        );

        listContainer->addChild(menu);

        y -= 32.0f;
        displayed++;
    }

    std::stringstream status;

    status << "LIVE MONITOR | "
           << history.size()
           << " REQUESTS";

    m_statusLabel->setString(
        status.str().c_str()
    );
}

// ==========================================
// REQUEST DETAILS
// ==========================================

void NeoBackendLayer::refreshDetails() {
    if (m_selectedID == 0) {
        return;
    }

    auto history =
        NeoActivityManager::get().getHistory();

    auto it = std::find_if(
        history.begin(),
        history.end(),
        [this](const NeoRequest& request) {
            return request.id == m_selectedID;
        }
    );

    if (it == history.end()) {
        m_selectedID = 0;

        m_detailsLabel->setString(
            "Selected request is no longer available."
        );

        return;
    }

    const auto& request = *it;

    std::stringstream details;

    details << "REQUEST #" << request.id << "\n\n";

    details << "SOURCE: "
            << getSourceName(request.source) << "\n";

    details << "METHOD: "
            << request.method << "\n";

    details << "URL:\n"
            << request.url << "\n\n";

    details << "STATE: "
            << getStateName(request.state) << "\n";

    details << "HTTP: "
            << request.responseCode << "\n";

    details << "TIME: "
            << request.durationMs << " ms\n\n";

    details << "REQUEST HEADERS:\n";

    for (const auto& [key, value] : request.requestHeaders) {
        details << key << ": " << value << "\n";
    }

    details << "\nREQUEST BODY:\n"
            << shorten(request.requestBody, 500)
            << "\n\n";

    details << "RESPONSE BODY:\n"
        << shorten(request.responseBody, 500);

    if (!request.error.empty()) {
        details << "\n\nERROR:\n"
                << request.error;
    }

    m_detailsLabel->setString(
        details.str().c_str()
    );
}

// ==========================================
// SELECT REQUEST
// ==========================================

void NeoBackendLayer::onSelectRequest(
    CCObject* sender
) {
    auto item = static_cast<CCMenuItemFont*>(sender);

    int index = item->getTag();

    if (
        index < 0 ||
        static_cast<size_t>(index) >= m_visibleIDs.size()
    ) {
        return;
    }

    m_selectedID = m_visibleIDs[index];

    refreshDetails();
}

// ==========================================
// FILTERS
// ==========================================

void NeoBackendLayer::onFilterAll(CCObject*) {
    m_filter = 0;
    m_dirtySignal->store(true);
}

void NeoBackendLayer::onFilterNeo(CCObject*) {
    m_filter = 1;
    m_dirtySignal->store(true);
}

void NeoBackendLayer::onFilterGD(CCObject*) {
    m_filter = 2;
    m_dirtySignal->store(true);
}

void NeoBackendLayer::onFilterGeode(CCObject*) {
    m_filter = 3;
    m_dirtySignal->store(true);
}

// ==========================================
// CLOSE
// ==========================================

void NeoBackendLayer::onClose(CCObject*) {
    CCDirector::sharedDirector()->popScene();
}

// ==========================================
// CLEANUP
// ==========================================

void NeoBackendLayer::onExit() {
    if (m_listenerID != 0) {
        NeoActivityManager::get().removeListener(
            m_listenerID
        );

        m_listenerID = 0;
    }

    m_dirtySignal.reset();

    CCLayer::onExit();
}

NeoBackendLayer::~NeoBackendLayer() = default;
