#include "terrain.hpp"
#include "../helpers/interpolation.hpp"

using namespace vcl;

/*
buffer<vec3> positions_riveG = { {-10.0f,-7.2f,0.0f}, {-10.0f,-7.2f,0.0f}, {-6.6f,-3.4f,0.0f}, {-5.6f,-0.2f,0.0f}, {-5.4f,1.4f,0.0f}, {1.3f,6.2f,0.0f}, {6.5f,8.8f,0.0f}, {10.0f,9.4f,0.0f}, {10.0f,9.4f,0.0f} };
buffer<vec3> positions_ile = { {-7.0f,-10.0f,0.0f}, {-7.0f,-10.0f,0.0f}, {-4.6f,-6.4f,0.0f}, {-1.42f,-6.6f,0.0f}, {1.5f,-10.0f,0.0f}, {1.5f,-10.0f,0.0f} };
buffer<vec3> positions_riveD = { {10.0f,-8.6f,0.0f}, {10.0f,-8.6f,0.0f}, {6.6f,-6.6f,0.0f}, {4.0f,-2.6f,0.0f}, {2.6f,0.6f,0.0f}, {3.2f,2.4f,0.0f}, {6.4f,4.0f,0.0f}, {10.0f,4.6f,0.0f}, {10.0f,4.6f,0.0f} };
buffer<vec3> *courbes_fleuve = new buffer<vec3>[3];
courbes_fleuve[0] = positions_riveG; // ranges abscisses croissants (ordre des points de la courbe)
courbes_fleuve[1] = positions_riveD; // ranges abscisses decroissants
courbes_fleuve[2] = positions_ile; // ranges abscisses croissants
**/

float taille_berge1 = 0.2;
float taille_berge2 = 0.3;
float taille_berge3 = 0.5;

perlin_noise_parameters parameters;

perlin_noise_parameters get_noise_params()
{
    return parameters;
}

vcl::mesh initialize_terrain()
{
    int const terrain_sample = 100;
    mesh terrain = mesh_primitive_grid({-8,-15,0},{8,-15,0},{8,15,0},{-8,15,0},terrain_sample,15/8*terrain_sample);
    return terrain;
}

mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const unsigned int N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.uv.resize(N*N);

    // Fill terrain geometry
    for(unsigned int ku=0; ku<N; ++ku)
    {
        for(unsigned int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute the local surface function
            vec3 const p = evaluate_terrain(u,v);

            // Store vertex coordinates
            terrain.position[kv+N*ku] = p;
            terrain.uv[kv+N*ku] = {8*u,15*v};
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(size_t ku=0; ku<N-1; ++ku)
    {
        for(size_t kv=0; kv<N-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+N, idx+1};
            const uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    terrain.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain;
}

vcl::vec3 evaluate_terrain2(float u, float v, vcl::mesh& terrain)   //renvoie la position du mesh aux coordonnees reduits (u,v)
{
    vec3 pos;
    int const N = std::sqrt(terrain.position.size());

    float ku = u*(N - 1.0f);
    float kv = v*(N - 1.0f);
    int const idx = ku*N+kv;

    return terrain.position[idx];
}

vcl::vec3 evaluate_terrain(float u, float v)
{
    float x = (u-0.5f)*16;
    float y = (v-0.5f)*30;
    float z = 0.0f;
    return {x,y,z};
}

void update_terrain(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual1, vcl::mesh_drawable& terrain_visual2, vcl::mesh_drawable& terrain_visual3, vcl::mesh_drawable& terrain_visual4, vcl::mesh_drawable& terrain_visual5, vcl::mesh_drawable& terrain_visual6, perlin_noise_parameters const& parameters, float t, float tmax)
{
    update_terrain_herbe(terrain, terrain_visual1, parameters);
    update_terrain_rive_droite(terrain, terrain_visual1, parameters);
    update_terrain_berge_bas(terrain, terrain_visual2, parameters);
    update_terrain_berge_milieu(terrain, terrain_visual3, parameters);
    update_terrain_berge_haut(terrain, terrain_visual4, parameters);
    update_terrain_dune(terrain, terrain_visual5, parameters);
    update_terrain_water(terrain, terrain_visual6, parameters, t, tmax);
}
void update_terrain2(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual1, vcl::mesh_drawable& terrain_visual2, perlin_noise_parameters const& parameters, float t, float tmax)
{
    update_terrain_not_water(terrain, terrain_visual1, parameters);
    update_terrain_water(terrain, terrain_visual2, parameters, t, tmax);
}

void update_terrain_not_water(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if (!is_water(terrain.position[idx].x,terrain.position[idx].y)
                    && is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*0.3;
                // use noise as color value
                terrain.color[idx] = vec3(0.31f,0.17f,0.04f)+0.5f*noise*vec3(1,1,1);
            }
            else if (!is_water(terrain.position[idx].x,terrain.position[idx].y)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                    && is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*0.6;
                // use noise as color value
                terrain.color[idx] = vec3(0.34f,0.16f,0.0f)+0.5f*noise*vec3(1,1,1);
            }
            if (!is_water(terrain.position[idx].x,terrain.position[idx].y)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)
                    && is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge3)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise;
                // use noise as color value
                terrain.color[idx] = vec3(0.34f,0.16f,0.0f)+0.5f*noise*vec3(1,1,1);
            }
            else if(!is_water(terrain.position[idx].x,terrain.position[idx].y)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge3)
                     && !is_dune(terrain.position[idx].x,terrain.position[idx].y)
                     && !is_rive_droite(terrain.position[idx].x,terrain.position[idx].y) )
            {
                // use also the noise as color value
                terrain.color[idx] = 0.3f*vec3(0,0.5f,0)+0.7f*noise*vec3(1,1,1);
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise + evaluate_dune(terrain.position[idx].x,terrain.position[idx].y, parameters.terrain_height);
            }

            else if(!is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge3)
                    && is_rive_droite(terrain.position[idx].x,terrain.position[idx].y) )
           {
               // use also the noise as color value
               terrain.color[idx] = 0.3f*vec3(0.76f,0.7f,0.5f)+0.7f*noise*vec3(1,1,1);
               // use the noise as height value
               terrain.position[idx].z = parameters.terrain_height*noise + evaluate_dune(terrain.position[idx].x,terrain.position[idx].y, parameters.terrain_height);
           }

            else if (is_dune(terrain.position[idx].x,terrain.position[idx].y)){
                // use also the noise as color value
                terrain.color[idx] = vec3(0.87,0.70,0.5)+0.5f*noise*vec3(1,1,1);
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*
                        std::exp(-(terrain.position[idx].y-dune(terrain.position[idx].x))*(terrain.position[idx].y-dune(terrain.position[idx].x)))
                        + evaluate_dune(terrain.position[idx].x,terrain.position[idx].y, parameters.terrain_height);
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

void update_terrain_water(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters, float t, float tmax)
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
            //float const noise1 = noise_perlin({u, v}, parameters.octave, parameters.persistency, parameters.frequency_gain);
            float const noise2 = noise_perlin({u, v}, 6.0f, 0.6f, 2.25 - 0.3*sin(pi/2 + pi*t/tmax));

            if(is_water(terrain.position[idx].x,terrain.position[idx].y)){
                terrain.position[idx].z = parameters.terrain_height*0.2f*noise2;
                // use noise as color value
                //terrain.color[idx] = 0.3f*vec3(0,0.0,1.0f);
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

void update_terrain_berge_bas(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if (!is_water(terrain.position[idx].x,terrain.position[idx].y)
                    && is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*0.3;
                // use noise as color value
                terrain.color[idx] = vec3(0.31f,0.17f,0.04f)+0.5f*noise*vec3(1,1,1);
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

void update_terrain_berge_milieu(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if (!is_water(terrain.position[idx].x,terrain.position[idx].y)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                    && is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*0.6;
                // use noise as color value
                terrain.color[idx] = vec3(0.34f,0.16f,0.0f)+0.5f*noise*vec3(1,1,1);
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

void update_terrain_berge_haut(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if (!is_water(terrain.position[idx].x,terrain.position[idx].y)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                    && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)
                    && is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge3)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise;
                // use noise as color value
                terrain.color[idx] = vec3(0.34f,0.16f,0.0f)+0.5f*noise*vec3(1,1,1);
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

void update_terrain_herbe(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if(!is_water(terrain.position[idx].x,terrain.position[idx].y)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge3)
                     && !is_dune(terrain.position[idx].x,terrain.position[idx].y)
                     && !is_rive_droite(terrain.position[idx].x,terrain.position[idx].y) )
            {
                // use also the noise as color value
                terrain.color[idx] = 0.3f*vec3(0,0.5f,0)+0.7f*noise*vec3(1,1,1);
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise + evaluate_dune(terrain.position[idx].x,terrain.position[idx].y, parameters.terrain_height);
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

void update_terrain_rive_droite(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if(!is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)
                     && !is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge3)
                     && is_rive_droite(terrain.position[idx].x,terrain.position[idx].y) )
            {
                // use also the noise as color value
                terrain.color[idx] = 0.3f*vec3(0.76f,0.7f,0.5f)+0.7f*noise*vec3(1,1,1);
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise + evaluate_dune(terrain.position[idx].x,terrain.position[idx].y, parameters.terrain_height);
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

void update_terrain_dune(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
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

            if (is_dune(terrain.position[idx].x,terrain.position[idx].y)){
                // use also the noise as color value
                terrain.color[idx] = vec3(0.87,0.70,0.5)+0.5f*noise*vec3(1,1,1);
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*
                        std::exp(-(terrain.position[idx].y-dune(terrain.position[idx].x))*(terrain.position[idx].y-dune(terrain.position[idx].x)))
                        + evaluate_dune(terrain.position[idx].x,terrain.position[idx].y, parameters.terrain_height);
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


float evaluate_dune(float x, float y, float height_param)
{
    buffer<vec2> bornes = { {-8,-1}, {-8,-2}, {-2.5,7}, {2,8}};
    float *possible_heights = new float[4];
    float dist;
    float height;
    float sig = 2.0f;
    for(int i=0; i<4; i++){
        if(x<bornes[i][0]) {
            dist = y - dunes(bornes[i][0])[i];
            //height = heights_dunes(bornes[i][0])[i]*height_param;
        }
        else if(x>bornes[i][1]) {
            dist = y - dunes(bornes[i][1])[i];
            //height = heights_dunes(bornes[i][1])[i]*height_param;
        }
        else {
            dist = y - dunes(x)[i];
            //height = heights_dunes(x)[i]*height_param;
        }
        height = 2*height_param;
        float d = dist*dist/sig;
        if(dist<0) possible_heights[i] = 1/((dist-1/std::sqrt(height))*(dist-1/std::sqrt(height)));
        else possible_heights[i] = height*std::exp(-d*d);
    }
    float max = possible_heights[0];
    for(int i=1; i<4; i++){
        if(max < possible_heights[i]) max = possible_heights[i];
    }
    return max;
}

float rive_gauche(float y)
{
    return -2.302 + y*(-0.566 + y*(-0.0496 + y*(0.017 + y*0.0011)));
}
float rive_gauche_d(float y)
{
    return -0.566 + y*(-0.0496*2 + y*(0.017*3 + y*0.0011*4));
}

float ile(float x)
{
    return -13.578 + x*(-2.5778 + x*(-0.5222 - x*0.0222));
}
float ile_d(float x)
{
    return -2.5778 + x*(-0.5222*2 - x*0.0222*3);
}

float rive_droite(float y)
{
    return 2.599 + y*(0.1762 + y*(-0.032 + y*(0.0258 + y*(0.006 + y*0.0002))));
}
float rive_droite_d(float y)
{
    return 0.1762 + y*(-0.032*2 + y*(0.0258*3 + y*(0.006*4 + y*0.0002*5)));
}

float dune(float x)     //zone des dunes
{
    return 10.767 + x*(0.3125 - x*0.0354);
}

vec4 dunes(float x)   //equation de la position des crêtes
{
    return {12.238f + x*(0.2857f - x*0.0476f), 15.0f + x*(-0.0833f - x*0.0417f),
                14.12f + x*(0.2158f - x*0.0129f), 16.5f + x*(-0.8333f + x*0.0417f)};
}
vec4 heights_dunes(float x)     //hauteurs des crêtes
{
    return {2.9238f + x*(0.2536f - x*0.0298f), 2.1f + x*(0.2167f - x*0.0333f),
                1.756f + x*(-0.0589f - x*0.0154f), 2.2f + x*(0.1667f - x*0.0083f)};
}

bool is_water(float x, float y)
{
    if(y>-10 && x < rive_gauche(y)) return false;
    else if(x<0.5 && y < ile(x)) return false;
    else if(y>-10.5 && y<4.5 && x > rive_droite(y)) return false;
    return true;
}

bool is_berge(float x, float y, float taille_berge)
{
    float deri_g = rive_gauche_d(y);
    float deri_i = ile_d(x);
    float deri_d = rive_droite_d(y);
    float uy_g = -deri_g/std::sqrt((1+deri_g*deri_g))*taille_berge;
    float ux_g = taille_berge/std::sqrt((1+deri_g*deri_g));
    float ux_i = -deri_i/std::sqrt((1+deri_i*deri_i))*taille_berge;
    float uy_i = taille_berge/std::sqrt((1+deri_i*deri_i));
    float uy_d = -deri_d/std::sqrt((1+deri_d*deri_d))*taille_berge;
    float ux_d = taille_berge/std::sqrt((1+deri_d*deri_d));

    if(y> -10 && x+ux_g < rive_gauche(y + uy_g)) return false;
    else if(x<0.5 && y+uy_i < ile(x+ux_i)) return false;
    else if(y>-10.5 && y<4.5 && x-ux_d > rive_droite(y-uy_d)) return false;
    return true;
}

bool is_dune(float x, float y)
{
    if(y>dune(x)) return true;
    return false;
}

bool is_rive_droite(float x, float y)
{
    if(y>-10.5 && y<4.5 && x > rive_droite(y)) return true;
    return false;
}


GLuint texture(const std::string& filename)
{
    // Load an image dune from a file
    image_raw const im1 = image_load_png(filename);

    // Send this image to the GPU, and get its identifier texture_image_id
    GLuint const texture_image_id1 = opengl_texture_to_gpu(im1,
        GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
        GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);
    // Associate the texture_image_id to the image texture used when displaying visual
    return texture_image_id1;
}

std::vector<vcl::vec3> generate_positions_forest(int N, vcl::mesh& terrain) // les N/2 premiers pour les arbres et les autres pour les fougères
{
    std::vector<vcl::vec3> tab;
    int i = 0;
    float dmin = 0.01f;
    int it = 0;
    int Max_it = 5*N;
    bool b;
    while(i<N && it < Max_it){
        it++;
        float u = rand_interval();
        float v = rand_interval();
        vec3 pos = evaluate_terrain2(u, v, terrain);
        b = true;
        float dist;

        if(is_water(pos[0],pos[1])
                || is_berge(pos[0],pos[1], taille_berge3)
                || is_dune(pos[0],pos[1])
                || is_rive_droite(pos[0],pos[1])
                || pos[1] > 7.0f )
            b = false;

        if(b){
            for(int j=0; j<i; j++){ //for(vec3 q : tab)
                dist = sqrt((pos[0]-tab[j][0])*(pos[0]-tab[j][0]) + (pos[1]-tab[j][1])*(pos[1]-tab[j][1]) + (pos[2]-tab[j][2])*(pos[2]-tab[j][2]));
                if(dist < dmin){
                    b = false;
                    break;
                }
            }
        }

        if(b) {
            tab.push_back(pos);
            i++;
        }
    }
    return tab;
}

std::vector<vcl::vec3> generate_positions_pyramids(vcl::mesh& terrain, perlin_noise_parameters const& parameters)
{
    std::vector<vcl::vec3> tab;
    tab.push_back({7.0f,-6.5f,parameters.terrain_height*0.6 + evaluate_dune(7.0f,-6.5f, parameters.terrain_height)});
    tab.push_back({4.0f,-6.0f,parameters.terrain_height*0.6 + evaluate_dune(4.0f,-6.0f, parameters.terrain_height)});
    tab.push_back({6.0f,0.0f,parameters.terrain_height*0.6 + evaluate_dune(6.0f,0.0f, parameters.terrain_height)});
    tab.push_back({2.0f,10.0f,parameters.terrain_height*0.7 + evaluate_dune(2.0f,10.0f, parameters.terrain_height)});
    return tab;
}

std::vector<vcl::vec3> generate_positions_columns(vcl::mesh& terrain, perlin_noise_parameters const& parameters)
{
    std::vector<vcl::vec3> tab;
    tab.push_back({-6.0f,6.3f,parameters.terrain_height + evaluate_dune(-6.0f,6.3f, parameters.terrain_height)});
    tab.push_back({-5.0f,7.0f,parameters.terrain_height + evaluate_dune(-5.0f,7.0f, parameters.terrain_height)});
    tab.push_back({-4.0f,7.7f,parameters.terrain_height + evaluate_dune(-4.0f,7.7f, parameters.terrain_height)});
    tab.push_back({-3.0f,8.3f,parameters.terrain_height + evaluate_dune(-3.0f,8.3f, parameters.terrain_height)});
    tab.push_back({-2.0f,9.0f,parameters.terrain_height + evaluate_dune(-2.0f,9.0f, parameters.terrain_height)});
    tab.push_back({4.0f,10.5f,parameters.terrain_height + evaluate_dune(4.0f,10.5f, parameters.terrain_height)});
    tab.push_back({5.0f,10.5f,parameters.terrain_height + evaluate_dune(5.0f,10.5f, parameters.terrain_height)});
    tab.push_back({6.0f,10.5f,parameters.terrain_height + evaluate_dune(6.0f,10.5f, parameters.terrain_height)});
    tab.push_back({4.0f,9.5f,parameters.terrain_height + evaluate_dune(4.0f,9.5f, parameters.terrain_height)});
    tab.push_back({5.0f,9.5f,parameters.terrain_height + evaluate_dune(5.0f,9.5f, parameters.terrain_height)});
    tab.push_back({6.0f,9.5f,parameters.terrain_height + evaluate_dune(6.0f,9.5f, parameters.terrain_height)});

    return tab;
}


std::vector<vcl::vec3> generate_positions_obelisque(vcl::mesh& terrain, perlin_noise_parameters const& parameters)
{
    std::vector<vcl::vec3> tab;
    tab.push_back({-4.0f,-3.0f,parameters.terrain_height*0.6 + evaluate_dune(-4.0f,-3.0f, parameters.terrain_height)});
    return tab;
}
