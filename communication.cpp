//
// Created by cyx02 on 2022/6/27.
//

#include "communication.h"
#include "stddef.h"
#include "vehicle.h"
#include "parameters.h"
#include "packet.h"
#include "collision.h"
#include <iostream>
#include <cstring>

using namespace std;


// transmit packets. For each vehicle, if it is a tx, it broadcast to neighbors within its communication range. If the rx also is a tx, there will be a collision, if not  it will receive the packet.
void handle_transmitter(struct Duallist *ALL_Vehicles, int slot){
    struct Item *aItem, *bItem;
    struct vehicle *aCar, *bCar;

    aItem = ALL_Vehicles->head;
    while(aItem != NULL){
        aCar = (struct vehicle*)aItem->datap;

        if(aCar->slot_appeared + 200 > slot){
            //logfile << aCar->id<<" is a new car and do not broadcast until slot ="<<aCar->slot_appeared + SlotPerFrame<<", Current Slot="<<slot<<endl;
            aItem = aItem->next;
            continue;
        }

        //若车辆占用时槽并非当前时槽，则跳过(略过所有的receiver)
        if(aCar->slot_occupied != slot % SlotPerFrame){
            //logfile << aCar->id<<" is silent in slot ="<<slot<<endl;
            aItem = aItem->next;
            continue;
        }

        // cout<<"Current Slot:"<<slot<<"Current Transmitter: "<<aCar->id<<endl;//对当前时槽正好发射的节点进行操作
        cnt_pkt_tx++;
        aCar->counter_tx++;

//        logfile <<"Event: "<< aCar->id<<" is transmitting in slot ="<<slot<<" ";
//        logACar(aCar);
//        logfile<<endl;
        int flag_tx_normal = true;//正常发包标志


        bItem = (struct Item*)aCar->neighbours.head;//遍历当前transmitter的邻居节点
        while(bItem != NULL){
            bCar = (struct vehicle*)bItem->datap;

            //printf("bCar Id: %s\n", bCar->id);
            double distanceAB = distance_between_vehicle(aCar, bCar);
            if(aCar->commRadius < distanceAB){
                //printf("%d 's Comm Range is: %lf, OutRange neighbor is %d, distance is %lf\n",aCar->id,  aCar->commRadius, bCar->id,distanceAB);
                bItem = bItem->next;
            }else{
                //printf("%d 's Comm Range is: %lf, InRange neighbor is %d, distance is %lf\n", aCar->id,aCar->commRadius, bCar->id, distanceAB);
                if(bCar->slot_occupied == aCar->slot_occupied){     //若此时目标车辆也在发送，则产生collision
//                    logfile << "      Receiver "<<bCar->id<<" is also transmitting in slot ="<<slot<<", Distance is"<<distanceAB<<" ";
//                    logACar(bCar, logfile);
//                    logfile<<endl;
                    flag_tx_normal = false;
                    aCar->counter_rx_TxCollision++;
                    cnt_tx_collision++;
                    struct packet* pkt = generate_packet(aCar,bCar,slot,TX_COLI);
                    cout<<"timestamp = "<<pkt->timestamp<< ",there is a TX collision, pkt->src = "<<pkt->srcVehicle->id<<" pkt->dst = "<< pkt->dstVehicle->id<<"src->slot="<<pkt->srcVehicle->slot_occupied <<" dst->slot= "<<pkt->dstVehicle->slot_occupied<<" src->commRange="<< pkt->srcVehicle->commRadius<<" dst->commRange="<< pkt->dstVehicle->commRadius<<endl;
                    bCar->packets->push_back(pkt);
                    //printf("A packet! cnt_pkt: %d, src: %s, dst:%s ,slot:%d, condition:%d \n", cnt_pkt, aCar->id, bCar->id,slot,pkt->condition);
                    log_packet(pkt, slot, log_process_file);

                    struct collision* coli =  generate_collision(aCar,bCar,0,slot);
                    //log_collision(coli);
                    bItem = bItem->next;
                }else{//正常收包
//                    logfile << "      Receiver "<<bCar->id<<" is also listening in slot ="<<slot<<", Distance is"<<distanceAB<<" ";
//                    logACar(bCar, logfile);
//                    logfile<<endl;

                    struct packet* pkt = generate_packet(aCar,bCar,slot,NO_COLI);
                    //duallist_add_to_head(&(bCar->packets), pkt);
                    bCar->packets->push_back(pkt);

                    //printf("A packet! cnt_pkt: %d, src: %s, dst:%s ,slot:%d, condition:%d \n", cnt_pkt, aCar->id, bCar->id,slot,pkt->condition);
                    //log_packet(pkt,slot);
                    bItem = bItem->next;
                }
            }
        }

        if(flag_tx_normal){
            cnt_pkt_tx_normal++;
        }
        aItem =aItem->next;
    }
}


