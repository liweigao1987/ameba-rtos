#include "pc_recorder.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "rtw_wifi_constants.h"
#include "audio/audio_service.h"

#include "media/rtplayer.h"
#include <dlist.h>

#define RAW_DUMP			1

#define PR_UART_TX			PA_25
#define PR_UART_RX			PA_24
#define PR_UART_USE_DMA_TX	1

#define PR_BAUDRATE			1500000
#define PC_BUFLEN			1024
#define RECORD_PAGE_NUM		2
#define RECORD_PAGE_SIZE	1024
#define SERVER_PORT			80

#define MAX_URL_LEN			64


static int start_cnt = 0;
static int end_cnt = 0;

unsigned char pc_buf[PC_BUFLEN];
int pc_datasize = 0;
rtos_sema_t pr_rx_sema;
rtos_sema_t pr_dma_tx_sema;
rtos_mutex_t pr_tx_mutex;

rtos_task_t playback_task;
serial_t pr_sobj;
pr_adapter_t pr_adapter = {0};

const struct pr_msg msg_type[] = {
	{PR_MSG_ACK, 		"ack"		},
	{PR_MSG_ERROR, 	    "error"		},
	{PR_MSG_CONFIG, 	"config"		},
	{PR_MSG_START, 		"start"		},
	{PR_MSG_STOP, 		"stop"		},
	{PR_MSG_DATA,		"data"},
	{PR_MSG_QUERY,      "query"},
	{PR_MSG_VOLUME,		"volume"},
};

volatile bool player_is_running = false;

struct list_head pr_url_list;


struct url_item {
	char item_str[MAX_URL_LEN];
	struct list_head node;
};

enum PlayingStatus {
	IDLE,
	PLAYING,
	PAUSED,
	PLAYING_COMPLETED,
	REWIND_COMPLETE,
	STOPPED,
	RESET,
};

int g_pc_playing_status;
struct RTPlayer *g_pc_player = NULL;


void pc_recorder_start(msg_attrib_t *pattrib);
void pc_playback_task(void *param);
void uart_audio_dump_data(char *buffer);
void uart_audio_dump_init(void);


void OnStateChangedPC(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int state)
{
	printf("OnStateChanged(%p %p), (%d)\n", listener, player, state);

	switch (state) {
	case RTPLAYER_PREPARED: { //entered for async prepare
		break;
	}

	case RTPLAYER_PLAYBACK_COMPLETE: { //eos received, then stop
		g_pc_playing_status = PLAYING_COMPLETED;
		break;
	}

	case RTPLAYER_STOPPED: { //stop received, then reset
		printf("start reset\n");
		g_pc_playing_status = STOPPED;
		break;
	}

	case RTPLAYER_PAUSED: { //pause received when do pause or start rewinding
		printf("paused\n");
		g_pc_playing_status = PAUSED;
		break;
	}

	case RTPLAYER_REWIND_COMPLETE: { //rewind done received, then start
		printf("rewind complete\n");
		g_pc_playing_status = REWIND_COMPLETE;
		break;
	}
	}
}

