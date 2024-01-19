#define GLAD_GLES2_IMPLEMENTATION
#include <glad/gles2.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#if 0
#include <stdbool.h>
#else
#undef bool
typedef unsigned char bool;
#endif

typedef uint32_t Color;
#if 1
#define COLOR_0 0xf4debefful
#define COLOR_1 0xbfa082fful
#define COLOR_2 0x4d675afful
#define COLOR_3 0x847b45fful
#define COLOR_4 0xf5aa5ffful
#define COLOR_5 0xc79773fful
#else
#define COLOR_0 0xf9f7f1fful
#define COLOR_1 0xf6dbbcfful
#define COLOR_2 0xcaede3fful
#define COLOR_3 0xd3b94ffful
#define COLOR_4 0xb78763fful
#endif

/* static double lerp(double v0, double v1, double t) { return v0 + t * (v1 - v0); } */
/* static double inv_lerp(double min, double max, double p) { return (p - min) / (max - min); } */

/* static float lerpf(float v0, float v1, float t) { return v0 + t * (v1 - v0); } */
/* static float inv_lerpf(float min, float max, float p) { return (p - min) / (max - min); } */
void norm2(float *x, float *y) {
    float len;
    if (*x == 0 && *y == 0)return;

    len = sqrtf(*x**x + *y**y);
    *x /= len;
    *y /= len;
}
#define COUNT(ARR) ((sizeof(ARR) / sizeof(ARR[0])))

static void mat4x4_invert(float t[4][4], float m[4][4]) {
    float idet;
    float s[6];
    float c[6];

    s[0] = m[0][0]*m[1][1] - m[1][0]*m[0][1];
    s[1] = m[0][0]*m[1][2] - m[1][0]*m[0][2];
    s[2] = m[0][0]*m[1][3] - m[1][0]*m[0][3];
    s[3] = m[0][1]*m[1][2] - m[1][1]*m[0][2];
    s[4] = m[0][1]*m[1][3] - m[1][1]*m[0][3];
    s[5] = m[0][2]*m[1][3] - m[1][2]*m[0][3];

    c[0] = m[2][0]*m[3][1] - m[3][0]*m[2][1];
    c[1] = m[2][0]*m[3][2] - m[3][0]*m[2][2];
    c[2] = m[2][0]*m[3][3] - m[3][0]*m[2][3];
    c[3] = m[2][1]*m[3][2] - m[3][1]*m[2][2];
    c[4] = m[2][1]*m[3][3] - m[3][1]*m[2][3];
    c[5] = m[2][2]*m[3][3] - m[3][2]*m[2][3];
    
    /* assumes it is invertible */
    idet = 1.0f/( s[0]*c[5]-s[1]*c[4]+s[2]*c[3]+s[3]*c[2]-s[4]*c[1]+s[5]*c[0] );
    
    t[0][0] = ( m[1][1] * c[5] - m[1][2] * c[4] + m[1][3] * c[3]) * idet;
    t[0][1] = (-m[0][1] * c[5] + m[0][2] * c[4] - m[0][3] * c[3]) * idet;
    t[0][2] = ( m[3][1] * s[5] - m[3][2] * s[4] + m[3][3] * s[3]) * idet;
    t[0][3] = (-m[2][1] * s[5] + m[2][2] * s[4] - m[2][3] * s[3]) * idet;

    t[1][0] = (-m[1][0] * c[5] + m[1][2] * c[2] - m[1][3] * c[1]) * idet;
    t[1][1] = ( m[0][0] * c[5] - m[0][2] * c[2] + m[0][3] * c[1]) * idet;
    t[1][2] = (-m[3][0] * s[5] + m[3][2] * s[2] - m[3][3] * s[1]) * idet;
    t[1][3] = ( m[2][0] * s[5] - m[2][2] * s[2] + m[2][3] * s[1]) * idet;

    t[2][0] = ( m[1][0] * c[4] - m[1][1] * c[2] + m[1][3] * c[0]) * idet;
    t[2][1] = (-m[0][0] * c[4] + m[0][1] * c[2] - m[0][3] * c[0]) * idet;
    t[2][2] = ( m[3][0] * s[4] - m[3][1] * s[2] + m[3][3] * s[0]) * idet;
    t[2][3] = (-m[2][0] * s[4] + m[2][1] * s[2] - m[2][3] * s[0]) * idet;

    t[3][0] = (-m[1][0] * c[3] + m[1][1] * c[1] - m[1][2] * c[0]) * idet;
    t[3][1] = ( m[0][0] * c[3] - m[0][1] * c[1] + m[0][2] * c[0]) * idet;
    t[3][2] = (-m[3][0] * s[3] + m[3][1] * s[1] - m[3][2] * s[0]) * idet;
    t[3][3] = ( m[2][0] * s[3] - m[2][1] * s[1] + m[2][2] * s[0]) * idet;
}
static void mat4x4_mul_vec4(float r[4], float m[4][4], const float v[4]) {
    int i, j;
    for(j=0; j<4; ++j) {
        r[j] = 0.f;
        for(i=0; i<4; ++i)
            r[j] += m[i][j] * v[i];
    }
}


