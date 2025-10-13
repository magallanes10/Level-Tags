#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include "levelCell.hpp"

using namespace geode::prelude;

class $modify(TagsLevelBrowserLayer, LevelBrowserLayer) {
    struct Fields {
        matjson::Value tags;
        EventListener<web::WebTask> m_listener;
        int m_listeners = 0;
    };
    void loadLevelsFinished(CCArray* p0, char const* p1, int p2) override;
    void loadPage(GJSearchObject* p0);
};