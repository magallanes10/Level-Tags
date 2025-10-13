#include "levelBrowserLayer.hpp"
#include "Geode/binding/GJSearchObject.hpp"
#include "Geode/binding/LevelBrowserLayer.hpp"

using namespace geode::prelude;

void TagsLevelBrowserLayer::loadPage(GJSearchObject* p0) {
    m_fields->m_listener.setFilter(web::WebTask{});

    LevelBrowserLayer::loadPage(p0);
}

void TagsLevelBrowserLayer::loadLevelsFinished(CCArray* p0, char const* p1, int p2) {
    LevelBrowserLayer::loadLevelsFinished(p0, p1, p2);
    std::vector<int> levelIds;

    if (m_searchObject->m_searchType == SearchType::MyLevels) return;

    CCObject* obj;
    CCARRAY_FOREACH(p0, obj) {
        if (!static_cast<GJGameLevel*>(obj)->m_levelID) return;
        if (auto level = static_cast<GJGameLevel*>(obj); level && level->m_levelID.value() > 0) levelIds.push_back(level->m_levelID.value());
    }

    if (std::all_of(levelIds.begin(), levelIds.end(), [](int id) {
        return TagsManager::sharedState()->cachedTags[std::to_string(id)].size() != 0;
    })) {
        return;
    }

    int curList = ++m_fields->m_listeners;
    m_fields->m_listener.bind([this, curList](web::WebTask::Event* e) {
        if (curList != m_fields->m_listeners) return;
        if (auto res = e->getValue(); res && res->ok()) {
            auto jsonStr = res->string().unwrapOr("{}");
            auto json = matjson::parse(jsonStr).unwrapOr("{}");
                
            for (const auto& [id, tags] : json) {
                if (tags.size() == 0) continue;
                TagsManager::sharedState()->cachedTags[id] = tags;
            }
            if (m_list->m_listView->m_tableView->getChildren()->count() == 0) return;
            auto contentLayer = static_cast<CCContentLayer*>(m_list->m_listView->m_tableView->getChildren()->objectAtIndex(0));
            auto children = contentLayer->getChildren();
            for (int i = 0; i < children->count(); ++i) {
                if (auto levelCell = static_cast<TagsLevelCell*>(children->objectAtIndex(i))) {
                    if (TagsManager::sharedState()->cachedTags[std::to_string(levelCell->m_level->m_levelID.value())].size() == 0) continue;
                    levelCell->m_fields->tags = TagsManager::sortTags(TagsManager::sharedState()->cachedTags[std::to_string(levelCell->m_level->m_levelID.value())]);
                    if (levelCell->m_fields->tags.size() == 0) continue;
                    levelCell->adjustPositions();
                    levelCell->updateTags(false);
                }
            }
        }
    });

    auto req = web::WebRequest();
    std::string ids;
    for (size_t i = 0; i < levelIds.size(); ++i) {
        ids += std::to_string(levelIds[i]);
        if (i != levelIds.size() - 1) ids += ",";
    }

    m_fields->m_listener.setFilter(req.get(fmt::format("{}/get?id={}", Mod::get()->getSettingValue<std::string>("serverUrl"), ids)));
}