#include "badge.h"
#include "stdint.h"
#include "gui.h"
#include "menus.h"
#include <RFM69.h>
#include <Keyboard.h>
#include <KeyStore.h>
#include <tim.h>
#include <usart.h>
#include "menus/irmenu.h"
#include "menus/MessageState.h"

char sendingBuf[64] = { '\0' };
char receivingBuf[64] = { '\0' };

#define ONE_TIME 0

int state = 0;
int tmp = 0;

//RH_RF69 Radio(RFM69_SPI_NSS_Pin,RFM69_Interrupt_DIO0_Pin);
QKeyboard KB(QKeyboard::PinConfig(KEYBOARD_Y1_GPIO_Port, KEYBOARD_Y1_Pin),
		QKeyboard::PinConfig(KEYBOARD_Y2_GPIO_Port, KEYBOARD_Y2_Pin),
		QKeyboard::PinConfig(KEYBOARD_Y3_GPIO_Port, KEYBOARD_Y3_Pin),
		QKeyboard::PinConfig(KEYBOARD_X1_GPIO_Port, KEYBOARD_X1_Pin),
		QKeyboard::PinConfig(KEYBOARD_X2_GPIO_Port, KEYBOARD_X2_Pin),
		QKeyboard::PinConfig(KEYBOARD_X3_GPIO_Port, KEYBOARD_X3_Pin),
		QKeyboard::PinConfig(KEYBOARD_X4_GPIO_Port, KEYBOARD_X4_Pin));

RFM69 Radio(RFM69_SPI_NSS_Pin, RFM69_Interrupt_DIO0_Pin, true);

static const uint16_t SETTING_SECTOR = 57; //0x800e400
static const uint16_t FIRST_CONTACT_SECTOR = SETTING_SECTOR + 1; //0x800e800
static const uint16_t NUM_CONTACT_SECTOR = 64 - FIRST_CONTACT_SECTOR;
static const uint32_t MY_INFO_ADDRESS = 0x800FFD4;

ContactStore MyContacts(SETTING_SECTOR, FIRST_CONTACT_SECTOR, NUM_CONTACT_SECTOR, MY_INFO_ADDRESS); //0x2710); //0x4E00);

ContactStore &getContactStore() {
	return MyContacts;
}

RFM69 &getRadio() {
	return Radio;
}

void delay(uint32_t time) {
	HAL_Delay(time);
}

const char *ErrorType::getMessage() {
	return "ErrorType:  TODO";
}

ErrorType::ErrorType(const ErrorType &r) {
	this->Error = r.Error;
}

