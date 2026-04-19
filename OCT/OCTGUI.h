#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OCTGUI.h"

class OCTGUI : public QMainWindow
{
    Q_OBJECT

public:
    OCTGUI(QWidget *parent = nullptr);
    ~OCTGUI();

    Ui::OCTGUIClass ui;
};

