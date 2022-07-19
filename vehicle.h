//
// Created by cyx02 on 2022/6/27.
//

#ifndef BUBBLEMAC_INFOCOM_VEHICLE_H
#define BUBBLEMAC_INFOCOM_VEHICLE_H

#include "common.h"
#include <vector>
#include <unordered_map>
#include "parameters.h"
#include <fstream>
#include "packet.h"

#define ROLE_H 1
#define ROLE_T 2
#define ROLE_I 3
#define ROLE_S 0
#define OCCUPIED 1
#define ACCESS 0

using namespace std;

typedef struct vehicle_info{
    struct vehicle* address;
    int timestamp;
}Vehicle_info;



typedef struct vehicle
{
    int handled;                  //  to indicate whether the car has been updated during this time
    int slot_appeared;
    //basic info
    char id[20];// id of the car
    char type[10]; //model of the car

    //dynamic info
    double x;
    double y;
    double angle;
    double speed;
    double pos;
    char lane[10];
    char prev_lane[10];//记录上一个车道
    double slope;
    double flow;
    double speed2;


    double acc = 4.5;  // for this version. this is determined by type.

    //slot info
    int slot_condition ;  //OCCUPIED or ACCESS
    int slot_occupied; //the occupied slot
    int resource_pool;  //0 for left(0, SlotPerframe/2 -1) and 1 for R(SlotPerframe/2, SlotPerFrame-1)


    struct vehicle* OHN[200];
    struct vehicle* THN[200];
    struct vehicle* prev_OHN[200];


    int ve_check_flag;

    //Commrange
    double commRadius;

    //Packets Received
    struct Duallist packets;

    //Neighbors to acc
    struct Duallist neighbours;

    int acceess_timestamp;


}Vehicle;

double distance_between_vehicle(const struct vehicle* aCar, const struct vehicle* bCar);
void printVehilces(struct Duallist *ALL_Vehicles);
void logVehilcesInfo(struct Duallist *ALL_Vehicles, ofstream & logfile);
void logACar(struct vehicle* aCar, ofstream & logfile);

#endif //BUBBLEMAC_INFOCOM_VEHICLE_H
