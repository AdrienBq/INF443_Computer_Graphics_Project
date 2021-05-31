
#include "corde.hpp"

using namespace vcl;


int NbrSpring = 10;
float const m  = 0.1f;        // particle mass
float const mu = 0.01f;       // damping coefficient
vcl::vec3 const g   = {0,0,-9.81f}; // gravity


vcl::vec3 spring_force(vcl::vec3 const& p_i, vcl::vec3 const& p_j, float L_0, float K)
{
    float L = sqrt((p_i[0] - p_j[0])*(p_i[0] - p_j[0])
            + (p_i[1] - p_j[1])*(p_i[1] - p_j[1]) + (p_i[2] - p_j[2])*(p_i[2] - p_j[2]));
    vec3 U = p_i - p_j;
    vec3 u = U/L;
    vec3 f = -K*(L - L_0)*u;
    return f;
}

void update_pos_rope(vcl::vec3 pos_bateau, vcl::buffer<vcl::vec3>& particules, vcl::buffer<vcl::vec3>& vitesses, vcl::buffer<float>& L0_array, vcl::buffer<float>& raideurs, vcl::mesh& terrain, float dt)
{
    // Forces
    buffer<vec3> forces;
    forces = {};
    forces.push_back({0,0,0});
    for(int i=1; i<NbrSpring-1; i++){
        vec3 pprece = particules[i-1];
        vec3 psuiv = particules[i+1];
        vec3 pcourant = particules[i];
        vec3 v = vitesses[i];
        vec3 const f_spring  = spring_force(pcourant, pprece, L0_array[i-1], raideurs[i-1])
                + spring_force(pcourant, psuiv, L0_array[i], raideurs[i]);
        vec3 const f_weight  =  m * g;
        vec3 const f_damping =  -mu*v;
        vec3 const f = f_spring + f_weight + f_damping;
        forces.push_back(f);
    }
    forces.push_back({0,0,0});

    // Numerical Integration (Verlet)

    // update particules positions

    particules[NbrSpring-1] = pos_bateau;
    for(int i=1; i<NbrSpring-1; i++){
        vitesses[i] = (1-mu)*vitesses[i] + dt * forces[i] / m;
        particules[i] = particules[i] + dt * vitesses[i];
        float u = 0.5f + particules[i][0]/20;
        float v = 0.5f + particules[i][1]/20;
        float h = evaluate_terrain2(u,v,terrain)[2];
        if(h>particules[i][2]) {
            particules[i][2] = h;
            vitesses[i][2] = - vitesses[i][2]*0.7;
        }
    }
}

void initialize_corde(vcl::vec3 pos_bateau, vcl::vec3& pos_poteau, vcl::buffer<vcl::vec3>& particules, vcl::buffer<vcl::vec3>& vitesses, vcl::buffer<float>& L0_array, vcl::buffer<float>& raideurs)
{
    particules = {};
    particules.push_back(pos_poteau);
    vitesses.push_back({0,0,0});
    for(int i=1; i<NbrSpring-1; i++){
        vec3 part = pos_poteau +(pos_bateau - pos_poteau)*i/NbrSpring;
        particules.push_back(part);
        vitesses.push_back({0,0,0});
    }
    particules.push_back(pos_bateau);
    vitesses.push_back({0,0,0});

    L0_array = {};
    for(int i=0; i<NbrSpring; i++){
        L0_array.push_back(std::sqrt((pos_bateau[0]-pos_poteau[0])*(pos_bateau[0]-pos_poteau[0])
                + (pos_bateau[1]-pos_poteau[1])*(pos_bateau[1]-pos_poteau[1])
                + (pos_bateau[2]-pos_poteau[2])*(pos_bateau[2]-pos_poteau[2]))
                /NbrSpring);
    }

    raideurs = {};
    for(int i=0; i<NbrSpring; i++){
        raideurs.push_back(0.01);

    }
}




