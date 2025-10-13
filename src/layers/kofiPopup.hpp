#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

#include "../tagsManager.hpp"
#include "uiSquare.hpp"

using namespace geode::prelude;

class KofiPopup : public geode::Popup<bool> {
protected:
    bool setup(bool a);
    void open(CCObject*);
public:
    static KofiPopup* create(bool a);
};