void initFlash() {
	/*
	 HAL_FLASH_Unlock();
	 FLASH_EraseInitTypeDef EraseInitStruct;
	 EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	 EraseInitStruct.Banks = FLASH_BANK_1;
	 EraseInitStruct.PageAddress = FLASH_BASE + (57 * FLASH_PAGE_SIZE);
	 EraseInitStruct.NbPages = 1;
	 uint32_t SectorError = 0;

	 //if(FLASH_PageErase(FLASH_BASE + ( 57 * FLASH_PAGE_SIZE))==HAL_OK) {
	 if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) == HAL_OK) {
	 HAL_FLASH_Lock();
	 HAL_FLASH_Unlock();
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x800e400, 0xDCDC);
	 HAL_FLASH_Lock();
	 uint16_t v = *((uint16_t *) 0x800e400);
	 char b[10];
	 sprintf(&b[0], "%d", v);

	 HAL_FLASH_Unlock();
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x800e400, 0x0);
	 HAL_FLASH_Lock();
	 v = *((uint16_t *) 0x800e400);
	 sprintf(&b[0], "%d", v);
	 HAL_FLASH_Unlock();
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x800e400, 0xDCDC);
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x800e402, 0xDCDC);
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x800e412, 0xDCDC);
	 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x800e4A2, 0xDCDC);
	 }

	 HAL_FLASH_Lock();
	 */
#if ONE_TIME==1
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

	HAL_StatusTypeDef s = HAL_FLASH_Unlock();
	//assert(s==HAL_OK);
	uint16_t loc = 0;
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, START_STORAGE_LOCATION, 0xDCDC);
	loc += 2;
	static const unsigned int defaults1 = 0b00100001;//screen saver type = 1 sleep time = 2
	static const unsigned int defaults2 = 0b00000001;//screen saver time = 1
	unsigned char reserveFlags = 0;// makeUber == 1 ? 0x1 : 0x0;
	uint16_t ReserveContacts = ((reserveFlags) << 8) | 0x0;
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, START_STORAGE_LOCATION + loc, ReserveContacts);
	uint16_t Settings = (defaults1 << 8) | defaults2;
	loc += 2;
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, START_STORAGE_LOCATION + loc, Settings);
	loc += 2;
	uint8_t RadioID[2] = {0x3, 0x4};
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, START_STORAGE_LOCATION + loc, *((uint32_t*) &RadioID[0]));
	loc += 2;
	uint8_t privateKey[ContactStore::PRIVATE_KEY_LENGTH] = {0xab, 0x34, 0x4e, 0x58, 0x3f, 0x2a, 0x56, 0x39, 0x17, 0xef, 0x5c, 0xff, 0x8b,
		0xf8, 0x72, 0xe8, 0x87, 0x65, 0xd5, 0x11, 0x26, 0x58, 0x14, 0xb4};
	for (int i = 0; i < ContactStore::PRIVATE_KEY_LENGTH/2; i++, loc += 2) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, START_STORAGE_LOCATION + loc, *((uint16_t*) &privateKey[i]));
	}
	uint8_t agentName[ContactStore::AGENT_NAME_LENGTH] = {0x0};
	for (int i = 0; i < ContactStore::AGENT_NAME_LENGTH/2; i++, loc += 2) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, START_STORAGE_LOCATION + loc, *((uint16_t*) &agentName[i]));
	}

	HAL_FLASH_Lock();
#pragma GCC diagnostic pop
#endif
}

StateBase *CurrentState = 0;
static const uint32_t TIME_BETWEEN_INITS = 100;

uint32_t startBadge() {
	//SSD1306_UpdateScreen();
	//delay(5000);

	//uint32_t retVal = 0;
	//initFlash();


	//GUI_ListItemData items[4];
	//GUI_ListData DrawList((const char *) "Self Check", items, uint8_t(0), uint8_t(0), uint8_t(128), uint8_t(64),
	//		uint8_t(0), uint8_t(0));
	//DO SELF CHECK
	if (gui_init()) {
		delay(5000); // This shows the splash screen for 5 seconds.
		//gui_draw();
		//int8_t pix_val = 1;
		//while(1){
		//	for(int j = 0; j < SSD1306_HEIGHT; j++)
		//	{
		//		for(int i = 0; i < SSD1306_WIDTH; i++)
		//		{
		//			SSD1306_DrawPixel(i,j, pix_val);

		//			//delay(500);
		//		}
		//		SSD1306_UpdateScreen();
		//	}
		//	pix_val = ~pix_val;
		//}
		//delay(1000);
		//gui_draw();
		//SSD1306_Puts("Hello\n", &Font_7x10, 1);
		/*SSD1306_UpdateScreen();
		int16_t x0 = 63;
		int16_t y0 = 31;
		int16_t r = 10;
		uint8_t color = 1;
		SSD1306_DrawCircle(x0, y0, r, color);
		SSD1306_UpdateScreen();
		SSD1306_DrawTriangle(20, 20, 30, 15, 45, 50, color);

		const char *message1 = "Hello, world\n";
		const char *message2 = "I am a comupter.\n";
		const char *message3 = "10101010010101001010";
		SSD1306_Puts(message1, &Font_7x10, 1);
		SSD1306_GotoXY(0, 10);
		SSD1306_Puts(message2, &Font_7x10, 1);
		SSD1306_GotoXY(0, 20);
		SSD1306_Puts(message3, &Font_7x10, 1);
		SSD1306_UpdateScreen();*/
		//SSD1306_DrawPixel(127, 0, 1);

		//SSD1306_UpdateScreen();


		//while(1){}
	}

	return true;
}

