// VisionCast-AI — Licence officielle Prestige Technologie Company,
// développée par Glody Dimputu Ngola.

pragma Singleton

import QtQuick 2.15

/// VCTheme — global design token singleton.
/// Import as: import "../theme" then use VCTheme.<token>
QtObject {
    // ── Colors ────────────────────────────────────────────────────
    readonly property color bgPrimary:   "#0D1117"
    readonly property color bgSecondary: "#161B22"
    readonly property color bgTertiary:  "#1C2128"
    readonly property color surface:     "#21262D"
    readonly property color border:      "#30363D"
    readonly property color borderHover: "#484F58"

    readonly property color textPrimary:   "#E6EDF3"
    readonly property color textSecondary: "#8B949E"
    readonly property color textDisabled:  "#484F58"

    readonly property color accentBlue:   "#1F6FEB"
    readonly property color accentBlue2:  "#58A6FF"
    readonly property color accentPurple: "#A855F7"
    readonly property color liveRed:      "#F85149"
    readonly property color successGreen: "#3FB950"
    readonly property color warningAmber: "#D29922"

    // ── Semantic ──────────────────────────────────────────────────
    readonly property color selectionBg:  "#1F6FEB33"

    // ── Typography ────────────────────────────────────────────────
    readonly property string fontFamily:    "\"Segoe UI\", \"Inter\", \"Helvetica Neue\", Arial, sans-serif"
    readonly property string fontMono:      "\"JetBrains Mono\", \"Cascadia Code\", \"Fira Code\", monospace"

    readonly property int fontSizeXs:  10
    readonly property int fontSizeSm:  11
    readonly property int fontSizeMd:  13
    readonly property int fontSizeLg:  16
    readonly property int fontSizeXl:  20
    readonly property int fontSizeH1:  28

    // ── Spacing ───────────────────────────────────────────────────
    readonly property int spaceXs:  4
    readonly property int spaceSm:  8
    readonly property int spaceMd:  12
    readonly property int spaceLg:  16
    readonly property int spaceXl:  24
    readonly property int spaceXxl: 32

    // ── Radii ─────────────────────────────────────────────────────
    readonly property int radiusSm: 4
    readonly property int radiusMd: 6
    readonly property int radiusLg: 8

    // ── Elevation (drop shadow opacity) ───────────────────────────
    readonly property real shadowOpacity: 0.4

    // ── Animations ────────────────────────────────────────────────
    readonly property int durationFast:   150
    readonly property int durationNormal: 200
    readonly property int durationSlow:   300
    readonly property int easingType:     Easing.OutCubic

    // ── Layout constants ──────────────────────────────────────────
    readonly property int topBarHeight:    48
    readonly property int leftColumnWidth: 300
    readonly property int rightColumnWidth:360
    readonly property int bottomBarHeight: 200
}
