
#include "math.h"
#include "ameba_soc.h"

#include "rpc_api.h"
#include "rpc_struct.h"
#include "ring_buffer.h"
#include "audio/audio_record.h"
#include "audio/audio_control.h"

#include "voice_manager.h"
//#include "voice_player.h"

#include "VoiceRPC_Agent_data.h"
#include "VoiceRPC_System.h"
#include "VoiceRPC_Agent.h"

#define DUMP_DATA_ENABLE 1
#if DUMP_DATA_ENABLE
#define DUMP_PAGE_SIZE 1024
#define DUMP_PAGE_NUM 4
#define MIC_FRAME_LEN 256
#define VAD_FRAME_LEN 512
typedef struct wakeup_info_to_host_st {
    unsigned int wp_frame_num;
    int direction;
    int keyword_id;
    int word_duration_num;
    int word_duration[16];
    int location;
    char wakeup_keywords[32];
    float score;
    float dci_result;
    //int resevred[33];
} wakeup_info_to_host_t;
#define DSP_VERSION_LEN                 (64)

typedef struct dbg_ver_info_st {
    char wak_src_version[DSP_VERSION_LEN];
    char wak_eng_version[DSP_VERSION_LEN];
    char dci_version[DSP_VERSION_LEN];
    char aec_version[DSP_VERSION_LEN];
    char productor_version[DSP_VERSION_LEN];
    char build_time[DSP_VERSION_LEN];
} dbg_ver_info_t;
#include "pc_recorder.h"
extern pr_adapter_t pr_adapter;
char g_dump_data_buffer[DUMP_PAGE_NUM * DUMP_PAGE_SIZE]__attribute__((aligned(64)));
uint8_t g_vad_data[VAD_FRAME_LEN] = {0};

extern void uart_audio_dump_data(char *buffer);
extern void uart_audio_dump_init(void);
#endif


struct RTAudioRecord *audio_record = NULL;
struct VOICE_RPC_INSTANCE *g_instance = NULL;

static RingBuffer *g_mic_ringbuffer = NULL;
static RingBuffer *g_vad_ringbuffer = NULL;
static RingBuffer *g_wkmsg_ringbuffer = NULL;

static int g_record_state = -1;
static void *g_record_data = NULL;
static uint8_t *g_res_data = NULL;

static bool g_thread_running = false;
static bool g_recording = false;
static uint32_t g_mic_data_cnt = 0;

CLNT_STRUCT clnt;
static void StartRecordTask(void);

static bool SendHostCmd(uint32_t cmd, void *data)
{
	VOICE_RPC_USR_CMD arg;
	arg.cmd = (long)cmd;
	arg.data_addr = (uint32_t)data;
	VOICE_RPC_ToAgent_USR_CMD_0(&arg, &clnt);
}

static bool SyncData(void)
{
	bool ret = true;
	uint32_t size = RingBuffer_Space(g_mic_ringbuffer);
	if (size >= RECORD_SIZE) {
		RingBuffer_Write(g_mic_ringbuffer, g_record_data, RECORD_SIZE);
		g_mic_data_cnt++;
	} else {
		LOGV("%s ringbuffer overflow size=%ld", __FUNCTION__, size);
		ret = false;
	}
	return ret;
}
static void RecordLoop(void *param)
{
	(void)param;
	int index = 0, cnt = 0;
	LOGD("%s Enter, g_thread_running=%d", __FUNCTION__, g_thread_running);
	StartRecordTask();
	while (g_thread_running) {
		g_recording = true;
		if (g_record_state == -1) {
			LOGD("%s Enter, g_record_state =%d", __FUNCTION__, g_record_state);
			break;
		}
		if (g_record_data != NULL) {
			if(RECORD_SIZE == RTAudioRecord_Read(audio_record, g_record_data, RECORD_SIZE, true))//;			
			if (g_record_state != 0) {
				LOGD("%s Enter, g_record_state =%d", __FUNCTION__, g_record_state);
				continue;
			}
			while (g_thread_running && !SyncData()) {
				LOGD("SyncData fail");
				rtos_time_delay_ms(1);
			}
			//if(cnt++%500 == 0){
			//	LOGD("recoder_tick");
			//}
#if DUMP_DATA_ENABLE
			if (pr_adapter.record_status == RECORD_BUSY) {
				index = pr_adapter.rx_cnt % DUMP_PAGE_NUM;
				if (RingBuffer_Available(g_vad_ringbuffer) >= VAD_FRAME_LEN) {
					RingBuffer_Read(g_vad_ringbuffer, (uint8_t *)g_vad_data, VAD_FRAME_LEN);
					for (int j = 0; j < MIC_FRAME_LEN / 2; j++) {
						((short *)(g_dump_data_buffer + index * DUMP_PAGE_SIZE))[4 * j] = ((short *)g_record_data)[3 * j];
						((short *)(g_dump_data_buffer + index * DUMP_PAGE_SIZE))[4 * j + 1] = ((short *)g_record_data)[3 * j + 1];
						((short *)(g_dump_data_buffer + index * DUMP_PAGE_SIZE))[4 * j + 2] = ((short *)g_record_data)[3 * j + 2]; //ref
						((short *)(g_dump_data_buffer + index * DUMP_PAGE_SIZE))[4 * j + 3] = ((short *)(g_vad_data))[j]; //vad out
					}
					uart_audio_dump_data(g_dump_data_buffer + index * DUMP_PAGE_SIZE);
				}
			} else {
				while (RingBuffer_Available(g_vad_ringbuffer) >= VAD_FRAME_LEN ) {
					RingBuffer_Read(g_vad_ringbuffer, (uint8_t *)g_vad_data, VAD_FRAME_LEN);
				}
			}
#endif

		} else {
			rtos_time_delay_ms(10);
		}
	}
	LOGD("%s Enter, rtos_task_delete", __FUNCTION__);
	g_recording = false;
	rtos_task_delete(NULL);
}

