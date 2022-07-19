//
// Created by cyx02 on 2022/6/27.
//

#ifndef BUBBLEMAC_INFOCOM_COMMUNICATION_H
#define BUBBLEMAC_INFOCOM_COMMUNICATION_H

#include <fstream>
using namespace std;

void handle_transmitter(struct Duallist *ALL_Vehicles, int slot, ofstream &logfile);
void handle_receiver(struct Duallist *ALL_Vehicles, int slot,ofstream &logfile);


struct packet * generate_packet(struct vehicle *aCar, struct vehicle *bCar ,int slot, int condition);
void log_collision(struct collision* coli);
void log_packet(struct packet * aPkt, int slot);
bool IsFront(struct vehicle *aCar, struct vehicle *tCar);

#endif //BUBBLEMAC_INFOCOM_COMMUNICATION_H
