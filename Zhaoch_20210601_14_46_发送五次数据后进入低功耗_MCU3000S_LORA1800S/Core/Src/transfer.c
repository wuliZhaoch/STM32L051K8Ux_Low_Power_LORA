#include "transfer.h"
//#define M202C
#define IRWML201G24
static unsigned char frameHead[5]={0x24, 0x50, 0x41, 0x52, 0x41};

static unsigned char auchCRCHi[256] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
static unsigned char auchCRCLo[256] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

unsigned short GetCRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
    unsigned char uchCRCHi = 0xFF ; /* 高 CRC 字节初始化 */
    unsigned char uchCRCLo = 0xFF ; /* 低 CRC 字节初始化 */
    unsigned uIndex = 0;
    /* CRC 循环中的索引 */while (usDataLen--) /* 传输消息缓冲区 */
    { uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算 CRC */ uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (unsigned short)((unsigned short)uchCRCHi << 8 | uchCRCLo) ;
}

unsigned char* reverse(unsigned char* pSrc, unsigned char nSrcLen)
{
    assert(pSrc!=NULL);
    unsigned char pTemp[4];
    memset(pTemp, 0, nSrcLen);
    memcpy(pTemp, pSrc, nSrcLen);
    for(int i=0; i<nSrcLen; i++)
    {
        pSrc[i] = pTemp[nSrcLen-1-i];
    }
    return pSrc;
}

unsigned char transfer_moa(
        const unsigned char* pBufIn,
        unsigned char ucBufInLen,
        unsigned char *parameterData,
        unsigned char* parameterDataLen,
        unsigned char* parameterDataNum)
{
    if(ucBufInLen!=28)
    {
        return 1;
    }

    unsigned short usCheckCalc = 0;
    for(int i=0; i<25; i++)
    {
        unsigned char ucData = pBufIn[i];
        usCheckCalc += ucData;
    }

    unsigned short usCheck = (unsigned short)(pBufIn[26]<<8) + (unsigned short)pBufIn[25];
    if(usCheck!=usCheckCalc)
    {
        return 1;
    }

    //
    #pragma pack(1)
    typedef struct _Parameter
    {
        unsigned char head;
        unsigned char  deviceType;				//设备类型
        unsigned char  pashe;					//相位
        float toaA;                             //泄漏电流（全电流）
        float resA;                             //避雷器阻性电流
        float ratioRC;                          //避雷器阻性电流相角
        unsigned int ligtngNum;                 //避雷器动作次数
        unsigned char sensorid[6];
        unsigned short  checkSum;
        unsigned char  end;
    } Parameter;
    #pragma pack() //让编译器对这个结构作1字节对齐

    Parameter parameter;
    //memcpy(sensorid, parameter.sensorid, 6);
    memcpy(&parameter, pBufIn, ucBufInLen);


    float toaA = parameter.toaA;  //泄漏电流（全电流）
    float resA = parameter.resA; //避雷器阻性电流
    float ratioRC = parameter.ratioRC; //避雷器阻性电流相角
    unsigned int ligtngNum = parameter.ligtngNum; //避雷器动作次数

    /*
    000000 0100 1010
    000000 0100 1011
    000000 0100 1100
    000000 0100 1101
    */

    CANSHU canshus[4] = {
        {0x004A, 0x01, 4, 0, toaA, 0, 0},
        {0x004B, 0x01, 4, 0, resA, 0, 0},
        {0x004C, 0x01, 4, 0, ratioRC, 0, 0},
        {0x004D, 0x01, 4, 1, 0, ligtngNum, 0}
    };

    *parameterDataNum = 4;

    unsigned char ucLen = 0;
    for(int i=0; i<4; i++)
    {
        CANSHU canshu = canshus[i];

        unsigned short ucTemp = ((unsigned short)(canshu.SensorType << 2)) | canshu.LengthFlag;

        memcpy(parameterData + ucLen, &ucTemp, 2);
        ucLen+=2;

        memcpy(parameterData + ucLen, &canshu.length, 1);
        ucLen+=1;

        #pragma pack(1)
        if(canshu.ucDataType==0)
        {
            memcpy((unsigned char*)parameterData + ucLen, (unsigned char*)&canshu.fData, 4);
            ucLen+=4;
        }
        if(canshu.ucDataType==1)
        {
            memcpy(parameterData + ucLen, &canshu.uiData, 4);
            ucLen+=4;
        }
        if(canshu.ucDataType==2)
        {
            memcpy(parameterData + ucLen, &canshu.ucData, 1);
            ucLen+=1;
        }
        #pragma pack()
    }

    *parameterDataLen = ucLen;
    return 0;
}

