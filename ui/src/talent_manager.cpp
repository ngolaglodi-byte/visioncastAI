/// @file talent_manager.cpp
/// @brief TalentManager implementation — table + detail panel.

#include "visioncast_ui/talent_manager.h"

#include <QColor>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

namespace visioncast_ui {

namespace {
const QStringList kAnimations = {
    "none", "fade_in", "fade_out",
    "slide_left", "slide_right", "slide_up", "slide_down",
    "scale_in", "scale_out",
};

const QStringList kOverlayTemplates = {
    "overlays/templates/default.json",
    "overlays/templates/modern_blue.json",
    "overlays/templates/minimal.json",
};
} // namespace

// =====================================================================
// Construction
// =====================================================================

TalentManager::TalentManager(QWidget* parent)
    : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    setupTable(root);
    setupDetailPanel(root);
    setLayout(root);
}

// =====================================================================
// Table setup
// =====================================================================

void TalentManager::setupTable(QVBoxLayout* parent) {
    model_ = new QStandardItemModel(0, 3, this);
    model_->setHorizontalHeaderLabels({"ID", "Name", "Role"});

    talentTable_ = new QTableView(this);
    talentTable_->setModel(model_);
    talentTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    talentTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    talentTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    talentTable_->horizontalHeader()->setStretchLastSection(true);
    parent->addWidget(talentTable_);

    connect(talentTable_->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &TalentManager::onSelectionChanged);

    auto* buttonLayout = new QHBoxLayout();
    addButton_ = new QPushButton("Add", this);
    removeButton_ = new QPushButton("Remove", this);
    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(removeButton_);
    parent->addLayout(buttonLayout);

    connect(addButton_, &QPushButton::clicked,
            this, &TalentManager::onAddTalent);
    connect(removeButton_, &QPushButton::clicked,
            this, &TalentManager::onRemoveTalent);
}

// =====================================================================
// Detail panel setup
// =====================================================================

void TalentManager::setupDetailPanel(QVBoxLayout* parent) {
    detailGroup_ = new QGroupBox("Talent Details", this);
    auto* form = new QFormLayout(detailGroup_);

    // Name
    nameEdit_ = new QLineEdit(detailGroup_);
    form->addRow("Name:", nameEdit_);

    // Role
    roleEdit_ = new QLineEdit(detailGroup_);
    form->addRow("Role:", roleEdit_);

    // Organisation
    orgEdit_ = new QLineEdit(detailGroup_);
    form->addRow("Organisation:", orgEdit_);

    // Photo path + thumbnail preview
    photoEdit_ = new QLineEdit(detailGroup_);
    form->addRow("Photo:", photoEdit_);

    photoPreview_ = new QLabel(detailGroup_);
    photoPreview_->setFixedSize(80, 80);
    photoPreview_->setScaledContents(true);
    form->addRow("", photoPreview_);

    // Overlay template
    overlayCombo_ = new QComboBox(detailGroup_);
    overlayCombo_->addItems(kOverlayTemplates);
    form->addRow("Overlay:", overlayCombo_);

    // Theme colour
    auto* colorRow = new QHBoxLayout();
    colorEdit_ = new QLineEdit(detailGroup_);
    colorEdit_->setPlaceholderText("#RRGGBB");
    colorSwatch_ = new QLabel(detailGroup_);
    colorSwatch_->setFixedSize(24, 24);
    colorSwatch_->setAutoFillBackground(true);
    colorRow->addWidget(colorEdit_);
    colorRow->addWidget(colorSwatch_);
    form->addRow("Colour:", colorRow);

    connect(colorEdit_, &QLineEdit::textChanged, this, [this](const QString& text) {
        QColor c(text);
        if (c.isValid()) {
            QPalette pal = colorSwatch_->palette();
            pal.setColor(QPalette::Window, c);
            colorSwatch_->setPalette(pal);
        }
    });

    // Animations
    entryAnimCombo_ = new QComboBox(detailGroup_);
    entryAnimCombo_->addItems(kAnimations);
    form->addRow("Entry animation:", entryAnimCombo_);

    exitAnimCombo_ = new QComboBox(detailGroup_);
    exitAnimCombo_->addItems(kAnimations);
    form->addRow("Exit animation:", exitAnimCombo_);

    animDurationSpin_ = new QSpinBox(detailGroup_);
    animDurationSpin_->setRange(0, 10000);
    animDurationSpin_->setSuffix(" ms");
    animDurationSpin_->setValue(500);
    form->addRow("Duration:", animDurationSpin_);

    // Apply button
    applyButton_ = new QPushButton("Apply", detailGroup_);
    form->addRow(applyButton_);

    connect(applyButton_, &QPushButton::clicked,
            this, &TalentManager::onApplyDetails);

    detailGroup_->setLayout(form);
    detailGroup_->setEnabled(false);
    parent->addWidget(detailGroup_);
}

