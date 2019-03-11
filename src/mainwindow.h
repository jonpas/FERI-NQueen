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

    void setupBoard();

private slots:
    void on_comboBoxN_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
};
