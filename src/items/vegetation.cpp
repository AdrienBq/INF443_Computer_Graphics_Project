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




vcl::mesh create_leaf(float length, float max_width)
{
    // TO DO
    return vcl::mesh_primitive_arrow();
}


void rule_leaf(std::vector<vcl::mesh> &list_leafs, int N)
{
    // TO DO
}


void rule_trunk(std::vector<vcl::mesh> &list_trunks, std::vector<vcl::vec3> list_centers, float r, float h, int N)
{
    int s = list_trunks.size();
    for (int i = 0; i < s; i++) {
        // TO DO
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


void leaf_to_triangles(vcl::mesh &leaf, int N)
{
    // TO DO
}


vcl::mesh create_fern(float length, float max_width, float radius, float height, int detail_level, int N_leafs)
{
    const unsigned int N = 100;
    std::vector<vcl::mesh> list_leafs, list_trunks;
    std::vector<vcl::vec3> list_centers;
    list_trunks.push_back(create_tree_trunk_cylinder(radius, height));
    list_centers.push_back(vec3(0.0f, 0.0f, 0.0f));
    for (int i = 0; i < detail_level; i++) {
        radius /= 3;
        rule_leaf(list_leafs, N);
        rule_trunk(list_trunks, list_centers, radius, height, N);  
    }
    vcl::mesh fern;
    for (auto leaf : list_leafs) {
        leaf_to_triangles(leaf, N);
        fern.push_back(leaf);
    }
    for (auto trunk : list_trunks) {
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
