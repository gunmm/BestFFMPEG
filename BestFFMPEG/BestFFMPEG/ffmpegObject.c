//
//  ffmpegObject.c
//  BestFFMPEG
//
//  Created by gunmm on 2018/7/12.
//  Copyright © 2018年 gunmm. All rights reserved.
//

#include "ffmpegObject.h"


AVFormatContext *pFormatCtxMP4 = NULL;
AVCodecContext *pCodecCtxMP4=NULL;
AVStream *video_stMP4 = NULL;

AVFormatContext *fmtCtx = NULL;
AVDictionary *optionsDict = NULL;
AVCodecContext *pCodeContext = NULL;
AVPacket *packet;
AVInputFormat *inputFmt;
FILE *fp;



int record_exit = 0;




int flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index)
{
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    enc_pkt.stream_index = stream_index;
    av_packet_rescale_ts(&enc_pkt,
                         fmt_ctx->streams[stream_index]->codec->time_base,
                         fmt_ctx->streams[stream_index]->time_base);
    
    while (1) {
        printf("Flushing stream #%u encoder\n", stream_index);
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        
        
        avcodec_send_frame(fmt_ctx->streams[stream_index]->codec, NULL);
        avcodec_receive_packet(fmt_ctx->streams[stream_index]->codec, &enc_pkt);
        
        
        
        
        
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame)
        {ret=0;break;}
        printf("success encoder 1 frame");
        
        // parpare packet for muxing
        enc_pkt.stream_index = stream_index;
        av_packet_rescale_ts(&enc_pkt,
                             fmt_ctx->streams[stream_index]->codec->time_base,
                             fmt_ctx->streams[stream_index]->time_base);
        ret = av_interleaved_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}


const char* out_file  = "/usr/local/Cellar/ffmpeg/4.0.1/bin/test.yuv";



