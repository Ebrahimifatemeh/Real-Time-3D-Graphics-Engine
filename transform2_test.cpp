#include "asr.h"

#include <chrono>
#include <ctime>
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
    };

    @group(0) @binding(0) var<uniform> uniforms: Uniforms;

    @vertex
    fn vs_main(input: VertexInput) -> VertexOutput {
        var output: VertexOutput;
        output.position = uniforms.mvp * vec4f(input.position, 1.0);
        output.color = input.color;
        return output;
    }

    @fragment
    fn fs_main(input: VertexOutput) -> @location(0) vec4f {
        return input.color;
    }
)";

static asr::Geometry make_centered_rect(const glm::vec4& color)
{
    return asr::create_geometry(
        asr::GeometryType::Triangles,
        {
            {-0.5f, -0.5f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f},
            { 0.5f, -0.5f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f},
            { 0.5f,  0.5f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f},
            {-0.5f,  0.5f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f}
        },
        {0, 1, 2, 0, 2, 3}
    );
}

static asr::Geometry make_hand_rect(const glm::vec4& color)
{
    return asr::create_geometry(
        asr::GeometryType::Triangles,
        {
            {-0.5f, 0.0f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f},
            { 0.5f, 0.0f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f},
            { 0.5f, 1.0f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f},
            {-0.5f, 1.0f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f}
        },
        {0, 1, 2, 0, 2, 3}
    );
}

static asr::Geometry make_center_sphere_geometry(const glm::vec4& color)
{
    asr::Vertices vertices;
    asr::Indices indices;

    const float radius = 0.08f;
    const unsigned int segments = 24U;

    vertices.push_back({0.0f, 0.0f, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f});

    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = static_cast<float>(i) * asr::two_pi / static_cast<float>(segments);
        float x = std::cos(angle) * radius;
        float y = std::sin(angle) * radius;
        vertices.push_back({x, y, 0.0f, color.r, color.g, color.b, color.a, 0.0f, 0.0f});
    }

    for (unsigned int i = 1; i <= segments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    return asr::create_geometry(asr::GeometryType::Triangles, vertices, indices);
}

int main()
{
    using namespace asr;

    create_window(1280U, 720U, "Transformation Test on ASR Version 1.2");
    create_shader(Shader_Source);

    auto white_mark = make_centered_rect({1.0f, 1.0f, 1.0f, 1.0f});
    auto pink_mark = make_centered_rect({1.0f, 0.60f, 0.72f, 1.0f});
    auto red_mark = make_centered_rect({1.0f, 0.0f, 0.0f, 1.0f});

    auto hour_hand = make_hand_rect({1.0f, 0.0f, 0.0f, 1.0f});
    auto minute_hand = make_hand_rect({1.0f, 0.0f, 0.0f, 1.0f});
    auto second_hand = make_hand_rect({1.0f, 0.0f, 0.0f, 1.0f});
    auto center_sphere = make_center_sphere_geometry({1.0f, 0.2f, 0.2f, 1.0f});

    prepare_for_rendering();

    float face_rotation = 0.0f;
    bool should_stop{false};

    while (!should_stop) {
        process_window_events(&should_stop);

        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm* local_time = std::localtime(&t);

        float seconds = static_cast<float>(local_time->tm_sec);
        float minutes = static_cast<float>(local_time->tm_min) + seconds / 60.0f;
        float hours = static_cast<float>(local_time->tm_hour % 12) + minutes / 60.0f;

        float second_angle = -seconds * (two_pi / 60.0f);
        float minute_angle = -minutes * (two_pi / 60.0f);
        float hour_angle = -hours * (two_pi / 12.0f);

        face_rotation += get_dt() * 0.08f;

        prepare_to_render_frame();

        set_matrix_mode(Projection);
        load_orthographic_projection_matrix(2.2f, -10.0f, 10.0f);

        set_matrix_mode(View);
        load_identity_matrix();

        set_matrix_mode(Model);
        load_identity_matrix();

        push_matrix();
        rotate_matrix({0.0f, 0.0f, face_rotation});

        set_geometry_current(&white_mark);
        for (int i = 0; i < 60; ++i) {
            float angle = i * (two_pi / 60.0f);
            push_matrix();
            rotate_matrix({0.0f, 0.0f, angle});
            translate_matrix({0.0f, 1.33f, 0.0f});
            scale_matrix({0.022f, 0.022f, 1.0f});
            render_current_geometry();
            pop_matrix();
        }

        set_geometry_current(&pink_mark);
        for (int i = 0; i < 12; ++i) {
            float angle = i * (two_pi / 12.0f);
            push_matrix();
            rotate_matrix({0.0f, 0.0f, angle});
            translate_matrix({0.0f, 1.33f, 0.0f});
            rotate_matrix({0.0f, 0.0f, quarter_pi});
            scale_matrix({0.050f, 0.050f, 1.0f});
            render_current_geometry();
            pop_matrix();
        }

        set_geometry_current(&red_mark);
        for (int i = 0; i < 4; ++i) {
            float angle = i * (two_pi / 4.0f);
            push_matrix();
            rotate_matrix({0.0f, 0.0f, angle});
            translate_matrix({0.0f, 1.33f, 0.0f});
            rotate_matrix({0.0f, 0.0f, quarter_pi});
            scale_matrix({0.095f, 0.095f, 1.0f});
            render_current_geometry();
            pop_matrix();
        }

        pop_matrix();

        set_geometry_current(&hour_hand);
        push_matrix();
        rotate_matrix({0.0f, 0.0f, hour_angle});
        translate_matrix({0.0f, -0.03f, 0.0f});
        scale_matrix({0.030f, 0.62f, 1.0f});
        render_current_geometry();
        pop_matrix();

        set_geometry_current(&minute_hand);
        push_matrix();
        rotate_matrix({0.0f, 0.0f, minute_angle});
        translate_matrix({0.0f, -0.02f, 0.0f});
        scale_matrix({0.020f, 0.88f, 1.0f});
        render_current_geometry();
        pop_matrix();

        set_geometry_current(&second_hand);
        push_matrix();
        rotate_matrix({0.0f, 0.0f, second_angle});
        translate_matrix({0.0f, -0.015f, 0.0f});
        scale_matrix({0.010f, 0.98f, 1.0f});
        render_current_geometry();
        pop_matrix();

        set_geometry_current(&center_sphere);
        render_current_geometry();

        finish_frame_rendering();
    }

    destroy_geometry(white_mark);
    destroy_geometry(pink_mark);
    destroy_geometry(red_mark);
    destroy_geometry(hour_hand);
    destroy_geometry(minute_hand);
    destroy_geometry(second_hand);
    destroy_geometry(center_sphere);
    destroy_window();

    return 0;
}