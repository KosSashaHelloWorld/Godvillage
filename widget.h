#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLocale>
#include <QTranslator>
#include <QProgressBar>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QVBoxLayout>
#include <QObject>
#include <QDebug>
#include <curl/curl.h>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileDialog>

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
};

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
    Mode mode = Mode::fileMode;

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
            qInfo() << "GodProfile mode error";
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
        qInfo() << "Json object parsed from string";

        QJsonObject jsonObj = jsonDoc.object();

        return jsonObj;
    }

    static size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

    QJsonObject readJsonObjectFromUrl(const QString url, const QString urlToken)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        auto curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, (url + urlToken).toStdString().c_str());
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
            std::string response;
            std::string header_string;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            qInfo() << "Url readed";
            return readJsonObjectFromString(QString(response.c_str()));
        }

        qInfo() << "Error: curl didn't initialize";
        return QJsonObject();
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
        qInfo() << "File readed";

        return readJsonObjectFromString(jsonData);
    }
};



class RefreshGodProfileButton : public QPushButton
{
    Q_OBJECT
    const std::string gvAPIurl = "https://godville.net/gods/api/%D0%A2%D0%B0%D0%BB%D1%8C%D0%B7%D0%B5%D1%83%D1%80/";
    QProgressBar* pranaBar;
    QProgressBar* healthBar;
    QLabel* label;
    GodProfile* profile;
    const QCheckBox* isUrlToken;
    const QLineEdit* urlTokenLine;
    QString urlToken;
    Widget* parentWidget;
    QString filePath;
public:
    RefreshGodProfileButton
        (
            QLabel* label,
            QProgressBar* pranaBar,
            QProgressBar* healthBar,
            QCheckBox* isUrlToken,
            QLineEdit* urlTokenLine,
            Widget* parent = nullptr
        ) :
        label(label),
        pranaBar(pranaBar),
        healthBar(healthBar),
        isUrlToken(isUrlToken),
        urlTokenLine(urlTokenLine),
        QPushButton(parent),
        parentWidget(parent)
    {
        connect(this, SIGNAL(clicked()), this, SLOT(handleClick()));
        connect(isUrlToken, &QCheckBox::clicked, this, &RefreshGodProfileButton::onIsUrlCheckBoxChanged);
        connect(urlTokenLine, &QLineEdit::editingFinished, this, &RefreshGodProfileButton::updateUrlToken);
        profile = new GodProfile(filePath);
    }
    void setOpenFileBtn(QPushButton* openFileBtn)
    {
        connect(openFileBtn, &QPushButton::clicked, this, &RefreshGodProfileButton::openFile);
    }

private slots:

    void openFile()
    {
        filePath = QFileDialog::getOpenFileName(this, QTranslator::tr("Open profile file"), "", QTranslator::tr("Profile (*.json)"));
        qInfo() << "File opened:\n\t" << filePath;
        profile = new GodProfile(filePath);
        emit clicked();
    }
    void handleClick()
    {
        profile->update();
        label->setText(profile->get());
        pranaBar->setValue(profile->prana);
        healthBar->setMaximum(profile->maxHealth);
        healthBar->setValue(profile->curHealth);
        qInfo() << "God profile refreshed\n";
    }
    void onIsUrlCheckBoxChanged()
    {
        if (isUrlToken->isChecked())
        {
            urlToken = urlTokenLine->text();
            profile = new GodProfile(gvAPIurl.c_str(), urlToken.toStdString().c_str());
        }
        else
        {
            profile = new GodProfile(filePath);
        }
    }
public slots:
    void updateUrlToken()
    {
        urlToken = urlTokenLine->text();
        profile = new GodProfile(gvAPIurl.c_str(), urlToken.toStdString().c_str());
    }
};






#endif // WIDGET_H