void captureOneFrame(const char* input_name, char *savePath, const char* framerate, const char* video_size, long long saveLength, DownloadCallback callback){

    fp = fopen(out_file, "wb+");
    
    //设置输入
    AVInputFormat *ifmt = av_find_input_format(input_name);
    if (ifmt == NULL)    {
        printf("can not find_input_format\n");
        return;
    }
    
    
    //连设备 设置参数
    av_dict_set(&optionsDict, "framerate", framerate, 0);
    //    av_dict_set(&optionsDict, "pixel_format", "uyvy422", 0);
    //    av_dict_set(&optionsDict, "video_size", "640*480", 0);
    av_dict_set(&optionsDict, "video_size", video_size, 0);
    
    int aaa = -1;
    aaa = avformat_open_input(&fmtCtx, "0", ifmt,&optionsDict);
    if(aaa < 0){
        printf("can not open_input\n");
        return;
    }
    
    
    if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
        printf("can not find_stream_info\n");
        return;
    }
    
    int video_index = -1;
    AVStream *videoStream = NULL;
    for (int i = 0; i != fmtCtx->nb_streams; ++i) {
        printf("%d", i);
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            videoStream = fmtCtx->streams[i];
            break;
        }
    }
    
    if (video_index == -1) {
        printf("can not find any video stream\n");
        return;
    }
    
    
    
    
    AVCodec *pCodecnext = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if(pCodecnext == NULL){
        printf("can not find any decoder\n");
        return;
    }
    
    pCodeContext = avcodec_alloc_context3(pCodecnext);
    avcodec_parameters_to_context(pCodeContext, videoStream->codecpar);
    
    if (avcodec_open2(pCodeContext, pCodecnext, NULL) != 0) {
        printf("can not open decoder\n");
        return;
    }
    
    
    
    av_dump_format(fmtCtx, 0, 0, 0);
    
    
    
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    //用来接收packet拿出来的422数据
    AVFrame *frame = av_frame_alloc();
    //用来接收422-》420的数据
    AVFrame *frame_yuv = av_frame_alloc();
    
    
    //初始化420的frame
    int bufSize = avpicture_get_size(AV_PIX_FMT_YUV420P, pCodeContext->width, pCodeContext->height);
    uint8_t *buffer = (uint8_t *)av_malloc(bufSize);
    frame_yuv->format = AV_PIX_FMT_YUV420P;
    avpicture_fill((AVPicture *)frame_yuv, buffer, AV_PIX_FMT_YUV420P, pCodeContext->width, pCodeContext->height);
    
    
    
    AVPacket pkt; //创建已编码帧
    
    
    
    
    
    
    
    //窗口
    SDL_Window *sdlWindow = SDL_CreateWindow("camera01", 0, 0, pCodeContext->width, pCodeContext->height, SDL_WINDOW_OPENGL);

    SDL_Renderer *render = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_Texture *sdlTexture = SDL_CreateTexture(render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodeContext->width, pCodeContext->height);
    SDL_Rect sdlRect;
    sdlRect.x = sdlRect.y = 0;
    sdlRect.w = pCodeContext->width;
    sdlRect.h = pCodeContext->height;
  
    
    
    
    
    int i = 0;
    while (1) {

        if (record_exit == 1) {
            endStream();
            break;
        }
        
            if (av_read_frame(fmtCtx, packet) >= 0) {
                printf("data length = %d\n",packet->size);
                
                
                int ret = avcodec_send_packet(pCodeContext, packet);
                if (ret != 0) {
                    printf("can not decode packet\n");
                    return;
                }
                
                
                
                
                if (avcodec_receive_frame(pCodeContext, frame) >= 0) {
                    struct SwsContext *img_convert_ctx;
                    
                    img_convert_ctx = sws_getContext(pCodeContext->width, pCodeContext->height, pCodeContext->pix_fmt, pCodeContext->width, pCodeContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
                    
                    sws_scale(img_convert_ctx, (const uint8_t* const*)frame->data, frame->linesize, 0, pCodeContext->height, frame_yuv->data, frame_yuv->linesize);
                

                    if (frame_yuv->format == AV_PIX_FMT_YUV420P) {
                        
                        
                        SDL_UpdateYUVTexture(sdlTexture, &sdlRect, frame_yuv->data[0], frame_yuv->linesize[0], frame_yuv->data[1], frame_yuv->linesize[1], frame_yuv->data[2], frame_yuv->linesize[2]);
                        
                        
                        if (fp) {
                            fwrite(frame_yuv->data[0], 1, pCodeContext->width*pCodeContext->height, fp);
                            fwrite(frame_yuv->data[1], 1, pCodeContext->width*pCodeContext->height/4, fp);
                            fwrite(frame_yuv->data[2], 1, pCodeContext->width*pCodeContext->height/4, fp);
                            
                        }
                        callback(frame_yuv);
                        
                        
                        //AVFrame PTS
                        frame_yuv->pts=i;
                        int got_picture = 0;
                        
                        if (i==0) {
                            
                            AVCodec *pCodecMP4=NULL;
                            
                            char *path = savePath;
                            char *name = ".mp4";

                            time_t time_T;
                            time_T = time(NULL);
                            struct tm *tmTime;
                            tmTime = localtime(&time_T);
                            char* format = "%Y-%m-%d %H:%M:%S";
                            char strTime[100];
                            strftime(strTime, sizeof(strTime), format, tmTime);


                            char *fileStr = malloc(strlen(path)+strlen(name)+30);
                            strcpy(fileStr,path);
                            strcat(fileStr,strTime);
                            strcat(fileStr,name);
                            printf("%s",fileStr);
                            
                            
                            const char *out_fileMP4 = NULL;
                            out_fileMP4 = fileStr;
                            
                            
                            
                            
//                            printf("---------------------------------------%s\n", strTime);
                            //
                            avformat_alloc_output_context2(&pFormatCtxMP4,NULL,NULL,out_fileMP4);
                            //                            free(fileStr);
                            AVOutputFormat *fmtMP4 = NULL;
                            
                            fmtMP4 = pFormatCtxMP4->oformat;
                            if(avio_open(&pFormatCtxMP4->pb,out_fileMP4,AVIO_FLAG_READ_WRITE))
                            {
                                printf("output file open fail\n");
                            }
                            
                            video_stMP4 = avformat_new_stream(pFormatCtxMP4,0);
                            if(video_stMP4==NULL)
                            {
                                printf("failed allocating output stram\n");
                            }
                            video_stMP4->time_base.num = 1;
                            video_stMP4->time_base.den =25;
                            
                            //[5] --编码器Context设置参数
                            pCodecCtxMP4 = video_stMP4->codec;
                            pCodecCtxMP4->codec_id = fmtMP4->video_codec;
                            pCodecCtxMP4->codec_type = AVMEDIA_TYPE_VIDEO;
                            pCodecCtxMP4->pix_fmt = AV_PIX_FMT_YUV420P;
                            pCodecCtxMP4->width=pCodeContext->width;
                            pCodecCtxMP4->height=pCodeContext->height;
                            pCodecCtxMP4->time_base.num = 1;
                            pCodecCtxMP4->time_base.den = 25;
                            pCodecCtxMP4->bit_rate = 400000;
                            pCodecCtxMP4->gop_size = 12;
                            
                            
                            
                            if(pCodecCtxMP4->codec_id == AV_CODEC_ID_H264)
                            {
                                pCodecCtxMP4->qmin = 10;
                                pCodecCtxMP4->qmax = 51;
                                pCodecCtxMP4->qcompress = 0.6;
                            }
                            if (pCodecCtxMP4->codec_id == AV_CODEC_ID_MPEG2VIDEO)
                                pCodecCtxMP4->max_b_frames = 2;
                            if (pCodecCtxMP4->codec_id == AV_CODEC_ID_MPEG1VIDEO)
                                pCodecCtxMP4->mb_decision = 2;
                            //[6] --寻找编码器并打开编码器
                            pCodecMP4 = avcodec_find_encoder(pCodecCtxMP4->codec_id);
                            if(!pCodecMP4)
                            {
                                printf("no right encoder!\n");
                            }
                            if(avcodec_open2(pCodecCtxMP4,pCodecMP4,NULL)<0)
                            {
                                printf("open encoder fail\n");
                            }
                            
                            int y_size = pCodeContext->width*pCodeContext->height;
                            av_new_packet(&pkt,bufSize*3);
                            
                            av_dump_format(pFormatCtxMP4,0,out_fileMP4,1);
                            //[7] --写头文件
                            avformat_write_header(pFormatCtxMP4,NULL);
                        }
                        
                        //编码
                        frame_yuv->width = pCodecCtxMP4->coded_width;
                        frame_yuv->height = pCodecCtxMP4->coded_height;
                        int ret = avcodec_encode_video2(pCodecCtxMP4,&pkt,frame_yuv,&got_picture);
                        if(ret<0)
                        {
                            printf("encoder fail!!\n");
                        }
                        
                        if(got_picture == 1)
                        {
                            printf("encoder success!!\n");
                            
                            
                            // parpare packet for muxing
                            pkt.stream_index = video_stMP4->index;
                            av_packet_rescale_ts(&pkt, pCodecCtxMP4->time_base, video_stMP4->time_base);
                            pkt.pos = -1;
                            ret = av_interleaved_write_frame(pFormatCtxMP4,&pkt);
                            av_free_packet(&pkt);
                        }
                        i ++;
                        
                        if (i == 30 * saveLength) {

                        
                            i = 0;
                            int ret = flush_encoder(pFormatCtxMP4,0);
                            if(ret < 0)
                            {
                                printf("flushing encoder failed!\n");
                            }
                            
                            //[10] --写文件尾
                            av_write_trailer(pFormatCtxMP4);
                            //                            return;
                            //释放内存
                            if(video_stMP4)
                            {
                                avcodec_close(video_stMP4->codec);
                            }
                            if(pFormatCtxMP4)
                            {
                                avio_close(pFormatCtxMP4->pb);
                                avformat_free_context(pFormatCtxMP4);
                            }
                        }
                        
                        
                    }
                    
                    SDL_RenderClear(render);
                    SDL_RenderCopy(render, sdlTexture, NULL, &sdlRect);
                    SDL_RenderPresent(render);
                }
                
                
            }
    }
    
    
    
    
    
}


