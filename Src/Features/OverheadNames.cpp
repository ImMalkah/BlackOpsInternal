#include "OverheadNames.h"
#include "imgui.h"

OverheadNamesFeature::OverheadNamesFeature() 
    : FeatureBase("Overhead Names Block", "Visuals") 
{
}

void OverheadNamesFeature::OnMenu()
{
    ImGui::Checkbox("Disable Overhead Names", &m_disableNames);
    ImGui::Checkbox("Show Enemy Names (Hook sub_5E2CB0)", &m_forceEnemyNames);
    ImGui::SliderInt("Force Display Mode", &m_forcedDisplayMode, -1, 5, "%d (-1 = Default)");
}

// Define the global instance
OverheadNamesFeature g_overheadNamesFeature;
