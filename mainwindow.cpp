#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Smo_kurs::Analytics *analytics = new Smo_kurs::Analytics();

    configure = new Configure();
    configure->setAnalytics(analytics);
    autoMode = new AutoMode();
    autoMode->setAnalytics(analytics);
    stepMode = new StepMode();
    stepMode->setAnalytics(analytics);

    ui->tabWidget->addTab(configure->topLevelWidget(), "Configure");
    ui->tabWidget->addTab(autoMode->topLevelWidget(), "Auto mode");
    ui->tabWidget->addTab(stepMode->topLevelWidget(), "Step mode");
}

MainWindow::~MainWindow()
{
    delete ui;
}
