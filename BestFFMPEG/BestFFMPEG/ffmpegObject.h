//
//  ffmpegObject.h
//  BestFFMPEG
//
//  Created by gunmm on 2018/7/12.
//  Copyright © 2018年 gunmm. All rights reserved.
//



#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libpostproc/postprocess.h"
#include "libpostproc/postprocess.h"
#include "libavresample/avresample.h"
#include "SDL2/SDL.h"



//用来时时回调每一帧
typedef void (__stdcall *DownloadCallback)(AVFrame *frame_yuv);



//1.系统调用摄像头格式  2.MP4文件保存路径  3.帧率  4.视频尺寸   5.存储时一小段长度   6.每一帧的回调
void beginStream(const char* input_name, char *savePath, const char* framerate, const char* video_size, long long saveLength, DownloadCallback callback);

//结束视频  并且释放所有
void endStream(void);

void setRecordValue(int recordValue);


char** getFileNameArray(const char *path, int* fileCount);





