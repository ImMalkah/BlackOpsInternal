#pragma once
#include "Feature.h"

class OverheadNamesFeature final : public FeatureBase
{
public:
    OverheadNamesFeature();

    void OnMenu() override;

    bool ShouldDisable() const { return m_disableNames; }
    bool ShouldForceEnemyNames() const { return m_forceEnemyNames; }
    int GetForcedDisplayMode() const { return m_forcedDisplayMode; }

private:
    bool m_disableNames = false;
    bool m_forceEnemyNames = false;
    int m_forcedDisplayMode = -1;
};

// Global instance to allow easy integration into the hooks
extern OverheadNamesFeature g_overheadNamesFeature;
