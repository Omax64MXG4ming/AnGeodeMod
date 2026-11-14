
#include <Geode/Geode.hpp>

#include "NeoBackendLayer.hpp"

using namespace geode::prelude;

class $modify(NeoMenuLayer, MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto winSize =
            CCDirector::sharedDirector()->getWinSize();

        // ======================================
        // NEO INSPECTOR BUTTON
        // ======================================

        auto buttonSprite = ButtonSprite::create(
            "NEO INSPECTOR",
            "goldFont.fnt",
            "GJ_button_01.png",
            0.65f
        );

        if (!buttonSprite) {
            return true;
        }

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(NeoMenuLayer::onNeoInspector)
        );

        if (!button) {
            return true;
        }

        auto menu = CCMenu::create();

        if (!menu) {
            return true;
        }

        button->setID("neo-inspector-button");

        menu->addChild(button);

        menu->setPosition(
            ccp(
                winSize.width - 85.0f,
                45.0f
            )
        );

        menu->setID("neo-inspector-menu");

        addChild(menu, 100);

        return true;
    }

    void onNeoInspector(CCObject*) {
        log::info("[NeoInspector] Opening Inspector");

        NeoBackendLayer::open();
    }
};