/* PLATFORM { */

static void error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

static struct {
    float mvp[4][4];

    struct { float x; float y; } cursor; /* in world space, converted with mvp */
    uint8_t keysdown[GLFW_KEY_LAST + 1];
} input = {0};
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    /* "not release" sounds like a double negative, but GLFW_REPEAT ... */
    input.keysdown[key] = !(action == GLFW_RELEASE);
}
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    float v[4] = {0};
    v[0] = xpos;
    v[1] = ypos;
    v[2] = 0;
    v[3] = 1;

    mat4x4_mul_vec4(v, input.mvp, v);
}

/* } PLATFORM */



/* GRAPHIX { */

static GLuint glayout_vertex_pos;
static GLuint glayout_vertex_col;
typedef struct Vertex {
    struct { float x; float y; float z; } pos;
    Color rgba;
} Vertex;

static const char *vertex_shader_text =
"#version 100\n"
"precision mediump float;\n"
"uniform mat4 u_mvp;\n"
"attribute vec4 v_col;\n"
"attribute vec2 v_pos;\n"
"varying vec4 a_col;\n"
"void main()\n"
"{\n"
"    gl_Position = u_mvp * vec4(v_pos, 0.0, 1.0);\n"
"    a_col = v_col;\n"
"}\n";

/* #define SRGB */
static const char *fragment_shader_text =
"#version 100\n"
"precision mediump float;\n"
#ifdef SRGB
    "// Converts a color from linear light gamma to sRGB gamma\n"
    "vec4 fromLinear(vec4 linearRGB)\n"
    "{\n"
    "    bvec3 cutoff = lessThan(linearRGB.rgb, vec3(0.0031308));\n"
    "    vec3 higher = vec3(1.055)*pow(linearRGB.rgb, vec3(1.0/2.4)) - vec3(0.055);\n"
    "    vec3 lower = linearRGB.rgb * vec3(12.92);\n"
    "\n"
    "    return vec4(mix(higher, lower, vec3(cutoff)), linearRGB.a);\n"
    "}\n"
#endif 
"\n"
"varying vec4 a_col;\n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(a_col.a, a_col.b, a_col.g, a_col.r);\n"
#ifdef SRGB
"    gl_FragColor = fromLinear(gl_FragColor);\n"
#endif
"}\n";

typedef struct {
    uint32_t vbuf_count, ibuf_count;
    struct { GLuint vbuf, ibuf; } gpu;
    struct {
        Vertex *vbuf;
        uint16_t *ibuf;
        uint32_t ibuf_i, vbuf_i;
    } cpu;
} Geo;

static void geo_gl_bind(Geo *geo) {
    glBindBuffer(GL_ARRAY_BUFFER, geo->gpu.vbuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geo->gpu.ibuf);
}

static void geo_alloc(Geo *geo, uint32_t vbuf_count, uint32_t ibuf_count) {
    size_t vbuf_size = sizeof(Vertex)   * (size_t)vbuf_count;
    size_t ibuf_size = sizeof(uint16_t) * (size_t)ibuf_count;

    geo->vbuf_count = vbuf_count;
    geo->ibuf_count = ibuf_count;

    geo->cpu.vbuf = malloc(vbuf_size);
    geo->cpu.ibuf = malloc(ibuf_size);

    glGenBuffers(1, &geo->gpu.vbuf);
    glGenBuffers(1, &geo->gpu.ibuf);

    geo_gl_bind(geo);

    glBufferData(GL_ARRAY_BUFFER, vbuf_size, NULL, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibuf_size, NULL, GL_STREAM_DRAW);
}

