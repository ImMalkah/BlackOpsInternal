#pragma once
#include "Feature.h"

class ConstantRadarFeature final : public FeatureBase
{
public:
    ConstantRadarFeature();

    void OnMenu() override;
    void OnUpdate() override;
    void OnDisable() override;
};

extern ConstantRadarFeature g_constantRadarFeature;
