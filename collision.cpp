//
// Created by cyx02 on 2022/6/27.
//

#include "collision.h"
#include <cstdio>
#include "parameters.h"
#include "vehicle.h"
#include <cstdlib>

void log_collision(struct collision* coli){
    char output_collisions_path[100];
    FILE * Collisions_output;
    sprintf(output_collisions_path, "./simulation_result/bubble_collision_%d_%d.txt", SlotPerFrame, traffic_density);

    Collisions_output = fopen(output_collisions_path,"a");

    fprintf(Collisions_output, "No:%d, Type: %d, Slot: %d, Distance:%lf\n", cnt_coli++,coli->type, coli->slot, distance_between_vehicle(coli->src, coli->dst));

    //src
    fprintf(Collisions_output,"src ID: %d, x: %lf, y: %lf, comm.R: %lf \n",coli->src->id, coli->src->x, coli->src->y, coli->src->commRadius);
    // for(int i = 0; i< SlotPerFrame; i++){
    //     fprintf(Collisions_output,"%d ",coli->src_oneHop[i]);
    // }
    // fprintf(Collisions_output,"\n");
    // for(int i = 0; i< SlotPerFrame; i++){
    //     fprintf(Collisions_output,"%d ",coli->src_twoHop[i]);
    // }
    // fprintf(Collisions_output,"\n");

    //dst
    fprintf(Collisions_output,"dst ID: %d, x: %lf, y: %lf ,comm.R: %lf \n",coli->dst->id, coli->dst->x, coli->dst->y, coli->dst->commRadius);
    // for(int i = 0; i< SlotPerFrame; i++){
    //     fprintf(Collisions_output,"%d ",coli->dst_oneHop[i]);
    // }
    // fprintf(Collisions_output,"\n");
    // for(int i = 0; i< SlotPerFrame; i++){
    //     fprintf(Collisions_output,"%d ",coli->dst_twoHop[i]);
    // }
    fprintf(Collisions_output,"\n");

    fclose(Collisions_output);

    //释放当前结构体内存
    free(coli->src_oneHop);
    free(coli->src_twoHop);
    free(coli->dst_oneHop);
    free(coli->dst_twoHop);
    free(coli);
}



//生成Collision， 记录type, slot, 两个车及两车对应的当前slot使用情况
struct collision* generate_collision(struct vehicle *aCar, struct vehicle *bCar,  int type, int slot){
    struct collision * coli;
    coli = (struct collision*)malloc(sizeof(struct collision));
    coli->type = type;
    coli->slot = slot;
    coli->src = aCar;
    coli->dst = bCar;

    coli->src_oneHop = (int*)malloc(sizeof(int)*SlotPerFrame);
    coli->src_twoHop = (int*)malloc(sizeof(int)*SlotPerFrame);
    coli->dst_oneHop = (int*)malloc(sizeof(int)*SlotPerFrame);
    coli->dst_twoHop = (int*)malloc(sizeof(int)*SlotPerFrame);

    for(int i = 0; i < SlotPerFrame; i++){   //将时槽占用情况输出出来；如果有占用，则输出1，如果没占用，则输出0；后面可以改成输出ID
        coli->src_oneHop[i] = aCar->OHN[i]==NULL? 0 : 1;
        coli->src_twoHop[i] = aCar->THN[i]==NULL? 0 : 1;
        coli->dst_oneHop[i] = bCar->OHN[i]==NULL? 0 : 1;
        coli->dst_twoHop[i] = bCar->THN[i]==NULL? 0 : 1;
    }

    return coli;
}
