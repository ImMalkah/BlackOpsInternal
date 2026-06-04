#pragma once
#include "Feature.h"

class PlayerESPFeature final : public FeatureBase
{
public:
    PlayerESPFeature();

    void OnMenu() override;
    void OnRender() override;

    bool IsShowNames() const { return m_showNames; }
    bool IsShowBoxes() const { return m_showBoxes; }
    bool IsShowTeammates() const { return m_showTeammates; }
    float GetMaxDistance() const { return m_maxDistance; }

private:
    bool m_showNames = false;
    bool m_showBoxes = false;
    bool m_showTeammates = true;
    float m_maxDistance = 150.0f;
};

// Global instance to allow easy registration in FeatureManager
extern PlayerESPFeature g_playerESPFeature;
