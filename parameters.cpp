//
// Created by cyx02 on 2022/6/27.
//

#include "parameters.h"

int UpLocSlot = 10;//每个slot单位是1ms，更新位置的时间间隔为5ms，故为5个slot.      slot/UpLocSlot即为第几个5ms，或第几个位置
int SlotPerFrame = 200;
int Car_Number = 0;

int traffic_density = 50;



// Log parameters
bool log_process_flag = true;    // to log the process
ofstream log_process_file;
bool log_statistic_flag = true;    // to log the statistical target
ofstream log_statistic_file;


// Statistic Parameters
int cnt_cars = 0;
int cnt_tx_collision = 0;     //两个发射碰撞
int cnt_rx_normal = 0;       //正常收包
int cnt_rx_colli = 0;        //产生碰撞的包
int cnt_pkt_tx = 0;          //发射的包个数
int cnt_pkt_tx_normal = 0;   //发射过程中没有TX碰撞的个数
int cnt_frontV_normal;       //收到的前方的正常包
int cnt_rearV_normal;       //收到的后车的正常包
int cnt_frontV_colli;       //收到的前车的碰撞包
int cnt_rearV_Colli;        //收到的后车的碰撞包

struct vehicle* collision_vehicle= nullptr;
