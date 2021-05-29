#include "vegetation.hpp"


using namespace vcl;


mesh create_tree_trunk_cylinder(float radius, float height)
{
    mesh m;
    const unsigned int N = 100;
    m.position.resize(N);

    float z = height;
    float x = 0.0f;
    float y = 0.0f;
    for (int i = 0; i < N; i++) {
        z = height - z;
        if (i % 2 == 0) {
            x = radius * std::cos(2 * 3.14f * i / N);
            y = radius * std::sin(2 * 3.14f * i / N);
        }
        m.position[i] = { x, y, z };
    }

    for (int i = 0; i < N - 2; i++) {
        if (i % 2 == 0)
            m.connectivity.push_back({ i, i + 1, i + 2 });
        else
            m.connectivity.push_back({ i + 1, i, i + 2 });
    }
    if ((N - 2) % 2 == 0) {
        m.connectivity.push_back({ N - 2, N - 1, 0 });
        m.connectivity.push_back({ N - 1, 1, 0 });
    }
    else {
        m.connectivity.push_back({ N - 2, 0, N - 1 });
        m.connectivity.push_back({ N - 1, 0, 1 });
    }

    m.fill_empty_field();
    return m;
}


mesh create_palm_leaf(float width, float m, vec3 v_0, float t_max, unsigned int N, float coef_end)
{
    mesh leaf;
    leaf.position.resize(3*N + 2);
    double dt = t_max / N;
    vec3 g = { 0.0f, 0.0f, -9.81f / m };
    float norm = v_0.x * v_0.x + v_0.y * v_0.y + v_0.z * v_0.z;
    vec3 dir_width = { - v_0.y / norm, v_0.x / norm, 0.0f };
    double t = 0.0;
    leaf.position[0] = { 0.0f, 0.0f, 0.0f };
    for (int i = 2; i < 3 * N + 1; i += 3) {
        t += dt;
        leaf.position[i] = t * v_0 + 0.5f * t * t * g;
        leaf.position[i - 1] = leaf.position[i] - (width / 2) * dir_width;
        leaf.position[i + 1] = leaf.position[i] + (width / 2) * dir_width;
    }
    t += dt;
    leaf.position[3 * N + 1] = t * v_0 + 0.5f * t * t * g;
    leaf.position[3 * N + 1] += coef_end * width * (leaf.position[3 * N + 1] - leaf.position[3 * N - 1]);
    leaf.connectivity.push_back({ 0,1,2 });
    leaf.connectivity.push_back({ 0,3,2 });
    for (int i = 2; i < 3 * N - 1; i += 3) {
        leaf.connectivity.push_back({ i - 1, +i, i + 3 });
        leaf.connectivity.push_back({ i - 1, i + 2, i + 3 });
        leaf.connectivity.push_back({ i + 1, i + 4, i + 3 });
        leaf.connectivity.push_back({ i + 1, +i, i + 3 });
    }
    leaf.connectivity.push_back({ 3*N-1, 3*N-2, 3*N+1 });
    leaf.connectivity.push_back({ 3*N-1, 3*N, 3*N+1 });

    leaf.fill_empty_field();
    return leaf;
}



vcl::mesh create_palm_tree(float size, int N_leafs, float spreading)
{
    float const h = size; // trunk height
    float const r = size / 10; // trunk radius
    float const width = 2.0f;
    float const m = 5.0f;
    float const t_max = size / 2;

    // Trunk
    mesh trunk = create_tree_trunk_cylinder(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });

    // Foliage
    float da = 2 * 3.14 / N_leafs;
    mesh foliage = create_palm_leaf(width, m, { spreading, 0.0f, 1.0f }, t_max);
    for (int i = 1; i < 10; i++) {
        foliage.push_back(create_palm_leaf(width, m, { spreading * std::cos(i * da), spreading * std::sin(i * da), 1.0f }, t_max));
    }
    foliage.position += { 0.0f, 0.0f, h }; // place foliage at the top of the trunk
    foliage.color.fill({ 0.0f, 1.0f, 0.0f });

    // Tree
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}


