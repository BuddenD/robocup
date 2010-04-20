#include "nulVersion1FormatReader.h"
#include <QDebug>

nulVersion1FormatReader::nulVersion1FormatReader(QObject *parent) :
    LogFileFormatReader(parent)
{
}

nulVersion1FormatReader::nulVersion1FormatReader(const QString& filename, QObject *parent):
    LogFileFormatReader(parent)
{
    openFile(filename);
    return;
}

nulVersion1FormatReader::~nulVersion1FormatReader()
{
    closeFile();
}

int nulVersion1FormatReader::openFile(const QString& filename)
{
    fileInformation.setFile(filename);
    closeFile();
    fileStream.open(filename.toStdString().c_str(),std::ios_base::in|std::ios_base::binary);
    if(fileGood())
    {
        fileStream.seekg(0,std::ios_base::beg);
        int x,y;
        fileStream.read(reinterpret_cast<char*>(&x), sizeof(x));
        fileStream.read(reinterpret_cast<char*>(&y), sizeof(y));
        fileStream.seekg(0,std::ios_base::beg);
        m_frameLength = (2*sizeof(x) + x*y*sizeof(int) + sizeof(double));
        totalFrames = fileInformation.size() / m_frameLength;
    }
    return numFrames();
}

bool nulVersion1FormatReader::closeFile()
{
    fileStream.close();
    return true;
}

bool nulVersion1FormatReader::isNextFrameAvailable()
{
    return ( fileGood() && (numFrames() > 0) && (currentFrame() < numFrames()) );
}

bool nulVersion1FormatReader::isPreviousFrameAvailable()
{
    return (fileGood() && (currentFrame() > 1) );
}

bool nulVersion1FormatReader::isFirstFrameAvailable()
{
    return (fileGood() && (numFrames() > 0) );
}

int nulVersion1FormatReader::nextFrame()
{
    if(isNextFrameAvailable())
    {

        currentFrameIndex++;
        fileStream >> rawImageBuffer;
        qDebug() << fileStream.tellg();
        emit rawImageChanged(&rawImageBuffer);
        emit frameChanged(currentFrame(),numFrames());
    }
    return currentFrame();

}

int nulVersion1FormatReader::firstFrame()
{
    currentFrameIndex = 0;
    fileStream.seekg(0,std::ios_base::beg);
    nextFrame();
    return currentFrame();
}

int nulVersion1FormatReader::previousFrame()
{
    if(isPreviousFrameAvailable())
    {
        --currentFrameIndex;
        qDebug() << "Frame: " << currentFrameIndex << " Seeking to:" << currentFrameIndex*m_frameLength << endl;
        fileStream.seekg((currentFrameIndex-1)*m_frameLength,std::ios_base::beg);
        qDebug() << fileStream.tellg();
        fileStream >> rawImageBuffer;
        emit rawImageChanged(&rawImageBuffer);
        emit frameChanged(currentFrame(),numFrames());
    }
    return currentFrame();
}
