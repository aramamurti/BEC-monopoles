//
//  pimc.cpp
//  PIMC
//
//  Created by Adith Ramamurti on 5/20/15.
//  Copyright (c) 2015 Adith Ramamurti. All rights reserved.
//

#include "moves.h"
#include "paths.h"
#include "pimc.h"
#include <iostream>


pimc::pimc(){
    numacceptc = 0;
    numaccepts = 0;
}

std::vector<double> pimc::run(int numSteps, paths* path, std::vector<bool> pmv){
    std::vector<double> energytr = {};
    
    moves mvs;
    
    for(int step = 0; step < numSteps; step++){

        int ptcl = (int) path->getUte()->randnormed(path->getParam()->getNumParticles())%path->getParam()->getNumParticles();
        if(pmv[0])
            if(mvs.comMove(path, ptcl))
                numacceptc += 1;
        if(pmv[1])
            if(mvs.stagingMoveHelper(path, ptcl))
                numaccepts += 1;
        
        if(step % path->getParam()->getSkip() == 0 && step>path->getParam()->getEquil()){
            energytr.push_back(path->energy());
        }
    }
    
    std::cout << "\nCenter of mass acceptance: "<< 1.0*numacceptc/(numSteps*path->getParam()->getNumParticles()) << "\n" << "Staging acceptance: "<< 1.0*numaccepts/(numSteps*path->getParam()->getNumParticles()) << "\n\n";
    
    return energytr;
}