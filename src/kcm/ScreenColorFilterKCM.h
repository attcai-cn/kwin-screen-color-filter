#pragma once

#include <kcmodule.h>

namespace Ui {
    class Form;
}

class ScreenColorFilterKCM : public KCModule
{
    Q_OBJECT
public:
#if (QT_VERSION_MAJOR >= 6)
    explicit ScreenColorFilterKCM(QObject* parent, const KPluginMetaData& args);
#else
    explicit ScreenColorFilterKCM(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
#endif

public Q_SLOTS:
    void defaults() override;
    void load() override;
    void save() override;

private:
    std::shared_ptr<Ui::Form> ui;
};
