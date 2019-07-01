//
// Created by vedanshi on 18/4/19.
//

#ifndef LEWEISTREAM_LIBLEWEI_H
#define LEWEISTREAM_LIBLEWEI_H


#ifdef __cplusplus
extern "C" {
#endif

#define LEWEI_ERROR_BASE		8000
#define LEWEI_ERROR_NOT_INITIALIZED	(LEWEI_ERROR_BASE+1)
#define LEWEI_ERROR_MALLOC_FAIL		(LEWEI_ERROR_BASE+2)
#define LEWEI_ERROR_NO_CALLBACK		(LEWEI_ERROR_BASE+3)
#define LEWEI_ERROR_SOCKET			(LEWEI_ERROR_BASE+4)
#define LEWEI_ERROR_CREATE_FILE		(LEWEI_ERROR_BASE+5)

typedef struct _LEWEI_VIDEO_FRAME_T
{
    int64_t timestamp;			//video֡ʱ����
    uint32_t iFrame;			//�Ƿ��ؼ�֡
    uint32_t size;				//֡����
    uint8_t *buf;			//֡����buffer
} lewei_video_frame;

typedef void P_LEWEI_CALLBACK(void *param, lewei_video_frame *pframe);

void video_free_frame_ram(void *pFrame);
long lewei_start_stream(void *mydata, P_LEWEI_CALLBACK pcallback);
void lewei_stop_stream();

int lewei_initialize_stream();
void lewei_finalize_stream();


#ifdef __cplusplus
}
#endif

#endif //LEWEISTREAM_LIBLEWEI_H

