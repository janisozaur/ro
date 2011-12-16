#include "LBP.h"
#include "../../common/CommonDefines.h"

#include <QDebug>

LBP::LBP()
{
}

bool LBP::init(const QStringList &/*params*/)
{
    for (int i = 0; i < 256; i++) {
        mGreyscaleColorTable << qRgb(i, i, i);
        QString num(QString::number(i, 2).rightJustified(8, '0'));
        int changes = 0;
        for (int j = 0; j < num.size() - 1; j++) {
            const QString part = num.mid(j, 2);
            if (part == "01" || part == "10") {
                changes++;
            }
        }
        if (changes <= 2) {
            qDebug() << num << "is uniform!";
            mUniforms << i;
        }
    }
    return true;
}

int LBP::size() const
{
    return mUniforms.size() + 1;
}

bool LBP::extracts() const
{
    return false;
}

QPoint LBP::getPointDelta(const int &i) const
{
    switch (i) {
    case 0:
        return QPoint(1, 0);
        break;
    case 1:
        return QPoint(1, 1);
        break;
    case 2:
        return QPoint(0, 1);
        break;
    case 3:
        return QPoint(-1, 1);
        break;
    case 4:
        return QPoint(-1, 0);
        break;
    case 5:
        return QPoint(-1, -1);
        break;
    case 6:
        return QPoint(0, -1);
        break;
    case 7:
        return QPoint(1, -1);
        break;
    }
    return QPoint();
    qCritical() << "shouldn't have reached here!";
}

void LBP::preprocess(const QImage &img)
{
    const int w = img.width();
    const int h = img.height();
    mLBPImage = QImage(img.size(), QImage::Format_Indexed8);
    mLBPImage.setColorTable(mGreyscaleColorTable);

    // fill only the data that exists in the image
    for (int ay = 0; ay < h; ay++) {
        for (int ax = 0; ax < w; ax++) {
            QPoint centre(ay, ax);
            const int centreVal = img.pixelIndex(centre);
            int acc = 0;
            int delta = 1;
            for (int i = 0; i < 8; i++) {
                QPoint local = centre + getPointDelta(i);
                if (local.x() >= img.width()) {
                    local.rx() -= img.width();
                } else if (local.x() < 0) {
                    local.rx() += img.width();
                }
                if (local.y() >= img.height()) {
                    local.ry() -= img.height();
                } else if (local.y() < 0) {
                    local.ry() += img.height();
                }
                if (img.pixelIndex(local) >= centreVal) {
                    acc += delta;
                }
                delta *= 2;
            }
            int p = mUniforms.indexOf(acc) + 1;
            mLBPImage.setPixel(centre, p);
        }
    }
    //mLBPImage.save(img.text("filename") + "_lbp.png");
}

QVector<QVector<float> > LBP::postprocess(const QImage &/*img*/) const
{
    QVector<QVector<float> > result;
    QVector<float> hist(mUniforms.size() + 1);
    float *histPtr = hist.data();
    for (int y = 0; y < mLBPImage.height(); y++) {
        const uchar *data = mLBPImage.constScanLine(y);
        for (int x = 0; x < mLBPImage.bytesPerLine(); x++) {
            const int p = data[x];
            histPtr[p]++;
        }
    }
    float sum = 0;
    for (int i = 0; i < hist.size(); i++) {
        sum += hist.at(i);
    }
    for (int i = 0; i < hist.size(); i++) {
        hist[i] /= sum;
    }
    result.append(hist);
    return result;
}

QVector<float> LBP::extract(const QImage &/*data*/, const int &/*x*/, const int &/*y*/) const
{
    return QVector<float>();
}

void LBP::preprocessTest(const QImage &data, const QImage &/*label*/)
{
    preprocess(data);
}

QVector<LabelledData> LBP::postprocessTest(const QImage &data, const QImage &label) const
{
    QVector<LabelledData> result;
    const int w = 32;
    const int h = 32;
    const int dw = data.width();
    const int dh = data.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            QVector<nnreal> hist(mUniforms.size() + 1);
            float *histPtr = hist.data();
            for (int ay = 0; ay < h; ay++) {
                int py = (y - h / 2 + ay) + dh;
                while (py >= dh) {
                    py -= dh;
                }
                #ifdef HAS_IMAGE_CONSTSCANLINE
                    const uchar *d = mLBPImage.constScanLine(py);
                #else
                    const uchar *d = mLBPImage.scanLine(py);
                #endif
                for (int ax = 0; ax < w; ax++) {
                    int px = (x - w / 2 + ax) + dw;
                    while (px >= dw) {
                        px -= dw;
                    }
                    const int p = d[x];
                    histPtr[p]++;
                }
            }
            float sum = 0;
            for (int i = 0; i < hist.size(); i++) {
                sum += hist.at(i);
            }
            for (int i = 0; i < hist.size(); i++) {
                hist[i] /= sum;
            }
            const LabelledData li(hist, label.pixelIndex(x, y));
            result.append(li);
        }
    }
    return result;
}
