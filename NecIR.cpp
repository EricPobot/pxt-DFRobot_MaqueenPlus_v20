
#include "pxt.h"


#include "MicroBit.h"

#ifndef MICROBIT_CODAL
#ifdef CODAL_CONFIG_H
#define MICROBIT_CODAL 1
#else
#define MICROBIT_CODAL 0
#endif
#endif

#if MICROBIT_CODAL

#else // MICROBIT_CODAL



class PullMode
{
public:
    PinMode pm;

    PullMode()            : pm( PullNone) {};
    PullMode( uint8_t p)  : pm( (PinMode)p) {};
    PullMode( PinMode p)  : pm( p) {};

    static const PinMode None = PullNone;
    static const PinMode Down = PullDown;
    static const PinMode Up   = PullUp;

    operator PinMode() { return pm; }
    operator uint8_t() { return pm; }
};

#endif // MICROBIT_CODAL

//% color=50 weight=80
//% icon="\uf1eb"
namespace maqueenIRV2 { 
int ir_code = 0x00;
int ir_addr = 0x00;
int data1 = 0;
int data;

int logic_value(){// logic value "0" and "1" assessment sub-function
    uint32_t lasttime = system_timer_current_time_us();
    uint32_t nowtime;
    while(!uBit.io.P16.getDigitalValue());//Low wait
    nowtime = system_timer_current_time_us();
    //uBit.serial.printf("3 %d\r\n",(nowtime - lasttime));
    if((nowtime - lasttime) > 460 && (nowtime - lasttime) < 660){//Low level 560us
        while(uBit.io.P16.getDigitalValue());// wait until low
        lasttime = system_timer_current_time_us();
        //uBit.serial.printf("%d\r\n",(lasttime - nowtime));
        if((lasttime - nowtime)>460 && (lasttime - nowtime) < 660){// Continue high level 560us
            return 0;
        }else if((lasttime - nowtime)>1600 && (lasttime - nowtime) < 1800){//Continue high level for 1.7ms
            return 1;
       }
    }
//uBit.serial.printf("error\r\n");
    return -1;
}

void pulse_deal(){
    int i;
    ir_addr=0x00;// clear
    for(i=0; i<16;i++ )
    {
      if(logic_value() == 1)
      {
        ir_addr |=(1<<i);
      }
    }
    // Analyze the command command in the remote control code
    ir_code=0x00;//清零
    for(i=0; i<16;i++ )
    {
      if(logic_value() == 1)
      {
        ir_code |=(1<<i);
      }
    }

}

void remote_decode(void){
    data = 0x00;
    uint32_t lasttime = system_timer_current_time_us();
    uint32_t nowtime;
    while(uBit.io.P16.getDigitalValue()){// high level wait
    //uBit.serial.printf("1\r\n");
        nowtime = system_timer_current_time_us();
        if((nowtime - lasttime) > 100000){// more than 100 ms, indicating that no button is pressed at this time
            ir_code = 0xffff;
            return;
        }
    }
    //uBit.serial.printf("2\r\n");
    //If the high level duration does not exceed 100ms
    lasttime = system_timer_current_time_us();
    while(!uBit.io.P16.getDigitalValue());// Low wait
    //uBit.serial.printf("3\r\n");
    nowtime = system_timer_current_time_us();
    //uBit.serial.printf("1 %d\r\n",(nowtime - lasttime));
    if((nowtime - lasttime) < 9100 && (nowtime - lasttime) > 8800){//9ms
        while(uBit.io.P16.getDigitalValue());// high wait
        lasttime = system_timer_current_time_us();
        //uBit.serial.printf("1 %d\r\n",(lasttime - nowtime));
        if((lasttime - nowtime) > 4400 && (lasttime - nowtime) < 4510){// 4.5ms, the infrared protocol header is received and it is newly sent data. Start parsing logical 0s and 1s
            pulse_deal();
            //uBit.serial.printf("addr=0x%X,code = 0x%X\r\n",ir_addr,ir_code);
            //uBit.serial.printf("1\r\n");
            data = data1 = ir_code;
            
            return;//ir_code;
        }else if((lasttime - nowtime) > 2150 && (lasttime - nowtime) < 2260){// the infrared protocol header is received and it is newly sent data. Start parsing logical 0s and 1s
        
            while(!uBit.io.P16.getDigitalValue());// Low wait
            nowtime = system_timer_current_time_us();
            if((nowtime - lasttime) > 460 && (nowtime - lasttime) < 660){//560us
                //uBit.serial.printf("addr=0x%X,code = 0x%X\r\n",ir_addr,ir_code);
                //uBit.serial.printf("2\r\n");
                data = data1;
                
                return;//ir_code;
            }
        }
    }
}

 //% 
int irCode(){
    PullMode pullmode = PullMode::Up;
    uBit.io.P16.setPull(pullmode);
    remote_decode();
    return data;
}

}
