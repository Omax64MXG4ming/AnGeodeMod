#include <Geode/Bindings.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MenuLayer) {
	void onFristStart(CCObject*) {
		FLAlertLayer::create(
			"Geode",
			"V",
			"OK"
		)->show();
	}
};
