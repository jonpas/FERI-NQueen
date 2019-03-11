#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setupBoard();
}

MainWindow::~MainWindow() {
    ui->tableWidgetBoard->clearContents();

    delete ui;
}

void MainWindow::setupBoard() {
    QTableWidget *board = ui->tableWidgetBoard;

    int size = ui->comboBoxN->currentText().toInt();
    //int algorithm = ui->comboBoxAlgorithm->currentIndex();

    board->setRowCount(size);
    board->setColumnCount(size);
    board->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    board->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QColor colorAlt(245, 222, 179); // Wheat

    for (int i = 0; i < board->rowCount(); i++) {
        for (int j = 0; j < board->columnCount(); j++) {
            QTableWidgetItem *item = board->item(i, j);

            // Create new item if it doesn't exist yet
            if (item == nullptr) {
                item = new QTableWidgetItem();
                board->setItem(i, j, item);
            }

            // Alternate cell color
            if ((i + j) % 2 != 0) {
                item->setBackgroundColor(colorAlt);
            }
        }
    }
}

void MainWindow::on_comboBoxN_currentIndexChanged(const QString &/*arg1*/) {
    setupBoard();
}