void beginStream(const char* input_name, char *savePath, const char* framerate, const char* video_size, long long saveLength, DownloadCallback callback){
    av_register_all();
    avcodec_register_all();
    avdevice_register_all();
    avformat_network_init();
    
    
    
    //获取设备列表
//    AVFormatContext *pFormatCtx = avformat_alloc_context();
//    AVDictionary* options = NULL;
//    av_dict_set(&options,"list_devices","true",0);
//    AVInputFormat *iformat = av_find_input_format("avfoundation");
//    printf("Device Info=============\n");
//    int  aaa = -100;
//    aaa = avformat_open_input(&pFormatCtx,"video=Capture screen 0",iformat,&options);
    
    
    
    //调用方法查看和存储为MP4
    
    captureOneFrame(input_name, savePath, framerate, video_size, saveLength, callback);
    
    printf("111\n");
}

void setRecordValue(int recordValue) {
    record_exit = 1;
}


void endStream(void){
    int ret = flush_encoder(pFormatCtxMP4,0);
    if(ret < 0)
    {
        printf("flushing encoder failed!\n");
    }
    
    //[10] --写文件尾
    av_write_trailer(pFormatCtxMP4);
    
    
    //释放内存
    if(video_stMP4)
    {
        avcodec_close(video_stMP4->codec);
    }
    if(pFormatCtxMP4)
    {
        avio_close(pFormatCtxMP4->pb);
        avformat_free_context(pFormatCtxMP4);
    }
    
    if (fmtCtx) {
        avio_close(fmtCtx->pb);
        avformat_free_context(fmtCtx);
    }
    
    fclose(fp);
    avcodec_close(pCodeContext);
//    SDL_Quit();
    
    
    
}




