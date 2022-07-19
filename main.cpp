#include <iostream>
#include <fstream>
#include "common.h"
#include "UpLocation.h"
#include "parameters.h"
#include "Protocol_VeMAC.h"
#include "communication.h"
#include "vehicle.h"
/*
 * This will be a final version of VeMAC
 *
 * By Maxwell 2022/07/13
 *
 */

using namespace std;

int main(int argc, char * argv[]) {

    cout << "Hello, World!" << std::endl;

    // Set the trace path
//    string trace_path;
//    trace_path = argv[1];       //读取文件路径
//    cout<< "Current trace file is from:"<< trace_path <<"..."<<endl;


    string trace_path = "C:\\Users\\cyx02\\Desktop\\transformed\\transformed\\carposition_";
    //仿真的详细log路径，记录所有发生的事件
    string log_dir = ".\\log.txt";

    ofstream logfile;

    logfile.open(log_dir);
    if(!logfile){cerr <<"create file error \n"; return 1;}

    srand(0);

    int slot_start = 0;
    int slot_end =40000; //注意单位是毫秒
    int slot_step = 1;

    struct Duallist ALL_Vehicles;
    duallist_init(&ALL_Vehicles);

    for(int slot = slot_start; slot < slot_end; slot += slot_step){
        //cout<<"slot = "<< slot<<endl;
        if(slot %1000 == 0)
            cout<<"slot = "<< slot<<endl;

        if(slot % UpLocSlot == 0){
            init_simulation(&ALL_Vehicles);
            updateLocation(&ALL_Vehicles, slot, trace_path);
            handle_neighbours(&ALL_Vehicles);
            //cout<<"The location of vehicles has been updated, Current slot = "<<slot<<endl;
            //printVehilces(&ALL_Vehicles);

            logfile<<"Event: Update location! Current slot = "<<slot<<", Car_Number="<<Car_Number<<endl;
            logVehilcesInfo(&ALL_Vehicles, logfile);
            logfile<<endl;
        }


        ve_mac(&ALL_Vehicles, slot);

//        // handle the transmitter
        handle_transmitter(&ALL_Vehicles, slot, logfile);
//
//        // handle the receiver
        handle_receiver(&ALL_Vehicles, slot,logfile);

    }

    printf(" Total Cars: %d\n Total slot:%d \n cnt_pkt_tx: %d\n cnt_pkt_0: %d\n cnt_pkt_1: %d\n cnt_pkt_2: %d\n Receive Rate=%lf \n", Car_Number,slot_end,  cnt_pkt_tx, cnt_pkt_0, cnt_pkt_1, cnt_pkt_2, (double)(cnt_pkt_1)/(cnt_pkt_0+cnt_pkt_1+cnt_pkt_2));

    logfile.close();

    return 0;
}
