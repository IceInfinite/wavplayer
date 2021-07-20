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
    }else {
        setWindowFilePath(QString());
        infoLabel->setText(url.toString());
        fileName.clear();
    }
    fileUrl = new QUrl();
    *fileUrl = url;
    sourceFile->setFileName(QDir::toNativeSeparators(fileUrl->toLocalFile()));
    sourceFile->open(QIODevice::ReadOnly);

    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio = new QAudioOutput(format, this);
    connect(audio, &QAudioOutput::stateChanged, this, &WavPlayer::handleStateChanged);
    audio->start(sourceFile);
}

void play()
{
}

void WavPlayer::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            // 完成播放
            qDebug() << "Finshed playing!";
            audio->stop();
            sourceFile->close();
            delete audio;
            break;

        case QAudio::StoppedState:
            // 由于某些原因暂停
            if (audio->error() != QAudio::NoError) {
                // 出错处理
                qWarning() << "error: " << audio->error();
            }
            break;

        default:
            //其他情况
            break;
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
    connect(playButton, &QAbstractButton::clicked, this, &WavPlayer::play);

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setEnabled(false);
    positionSlider->setToolTip(tr("Seek"));
    //connect(positionSlider, &QAbstractSlider::valueChanged, this, &setPosition);

    infoLabel = new QLabel(this);
    positionLabel = new QLabel(tr("00:00"),this);
    positionLabel->setMinimumWidth(positionLabel->sizeHint().width());

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(positionSlider);
    controlLayout->addWidget(positionLabel);

    QBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(controlLayout);
}

// 快捷键设置
void WavPlayer::createShortcut()
{
    QShortcut *quitShortcut = new QShortcut(QKeySequence::Quit,this);
    connect(quitShortcut, &QShortcut::activated,QCoreApplication::quit);

    QShortcut *openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, &QShortcut::activated, this, &WavPlayer::openFile);

    QShortcut *toggleShortcut = new QShortcut(Qt::Key_Space, this);
    //connect(toggleShortcut, &QShortcut::activated, this, &WavPlayer::togglePlayback);

    QShortcut *forwardShortcut = new QShortcut(Qt::Key_Right, this);
    //connect(forwardShortcut, &QShortcut::activated, this, &WavPlayer::seekForward);

    QShortcut *backwardShortcut = new QShortcut(Qt::Key_Left, this);
    //connect(backwardShortcut, &QShortcut::activated, this, &WavPlayer::seekBackward);

}
