Before trying to get an Edge Impulse model working in Arduino, I suggest running the official Edge Impulse Spresense Runner code found [here](https://github.com/edgeimpulse/firmware-sony-spresense). This will verifyyour Spresense if configured properly. If it doesn't work there, it ain't gonna work in the Arduino IDE.

**Using Spresense with an Edge Impulse Runner via C Code**

	Download a ZIP of this code:
		`https://github.com/edgeimpulse/firmware-sony-spresense`
		Extract to folder `".../firmware-sony-spresense-main/"`

	Install EdgeImpulse CLI
		`https://docs.edgeimpulse.com/docs/edge-impulse-cli/cli-installation#installation-linux-ubuntu-and-raspbian-os`
		curl -sL https://deb.nodesource.com/setup_14.x | sudo -E bash -
		sudo apt-get install -y nodejs
		node -version
		npm config get prefix
		npm install -g edge-impulse-cli
	
	If you are running Ubuntu 22.04
		HOWTO-Check Ubuntu Version:
			`$ lsb_release -a`
				`No LSB modules are available.`
				`Distributor ID:	Ubuntu`
				`Description:	Ubuntu 22.04.1 LTS`
				`Release:	22.04`
				`Codename:	jammy`

	Then try this to get Node.js installed:
		`https://www.digitalocean.com/community/tutorials/how-to-install-node-js-on-ubuntu-22-04`
		sudo apt update
		sudo apt install nodejs
		node -v
		sudo apt install npm

	Extract your EI Arduino Library
		Copy folders:
			`edge-impulse-sdk/`
			`model-parametsrs/`
			`tflite-model`
		To:
			when asked "merge" the folders
			`firmware-sony-spresense-main/edge_impulse/`
			
	Build the `"firmware-sony-spresense"` application
		`cd firmware-sony-spresense-main`
		`make -j`

	Flash the Sony Spresence	
		`make flash`
		
	/home/<ubuntu_username>/.npm-global/bin/edge-impulse-run-impulse

**Additional Help**

	Spresence Overview
		https://developer.sony.com/develop/spresense/specifications
		https://developer.sony.com/develop/spresense/docs/introduction_en.html
		
	Using Spresense with EdgeImpulse via Daemon
		https://docs.edgeimpulse.com/docs/development-platforms/officially-supported-mcu-targets/sony-spresense
	
	Using Spresense with EdgeImpulse via Impulse Runner
		https://docs.edgeimpulse.com/docs/edge-impulse-cli/cli-run-impulse

	For Audio Models:
		Spresense: Hooking up a Mic
		https://developer.sony.com/develop/spresense/docs/hw_docs_en.html#_electret_microphones
		Install DSP Files:
		https://developer.sony.com/develop/spresense/docs/arduino_tutorials_en.html#_install_dsp_files
			Get Files here:
				https://github.com/sonydevworld/spresense-arduino-compatible/raw/master/Arduino15/packages/SPRESENSE/hardware/spresense/1.0.0/libraries/Audio/examples/dsp_installer/mp3_dec_installer/MP3DEC
				https://github.com/sonydevworld/spresense-arduino-compatible/raw/master/Arduino15/packages/SPRESENSE/hardware/spresense/1.0.0/libraries/Audio/examples/dsp_installer/mp3_enc_installer/MP3ENC
				https://github.com/sonydevworld/spresense-arduino-compatible/raw/master/Arduino15/packages/SPRESENSE/hardware/spresense/1.0.0/libraries/Audio/examples/dsp_installer/wav_dec_installer/WAVDEC
				https://github.com/sonydevworld/spresense-arduino-compatible/raw/master/Arduino15/packages/SPRESENSE/hardware/spresense/1.0.0/libraries/Audio/examples/dsp_installer/src_installer/SRC
		SD Card should look like		
			|--BIN/
			|--|--MP3DEC
			|--|--MP3ENC
			|--|--SRC
			|--|--WAVEDEC
		
	Spresence SubCores do not support Audio:
		https://github.com/sonydevworld/spresense-arduino-compatible/blob/master/Arduino15/packages/SPRESENSE/hardware/spresense/1.0.0/libraries/Audio/Audio.h
		#ifdef SUBCORE
		#error "Audio library is NOT supported by SubCore."
		#endif

	For Vision Models
		Spresense: Hooking up a Camera
		https://developer.sony.com/develop/spresense/docs/introduction_en.html#_camera_board

	Spresense and Ardunio Examples
		https://developer.sony.com/develop/spresense/docs/arduino_tutorials_en.html

	Spresense Arduino Library: Multi-Processor (MP) Class
		https://developer.sony.com/develop/spresense/developer-tools/api-reference/api-references-arduino/classMPClass.html

	VS Code
	https://developer.sony.com/develop/spresense/docs/sdk_set_up_ide_en.html#_creating_an_application_project
