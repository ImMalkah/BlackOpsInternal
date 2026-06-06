#pragma once
#include "Feature.h"

class DebugInfoFeature final : public FeatureBase
{
public:
    DebugInfoFeature();

    void OnMenu() override;
    void OnUpdate() override;
    void OnDisable() override;

private:
    bool m_showFPS = false;
    bool m_showLagometer = false;
    bool m_showViewPos = false;
};

extern DebugInfoFeature g_debugInfoFeature;