unsigned char transfer_sf6ws(
        const unsigned char* pBufIn,
        unsigned char ucBufInLen,
        unsigned char *parameterData,
        unsigned char* parameterDataLen,
        unsigned char* parameterDataNum)
{
    if(ucBufInLen!=42)
    {
        return 1;
    }

    unsigned char usCheckCalc = 0;
    for(int i=2; i<41; i++)
    {
        unsigned char ucData = pBufIn[i];
        usCheckCalc += ucData;
    }
    usCheckCalc = usCheckCalc ^ 0xff;
    unsigned char usCheck = pBufIn[41];
    if(usCheck!=usCheckCalc)
    {
        return 1;
    }

    //
    #pragma pack(1)
    typedef struct _Parameter
    {
        //温度，湿度，压力，露点，密度，微水
        unsigned short head;
        unsigned char id[10];
        unsigned short addr;
        unsigned char control;
        unsigned short len;
        float tmp;      //温度
        float hum;      //湿度
        float press;    //压力
        float ludian;   //露点
        float midu;     //密度
        float weishui;  //微水
        unsigned char end;
    } Parameter;
    #pragma pack() //让编译器对这个结构作1字节对齐

    Parameter parameter;
    memcpy(&parameter, pBufIn, ucBufInLen);

//    SF6气体密度 00000001111010   7A
//    SF6露点 00000001111011      7B
//    SF6微水 00000001111100        7C
//    SF6纯度 00000001111101        7D 无
    reverse((unsigned char*)&parameter.ludian, 4);
    reverse((unsigned char*)&parameter.midu, 4);
    reverse((unsigned char*)&parameter.weishui, 4);

    float ludian = parameter.ludian;  //露点
    float midu = parameter.midu; //密度
    float weishui = parameter.weishui; //微水

    CANSHU canshus[3] = {
        {0x007A, 0x01, 4, 0, midu, 0, 0},
        {0x007B, 0x01, 4, 0, ludian, 0, 0},
        {0x007C, 0x01, 4, 0, weishui, 0, 0}
    };

    *parameterDataNum = 3;

    unsigned char ucLen = 0;
    for(int i=0; i<3; i++)
    {
        CANSHU canshu = canshus[i];

        unsigned short ucTemp = ((unsigned short)(canshu.SensorType << 2)) | canshu.LengthFlag;

        memcpy(parameterData + ucLen, &ucTemp, 2);
        ucLen+=2;

        memcpy(parameterData + ucLen, &canshu.length, 1);
        ucLen+=1;

        if(canshu.ucDataType==0)
        {
            memcpy(parameterData + ucLen, &canshu.fData, 4);
            ucLen+=4;
        }
        if(canshu.ucDataType==1)
        {
            memcpy(parameterData + ucLen, &canshu.uiData, 4);
            ucLen+=4;
        }
        if(canshu.ucDataType==2)
        {
            memcpy(parameterData + ucLen, &canshu.ucData, 1);
            ucLen+=1;
        }
    }

    *parameterDataLen = ucLen;
    return 0;
}



