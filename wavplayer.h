#ifndef WAVPLAYER_H
#define WAVPLAYER_H

#include <QWidget>
#include <QAudioOutput>
#include <QAudioFormat>

QT_FORWARD_DECLARE_CLASS(QAbstractButton)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QFile)
QT_FORWARD_DECLARE_CLASS(QUrl)

class WavPlayer : public QWidget
{
    Q_OBJECT

public:
    WavPlayer(QWidget *parent = nullptr);
    static QStringList supportedAudioTypes();

    ~WavPlayer();
private:
    void createWidgets();
    void createShortcut();

    QAbstractButton *openButton = nullptr;
    QAbstractButton *playButton = nullptr;
    QSlider *positionSlider = nullptr;
    QLabel *infoLabel = nullptr;
    QLabel *positionLabel = nullptr;
    QString fileName;
    QFile *sourceFile = nullptr;
    QAudioOutput *audio = nullptr;
    QUrl *fileUrl = nullptr;
private:
    void openFile();
    void playUrl(const QUrl& url);
    void togglePlayback();
    void seekForward();
    void seekBackward();
private slots:
    void handleStateChanged(QAudio::State newState);
    void play();
    void pause();
};

#endif // WAVPLAYER_H
