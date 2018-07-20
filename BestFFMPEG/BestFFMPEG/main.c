//
//  main.c
//  BestFFMPEG
//
//  Created by gunmm on 2018/7/11.
//  Copyright © 2018年 gunmm. All rights reserved.
//

#include <stdio.h>
#include "ffmpegObject.h"



SDL_Texture *sdlTexture = NULL;
SDL_Rect sdlRect;
SDL_Renderer *render;

void __stdcall onDownloadFinished(AVFrame *frame_yuv)
{
    
}


void * print_a(void *);

int main(int argc, const char * argv[]) {
    
    
    pthread_t t0;
    pthread_create(&t0, NULL, print_a, NULL);
    

    beginStream("avfoundation", "/usr/local/Cellar/ffmpeg/4.0.1/bin/", "30", "320*240", 30, onDownloadFinished);
    
    
    printf("1111");
    
    return 0;
}


// 线程A 方法
void * print_a(void *a){
    for(int i = 0;i < 10000; i++){
        for(int j = 0;j < 10000; j++){
            for(int k = 0;k < 10000; k++){
                printf("%d-%d-%d\n", i, j, k);
                if (j == 1000) {
                    setRecordValue(1);
                    break;
                }
            }
        }
    }
    
    return NULL;
}


