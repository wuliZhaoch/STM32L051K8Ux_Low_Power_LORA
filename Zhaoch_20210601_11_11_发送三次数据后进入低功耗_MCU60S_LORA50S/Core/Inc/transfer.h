#ifndef TRANSFER_H
#define TRANSFER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
断路器机械特性输出报文：2450415241005658a92e0000ff01b05d010400000000710104000000005501040000000069010400000000650104000000007901049a1d14447d010434d32e44610104000000007501040000804081010400000000890101009654da4f
避雷器输出报文：2450415241002858a92e0000ff01402901048fc2753f2d01049eefa73d3101040000aa423501040cae01029f531e24
SF6微水输出报文：2450415241002158a92e0000ff0130e90104ce97833aed010420ce2641f101042102464639058cb9
//调用例程
#include "transfer.h"
const unsigned char pBufIn[42]= {0x4f, 0x4d, 0x20, 0x20, 0x20, 0x4d, 0x38, 0x30, 0x30, 0x2d, 0x4d, 0x57, 0x00, 0x10, 0x01, 0x00, 0x18, 0x41, 0x96, 0xb8, 0x52, 0x42, 0x68, 0x4b, 0x8e, 0x3a, 0x83, 0x12, 0x6f, 0x41, 0x26, 0xce, 0x20, 0x3a, 0x83, 0x97, 0xce, 0x46, 0x46, 0x02, 0x21, 0xf8};
unsigned char bufInLen = 42;
unsigned char* pBufOut = static_cast<unsigned char* >(malloc(1400));
unsigned char bufOutLen = 0;
unsigned char ret = transfer(static_cast<unsigned char>(PROTOCAL_SF6WS), sensorId, static_cast< unsigned char const *>(pBufIn), static_cast<unsigned char>(bufInLen), pBufOut, &bufOutLen);
QByteArray ba_bufout((const char*)pBufOut, (int)bufOutLen);
if(ret>0)
{
    // 错误
}
free(pBufOut);
pBufOut = NULL;
qDebug()<<ba_bufout.toHex().data();
*/
#ifndef PROTOCAL_MOA
#define PROTOCAL_MOA 1
#endif

#ifndef PROTOCAL_JXTX
#define PROTOCAL_JXTX 2
#endif

#ifndef PROTOCAL_SF6WS
#define PROTOCAL_SF6WS 3
#endif
//enum PROTOCAL {PROTOCAL_MOA=1, PROTOCAL_JXTX, PROTOCAL_SF6WS};

#pragma pack(1) //让编译器对这个结构作1字节对齐
typedef struct _CANSHU
{
    unsigned short SensorType;
    unsigned short LengthFlag;
    unsigned char length;
    unsigned char ucDataType;   // 0 float   1 int  2 char
    float fData;
    unsigned int uiData;
    unsigned char ucData;
}CANSHU;
#pragma pack() //让编译器对这个结构作1字节对齐

unsigned short GetCRC16(unsigned char *puchMsg, unsigned short usDataLen);
unsigned char* reverse(unsigned char* pSrc, unsigned char nSrcLen);

/********************************************************************************
* @Author:yezhiwei
* @Version: 1.0.0
* @Date: 2021-3-24
* @Description:
* 从传感器接收的数据端解释并提取数据，转成标准的1531物联网协议中的监测数据报文内容字段格式
* @params：
* pBufIn：传感器发过来的报文（输入）
* ucBufInLen：传感器发过来的报文长度（输入）
* parameterData：监测数据报文指针（输出）
* parameterDataLen：监测数据报文长度（输出）
* parameterDataNum：监测参量个数
* @return：
* 监测数据报文指针
********************************************************************************/
//const unsigned char pBufIn[22]= {0xA5, 0xA1, 0x01, 0x8F, 0xC2, 0x75, 0x3F, 0x9E, 0xEF, 0xA7, 0x3D, 0x00, 0x00, 0xAA, 0x42, 0x0C, 0xAE, 0x01, 0x02, 0x66, 0x07, 0x5A};
unsigned char transfer_moa(
    const unsigned char* pBufIn,
    unsigned char ucBufInLen,
    unsigned char *parameterData,
    unsigned char *parameterDataLen,
    unsigned char* parameterDataNum);
//const unsigned char pBufIn[42]= {0x4f, 0x4d, 0x20, 0x20, 0x20, 0x4d, 0x38, 0x30, 0x30, 0x2d, 0x4d, 0x57, 0x00, 0x10, 0x01, 0x00, 0x18, 0x41, 0x96, 0xb8, 0x52, 0x42, 0x68, 0x4b, 0x8e, 0x3a, 0x83, 0x12, 0x6f, 0x41, 0x26, 0xce, 0x20, 0x3a, 0x83, 0x97, 0xce, 0x46, 0x46, 0x02, 0x21, 0xf8};
unsigned char transfer_sf6ws(
    const unsigned char* pBufIn,
    unsigned char ucBufInLen,
    unsigned char *parameterData,
    unsigned char *parameterDataLen,
    unsigned char* parameterDataNum);
//const unsigned char pBufIn[74]= {0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9A, 0x1D, 0x14, 0x44, 0x00, 0x00, 0x00, 0x00, 0x34, 0xD3, 0x2E, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x78, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x05, 0x5A};
unsigned char transfer_jxtx(
    const unsigned char* pBufIn,
    unsigned char ucBufInLen,
    unsigned char *parameterData,
    unsigned char *parameterDataLen,
    unsigned char* parameterDataNum);
/********************************************************************************/


/*
返回值，0代表成功，>0代表失败
*/
unsigned char transfer(
    // unsigned char protocalType,
    // unsigned char* sensor_id,
    const unsigned char* pBufIn,
    unsigned char ucBufInLen,
    unsigned char *pBufOut,
    unsigned char* ucBufOutLen);

//上行数据报文
unsigned char* makeupFramePacket(
    unsigned char* pFramePacket,
    unsigned char *pNetworkData,
    unsigned short usDataLen);

//上行数据网络报文
unsigned char* makeupNetworkPacket(
    unsigned char* pNetworkData,
    unsigned short *networkDataLen,
    const unsigned char* pSensorId,
    unsigned char pCanshuNum,
    unsigned char fragInd,
    unsigned char packetType,
    unsigned char* pCanshuData,
    unsigned char canshuDataLen);

#endif // TRANSFER_H
