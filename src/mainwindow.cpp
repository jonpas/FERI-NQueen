#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QMetaEnum>
#include <QDebug>

const QPair<uint8_t, uint8_t> MainWindow::SIZE_RANGE = {4, 12};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    board = ui->tableWidgetBoard;
    populateUi();

    toggleAlgorithmOptions(0);
    setupBoard();
}

MainWindow::~MainWindow() {
    for (int i = 0; i < board->rowCount(); i++) {
        for (int j = 0; j < board->columnCount(); j++) {
            delete board->item(i, j);
        }
    }

    delete ui;
}

void MainWindow::populateUi() {
    // Size
    ui->comboBoxSize->blockSignals(true);
    ui->comboBoxSize->clear();
    for (uint8_t i = SIZE_RANGE.first; i <= SIZE_RANGE.second; i++) {
        ui->comboBoxSize->addItem(QString::number(i));
    }
    ui->comboBoxSize->blockSignals(false);

    // Placement
    ui->comboBoxPlacement->blockSignals(true);
    ui->comboBoxPlacement->clear();
    QMetaEnum metaPlacement = QMetaEnum::fromType<Placement>();
    for (int i = 0; i < metaPlacement.keyCount(); i++) {
        ui->comboBoxPlacement->addItem(prettifyCamelCase(metaPlacement.key(i)));
    }
    ui->comboBoxPlacement->setCurrentIndex(Placement::TopRow);
    ui->comboBoxPlacement->blockSignals(false);

    // Algorithm
    ui->comboBoxAlgorithm->blockSignals(true);
    ui->comboBoxAlgorithm->clear();
    QMetaEnum metaAlgorithm = QMetaEnum::fromType<Algorithm>();
    for (int i = 0; i < metaAlgorithm.keyCount(); i++) {
        ui->comboBoxAlgorithm->addItem(prettifyCamelCase(metaAlgorithm.key(i)));
    }
    ui->comboBoxAlgorithm->blockSignals(false);
}

void MainWindow::setupBoard() {
    int size = getBoardSize();
    QList<QPoint> queens = getQueenPositions();
    //Algorithm algorithm = getAlgorithm();

    board->setRowCount(size);
    board->setColumnCount(size);
    board->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    board->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QColor colorAlt(245, 222, 179); // Wheat

    for (int y = 0; y < board->rowCount(); y++) {
        for (int x = 0; x < board->columnCount(); x++) {
            QTableWidgetItem *item = board->item(y, x);

            // Create new item if it doesn't exist yet
            if (item == nullptr) {
                item = new QTableWidgetItem();
                board->setItem(y, x, item);

                // Align cell text to center
                // TODO Queen image instead of text
                item->setTextAlignment(Qt::AlignCenter);
            }

            // Alternate cell color
            if ((y + x) % 2 != 0) {
                item->setBackgroundColor(colorAlt);
            }

            // Clear image
            // TODO Queen image instead of text
            item->setText("");
        }
    }

    for (auto &queen : queens) {
        // TODO Queen image instead of text
        board->item(queen.x(), queen.y())->setText("QUEEN"); // X and Y swapped
    }
}

QList<QPoint> MainWindow::getQueenPositions() {
    int size = getBoardSize();
    Placement type = getPlacementType();

    QList<QPoint> positions;
    if (type == Placement::TopRow) {
        for (uint8_t i = 0; i < size; i++) {
            positions.push_back({0, i});
        }
    } else if (type == Placement::Random) {
        QRandomGenerator *gen = QRandomGenerator::global();
        QPoint point;
        while (positions.size() < size) {
            point = QPoint(gen->bounded(0, size), gen->bounded(0, size));
            if (!positions.contains(point)) {
                positions.push_back(point);
            }
        }
    }

    return positions;
}

void MainWindow::toggleAlgorithmOptions(int index) {
    for (int i = 0; i < ui->optionsAlgorithms->count(); i++) {
        QLayoutItem *item = ui->optionsAlgorithms->itemAt(i);
        item->widget()->setHidden(true);
    }

    // Unhide after all are hidden to prevent UI resizing glitches due to minimal sizes
    ui->optionsAlgorithms->itemAt(index)->widget()->setHidden(false);
}

int MainWindow::getBoardSize() {
    return ui->comboBoxSize->currentText().toInt();
}

MainWindow::Placement MainWindow::getPlacementType() {
    return static_cast<Placement>(ui->comboBoxPlacement->currentIndex());
}

MainWindow::Algorithm MainWindow::getAlgorithm() {
    return static_cast<Algorithm>(ui->comboBoxAlgorithm->currentIndex());
}

void MainWindow::on_comboBoxSize_currentIndexChanged(const QString &/*arg1*/) {
    setupBoard();
}

void MainWindow::on_comboBoxPlacement_currentIndexChanged(const QString &/*arg1*/) {
    setupBoard();
}

void MainWindow::on_pushButtonReset_clicked() {
    setupBoard();
}

void MainWindow::on_comboBoxAlgorithm_currentIndexChanged(int index) {
    toggleAlgorithmOptions(index);
}

QString MainWindow::prettifyCamelCase(const QString &s) {
    static QRegularExpression regExp1 {"(.)([A-Z][a-z]+)"};
    static QRegularExpression regExp2 {"([a-z0-9])([A-Z])"};

    QString result = s;
    result.replace(regExp1, "\\1 \\2");
    result.replace(regExp2, "\\1 \\2");

    return result;
}
