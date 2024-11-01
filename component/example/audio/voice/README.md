
### example_voice ###

Date: 2023-12-15

Table of Contents

 - Description

 - Setup Guide

 - Parameter Setting and Configuration

 - Note

 - Result description

 - Supported List

   #### Description

   ​        This example is based on IPC-2.0 API and RPC API, a simple example of transcation between two cores is provided.

   #### Setup Guide

   ​     For Changes how example is compiled:
   ​    	GCC:use CMD "make all EXAMPLE=voice" to compile voice expample.

#### Parameter Setting and Configuration

For example, communication between DSP and KR4:

1. Audio Framework and RPC Framework enable:
	For GCC environmnet, type command "make menuconfig" in project/realtek_amebaLite_va0_example/GCC-RELEASE/auto_build:
	Choose:
		< MENUCONFIG FOR KR4 CONFIG  --->
			< CONFIG IPC Message Queue  --->
				[*] Enable IPC Message Queue
				[*] Enable RPC

2. Define DSP the macro: RPC_KR4_DSP in DSP build environmnet.
		In explorer: open Build Properties for project_dsp, choose Symblos item, define RPC_KR4_DSP, define CONFIG_RPC_EN 1

3. build the project and download firmware to DEV board.

Communication between DSP and KM4:

1. Audio Framework and RPC Framework enable:
	For GCC environmnet, type command "make menuconfig" in project/realtek_amebaLite_va0_example/GCC-RELEASE/auto_build:
	Choose:
		< MENUCONFIG FOR KM4 CONFIG  --->
			< CONFIG IPC Message Queue  --->
				[*] Enable IPC Message Queue
				[*] Enable RPC

2. Define DSP the macro: RPC_KM4_DSP in DSP build environmnet.
	In explorer: open Build Properties for project_dsp, choose Symblos item, define RPC_KM4_DSP, define CONFIG_RPC_EN 1

3. build the project and download firmware to DEV board.

#### Result description

This example is an voice example thread is started automatically when booting, and need dsp boot config enable,
and need modify heap size for support Audio heap, and DSP supproted voice version.


#### Supported List

[Supported List]
        Supported IC :
                RTL8276E