static void StartRecordTask(void)
{
	LOGV("%s Enter", __FUNCTION__);
	unsigned int rate = 16000;
	//*create audio_record*/
	audio_record = RTAudioRecord_Create();
	if (!audio_record) {
		LOGE("record create failed");
		return;
	}

	RTAudioRecordConfig record_config;
	record_config.sample_rate = rate;
	record_config.format = RTAUDIO_FORMAT_PCM_16_BIT;
	record_config.channel_count = 3;

	record_config.device = RTDEVICE_IN_MIC;
	record_config.buffer_bytes = RECORD_SIZE;
	RTAudioRecord_Init(audio_record, &record_config, RTAUDIO_INPUT_FLAG_NONE);
	RTAudioRecord_Start(audio_record);
	RTAudioControl_SetChannelMicCategory(0, RTAUDIO_AMIC1);
	RTAudioControl_SetChannelMicCategory(1, RTAUDIO_AMIC2);
	RTAudioControl_SetChannelMicCategory(2, RTAUDIO_AMIC3);

	RTAudioControl_SetCaptureChannelVolume(0, 0x50);
	RTAudioControl_SetCaptureChannelVolume(1, 0x50);
	RTAudioControl_SetMicBstGain(RTAUDIO_AMIC1, RTAUDIO_MICBST_GAIN_0DB);
	RTAudioControl_SetMicBstGain(RTAUDIO_AMIC2, RTAUDIO_MICBST_GAIN_0DB);
	RTAudioControl_SetMicBstGain(RTAUDIO_AMIC3, RTAUDIO_MICBST_GAIN_0DB);

	RTAudioRecord_SetParameters(audio_record, "cap_mode=no_afe_pure_data");
	LOGD("RecordStartTask.. END");
}

static HRESULT *OnError(VOICE_RPC_ERROR_STATE *pParam, RPC_STRUCT *pRpcStruct, HRESULT *pRes)
{
	(void)pParam;
	(void)pRpcStruct;
	*pRes = 0;
	return pRes;
}
static HRESULT *OnRecognizer(VOICE_RPC_RECOGNIZER_RESULT *pParam, RPC_STRUCT *pRpcStruct, HRESULT *pRes)
{
	wakeup_info_to_host_t tmp;
	(void)pRpcStruct;
	long uv_ret = pParam->uv_ret;
	long word_id = pParam->word_id;
	float confidence = pParam->confidence;
	LOGD("OnRecognizer ret:%d, word_id:%d, confidence:%f", (int)uv_ret, (int)word_id, confidence);
	while (RingBuffer_Available(g_wkmsg_ringbuffer) > sizeof(wakeup_info_to_host_t)) {
		RingBuffer_Read(g_wkmsg_ringbuffer, (uint8_t *)&tmp, sizeof(wakeup_info_to_host_t));
		LOGD("WK  id:%d, word:%s, score:%f, dir:%d", tmp.keyword_id, tmp.wakeup_keywords, tmp.score, tmp.direction);
	}
	*pRes = 0;
	return pRes;
}
void ReadVadByOnline(uint8_t *data_buffer, int data_buffer_len, uint32_t *data_no)
{
	int64_t seq = 0;
	while (RingBuffer_Available(g_vad_ringbuffer) < (uint32_t)data_buffer_len) {
		rtos_time_delay_ms(1);
	}
	RingBuffer_Read(g_vad_ringbuffer, (uint8_t *)data_buffer, data_buffer_len);
	*data_no = g_mic_data_cnt;
	//memcpy(&seq, data_buffer, 8);
	//if (seq % 1000 == 0) {
	//	LOGD("ReadVadByOnline len=%d, seq=%lld", data_buffer_len, seq);
	//}
}

