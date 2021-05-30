#include "columns.hpp"
#include "vegetation.hpp"


using namespace vcl;


mesh create_disc(float radius)
{
    float const r = radius;

    mesh disc;
    size_t N = 20;

    for (size_t k = 0; k < size_t(N); ++k)
    {
        float const u = k/(N-1.0f);
        vec3 const p = r * vec3(std::cos(2*pi*u), std::sin(2*pi*u), 0.0f);
        disc.position.push_back(p);
        disc.uv.push_back({0.4*std::cos(2*pi*u)+0.5f, 0.4*std::sin(2*pi*u) + 0.5f});

    }
    // middle point
    disc.position.push_back({0,0,0});
    disc.uv.push_back({0.5, 0.5});

    for (size_t k = 0; k < size_t(N-1); ++k)
        disc.connectivity.push_back( uint3{ unsigned(N), unsigned(k), unsigned(k+1)});

    disc.fill_empty_field();
    return disc;
}

vcl::mesh create_column_cyl(float size)
{
    float const h = size * 4.0f; // trunk height
    float r = size * 4.0 / 5; // trunk radius
    const int detail_level = 2;

    // column Hat
    mesh hat = create_disc(3 / 2 * r);
    mesh hat_cyl = create_tree_trunk_cylinder(3 / 2 * r, h / 10);
    mesh top_disc = create_disc(3 / 2 * r);
    top_disc.position += {0.0f, 0.0f, h / 10};
    hat.push_back(hat_cyl);
    hat.push_back(top_disc);
    hat.color.fill({ 0.3f, 0.3f, 0.3f });

    // column body
    std::vector<vcl::mesh> list_trunks;
    std::vector<vcl::vec3> list_centers;
    list_trunks.push_back(create_tree_trunk_cylinder(r, h));
    list_centers.push_back(vec3(0.0f, 0.0f, 0.0f));
    for (int i = 0; i < detail_level; i++) {
        r /= 3;
        rule_trunk(list_trunks, list_centers, r, h);
    }
    mesh column_body;
    for (auto trunk : list_trunks) {
        trunk.color.fill({ 196.0 / 255, 128.0 / 255, 77.0 / 255 });
        column_body.push_back(trunk);
    }
    column_body.fill_empty_field();
    column_body.position += { 0.0f, 0.0f, h/10 }; // place hat at the top of the column
    column_body.color.fill({ 0.3f, 0.3f, 0.3f });

    // entire column
    mesh column = hat;
    column.push_back(column_body);
    hat.position += { 0.0f, 0.0f, h }; // place hat at the top of the column
    column.push_back(hat);

    return column;
}
void initialize_column_cyl(vcl::mesh_drawable& column, float size)
{
    column = mesh_drawable(create_column_cyl(size));
    column.transform.translate.x = 6.0f;
}
