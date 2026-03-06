/// @file design_panel.cpp
/// @brief DesignPanel implementation.

#include "visioncast_ui/design_panel.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

namespace visioncast_ui {

// -----------------------------------------------------------------
// Construction
// -----------------------------------------------------------------

DesignPanel::DesignPanel(QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);

    tabs_ = new QTabWidget(this);
    tabs_->addTab(createLowerThirdsTab(),  "Lower Thirds");
    tabs_->addTab(createTemplatesTab(),    "Templates");
    tabs_->addTab(createThemesTab(),       "Themes");
    tabs_->addTab(createColorsTab(),       "Colors");
    tabs_->addTab(createLogosTab(),        "Logos");
    tabs_->addTab(createTransitionsTab(),  "Transitions");

    root->addWidget(tabs_);
    setLayout(root);
}

// -----------------------------------------------------------------
// Tab factories
// -----------------------------------------------------------------

QWidget* DesignPanel::createLowerThirdsTab() {
    auto* page   = new QWidget(this);
    auto* layout = new QFormLayout(page);

    lowerThirdTitleEdit_    = new QLineEdit(page);
    lowerThirdSubtitleEdit_ = new QLineEdit(page);
    applyLowerThirdButton_  = new QPushButton("Apply", page);

    layout->addRow("Title:",    lowerThirdTitleEdit_);
    layout->addRow("Subtitle:", lowerThirdSubtitleEdit_);
    layout->addRow(applyLowerThirdButton_);

    connect(applyLowerThirdButton_, &QPushButton::clicked, this, [this]() {
        emit lowerThirdApplied(lowerThirdTitleEdit_->text(),
                               lowerThirdSubtitleEdit_->text());
        emit designChanged();
    });

    return page;
}

QWidget* DesignPanel::createTemplatesTab() {
    auto* page   = new QWidget(this);
    auto* layout = new QFormLayout(page);

    templateCombo_ = new QComboBox(page);
    templateCombo_->addItems({"default", "modern_blue", "minimal"});

    layout->addRow("Template:", templateCombo_);

    connect(templateCombo_, &QComboBox::currentTextChanged,
            this, [this]() { emit designChanged(); });

    return page;
}

QWidget* DesignPanel::createThemesTab() {
    auto* page   = new QWidget(this);
    auto* layout = new QFormLayout(page);

    themeCombo_ = new QComboBox(page);
    themeCombo_->addItems({"Dark", "Light", "Broadcast", "Custom"});

    layout->addRow("Theme:", themeCombo_);

    connect(themeCombo_, &QComboBox::currentTextChanged,
            this, [this](const QString& name) {
        emit themeChanged(name);
        emit designChanged();
    });

    return page;
}

QWidget* DesignPanel::createColorsTab() {
    auto* page   = new QWidget(this);
    auto* layout = new QFormLayout(page);

    primaryColorEdit_ = new QLineEdit("#000000", page);
    accentColorEdit_  = new QLineEdit("#FFFFFF", page);
    pickPrimaryButton_ = new QPushButton("Pick…", page);
    pickAccentButton_  = new QPushButton("Pick…", page);

    layout->addRow("Primary Color:", primaryColorEdit_);
    layout->addRow("",               pickPrimaryButton_);
    layout->addRow("Accent Color:",  accentColorEdit_);
    layout->addRow("",               pickAccentButton_);

    auto emitColors = [this]() {
        emit colorsChanged(primaryColorEdit_->text(),
                           accentColorEdit_->text());
        emit designChanged();
    };

    connect(primaryColorEdit_, &QLineEdit::editingFinished, this, emitColors);
    connect(accentColorEdit_,  &QLineEdit::editingFinished, this, emitColors);

    return page;
}

QWidget* DesignPanel::createLogosTab() {
    auto* page   = new QWidget(this);
    auto* layout = new QFormLayout(page);

    logoPathEdit_     = new QLineEdit(page);
    browseLogoButton_ = new QPushButton("Browse…", page);

    layout->addRow("Logo Path:", logoPathEdit_);
    layout->addRow(browseLogoButton_);

    connect(browseLogoButton_, &QPushButton::clicked, this, [this]() {
        // In production this would open a QFileDialog.
        emit logoChanged(logoPathEdit_->text());
        emit designChanged();
    });

    return page;
}

