#pragma once

class Feature
{
public:
    virtual ~Feature() = default;

    virtual const char* GetName() const = 0;
    virtual const char* GetCategory() const = 0;

    virtual bool IsEnabled() const = 0;
    virtual void SetEnabled(bool enabled) = 0;

    virtual void OnEnable() {}
    virtual void OnDisable() {}

    virtual void RegisterConfig() {}

    virtual void OnUpdate() {}
    virtual void OnRender() {}
    virtual void OnMenu() {}
};

class FeatureBase : public Feature
{
public:
    FeatureBase(const char* name, const char* category)
        : m_name(name), m_category(category)
    {
    }

    const char* GetName() const override { return m_name; }
    const char* GetCategory() const override { return m_category; }

    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }

protected:
    bool m_enabled = false;

private:
    const char* m_name = "";
    const char* m_category = "";
};
