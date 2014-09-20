#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <cstdint>
#include <iostream>

int main() try
{
    if(!glfwInit()) throw std::runtime_error("glfwInit() failed.");
    auto window = glfwCreateWindow(1280, 720, "Development Sandbox", nullptr, nullptr);
    if(!window) throw std::runtime_error("glfwCreateWindow(...) failed.");

    glfwMakeContextCurrent(window);

    const uint32_t compressedFont[] = {
        0,0x18,0,0,0x660000,0xc000018,0,0,0x36661800,0xc1c003e,0xc30,0x40000000,0x36663c00,0xc360063,0x1818,0x60000000,0x7f243c00,0x6364343,0x1866300c,0x30000000,0x36003c00,0x1c6303,0x183c300c,
        0x18000000,0x36001800,0x6e303e,0x7eff300c,0xc007f00,0x36001800,0x3b1860,0x183c300c,0x6000000,0x7f000000,0x330c61,0x1866300c,0x3000018,0x36001800,0x336663,0x1818,0x1180018,0x36001800,
        0x6e633e,0xc30,0x180018,0,0x18,0,0xc,0,0x18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3e3e183e,0x7f1c7f30,0x3e3e,0x3e060060,0x63631c63,0x63060338,0x18186363,0x630c0030,0x60601e73,0x6003033c,0x18186363,
        0x63180018,0x6030187b,0x30030336,0x6363,0x30307e0c,0x3c18186f,0x183f3f33,0x7e3e,0x18600006,0x600c1867,0xc63607f,0x6063,0x1830000c,0x60061863,0xc636030,0x18186063,0x187e18,0x63631863,
        0xc636330,0x18183063,0x180c0030,0x3e7f7e3e,0xc3e3e78,0xc001e3e,0x18060060,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3c3f083e,0x3c7f7f1f,0x67783c63,0x1c63630f,0x66661c63,0x66666636,
        0x66301863,0x36677706,0x43663663,0x43464666,0x36301863,0x636f7f06,0x366637b,0x3161666,0x36301863,0x637f7f06,0x33e637b,0x31e1e66,0x1e30187f,0x637b6b06,0x3667f7b,0x7b161666,0x36301863,
        0x63736306,0x4366633b,0x63064666,0x36331863,0x63636346,0x66666303,0x66066636,0x66331863,0x36636366,0x3c3f633e,0x5c0f7f1f,0x671e3c63,0x1c63637f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x80000,0,0,0,
        0x1c0000,0x3e3f3e3f,0x6363637e,0x3c7f6663,0x363c01,0x63666366,0x6363637e,0xc636663,0x633003,0x63666366,0x6363635a,0xc316636,0x3007,0x6666366,0x63636318,0xc18661c,0x300e,0x1c3e633e,
        0x6b636318,0xc0c3c1c,0x301c,0x30366b06,0x6b636318,0xc06181c,0x3038,0x63667b06,0x7f366318,0xc431836,0x3070,0x63663e06,0x3e1c6318,0xc631863,0x3060,0x3e67300f,0x36083e3c,0x3c7f3c63,0x3c40,
        0x7000,0,0,0,0,0,0,0xff000000,0,0,0,0,0xc,0,0,0,0xc,0,0,0,0x70018,0x1c0038,0x7601807,0x1c,0x60000,0x360030,0x6601806,0x18,0x60000,0x260030,0x6000006,0x18,0x3e1e1e00,0x6e063e3c,0x66701c36,
        0x3e3b3718,0x63363000,0x330f6336,0x3660186e,0x63667f18,0x3663e00,0x33067f33,0x1e601866,0x63666b18,0x3663300,0x33060333,0x36601866,0x63666b18,0x63663300,0x3e066333,0x66601866,0x63666b18,
        0x3e3e6e00,0x300f3e6e,0x67663c67,0x3e66633c,0,0x33000000,0x660000,0,0,0x1e000000,0x3c0000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x8,0x70000000,0x6e0e18,0,0xc,0x18000000,0x3b1818,0,0xc,0x18000000,
        0x1818,0x3e3b6e3b,0x6366333f,0x187f6363,0x1818,0x636e3366,0x6366330c,0xe336336,0x7000,0xe663366,0x6b66330c,0x1818631c,0x1818,0x38063366,0x6b66330c,0x180c631c,0x1818,0x63063e3e,0x7f3c336c,
        0x18667e36,0x1818,0x3e0f3006,0x36186e38,0x707f6063,0xe18,0x3006,0,0x3000,0,0x780f,0,0x1f00 };
    uint8_t fontPixels[128*128] = {};
    auto out = fontPixels;
    for(auto num : compressedFont)
    {
        for(int i=0; i<32; ++i)
        {
            *out++ = num & (1 << i) ? 255 : 0;
        }
    }

    // Load font texture
    GLuint fontTexture;
    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, fontPixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glfwMakeContextCurrent(window);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glOrtho(0, width, height, 0, -1, +1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2f(32,32);
        glTexCoord2f(1,0); glVertex2f(280,32);
        glTexCoord2f(1,1); glVertex2f(280,280);
        glTexCoord2f(0,1); glVertex2f(32,280);

        char message[] = "This is a test of basic font rendering.";
        int x0 = 32, y0 = 320;
        for(auto ch : message)
        {
            int cx = (ch - 32) % 16, cy = (ch - 32) / 16;

            float s0 = cx * 8.0f/128, s1 = (cx+1) * 8.0f/128, t0 = cy * 14.0f/128, t1 = t0 + 14.0f/128;
            int x1 = x0 + 8, y1 = y0 + 14;

            glTexCoord2f(s0,t0); glVertex2f(x0,y0);
            glTexCoord2f(s1,t0); glVertex2f(x1,y0);
            glTexCoord2f(s1,t1); glVertex2f(x1,y1);
            glTexCoord2f(s0,t1); glVertex2f(x0,y1);  

            x0 += 8;          
        }

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
catch(const std::exception & e)
{
    std::cerr << "Unhandled exception: " << e.what() << std::endl;
    return -1;
}