// =====================================================================
// Database I/O
// =====================================================================

void TalentManager::loadDatabase(const QString& path) {
    dbPath_ = path;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("TalentManager: cannot open %s", qPrintable(path));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject())
        return;

    QJsonArray arr = doc.object().value("talents").toArray();
    model_->removeRows(0, model_->rowCount());

    for (int i = 0; i < arr.size(); ++i) {
        QJsonObject obj = arr[i].toObject();
        QList<QStandardItem*> row;
        row << new QStandardItem(obj.value("talent_id").toString());
        row << new QStandardItem(obj.value("name").toString());
        row << new QStandardItem(obj.value("role").toString());

        // Store the full JSON object as user data on the first column item.
        QJsonDocument tmpDoc(obj);
        row[0]->setData(QString(tmpDoc.toJson(QJsonDocument::Compact)),
                        Qt::UserRole);

        model_->appendRow(row);
    }

    clearDetailPanel();
}

void TalentManager::saveDatabase(const QString& path) {
    QString target = path.isEmpty() ? dbPath_ : path;
    if (target.isEmpty())
        return;

    QJsonArray arr;
    for (int r = 0; r < model_->rowCount(); ++r) {
        QString raw = model_->item(r, 0)->data(Qt::UserRole).toString();
        QJsonDocument tmpDoc = QJsonDocument::fromJson(raw.toUtf8());
        if (tmpDoc.isObject())
            arr.append(tmpDoc.object());
    }

    QJsonObject root;
    root["talents"] = arr;

    QFile file(target);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("TalentManager: cannot write %s", qPrintable(target));
        return;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();

    emit databaseUpdated();
}

// =====================================================================
// Selection handling
// =====================================================================

int TalentManager::selectedRow() const {
    auto indices = talentTable_->selectionModel()->selectedRows();
    if (indices.isEmpty())
        return -1;
    return indices.first().row();
}

void TalentManager::onSelectionChanged() {
    int row = selectedRow();
    if (row < 0) {
        clearDetailPanel();
        return;
    }

    QString raw = model_->item(row, 0)->data(Qt::UserRole).toString();
    QJsonObject obj = QJsonDocument::fromJson(raw.toUtf8()).object();

    nameEdit_->setText(obj.value("name").toString());
    roleEdit_->setText(obj.value("role").toString());
    orgEdit_->setText(obj.value("organization").toString());
    photoEdit_->setText(obj.value("photo").toString());

    // Thumbnail preview — resolve relative to database directory.
    QString photoPath = obj.value("photo").toString();
    if (!dbPath_.isEmpty() && !photoPath.isEmpty()) {
        QDir baseDir = QFileInfo(dbPath_).dir();   // e.g. talents/
        baseDir.cdUp();                             // project root
        QString absPhoto = baseDir.filePath(photoPath);
        QPixmap pix(absPhoto);
        if (!pix.isNull())
            photoPreview_->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
        else
            photoPreview_->clear();
    }

    // Overlay
    QString overlay = obj.value("overlay").toString();
    int idx = overlayCombo_->findText(overlay);
    overlayCombo_->setCurrentIndex(idx >= 0 ? idx : 0);

    // Colour
    colorEdit_->setText(obj.value("theme_color").toString());

    // Animations
    QJsonObject anims = obj.value("animations").toObject();
    int entryIdx = entryAnimCombo_->findText(anims.value("entry").toString());
    entryAnimCombo_->setCurrentIndex(entryIdx >= 0 ? entryIdx : 0);

    int exitIdx = exitAnimCombo_->findText(anims.value("exit").toString());
    exitAnimCombo_->setCurrentIndex(exitIdx >= 0 ? exitIdx : 0);

    animDurationSpin_->setValue(anims.value("duration_ms").toInt(500));

    detailGroup_->setEnabled(true);
}

