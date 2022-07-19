//
// Created by cyx02 on 2022/6/27.
//

#ifndef BUBBLEMAC_INFOCOM_PACKETS_H
#define BUBBLEMAC_INFOCOM_PACKETS_H

#include "vehicle.h"
#include <unordered_map>
#include <vector>
#include <fstream>
#include "common.h"
using namespace std;


typedef struct packet{
    struct vehicle* srcVehicle, *dstVehicle;//for debug
    //timestamp
    int timestamp;
    int condition;//0 for tx collsion;1 for not

    //basic info
    char id[20];

    //Dynamic info
    double x, y, a, angle, speed;
    char lane[10];
    int turn_lane;

    //Slot info
    int slot_condition;
    int slot_occupied;
    struct vehicle* OHN_snapshot[200];  //slotPerframe=200，当前ONH时槽使用情况的快照
    //power info
    double commRange;

    // queue info
    // unordered_map<struct vehicle*, int> hashtable;
    vector<struct vehicle*> *hashtable_vehicles;
    vector<int> *hashtable_slot;
}Packet;



void log_packet(struct packet * aPkt, int slot, ofstream &logfile);



#endif //BUBBLEMAC_INFOCOM_PACKETS_H