QWidget* DesignPanel::createTransitionsTab() {
    auto* page   = new QWidget(this);
    auto* layout = new QFormLayout(page);

    transitionCombo_ = new QComboBox(page);
    transitionCombo_->addItems({"fade", "cut", "dissolve",
                                 "wipe_left", "wipe_right",
                                 "wipe_up", "wipe_down"});

    transitionDurationSpin_ = new QSpinBox(page);
    transitionDurationSpin_->setRange(0, 5000);
    transitionDurationSpin_->setSuffix(" ms");
    transitionDurationSpin_->setValue(500);

    layout->addRow("Type:",     transitionCombo_);
    layout->addRow("Duration:", transitionDurationSpin_);

    auto emitTransition = [this]() {
        emit transitionChanged(transitionCombo_->currentText(),
                               transitionDurationSpin_->value());
        emit designChanged();
    };

    connect(transitionCombo_, &QComboBox::currentTextChanged,
            this, emitTransition);
    connect(transitionDurationSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, emitTransition);

    return page;
}

// -----------------------------------------------------------------
// Lower thirds
// -----------------------------------------------------------------

void DesignPanel::setLowerThirdTitle(const QString& title) {
    lowerThirdTitleEdit_->setText(title);
}

void DesignPanel::setLowerThirdSubtitle(const QString& subtitle) {
    lowerThirdSubtitleEdit_->setText(subtitle);
}

QString DesignPanel::lowerThirdTitle() const {
    return lowerThirdTitleEdit_->text();
}

QString DesignPanel::lowerThirdSubtitle() const {
    return lowerThirdSubtitleEdit_->text();
}

// -----------------------------------------------------------------
// Templates
// -----------------------------------------------------------------

QString DesignPanel::currentTemplateName() const {
    return templateCombo_->currentText();
}

// -----------------------------------------------------------------
// Themes
// -----------------------------------------------------------------

void DesignPanel::setTheme(const QString& themeName) {
    themeCombo_->setCurrentText(themeName);
}

QString DesignPanel::currentTheme() const {
    return themeCombo_->currentText();
}

// -----------------------------------------------------------------
// Colors
// -----------------------------------------------------------------

void DesignPanel::setPrimaryColor(const QString& hex) {
    primaryColorEdit_->setText(hex);
}

void DesignPanel::setAccentColor(const QString& hex) {
    accentColorEdit_->setText(hex);
}

QString DesignPanel::primaryColor() const {
    return primaryColorEdit_->text();
}

QString DesignPanel::accentColor() const {
    return accentColorEdit_->text();
}

// -----------------------------------------------------------------
// Logo
// -----------------------------------------------------------------

void DesignPanel::setLogoPath(const QString& path) {
    logoPathEdit_->setText(path);
}

QString DesignPanel::logoPath() const {
    return logoPathEdit_->text();
}

// -----------------------------------------------------------------
// Transitions
// -----------------------------------------------------------------

void DesignPanel::setTransitionType(const QString& type) {
    transitionCombo_->setCurrentText(type);
}

void DesignPanel::setTransitionDuration(int ms) {
    transitionDurationSpin_->setValue(ms);
}

QString DesignPanel::transitionType() const {
    return transitionCombo_->currentText();
}

int DesignPanel::transitionDuration() const {
    return transitionDurationSpin_->value();
}

// -----------------------------------------------------------------
// Configuration (load / save)
// -----------------------------------------------------------------

void DesignPanel::loadConfig(const QString& path) {
    // TODO: Load design configuration from JSON file
    Q_UNUSED(path)
}

void DesignPanel::saveConfig(const QString& path) {
    // TODO: Save design configuration to JSON file
    Q_UNUSED(path)
}

} // namespace visioncast_ui
