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

float taille_berge1 = 0.1;
float taille_berge2 = 0.3;

vcl::mesh initialize_terrain()
{
    int const terrain_sample = 360;
    mesh terrain = mesh_primitive_grid({-8,-15,0},{8,-15,0},{8,15,0},{-8,15,0},terrain_sample,15/8*terrain_sample);
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

vcl::vec3 evaluate_terrain(float u, float v)
{
    float x = (u-0.5f)*16;
    float y = (v-0.5f)*30;
    float z = 0.0f;
    return {x,y,z};
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

            if(is_water(terrain.position[idx].x,terrain.position[idx].y)){
                terrain.position[idx].z = 0;
                // use noise as color value
                terrain.color[idx] = 0.3f*vec3(0,0.0,1.0f)+0.5f*noise*vec3(1,1,1);
            }

            else if (is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge1)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*0.5;
                // use noise as color value
                terrain.color[idx] = vec3(0.31f,0.17f,0.04f)+0.5f*noise*vec3(1,1,1);
            }
            else if (is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)){
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise;
                // use noise as color value
                terrain.color[idx] = vec3(0.34f,0.16f,0.0f)+0.5f*noise*vec3(1,1,1);
            }

            else if (is_dune(terrain.position[idx].x,terrain.position[idx].y)){
                // use also the noise as color value
                terrain.color[idx] = vec3(0.87,0.70,0.5)+0.5f*noise*vec3(1,1,1);
                // use the noise as height value
                terrain.position[idx].z = parameters.terrain_height*noise*
                        std::exp(-(terrain.position[idx].y-dune(terrain.position[idx].x))*(terrain.position[idx].y-dune(terrain.position[idx].x)))
                        + evaluate_dune(terrain.position[idx].x,terrain.position[idx].y, parameters.terrain_height);
            }
            else {
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

void update_terrain(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual1, vcl::mesh_drawable& terrain_visual2, vcl::mesh_drawable& terrain_visual3, vcl::mesh_drawable& terrain_visual4, vcl::mesh_drawable& terrain_visual5, perlin_noise_parameters const& parameters, float t)
{
    update_terrain_herbe(terrain, terrain_visual1, parameters);
    update_terrain_berge_bas(terrain, terrain_visual2, parameters);
    update_terrain_berge_haut(terrain, terrain_visual3, parameters);
    update_terrain_dune(terrain, terrain_visual4, parameters);
    update_terrain_water(terrain, terrain_visual5, parameters, t);
}

void update_terrain_water(vcl::mesh& terrain, vcl::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters, float t)
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
            float const noise2 = noise_perlin({u, v}, 6.0f, 0.6f, 2.25 - 0.3*sin(pi/2 + pi*t));

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
                terrain.position[idx].z = parameters.terrain_height*noise*0.5;
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
                    && is_berge(terrain.position[idx].x,terrain.position[idx].y, taille_berge2)){
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
                     && !is_dune(terrain.position[idx].x,terrain.position[idx].y) )
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
float ile(float x)
{
    return -13.578 + x*(-2.5778 + x*(-0.5222 - x*0.0222));
}
float rive_droite(float y)
{
    return 2.599 + y*(0.1762 + y*(-0.032 + y*(0.0258 + y*(0.006 + y*0.0002))));
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

bool is_water_perlin(float x, float y, float noise)
{
    if(x < rive_gauche(y) + noise) return false;
    else if(x<0.5 && y < ile(x) + noise) return false;
    else if(y>-10.5 && y<4.5 && x > rive_droite(y) + noise) return false;
    return true;
}

bool is_berge(float x, float y, float taille_berge)
{
    if(y> -10 && x < rive_gauche(y) - taille_berge) return false;
    else if(x<0.5 && y < ile(x) - taille_berge) return false;
    else if(y>-10.5 && y<4.5 && x > rive_droite(y) + taille_berge) return false;
    return true;
}

bool is_dune(float x, float y)
{
    if(y>dune(x)) return true;
    return false;
}

float evaluate_dune1(float x, float y, float height)
{
    vec2 const d0 = {-8.5f, 14.5f};
    vec2 const d1 = {-7.0f, 11.0f};
    vec2 const d2 = {-5.0f, 13.7f};
    vec2 const d3 = {-3.5f, 12.2f};
    vec2 const d4 = {-2.5f, 14.7f};
    vec2 const d5 = {-0.0f, 13.4f};
    vec2 const d6 = {2.0f, 15.2f};
    vec2 const d7 = {3.5f, 13.1f};
    vec2 const d8 = {6.0f, 14.5f};
    vec2 const d9 = {8.5f, 13.6f};
    vec2 vects[10] = {d0, d1, d2, d3, d4, d5, d6, d7, d8, d9};

    float const h0 = 7.0f*height;
    float const h1 = 4.0f*height;
    float const h2 = 5.0f*height;
    float const h3 = 2.0f*height;
    float const h4 = 5.0f*height;
    float const h5 = 3.0f*height;
    float const h6 = 6.0f*height;
    float const h7 = 2.0f*height;
    float const h8 = 5.0f*height;
    float const h9 = 3.0f*height;
    float h[10] = {h0, h1, h2, h3, h4, h5, h6, h7, h8, h9};

    float const sigma0 = 2.4f;
    float const sigma1 = 1.8f;
    float const sigma2 = 1.9f;
    float const sigma3 = 3.0f;
    float const sigma4 = 1.7f;
    float const sigma5 = 2.6f;
    float const sigma6 = 2.2f;
    float const sigma7 = 2.8f;
    float const sigma8 = 1.4f;
    float const sigma9 = 2.4f;
    float sig[10] = {sigma0, sigma1, sigma2, sigma3, sigma4, sigma5, sigma6, sigma7, sigma8, sigma9};

    float z = 0.0f;
    float d;
    for(int i = 0; i<10; i+=2){
        d = norm(vec2(x,y)-vects[i])/sig[i];
        z+= h[i]*std::exp(-d*d);
    }


    return z;
}

bool is_water1(float x, float y, buffer<vec3> *courbes_fleuve)
{
    float dt = 0.01f;

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


mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const unsigned int N = 360;

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
            terrain.uv[kv+N*ku] = {10*u,10*v};
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
