
// INITIAL HLSL BEGIN

#pragma warning( disable: 3556 3571 )
struct _geometry
{
    float _dist;
    float _materialIndex;
    float _specular;
    float _diffuse;
    float3 _space;
    float3 _color;
};
#pragma pack_matrix(row_major)
struct rm__geometry
{
    float _dist;
    float _materialIndex;
    float _specular;
    float _diffuse;
    float3 _space;
    float3 _color;
};
#pragma pack_matrix(column_major)
struct std__geometry
{
    float _dist;
    float _materialIndex;
    float _specular;
    float _diffuse;
    float3 _space;
    float3 _color;
};
#pragma pack_matrix(row_major)
struct std_rm__geometry
{
    float _dist;
    float _materialIndex;
    float _specular;
    float _diffuse;
    float3 _space;
    float3 _color;
};
#pragma pack_matrix(column_major)
struct std_fp__geometry
{
    float _dist;
    float _materialIndex;
    float _specular;
    float _diffuse;
    float3 _space;
    float pad_0;
    float3 _color;
    float pad_1;
};
#pragma pack_matrix(row_major)
struct std_rm_fp__geometry
{
    float _dist;
    float _materialIndex;
    float _specular;
    float _diffuse;
    float3 _space;
    float pad_2;
    float3 _color;
    float pad_3;
};
#pragma pack_matrix(column_major)
float float_ctor(int x0)
{
    return float(x0);
}
float3 vec3_ctor(float x0, float x1, float x2)
{
    return float3(x0, x1, x2);
}
float4 vec4_ctor(float3 x0, float x1)
{
    return float4(x0, x1);
}
// Uniforms

uniform float3 _iResolution : register(c0);
uniform float _iTime : register(c1);
static const uint _iChannel0 = 0;
uniform Texture2D textures2D[1] : register(t0);
uniform SamplerState samplers2D[1] : register(s0);
#ifdef ANGLE_ENABLE_LOOP_FLATTEN
#define LOOP [loop]
#define FLATTEN [flatten]
#else
#define LOOP
#define FLATTEN
#endif

#define ATOMIC_COUNTER_ARRAY_STRIDE 4

// Varyings

static float4 out_shadertoy_out_color = {0, 0, 0, 0};
static float4 gl_FragCoord = float4(0, 0, 0, 0);

cbuffer DriverConstants : register(b1)
{
    float4 dx_ViewCoords : packoffset(c1);
    float3 dx_DepthFront : packoffset(c2);
    float2 dx_ViewScale : packoffset(c3);
    struct SamplerMetadata
    {
        int baseLevel;
        int internalFormatBits;
        int wrapModes;
        int padding;
        int4 intBorderColor;
    };
    SamplerMetadata samplerMetadata[1] : packoffset(c4);
};

float4 gl_texture2D(uint samplerIndex, float2 t)
{
    return textures2D[samplerIndex].Sample(samplers2D[samplerIndex], float2(t.x, t.y));
}

#define GL_USES_FRAG_COORD
float3 mod_emu(float3 x, float y)
{
    return x - y * floor(x / y);
}


void f_mainImage_float4(inout float4 _fragColor, in float2 _fragCoord);
;
@@ PIXEL OUTPUT @@

