//
// Created by cyx02 on 2022/7/13.
//
#include <iostream>
#include "Protocol_VeMAC.h"
#include "common.h"
#include "vehicle.h"
#include "packet.h"

/*
 *
对于VeMAC的全新理解：
 1. 什么时候进行决策？
 每个slot都要进行决策，决策的依据是之前听到的一个frame。如果没有听完一个完整的frame，则先听完一个frame再进行决策

 2. 广播半径选择多大？
 300米

 3. 广播时槽怎么选？
如果是第一次做决策，则根据上一帧听到的packets。先生成T(x)，即哪些时槽被实用了。让后生成A(x) = T（x）& R或者T(x) & L。然后在A（x）中随机选择一个时槽，进行申请。
怎么判断申请是否成功了呢？再收听一帧，看是否被所有的邻居都接受，若被所有邻居都接受，则切换为占有状态，否则重新选择。

对于一个处于占有状态的车辆，其必须时刻根据上一帧收听到的packets，来判断是否产生了merging collisions，如果产生了，则需要重新选择时槽。

slot_occupied: 表示占用的时槽
ve_slot_condition: 表示是正在申请还是已经占用


具体程序应该咋写呢？
对于每个时槽来说，
    先看距离出现是否过了一帧，如果没过，则处于单纯收听阶段，不进行时槽选择；（出现的第一帧进行收听）
    若刚好到了出现后的第一帧结束，则申请一个时槽，记录下时槽申请的时间（在这个时间后的一帧内只收听，不决策）

    如果处于时槽申请状态，判断是否已经监听满了一帧，如果没满，则处于单纯收听阶段，不进行时槽选择；
    如果刚好满了一帧，则判断是否得到了所有邻居的认可，如果得到了所有邻居的认可，则切换状态未占用，并保持使用该时槽；
    否则重新申请一个时槽；

    如果处于占用状态，判断刚听到的一个slot内是否有不认可他的节点，如果有，则发生了merging collision；进行时槽的重新选择，记录时槽选择时间
    如果没有不认可他的节点，则继续使用该时槽并保持

*/

using namespace std;

void ve_mac(struct Duallist *ALL_Vehicles, int slot) {
    struct Item * aItem, *bItem;
    struct vehicle* aCar;

    aItem = ALL_Vehicles->head;
    while(aItem != NULL) {
        aCar = (struct vehicle*)aItem->datap;               //[a,b] (rand()%(b-a+1))+a
        aCar->commRadius = 300;

        // 初始化prev_OHN OHN THN
        for(int ii = 0; ii < SlotPerFrame; ii++){
            aCar->prev_OHN[ii] = aCar->OHN[ii];
            aCar->OHN[ii] = nullptr;
            aCar->THN[ii] = nullptr;
        }

        if(slot < aCar->slot_appeared + SlotPerFrame){ //忽略掉刚出现的车
            aItem = aItem->next;
            continue;
        }else if(slot == aCar->slot_appeared + SlotPerFrame){
            //选择一个时槽并且变成申请态
            aCar->slot_occupied = ve_mac_choose_slot(aCar, slot);
            aCar->slot_condition = ACCESS;
            aCar->acceess_timestamp = slot;
            aItem = aItem->next;
            continue;
        }

        if(aCar->slot_condition == ACCESS){
            if(slot < aCar->acceess_timestamp + SlotPerFrame){ //距离上次申请未超过一帧，不处理
                aItem = aItem->next;
                continue;
            }else if(slot == aCar->acceess_timestamp + SlotPerFrame){
                int tmp = ve_mac_choose_slot(aCar, slot);
                //若所有的车辆都认可
                if(aCar->ve_check_flag != 0){
                    aCar->slot_condition = OCCUPIED;
                    aCar->acceess_timestamp = -1;
                    aItem = aItem->next;
                    continue;
                }else{  //若未得到所有车辆的认可
                    aCar->slot_occupied = tmp;
                    aCar->slot_condition = ACCESS;
                    aCar->acceess_timestamp = slot;
                    aItem = aItem->next;
                    continue;
                }
            }else{
                cout<<"error! It should be handled before"<<endl;
            }
        }else if(aCar->slot_condition == OCCUPIED){
            //若刚才的slot没有出现merging collision
            if(isMergingCollision(aCar, slot)){
                aItem = aItem->next;
                continue;
            }else{ //若刚才的slot出现了merging collision
                aCar->slot_occupied = ve_mac_choose_slot(aCar, slot);
                aCar->slot_condition = ACCESS;
                aCar->acceess_timestamp = slot;
                aItem = aItem->next;
                cout<<"Merging Collision!!"<<endl;
                continue;
            }
        }


    }
}

