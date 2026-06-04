#pragma once

#include <vector>
#include <string>
#include <cstdint>

class Feature;

namespace FeatureManager
{
    void RegisterFeatures();
    void Shutdown();

    void RegisterConfigs();

    void OnUpdate();
    void OnRender();
    void DrawMenu();
    void DrawCategoryMenu(const char* category);
    void DrawMenuExcept(const std::vector<const char*>& excludedCategories);
    
    void DrawEntityListTab();

    const std::vector<Feature*>& GetFeatures();
}