void OnlineTask(void *param)
{
	(void) param;
	uint8_t tmp_data[512] = {0};
	short tmp1=0,tmp2 = 0;
	uint32_t frame_no = 0;
	while (1) {
		ReadVadByOnline(tmp_data, 512, &frame_no);
		rtos_time_delay_ms(5);
		tmp1=((short *)tmp_data)[0];
		if(tmp2!=tmp1)
		{
			LOGD("tmp1=%d, tmp2=%d", (int)tmp1, (int)tmp2);
			tmp2=tmp1;
		}
	}
	rtos_task_delete(NULL);
}

int32_t Init(void)
{
	LOGV("%s Enter", __FUNCTION__);
	int32_t ret = 0;
	VOICE_RPC_INIT arg;
	//StartRecordTask();
	g_instance = rtos_mem_malloc(sizeof(VOICE_RPC_INSTANCE));
	memset((void *)g_instance, 0, sizeof(VOICE_RPC_INSTANCE));
	g_record_data = rtos_mem_malloc(RECORD_SIZE);
	memset((void *)g_record_data, 0, RECORD_SIZE);
	p_VOICE_RPC_ToSystem_Error_0_svc = OnError;
	p_VOICE_RPC_ToSystem_Recognizer_Callback_0_svc = OnRecognizer;
	g_mic_ringbuffer = RingBuffer_Create(NULL, RECORD_SIZE * 8, SHARED_RINGBUFF, 1);
	g_record_state = 0;
	arg.mic_data_addr = (uint32_t)g_mic_ringbuffer;
	arg.mic_data_length = sizeof(RingBuffer);
	arg.res_data_addr = (uint32_t)g_res_data;
	arg.res_data_length = 0;//total_len;
	DCache_Clean((uint32_t)g_mic_ringbuffer, sizeof(RingBuffer));
	DCache_Clean((uint32_t)g_res_data, 0);
	clnt = RPC_PrepareCLNT(VOICE_RPC_MODE, VOICE_SYSTEM, 0);
	VOICE_RPCRES_INIT *res = VOICE_RPC_ToAgent_Init_0(&arg, &clnt);
	g_vad_ringbuffer = (RingBuffer *)res->rpc_result;
	DCache_Invalidate((uint32_t)g_vad_ringbuffer, sizeof(RingBuffer));
	dbg_ver_info_t *ver_info = (dbg_ver_info_t *)res->instance.streaming_ins;
	LOGD("%s",ver_info->productor_version);
	g_wkmsg_ringbuffer = (RingBuffer *)res->instance.recognizer_ins;
	DCache_Invalidate((uint32_t)g_wkmsg_ringbuffer, sizeof(RingBuffer));
	rtos_mem_free(res);
	LOGD("Agent_Init End");
	return ret;
}

int32_t Start(void)
{
	LOGD("%s Enter", __FUNCTION__);
	int32_t ret = 0;
	g_thread_running = true;
	g_mic_data_cnt = 0;
	if (rtos_task_create(NULL, ((const char *)"RecordLoop"), RecordLoop, NULL, 1024 * 4, 5) != SUCCESS) {
		LOGE("\n\r%s rtos_task_create(RecordLoop) failed", __FUNCTION__);
	}
#if DUMP_DATA_ENABLE
	uart_audio_dump_init();
#else
	if (rtos_task_create(NULL, ((const char *)"OnlineTask"), OnlineTask, NULL, 1024 * 4, 5) != SUCCESS) {
		LOGE("\n\r%s rtos_task_create(OnlineTask) failed", __FUNCTION__);
	}
#endif
	return ret;
}

int32_t Stop(void)
{
	LOGV("%s Enter", __FUNCTION__);
	if (audio_record) {
		LOGD("RTAudioRecord_Stop start");
		g_record_state = -1;
		LOGD("RTAudioRecord_Stop start real");
		RTAudioRecord_Stop(audio_record);
		LOGD("RTAudioRecord_Stop End");
	}
	return 0;
}

int32_t Pause(void)
{
	LOGV("%s Enter", __FUNCTION__);
	if (audio_record) {
		g_record_state = 1;
	}
	return 0;
}

int32_t Resume(void)
{
	LOGV("%s Enter", __FUNCTION__);
	if (audio_record) {
		g_record_state = 0;
	}
	return 0;
}


int32_t Release(void)
{
	LOGV("%s Enter", __FUNCTION__);
	if (audio_record) {
		RTAudioRecord_Destroy(audio_record);
		audio_record = NULL;
	}
	CLNT_STRUCT clnt = RPC_PrepareCLNT(VOICE_RPC_MODE, VOICE_SYSTEM, 0);
	HRESULT *res = VOICE_RPC_ToAgent_Release_0(g_instance, &clnt);
	LOGV("Release:VOICE_RPC_ToAgent_Release_0");
	LOGD("Release res=%ld", *res);
	rtos_mem_free(res);

	rtos_mem_free(g_instance);
	rtos_mem_free(g_record_data);
	rtos_mem_free(g_res_data);
	RingBuffer_Destroy(g_mic_ringbuffer);

	return 0;
}
