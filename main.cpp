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
    QWidget w;
    w.show();

    QVBoxLayout layout = QVBoxLayout(&w);
    layout.setAlignment(Qt::Alignment::enum_type::AlignAbsolute);

    QVBoxLayout horizontalBarL = QVBoxLayout(&w);
    horizontalBarL.setDirection(QVBoxLayout::Direction::LeftToRight);

    QVBoxLayout horizontalUrlL = QVBoxLayout(&w);
    horizontalUrlL.setDirection(QVBoxLayout::Direction::LeftToRight);

    QProgressBar pranaBar = QProgressBar(&w);
    pranaBar.setMinimum(0);
    pranaBar.setMaximum(100);
    pranaBar.setAlignment(Qt::Alignment::enum_type::AlignBottom);

    QProgressBar healthBar = QProgressBar(&w);
    healthBar.setMinimum(0);
    healthBar.setAlignment(Qt::Alignment::enum_type::AlignBottom);

    QLabel label = QLabel(&w);
    label.setAlignment(Qt::Alignment::enum_type::AlignLeft);


    QCheckBox isUrlToken = QCheckBox(&w);
    isUrlToken.setText("Use url instead of file (tooltip)");
    isUrlToken.setToolTip("In case you doesn't have a profile.json check it. It's not recommended to click more often than once per 20 sec if you use url instead of file");
    QLineEdit urlTokenLine = QLineEdit(&w);
    urlTokenLine.setPlaceholderText("godvill token");
    urlTokenLine.setText("75446b02cad4");

    RefreshGodProfileButton refreshbtn = RefreshGodProfileButton(&label, &pranaBar, &healthBar, &isUrlToken, &urlTokenLine, &w);
    refreshbtn.setText(translator.tr("Refresh"));

    horizontalBarL.addWidget(&healthBar);
    horizontalBarL.addWidget(&pranaBar);

    horizontalUrlL.addWidget(&isUrlToken);
    horizontalUrlL.addWidget(&urlTokenLine);

    layout.addWidget(&label);
    layout.addLayout(&horizontalBarL);
    layout.addWidget(&refreshbtn);
    layout.addLayout(&horizontalUrlL);

    w.resize(700, 230);

    return a.exec();
}
