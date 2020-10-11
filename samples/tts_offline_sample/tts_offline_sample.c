/*
 * * 语音合成（Text To Speech，TTS）技术能够自动将任意文字实时转换为连续的
 * * 自然语音，是一种能够在任何时间、任何地点，向任何人提供语音信息服务�?
 * * 高效便捷手段，非常符合信息时代海量数据、动态更新和个性化查询的需求�?
 * */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "../../include/qtts.h"
#include "../../include/msp_cmn.h"
#include "../../include/msp_errors.h"
typedef int SR_DWORD;
typedef short int SR_WORD ;

/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
    char            riff[4];                // = "RIFF"
    int             size_8;                 // = FileSize - 8
    char            wave[4];                // = "WAVE"
    char            fmt[4];                 // = "fmt "
    int             fmt_size;               // = 下一个结构体的大�?: 16

    short int       format_tag;             // = PCM : 1
    short int       channels;               // = 通道�?: 1
    int             samples_per_sec;        // = 采样�?: 8000 | 6000 | 11025 | 16000
    int             avg_bytes_per_sec;      // = 每秒字节�?: samples_per_sec * bits_per_sample / 8
    short int       block_align;            // = 每采样点字节�?: wBitsPerSample / 8
    short int       bits_per_sample;        // = 量化比特�? 8 | 16

    char            data[4];                // = "data";
    int             data_size;              // = 纯数据长�?: FileSize - 44 
} wave_pcm_hdr;

/* 默认wav音频头部数据 */
wave_pcm_hdr default_wav_hdr = 
{
    { 'R', 'I', 'F', 'F' },
    0,
    {'W', 'A', 'V', 'E'},
    {'f', 'm', 't', ' '},
    16,
    1,
    1,
    16000,
    32000,
    2,
    16,
    {'d', 'a', 't', 'a'},
    0  
};
/* 文本合成 */
int text_to_speech(const char* src_text, 
				   FILE*        fp, 
				   const char* params)
{
    int          ret          = -1;
    const char*  sessionID    = NULL;
    unsigned int audio_len    = 0;
	unsigned int data_size    = 0;
    int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

    if (NULL == src_text)
    {
        printf("params is error!\n");
        return data_size;
    }

    /* 开始合�?*/
    sessionID = QTTSSessionBegin(params, &ret);
    if (MSP_SUCCESS != ret)
    {
        printf("QTTSSessionBegin failed, error code: %d.\n", ret);
        return data_size;
    }
    ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
    if (MSP_SUCCESS != ret)
    {
        printf("QTTSTextPut failed, error code: %d.\n",ret);
        QTTSSessionEnd(sessionID, "TextPutError");
        return data_size;
    }
	
    while (1) 
    {
        /* 获取合成音频 */
        const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
        if (MSP_SUCCESS != ret)
            break;
        if (NULL != data)
        {
            fwrite(data, audio_len, 1, fp);
            data_size += audio_len; //计算data_size大小
        }
        if (MSP_TTS_FLAG_DATA_END == synth_status)
            break;
    }
    printf("\n");
    if (MSP_SUCCESS != ret)
    {
        printf("QTTSAudioGet failed, error code: %d.\n",ret);
        QTTSSessionEnd(sessionID, "AudioGetError");
    }
	else
	{ 
	    /* 合成完毕 */
	    ret = QTTSSessionEnd(sessionID, "Normal");
	    if (MSP_SUCCESS != ret)
	    {
	        printf("QTTSSessionEnd failed, error code: %d.\n",ret);
	    }
	}

    return data_size;
}

int main(int argc, char* argv[])
{
    int         ret                  = MSP_SUCCESS;
    const char* login_params         = "appid = 5f827dae, work_dir = .";//登录参数,appid与msc库绑�?请勿随意改动
    FILE*        fp           = NULL;
	wave_pcm_hdr wav_hdr	  = default_wav_hdr;
	unsigned int data_size	  = 0;
	FILE*		 fp_text 		  = NULL;
	char buf[10000];
	/*
 *     * rdn:           合成音频数字发音方式
 *         * volume:        合成音频的音�?
 *             * pitch:         合成音频的音�?
 *                 * speed:         合成音频对应的语�?
 *                     * voice_name:    合成发音�?
 *                         * sample_rate:   合成音频采样�?
 *                             * text_encoding: 合成文本编码格式
 *                                 *
 *                                     */
    const char* session_begin_params = "engine_type = local,voice_name=xiaoyan, text_encoding = UTF8, tts_res_path = fo|res/tts/xiaoyan.jet;fo|res/tts/common.jet, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
    const char* filename             = "tts_sample.wav"; //合成的语音文件名�?
    const char* text                 = "亲爱的用户，您好，这是一个语音合成示例，感谢您对科大讯飞语音技术的支持！科大讯飞是亚太地区最大的语音上市公司，股票代码：002230"; //合成文本
    /* 用户登录 */
    ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取
    if (MSP_SUCCESS != ret)
    {
        printf("MSPLogin failed, error code: %d.\n", ret);
        goto exit ;//登录失败，退出登�?
    }

    printf("\n###########################################################################\n");
    printf("## 语音合成（Text To Speech，TTS）技术能够自动将任意文字实时转换为连续的 ##\n");
    printf("## 自然语音，是一种能够在任何时间、任何地点，向任何人提供语音信息服务�? ##\n");
    printf("## 高效便捷手段，非常符合信息时代海量数据、动态更新和个性化查询的需求�? ##\n");
    printf("###########################################################################\n\n");

    fp = fopen(filename, "wb");
	fp_text = fopen("input.txt", "r");
    if (NULL == fp)
    {
        printf("open %s error.\n", filename);
        return ret;
    }

	if (NULL == fp_text)
    {
        printf("open %s error.\n", "input.txt");
        return ret;
    }

    /* 文本合成 */
    printf("开始合成...\n");
    fwrite(&wav_hdr, sizeof(wav_hdr) ,1, fp); //添加wav音频头，使用采样率为16000

	memset(buf, 0, sizeof(buf));
    while(fgets(buf, 10000, fp_text))
    {
        data_size += text_to_speech(buf, fp, session_begin_params);
    }

    /* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += data_size + (sizeof(wav_hdr) - 8);
	wav_hdr.data_size += data_size;
	
	/* 将修正过的数据写回文件头�?音频文件为wav格式 */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8,sizeof(wav_hdr.size_8), 1, fp); //写入size_8的�?
	fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
	fwrite(&wav_hdr.data_size,sizeof(wav_hdr.data_size), 1, fp); //写入data_size的�?
	fclose(fp);
	fclose(fp_text);
	fp = NULL;
	fp_text = NULL;
	
	printf("合成完毕\n");
exit:
    printf("按任意键退�?...\n");
    getchar();
    MSPLogout(); //退出登�?

    return 0;
}

