#ifndef WAVPLAYER_H
#define WAVPLAYER_H

#include <QWidget>
#include <QAudioOutput>
#include <QAudioFormat>

class QAbstractButton;
class QLabel;
class QSlider;
class QFile;
class QUrl;

class WavPlayer : public QWidget
{
    Q_OBJECT
public:
    enum PlayState{
        StoppedState,
        PlayingState,
        PausedState
    };

    WavPlayer(QWidget *parent = nullptr);
    static QStringList supportedAudioTypes(); //支持音频类型
    ~WavPlayer();

    PlayState State() const;
private:
    void createWidgets();
    void createShortcut();

    QAbstractButton* openButton = nullptr;
    QAbstractButton* playButton = nullptr;
    QSlider* positionSlider = nullptr;
    QLabel* infoLabel = nullptr;
    QLabel* positionLabel = nullptr;
    QString fileName;
    QFile* sourceFile = nullptr;
    QAudioOutput* audio = nullptr;
    QUrl* fileUrl = nullptr;
    PlayState state; //播放状态

signals:
    void stateChange(WavPlayer::PlayState newState);

private:
    void openFile();
    void playUrl(const QUrl& url);
    void togglePlayback();
    void setPlayState(WavPlayer::PlayState newState);
    void setAudioUrl(const QUrl &url);

private slots:
    void upadateState(QAudio::State newState);
    void play();
    void pause();
};

#endif // WAVPLAYER_H
