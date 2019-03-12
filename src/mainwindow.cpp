#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "localsearch.h"

#include <QRandomGenerator>
#include <QMetaEnum>
#include <QDebug>

const QPair<uint8_t, uint8_t> MainWindow::SIZE_RANGE = {4, 12};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    board = ui->tableWidgetBoard;
    board->installEventFilter(this);
    populateUi();

    toggleAlgorithmOptions();
    generateQueens();
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
        ui->comboBoxPlacement->addItem(spaceCamelCase(metaPlacement.key(i)));
    }
    ui->comboBoxPlacement->setCurrentIndex(Placement::TopRow);
    ui->comboBoxPlacement->blockSignals(false);

    // Algorithm
    ui->comboBoxAlgorithm->blockSignals(true);
    ui->comboBoxAlgorithm->clear();
    QMetaEnum metaAlgorithm = QMetaEnum::fromType<Algorithm>();
    for (int i = 0; i < metaAlgorithm.keyCount(); i++) {
        ui->comboBoxAlgorithm->addItem(spaceCamelCase(metaAlgorithm.key(i)));
    }
    ui->comboBoxAlgorithm->setCurrentIndex(Algorithm::GeneticAlgorithm);
    ui->comboBoxAlgorithm->blockSignals(false);
}

void MainWindow::setupBoard() {
    int size = getBoardSize();

    board->setRowCount(size);
    board->setColumnCount(size);
    board->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    board->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QColor colorAlt(245, 222, 179); // Wheat

    for (int y = 0; y < board->rowCount(); y++) {
        for (int x = 0; x < board->columnCount(); x++) {
            QTableWidgetItem *item = board->item(x, y);

            // Create new item if it doesn't exist yet
            if (item == nullptr) {
                item = new QTableWidgetItem();
                board->setItem(x, y, item);
            }

            // Alternate cell color
            if ((x + y) % 2 != 0) {
                item->setBackgroundColor(colorAlt);
            }

            // Clear image
            item->setIcon(QIcon());
        }
    }

    for (auto &queen : queens) {
        QTableWidgetItem *item = board->item(queen.y(), queen.x());
        item->setIcon(QIcon(":/rsc/crown.png"));
    }
}

bool MainWindow::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::Resize) {
        board->setIconSize(board->size() / getBoardSize());
    }
    return false;
}

void MainWindow::generateQueens() {
    int size = getBoardSize();
    Placement type = getPlacementType();

    queens.clear();
    if (type == Placement::TopRow) {
        for (uint8_t i = 0; i < size; i++) {
            queens.push_back({i, 0});
        }
    } else if (type == Placement::Random) {
        QRandomGenerator *gen = QRandomGenerator::global();
        QPoint point;
        while (queens.size() < size) {
            point = QPoint(gen->bounded(0, size), gen->bounded(0, size));
            if (!queens.contains(point)) {
                queens.push_back(point);
            }
        }
    }
}

void MainWindow::toggleAlgorithmOptions() {
    Algorithm algorithm = getAlgorithm();

    // (Un)Hide options groups
    for (int i = 0; i < ui->optionsAlgorithms->count(); i++) {
        QLayoutItem *item = ui->optionsAlgorithms->itemAt(i);
        item->widget()->setHidden(i != algorithm);
    }

    // Enable Step mode only for supported algorithms
    ui->checkBoxRunStep->setEnabled(algorithm == Algorithm::HillClimbing || algorithm == Algorithm::SimulatedAnnealing);
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

QString MainWindow::getAlgorithmName() {
    return ui->comboBoxAlgorithm->currentText();
}

bool MainWindow::isStepsChecked() {
    return ui->checkBoxRunStep->isChecked();
}

void MainWindow::on_comboBoxSize_currentIndexChanged(const QString &/*arg1*/) {
    generateQueens();
    setupBoard();
}

void MainWindow::on_comboBoxPlacement_currentIndexChanged(const QString &/*arg1*/) {
    generateQueens();
    setupBoard();
}

void MainWindow::on_pushButtonReset_clicked() {
    generateQueens();
    setupBoard();
}

void MainWindow::on_comboBoxAlgorithm_currentIndexChanged(int /*index*/) {
    toggleAlgorithmOptions();
}

void MainWindow::on_pushButtonRun_clicked() {
    QString algorithmName = getAlgorithmName();
    LocalSearch::State state;

    switch (getAlgorithm()) {
        case Algorithm::HillClimbing: {
            int equivalentMoves = ui->lineEditEquivalentMoves->text().toInt();

            if (isStepsChecked()) {
                state = LocalSearch::hillClimbStep(getBoardSize(), queens);
            } else {
                state = LocalSearch::hillClimb(getBoardSize(), queens, equivalentMoves);
            }

            queens = state.queens;
            setupBoard();
            break;
        }
        case Algorithm::SimulatedAnnealing: {
            int tempStart = ui->lineEditTempStart->text().toInt();
            int tempChange = ui->lineEditTempChange->text().toInt();

            if (isStepsChecked()) {
                state = LocalSearch::simulatedAnnealingStep(getBoardSize(), queens, tempStart, tempChange);
                // Set last temperature for next step (same as standard loop)
                ui->lineEditTempStart->setText(QString::number(tempStart));
            } else {
                state = LocalSearch::simulatedAnnealing(getBoardSize(), queens, tempStart, tempChange);
            }

            queens = state.queens;
            setupBoard();
            break;
        }
        case Algorithm::LocalBeamSearch: {
            int nStates = ui->lineEditStates->text().toInt();
            int maxIters = ui->lineEditMaxIters->text().toInt();

            state = LocalSearch::localBeam(getBoardSize(), queens, nStates, maxIters);

            queens = state.queens;
            setupBoard();
            break;
        }
        case Algorithm::GeneticAlgorithm: {
            int populationSize = ui->lineEditPopulationSize->text().toInt();
            int elitePerc = ui->lineEditElitePerc->text().toInt();
            int crossProb = ui->lineEditCrossProb->text().toInt();
            int mutationProb = ui->lineEditMutationProb->text().toInt();
            int generations = ui->lineEditGenerations->text().toInt();

            state = LocalSearch::genetic(getBoardSize(), queens, populationSize, elitePerc, crossProb, mutationProb, generations);

            queens = state.queens;
            setupBoard();
            break;
        }
    }

    ui->statusBar->showMessage(
        QString("Finished %1: %2! [%3 steps]")
                .arg(algorithmName,
                     state.heuristics == 0 ? "Success": "Failure",
                     isStepsChecked() ? "1" : QString::number(state.steps)),
        10000);
}

QString MainWindow::spaceCamelCase(const QString &s) {
    static QRegularExpression regExp1 {"(.)([A-Z][a-z]+)"};
    static QRegularExpression regExp2 {"([a-z0-9])([A-Z])"};

    QString result = s;
    result.replace(regExp1, "\\1 \\2").replace(regExp2, "\\1 \\2");

    return result;
}
