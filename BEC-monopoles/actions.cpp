//
//  actions.cpp
//  BEC-monopoles
//
//  Created by Adith Ramamurti on 11/12/15.
//  Copyright © 2015 Adith Ramamurti. All rights reserved.
//

#include "actions.hpp"

double Potential_Action::potential_helper(int slice, int ptcl){
    double pe = 0;
    int num_particles = path->get_beads()->get_num_particles();
    
    for(std::vector<boost::shared_ptr<Potential_Functions> >::iterator it = pot_funcs.begin(); it != pot_funcs.end(); it++){
        switch(potentials[it-pot_funcs.begin()]){
            case 0:
                pe += (*it)->potential_value(path->get_beads()->get_bead_data(ptcl, slice));
                break;
            case 1:
            case 2:
                for(int i = ptcl+1; i < num_particles; i++){
                    dVector distvec = path->get_beads()->get_path_separation(ptcl, i, slice);
                    double dist = sqrt(inner_product(distvec.begin(), distvec.end(),distvec.begin(), 0.0));
                    pe += (*it)->potential_value(dist);
                }
                break;
            case 3:
                break;
        }
    }
    
    return pe;
}

double Potential_Action::potential_helper_worm(int slice,int start_omit = 0, int end_omit = 0){
    double pe = 0;
    int num_particles = path->get_beads()->get_num_particles();
    
    for(std::vector<boost::shared_ptr<Potential_Functions> >::iterator it = pot_funcs.begin(); it != pot_funcs.end(); it++){
        switch(potentials[it-pot_funcs.begin()]){
            case 0:
                break;
            case 1:
            case 2:
                if(path->worm_exists()){
                    std::vector<std::pair<int, int> > ht = path->get_beads()->get_worm_indices();
                    int worm_end_row = ht[1].first;
                    
                    int worm_start_col = (ht[0].second+start_omit)%path->get_parameters()->get_num_timeslices();
                    int worm_end_col = (ht[0].second-end_omit+path->get_parameters()->get_num_timeslices())%path->get_parameters()->get_num_timeslices();
                    
                    iVector rel_worm_rows;
                    
                    int cur_row = 0;
                    if(worm_start_col > slice)
                        cur_row++;
                    while(cur_row < worm_end_row){
                        rel_worm_rows.push_back(cur_row);
                        cur_row++;
                    }
                    if(worm_end_col >= slice)
                        rel_worm_rows.push_back(cur_row);
                    for(iVector::iterator worm_it = rel_worm_rows.begin(); worm_it != rel_worm_rows.end(); worm_it++){
                        for(int i = 0; i < num_particles; i++){
                            dVector distvec = path->get_beads()->get_worm_path_separation(i, *worm_it, slice);
                            double dist = sqrt(inner_product(distvec.begin(), distvec.end(),distvec.begin(), 0.0));
                            pe += (*it)->potential_value(dist);
                        }
                        for(iVector::iterator worm_it2 = worm_it+1; worm_it2 != rel_worm_rows.end(); worm_it2++){
                            dVector distvec = path->get_beads()->get_worm_separation(*worm_it, *worm_it2, slice);
                            double dist = sqrt(inner_product(distvec.begin(), distvec.end(),distvec.begin(), 0.0));
                            pe += (*it)->potential_value(dist);
                        }
                    }
                }
                break;
            case 3:
                break;
        }
    }
    
    return pe;
}

double Potential_Action::get_action(int slice, int dist, bool only_worm, int start_omit, int end_omit){
    double pot = 0;
    int num_particles = path->get_beads()->get_num_particles();
    if(!only_worm){
        for(int ptcl = 0; ptcl < num_particles; ptcl++){
            pot += potential_helper(slice, ptcl);
        }
    }
    pot += potential_helper_worm(slice, start_omit, end_omit);
    
    return path->get_parameters()->get_tau()*pot;
}