static void geo_gl_vertex_layout(Geo *geo) {
    glEnableVertexAttribArray(glayout_vertex_pos);
    glEnableVertexAttribArray(glayout_vertex_col);
    glVertexAttribPointer(glayout_vertex_pos, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glVertexAttribPointer(glayout_vertex_col, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                          sizeof(Vertex), (void*) offsetof(Vertex, rgba));
}

static void geo_gl_upload(Geo *geo) {
    size_t vbuf_size = sizeof(Vertex)   * (size_t)geo->vbuf_count;
    size_t ibuf_size = sizeof(uint16_t) * (size_t)geo->ibuf_count;

    geo_gl_bind(geo);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vbuf_size, geo->cpu.vbuf);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ibuf_size, geo->cpu.ibuf);
}

static void geo_clear(Geo *geo) {
    geo->cpu.vbuf_i = 0;
    geo->cpu.ibuf_i = 0;
}

static void geo_box(Geo *geo, float x, float y, float w, float h, Color clr) {
    uint16_t start_i = geo->cpu.vbuf_i;

    Vertex br, bl, tl, tr;
    br.pos.x = x + w/2, br.pos.y = y - h/2, br.rgba = clr;
    bl.pos.x = x - w/2, bl.pos.y = y - h/2, bl.rgba = clr;
    tl.pos.x = x - w/2, tl.pos.y = y + h/2, tl.rgba = clr;
    tr.pos.x = x + w/2, tr.pos.y = y + h/2, tr.rgba = clr;

    geo->cpu.vbuf[geo->cpu.vbuf_i++] = br;
    geo->cpu.vbuf[geo->cpu.vbuf_i++] = bl;
    geo->cpu.vbuf[geo->cpu.vbuf_i++] = tl;
    geo->cpu.vbuf[geo->cpu.vbuf_i++] = tr;

    geo->cpu.ibuf[geo->cpu.ibuf_i++] = 0 + start_i;
    geo->cpu.ibuf[geo->cpu.ibuf_i++] = 1 + start_i;
    geo->cpu.ibuf[geo->cpu.ibuf_i++] = 2 + start_i;
    geo->cpu.ibuf[geo->cpu.ibuf_i++] = 3 + start_i;
    geo->cpu.ibuf[geo->cpu.ibuf_i++] = 2 + start_i;
    geo->cpu.ibuf[geo->cpu.ibuf_i++] = 0 + start_i;
}

static void geo_circ(Geo *geo, float x, float y, float r, Color clr) {
    uint16_t idx[] = {
        2, 6, 8,
        0, 1, 8,
        1, 2, 8,
        2, 3, 4,
        4, 5, 2,
        5, 6, 2,
        6, 7, 8
    };

    struct { float x; float y; } vtx[] = {
        {  0.0000f,  1.0000f },
        { -0.6428f,  0.7660f },
        { -0.9848f,  0.1736f },
        { -0.8660f, -0.5000f },
        { -0.3420f, -0.9397f },
        {  0.3420f, -0.9397f },
        {  0.8660f, -0.5000f },
        {  0.9848f,  0.1736f },
        {  0.6428f,  0.7660f }
    };

    uint16_t start_i = geo->cpu.vbuf_i;

    int i;

    for (i = 0; i < COUNT(idx); i++) {
        geo->cpu.ibuf[geo->cpu.ibuf_i++] = idx[i] + start_i;
    }

    for (i = 0; i < COUNT(vtx); i++) {
        Vertex v = {0};
        v.pos.x = x + vtx[i].x * r;
        v.pos.y = y + vtx[i].y * r;
        v.rgba = clr;
        geo->cpu.vbuf[geo->cpu.vbuf_i++] = v;
    }
}

/* } GRAPHIX */



/* SIMULATION { */

typedef unsigned int Ticks;

#define FRAME_SECS               (1.0 / 120.0)
#define ACTOR_MOVE_TURN_AGILITY  (0.008f)
#define ACTOR_MOVE_MAX_SPEED     (FRAME_SECS * 5.0f)
#define ACTOR_MOVE_ACCEL         (0.07f)
#define ACTOR_STOP_DECEL         (0.15f)
#define SIM_CAM_SPEED            (FRAME_SECS * 2.5f)

