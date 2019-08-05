#include <QtCore>
#include <exiv2/image.hpp>

bool getExifData(const QString & inFile, QString &retComm, QString &retOrtn, QString &retDate)
// Get the UserComment, Orientation and DateTimeOriginal from the EXIF data.
{
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(inFile.toUtf8().toStdString()); // ImageFactory::open() expects a std::string() value.
    image.get();
    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();
    if (exifData.empty()) return false;
    Exiv2::Exifdatum &datum = exifData["Exif.Photo.UserComment"];
    retComm = datum.toString().c_str();
    if (retComm.startsWith("charset="))     // This test is probably never true, but left in just in case.
    {
        int spos = retComm.indexOf(' ');
        retComm = retComm.remove(0,spos+1);
    }
    retComm = retComm.trimmed();        // Some cameras populate the UserComment with spaces.
    datum = exifData["Exif.Photo.DateTimeOriginal"];
    retDate = datum.toString().c_str();
    datum = exifData["Exif.Image.Orientation"];
    retOrtn = datum.toString().c_str();
    return true;
}
