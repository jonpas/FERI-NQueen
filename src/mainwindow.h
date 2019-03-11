#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBoxN_currentIndexChanged(const QString &arg1);
    void on_pushButtonReset_clicked();

    void on_comboBoxAlgorithm_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    void setupBoard();
    void toggleAlgorithmOptions(int index);
};