typedef struct {
    double dt;
    Ticks ticks;

    struct {
        float x; float vx;
        float y; float vy;
    } actor;

    struct { float x; float old_x;
             float y; float old_y; } cam;
} Sim;

void sim_init(Sim *sim) {
    sim->cam.x = -(sim->actor.x = 0.0f);
    sim->cam.y = -(sim->actor.y = 0.0f);

    /* ticks == 0 is special */
    sim->ticks = 1;
}

void sim_step(Sim *sim, double dt) {
    /* avoid stepping more than 3 frames at once */
    dt = fmin(dt, FRAME_SECS * 3.0);

    sim->dt += dt;

    while (sim->dt >= FRAME_SECS) {
        sim->dt -= FRAME_SECS;
        sim->ticks += 1;

        /* actor movement */
        {
            float dx = 0;
            float dy = 0;
            bool moving;

            if (input.keysdown[GLFW_KEY_W]) dy += 1.0f;
            if (input.keysdown[GLFW_KEY_S]) dy -= 1.0f;
            if (input.keysdown[GLFW_KEY_A]) dx -= 1.0f;
            if (input.keysdown[GLFW_KEY_D]) dx += 1.0f;

            moving = (dx != 0) || (dy != 0);
            if (moving) norm2(&dx, &dy);

            {
                float speed = sqrtf(sim->actor.vx*sim->actor.vx + sim->actor.vy*sim->actor.vy);
                float dir_x = sim->actor.vx + dx*ACTOR_MOVE_TURN_AGILITY;
                float dir_y = sim->actor.vy + dy*ACTOR_MOVE_TURN_AGILITY;
                norm2(&dir_x, &dir_y);

                {
                    float ideal_speed = moving ? ACTOR_MOVE_MAX_SPEED : 0.0f;
                    /* break speed delta apart into sign and magnitude */
                    float delta_mag = fabsf(speed - ideal_speed);
                    float delta_sign = ((speed - ideal_speed) < 0) ? 1 : -1;
                    float speed_step = FRAME_SECS * (moving ? ACTOR_MOVE_ACCEL : ACTOR_STOP_DECEL);

                    if (delta_mag < speed_step) {
                        speed = ideal_speed;
                    } else {
                        speed += speed_step * delta_sign;
                    }
                }

                sim->actor.vx = dir_x * speed;
                sim->actor.vy = dir_y * speed;

                sim->actor.x += sim->actor.vx;
                sim->actor.y += sim->actor.vy;
            }
        }

        /* camera */
        {
            float ideal_cam_x = -sim->actor.x;
            float ideal_cam_y = -sim->actor.y;

            float dx = ideal_cam_x - sim->cam.x;
            float dy = ideal_cam_y - sim->cam.y;

            if (dx != 0 || dy != 0) {
                float len = 0;
                len = sqrtf(dx*dx + dy*dy);
                dx /= len;
                dy /= len;

                /* move camera slower if we're already close */
                {
                    float t;
                    t = len * SIM_CAM_SPEED;

                    dx *= t;
                    dy *= t;
                }

                sim->cam.x += dx;
                sim->cam.y += dy;
            }
        }
    }
}

/* } SIMULATION */