double Potential_Action::get_action_single_particle(int ptcl, int slice){
    double pot = 0;
    int num_particles = path->get_beads()->get_num_particles();
    for(std::vector<boost::shared_ptr<Potential_Functions> >::iterator it = pot_funcs.begin(); it != pot_funcs.end(); it++){
        switch(potentials[it-pot_funcs.begin()]){
            case 0:
                pot += (*it)->potential_value(path->get_beads()->get_bead_data(ptcl, slice));
                break;
            case 1:
            case 2:
                for(int i = 0; i < num_particles; i++)
                    if(i != ptcl){
                        dVector distvec = path->get_beads()->get_path_separation(ptcl, i, slice);
                        double dist = sqrt(inner_product(distvec.begin(), distvec.end(),distvec.begin(), 0.0));
                        pot += (*it)->potential_value(dist);
                    }
                if(path->worm_exists()){
                    std::vector<std::pair<int, int> > ht = path->get_beads()->get_worm_indices();
                    int worm_end_row = ht[1].first;
                    
                    int worm_start_col = (ht[0].second)%path->get_parameters()->get_num_timeslices();
                    int worm_end_col = (ht[0].second+path->get_parameters()->get_num_timeslices())%path->get_parameters()->get_num_timeslices();
                    
                    iVector rel_worm_rows;
                    
                    int cur_row = 0;
                    if(worm_start_col > slice)
                        cur_row++;
                    while(cur_row < worm_end_row){
                        rel_worm_rows.push_back(cur_row);
                        cur_row++;
                    }
                    if(worm_end_col >= slice)
                        rel_worm_rows.push_back(cur_row);
                    for(iVector::iterator worm_it = rel_worm_rows.begin(); worm_it != rel_worm_rows.end(); worm_it++){
                        dVector distvec = path->get_beads()->get_worm_path_separation(ptcl, *worm_it, slice);
                        double dist = sqrt(inner_product(distvec.begin(), distvec.end(),distvec.begin(), 0.0));
                        pot += (*it)->potential_value(dist);
                    }
                }
                break;
            case 3:
                break;
        }
    }
    
    
    return path->get_parameters()->get_tau()*pot;
}

double Kinetic_Action::get_action(int slice, int dist){
    double kin = 0;
    int num_particles = path->get_beads()->get_num_particles();
    
    for(int ptcl = 0; ptcl < num_particles; ptcl++){
        ddVector pair = path->get_beads()->get_pair_same_path(ptcl, slice, dist);
        dVector distVec = utility->dist(pair, path->get_parameters()->get_box_size());
        double ipdist =  inner_product(distVec.begin(),distVec.end(),distVec.begin(),0.0);
        kin += norm/dist*ipdist;
    }
    return kin;
}

double Kinetic_Action::get_action_single_particle(int ptcl, int slice, int dist){
    double kin = 0;    
    ddVector pair = path->get_beads()->get_pair_same_path(ptcl, slice, dist);
    dVector distVec = utility->dist(pair, path->get_parameters()->get_box_size());
    double ipdist =  inner_product(distVec.begin(),distVec.end(),distVec.begin(),0.0);
    kin += norm/dist*ipdist;
    return kin;
}

double Kinetic_Action::get_action_worm_head_tail(int head_col, int tail_col, int dist){
    double kin = 0;
    
    dVector head = path->get_beads()->get_worm_bead_data(0, head_col);
    dVector tail = path->get_beads()->get_worm_bead_data(path->get_beads()->get_worm_dims()[1], tail_col);
    ddVector pair;
    pair.push_back(head);
    pair.push_back(tail);
    
    dVector distVec = utility->dist(pair, path->get_parameters()->get_box_size());
    double ipdist =  inner_product(distVec.begin(),distVec.end(),distVec.begin(),0.0);
    kin += norm/dist*ipdist;
    return kin;
}

double Kinetic_Action::get_action_pos(ddVector pair, int dist){
    double kin = 0;
    dVector distVec = utility->dist(pair, path->get_parameters()->get_box_size());
    double ipdist =  inner_product(distVec.begin(),distVec.end(),distVec.begin(),0.0);
    kin += norm/dist*ipdist;
    return kin;
}