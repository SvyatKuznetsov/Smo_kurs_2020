#ifndef STEPMODE_H
#define STEPMODE_H

#include <QWidget>
#include <analytics.h>

namespace Ui {
class StepMode;
}

class StepMode : public QWidget
{
    Q_OBJECT

public:
    explicit StepMode(QWidget *parent = nullptr);
    ~StepMode();

    void setAnalytics(Smo_kurs::Analytics *value);

private slots:
    void on_buttonRefresh_clicked();

    void on_buttonNext_clicked();

    void on_buttonBack_clicked();

private:
    void refreshScene();
    void prepareScene();

    Ui::StepMode *ui;

    Smo_kurs::Analytics *analytics;

    int currentStep;
};

#endif // STEPMODE_H