////获取指定目录下的所有文件列表 author:wangchangshaui jlu
//char** getFileNameArray(const char *path, int* fileCount)
//{
//    int count = 0;
//    char **fileNameList = NULL;
//    struct dirent* ent = NULL;
//    DIR *pDir;
//    char dir[512];
//    struct stat statbuf;
//    
//    //打开目录
//    if ((pDir = opendir(path)) == NULL)
//    {
//        printf("Cannot open directory:%s\n", path);
//        return NULL;
//    }
//    //读取目录
//    while ((ent = readdir(pDir)) != NULL)
//    { //统计当前文件夹下有多少文件（不包括文件夹）
//        //得到读取文件的绝对路径名
//        snprintf(dir, 512, "%s/%s", path, ent->d_name);
//        //得到文件信息
//        lstat(dir, &statbuf);
//        //判断是目录还是文件
//        if (!S_ISDIR(statbuf.st_mode))
//        {
//            count++;
//        }
//    } //while
//    //关闭目录
//    closedir(pDir);
//    //    myLog("共%d个文件\n", count);
//
//    //开辟字符指针数组，用于下一步的开辟容纳文件名字符串的空间
//    if ((fileNameList = (char**) malloc(sizeof(char*) * count)) == NULL)
//    {
//        printf("Malloc heap failed!\n");
//        return NULL;
//    }
//
//    //打开目录
//    if ((pDir = opendir(path)) == NULL)
//    {
//        printf("Cannot open directory:%s\n", path);
//        return NULL;
//    }
//    //读取目录
//    int i;
//    for (i = 0; (ent = readdir(pDir)) != NULL && i < count;)
//    {
//        if (strlen(ent->d_name) <= 0)
//        {
//            continue;
//        }
//        //得到读取文件的绝对路径名
//        snprintf(dir, 512, "%s/%s", path, ent->d_name);
//        //得到文件信息
//        lstat(dir, &statbuf);
//        //判断是目录还是文件
//        if (!S_ISDIR(statbuf.st_mode))
//        {
//            if ((fileNameList[i] = (char*) alloca((strlen(ent->d_name) + 1)))
//                == NULL)
//            {
//                printf("Malloc heap failed!\n");
//                return NULL;
//            }
//            memset(fileNameList[i], 0, strlen(ent->d_name) + 1);
//            strcpy(fileNameList[i], ent->d_name);
//            printf("第%d个文件:%s\n", i, ent->d_name);
//            i++;
//        }
//    } //for
//    //关闭目录
//    closedir(pDir);
//    
//    *fileCount = count;
//    return fileNameList;
//}


