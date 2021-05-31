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
        column_body.push_back(trunk);
    }
    column_body.fill_empty_field();
    column_body.position += { 0.0f, 0.0f, h/10 }; // place hat at the top of the column

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

    // Load an image from a file
    image_raw const im = image_load_png("pictures/texture_column_2.png");

    // Send this image to the GPU, and get its identifier texture_image_id
    GLuint const texture_image_id = opengl_texture_to_gpu(im,
        GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_S*/,
        GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_T*/);

    // Associate the texture_image_id to the image texture used when displaying visual
    column.texture = texture_image_id;
}

vcl::mesh create_obelisque(float base, float height)
{
    vcl::mesh obelisque;
    obelisque.position = { {-base / 2, -base / 2, 0.0f},
                         {-base / 2, base / 2, 0.0f},
                         {base / 2, base / 2, 0.0f},
                         {base / 2, -base / 2, 0.0f},
                         {-base / 4, -base / 4, height},
                         {-base / 4, base / 4, height},
                         {base / 4, base / 4, height},
                         {base / 4, -base / 4, height},
                         {0.0f, 0.0f, height+height/10} };

    obelisque.uv = { {8.0f, 8.0f},
                    {8.0f, 8.0f},
                    {8.0f, 8.0f},
                    {8.0f, 8.0f},
                    {1.0f, 1.0f},
                    {1.0f, 1.0f},
                    {1.0f, 1.0f},
                    {1.0f, 1.0f},
                    {0.0f, 0.0f} };

    obelisque.connectivity.push_back({ 0,1,2 });
    obelisque.connectivity.push_back({ 2,3,0 });
    obelisque.connectivity.push_back({ 0,1,5 });
    obelisque.connectivity.push_back({ 5,4,0 });
    obelisque.connectivity.push_back({ 1,2,6 });
    obelisque.connectivity.push_back({ 6,5,1 });
    obelisque.connectivity.push_back({ 2,3,7 });
    obelisque.connectivity.push_back({ 7,6,2 });
    obelisque.connectivity.push_back({ 3,0,4 });
    obelisque.connectivity.push_back({ 4,7,3 });
    obelisque.connectivity.push_back({ 4,5,8 });
    obelisque.connectivity.push_back({ 5,6,8 });
    obelisque.connectivity.push_back({ 6,7,8 });
    obelisque.connectivity.push_back({ 7,4,8 });

    obelisque.fill_empty_field();

    return obelisque;
}


void initialize_obelisque(vcl::mesh_drawable &obelisque, float size)
{
    obelisque = mesh_drawable(create_obelisque(size * 2.0f, size * 10.0f));
    obelisque.transform.translate.z = 1.0f;
    obelisque.transform.translate.x = -4.0f;
    obelisque.transform.translate.y = -4.0f;

    // Load an image from a file
    image_raw const im = image_load_png("pictures/texture_obelisque.png");

    // Send this image to the GPU, and get its identifier texture_image_id
    GLuint const texture_image_id = opengl_texture_to_gpu(im,
        GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_S*/,
        GL_MIRRORED_REPEAT /*GL_CLAMP_TO_EDGE*/ /**GL_TEXTURE_WRAP_T*/);

    // Associate the texture_image_id to the image texture used when displaying visual
    obelisque.texture = texture_image_id;
}