void TalentManager::clearDetailPanel() {
    nameEdit_->clear();
    roleEdit_->clear();
    orgEdit_->clear();
    photoEdit_->clear();
    photoPreview_->clear();
    overlayCombo_->setCurrentIndex(0);
    colorEdit_->clear();
    entryAnimCombo_->setCurrentIndex(0);
    exitAnimCombo_->setCurrentIndex(0);
    animDurationSpin_->setValue(500);
    detailGroup_->setEnabled(false);
}

// =====================================================================
// Actions
// =====================================================================

void TalentManager::onAddTalent() {
    // Generate a unique talent_id by finding the highest existing numeric suffix.
    int maxId = 0;
    for (int r = 0; r < model_->rowCount(); ++r) {
        QString id = model_->item(r, 0)->text();
        if (id.startsWith("talent_")) {
            bool ok = false;
            int n = id.mid(7).toInt(&ok);
            if (ok && n > maxId)
                maxId = n;
        }
    }

    QJsonObject obj;
    obj["talent_id"] = QString("talent_%1").arg(maxId + 1);
    obj["name"] = "";
    obj["role"] = "";
    obj["organization"] = "";
    obj["photo"] = "";
    obj["overlay"] = kOverlayTemplates.first();
    obj["theme_color"] = "#FFFFFF";

    QJsonObject anims;
    anims["entry"] = "fade_in";
    anims["exit"] = "fade_out";
    anims["duration_ms"] = 500;
    obj["animations"] = anims;

    QList<QStandardItem*> row;
    row << new QStandardItem(obj.value("talent_id").toString());
    row << new QStandardItem("");
    row << new QStandardItem("");

    QJsonDocument tmpDoc(obj);
    row[0]->setData(QString(tmpDoc.toJson(QJsonDocument::Compact)),
                    Qt::UserRole);
    model_->appendRow(row);

    // Select the new row so the detail panel activates.
    talentTable_->selectRow(model_->rowCount() - 1);
}

void TalentManager::onRemoveTalent() {
    int row = selectedRow();
    if (row >= 0)
        model_->removeRow(row);
    clearDetailPanel();
}

void TalentManager::onApplyDetails() {
    int row = selectedRow();
    if (row < 0)
        return;

    // Rebuild JSON from detail fields.
    QString raw = model_->item(row, 0)->data(Qt::UserRole).toString();
    QJsonObject obj = QJsonDocument::fromJson(raw.toUtf8()).object();

    obj["name"] = nameEdit_->text();
    obj["role"] = roleEdit_->text();
    obj["organization"] = orgEdit_->text();
    obj["photo"] = photoEdit_->text();
    obj["overlay"] = overlayCombo_->currentText();
    obj["theme_color"] = colorEdit_->text();

    QJsonObject anims;
    anims["entry"] = entryAnimCombo_->currentText();
    anims["exit"] = exitAnimCombo_->currentText();
    anims["duration_ms"] = animDurationSpin_->value();
    obj["animations"] = anims;

    // Persist back into the model.
    QJsonDocument tmpDoc(obj);
    model_->item(row, 0)->setData(
        QString(tmpDoc.toJson(QJsonDocument::Compact)), Qt::UserRole);

    // Update visible table columns.
    model_->item(row, 0)->setText(obj.value("talent_id").toString());
    model_->item(row, 1)->setText(obj.value("name").toString());
    model_->item(row, 2)->setText(obj.value("role").toString());

    emit databaseUpdated();
}

} // namespace visioncast_ui
