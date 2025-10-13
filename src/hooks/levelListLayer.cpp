#include <Geode/Geode.hpp>
#include <Geode/modify/LevelListLayer.hpp>
#include "../tagsManager.hpp"
#include "Geode/binding/CCContentLayer.hpp"
#include "levelCell.hpp"

using namespace geode::prelude;

class $modify(TagsLevelListLayer, LevelListLayer) {
    struct Fields {
        matjson::Value tags;
        EventListener<web::WebTask> m_listener;
    };

    $override
    void loadLevelsFinished(CCArray* p0, char const* p1, int p2) {
        std::vector<int> levelIds;

        CCObject* obj;
        CCARRAY_FOREACH(p0, obj) {
            if (auto level = static_cast<GJGameLevel*>(obj)) levelIds.push_back(level->m_levelID.value());
        }

        if (std::all_of(levelIds.begin(), levelIds.end(), [](int id) {
            return TagsManager::sharedState()->cachedTags[std::to_string(id)].size() != 0;
        })) {
            LevelBrowserLayer::loadLevelsFinished(p0, p1, p2);
            return;
        }

        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                auto jsonStr = res->string().unwrapOr("{}");
                auto json = matjson::parse(jsonStr).unwrapOr("{}");
                
                for (const auto& [id, tags] : json) {
                    if (tags.size() == 0) continue;
                    TagsManager::sharedState()->cachedTags[id] = tags;
                }
                auto contentLayer = static_cast<CCContentLayer*>(m_list->m_listView->m_tableView->getChildren()->objectAtIndex(0));
                auto children = contentLayer->getChildren();
                for (int i = 0; i < children->count(); ++i) {
                    
                    auto levelCell = static_cast<TagsLevelCell*>(children->objectAtIndex(i));
                    if (TagsManager::sharedState()->cachedTags[std::to_string(levelCell->m_level->m_levelID.value())].size() == 0) continue;
                    levelCell->m_fields->tags = TagsManager::sortTags(TagsManager::sharedState()->cachedTags[std::to_string(levelCell->m_level->m_levelID.value())]);
                    if (levelCell->m_fields->tags.size() == 0) return;
                    levelCell->adjustPositions();
                    levelCell->updateTags(false);
                }
            }
        });

        auto req = web::WebRequest();
        std::string ids;
        for (size_t i = 0; i < levelIds.size(); ++i) {
            ids += std::to_string(levelIds[i]);
            if (i != levelIds.size() - 1) ids += ",";
        }

        m_fields->m_listener.setFilter(req.get(
            fmt::format("{}/get?id={}", Mod::get()->getSettingValue<std::string>("serverUrl"), ids)
        ));

        LevelBrowserLayer::loadLevelsFinished(p0, p1, p2);
    }

    bool init(GJSearchObject* p0) {
        if (!LevelBrowserLayer::init(p0)) return false;

        return true;
    };
};