/*
1Byte	44Byte	1Byte	1Byte
0xA5	数据区	累加和取反	0x5A
**/
unsigned char transfer_jxtx(
        const unsigned char* pBufIn,
        unsigned char ucBufInLen,
        unsigned char *parameterData,
        unsigned char* parameterDataLen,
        unsigned char* parameterDataNum)
{
    if(ucBufInLen!=74)
    {
        return 1;
    }

    unsigned short usCheckCalc = 0;
    for(int i=0; i<71; i++)
    {
        unsigned char ucData = pBufIn[i];
        usCheckCalc += ucData;
    }

    unsigned short usCheck = (unsigned short)(pBufIn[72]<<8) + (unsigned short)pBufIn[71];
    if(usCheck!=usCheckCalc)
    {
        return 1;
    }

    //
    #pragma pack(1)
    typedef struct _Parameter
    {
        unsigned char head;
        float fBClsTravelSpd;
        float fBClsTrip;
        float fBOpTravelSpd;
        float fBOpTrip;
        float clsTravelSpd;     //合闸速度
        float opTravelSpd;      //分闸速度
        float clsTrip;          //合位移开距
        float opTrip;           //分位移开距
        float clsCut;           //合闸线圈电流
        float opCut;            //分闸线圈电流
        float op2Cut;
        float engCutAvr;        //储能电机电流
        float clsTime;          //合闸时间
        float opTime;           //分闸时间
        float op2Time;
        float engCutMax;
        unsigned int optNum;	//机构动作次数
        unsigned char swState;	//开关分合位置
        unsigned char clsState;
        unsigned short checkSum;
        unsigned char end;
    } Parameter;
    #pragma pack() //让编译器对这个结构作1字节对齐

    //unsigned char m = sizeof(Parameter);
    Parameter parameter;
    memcpy(&parameter, pBufIn, ucBufInLen);


    //合闸速度m/s
    float clsTravelSpd = parameter.clsTravelSpd;

    //分闸速度m/s
    float opTravelSpd = parameter.opTravelSpd;

    //合位移开距mm，乘以0.1
    float clsTrip = parameter.clsTrip;

    //分位移开距mm，乘以0.1
    float opTrip = parameter.opTrip;


    //合闸线圈电流A，乘以0.01
    float clsCut = parameter.clsCut;

    //分闸线圈电流A，乘以0.01
    float opCut = parameter.opCut;


    //合闸时间ms，乘以0.1
    float clsTime = parameter.clsTime;

    //分闸时间ms，乘以0.1
    float opTime = parameter.opTime;

    //储能电机电流A，乘以0.01
    float engCutAvr = parameter.engCutAvr;

    //机构动作次数
    unsigned int optNum = parameter.optNum;

    //开关分合位置
    unsigned char swState = parameter.swState;


    //
    /*
    合位移开距	00000001010101	4	浮点数	mm
    合位移开距（角度）	00000001010110	4	浮点数	°
    合闸速度	00000001010111	4	浮点数	m/S
    合闸时间	00000001011000	4	浮点数	S
    合闸线圈电流	00000001011001	N	浮点数	A
    分位移开距	00000001011010	4	浮点数	mm
    分位移开距（角度）	00000001011011	4	浮点数	°
    分闸速度	00000001011100	4	浮点数	m/S
    分闸时间	00000001011101	4	浮点数	S
    分闸线圈电流	00000001011110	N	浮点数	A
    储能电机电流	00000001011111	N	浮点数	A
    机构动作次数	00000001100000	1	无符号整型	次
    机构行程曲线	00000001100001	N	浮点数	mm
    开关分合位置	00000001100010	1	无符号整型，分别为：0、其他；1、分闸到位；2、合闸到位；	\
    开关触头压紧力	00000001100011	4	浮点数	N
    机构档位	00000001100100	2	无符号整型	次
    clsTravelSpd    00 0000 0101 0111   0x0057
    opTravelSpd     00 0000 0101 1100   0x005C
    clsTrip         00 0000 0101 0101   0x0055
    opTrip          00 0000 0101 1010   0x005A
    clsCut          00 0000 0101 1001   0x0059
    opCut           00 0000 0101 1110   0x005E
    engCutAvr       00 0000 0101 1111   0x005F
    clsTime         00 0000 0101 1000   0x0058
    opTime          00 0000 0101 1101   0x005D
    optNum          00 0000 0110 0000   0x0060
    swState         00 0000 0110 0010   0x0062
    ******************/

    CANSHU canshus[11] = {
        {0x0057, 0x01, 4, 0, clsTravelSpd, 0, 0},
        {0x005C, 0x01, 4, 0, opTravelSpd, 0, 0},
        {0x0055, 0x01, 4, 0, clsTrip, 0, 0},
        {0x005A, 0x01, 4, 0, opTrip, 0, 0},
        {0x0059, 0x01, 4, 0, clsCut, 0, 0},
        {0x005E, 0x01, 4, 0, opCut, 0, 0},
        {0x005F, 0x01, 4, 0, engCutAvr, 0, 0},
        {0x0058, 0x01, 4, 0, clsTime, 0, 0},
        {0x005D, 0x01, 4, 0, opTime, 0, 0},
        {0x0060, 0x01, 4, 1, 0, optNum, 0},
        {0x0062, 0x01, 1, 2, 0, 0, swState}
    };

    *parameterDataNum = 11;

    unsigned char ucLen = 0;
    for(int i=0; i<11; i++)
    {
        CANSHU canshu = canshus[i];

        unsigned short ucTemp = ((unsigned short)(canshu.SensorType << 2)) | canshu.LengthFlag;

        memcpy(parameterData + ucLen, &ucTemp, 2);
        ucLen+=2;

        memcpy(parameterData + ucLen, &canshu.length, 1);
        ucLen+=1;

        if(canshu.ucDataType==0)
        {
            memcpy(parameterData + ucLen, &canshu.fData, 4);
            ucLen+=4;
        }
        if(canshu.ucDataType==1)
        {
            memcpy(parameterData + ucLen, &canshu.uiData, 4);
            ucLen+=4;
        }
        if(canshu.ucDataType==2)
        {
            memcpy(parameterData + ucLen, &canshu.ucData, 1);
            ucLen+=1;
        }
    }

    *parameterDataLen = ucLen;
    return 0;
}

