//
// Created by cyx02 on 2022/6/27.
//

#ifndef BUBBLEMAC_INFOCOM_COLLISION_H
#define BUBBLEMAC_INFOCOM_COLLISION_H


typedef struct collision{//发生碰撞的时候记录：碰撞类型，slot，两个车及其onehop twohop的使用情况
    int type; //1:tx & tx   2:access_collison 3. merge_collison
    int slot;
    struct vehicle *src, *dst;
    int *src_oneHop;
    int *dst_oneHop;
    int *src_twoHop;
    int *dst_twoHop;
    // int *two_oneHop;
}Collision;

void log_collision(struct collision* coli);
struct collision* generate_collision(struct vehicle *aCar, struct vehicle *bCar,  int type, int slot);



#endif //BUBBLEMAC_INFOCOM_COLLISION_H
