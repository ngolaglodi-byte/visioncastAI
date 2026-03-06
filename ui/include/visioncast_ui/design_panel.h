#pragma once

/// @file design_panel.h
/// @brief Design & overlays panel — lower thirds, animated templates,
///        themes, colors, logos, and transitions.

#include <QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;

namespace visioncast_ui {

/// Design panel that exposes the six overlay-related sub-sections
/// requested for the broadcast UI: lower thirds, animated templates,
/// themes, colors, logos and transitions.
class DesignPanel : public QWidget {
    Q_OBJECT

public:
    explicit DesignPanel(QWidget* parent = nullptr);

    // -- Lower thirds ------------------------------------------------
    void    setLowerThirdTitle(const QString& title);
    void    setLowerThirdSubtitle(const QString& subtitle);
    QString lowerThirdTitle() const;
    QString lowerThirdSubtitle() const;

    // -- Templates ---------------------------------------------------
    QString currentTemplateName() const;

    // -- Themes ------------------------------------------------------
    void    setTheme(const QString& themeName);
    QString currentTheme() const;

    // -- Colors ------------------------------------------------------
    void    setPrimaryColor(const QString& hex);
    void    setAccentColor(const QString& hex);
    QString primaryColor() const;
    QString accentColor() const;

    // -- Logo --------------------------------------------------------
    void    setLogoPath(const QString& path);
    QString logoPath() const;

    // -- Transitions -------------------------------------------------
    void    setTransitionType(const QString& type);
    void    setTransitionDuration(int ms);
    QString transitionType() const;
    int     transitionDuration() const;

    // -- Configuration -----------------------------------------------
    void loadConfig(const QString& path);
    void saveConfig(const QString& path);

signals:
    void designChanged();
    void lowerThirdApplied(const QString& title, const QString& subtitle);
    void themeChanged(const QString& themeName);
    void colorsChanged(const QString& primary, const QString& accent);
    void logoChanged(const QString& path);
    void transitionChanged(const QString& type, int durationMs);

private:
    // Lower thirds
    QLineEdit*   lowerThirdTitleEdit_    = nullptr;
    QLineEdit*   lowerThirdSubtitleEdit_ = nullptr;
    QPushButton* applyLowerThirdButton_  = nullptr;

    // Templates
    QComboBox* templateCombo_ = nullptr;

    // Themes
    QComboBox* themeCombo_ = nullptr;

    // Colors
    QLineEdit*   primaryColorEdit_ = nullptr;
    QLineEdit*   accentColorEdit_  = nullptr;
    QPushButton* pickPrimaryButton_ = nullptr;
    QPushButton* pickAccentButton_  = nullptr;

    // Logo
    QLineEdit*   logoPathEdit_     = nullptr;
    QPushButton* browseLogoButton_ = nullptr;

    // Transitions
    QComboBox* transitionCombo_        = nullptr;
    QSpinBox*  transitionDurationSpin_ = nullptr;

    // Tabs
    QTabWidget* tabs_ = nullptr;

    QWidget* createLowerThirdsTab();
    QWidget* createTemplatesTab();
    QWidget* createThemesTab();
    QWidget* createColorsTab();
    QWidget* createLogosTab();
    QWidget* createTransitionsTab();
};

} // namespace visioncast_ui
