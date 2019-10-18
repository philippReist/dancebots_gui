#include <AudioFile.h>

#include <gtest/gtest.h>
#include <QtCore/QFile>
#include <string>

namespace {
  class AudioFileTest : public ::testing::Test {
  protected:
    static void SetUpTestSuite(void) {
      // init
    }

    static void TearDownTestSuite(void) {
      // cleanup temp file
      std::remove(fileTemp.toStdString().c_str());
    }

    static const QString fileFolderPath;
    static const QString fileMusic44k;
    static const QString fileTemp;
  };

  const QString AudioFileTest::fileFolderPath{ "./../test_mp3_files/" };
  const QString AudioFileTest::fileMusic44k{ fileFolderPath + "in44100.mp3" };
  const QString AudioFileTest::fileTemp { fileFolderPath + "temp.mp3" };

  TEST_F(AudioFileTest, testAudioFileNotExist) {
    // test opening a fake file:
    const QString fakeFileName("fakefakefile.mp3");

    AudioFile fakeFile{};
    auto result = fakeFile.load(fileFolderPath + fakeFileName);
    
    EXPECT_EQ(result,
              AudioFile::Result::eFileDoesNotExist);
    EXPECT_FALSE(fakeFile.hasData());

    // try to save empty audiofile
    result = fakeFile.save(fileTemp);

    EXPECT_EQ(result, AudioFile::Result::eFileWriteError);
  }

  TEST_F(AudioFileTest, testSave) {
    AudioFile mp3File44k{ };
    mp3File44k.load(fileMusic44k);

    // test tag data:
    EXPECT_STREQ(mp3File44k.getArtist().c_str(), "Daft Punk");
    EXPECT_STREQ(mp3File44k.getTitle().c_str(), "Face To Face");

    // verify flags:
    EXPECT_FALSE(mp3File44k.isDancefile());
    EXPECT_TRUE(mp3File44k.hasData());

    // write new data:
    const std::string artist{ "Roboto" };
    const std::string title{ "BeepBeep" };
    mp3File44k.setArtist(artist);
    mp3File44k.setTitle(title);

    // write some header data
    const size_t nPrePendData = 128 + 16 + 2;

    for(size_t i = 0; i < nPrePendData; ++i) {
      mp3File44k.mMP3PrependData.append(static_cast<char>(i));
    }

    // save the file:
    mp3File44k.save(fileTemp);

    // load the file again:
    AudioFile checkFile{};

    AudioFile::Result result = checkFile.load(fileTemp);

    // check changed tag data
    EXPECT_STREQ(artist.c_str(), checkFile.getArtist().c_str());
    EXPECT_STREQ(artist.c_str(), checkFile.getArtist().c_str());

    // check header data:
    EXPECT_EQ(result, AudioFile::Result::eSuccess);
    EXPECT_TRUE(checkFile.isDancefile());
    EXPECT_EQ(checkFile.mMP3PrependData.size(), nPrePendData);

    // go through header data:
    for(size_t i = 0; i < nPrePendData; ++i) {
      EXPECT_EQ(static_cast<char>(i),
        checkFile.mMP3PrependData.at(i));
    }

  }

  TEST_F(AudioFileTest, testResample) {
    // Test resampling
    std::vector<QString> fileNames = {
      "in8000.mp3",
      "in11025.mp3",
      "in22050.mp3",
      "in32000.mp3",
      "in48000.mp3"
    };

    // now load all files and check that sample numbers are close to 44k file
    AudioFile mp3File44k{};
    mp3File44k.load(fileMusic44k);
    const size_t nSamples = mp3File44k.mFloatMusic.size();
    const size_t sampleRange{ 44100 / 10 * 7 }; // allow for 0.7s deviation

    for(const auto& filename : fileNames) {
      AudioFile resampleFile{};
      resampleFile.load(fileFolderPath + filename);
      // ensure resampled total samples are close to 44.1k Hz file
      // because of large deviation in 8kHz file, have quite broad threshold
      EXPECT_GT(resampleFile.mFloatMusic.size(), nSamples - sampleRange);
      EXPECT_LT(resampleFile.mFloatMusic.size(), nSamples + sampleRange);
    }
  }

  TEST_F(AudioFileTest, test44kFile) {
    // test opening a music mp3 file:

    AudioFile mp3File44k{};
    auto result = mp3File44k.load(fileMusic44k);

    ASSERT_EQ(result, AudioFile::Result::eSuccess);
    ASSERT_TRUE(mp3File44k.hasData());

    EXPECT_FALSE(mp3File44k.isDancefile());

    // make sure mp3 data is read from beginning of file
    // if test file is not a dance file
    QFile testFile{ fileMusic44k };

    testFile.open(QIODevice::ReadOnly);

    const size_t kNRead = 100;
    QByteArray testData = testFile.read(kNRead);

    const char* mp3FileData = mp3File44k.getRawMP3Data();

    for (size_t i = 0; i < testData.size(); ++i) {
      EXPECT_EQ(mp3FileData[i], testData.data()[i]);
    }
  }

  TEST_F(AudioFileTest, testClear) {
    // test opening a music mp3 file:

    AudioFile mp3File44k{};
    auto result = mp3File44k.load(fileMusic44k);

    ASSERT_EQ(result, AudioFile::Result::eSuccess);
    ASSERT_TRUE(mp3File44k.hasData());

    // clear data:
    mp3File44k.clear();

    EXPECT_TRUE(mp3File44k.mFloatData.empty());
    EXPECT_TRUE(mp3File44k.mFloatMusic.empty());
    EXPECT_TRUE(mp3File44k.mMP3PrependData.isEmpty());

    EXPECT_FALSE(mp3File44k.hasData());

    EXPECT_TRUE(mp3File44k.getArtist().empty());
    EXPECT_TRUE(mp3File44k.getTitle().empty());
  }

  TEST_F(AudioFileTest, testDeEnCodeCycles) {
    AudioFile mp3FileHeaderTest{};
    mp3FileHeaderTest.load(fileMusic44k);
    
    // save and load file to get steady-state data length that should be const
    mp3FileHeaderTest.save(fileTemp);
    mp3FileHeaderTest.load(fileTemp);

    const size_t nMP3Samples = mp3FileHeaderTest.mFloatData.size();

    for (uint a = 0; a < 10; ++a) {
      AudioFile mp3FileCycleTest{};
      mp3FileCycleTest.load(fileTemp);
      EXPECT_EQ(nMP3Samples, mp3FileCycleTest.mFloatData.size());
      mp3FileCycleTest.save(fileTemp);
    }
  }

} // anon namespace

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}