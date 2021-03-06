#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <random>
#include "lib.h"

#define debug 0
#define iterations 25
#define penalty 9999

int get_random(int upper){
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0,upper);
    return distr(eng);
}

bool in(int* indices, int count){
    int i = 0;
    while(indices[i] != NULL){
        if(count == indices[i])
            return true;
        i++;
    }
    return false;
}

point* get_points(map<point, vector<point>> &list, int* indices){
    int count = 0;
    int ret_ind = 0;
    point* ret = new point[2];
    for(map<point,vector<point>>::iterator it = list.begin(); it != list.end(); it++, count++)
        if(in(indices, count)){
            ret[ret_ind] = point(it->first.x, it->first.y);
            ret_ind++;
        }
    return ret;
}

std::vector<point> find(map<point,vector<point>> &list, point p){
    for(map<point,vector<point>>::iterator it = list.begin(); it != list.end(); it++)
        if(it->first.x == p.x && it->first.y == p.y)
            return it->second;
    return std::vector<point>();
}

bool find(set<point> visited, point p){
    for(auto i: visited)
        if(i.x == p.x && i.y == p.y)
            return true;
    return false;
}

// helper to check if path is blocked by any obstacle
bool if_blocked(vector<line> &obs,point &from , point &to){
    for (line &o : obs)
        if (o.is_blocked(from, to) == true)
            return true;
    return false;
}

int dfs(point &src, point &dest, map<point,vector<point>> &list,vector<line> obstacles, set<point> visited, int hops){
    if(src.x == dest.x && src.y == dest.y){
        return hops;
    }
    hops++;
    visited.insert(src);
    std::vector<point> adj = find(list, src);
    for(auto i: adj)
        if(!find(visited, i) && !if_blocked(obstacles,src,i))
            return dfs(i, dest, list, obstacles, visited, hops);
    if(debug) printf("dfs return -1\n");
    return -1;
}

int bfs(point &src, point &dest, map<point,vector<point>> &list,vector<line> obstacles){
    if(debug)
        printf("begin bfs\n");
    queue<point> q1;
    q1.push(src);
    set<point> visited;
    visited.insert(src);
    int res = 0;
    while(!q1.empty()){
        point top = q1.front();
        q1.pop();
        if(top==dest)return res;
        std::vector<point> toplist = find(list, top);
        if(debug) std::cout<<"toplist size "<<toplist.size()<<'\n';
        for(auto nig : toplist){
            if(if_blocked(obstacles,top,nig))
                continue;
            if(!find(visited, nig)){
                q1.push(nig);
                visited.insert(nig);
                if(debug) std::cout<<"q1 size "<<q1.size()<<" visited size "<<visited.size()<<'\n';
            }
        }
        res++;
    }
    return -1;
}

int get_gridVal(std::string filename){
    std::ifstream in(filename);
    std::string line;
    // get max possible coverage
    getline(in, line);
    return atoi(line.c_str());
}

int get_Coverage(std::string filename){
    if(debug) std::cout<<"get coverage " << filename << '\n';
    int total_area = pow(get_gridVal(filename), 2);
    if(debug) std::cout<< "total area "<<total_area<<'\n';
    return 0;
}

float get_Hops(float* hops, map<point,vector<point>> &list,vector<line> obstacles, std::string filename){
    if(debug) printf("get hops\n");
    float avg_hops = 0;
    int listSize = list.size();
    int ind1;
    int ind2;
    for(int i = 0; i < iterations; i++){
        ind1 = get_random(list.size()-1);
        ind2 = get_random(list.size()-1);
        int indices[] = {ind1, ind2};
        if(debug) printf("indices %d %d\n", ind1, ind2);
        if(ind1 == ind2){
            i--;
            continue;
        }
        // set up bfs
        point* ps = new point[2];
        ps = get_points(list, indices);
        int bfs_ = bfs(ps[0], ps[1], list, obstacles);
        // set up dfs
        set<point> visited;
        int dfs_ = dfs(ps[0], ps[1], list, obstacles, visited, 0);

        if(bfs_ == -1){
            avg_hops += penalty;
            if(debug) printf("no connection\n");
            i--;
            continue;
        }
        avg_hops += (bfs_ + 2*dfs_) / 3;
        if(debug) printf("output of bfs is %u\noutput of dfs is %u\navg_hops is %f\niteration %d\n", bfs_, dfs_, avg_hops, i);
    }
    *hops = avg_hops / iterations;
    if(debug) std::cout<<"hops "<<*hops<<' '<<hops<<'\n';
    return *hops;
}