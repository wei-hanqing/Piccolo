#version 310 es

#extension GL_GOOGLE_include_directive : enable

#include "constants.h"

layout(input_attachment_index = 0, set = 0, binding = 0) uniform highp subpassInput in_color;

layout(set = 0, binding = 1) uniform sampler2D color_grading_lut_texture_sampler;

layout(location = 0) out highp vec4 out_color;

void main()
{
    highp ivec2 lut_tex_size = textureSize(color_grading_lut_texture_sampler, 0);
    highp float _COLORS      = float(lut_tex_size.y);

    highp vec4 color       = subpassLoad(in_color).rgba;
    
    // my code
    // texture(color_grading_lut_texture_sampler, uv)
    highp vec2 uv;
    uv.y = color.g;
    
    uv.x = floor(color.b * _COLORS) + color.r;
    uv.x /= _COLORS;
    highp vec4 color_sampled_l = texture(color_grading_lut_texture_sampler, uv);

    uv.x = ceil(color.b * _COLORS) + color.r;
    uv.x /= _COLORS;
    highp vec4 color_sampled_r = texture(color_grading_lut_texture_sampler, uv);

    color = mix(color_sampled_l, color_sampled_r, color.b - floor(color.b));

    out_color = color;
}
