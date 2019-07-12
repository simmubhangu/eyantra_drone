/*
BSD 2-Clause License

Copyright (c) 2019, Simranjeet Singh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Created by vedanshi on 26/4/19.
Edited by badrobot15 on 20/5/19.

*/


extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
}

#include <iostream>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <eyantra_drone/liblewei.h>
#include <std_msgs/String.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

ros::Publisher chatter_pub;
image_transport::Publisher cam_pub;
sensor_msgs::ImagePtr msg;

uint8_t *pframe_pixel;
AVFrame *pVideoFrameIn, *pVideoFrameOut;
AVCodecContext *pCodecCtxc;
AVCodec *pdecode;
SwsContext *pSwsCtx;


static void read_buffer(void* lpParam, lewei_video_frame *pFrame);


int main(int argc, char **argv)
{

  ros::init(argc, argv, "droneCam");

  ros::NodeHandle n;

  image_transport::ImageTransport it(n);

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

    cam_pub.publish(msg);
    
    ros::spinOnce();
    
    video_free_frame_ram(pFrame);
    av_free_packet(&pkt);
    av_free(buffer);
    sws_freeContext(img_convert_ctx);
}
