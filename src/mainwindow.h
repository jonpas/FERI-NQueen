#pragma once

#include <QMainWindow>
#include <QTableWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    enum Placement { Random, TopRow };
    Q_ENUM(Placement)
    enum Algorithm { HillClimbing, SimulatedAnnealing, LocalBeamSearch, GeneticAlgorithm };
    Q_ENUM(Algorithm)

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTableWidget *board;

    static const QPair<uint8_t, uint8_t> SIZE_RANGE;

    void populateUi();
    void setupBoard();
    QList<QPoint> getQueenPositions();
    void toggleAlgorithmOptions(int index);

    int getBoardSize();
    Placement getPlacementType();
    Algorithm getAlgorithm();

    QString prettifyCamelCase(const QString &s);

private slots:
    void on_comboBoxSize_currentIndexChanged(const QString &arg1);
    void on_comboBoxPlacement_currentIndexChanged(const QString &arg1);
    void on_pushButtonReset_clicked();
    void on_comboBoxAlgorithm_currentIndexChanged(int index);
};
