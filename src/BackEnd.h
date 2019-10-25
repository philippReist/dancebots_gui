#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>

#include "AudioFile.h"
#include "BeatDetector.h"
#include "PrimitiveList.h"

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString songArtist READ songArtist WRITE setSongArtist)
    Q_PROPERTY(QString songTitle READ songTitle WRITE setSongTitle)
    Q_PROPERTY(QString loadStatus READ loadStatus NOTIFY loadStatusChanged)
    Q_PROPERTY(PrimitiveList* motorPrimitives READ motorPrimitives NOTIFY motorPrimitivesChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);

    QString songArtist();
    QString songTitle();
    QString loadStatus();
    PrimitiveList* motorPrimitives(void);
    void setSongArtist(const QString &name);
    void setSongTitle(const QString& name);
    Q_INVOKABLE void loadMP3(const QString& filePath);
    Q_INVOKABLE QVector<int> getBeats(void) const;
    Q_INVOKABLE int getAudioLengthInFrames(void) const;

signals:
    void loadStatusChanged();
    void motorPrimitivesChanged();
    void doneLoading(const bool result);

public slots:
    void handleDoneLoading(void);

private:
    QString mSongArtist;
    QString mSongTitle;

    QString mLoadStatus;
    AudioFile mAudioFile;
    BeatDetector mBeatDetector;
    std::vector<long> mBeatFrames;
    QFuture<bool> mLoadFuture;
    QFutureWatcher<bool> mLoadFutureWatcher;

    PrimitiveList* mMotorPrimitives;

    bool loadMP3Worker(const QString& fileName);
};

#endif // BACKEND_H