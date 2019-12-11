/*
*  Dancebots GUI - Create choreographies for Dancebots
*  https://github.com/philippReist/dancebots_gui
*
*  Copyright 2019 - Philipp Reist
*
*  This program is free software : you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*  See the GNU General Public License for more details, available in the
*  LICENSE file included in the repository.
*/

#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>

#include "AudioFile.h"
#include "AudioPlayer.h"
#include "BeatDetector.h"
#include "PrimitiveList.h"

/** \class BackEnd
 * \brief Backend class providing primitive models and audio data handling and
 * playback to GUI frontend.
 */
class BackEnd : public QObject {
  Q_OBJECT;
  Q_PROPERTY(QString songArtist 
             READ songArtist
             WRITE setSongArtist
             NOTIFY songArtistChanged);
  Q_PROPERTY(QString songTitle
             READ songTitle
             WRITE setSongTitle
             NOTIFY songTitleChanged);
  Q_PROPERTY(QString songComment
             READ songComment
             WRITE setSongComment
             NOTIFY songCommentChanged);
  Q_PROPERTY(QString fileStatus
             READ fileStatus
             NOTIFY fileStatusChanged);
  Q_PROPERTY(PrimitiveList* motorPrimitives
             READ motorPrimitives
             NOTIFY motorPrimitivesChanged);
  Q_PROPERTY(PrimitiveList* ledPrimitives
             READ ledPrimitives
             NOTIFY ledPrimitivesChanged);
  Q_PROPERTY(AudioPlayer* audioPlayer
             READ audioPlayer
             NOTIFY audioPlayerChanged);

public:
  explicit BackEnd(QObject* parent = nullptr);

  /**
   * \brief Get ID3-Tag song artist string
   */
  QString songArtist();

  /**
   * \brief Get ID3-Tag song title string
   */
  QString songTitle();

  /**
   * \brief Get ID3-Tag song comment string
   */
  QString songComment();

  /**
   * \brief Get file load and save status string
   */
  QString fileStatus();

  /**
   * \brief Get motor primitive model
   */
  PrimitiveList* motorPrimitives(void);

  /**
   * \brief Get led primitive model
   */
  PrimitiveList* ledPrimitives(void);

  /**
   * \brief Get pointer to audio player instance
   */
  AudioPlayer* audioPlayer(void);

  /**
   * \brief Set ID3-Tag song artist string
   */
  void setSongArtist(const QString& name);

  /**
   * \brief Set ID3-Tag song title string
   */
  void setSongTitle(const QString& name);

  /**
   * \brief Set ID3-Tag song comment string
   */
  void setSongComment(const QString& comment);

  /**
   * \brief Load MP3 from given file path
   *
   * \param[in] filePath - path to MP3 file to load
   */
  Q_INVOKABLE void loadMP3(const QString& filePath);

  /**
   * \brief Save MP3 to given file path
   * \param[in] filePath - path to MP3 file to save
   */
  Q_INVOKABLE void saveMP3(const QString& filePath);

  /**
   * \brief Get vector of beat locations in audio frames
   */
  Q_INVOKABLE std::vector<int> getBeats(void) const;

  /**
   * \brief Get total audio length in frames
   */
  Q_INVOKABLE int getAudioLengthInFrames(void) const;

  /**
   * \brief Get sample rate in Hz
   */
  Q_INVOKABLE int getSampleRate(void) const;

  /**
   * \brief Get average beat duration in frames
   */
  Q_INVOKABLE int getAverageBeatFrames(void) const;

  /**
   * \brief Given an audio frame number, find the beat that is the lower bound
   * of the beat interval containing the frame number. I.e.
   *
   *  beatFrame[i] <= frame number < beatFrame[i + 1]
   *
   * \return beat index, or -1 if no valid interval can be found
   */
  Q_INVOKABLE int getBeatAtFrame(const int frame) const;
signals:
  void fileStatusChanged();
  void songArtistChanged();
  void songTitleChanged();
  void songCommentChanged();
  void motorPrimitivesChanged();
  void ledPrimitivesChanged();
  void audioPlayerChanged();
  void doneLoading(const bool result);
  void doneSaving(const bool result);

public slots:
  void handleDoneLoading(void);
  void handleDoneSaving(void);
  void printMotPrimitives(void) const;
  void printLedPrimitives(void) const;

private:
  // init to 100bpm
  int mAverageBeatFrames{ 60 * AudioFile::sampleRate / 100 };

  // song ID3 tag strings
  QString mSongArtist;
  QString mSongTitle;
  QString mSongComment;

  // string used to communicate loading/saving progress to UI
  QString mFileStatus;
  AudioFile mAudioFile;
  AudioPlayer* mAudioPlayer;
  BeatDetector mBeatDetector;
  std::vector<int> mBeatFrames; /**< beat locations in audio frames */

  // multi-threading members for loading and saving in separate threads
  // to keep UI responsive / showing messages during loading and saving
  QFuture<bool> mLoadFuture;
  QFutureWatcher<bool> mLoadFutureWatcher;
  QFuture<bool> mSaveFuture;
  QFutureWatcher<bool> mSaveFutureWatcher;
  bool loadMP3Worker(const QString& fileName);
  bool saveMP3Worker(const QString& fileName);

  // data models for motor and led primitives
  PrimitiveList* mMotorPrimitives; // raw pointer fine because it is QObject
  PrimitiveList* mLedPrimitives; // raw pointer fine because it is QObject

  /**
   * \brief Write beats and primitives to MP3 prepend data
   */
  bool writePrependData(void);

  /**
   * \brief Extract beat frames from prepend data
   */
  bool readBeatsFromPrependData(void);

  /**
   * \brief Extract primitives from prepend data
   */
  bool readPrimitivesFromPrependData(void);
};

#endif // BACKEND_H