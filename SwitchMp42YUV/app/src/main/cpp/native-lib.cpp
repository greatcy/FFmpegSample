#include <jni.h>
#include <stdio.h>
#include <time.h>

#ifdef ANDROID

#include <android/log.h>

#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>

JNIEXPORT jint JNICALL
Java_com_eli_switchmp42yuv_MainActivity_parseMP4Video(JNIEnv *env, jobject instance,
                                                      jstring inputUrl, jstring outputUrl) {
    const char *input_str = env->GetStringUTFChars(inputUrl, 0);
    const char *output_str = env->GetStringUTFChars(outputUrl, 0);

    AVFormatContext *pFormatCtx;
    int i, videoIndex;
    AVCodecContext *avCodecContext;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double time_duration = 0.0;

    char info[1000] = {0};

    //TODO receive FFMPEG log
    //receive

    //初始化格式支持库 libavformat
    av_register_all();

    //初始化网络组件
    avformat_network_init();

    //生成 AVFormatContext
    pFormatCtx = avformat_alloc_context();

    //从媒体流读取媒体数据格式,后面需要调用avformat_close_input关闭流
    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.%s\n", input_str);
        return -1;
    }

    //从打开的格式中读取媒体信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }

    //找到stream里面的视频类型(streams里面可能包含音频或者字幕)
    videoIndex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }

    if (videoIndex == -1) {
        LOGE("Couldn't find a video stream.\n");
        return -1;
    }

    //找到视频(未解码)对应的解码器
    avCodecContext = pFormatCtx->streams[videoIndex]->codec;
    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codecpar->codec_id);
    if (pCodec == NULL) {
        LOGE("Couldn't find Codec.\n");
        return -1;
    }

    //打开解码器
    if (avcodec_open2(avCodecContext, pCodec, NULL) < 0) {
        LOGE("Could not open codec \n");
        return -1;
    }

    //分配流里面的帧
    pFrame = av_frame_alloc();
    //分配yuv 420对应的帧
    pFrameYUV = av_frame_alloc();

    //初始化yuv 420帧数据
    out_buffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, avCodecContext->width,
                                     avCodecContext->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         avCodecContext->width, avCodecContext->height, 1);

    //初始化数据包
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    //创建帧格式转换上下文
    img_convert_ctx = sws_getContext(avCodecContext->width, avCodecContext->height,
                                     avCodecContext->pix_fmt,
                                     avCodecContext->width, avCodecContext->height,
                                     AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    LOGI(info, "[Input     ]%s\n", input_str);
    LOGI(info, "%s[Output    ]%s\n", info, output_str);
    LOGI(info, "%s[Format    ]%s\n", info, pFormatCtx->iformat->name);
    LOGI(info, "%s[Codec     ]%s\n", info, avCodecContext->codec->name);
    LOGI(info, "%s[Resolution]%dx%d\n", info, avCodecContext->width, avCodecContext->height);

    fp_yuv = fopen(output_str, "w");
    if (fp_yuv == NULL) {
        printf("cannot open output file.\n");
        return -1;
    }

    //帧数和计时器
    frame_cnt = 0;
    time_start = clock();

    //遍历每个packet 找到属于视频的packet
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoIndex) {
            ret = avcodec_decode_video2(avCodecContext, pFrame, &got_picture, packet);
            if (ret < 0) {
                LOGE("DECODE ERROR!\n");
                return -1;
            }

            if (got_picture) {
                //首先转换frame格式，然后按照YUV的plane模式存储到文件中
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0, pFrame->height, pFrameYUV->data, pFrameYUV->linesize);
                y_size = avCodecContext->width * avCodecContext->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);
                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);

                //Output info
                char pictype_str[10] = {0};
                switch (pFrame->pict_type) {
                    case AV_PICTURE_TYPE_I:
                        sprintf(pictype_str, "I");
                        break;
                    case AV_PICTURE_TYPE_P:
                        sprintf(pictype_str, "P");
                        break;
                    case AV_PICTURE_TYPE_B:
                        sprintf(pictype_str, "B");
                        break;
                    default:
                        sprintf(pictype_str, "Other");
                        break;
                }
                LOGI("Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
                frame_cnt++;

            }
        }
        av_free_packet(packet);
    }

    //上面的帧遍历后可能会存在一些未写出的帧在解码器中（猜测跟写文件的缓存类似）
    while (1) {
        ret = avcodec_decode_video2(avCodecContext, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  avCodecContext->height,
                  pFrameYUV->data, pFrameYUV->linesize);
        int y_size = avCodecContext->width * avCodecContext->height;
        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
        fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
        fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
        //Output info
        char pictype_str[10] = {0};
        switch (pFrame->pict_type) {
            case AV_PICTURE_TYPE_I:
                sprintf(pictype_str, "I");
                break;
            case AV_PICTURE_TYPE_P:
                sprintf(pictype_str, "P");
                break;
            case AV_PICTURE_TYPE_B:
                sprintf(pictype_str, "B");
                break;
            default:
                sprintf(pictype_str, "Other");
                break;
        }
        LOGI("Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
        frame_cnt++;
    }
    time_finish = clock();
    time_duration = (double) (time_finish - time_start);

    LOGI(info, "%s[Time      ]%fms\n", info, time_duration);
    LOGI(info, "%s[Count     ]%d\n", info, frame_cnt);

    sws_freeContext(img_convert_ctx);

    fclose(fp_yuv);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(avCodecContext);
    avformat_close_input(&pFormatCtx);


    env->ReleaseStringUTFChars(inputUrl, input_str);
    env->ReleaseStringUTFChars(outputUrl, output_str);

    LOGI("all parse done!");
    return 0;
}
}
