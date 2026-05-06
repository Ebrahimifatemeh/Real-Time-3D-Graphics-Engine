#include "asr.h"

#include <cmath>
#include <string>
#include <utility>

static const std::string Shader_Source = R"(
    struct VertexInput {
        @location(0) position: vec3f,
        @location(1) color: vec4f,
        @location(2) uv: vec2f,
    }

    struct VertexOutput {
        @builtin(position) position: vec4f,
        @location(0) color: vec4f,
        @location(1) uv: vec2f,
    }

    struct Uniforms {
        mvp: mat4x4<f32>,
        model_matrix: mat4x4<f32>,
        view_matrix: mat4x4<f32>,
        projection_matrix: mat4x4<f32>,
        texture_matrix: mat4x4<f32>,
        resolution: vec2<f32>,
        mouse: vec2<f32>,
        time: f32,
        dt: f32,
        texture_enabled: u32,
        texturing_mode: u32,
    }
    @group(0) @binding(0) var<uniform> uniforms: Uniforms;
    @group(0) @binding(1) var texture_sampler_val: sampler;
    @group(0) @binding(2) var texture_image: texture_2d<f32>;

    @vertex
    fn vs_main(input: VertexInput) -> VertexOutput {
        var output: VertexOutput;
        output.position = uniforms.mvp * vec4f(input.position, 1.0);
        output.color = input.color;
        let transformed_uv = uniforms.texture_matrix * vec4f(input.uv, 0.0, 1.0);
        output.uv = transformed_uv.xy;
        return output;
    }

    @fragment
    fn fs_main(input: VertexOutput) -> @location(0) vec4f {
        var color = input.color;

        if (uniforms.texture_enabled != 0u) {
            let texel = textureSample(texture_image, texture_sampler_val, input.uv);
            if (uniforms.texturing_mode == 0u) {
                color = color + texel;
            } else if (uniforms.texturing_mode == 1u) {
                color = color - texel;
            } else if (uniforms.texturing_mode == 2u) {
                color = texel - color;
            } else if (uniforms.texturing_mode == 3u) {
                color = color * texel;
            } else if (uniforms.texturing_mode == 4u) {
                color = vec4f(mix(color.rgb, texel.rgb, texel.a), color.a);
            }
        }

        return color;
    }
)";

static asr::GeometryPair generate_sphere_geometry_data(
    const asr::GeometryType geometry_type,
    const float radius,
    const unsigned int width_segments_count,
    const unsigned int height_segments_count,
    const glm::vec4 color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
)
{
    asr::Vertices vertices;
    asr::Indices indices;

    for (auto i = 0U; i <= height_segments_count; ++i) {
        const float v{static_cast<float>(i) / static_cast<float>(height_segments_count)};
        const float phi{v * asr::pi};

        for (auto j = 0U; j <= width_segments_count; ++j) {
            const float u{static_cast<float>(j) / static_cast<float>(width_segments_count)};
            const float theta{u * asr::two_pi};

            const float cos_phi{std::cos(phi)};
            const float sin_phi{std::sin(phi)};
            const float cos_theta{std::cos(theta)};
            const float sin_theta{std::sin(theta)};

            const float x{cos_theta * sin_phi};
            const float y{cos_phi};
            const float z{sin_phi * sin_theta};

            vertices.push_back(asr::Vertex{
                x * radius, y * radius, z * radius,
                color.r, color.g, color.b, color.a,
                1.0f - u, v
            });

            if (geometry_type == asr::GeometryType::Points) {
                indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
            }
        }
    }

    for (auto rows = 0U; rows < height_segments_count; ++rows) {
        for (auto columns = 0U; columns < width_segments_count; ++columns) {
            unsigned int a{rows * (width_segments_count + 1) + columns};
            unsigned int b{a + 1};
            unsigned int c{a + (width_segments_count + 1)};
            unsigned int d{c + 1};

            if (geometry_type == asr::GeometryType::Triangles) {
                if (rows != 0) indices.insert(indices.end(), {a, b, c});
                if (rows != height_segments_count - 1) indices.insert(indices.end(), {b, d, c});
            }
        }
    }

    return std::make_pair(vertices, indices);
}

