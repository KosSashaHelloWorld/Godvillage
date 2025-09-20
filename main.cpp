#include "widget.h"
#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>
#include <QLocale>
#include <QTranslator>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Godvill_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    Widget w = Widget();
    w.show();

    QPushButton* openFileBtn = new QPushButton(&w);
    openFileBtn->setText(translator.tr("Open profile file"));

    QVBoxLayout layout = QVBoxLayout(&w);
    layout.setAlignment(Qt::Alignment::enum_type::AlignAbsolute);

    QVBoxLayout horizontalBarL = QVBoxLayout(&w);
    horizontalBarL.setDirection(QVBoxLayout::Direction::LeftToRight);

    QVBoxLayout horizontalUrlL = QVBoxLayout(&w);
    horizontalUrlL.setDirection(QVBoxLayout::Direction::LeftToRight);

    QVBoxLayout pranaL = QVBoxLayout(&w);
    QLabel pranaLabel = QLabel(&w);
    pranaLabel.setText("Prana (God's power):");
    QProgressBar pranaBar = QProgressBar(&w);
    pranaBar.setMinimum(0);
    pranaBar.setMaximum(100);
    pranaBar.setAlignment(Qt::Alignment::enum_type::AlignBottom);
    pranaBar.setToolTip("Prana");
    pranaL.addWidget(&pranaLabel);
    pranaL.addWidget(&pranaBar);

    QVBoxLayout healthL = QVBoxLayout(&w);
    QLabel healthLabel = QLabel(&w);
    healthLabel.setText("Health: ");
    QProgressBar healthBar = QProgressBar(&w);
    healthBar.setMinimum(0);
    healthBar.setAlignment(Qt::Alignment::enum_type::AlignBottom);
    healthBar.setToolTip("Health");
    healthL.addWidget(&healthLabel);
    healthL.addWidget(&healthBar);

    QLabel label = QLabel(&w);
    label.setAlignment(Qt::Alignment::enum_type::AlignLeft);


    QCheckBox isUrlToken = QCheckBox(&w);
    isUrlToken.setText("Use url instead of file (tooltip)");
    isUrlToken.setToolTip("In case you doesn't have a profile.json check it. It's not recommended to click more often than once per 20 sec if you use url instead of file");
    QLineEdit urlTokenLine = QLineEdit(&w);
    urlTokenLine.setPlaceholderText("godvill token");
    urlTokenLine.setText("75446b02cad4");

    QLineEdit godNameLine = QLineEdit(&w);
    godNameLine.setPlaceholderText("god name");
    godNameLine.setText("Тальзеур");

    RefreshGodProfileButton refreshbtn = RefreshGodProfileButton(&label, &pranaBar, &healthBar, &isUrlToken, &godNameLine, &urlTokenLine, &w);
    refreshbtn.setText(translator.tr("Refresh"));

    QFileDialog fileDial = QFileDialog(&w);
    fileDial.selectNameFilter("Profile (*.json)");
    fileDial.setFileMode(QFileDialog::FileMode::ExistingFile);

    refreshbtn.setOpenFileBtn(openFileBtn);

    horizontalBarL.addLayout(&healthL);
    horizontalBarL.addLayout(&pranaL);

    horizontalUrlL.addWidget(&isUrlToken);
    horizontalUrlL.addWidget(&godNameLine);
    horizontalUrlL.addWidget(&urlTokenLine);

    layout.addWidget(&label);
    layout.addLayout(&horizontalBarL);
    layout.addWidget(&refreshbtn);
    layout.addLayout(&horizontalUrlL);
    layout.addWidget(openFileBtn);

    w.resize(700, 230);

    return a.exec();
}
