#include "wavplayer.h"
#include <QtWidgets>


WavPlayer::WavPlayer(QWidget *parent)
    : QWidget(parent)
{
    createWidgets();
    createShortcut();

    //窗口可拖动
    setAcceptDrops(true);
}

WavPlayer::~WavPlayer()
{
}

QStringList WavPlayer::supportedAudioTypes()
{
    QStringList result;
    if(result.isEmpty())
        result.append(QStringLiteral("audio/wav"));
    return result;
}

WavPlayer::PlayState WavPlayer::State() const
{
    return state;
}

void WavPlayer::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open File"));
    fileDialog.setMimeTypeFilters(WavPlayer::supportedAudioTypes());
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    if(fileDialog.exec() == QDialog::Accepted)
        playUrl(fileDialog.selectedUrls().constFirst());
}

void WavPlayer::playUrl(const QUrl &url)
{
    playButton->setEnabled(true);
    if(url.isLocalFile())
    {
        const QString filePath = url.toLocalFile();
        setWindowFilePath(filePath);
        infoLabel->setText(QDir::toNativeSeparators(filePath));
        fileName = QFileInfo(filePath).fileName();
        setAudioUrl(url);
        play();
    }else {
        setWindowFilePath(QString());
        infoLabel->setText(url.toString());
        fileName.clear();
    }
}

void WavPlayer::play()
{
    if(State() == WavPlayer::PausedState)
    {
        audio->resume();
    }else {
        QString newFileName = QDir::toNativeSeparators(fileUrl->toLocalFile());
        sourceFile->setFileName(newFileName);
        sourceFile->open(QIODevice::ReadOnly);

        QAudioFormat format;
        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(8);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format))
        {
            qWarning() << "Raw audio format not supported by backend, cannot play audio.";
            return;
        }

        audio = new QAudioOutput(format, this);
        connect(audio, &QAudioOutput::stateChanged, this, &WavPlayer::upadateState);
        audio->start(sourceFile);
    }

}
void WavPlayer::upadateState(QAudio::State newState)
{
    //可以简化处理
    switch (newState) {
    case QAudio::IdleState:
        // 完成播放
        qDebug() << "Finshed playing!";
        audio->stop();
        sourceFile->close();
        delete audio;
        audio = nullptr; //与setAduioUrl有关
        setPlayState(WavPlayer::StoppedState);
        playButton->setToolTip(tr("Play"));
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;

    case QAudio::StoppedState:
        // 停止音频输出
        qDebug() << "stop";
        setPlayState(WavPlayer::StoppedState);
        if (audio->error() != QAudio::NoError)
        {
            // 出错处理
            qWarning() << "error: " << audio->error();
        }
        break;

    case QAudio::ActiveState:
        qDebug() << "I'm playing!";
        setPlayState(WavPlayer::PlayingState);
        playButton->setToolTip(tr("Pause"));
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;

    case QAudio::SuspendedState:
        // 挂起
        qDebug() << "I'm stopping!";
        setPlayState(WavPlayer::PausedState);
        playButton->setToolTip(tr("Play"));
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    default:
        //其他情况
        break;
    }
}

void WavPlayer::pause()
{
    audio->suspend();
}

void WavPlayer::setPlayState(WavPlayer::PlayState newState)
{
    if(state == newState)
    {
        return;
    }
    else{
        state = newState;
        emit stateChange(newState);
    }
}

void WavPlayer::setAudioUrl(const QUrl &url)
{
    *fileUrl = url;
    if(audio != nullptr)
    {
        audio->stop();
        sourceFile->close();
        delete audio;
    }
}

void WavPlayer::togglePlayback()
{
    //未存在音频情况待处理


    if(State() == WavPlayer::PlayingState)
    {
        pause();
    }else {
        play();
    }
}
void WavPlayer::createWidgets()
{
    openButton = new QToolButton(this);
    openButton->setText("...");
    openButton->setToolTip(tr("Open an Aduiofile.."));
    connect(openButton, &QAbstractButton::clicked, this, &WavPlayer::openFile);

    playButton = new QToolButton(this);
    playButton->setEnabled(false);
    playButton->setToolTip(tr("Play"));
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->setFixedSize(openButton->sizeHint());
    connect(playButton, &QAbstractButton::clicked, this, &WavPlayer::togglePlayback);

//    positionSlider = new QSlider(Qt::Horizontal, this);
//    positionSlider->setEnabled(false);
//    positionSlider->setToolTip(tr("Seek"));
    //connect(positionSlider, &QAbstractSlider::valueChanged, this, &setPosition);

    infoLabel = new QLabel(this);
    infoLabel->setMinimumWidth(240);
//    positionLabel = new QLabel(tr("00:00"),this);
//    positionLabel->setMinimumWidth(positionLabel->sizeHint().width());

    QBoxLayout* controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
//    controlLayout->addWidget(positionSlider);
//    controlLayout->addWidget(positionLabel);

    QBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(controlLayout);

    //变量初始化
    fileUrl = new QUrl();
    sourceFile = new QFile();
    state = WavPlayer::StoppedState;
}

// 快捷键设置
void WavPlayer::createShortcut()
{
    QShortcut* quitShortcut = new QShortcut(QKeySequence::Quit,this);
    connect(quitShortcut, &QShortcut::activated,QCoreApplication::quit);

    QShortcut* openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, &QShortcut::activated, this, &WavPlayer::openFile);

    QShortcut* toggleShortcut = new QShortcut(Qt::Key_Space, this);
    connect(toggleShortcut, &QShortcut::activated, this, &WavPlayer::togglePlayback);

}