int main()
{
    using namespace asr;

    create_window(1280U, 720U, "Transform 1 - Solar System");
    create_shader(Shader_Source);

    auto [sphere_vertices, sphere_indices] =
        generate_sphere_geometry_data(GeometryType::Triangles, 1.0f, 32U, 16U);
    auto sphere = create_geometry(GeometryType::Triangles, sphere_vertices, sphere_indices);

    auto sun_image = read_image_file("data/images/sun.jpg");
    auto venus_image = read_image_file("data/images/venus.jpg");
    auto earth_image = read_image_file("data/images/earth.jpg");
    auto moon_image = read_image_file("data/images/moon.jpg");

    auto sun_texture = create_texture(sun_image);
    auto venus_texture = create_texture(venus_image);
    auto earth_texture = create_texture(earth_image);
    auto moon_texture = create_texture(moon_image);

    prepare_for_rendering();
    enable_depth_test();
    enable_face_culling();

    glm::vec3 camera_position{0.0f, 0.0f, 26.0f};

    set_keys_down_event_handler([&](const bool *keys) {
        if (keys[SDL_SCANCODE_W]) camera_position.z -= 6.0f * get_dt();
        if (keys[SDL_SCANCODE_S]) camera_position.z += 6.0f * get_dt();
        if (keys[SDL_SCANCODE_A]) camera_position.x -= 6.0f * get_dt();
        if (keys[SDL_SCANCODE_D]) camera_position.x += 6.0f * get_dt();
        if (keys[SDL_SCANCODE_UP]) camera_position.y += 6.0f * get_dt();
        if (keys[SDL_SCANCODE_DOWN]) camera_position.y -= 6.0f * get_dt();
    });

    set_matrix_mode(Projection);
    load_perspective_projection_matrix(1.13f, 0.1f, 100.0f);

    float sun_spin{0.0f};
    float venus_orbit{0.0f};
    float earth_orbit{0.0f};
    float moon_orbit{0.0f};

    set_geometry_current(&sphere);

    bool stop{false};
    while (!stop) {
        process_window_events(&stop);

        float dt = get_dt();
        sun_spin += dt;
        venus_orbit += dt;
        earth_orbit += dt;
        moon_orbit += dt;

        prepare_to_render_frame();

        set_matrix_mode(View);
        load_identity_matrix();
        translate_matrix(camera_position);

        set_matrix_mode(Model);
        load_identity_matrix();

        push_matrix();
        rotate_matrix({0.0f, sun_spin, 0.0f});
        scale_matrix({2.0f, 2.0f, 2.0f});
        set_texture_current(&sun_texture);
        set_texture_mode(TexturingMode::Modulation);
        render_current_geometry();
        pop_matrix();

        push_matrix();
        rotate_matrix({0.0f, venus_orbit, 0.0f});
        translate_matrix({-5.5f, 0.0f, 0.0f});
        scale_matrix({0.48f, 0.48f, 0.48f});
        set_texture_current(&venus_texture);
        set_texture_mode(TexturingMode::Modulation);
        render_current_geometry();
        pop_matrix();

        push_matrix();
        rotate_matrix({0.0f, earth_orbit, 0.0f});
        translate_matrix({13.0f, 0.0f, 0.0f});

        push_matrix();
        scale_matrix({0.72f, 0.72f, 0.72f});
        set_texture_current(&earth_texture);
        set_texture_mode(TexturingMode::Modulation);
        render_current_geometry();
        pop_matrix();

        push_matrix();
        rotate_matrix({0.0f, moon_orbit, 0.0f});
        translate_matrix({1.6f, -0.55f, 0.0f});
        scale_matrix({0.16f, 0.16f, 0.16f});
        set_texture_current(&moon_texture);
        set_texture_mode(TexturingMode::Modulation);
        render_current_geometry();
        pop_matrix();

        pop_matrix();

        finish_frame_rendering();
    }

    destroy_texture(sun_texture);
    destroy_texture(venus_texture);
    destroy_texture(earth_texture);
    destroy_texture(moon_texture);
    destroy_geometry(sphere);
    destroy_window();

    return 0;
}