/*
返回值，0代表成功，>0代表失败
*/
unsigned char transfer(
    // unsigned char protocalType,
    //unsigned char* sensor_id,
    const unsigned char* pBufIn,
    unsigned char ucBufInLen,
    unsigned char* pBufOut,
    unsigned char *ucBufOutLen
)
{
    if( (80!=ucBufInLen) && (48!=ucBufInLen) && (28!=ucBufInLen) )
    {
      return 1;
    }
    
    char sensor_id[6];

    unsigned char parameterData[100];
    unsigned char usParameterDataLen=0;
    unsigned char parameterDataNum = 0;

    //if(PROTOCAL_JXTX==protocalType)
    if(80==ucBufInLen)
    {
        //char pSensorId[6];
        //parserSensorId_moa(pBufIn, ucBufInLen, pSensorId);
        //memcpy(sensor_id, pSensorId, 6);
        unsigned char ret = transfer_jxtx(
            pBufIn,
            ucBufInLen,
            parameterData,
            &usParameterDataLen,
            &parameterDataNum);
        if(ret>0)
        {
          return ret;
        }
    }

    //if(PROTOCAL_MOA==protocalType)
    if(28==ucBufInLen)
    {
        memcpy(sensor_id, pBufIn+19, 6);
        //char pSensorId[6];
        //parserSensorId_moa(pBufIn, ucBufInLen, pSensorId);
        //memcpy(sensor_id, pSensorId, 6);
        //memcpy(sensor_id, pBufIn+10, 6);
        unsigned char ret = transfer_moa(
            pBufIn,
            ucBufInLen,
            parameterData,
            &usParameterDataLen,
            &parameterDataNum);
        if(ret>0)
        {
          return ret;
        }
    }

    //if(PROTOCAL_SF6WS==protocalType)
    if(48==ucBufInLen)
    {
        // char pSensorId[6];
        // parserSensorId_moa(pBufIn, ucBufInLen, pSensorId);
        // memcpy(sensor_id, pSensorId, 6);
        unsigned char ret = transfer_sf6ws(
            pBufIn,
            ucBufInLen,
            parameterData,
            &usParameterDataLen,
            &parameterDataNum);
        if(ret>0)
        {
          return ret;
        }
    }

    unsigned char pNetworkData[255];
    unsigned short networkDataLen = 0;
    unsigned char pCanshuNum = parameterDataNum;//参量个数
    unsigned char fragInd = 0x00;//分片指示
    unsigned char packetType = 0x00;//报文类型
    makeupNetworkPacket(pNetworkData, &networkDataLen, sensor_id, pCanshuNum, fragInd, packetType, parameterData, usParameterDataLen);

#ifdef M202C
    makeupFramePacket(pBufOut, pNetworkData, networkDataLen);
    *ucBufOutLen = networkDataLen+10;
#endif
    
#ifdef IRWML201G24
    memcpy(pBufOut, pNetworkData, networkDataLen);
    *ucBufOutLen = networkDataLen;
#endif
    return 0;
}

unsigned char* makeupNetworkPacket(
    unsigned char* pNetworkData,
    unsigned short* networkDataLen,
    const unsigned char *pSensorId,
    unsigned char pCanshuNum,
    unsigned char fragInd,
    unsigned char packetType,
    unsigned char* pCanshuData,
    unsigned char canshuDataLen
)
{
    assert(NULL!=pNetworkData); //9+canshuDataLen;
    memcpy(pNetworkData, pSensorId, 6);

    unsigned char temp = ((unsigned char)(pCanshuNum << 4)) | ((unsigned char)(fragInd<<3)) | packetType;
    memcpy(pNetworkData+6, &temp, 1);

    memcpy(pNetworkData+7, pCanshuData, canshuDataLen);

    unsigned short len = GetCRC16(pNetworkData, 7+canshuDataLen);
    memcpy(pNetworkData+7+canshuDataLen, &len, 2);

    *networkDataLen = canshuDataLen + 9;
    return pNetworkData;
}

unsigned char* makeupFramePacket(
    unsigned char* pFramePacket,
    unsigned char *pNetworkData,
    unsigned short usNetworkDataLen
)
{
    assert(NULL!=pFramePacket);

    memcpy(pFramePacket, frameHead, 5);

    unsigned short usFrameLen = usNetworkDataLen+3;
    reverse((unsigned char*)&usFrameLen, 2);
    memcpy(pFramePacket+5, &usFrameLen, 2);

    unsigned char cmd = 0x58;
    memcpy(pFramePacket+7, &cmd, 1);

    memcpy(pFramePacket+8, pNetworkData, usNetworkDataLen);

    unsigned short crcCheck = GetCRC16(pFramePacket, usNetworkDataLen+8);
    memcpy(pFramePacket+8+usNetworkDataLen, &crcCheck, 2);

    return pFramePacket;
}


