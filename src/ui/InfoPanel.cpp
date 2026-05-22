#include "ui/InfoPanel.hpp"

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace lumine::ui {

namespace {
constexpr int kPanelWidth = 250;
} // namespace

InfoPanel::InfoPanel(QWidget* parent) : QWidget(parent)
{
    setFixedWidth(kPanelWidth);
    setStyleSheet(
        QStringLiteral("QWidget { background: #161618; }"
                       "QLabel { color: #cfcfd6; font-size: 12px; }"
                       "QLabel[role=\"caption\"] { color: #6f6f7a; font-size: 11px; }"
                       "QLabel[role=\"title\"] { color: #ffffff; font-size: 14px;"
                       " font-weight: 600; }"));

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(18, 18, 18, 18);
    outer->setSpacing(14);

    auto* title = new QLabel(tr("Properties"), this);
    title->setProperty("role", "title");
    outer->addWidget(title);

    auto* formHost = new QWidget(this);
    m_form = new QFormLayout(formHost);
    m_form->setContentsMargins(0, 0, 0, 0);
    m_form->setVerticalSpacing(10);
    m_form->setHorizontalSpacing(12);
    m_form->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    outer->addWidget(formHost);
    outer->addStretch(1);
}

InfoPanel::~InfoPanel() = default;

void InfoPanel::setProperties(const QList<image::ImageProperty>& properties)
{
    while (m_form->rowCount() > 0) {
        m_form->removeRow(0); // removeRow deletes the row's widgets
    }

    for (const image::ImageProperty& property : properties) {
        auto* caption = new QLabel(property.label, this);
        caption->setProperty("role", "caption");

        auto* value = new QLabel(property.value, this);
        value->setWordWrap(true);
        value->setTextInteractionFlags(Qt::TextSelectableByMouse);

        m_form->addRow(caption, value);
    }
}

} // namespace lumine::ui
