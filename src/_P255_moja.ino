#include "_Plugin_Helper.h"

#ifdef USES_P255
//#######################################################################################################
//######################### Plugin 255: Mój plugin                       ################################
//#######################################################################################################

#define PLUGIN_255
#define PLUGIN_ID_255         255
#define PLUGIN_NAME_255       "RTC Sleep"
#define PLUGIN_255_VALUENAME1 "Active"
#define PLUGIN_255_VALUENAME2 "Temperature"

uint8_t buffor[20];

uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

boolean Plugin_255(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_255;
        Device[deviceCount].Type = DEVICE_TYPE_I2C;
        Device[deviceCount].VType = Sensor_VType::SENSOR_TYPE_DUAL;
        Device[deviceCount].Ports = 0;
		    Device[deviceCount].ValueCount = 2;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].SendDataOption = false;
		    Device[deviceCount].TimerOption = true;
        Device[deviceCount].TimerOptional = false;
        Device[deviceCount].OutputDataType = Output_Data_type_t::Simple;
        break;
      }
    
    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_255);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_255_VALUENAME1));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_255_VALUENAME2));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        addFormSubHeader(F("Ustaw interwal"));//dodatkowa linia
        addFormNumericBox(F("Sekund"), F("p255_sek"), PCONFIG(0),0,59);
        addFormNumericBox(F("Minut "), F("p255_min"), PCONFIG(1),0,59);
        addFormNumericBox(F("Godzin"), F("p255_god"), PCONFIG(2),0,23);
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        PCONFIG(0)=getFormItemInt(F("p255_sek"));
        PCONFIG(1)=getFormItemInt(F("p255_min"));
        PCONFIG(2)=getFormItemInt(F("p255_god"));
        success = true;
        break;
      }
      
     
    case PLUGIN_WEBFORM_SHOW_VALUES: //dodatkowe dane w polu Values strony Devices bez odświeżania
      {
        Plugin_255_I2C_READ_BUF(0x68, 0x00, 19, buffor);
        pluginWebformShowValue(event->TaskIndex, 4, F("Time"), String(buffor[2], HEX)+":"+String(buffor[1], HEX)+":"+String(buffor[0], HEX));
        pluginWebformShowValue(event->TaskIndex, 5, F("Wakeup"), String(buffor[9], HEX)+":"+String(buffor[8], HEX)+":"+String(buffor[7], HEX));
        pluginWebformShowValue(event->TaskIndex, 6, F("R_0Eh"), String(buffor[14], BIN));
        pluginWebformShowValue(event->TaskIndex, 7, F("R_0Fh"), String(buffor[15], BIN));
        pluginWebformShowValue(event->TaskIndex, 7, F("T_msb"), String(buffor[17], BIN));
        pluginWebformShowValue(event->TaskIndex, 7, F("T_lsb"), String(buffor[18]>>6, BIN), true);
        //pluginWebformShowValue(event->TaskIndex, 5, ExtraTaskSettings.TaskDeviceValueNames[3], String(buffor[0], BIN))); //true kończy linię :)
        //pluginWebformShowValue(event->TaskIndex, 6, ExtraTaskSettings.TaskDeviceValueNames[3], String(buffor[0], HEX))); //true kończy linię :)
        //success = true; //true nic nie dodaje, false - dodaje te az automatu,
        break;
      }
            
    case PLUGIN_INIT:
      {
        UserVar[event->BaseVarIndex]=0;
        break;
      }

    case PLUGIN_WEBFORM_SHOW_CONFIG: //pokazuje to w polu Port strony Devices
      {
        string = F("Sleep Time: ");
        string += String(PCONFIG(2));
        string += F("h ");
        string += String(PCONFIG(1));
        string += F("min ");
        string += String(PCONFIG(0));
        string += F("sek");
        success = true;
        break;
      }

    
    case PLUGIN_READ:
      {
        Plugin_255_I2C_READ_BUF(0x68, 0x00, 19, buffor);
    
        if(UserVar[event->BaseVarIndex])
           {
             UserVar[event->BaseVarIndex]=0;
             //zmiana na dec
             buffor[7]=bcd2bin(buffor[0]);
             buffor[8]=bcd2bin(buffor[1]);
             buffor[9]=bcd2bin(buffor[2]);

             //dodaje interwal         
             //sekundy
             buffor[7]+=PCONFIG(0);
             if (buffor[7]>=60) {buffor[7]-=60; buffor[8]++;}
             //minuty
             buffor[8]+=PCONFIG(1);
             if (buffor[8]>=60) {buffor[8]-=60; buffor[9]++;}
             //godziny
             buffor[9]+=PCONFIG(2);
             if (buffor[9]>=24) {buffor[9]-=24;} //nie ma przesunięcia o dzień
          
             //zmiana na bcd 
             buffor[8]=bin2bcd(buffor[8]);
             buffor[7]=bin2bcd(buffor[7]);
             buffor[9]=bin2bcd(buffor[9]);
          
             // reszta ustawien
             buffor[10]=0x80; //alarm 1 godzina, min, sek, match
             buffor[14]=0x45; //ustawienie konfiguracji
             buffor[15]=0; //skasowanie alarmu
     
             Plugin_255_I2C_WRITE_BUF(0x68, 0x07, 9, &buffor[7]);
          }

        UserVar[event->BaseVarIndex+1]=(float)buffor[17] + (buffor[18] >> 6) * 0.25f;

        success = true;
        break;
      }
    }
    return success;
}



void Plugin_255_I2C_READ_BUF(uint8_t S_ADDR, uint8_t U_ADDR, uint8_t DLUGOSC, uint8_t *BUFOR)
{
  Wire.beginTransmission(S_ADDR);
  Wire.write((uint8_t)U_ADDR);
	Wire.endTransmission();
  //Wire.beginTransmission(S_ADDR);
  Wire.requestFrom(S_ADDR, (byte)DLUGOSC); 
	while (Wire.available()) *BUFOR++ = Wire.read();
  Wire.endTransmission();
};

void Plugin_255_I2C_WRITE_BUF(uint8_t S_ADDR, uint8_t U_ADDR, uint8_t DLUGOSC, uint8_t *BUFOR)
{
  Wire.beginTransmission(S_ADDR);
  Wire.write((uint8_t)U_ADDR);
	Wire.write(BUFOR, (byte)DLUGOSC);
	Wire.endTransmission();
};
#endif // USES_P255