void initialize_palm_tree(vcl::mesh_drawable& palm_tree, float size)
{
    palm_tree = mesh_drawable(create_palm_tree(size));
    palm_tree.transform.translate.x = 4.0f;
}




vcl::mesh create_leaf(float radius, float width, int N)
{
    mesh leaf;
    leaf.position.resize(2 * N);
    double alpha = std::acos(1 - width / (2 * radius));
    double beta = 2 * alpha / N;
    float x = 0.0f, y = 0.0f;
    // k = 0
    leaf.position[0] = { 0.0f, -radius * std::sin(alpha), 0.0f };
    for (int k = 1; k < N; k++) {
        x = radius * std::cos(-alpha + k * beta);
        y = radius * std::sin(-alpha + k * beta);
        leaf.position[k] = { width / 2 - radius + x, y, -k*k*radius/50000 };
        leaf.position[N + k] = { radius - width / 2 - x, y, -k * k * radius / 50000 };
    }
    // k = N
    leaf.position[N] = { 0.0f, radius * std::sin(alpha), -N * N * radius / 50000 };
    translate_leaf(leaf, { 0.0f, 0.0f, 0.0f });
    return leaf;
}


void rotate_leaf(vcl::mesh &leaf, float alpha, int axis)
{
    assert(axis >= 0 && axis <= 2);
    mat3 M;
    if (axis == 0) {
        M = { 1.0f, 0.0f, 0.0f,
              0.0f, std::cos(alpha), -std::sin(alpha),
              0.0f, std::sin(alpha), std::cos(alpha) };
    }
    else if (axis == 1) {
        M = { std::cos(alpha), 0.0f, std::sin(alpha),
              0.0f, 1.0f, 0.0f,
              -std::sin(alpha), 0.0f, std::cos(alpha) };
    }
    else { // axis == 2
        M = { std::cos(alpha), -std::sin(alpha), 0.0f,
              std::sin(alpha), std::cos(alpha), 0.0f,
              0.0f, 0.0f, 1.0f };
    }
    for (int i = 0; i < leaf.position.size(); i++) {
        leaf.position[i] = M * leaf.position[i];
    }
}


void translate_leaf(vcl::mesh &leaf, vcl::vec3 p0)
{
    vec3 d = p0 - leaf.position[0];
    for (int i = 0; i < leaf.position.size(); i++) {
        leaf.position[i] += d;
    }
}


void rule_leaf(std::vector<vcl::mesh>& list_leafs, float r, float w, std::vector<float> list_alphas)
{
    const int resolution = 50;
    const int nb = 20; // number of leafs on each side of the former leaf
    const int moy = nb / 2;
    int s = list_leafs.size();
    int N = list_leafs[0].position.size() / 2;
    int pas = (N - 1) / nb;
    vcl::vec3 p0;
    for (int i = 0; i < s; i++) {
        float alphaR = list_alphas[0] - 3.14f / 2;
        float alphaL = list_alphas[0] + 3.14f / 2;
        for (int j = 1; j < nb + 1; j++) {
            p0 = (list_leafs[0].position[j * pas] + list_leafs[0].position[N + j * pas]) / 2;

            // Right
            mesh leaf = create_leaf(vcl::norm(list_leafs[0].position[j * pas] - p0), w, resolution);
            rotate_leaf(leaf, alphaR);
            translate_leaf(leaf, p0);
            list_leafs.push_back(leaf);
            list_alphas.push_back(alphaR);

            // Left
            leaf = create_leaf(vcl::norm(list_leafs[0].position[j * pas] - p0), w, resolution);
            rotate_leaf(leaf, alphaL);
            translate_leaf(leaf, p0);
            list_leafs.push_back(leaf);
            list_alphas.push_back(alphaL);
        }

        // Last leaf
        /*mesh leaf = create_leaf(r, w, resolution);
        rotate_leaf(leaf, list_alphas[0]);
        translate_leaf(leaf, p0);
        list_leafs.push_back(leaf);
        list_alphas.push_back(list_alphas[0]);*/

        list_leafs.erase(list_leafs.begin());
        list_alphas.erase(list_alphas.begin());
    }
}


