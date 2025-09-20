#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLocale>
#include <QTranslator>
#include <QProgressBar>
#include <QButtonGroup>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QVBoxLayout>
#include <QObject>
#include <QDebug>
#include <cpr/api.h>

class GodProfile : QObject
{

public:

    QString heroName;
    QString godName;
    QString lastNote;
    int prana;
    int maxHealth;
    int curHealth;
    int statusCode;

    enum Mode
    {
        fileMode,
        urlMode
    };

    GodProfile(QString filePath) : filePath(filePath), mode(Mode::fileMode)
    {

    }

    GodProfile(QString url, QString urlToken) : url(url), urlToken(urlToken), mode(Mode::urlMode)
    {

    }

    QString get()
    {
        QString strProfile;
        strProfile.append(tr("God's name: ")).append(this->godName).append("\n");
        strProfile.append(tr("Hero's name: ")).append(this->heroName).append("\n");
        strProfile.append(tr("God's power: ")).append(QString::number(this->prana)).append("\n");
        strProfile.append(tr("Hero's maximum health: ")).append(QString::number(this->maxHealth)).append("\n");
        strProfile.append(tr("Hero's current health: ")).append(QString::number(this->curHealth)).append("\n");
        strProfile.append(tr("Last diary: ")).append(this->lastNote).append("\n");
        return strProfile;
    }

    void update()
    {
        QJsonObject obj = readJsonObject();
        heroName = obj["name"].toString();
        godName = obj["godname"].toString();
        lastNote = obj["diary_last"].toString();
        prana = obj["godpower"].toInt();
        maxHealth = obj["max_health"].toInt();
        curHealth = obj["health"].toInt();
        qInfo("Profile updated");
    }

private:

    const QString filePath;
    const QString url;
    const QString urlToken;
    const Mode mode;

    QJsonObject readJsonObject()
    {
        if (mode == Mode::fileMode)
        {
            return readJsonObjectFromFile(filePath);
        }
        else if (mode == Mode::urlMode)
        {
            return readJsonObjectFromUrl(url, urlToken);
        }
        else
        {
            qDebug("GodProfile mode error");
            return QJsonObject();
        }
    }

    QJsonObject readJsonObjectFromString(QString str)
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());

        if (parseError.error != QJsonParseError::NoError)
        {
            qWarning() << "JSON parse error: " << parseError.errorString();
            return QJsonObject();
        }
        qInfo("File parsed");

        QJsonObject jsonObj = jsonDoc.object();

        return jsonObj;
    }

    QJsonObject readJsonObjectFromUrl(const QString url, const QString urlToken)
    {
        cpr::Response r = cpr::Get(cpr::Url((url + urlToken).toStdString()));
        if (r.status_code != 200)
        {
            qInfo("%s%ld", "Error: status code of request is ", r.status_code);
            return QJsonObject();
        }

        return readJsonObjectFromString(r.text.c_str());
    }

    QJsonObject readJsonObjectFromFile(const QString filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning() << "Could not open file: " << file.errorString();
            return QJsonObject();
        }

        QByteArray jsonData = file.readAll();
        file.close();
        qInfo("File readed");

        return readJsonObjectFromString(jsonData);
    }
};



class RefreshPranaButton : public QPushButton
{
    Q_OBJECT
    QProgressBar* pranaBar;
    QProgressBar* healthBar;
    QLabel* label;
    GodProfile* profile;
public:
    RefreshPranaButton(QLabel* label, QProgressBar* pranaBar, QProgressBar* healthBar, GodProfile* profile, QWidget* parent = nullptr) :
        QPushButton(parent), pranaBar(pranaBar), healthBar(healthBar), label(label), profile(profile)
    {
        connect(this, SIGNAL(clicked()), this, SLOT(handleClick()));
    }
private slots:
    void handleClick()
    {
        profile->update();
        label->setText(profile->get());
        pranaBar->setValue(profile->prana);
        healthBar->setMaximum(profile->maxHealth);
        healthBar->setValue(profile->curHealth);
        qInfo("handleClick called");
    }
};



class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
};
#endif // WIDGET_H
