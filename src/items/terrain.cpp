#include "terrain.hpp"
#include "../helpers/interpolation.hpp"

using namespace vcl;

buffer<vec3> positions_riveG;
buffer<vec3> positions_riveD;
buffer<vec3> positions_ile;

vcl::mesh initialize_terrain()
{
    int const terrain_sample = 180;
    mesh terrain = mesh_primitive_grid({-10,-10,0},{10,-10,0},{10,10,0},{-10,10,0},terrain_sample,terrain_sample);
    return terrain;
}

vcl::vec3 evaluate_terrain2(float u, float v, vcl::mesh& terrain)
{
    vec3 pos;
    int const N = std::sqrt(terrain.position.size());

    float ku = u*(N - 1.0f);
    float kv = v*(N - 1.0f);
    int const idx = ku*N+kv;

    return terrain.position[idx];
}

void update_terrain(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters, buffer<vec3> *courbes_fleuve)
{
    // Number of samples in each direction (assuming a square grid)
    int const N = std::sqrt(terrain.position.size());

    // Recompute the new vertices
    for (int ku = 0; ku < N; ++ku) {
        for (int kv = 0; kv < N; ++kv) {

            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            int const idx = ku*N+kv;

            // Compute the Perlin noise
            float const noise = noise_perlin({u, v}, parameters.octave, parameters.persistency, parameters.frequency_gain);

            if(is_water(terrain.position[idx].x,terrain.position[idx].y, courbes_fleuve)){
                terrain.position[idx].z = 0;
                // use noise as color value
                terrain.color[idx] = 0.3f*vec3(0,0.0,1.0f)+0.7f*noise*vec3(1,1,1);
            }

            else{
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise;

                // use also the noise as color value
                terrain.color[idx] = 0.3f*vec3(0,0.5f,0)+0.7f*noise*vec3(1,1,1);
            }
        }
    }

    // Update the normal of the mesh structure
    terrain.compute_normal();

    // Update step: Allows to update a mesh_drawable without creating a new one
    terrain_visual.update_position(terrain.position);
    terrain_visual.update_normal(terrain.normal);
    terrain_visual.update_color(terrain.color);

}

bool is_water(float x, float y, buffer<vec3> *courbes_fleuve)
{
    float dt = 0.01;

    for(int i=0; i<3; i++){
        int N = courbes_fleuve[i].size();
        int cpt = 0;
        bool B = false;
        float t = 0;
        if(i != 1){
            while(!B && cpt<N-1){
                if(courbes_fleuve[i][cpt][0] >= x){
                    B = true;
                }
                cpt++;
            }
        }
        else{
            while(!B && cpt<N-1){
                if(courbes_fleuve[i][cpt][1] >= y){
                    B = true;
                }
                cpt++;
            }
        }

        if(B){
            if(cpt == 1) cpt+=1; // on considÃ¨re le seg [1, 2] ie le premier seg car on a double le premier pt (on est tombÃ© pile sur l'abscisse du premier pt de controle)
            else cpt-=1;
            vec3 pprev = cardinal_spline_interpolation(t, 0, 1, 2, 3, courbes_fleuve[i][cpt-2], courbes_fleuve[i][cpt-1], courbes_fleuve[i][cpt], courbes_fleuve[i][cpt+1], 0.5);
            vec3 psuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[i][cpt-2], courbes_fleuve[i][cpt-1], courbes_fleuve[i][cpt], courbes_fleuve[i][cpt+1], 0.5);
            if(i!=1){
                while(psuiv[0] < x && t<1){
                    t += dt;
                    pprev = psuiv;
                    psuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[i][cpt-2], courbes_fleuve[i][cpt-1], courbes_fleuve[i][cpt], courbes_fleuve[i][cpt+1], 0.5);
                }
                float y_interpol = pprev[1] + (psuiv[1] - pprev[1])/(psuiv[0] - pprev[0])*(x - pprev[0]);
                //comparaison y_interpol et v en fonction de i (ie : de la rive)
                if(i == 0 && y > y_interpol) return false;
                if(i == 2 && y < y_interpol) return false;
            }
            else{
                while(psuiv[1] < y && t<1){
                    t += dt;
                    pprev = psuiv;
                    psuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[i][cpt-2], courbes_fleuve[i][cpt-1], courbes_fleuve[i][cpt], courbes_fleuve[i][cpt+1], 0.5);
                }
                float x_interpol = pprev[0] + (psuiv[0] - pprev[0])/(psuiv[1] - pprev[1])*(y - pprev[1]);
                //comparaison x_interpol et u en fonction de i (ie : de la rive)
                if(x > x_interpol) return false;
            }
        }
    }
    return true;
}
