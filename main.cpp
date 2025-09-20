#include "widget.h"
#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>



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

    QProgressBar pranaBar = QProgressBar(&w);
    pranaBar.setMinimum(0);
    pranaBar.setMaximum(100);
    pranaBar.setAlignment(Qt::Alignment::enum_type::AlignBottom);

    QProgressBar healthBar = QProgressBar(&w);
    healthBar.setMinimum(0);
    healthBar.setAlignment(Qt::Alignment::enum_type::AlignBottom);

    QLabel label = QLabel(&w);
    label.setAlignment(Qt::Alignment::enum_type::AlignLeft);

    GodProfile profile = GodProfile("/home/alex/Repos/Games/Godvill/profile.json");

    QCheckBox isUrlToken = QCheckBox(&w);
    isUrlToken.setText("Use url instead of file");
    QLineEdit urlTokenLine = QLineEdit(&w);
    urlTokenLine.setPlaceholderText("godvill token");

    RefreshPranaButton refreshbtn = RefreshPranaButton(&label, &pranaBar, &healthBar, &profile, &w);
    refreshbtn.setText(translator.tr("Refresh"));

    layout.addWidget(&label);
    layout.addWidget(&healthBar);
    layout.addWidget(&pranaBar);
    layout.addWidget(&refreshbtn);
    layout.addWidget(&isUrlToken);
    layout.addWidget(&urlTokenLine);

    w.resize(700, 230);

    return a.exec();
}