void handle_receiver(struct Duallist *ALL_Vehicles, int slot){
    struct Item *aItem, *bItem;
    struct vehicle *aCar;

    aItem = ALL_Vehicles->head;
    while(aItem != NULL){
        aCar = (struct vehicle*)aItem->datap;

        if(aCar->slot_occupied == slot%SlotPerFrame) {
            aItem = aItem->next;
            continue; //忽略掉transmitter
        }

        //printf("Current Slot: %d, Current Receiver: %d\n", slot, aCar->id);//对当前时槽正好发射的节点进行操作
        //到目前时间一直都没有收到包
        if(aCar->packets->size() == 0){
            aItem = aItem->next;
            continue;
        }

        //数一下当前slot收到了多少个包---->packet只有当一个完整的frame才刷新
        int cnt_cur_pkt = 0;

        int len = (*(aCar->packets)).size();
        for(int ii = len-1; ii >=0; ii--){
            struct packet *pkt = (struct packet*) (*(aCar->packets))[ii];
            if(pkt->timestamp != slot)//仅统计当前slot的pkt
                break;
            else
                cnt_cur_pkt++;
        }


        if(cnt_cur_pkt == 1){

            struct packet* pkt= (*(aCar->packets))[len-1];
            pkt->condition = NO_COLI;
            log_packet(pkt, slot, log_process_file);
            cout<<"timestamp = "<<pkt->timestamp<< ",there is normal packet, pkt->src = "<<pkt->srcVehicle->id<<" pkt->dst = "<< pkt->dstVehicle->id<<"src->slot="<<pkt->srcVehicle->slot_occupied <<" dst->slot= "<<pkt->dstVehicle->slot_occupied<<" src->commRange="<< pkt->srcVehicle->commRadius<<" dst->commRange="<< pkt->dstVehicle->commRadius<<endl;

            aCar->counter_rx_normal++;
            cnt_rx_normal++;
            if(strcmp(pkt->srcVehicle->lane, aCar->lane)==0 && pkt->srcVehicle->pos > aCar->pos)
                cnt_frontV_normal++;
            if(strcmp(pkt->srcVehicle->lane, aCar->lane)==0 && pkt->srcVehicle->pos < aCar->pos)
                cnt_rearV_normal++;

        }else if(cnt_cur_pkt >=2 ){     //产生碰撞的包
            cnt_rx_colli += cnt_cur_pkt;
            aCar->counter_rx_RxCollision += cnt_cur_pkt;
            // cout<<"asdasdasd----------------"<<endl;

            for(int ii = len-1; ii >=len-cnt_cur_pkt; ii--){
                struct packet *pkt = (struct packet*) (*(aCar->packets))[ii];
                pkt->condition = RX_COLI;
                log_packet(pkt,slot, log_process_file);
                if(pkt->timestamp == slot){
                    cout<<"timestamp = "<<pkt->timestamp<< ",there is a RX collision, pkt->src = "<<pkt->srcVehicle->id<<" pkt->dst = "<< pkt->dstVehicle->id<<"src->slot="<<pkt->srcVehicle->slot_occupied <<" dst->slot= "<<pkt->dstVehicle->slot_occupied<<" src->commRange="<< pkt->srcVehicle->commRadius<<" dst->commRange="<< pkt->dstVehicle->commRadius<<endl;
                }
                if(strcmp(pkt->srcVehicle->lane, aCar->lane)==0 && pkt->srcVehicle->pos > aCar->pos)
                    cnt_frontV_colli++;
                if(strcmp(pkt->srcVehicle->lane, aCar->lane)==0 && pkt->srcVehicle->pos < aCar->pos)
                    cnt_rearV_Colli++;
            }
        }
        //printf("hello\n");
        aItem = aItem->next;
    }
}