#include "credits.hpp"
#include "Geode/binding/CCMenuItemSpriteExtra.hpp"
#include "Geode/cocos/cocoa/CCObject.h"

bool CreditsPopup::setup(bool a) {
    setTitle("Staff");
    m_title->setPositionY(204.f);

    m_mainLayer->addChild(TagsManager::sharedState()->addBgAnim(m_size));
    m_mainLayer->addChild(TagsManager::sharedState()->addCorners(m_size, 1.0f));

    auto descMenu = UIsquare::create(true, {305.f, 160.f});
    descMenu->setPosition({m_mainLayer->getContentWidth() / 2, 105});
    m_mainLayer->addChild(descMenu);

    arrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    arrowSpr->setFlipX(true);

    auto pageArrow = CCMenuItemSpriteExtra::create(arrowSpr, this, menu_selector(CreditsPopup::switchPage));
    pageArrow->setPosition(360.f, m_mainLayer->getContentHeight() / 2);
    m_closeBtn->getParent()->addChild(pageArrow);

    auto loading = LoadingSpinner::create(20.f);
    loading->setPosition({m_mainLayer->getContentWidth() / 2, 105});
    m_mainLayer->addChild(loading);

    m_listener.bind([this, loading](web::WebTask::Event* e) {
        if (auto res = e->getValue(); res && res->ok()) {
            auto jsonStr = res->string().unwrapOr("{}");
            auto json = matjson::parse(jsonStr).unwrapOr("{}");

            if (loading) m_mainLayer->removeChild(loading);
            
            auto staffMenu = CCMenu::create();
            staffMenu->setPosition({m_mainLayer->getContentWidth() / 2, 180});
            staffMenu->setContentWidth(300);
            staffMenu->setAnchorPoint({0.5, 1});
            staffMenu->setLayout(AxisLayout::create()->setGrowCrossAxis(true)->setAutoScale(false)->setGap(5));
            staffMenu->setID("staff-menu");
            m_mainLayer->addChild(staffMenu);

            auto stMenu = CCMenu::create();
            stMenu->setPosition({m_mainLayer->getContentWidth() / 2, 180});
            stMenu->setContentWidth(300);
            stMenu->setAnchorPoint({0.5, 1});
            stMenu->setVisible(false);
            stMenu->setLayout(AxisLayout::create()->setGrowCrossAxis(true)->setAutoScale(false)->setGap(5));
            stMenu->setID("special-thanks-menu");
            m_mainLayer->addChild(stMenu);
            
            for (const auto& [key, value] : json) {
                auto role = value[0].asString().unwrapOr("");
                auto btn = CCMenuItemSpriteExtra::create( tabSprite(key, role), this, menu_selector(CreditsPopup::btn));
                btn->setID(key);
                btn->setTag(value[1].asInt().unwrapOr(0));
                if (role == "developer" || role == "admin" || role == "moderator" || role == "helper") {
                    staffMenu->addChild(btn);
                } else {
                    stMenu->addChild(btn);
                }
            }
        
            auto docs = CCMenuItemSpriteExtra::create( tabSprite("GD Styles List", "Mod Inspiration"), this, menu_selector(CreditsPopup::docs));
            docs->setID("Docs");
            stMenu->addChild(docs);
        
            staffMenu->updateLayout();
            stMenu->updateLayout();
        }
    });

    auto req = web::WebRequest();
    m_listener.setFilter(req.get(fmt::format("{}/credits", Mod::get()->getSettingValue<std::string>("serverUrl"))));

    return true;
}

CCNode* CreditsPopup::tabSprite(std::string name, std::string role) {
    auto bg = UIsquare::create(false, {70.f, 25.f});
    bg->setPosition({bg->getContentWidth() / 2, bg->getContentHeight() / 2});

    auto label = CCLabelBMFont::create(name.c_str(), "goldFont.fnt");
    label->setPosition({bg->getContentWidth() / 2, 18});
    label->limitLabelWidth(60, 0.6, 0.2);
    bg->addChild(label);

    ccColor3B color = { 255, 255, 255 };
    if (role == "developer") color = { 255, 150, 230 };
    else if (role == "admin") color = { 255, 50, 50 };
    else if (role == "moderator") color = { 200, 100, 0 };
    else if (role == "helper") color = { 0, 200, 255 };
    else if (role == "supporter") color = { 200, 0, 255 };

    auto roleLabel = CCLabelBMFont::create(role.c_str(), "bigFont.fnt");
    roleLabel->setPosition({bg->getContentWidth() / 2, 8});
    roleLabel->setColor(color);
    roleLabel->limitLabelWidth(60, 0.25, 0.2);
    bg->addChild(roleLabel);

    return bg;
}

void CreditsPopup::switchPage(CCObject* sender) {
    arrowSpr->setFlipX(!arrowSpr->isFlipX());
    if (auto btn = static_cast<CCMenuItemSpriteExtra*>(sender); btn->getPositionX() == 360.f) {
        btn->setPositionX(-20.f);
        setTitle("Special Thanks");
    } else {
        btn->setPositionX(360.f);
        setTitle("Staff");
    }
    m_mainLayer->getChildByID("staff-menu")->setVisible(!m_mainLayer->getChildByID("staff-menu")->isVisible());
    m_mainLayer->getChildByID("special-thanks-menu")->setVisible(!m_mainLayer->getChildByID("special-thanks-menu")->isVisible());
}

void CreditsPopup::btn(CCObject* sender) {
    ProfilePage::create(sender->getTag(), false)->show();
}

void CreditsPopup::docs(CCObject* sender) {
    web::openLinkInBrowser("https://x.com/stylesofgd");
}

CreditsPopup* CreditsPopup::create(bool a) {
    auto ret = new CreditsPopup();
    if (ret->initAnchored(340.f, 220.f, a, "square.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}