#include "camera.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <QImage>
#include <QDebug>
#include <QThread>

static const int32_t FRAME_RATE = 5;

Camera::Camera(QObject *parent) :
    QObject(parent)
{
    /* 实例化 */
    capture = new cv::VideoCapture();
    timer = new QTimer(this);
    m_thread = new QThread();

    m_thread->start();
    //this->moveToThread(m_thread);

    /* 信号槽连接 */
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()));
    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    connect(m_thread, &QThread::finished, m_thread, &Camera::deleteLater);
}

Camera::~Camera()
{
    delete capture;
    capture = NULL;
}

void Camera::selectCameraDevice(int index)
{
    /* 如果有其他摄像头打开了，先释放 */
    if (capture->isOpened()) {
        capture->release();
    }

    /* 打开摄像头设备 */
    capture->open(index);
}

bool Camera::cameraProcess(bool bl)
{
    if (bl) {
        /* 为什么是33？1000/33约等于30帧，也就是一秒最多显示30帧  */
        timer->start(1000 / FRAME_RATE);
    } else {
        timer->stop();
    }
    /* 返回摄像头的状态 */
    return capture->isOpened();
}

void Camera::timerTimeOut()
{
    /* 如果摄像头没有打开，停止定时器，返回 */
    if (!capture->isOpened()) {
        timer->stop();
        return;
    }

    static cv::Mat frame;
    *capture >> frame;
    if (frame.cols)
        /* 发送图片信号 */
        emit readyImage(matToQImage(frame));
}

QImage Camera::matToQImage(const cv::Mat &img)
{
    /* USB摄像头和OV5640等都是RGB三通道，不考虑单/四通道摄像头 */
    if(img.type() == CV_8UC3) {
        /* 得到图像的的首地址 */
        const uchar *pimg = (const uchar*)img.data;

        /* 以img构造图片 */
        QImage qImage(pimg, img.cols, img.rows, img.step,
                      QImage::Format_RGB888);

        /* 在不改变实际图像数据的条件下，交换红蓝通道 */
        return qImage.rgbSwapped();
    }

    /* 返回QImage */
    return QImage();
}
