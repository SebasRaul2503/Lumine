#pragma once

#include "image/ImageInfo.hpp"

#include <QList>
#include <QWidget>

class QFormLayout;

namespace lumine::ui {

// A slim side panel that lists the current image's properties as label/value
// rows. Purely presentational — it renders whatever rows it is handed and
// holds no state of its own.
class InfoPanel : public QWidget {
    Q_OBJECT

public:
    explicit InfoPanel(QWidget* parent = nullptr);
    ~InfoPanel() override;

    // Replaces the displayed rows.
    void setProperties(const QList<image::ImageProperty>& properties);

private:
    QFormLayout* m_form = nullptr;
};

} // namespace lumine::ui
