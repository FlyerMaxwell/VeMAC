//
// Created by cyx02 on 2022/6/27.
//

#include "parameters.h"

int UpLocSlot = 10;//每个slot单位是1ms，更新位置的时间间隔为5ms，故为5个slot.      slot/UpLocSlot即为第几个5ms，或第几个位置
int SlotPerFrame = 200;
int Car_Number = 0;
int cnt_pkt_tx = 0;

int cnt_pkt_0;  //两个发射碰撞A
int cnt_pkt;
int cnt_pkt_1;  //正常收包
int cnt_pkt_2;  //产生碰撞的包

int cnt_received;

int cnt_coli =0;

int traffic_density = 50;

int counterToChange = 3;
int len_head_resource = 10;
int len_tail_resouce = 10;
