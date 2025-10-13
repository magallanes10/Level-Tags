#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

#include "../tagsManager.hpp"
#include "uiSquare.hpp"

using namespace geode::prelude;

class CreditsPopup : public geode::Popup<bool> {
protected:
    EventListener<web::WebTask> m_listener;
    CCSprite* arrowSpr;

    bool setup(bool a);
    CCNode* tabSprite(std::string name, std::string role);
    void switchPage(CCObject* sender);
    void btn(CCObject* sender);
    void docs(CCObject* sender);
public:
    static CreditsPopup* create(bool a);
};