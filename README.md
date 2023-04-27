# Text-To-Speech Adapter Code Example

Rutronik Development Kit Programmable System-on-Chip CY8C6245AZI-S3D72 "Text-To-Speech " Code Example. 

This example demonstrates the Rutronik's Text-To-Speech Arduino adapter control.

 <img src="images/tts_photo.jpg" style="zoom:40%;" />

## Requirements

- [ModusToolbox® software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/) v3.0

### Using the code example with a ModusToolbox IDE:

1. Import the project: **File** > **Import...** > **General** > **Existing Projects into Workspace** > **Next**.
2. Select the directory where **"RDK3_TextToSpeech_Demo"** resides and click  **Finish**.
3. Update libraries using  a **"Library Manager"** tool.
4. Select and build the project **Project ** > **Build Project**.

## The Provisioning of the RDK3

The CYB06447BZI-BLD53 device must be provisioned with **keys** and **policies** before being programmed. If the kit is already provisioned, copy-paste the keys and policy folder to the application folder. If the unsigned or not properly signed image will be written to the RDK3 PSoC™ 64 – the microcontroller will not boot. 

The “[Secure Policy Configurator](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Secure_Policy_Configurator_1.30_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c8386267f0183a960762a5977)” tool is used for the provisioning of the new RDK3, please refer to the “ModusToolbox™ Secure Policy Configurator user guide”. 

The CYB06447BZI-BLD53 MCU must be powered from a 2.5V power source to be able to complete the provisioning. The RDK3 has an SMPS [Switching Mode Power Supply] which can be easily adjusted to provide 3.3V or 2.5V to the MCU by switching the slide-switch “SW1” on the bottom side of the board. 

<img src="images/voltage_switch.jpg" style="zoom:50%;" />

### Operation

This code example enables developer to play the voice messages [phrases] that are programmed into the Text-To-Speech board's serial flash memory. If the  button "USER BTN1" is pressed - next phrase will be played. The number of the phrase that is being played by the Text-To-Speech adapter will be printed out through the KitProg3 UART on a terminal window.

<img src="images/tts_output.png" style="zoom:100%;" />

If you are not using the voice files provided with this code example, please do not forget to modify the key-code provided by Epson in isc_msgs.c file. Please also note that it has to be entered in reverse sequence. For example, the key code **0x1AAA1F85** has to be provided as it is shown below:

```
unsigned char aucIscTestReq[HEADER_LEN + LEN_ISC_TEST_REQ] = {

	0x00, ID_START,

	LEN_ISC_TEST_REQ, 0x00,

	_GET_LOW_BYTE(ID_ISC_TEST_REQ), _GET_HIGH_BYTE(ID_ISC_TEST_REQ),

	0x00, 0x00,				// Check-Sum 1:Enable 0:Disable

	0x01, 0x00,				// 1:Full duplex, 0:Half duplex

	0x85, 0x1F, 0xAA, 0x1A, // key-code
};
```

The audio volume level is hardcoded in this code example. Please modify the isc_msgs.h file to adjust a volume:

`#define	INIT_AUDIO_VOLUME		0x38	// Audio volume (0x00 - 0x43)`

### Debugging

If you successfully have imported the example, the debug configurations are already prepared to use with a the KitProg3, MiniProg4, or J-link. Open the ModusToolbox perspective and find the Quick Panel. Click on the desired debug launch configuration and wait until the programming completes and the debugging starts.

<img src="images/tts_debug.png" style="zoom:100%;" />

## Legal Disclaimer

The evaluation board including the software is for testing purposes only and, because it has limited functions and limited resilience, is not suitable for permanent use under real conditions. If the evaluation board is nevertheless used under real conditions, this is done at one’s responsibility; any liability of Rutronik is insofar excluded. 

<img src="images/rutronik_origin_kaunas.png" style="zoom:50%;" />



