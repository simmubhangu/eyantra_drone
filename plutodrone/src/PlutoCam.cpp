//
// Created by vedanshi on 26/4/19.
// Edited by badrobot15 on 20/5/19.
//

extern "C" {
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
}

#include <iostream>

#include "ros/ros.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>


#include "plutodrone/liblewei.h"
#include "std_msgs/String.h"

#include <opencv2/opencv.hpp>

using namespace cv;

using namespace std;

ros::Publisher chatter_pub;
image_transport::Publisher cam_pub;
sensor_msgs::ImagePtr msg;

sensor_msgs::CameraInfo info_pluto;

ros::Publisher pub_info_pluto;

uint8_t *pframe_pixel;
AVFrame *pVideoFrameIn, *pVideoFrameOut;
AVCodecContext *pCodecCtxc;
AVCodec *pdecode;
SwsContext *pSwsCtx;


static void read_buffer(void* lpParam, lewei_video_frame *pFrame);


int main(int argc, char **argv)
{

  ros::init(argc, argv, "plutocam");

  ros::NodeHandle n;


  image_transport::ImageTransport it(n);

  pub_info_pluto = n.advertise<sensor_msgs::CameraInfo>("drone_cam/camera_info", 1);

  cam_pub = it.advertise("/drone_cam", 1);

  lewei_initialize_stream();

  long ret = lewei_start_stream(nullptr, read_buffer);

  pthread_join(ret, NULL);

  return 0;
}

static void read_buffer(void* lpParam, lewei_video_frame *pFrame)
{

    int ret = 0;
    int got_picture = 0;

    if (pFrame->size <= 0)
    {
       video_free_frame_ram(pFrame);
       return;
    }

    if (!pdecode) {
        avcodec_register_all();

        pdecode = avcodec_find_decoder(AV_CODEC_ID_H264);

        pCodecCtxc = avcodec_alloc_context3(pdecode);
        pCodecCtxc->bit_rate = 125000;
        pCodecCtxc->width = 1280;
        pCodecCtxc->height = 720;

        if(!pdecode) {
            cout <<"no h264 decoder found"<< endl;
        }

        if (avcodec_open2(pCodecCtxc, pdecode, nullptr) < 0) {
            cout << "could not open codec" << endl;
            return;
        }
    }

    if (!pVideoFrameIn)
        pVideoFrameIn = av_frame_alloc();

    if (!pVideoFrameOut)
    	pVideoFrameOut = av_frame_alloc();//avcodec_alloc_frame();

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = pFrame->buf;
    pkt.size = pFrame->size;
    pkt.flags = pFrame->iFrame;

    uint8_t *buffer;
    int numBytes;

    AVPixelFormat pFormat = AV_PIX_FMT_BGR24;

    numBytes = avpicture_get_size(pFormat,pCodecCtxc->width,pCodecCtxc->height);

    buffer = (uint8_t *) av_malloc(numBytes*sizeof(uint8_t));

    avpicture_fill((AVPicture *) pVideoFrameOut,buffer,pFormat,pCodecCtxc->width,pCodecCtxc->height);

    ret = avcodec_decode_video2(pCodecCtxc, pVideoFrameIn, &got_picture, &pkt);

    if (ret < 0)
    {
        cout << "decode error" << endl;
        return;
    }

    struct SwsContext * img_convert_ctx;

    img_convert_ctx = sws_getCachedContext(NULL,pCodecCtxc->width, pCodecCtxc->height, pCodecCtxc->pix_fmt, pCodecCtxc->width,
        pCodecCtxc->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,NULL);

    sws_scale(img_convert_ctx, ((AVPicture*)pVideoFrameIn)->data, ((AVPicture*)pVideoFrameIn)->linesize, 0, pCodecCtxc->height,
        ((AVPicture *)pVideoFrameOut)->data, ((AVPicture *)pVideoFrameOut)->linesize);

    Mat img(pVideoFrameIn->height,pVideoFrameIn->width,CV_8UC3,pVideoFrameOut->data[0]);

    try{
        msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img).toImageMsg();
    }catch(cv_bridge::Exception& e){
        ROS_ERROR("CV Bridge error: %s", e.what());
        return;
    }


    ros::Rate loop_rate(5);
    // imshow("display",img);
    cvWaitKey(1);

    info_pluto.header.stamp = ros::Time::now();
    pub_info_pluto.publish(info_pluto);

    cam_pub.publish(msg);

    ros::spinOnce();

    video_free_frame_ram(pFrame);
    av_free_packet(&pkt);
    av_free(buffer);
    sws_freeContext(img_convert_ctx);
}