int main(void) {
    GLFWwindow *window;
    GLint program, mvp_location;
    Geo geo = {0};
    Sim sim = {0};

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifndef WIN32
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif

    window = glfwCreateWindow(640, 480, "tomfoolery", NULL, NULL);
    if (!window) {
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
        window = glfwCreateWindow(640, 480, "tomfoolery", NULL, NULL);
        if (!window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLES2(glfwGetProcAddress);
    glfwSwapInterval(1);

    /* compile shaders, extract attribute locations */
    {
        GLint shader_ok;
        GLsizei log_length;
        char info_log[8192];

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
        glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);

        program = glCreateProgram();
        glCompileShader(vertex_shader);
        glCompileShader(fragment_shader);
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_ok);
        if (shader_ok != GL_TRUE)
        {
            fprintf(stderr, "ERROR: failed to compile vertex shader\n");
            glGetShaderInfoLog(vertex_shader, 8192, &log_length,info_log);
            fprintf(stderr, "ERROR: \n%s\n\n", info_log);
            glDeleteShader(vertex_shader);
            return 1;
        }

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_ok);
        if (shader_ok != GL_TRUE)
        {
            fprintf(stderr, "ERROR: Failed to compile fragment shader\n");
            glGetShaderInfoLog(fragment_shader, 8192, &log_length, info_log);
            fprintf(stderr, "ERROR: \n%s\n\n", info_log);
            glDeleteShader(fragment_shader);
            return 1;
        }

        mvp_location = glGetUniformLocation(program, "u_mvp");
        glayout_vertex_pos = glGetAttribLocation(program, "v_pos");
        glayout_vertex_col = glGetAttribLocation(program, "v_col");
    }

    /* allocate dynamic, per-frame geometry buffer */
    geo_alloc(&geo, 1 << 9, 1 << 12);
    sim_init(&sim);

    glfwSetTime(0);
    while (!glfwWindowShouldClose(window)) {
        float mvp[4][4] = {0};
        float ratio;
        int width, height;
        {
            glfwGetFramebufferSize(window, &width, &height);
            ratio = width / (float) height;
        }

        sim_step(&sim, glfwGetTime());
        glfwSetTime(0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(
            (float)(((0xfful << 24) & COLOR_0) >> 24) / 255.0f,
            (float)(((0xfful << 16) & COLOR_0) >> 16) / 255.0f,
            (float)(((0xfful << 8 ) & COLOR_0) >> 8 ) / 255.0f,
            1.0f
        );

        /* generate dynamic frame geometry */
        {
            geo_clear(&geo);
#if 0
            /* color swatches */
            geo_box(&geo, sim.actor.x+1, sim.actor.y+1, 1.0f, 1.0f, COLOR_1);
            geo_box(&geo, sim.actor.x+2, sim.actor.y+2, 1.0f, 1.0f, COLOR_2);
            geo_box(&geo, sim.actor.x+3, sim.actor.y+3, 1.0f, 1.0f, COLOR_3);
            geo_box(&geo, sim.actor.x+4, sim.actor.y+4, 1.0f, 1.0f, COLOR_4);

            geo_circ(&geo, sim.actor.x-1, sim.actor.y-1, 0.5f, COLOR_1);
            geo_circ(&geo, sim.actor.x-2, sim.actor.y-2, 0.5f, COLOR_2);
            geo_circ(&geo, sim.actor.x-3, sim.actor.y-3, 0.5f, COLOR_3);
            geo_circ(&geo, sim.actor.x-4, sim.actor.y-4, 0.5f, COLOR_4);
#else
            geo_box(&geo, sim.actor.x, sim.actor.y, 1.0f, 1.0f, COLOR_2);
            geo_box(&geo, input.cursor.x, input.cursor.y, 0.1f, 0.1f, COLOR_2);


            {
                int i;
                for (i = 0; i < 25; i++) {
                    float r = ((float)i / 25.0f) * 3.14159f * 2.0f;
                    float x = cosf(r) * 5.0f;
                    float y = sinf(r) * 5.0f;
                    geo_circ(&geo, x, y, 0.6f, COLOR_5);
                }
            }
#endif

            geo_gl_upload(&geo);
        }

        /* prepare to draw dynamic geometry */
        {
            glUseProgram(program);
            geo_gl_vertex_layout(&geo);
        }

        /* prepre mvp */
        {
            float zoom = 10.0f/2.0f; /* 10 units on screen */
            float l = -ratio*zoom - sim.cam.x;
            float r =  ratio*zoom - sim.cam.x;
            float b =  -1.0f*zoom - sim.cam.y;
            float t =   1.0f*zoom - sim.cam.y;
            float n =   1.0f;
            float f =  -1.0f;

            mvp[0][0] = 2.f/(r-l);
            mvp[0][1] = mvp[0][2] = mvp[0][3] = 0.f;

            mvp[1][1] = 2.f/(t-b);
            mvp[1][0] = mvp[1][2] = mvp[1][3] = 0.f;

            mvp[2][2] = -2.f/(f-n);
            mvp[2][0] = mvp[2][1] = mvp[2][3] = 0.f;

            mvp[3][0] = -(r+l)/(r-l);
            mvp[3][1] = -(t+b)/(t-b);
            mvp[3][2] = -(f+n)/(f-n);
            mvp[3][3] = 1.f;

            {
                float tmp[4][4] = {0};
                memcpy(tmp, mvp, sizeof(mvp));
                mat4x4_invert(input.mvp, tmp);
            }

            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        }

        glDrawElements(GL_TRIANGLES, geo.cpu.ibuf_i, GL_UNSIGNED_SHORT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
