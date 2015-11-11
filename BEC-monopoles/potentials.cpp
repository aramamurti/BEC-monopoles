//
//  potentials.cpp
//  PIMC
//
//  Created by Adith Ramamurti on 5/14/15.
//  Copyright (c) 2015 Adith Ramamurti. All rights reserved.
//

#include "potentials.h"

float Potential_Functions::harmonicPotential(fVector loc, float m, float w){
    float potVal = 0.0;
    for(int i = 0; i < loc.size(); i++){
        potVal += 0.5*m*pow(w,2)*pow(loc[i],2);
    }
    return potVal;
}

float Potential_Functions::lj_int(float dist){
    float eps = 10.22;
    float sig = 2.556;
    return 4*eps*(pow((sig/dist),12) - pow((sig/dist),6));
}

float Potential_Functions::hardSphere(float dist){
    if(dist < 140E-12)
        return 10E20;
    else
        return 0;
}

float Potential_Functions::aziz_int(float dist){
    float val = 10.8*(544850.4 * exp(-4.50018*dist)-(9424.94/pow(dist,10)+2556.63/pow(dist,8)+937.38/pow(dist,6)) *aziz_pcws(dist));
    return val;
}

inline float Potential_Functions::aziz_pcws(float dist){
    if(dist >= 3.68335)
        return 1;
    else
        return exp(-pow((3.68335/dist-1),2));
}

inline float Potential_Functions::real_coulomb(float dist, int chgi, int chgj){
    float real = chgi*chgj * erfc(alpha*dist) / dist;
    return real;
}

inline float Potential_Functions::reci_coulomb(fVector kx, int sfac, float box_size, int chgi, int chgj){
    float kfac = 2* M_PI / box_size;
    float efac = 1;
    float k2 = 0;
    for(fVector::iterator it = kx.begin(); it != kx.end(); it++){
        efac = efac * cos(kfac * *it);
        k2 += pow(kfac * *it,2);
    }
    float reci = sfac/k2*exp(-k2*1/(4*alpha*alpha))*chgi *chgj*efac;
    return reci;
}