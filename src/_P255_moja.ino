#include "_Plugin_Helper.h"

#ifdef USES_P255
//#######################################################################################################
//######################### Plugin 255: Mój plugin                       ################################
//#######################################################################################################

#define PLUGIN_255
#define PLUGIN_ID_255         255
#define PLUGIN_NAME_255       "RTC Sleep"
#define PLUGIN_VALUENAME1_255 "Active"
#define PLUGIN_VALUENAME2_255 "Sekund"
#define PLUGIN_VALUENAME3_255 "Minut"
#define PLUGIN_VALUENAME4_255 "Godziny_system"

#define PLUGIN_VALUENAME5_255 "1Active"
#define PLUGIN_VALUENAME6_255 "1Sekundy"
#define PLUGIN_VALUENAME7_255 "1Minuty"
#define PLUGIN_VALUENAME8_255 "1Godziny_system"

#define PLUGIN_VALUENAME9_255 "2Active"
#define PLUGIN_VALUENAME10_255 "2Sekundy"
#define PLUGIN_VALUENAME11_255 "2Minuty"
#define PLUGIN_VALUENAME12_255 "2Godziny_system"

#define PLUGIN_VALUENAME13_255 "3Active"
#define PLUGIN_VALUENAME14_255 "3Sekundy"
#define PLUGIN_VALUENAME15_255 "3Minuty"
#define PLUGIN_VALUENAME16_255 "3Godziny_system"

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
        Device[deviceCount].VType = Sensor_VType::SENSOR_TYPE_QUAD;
        Device[deviceCount].Ports = 0;
		    Device[deviceCount].ValueCount = 4;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].SendDataOption = false;
		    Device[deviceCount].TimerOption = true;
        Device[deviceCount].TimerOptional = false;
        //Device[deviceCount].OutputDataType = Output_Data_type_t::Simple;
        break;
      }
    
    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_255);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        switch PCONFIG(3){
          case 0: //zegar
            {
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME4_255));
              break;
            }
          
          case 1: //alarm
            {
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME5_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME6_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME7_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME8_255));
              break;
            }
          
          case 2: //alarm tryb
            {
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME9_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME10_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME11_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME12_255));
              break;
            } 

          case 3: //status
            {
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME13_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME14_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME15_255));
              strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME16_255));
              break;
            }
        }
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        addFormSubHeader(F("Ustaw interwal"));//dodatkowa linia
        addFormNumericBox(F("Sekund"), F("p255_sek"), PCONFIG(0),0,59);
        addFormNumericBox(F("Minut "), F("p255_min"), PCONFIG(1),0,59);
        addFormNumericBox(F("Godzin"), F("p255_god"), PCONFIG(2),0,23);

        addFormSubHeader(F("Tryb wyświetlania"));//dodatkowa linia
        byte wartosc_obecna = PCONFIG(3);
        String opcje[4] = { F("Zegar"), F("Alarm"), F("Alarm_tryb"), F("Status/Config")};
        addFormSelector(F("wybrane"), F("p255_tryb"), 4, opcje, NULL, wartosc_obecna );

        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        PCONFIG(0)=getFormItemInt(F("p255_sek"));
        PCONFIG(1)=getFormItemInt(F("p255_min"));
        PCONFIG(2)=getFormItemInt(F("p255_god"));
        PCONFIG(3)=getFormItemInt(F("p255_tryb"));
        success = true;
        break;
      }
      
      /*
    case PLUGIN_WEBFORM_SHOW_VALUES: //dodatkowe dane w polu Values strony Devices bez odświeżania
      {
        Plugin_255_I2C_READ_BUF(0x68, 0x00, 16, buffor);
        //addHtml(pluginWebformShowValue(event->TaskIndex, 0, ExtraTaskSettings.TaskDeviceValueNames[0], String(UserVar[event->BaseVarIndex], HEX)));
        //addHtml(pluginWebformShowValue(event->TaskIndex, 1, ExtraTaskSettings.TaskDeviceValueNames[1], String(UserVar[event->BaseVarIndex+1], BIN)));
        //addHtml(pluginWebformShowValue(event->TaskIndex, 2, ExtraTaskSettings.TaskDeviceValueNames[2], String(UserVar[event->BaseVarIndex+2], HEX)));
        addHtml(pluginWebformShowValue(event->TaskIndex, 3, ExtraTaskSettings.TaskDeviceValueNames[3], String(buffor[0], BIN))); //true kończy linię :)
        addHtml(pluginWebformShowValue(event->TaskIndex, 3, ExtraTaskSettings.TaskDeviceValueNames[3], String(buffor[0], HEX))); //true kończy linię :)
        success = true; //true nic nie dodaje, false - dodaje te az automatu,
        break;
      }
       //pluginWebformShowValue(x, varNr, ExtraTaskSettings.TaskDeviceValueNames[varNr], formatUserVarNoCheck(x, varNr));
        */
    case PLUGIN_INIT:
      {
        UserVar[event->BaseVarIndex]=0;
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SHOW_CONFIG: //pokazuje to w polu Port strony Devices
      {
        switch PCONFIG(3){
          case 0: //zegar
          {
            string = F("Sleep Time: ");
            string += String(PCONFIG(2));
            string += F("h ");
            string += String(PCONFIG(1));
            string += F("min ");
            string += String(PCONFIG(0));
            string += F("sek");
            break;
          }
          case 1: //alarm
          {
            string = F("Alarm");
            break;
          }
          case 2: //alarm tryb
          {
            string = F("Alarm_tryb");
            break;
          }
          case 3: //status
          {
            string = F("Status/Config");
            break;
          }
        }
        success = true;
        break;
      }

    

    case PLUGIN_READ:
    
      {
        Plugin_255_I2C_READ_BUF(0x68, 0x00, 19, buffor);
        switch (PCONFIG(3)){
          case 0: //zegar
          {
            if(UserVar[event->BaseVarIndex])
              {
                UserVar[event->BaseVarIndex]=0;
                
                //zmiana na dec
                buffor[7]=bcd2bin(buffor[0]);
                buffor[8]=bcd2bin(buffor[1]);
                buffor[9]=bcd2bin(buffor[2]);
          
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

              UserVar[event->BaseVarIndex+1]=10*(buffor[0]>>4)+(buffor[0]&0x0F);
              UserVar[event->BaseVarIndex+2]=10*(buffor[1]>>4)+(buffor[1]&0x0F);
              UserVar[event->BaseVarIndex+3]=10*(buffor[2]>>4)+(buffor[2]&0x0F);
              break;
          }
          case 1: //alarm
          {
            buffor[7]=buffor[7]&0x7F;
            buffor[8]=buffor[8]&0x7F;
            buffor[9]=buffor[9]&0x7F;
            UserVar[event->BaseVarIndex+1]=10*(buffor[7]>>4)+(buffor[7]&0x0F);
            UserVar[event->BaseVarIndex+2]=10*(buffor[8]>>4)+(buffor[8]&0x0F);
            UserVar[event->BaseVarIndex+3]=10*(buffor[9]>>4)+(buffor[9]&0x0F);
            break;
          }
          case 2: //alarm tryb
          {
            UserVar[event->BaseVarIndex+1]=(buffor[7]>>7);
            UserVar[event->BaseVarIndex+1]+=((buffor[8]>>7)<<1);
            UserVar[event->BaseVarIndex+1]+=((buffor[9]>>7)<<2);
            UserVar[event->BaseVarIndex+1]+=((buffor[10]>>7)<<3);
            UserVar[event->BaseVarIndex+2]=buffor[17];
            UserVar[event->BaseVarIndex+3]=buffor[18];
            break;
          }
          case 3: //status
          {
            UserVar[event->BaseVarIndex+1]=10*(buffor[14]>>4)+(buffor[14]&0x0F);
            UserVar[event->BaseVarIndex+2]=10*(buffor[15]>>4)+(buffor[15]&0x0F);
            float temperatura=0.0f;
            temperatura+=float(buffor[17]);
            temperatura*=4.0f;
            temperatura+=float(buffor[18]>>6);
            temperatura/=4.0f;
            UserVar[event->BaseVarIndex+3]=float(temperatura);
            break;
          }        
        }
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
