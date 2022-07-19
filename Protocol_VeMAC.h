//
// Created by cyx02 on 2022/7/13.
//

#ifndef VEMAC_INFOCOM_PROTOCOL_VEMAC_H
#define VEMAC_INFOCOM_PROTOCOL_VEMAC_H

void ve_mac(struct Duallist *ALL_Vehicles, int slot);

int ve_mac_choose_slot(struct vehicle* aCar, int slot);
bool isMergingCollision(struct vehicle* aCar, int slot);



#endif //VEMAC_INFOCOM_PROTOCOL_VEMAC_H
