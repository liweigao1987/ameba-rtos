

#include "ameba_soc.h"
#include "rpc_api.h"

#include "voice_manager.h"


#define DEFAULT_STACK_SIZE 4096
#if defined(CONFIG_IMQ_EN) && CONFIG_IMQ_EN
extern void ipc_app_init(void);
#endif

void VoiceTestTask(void *param)
{
	(void)param;
	LOGD("VoiceTestTask\n");
	rtos_time_delay_ms(2000);
	int32_t res = Init();
	LOGD("Init res = %ld\n", res);
	res = Start();
	while (1) {
		rtos_time_delay_ms(1000 * 120);
	}
	Stop();
	rtos_time_delay_ms(1000 * 2);
	Release();
	rtos_time_delay_ms(5000);
	rtos_task_delete(NULL);
}

void TestVoice(void)
{
	LOGV("%s Enter\n", __FUNCTION__);
	if (rtos_task_create(NULL, ((const char *)"VoiceTestTask"), VoiceTestTask, NULL, 1024 * 4, 1) != SUCCESS) {
		LOGE("\n\r%s rtos_task_create(VoiceTestTask) failed", __FUNCTION__);
	}
}

void example_voice(void)
{
	LOGD("example_voice\n");
	ipc_app_init();
	LOGD("TestVoice\n");
	TestVoice();
}