void OnInfoPC(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int info, int extra)
{
	(void) listener;
	(void) player;
	(void) extra;
	//printf("OnInfo (%p %p), (%d, %d)\n", listener, player, info, extra);

	switch (info) {
	case RTPLAYER_INFO_BUFFERING_START: {
		printf("RTPLAYER_INFO_BUFFERING_START\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_END: {
		printf("RTPLAYER_INFO_BUFFERING_END\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_INFO_UPDATE: {
		//printf("RTPLAYER_INFO_BUFFERING_INFO_UPDATE %d\n", extra);
		break;
	}

	case RTPLAYER_INFO_NOT_REWINDABLE: {
		printf("RTPLAYER_INFO_NOT_REWINDABLE\n");
		break;
	}
	}
}

void OnErrorPC(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int error, int extra)
{
	printf("OnError (%p %p), (%d, %d)\n", player, listener, error, extra);
}

void pc_StartPlay(struct RTPlayer *player, const char *url)
{
	if (player == NULL) {
		printf("start play fail, player is NULL!\n");
		return;
	}

	printf("start to play: %s\n", url);
	int32_t ret = 0;

	g_pc_playing_status = PLAYING;

	printf("SetSource\n");
	ret = RTPlayer_SetSource(player, url);
	if (ret) {
		printf("SetDataSource fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Prepare\n");
	ret = RTPlayer_Prepare(player);
	if (ret) {
		printf("prepare  fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Start\n");
	ret = RTPlayer_Start(player);
	if (ret) {
		printf("start  fail:error=%d\n", (int)ret);
		return ;
	}

	int64_t duration = 0;
	RTPlayer_GetDuration(player, &duration);
	printf("duration is %lldms\n", duration);

	while (g_pc_playing_status == PLAYING) {
		rtos_time_delay_ms(1000);
	}

	if (g_pc_playing_status == PLAYING_COMPLETED) {
		printf("play complete, now stop.\n");
		RTPlayer_Stop(player);
	}

	while (g_pc_playing_status == PLAYING_COMPLETED) {
		rtos_time_delay_ms(1000);
	}

	if (g_pc_playing_status == STOPPED) {
		printf("play stopped, now reset.\n");
		RTPlayer_Reset(player);
	}

	printf("play %s done!!!!\n", url);
}


int pc_player_test(const char *url)
{
	struct RTPlayerCallback *callback = (struct RTPlayerCallback *)rtos_mem_malloc(sizeof(struct RTPlayerCallback));
	if (!callback) {
		printf("Calloc RTPlayerCallback fail.\n");
		return -1;
	}

	callback->OnRTPlayerStateChanged = OnStateChangedPC;
	callback->OnRTPlayerInfo = OnInfoPC;
	callback->OnRTPlayerError = OnErrorPC;

	g_pc_player = RTPlayer_Create();

	RTPlayer_SetCallback(g_pc_player, callback);

	pc_StartPlay(g_pc_player, url);

	rtos_mem_free(callback);
	RTPlayer_Destory(g_pc_player);
	g_pc_player = NULL;

	rtos_time_delay_ms(1000);

	printf("exit\n");
	return 0;
}




void pr_uart_send_string(char *pstr, int len)
{
#if PR_UART_USE_DMA_TX
	int32_t ret = 0;
	rtos_sema_take(pr_dma_tx_sema, RTOS_MAX_TIMEOUT);

	ret = serial_send_stream_dma(&pr_sobj, pstr, len);
	if (ret != 0) {
		printf("%s Error(%d)\n", __FUNCTION__, (int)ret);
	}
#else
	while (len) {
		serial_putc(&pr_sobj, *pstr);
		pstr++;
		len--;
	}
#endif
}

void pr_uart_irq(uint32_t id, SerialIrq event)
{
	serial_t    *sobj = (serial_t *)id;
	unsigned char rc = 0;

	//UART_TypeDef *UARTx = UART_DEV_TABLE[sobj->uart_idx].UARTx;

	if (event == RxIrq) {
		rc = serial_getc(sobj);
		if (rc == '{') {
			start_cnt++;
		} else if (rc == '}') {
			end_cnt++;
		}

		pc_buf[pc_datasize++] = rc;

		if ((start_cnt != 0) && start_cnt == end_cnt) {
			start_cnt = 0;
			end_cnt = 0;
			rtos_sema_give(pr_rx_sema);
			return;
		}
	}
}

static void uart_dma_tx_done(uint32_t id)
{
	(void) id;
	rtos_sema_give(pr_dma_tx_sema);
	//printf("%s\n", __func__);
}

void pr_uart_init_mbed(void)
{
	pr_sobj.uart_idx = 0;
	serial_init(&pr_sobj, PR_UART_TX, PR_UART_RX);
	serial_baud(&pr_sobj, PR_BAUDRATE);
	serial_format(&pr_sobj, 8, ParityNone, 1);

	serial_rx_fifo_level(&pr_sobj, FifoLvHalf);
	serial_set_flow_control(&pr_sobj, FlowControlNone, 0, 0);
	serial_irq_handler(&pr_sobj, pr_uart_irq, (uint32_t)&pr_sobj);
	serial_irq_set(&pr_sobj, RxIrq, ENABLE);
#if PR_UART_USE_DMA_TX
	serial_send_comp_handler(&pr_sobj, (void *)uart_dma_tx_done, (uint32_t) &pr_sobj);
#endif
}

int pr_audiorecord_query(void)
{
	cJSON *msg_obj;
	char *msg_js = NULL;

	if ((msg_obj = cJSON_CreateObject()) == NULL) {
		return -1;
	}


	cJSON_AddStringToObject(msg_obj, "type", "query");
	cJSON_AddStringToObject(msg_obj, "version", PR_VERSION);

	switch (pr_adapter.record_status) {
	case RECORD_IDLE:
		cJSON_AddStringToObject(msg_obj, "status", "idle");
		break;
	case RECORD_BUSY:
		cJSON_AddStringToObject(msg_obj, "status", "busy");
		break;
	}


	msg_js = cJSON_Print(msg_obj);
	cJSON_Delete(msg_obj);

	printf("[PC RECORDER INFO] %s, %s\n", __func__, msg_js);
	rtos_mutex_take(pr_tx_mutex, MUTEX_WAIT_TIMEOUT);
	pr_uart_send_string(msg_js, strlen(msg_js));
	rtos_mutex_give(pr_tx_mutex);
	rtos_mem_free(msg_js);

	return 0;
}

int pc_msg_audio_data_binary(int seq, char *data, int data_len)
{
	unsigned char buf[64];
	unsigned char checksum = 0;
	unsigned int offset = 0;

	for (int i = 0; i < data_len; i++) {
		checksum ^= data[i];
	}

	memcpy(buf, "data", strlen("data"));
	offset += strlen("data");
	*(int *)(buf + offset) = seq;
	offset += 4;
	*(short *)(buf + offset) = data_len;
	offset += 2;
	*(unsigned char *)(buf + offset) = checksum;
	offset += 1;
	rtos_mutex_take(pr_tx_mutex, MUTEX_WAIT_TIMEOUT);
	pr_uart_send_string((char *)buf, offset);
	pr_uart_send_string(data, data_len);
	rtos_mutex_give(pr_tx_mutex);

	return 0;
}

int pc_msg_response_ack(int opt)
{
	cJSON *msg_obj;
	char *msg_js = NULL;

	if ((msg_obj = cJSON_CreateObject()) == NULL) {
		return -1;
	}

	if (opt >= 0) {
		cJSON_AddStringToObject(msg_obj, "type", "ack");
	} else {
		cJSON_AddStringToObject(msg_obj, "type", "error");
	}

	msg_js = cJSON_Print(msg_obj);
	cJSON_Delete(msg_obj);

	printf("[PC RECORDER INFO] %s, %s\n", __func__, msg_js);
	rtos_mutex_take(pr_tx_mutex, MUTEX_WAIT_TIMEOUT);
	pr_uart_send_string(msg_js, strlen(msg_js));
	printf("[PC RECORDER INFO] %s, send ack done(%d)\n", __func__, strlen(msg_js));
	rtos_mutex_give(pr_tx_mutex);

	rtos_mem_free(msg_js);

	return 0;
}

int pc_msg_audio_chmap(msg_attrib_t *pattrib, int ch, int index)
{
	(void)index;
	int amic, dmic, ref;
	unsigned char tempbuf[64];

	amic = ch & PR_AMIC_NONE;
	dmic = (ch >> 8) & PR_DMIC_NONE;
	ref = ch & PR_MIC_REF;

	if (amic != PR_AMIC_NONE) {

		memset(tempbuf, 0x00, 64);
		sprintf((char *)tempbuf, "ch%d_sel_amic=%d;", pattrib->adcindex, amic);

		memcpy(pattrib->chmap + pattrib->offset, tempbuf, strlen((char *)tempbuf));

		if (ref) {
			pattrib->refch = pattrib->adcindex;
		}
		pattrib->offset += strlen((char *)tempbuf);
		pattrib->adcindex++;
	} else if (dmic != PR_DMIC_NONE) {
		memset(tempbuf, 0x00, 64);
		sprintf((char *)tempbuf, "ch%d_sel_dmic=%d;", pattrib->adcindex, dmic);

		memcpy(pattrib->chmap + pattrib->offset, tempbuf, strlen((char *)tempbuf));

		if (ref) {
			pattrib->refch = pattrib->adcindex;
		}
		pattrib->offset += strlen((char *)tempbuf);
		pattrib->adcindex++;
	}


	return 0;
}

void pc_msg_process(msg_attrib_t *pattrib)
{
	unsigned char tempbuf[PC_BUFLEN];
	cJSON *root, *typeobj;
	int opt = -1;
	//int offset = 0;
	cJSON *mode, *play, *curl;
	memset(tempbuf, 0x00, PC_BUFLEN);
	memcpy(tempbuf, pc_buf, pc_datasize);
	pc_datasize = 0;

	printf("[PC RECORDER INFO] %s, msg: %s\n", __func__, (char *)tempbuf);

	if ((root = cJSON_Parse((char *)tempbuf)) != NULL) {
		if ((typeobj = cJSON_GetObjectItem(root, "type")) != NULL) {
			printf("[PC RECORDER INFO] type: %s\n", (char *)typeobj->valuestring);
			for (unsigned int i = 0; i < sizeof(msg_type) / sizeof(struct pr_msg); i++) {
				if (!strcmp(typeobj->valuestring, msg_type[i].item_str)) {
					pattrib->type = msg_type[i].item;
					break;
				}
			}
		}
	}

	switch (pattrib->type) {
	case PR_MSG_CONFIG:
	/* parse config information */


		mode = cJSON_GetObjectItem(root, "mode");
		if (mode && mode->valueint == 1) {
			play = cJSON_GetObjectItem(root, "play");
			curl = cJSON_GetObjectItem(play, "url");

			pattrib->url = rtos_mem_malloc(strlen(curl->valuestring) + 1);
			memset(pattrib->url, 0x00, strlen(curl->valuestring) + 1);

			strncpy((char *)pattrib->url, curl->valuestring, strlen(curl->valuestring));
			if (player_is_running) {
				rtos_time_delay_ms(200);
				printf("[PC RECORDER INFO] %s, Player is running\n", __func__);
			} else {
				pr_adapter.record_stop = 0; // add for play before start record
				if (rtos_task_create(&playback_task, ((const char *)"playback_task"), pc_playback_task,
									 pattrib, 8192 * 4, 2) != SUCCESS) {
					printf("\n\r%s rtos_task_create(playback_task) failed", __FUNCTION__);
				}
			}
		}
		pc_msg_response_ack(0);
		break;
	case PR_MSG_START:
		pr_adapter.record_status = RECORD_BUSY;
		pc_msg_response_ack(0);
		break;
	case PR_MSG_STOP:
		pr_adapter.record_status = RECORD_IDLE;
		pr_adapter.record_stop = 1;
		if(g_pc_player)
			RTPlayer_Stop(g_pc_player);
		pc_msg_response_ack(0);
		break;
	case PR_MSG_QUERY:
		opt = pr_audiorecord_query();
		if (opt != 0) {
			pc_msg_response_ack(opt);
		}
		break;
	case PR_MSG_VOLUME: {
		pc_msg_response_ack(0);
	}
	break;
	default:
		printf("[PC RECORDER INFO] %s, unsupport type =%d\n", __func__, pattrib->type);
		pc_msg_response_ack(opt);
	}

	cJSON_Delete(root);
}

void pc_rx_task(void *param)
{
	(void)param;

	msg_attrib_t pattrib = {0};
	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);

	memset(pc_buf, 0, PC_BUFLEN);

	while (1) {
		rtos_sema_take(pr_rx_sema, RTOS_MAX_TIMEOUT);
		pc_msg_process(&pattrib);
	}

	rtos_task_delete(NULL);
}

void uart_audio_dump_init(void)
{
#if PR_UART_USE_DMA_TX
	rtos_sema_create(&pr_dma_tx_sema, 1, RTOS_SEMA_MAX_COUNT);
#endif
	rtos_sema_create(&pr_rx_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_mutex_create(&pr_tx_mutex);

	pr_uart_init_mbed();
#if 1
	RTAudioService_Init();
	RTAudioControl_SetAmplifierEnPin(PA_12);
	RTAudioControl_SetHardwareVolume(0.6, 0.6);
#endif

	INIT_LIST_HEAD(&pr_url_list);
	pr_adapter.record_stop = 0;
	pr_adapter.record_status = RECORD_IDLE;
	pr_adapter.rx_cnt = 0;
	pr_adapter.tx_cnt = 0;

	if (rtos_task_create(NULL, (char const *)"pc_rx_task", pc_rx_task, NULL, 2048 * 4, 1) != SUCCESS) {
		printf("\n\r[%s] Create pc_rx_task failed", __FUNCTION__);
	}
}

void uart_audio_dump_data(char *buffer)
{
	pr_adapter.rx_cnt++;
	pc_msg_audio_data_binary(pr_adapter.rx_cnt, buffer, RECORD_PAGE_SIZE);
}

int pc_parse_url(char *url, char *host, char *resource, char *format)
{
	if (url) {
		char *http = NULL, *pos = NULL;

		http = strstr(url, "http://");
		if (http) { // remove http
			url += strlen("http://");
		}

		pos = strstr(url, "/");
		if (pos) {
			memcpy(host, url, (pos - url));
			url = pos;
		}
		printf("[PC RECORDER INFO] server: %s\n\r", host);

		pos = strstr(url, "/");
		if (pos) {
			resource[0] = '/';
			memcpy(resource + 1, pos + 1, strlen(pos + 1));
		}
		printf("[PC RECORDER INFO] resource: %s\n\r", resource);

		pos = strstr(resource, ".");
		if (pos) {
			memcpy(format, pos + 1, strlen(pos + 1));
		}
		printf("[PC RECORDER INFO] format: %s\n\r", format);
		return 0;
	}
	return -1;
}
void pc_playback_task(void *param)
{
	player_is_running = true;
#if 1
	msg_attrib_t *pattrib = (msg_attrib_t *)param;
	int server_fd = -1;
	struct sockaddr_in server_addr;
	struct hostent *server_host;
	char host[64];
	char resource[128];
	char format[16];

	memset(host, 0, 64);
	memset(resource, 0, 128);
	memset(format, 0, 16);

	pc_parse_url((char *)pattrib->url, host, resource, format);
replay:	pc_player_test((const char *)pattrib->url);

		if (!pr_adapter.record_stop) {
			goto replay;
		}

exit:
	rtos_mem_free(pattrib->url);

#else
	(void)param;

	struct RTAudioTrack *audio_track;
	int track_buf_size = 4096;
	unsigned int channels = 2;
	unsigned int rate = 48000;
	printf("[PC RECORDER INFO] play sample channels:%d, rate:%d\n", channels, rate);

	audio_track = RTAudioTrack_Create();
	if (!audio_track) {
		printf("[PC RECORDER INFO] new RTAudioTrack failed\n");
		return;
	}
	uint32_t format = RTAUDIO_FORMAT_PCM_16_BIT;
	track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, rate, format, channels) * 4;
	RTAudioTrackConfig	track_config;
	track_config.category_type = RTAUDIO_CATEGORY_MEDIA;
	track_config.sample_rate = rate;
	track_config.format = format;
	track_config.channel_count = channels;
	track_config.buffer_bytes = track_buf_size;
	RTAudioTrack_Init(audio_track, &track_config, RTAUDIO_OUTPUT_FLAG_NONE);

	RTAudioTrack_Start(audio_track);

	ssize_t size = track_buf_size / 4;

	printf("[PC RECORDER INFO] audio track get size = %d\n", size);

	if (rate == 48000) {
		size = 96 * 2;
		while (1) {
			RTAudioTrack_Write(audio_track, (unsigned char *)sine_48000, size, true);
			if (pr_adapter.record_stop) {
				break;
			}
		}
	}

	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

#endif
	printf("[PC RECORDER INFO] %s, exit\n", __func__);
	player_is_running = false;
	rtos_task_delete(playback_task);
}