PS_OUTPUT main(@@ PIXEL MAIN PARAMETERS @@){
@@ MAIN PROLOGUE @@
(out_shadertoy_out_color = float4(1.0, 1.0, 1.0, 1.0));
float4 _color3107 = {0.0, 0.0, 0.0, 1.0};
f_mainImage_float4(_color3107, gl_FragCoord.xy);
if ((out_shadertoy_out_color.x < 0.0))
{
(_color3107 = float4(1.0, 0.0, 0.0, 1.0));
}
if ((out_shadertoy_out_color.y < 0.0))
{
(_color3107 = float4(0.0, 1.0, 0.0, 1.0));
}
if ((out_shadertoy_out_color.z < 0.0))
{
(_color3107 = float4(0.0, 0.0, 1.0, 1.0));
}
if ((out_shadertoy_out_color.w < 0.0))
{
(_color3107 = float4(1.0, 1.0, 0.0, 1.0));
}
(out_shadertoy_out_color = vec4_ctor(_color3107.xyz, 1.0));
return generateOutput();
}
static float _vol = {0.0};
static float _noise = {0.0};
float f_hash12(in float2 _p)
{
float _h3112 = dot(_p, float2(127.1, 311.70001));
return frac((sin(_h3112) * 43758.547));
}
float f_noise_3(in float3 _p)
{
float3 _i3115 = floor(_p);
float3 _f3116 = frac(_p);
float3 _u3117 = ((_f3116 * _f3116) * (3.0 - (2.0 * _f3116)));
float2 _ii3118 = (_i3115.xy + (_i3115.z * float2(5.0, 5.0)));
float _a3119 = f_hash12((_ii3118 + float2(0.0, 0.0)));
float _b3120 = f_hash12((_ii3118 + float2(1.0, 0.0)));
float _c3121 = f_hash12((_ii3118 + float2(0.0, 1.0)));
float _d3122 = f_hash12((_ii3118 + float2(1.0, 1.0)));
float _v13123 = lerp(lerp(_a3119, _b3120, _u3117.x), lerp(_c3121, _d3122, _u3117.x), _u3117.y);
(_ii3118 += float2(5.0, 5.0));
(_a3119 = f_hash12((_ii3118 + float2(0.0, 0.0))));
(_b3120 = f_hash12((_ii3118 + float2(1.0, 0.0))));
(_c3121 = f_hash12((_ii3118 + float2(0.0, 1.0))));
(_d3122 = f_hash12((_ii3118 + float2(1.0, 1.0))));
float _v23124 = lerp(lerp(_a3119, _b3120, _u3117.x), lerp(_c3121, _d3122, _u3117.x), _u3117.y);
return max(lerp(_v13123, _v23124, _u3117.z), 0.0);
}
float f_fbm(in float3 _x)
{
float _r3127 = {0.0};
float _w3128 = {1.0};
float _s3129 = {1.0};
{ for(int _i3130 = {0}; (_i3130 < 7); (_i3130++))
{
(_w3128 *= 0.5);
(_s3129 *= 2.0);
(_r3127 += (_w3128 * f_noise_3((_s3129 * _x))));
}
}
return _r3127;
}
float3 f_fromRGB(in int _r, in int _g, in int _b)
{
return (vec3_ctor(float_ctor(_r), float_ctor(_g), float_ctor(_b)) / 255.0);
}
static float3 _light = {0.0, 0.0, 0.0};
static float3 _lightDir = {0.0, 0.0, 0.0};
static float3 _lightColour = {0.86502481, 0.48056933, 0.14417081};
float f_saturate(in float _a)
{
return clamp(_a, 0.0, 1.0);
}
float f_smin(in float _a, in float _b, in float _k)
{
float _res3150 = (exp(((-_k) * _a)) + exp(((-_k) * _b)));
return ((-log(_res3150)) / _k);
}
;
_geometry f_scene(in float3 _p)
{
_geometry _plane3161 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float _localNoise3162 = (f_fbm((_p / 10.0)) * 2.0);
(_p.y -= (_localNoise3162 * 0.2));
(_plane3161._dist = _p.y);
(_p.y *= 3.5);
(_plane3161._dist = f_smin(_plane3161._dist, (length(_p) - 25.0), (0.15000001 + (_localNoise3162 * 0.2))));
(_plane3161._dist = max(_plane3161._dist, (((-length(_p)) + 29.0) + _localNoise3162)));
(_plane3161._materialIndex = 4.0);
(_plane3161._space = _p);
(_plane3161._color = float3(1.0, 0.2, 0.0));
(_plane3161._diffuse = 0.0);
(_plane3161._specular = 22.1);
return _plane3161;
}
static float _t_max = {570.0};
_geometry f_enchancedRayMarcher(in float3 _o, in float3 _d, in int _maxI)
{
_geometry _mp3170 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float _t_min3171 = {0.001};
float _tb3172 = ((2.0999999 - _o.y) / _d.y);
if ((_tb3172 < 0.0))
{
(_mp3170._dist = 100000003318135351409612647563264.0);
return _mp3170;
}
(_t_min3171 = _tb3172);
float _omega3173 = {1.3};
float _t3174 = _t_min3171;
float _candidate_error3175 = {100000003318135351409612647563264.0};
float _candidate_t3176 = _t_min3171;
float _previousRadius3177 = {0.0};
float _stepLength3178 = {0.0};
float _pixelRadius3179 = {0.0028571428};
float scbd = {0};
if ((f_scene(_o)._dist < 0.0))
{
(scbd = -1.0);
}
else
{
(scbd = 1.0);
}
float _functionSign3180 = scbd;
{ for(int _i3181 = {0}; (_i3181 < 90); (++_i3181))
{
if (((_maxI > 0) && (_i3181 > _maxI)))
{
break;
}
(_mp3170 = f_scene(((_d * _t3174) + _o)));
float _signedRadius3182 = (_functionSign3180 * _mp3170._dist);
float _radius3183 = abs(_signedRadius3182);
bool _sorFail3184 = ((_omega3173 > 1.0) && ((_radius3183 + _previousRadius3177) < _stepLength3178));
if (_sorFail3184)
{
(_stepLength3178 -= (_omega3173 * _stepLength3178));
(_omega3173 = 1.0);
}
else
{
(_stepLength3178 = ((_signedRadius3182 * _omega3173) * 0.80000001));
}
(_previousRadius3177 = _radius3183);
float _error3185 = (_radius3183 / _t3174);
if (((!_sorFail3184) && (_error3185 < _candidate_error3175)))
{
(_candidate_t3176 = _t3174);
(_candidate_error3175 = _error3185);
}
if ((((!_sorFail3184) && (_error3185 < _pixelRadius3179)) || (_t3174 > _t_max)))
{
break;
}
(_t3174 += _stepLength3178);
}
}
(_mp3170._dist = _candidate_t3176);
if (((_t3174 > _t_max) || (_candidate_error3175 > _pixelRadius3179)))
{
(_mp3170._dist = 100000003318135351409612647563264.0);
}
return _mp3170;
}
float f_softShadow(in float3 _ro, in float3 _lp, in float _k)
{
float3 _rd3198 = (_lp - _ro);
float _shade3199 = {1.0};
float _dist3200 = {1.0};
float _end3201 = max(length(_rd3198), 0.0099999998);
float _stepDist3202 = (_end3201 / 125.0);
float _tb3203 = ((8.0 - _ro.y) / normalize(_rd3198).y);
if ((_tb3203 > 0.0))
{
(_end3201 = min(_end3201, _tb3203));
}
(_rd3198 /= _end3201);
{ for(int _i3204 = {0}; (_i3204 < 125); (_i3204++))
{
float _h3205 = f_scene((_ro + (_rd3198 * _dist3200)))._dist;
(_shade3199 = min(_shade3199, smoothstep(0.0, 1.0, ((_k * _h3205) / _dist3200))));
(_dist3200 += min(_h3205, (_stepDist3202 * 2.0)));
if (((_h3205 < 0.001) || (_dist3200 > _end3201)))
{
break;
}
}
}
return min(max(_shade3199, 0.30000001), 1.0);
}
float3 f_getNormalHex(in float3 _pos)
{
float _d3208 = f_scene(_pos)._dist;
return normalize(vec3_ctor((f_scene((_pos + float3(0.001, 0.0, 0.0)))._dist - _d3208), (f_scene((_pos + float3(0.0, 0.001, 0.0)))._dist - _d3208), (f_scene((_pos + float3(0.0, 0.0, 0.001)))._dist - _d3208)));
}
float f_getAO(in float3 _hitp, in float3 _normal, in float _dist)
{
float3 _spos3213 = (_hitp + (_normal * _dist));
float _sdist3214 = f_scene(_spos3213)._dist;
return clamp((_sdist3214 / _dist), 0.40000001, 1.0);
}
float3 f_Sky(in float3 _rd, in bool _showSun, in float3 _lightDir)
{
float _sunSize3219 = {1.0};
float _sunAmount3220 = max(dot(_rd, _lightDir), 0.1);
float _v3221 = pow((1.2 - max(_rd.y, 0.5)), 1.1);
float3 _sky3222 = lerp(f_fromRGB(255, 200, 100), float3(0.11, 0.12, 0.13), _v3221);
(_sky3222 += ((((_lightColour * _sunAmount3220) * _sunAmount3220) * 1.0) + (_lightColour * min((pow(_sunAmount3220, 10000.0) * _sunSize3219), (1233.0 * _sunSize3219)))));
return clamp(_sky3222, 0.0, 1.0);
}
float3 f_doColor__geometry(in float3 _sp, in float3 _rd, in float3 _sn, in float3 _lp, in _geometry _obj)
{
float3 _sceneCol3229 = {0.0, 0.0, 0.0};
(_lp = (_sp + _lp));
float3 _ld3230 = (_lp - _sp);
float _lDist3231 = max(length((_ld3230 / 2.0)), 0.001);
(_ld3230 /= _lDist3231);
float _diff3233 = max(dot(_sn, _ld3230), _obj._diffuse);
float _spec3234 = max(dot(reflect((-_ld3230), _sn), (-_rd)), (_obj._specular / 2.0));
float3 _objCol3235 = _obj._color;
(_sceneCol3229 += (((_objCol3235 * (_diff3233 + 0.15000001)) * _spec3234) * 0.1));
return _sceneCol3229;
}
void f_mainImage_float4(inout float4 _fragColor, in float2 _fragCoord)
{
float2 _uv3239 = ((_fragCoord.xy / _iResolution.xy) - 0.5);
(_uv3239.y *= 1.2);
(_vol = (gl_texture2D(_iChannel0, float2(0.5, 0.25)).x * 4.0));
float _t23240 = _iTime;
float _sk3241 = (sin(((-_t23240) * 0.1)) * 48.0);
float _ck3242 = (cos(((-_t23240) * 0.1)) * 48.0);
(_light = float3(0.0, 7.0, 100.0));
(_lightDir = _light);
float3 _vuv3244 = {0.0, 1.0, 0.0};
float3 _ro3245 = vec3_ctor(_ck3242, 18.0, _sk3241);
float3 _vrp3246 = {0.0, 0.0, 0.0};
float3 _vpn3247 = normalize((_vrp3246 - _ro3245));
float3 _u3248 = normalize(cross(_vuv3244, _vpn3247));
float3 _v3249 = cross(_vpn3247, _u3248);
float3 _vcv3250 = (_ro3245 + _vpn3247);
float3 _scrCoord3251 = ((_vcv3250 + (((_uv3239.x * _u3248) * _iResolution.x) / _iResolution.y)) + (_uv3239.y * _v3249));
float3 _rd3252 = normalize((_scrCoord3251 - _ro3245));
float3 _hit3253 = {0, 0, 0};
float3 _sceneColor3254 = {0.0, 0.0, 0.0};
_geometry _tr3255 = f_enchancedRayMarcher(_ro3245, _rd3252, 0);
(_hit3253 = (_ro3245 + (_rd3252 * _tr3255._dist)));
float3 _sn3257 = f_getNormalHex(_hit3253);
float _sh3258 = f_softShadow(_hit3253, (_hit3253 + _light), 8.1999998);
float _ao3259 = f_getAO(_hit3253, _sn3257, 10.2);
(_noise = f_fbm((mod_emu(((_hit3253 + 20.0) + (_hit3253.yzx * 4.0)), 40.0) / 1.0)));
float3 _sky3260 = (f_Sky(_rd3252, true, normalize(_light)) * 1.0);
if ((_tr3255._dist < 570.0))
{
(_sceneColor3254 = ((f_doColor__geometry(_hit3253, _rd3252, _sn3257, _light, _tr3255) * 1.0) * 1.0));
(_sceneColor3254 *= _ao3259);
(_sceneColor3254 *= _sh3258);
(_sceneColor3254 = lerp(_sceneColor3254, _sky3260, f_saturate(((_tr3255._dist * 4.5) / 570.0))));
}
else
{
(_sceneColor3254 = _sky3260);
}
(_fragColor = vec4_ctor(clamp((_sceneColor3254 * (1.0 - (length(_uv3239) / 3.5))), 0.0, 1.0), 1.0));
(_fragColor = pow(_fragColor, float4(0.83333331, 0.83333331, 0.83333331, 0.83333331)));
}

// INITIAL HLSL END
