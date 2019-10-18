#ifndef AUDIO_FILE_H_
#define AUDIO_FILE_H_

#include <vector>
#include <QtCore/QFile>

#include <tbytevectorstream.h>
#include <mpegfile.h>
#include <mpegheader.h>
#include <id3v2tag.h>

/** \class AudioFile
* \brief Loads, de- and encodes, and saves Dancebot audio MP3 files
*/
class AudioFile {
public:
  // CONSTANTS //
  /** Result enum that indicates file processing outcomes
  */
  enum class Result {
    eSuccess,
    eNotAnMP3File,
    eCorruptHeader,
    eIOError,
    eFileWriteError,
    eFileOpenError,
    eFileDoesNotExist,
    eMP3DecodingError,
    eMP3EncodingError,
    eTagWriteError
  };
  
  /** String code at beginning and end of pre-pended header data */
  static const QByteArray danceFileHeaderCode;
  /** Sample rate used internally and for MP3 output.
  *
  * Inputs that are not at sampleRate will be up- or downsampled to match
  */
  static const int sampleRate{ 44100 };
  /** Lame MP3 compression quality parameter */
  static const int mp3Quality{ 3 };
  /** Lame output bitrate */
  static const int bitRateKB{ 160 };
  /** MP3 block size. Using 44.1k will be MPEG 1 Layer III, which has a sample
  * block size of 1152
  */
  static const size_t mp3BlockSize{ 1152 };
  /** target RMS level of music [0.0 1.0] that music is normalized to: */
  static const double musicRMSTarget;

  // PUBLIC METHODS //
  /** \brief Default constructor that returns an empty AudioFile object
   */
  explicit AudioFile(void);

  /**
  * \brief Loads an MP3 file from a file path and returns processing result
  * Discards previous file data in object.
  *
  * Usage:
  * const QString kFilePath{ "C:/Users/philipp/Desktop/test.mp3" };
  * AudioFile file{};
  * const auto result = file.load(kFilePath);
  * if(result != AudioFile::Result::eSuccess){
  *   // file failed to load, process error:
  *   process_load_error(result);
  * }
  *
  * \param filePath absolute path to MP3 file
  * \return Result enum of processing
  */
  Result load(const QString filePath);

  /**
  * \brief Saves an MP3 file and returns result of process
  *
  * Usage:
  * const QString kFilePath{ "C:/Users/philipp/Desktop/test.mp3" };
  * const auto result = file.save(kFilePath);
  * if(result != AudioFile::Result::eSuccess){
  *   // file failed to save, process error:
  *   process_save_error(result);
  * }
  *
  * \param filePath absolute path to MP3 file
  * \return Result enum of processing
  */
  Result save(const QString filePath);

  /** \brief Clears all audio file data
  */
  void clear(void);

  /** \brief Flag that indicates whether the file is a dancefile as id'd by
  * a valid header present.
  */
  const bool isDancefile(void) const {
    return mIsDanceFile;
  }

  /** \brief Flag that indicates whether the file contains data or not
  */
  const bool hasData(void) const {
    return mHasData;
  }

  /** \brief Sets artist string
  */
  void setArtist(const std::string& artist) {
    mArtist = artist;
  }

  /** \brief Sets song title string
  */
  void setTitle(const std::string& title) {
    mTitle = title;
  }

  /** \brief Get artist string
  */
  const std::string& getArtist(void) const {
    return mArtist;
  }

  /** \brief Get song title string
  */
  const std::string& getTitle(void) const{
    return mTitle;
  }

  /** MP3 prepend data containing dance-file header, if available */
  QByteArray mMP3PrependData;
  /** Data channel of audio file (R) */
  std::vector<float> mFloatData;
  /** Music channel of audio file (L) */
  std::vector<float> mFloatMusic;

  /** \brief Saves music and data channels to PCM (WAV) file
  * \param fileName absolute path to wav file to write
  * \return 0 if success and 1 if failure
  */
  int SavePCM(const QString fileName);

  /** \brief Saves music and beat beep channels to PCM (WAV) file
  *  left channel is music and right channel is beat beeps at detecte locations
  * \param fileName absolute path to wav file to write
  * \param beatFrames vector of detected beats, location in frames/samples
  * \return 0 if success and 1 if failure
  */
  int SavePCMBeats(const QString fileName,
                   const std::vector<long>& beatFrames);

  /** \brief Returns pointer to raw MP3 file data
  * \return const pointer to data
  */
  const char* getRawMP3Data(void) const {
    return mRawMP3Data.data();
  }

private:
  /** Lame encoding status enum
  */
  enum class LameEncCodes {
    eEncodeSuccess = 0,
    eMP3BufTooSmall = -1,
    eMallocProblem = -2,
    eInitNotCalled = -3,
    ePsychoIssue = -4,
    ePCMDataNotSameLength = -5,
    eNoPCMData = -6,
    eLameInitFailed = -7
  };

  /** MP3 file data container: */
  TagLib::ByteVector mRawMP3Data;

  QString mPath; /**< file path */
  bool mIsDanceFile = false; /**< dance file flag (valid header detected) */
  /** number of bytes that make up the total bytes in header unsigned int */
  static const size_t headerSizeNBytes{ 4 };
  bool mHasData{ false }; /** flag indicating data available in object */

  /** Sample rate read from load file tags - only for internal use as all 
  * music data will be resampled to the static const sampleRate = 44.1kHz
  */
  int mLoadFileSampleRate { 0 };
  int mLengthMS{ 0 }; /**< length of music in ms from mp3 tag*/
	std::string mArtist; /**< song artist (extracted from tag) */
	std::string mTitle; /**< song title */
  /** Calculated music gain to ensure music rms stays at musicRMSTarget */
  double mMP3MusicGain = 1.0;

  // PRIVATE FUNCTIONS //
  /** \brief read mp3 tag from raw mp3 data */
  int readTag(void);
  /** \brief write mp3 tag to raw mp3 data */
  int writeTag(void);
  /** \brief decode raw mp3 data */
  int decode(void);
  /** \brief encode data in music and data stream to raw mp3 data */
  LameEncCodes encode(void);
};

#endif // AUDIO_FILE_H_ header guard