//根据此前的一个frame收到的packets，决定自己的slot
int ve_mac_choose_slot(struct vehicle* aCar, int slot){
    struct Item* bItem =(struct Item *) aCar->packets.head;
    aCar->ve_check_flag = 1;

    while(bItem != NULL){
        struct packet* pkt = (struct packet*) bItem->datap;

        if(pkt->timestamp < slot - SlotPerFrame){
            break;
        }

        if(pkt->condition == 0){         //condition为0的包车辆是听不到的，也解不出
            bItem = bItem->next;
        }else if(pkt->condition == 1){
            int index = (pkt->timestamp)%SlotPerFrame;
            aCar->OHN[index] = pkt->srcVehicle;

            //对于能解出来的包，按照其OHN_snapshot更新自己的THN
            for(int i = 0; i < SlotPerFrame; i++){
                if(pkt->OHN_snapshot[i]!= nullptr)
                    aCar->THN[i] = pkt->OHN_snapshot[i];
            }
            //检测是否全部认可了自己
            if(pkt->OHN_snapshot[aCar->slot_occupied] != aCar)
                aCar->ve_check_flag = 0;

            bItem = bItem->next;
        }else if(pkt->condition == 2){//对于冲突的包，只需要更新OHN为占用即可
            int index = (pkt->timestamp)%SlotPerFrame;
            aCar->OHN[index] = pkt->srcVehicle;
            bItem = bItem->next;
        }
    }


    //THN 没用的时槽里选一个进行推荐
    vector<int> slot_candidate;
    if(aCar->resource_pool == 0){
        for(int ii = 0; ii <= SlotPerFrame/2-1; ii++){
            if(aCar->THN[ii] == nullptr)
                slot_candidate.push_back(ii);
        }
    }else if(aCar->resource_pool == 1){
        for(int ii = SlotPerFrame/2; ii <= SlotPerFrame-1; ii++){
            if(aCar->THN[ii] == nullptr)
                slot_candidate.push_back(ii);
        }
    }else{
        cout<<"Strange resource pool"<<endl;
    }

    int len_candidate = slot_candidate.size();
    int choosed_slot = slot_candidate[rand() % len_candidate];

    return choosed_slot;

}

bool isMergingCollision(struct vehicle* aCar, int slot){
    struct Item* bItem =(struct Item *) aCar->packets.head;

    while(bItem != NULL){
        struct packet* pkt = (struct packet*) bItem->datap;

        if(pkt->timestamp < slot){//只看最近的一个slot即可
            break;
        }

        if(pkt->condition == 0){         //condition为0的包车辆是听不到的，也解不出
            bItem = bItem->next;
        }else if(pkt->condition == 1){
            //检测是否全部认可了自己
            if(pkt->OHN_snapshot[aCar->slot_occupied] != aCar){
                for(int ii =0; ii <SlotPerFrame; ii++){
                    if(aCar->prev_OHN[ii] == pkt->srcVehicle){//这里很关键哈，上一帧的OHN
                        return false;
                    }
                }
            }

            bItem = bItem->next;
        }else if(pkt->condition == 2){//对于冲突的包，只需要更新OHN为占用即可
            bItem = bItem->next;
        }
    }

    return true;
}



