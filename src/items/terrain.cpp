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

void update_terrain(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if(is_water(u,v)){
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

bool is_water(float u, float v)
{
    positions_riveG = { {0,0.14,0}, {0,0.14,0}, {0.17,0.33,0}, {0.22,0.49,0}, {0.33,0.57,0}, {0.43,0.81,0}, {0.65,0.94,0}, {1,0.97,0}, {1,0.97,0} };
    positions_ile = { {0.1,0,0}, {0.1,0,0}, {0.28,0.18,0}, {0.44,0.17,0}, {0.46,0,0}, {0.46,0,0} };
    positions_riveD = { {1,0.07,0}, {1,0.07,0}, {0.73,0.17,0}, {0.6,0.37,0}, {0.53,0.53,0}, {0.56,0.62,0}, {0.72,0.70,0}, {1,0.73,0}, {1,0.73,0} };
    buffer<vec3> *courbes_fleuve = new buffer<vec3>[3];
    courbes_fleuve[0] = positions_riveG; // ranges abscisses croissants (ordre des points de la courbe)
    courbes_fleuve[1] = positions_riveD; // ranges abscisses decroissants
    courbes_fleuve[2] = positions_ile; // ranges abscisses croissants
    float dt = 0.1;

    for(int i=0; i<2; i++){
        int N = courbes_fleuve[i].size();
        int cpt = 0;
        bool B = false;
        float t = 0;
        if(i != 1){
            while(!B && cpt<N){
                if(courbes_fleuve[0][cpt][0] >= u){
                    B = true;
                }
                cpt++;
            }
        }
        else{
            while(!B && cpt<N){
                if(courbes_fleuve[0][cpt][1] >= v){
                    B = true;
                }
                cpt++;
            }
        }

        if(B){
            if(cpt == 1) cpt+=1; // on considère le seg [cpt, cpt+1] (on est tombé pile sur une abscisse d ept de controle)
            if(cpt == N-1) cpt-=1; // on considère le seg [cpt-2, cpt-1]
            float xprev = cardinal_spline_interpolation(t, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[0];
            float yprev = cardinal_spline_interpolation(t, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[1];
            float xsuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[0];
            float ysuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[1];
            if(i!=1){
                while(xsuiv < u){
                    t += dt;
                    xprev = xsuiv;
                    yprev = ysuiv;
                    xsuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[0];
                    ysuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[1];
                }
                float y_interpol = yprev + (ysuiv - yprev)*(u - xprev)/(xprev - xsuiv);
                //comparaison y_interpol et v en fonction de i (ie : de la rive)
                if(i == 0 && v > y_interpol) return false;
                if(i == 2 && v > y_interpol) return false;
            }
            else{
                while(ysuiv < v){
                    t += dt;
                    xprev = xsuiv;
                    yprev = ysuiv;
                    xsuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[0];
                    ysuiv = cardinal_spline_interpolation(t+dt, 0, 1, 2, 3, courbes_fleuve[0][cpt-2], courbes_fleuve[0][cpt-1], courbes_fleuve[0][cpt], courbes_fleuve[0][cpt+1], 0.5)[1];
                }
                float x_interpol = xprev + (xsuiv - xprev)*(v - yprev)/(yprev - ysuiv);
                //comparaison y_interpol et v en fonction de i (ie : de la rive)
                if(u > x_interpol) return false;
            }
        }
    }
    return true;
}