void scroll_screen()
{

    int8_t pix_val = 1;
    while(1){
        for(int j = 0; j < SSD1306_HEIGHT; j++)
        {
            for(int i = 0; i < SSD1306_WIDTH; i++)
            {
                SSD1306_DrawPixel(i,j, pix_val);

                //delay(500);
            }
            SSD1306_UpdateScreen();
        }
        pix_val = ~pix_val;
    }
}

void loopBadge() {
    scroll_screen();
	//check to see if keyboard should be ignored
	//uint32_t tick = HAL_GetTick();
	//KB.scan();

	//ReturnStateContext rsc = CurrentState->run(KB);

	//if (rsc.Err.ok()) {
	//	if (CurrentState != rsc.NextMenuToRun) {
	//		//on state switches reset keyboard and give a 1 second pause on reading from keyboard.
	//		KB.reset();
	//	}
	//	if (CurrentState != StateFactory::getGameOfLifeState() && (tick > KB.getLastPinSelectedTick())
	//			&& (tick - KB.getLastPinSelectedTick()
	//					> (1000 * 60 * getContactStore().getSettings().getScreenSaverTime()))) {
	//		CurrentState->shutdown();
	//		CurrentState = StateFactory::getGameOfLifeState();
	//	} else {
	//		CurrentState = rsc.NextMenuToRun;
	//	}
	//} else {
	//	CurrentState = StateFactory::getDisplayMessageState(StateFactory::getMenuState(), "Run State Error....", 2000);
	//}

	//if (getContactStore().getSettings().isNameSet()) {
	//	StateFactory::getIRPairingState()->ListenForAlice();
	//}
	//StateFactory::getMessageState()->blink();

	//static uint32_t lastSendTime = 0;
	//if (tick - lastSendTime > 10) {
	//	lastSendTime = tick;
	//	if (Radio.receiveDone()) {
	//		if (Radio.TARGETID == RF69_BROADCAST_ADDR) {
	//			StateFactory::getMessageState()->addRadioMessage((const char *) &Radio.DATA[0], Radio.DATALEN,
	//					RF69_BROADCAST_ADDR, Radio.RSSI);
	//		} else {
	//			StateFactory::getMessageState()->addRadioMessage((const char *) &Radio.DATA[0], Radio.DATALEN,
	//					Radio.SENDERID, Radio.RSSI);
	//		}
//#ifndef DONT_USE_ACK
//			if(Radio.ACK_REQUESTED && Radio.SENDERID!=RF69_BROADCAST_ADDR) {
//				Radio.sendACK("ACK",4);
//			}
//#endif
//		}
//	}
	//configure 1 time listen RegListen1, RegListen2, RegListen3
	//defaults are good except for ListenCriteria should be 1 not 0

	//how to stop listen mode
	// Program RegOpMode with ListenOn=0, ListenAbort=1, and the desired setting for the Mode bits (Sleep, Stdby, FS, Rx
	//	or Tx mode) in a single SPI access
	//Program RegOpMode with ListenOn=0, ListenAbort=0, and the desired setting for the Mode bits (Sleep, Stdby, FS, Rx
	//	or Tx mode) in a second SPI access
	//
	// Cycle:
	//	Set mode to standby with listen on
	//		If interrupt fires grab data, stop listen mode, start listen mode.
	//	if you need to transmit
	//		stop listen mode
	//		Transmit
	//		go back into listen mode
	//goto standby and listen

	//getRadio().setListen();

	//gui_draw();
}