void leaf_to_triangles(vcl::mesh& leaf)
{
    int N = leaf.position.size() / 2;
    for (int i = 1; i < N - 1; i++) {
        leaf.connectivity.push_back({ i, i + 1, N + i });
        leaf.connectivity.push_back({ N + i, i + 1, N + i + 1 });
    }
    leaf.connectivity.push_back({ 0, 1, N + 1 });
    leaf.connectivity.push_back({ N - 1, N, 2 * N - 1 });
    leaf.fill_empty_field();
}


void rule_trunk(std::vector<vcl::mesh> &list_trunks, std::vector<vcl::vec3> list_centers, float r, float h)
{
    int s = list_trunks.size();
    for (int i = 0; i < s; i++) {
        mesh trunk = create_tree_trunk_cylinder(r, h);
        int p = trunk.position.size();
        for (int k = 0; k < p; k++) {
            trunk.position[k] += list_centers[0];
        }
        list_trunks.push_back(trunk);
        list_centers.push_back(list_centers[0]);
        for (int j = 0; j < 6; j++) {
            mesh trunk = create_tree_trunk_cylinder(r, h);
            vcl::vec3 center = list_centers[0] + vcl::vec3(2 * r * std::cos(1.047 * j), 2 * r * std::sin(1.047 * j), 0.0f);
            int p = trunk.position.size();
            for (int k = 0; k < p; k++) {
                trunk.position[k] += center;
            }
            list_trunks.push_back(trunk);
            list_centers.push_back(center);
        }
        list_trunks.erase(list_trunks.begin());
        list_centers.erase(list_centers.begin());
    }
}


vcl::mesh create_fern(float leaf_radius, float leaf_width, float trunk_radius, float trunk_height, int detail_level, int N_leafs)
{
    const unsigned int N = 100;
    std::vector<vcl::mesh> list_leafs, list_trunks;
    std::vector<vcl::vec3> list_centers;
    std::vector<float> list_alphas;
    list_trunks.push_back(create_tree_trunk_cylinder(trunk_radius, trunk_height));
    list_centers.push_back(vec3(0.0f, 0.0f, 0.0f));
    for (int l = 0; l < N_leafs; l++) {
        list_leafs.push_back(create_leaf(leaf_radius, leaf_width, N));
        translate_leaf(list_leafs[l], { 0.0f, 0.0f, trunk_height - 0.01f });
        rotate_leaf(list_leafs[l], static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.14f / 4))), 0);
        float alpha = 2 * l * 3.14f / N_leafs;
        rotate_leaf(list_leafs[l], alpha);
        list_alphas.push_back(alpha);
    }
    for (int i = 0; i < detail_level; i++) {
        trunk_radius /= 3;
        leaf_radius /= 5;
        leaf_width /= 5;
        rule_leaf(list_leafs, leaf_radius, leaf_width, list_alphas);
        rule_trunk(list_trunks, list_centers, trunk_radius, trunk_height);  
    }
    vcl::mesh fern;
    for (auto leaf : list_leafs) {
        leaf_to_triangles(leaf);
        leaf.color.fill({ 0.0f, 1.0f, 0.0f });
        fern.push_back(leaf);
    }
    for (auto trunk : list_trunks) {
        trunk.color.fill({ 196.0 / 255, 128.0 / 255, 77.0 / 255 });
        fern.push_back(trunk);
    }
    fern.fill_empty_field();
    return fern;
}


void initialize_fern(vcl::mesh_drawable& fern, float size)
{
    fern = mesh_drawable(create_fern(1.0f, 0.3f, 0.3f, 0.2f, 2));
    fern.transform.translate.z = 0.4f;
}
