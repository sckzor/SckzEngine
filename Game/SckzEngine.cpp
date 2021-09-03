#include "SckzEngine.hpp"

int main()
{
    sckz::InitRand();

    sckz::Window win;
    win.CreateWindow("SckzEngine", 700, 500, true);

    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);

    win.SetIcon("Resources/icon.png");

    vkan.SetFPS(-1);

    bool isMenuOpen = false;

    sckz::Scene &            s1 = vkan.CreateScene();
    sckz::GraphicsPipeline & f1 = vkan.CreateFBOPipeline("Resources/fbo_fragment_normal.spv");

    sckz::GraphicsPipeline & p1 = s1.CreatePipeline("Resources/simple_vertex.spv",
                                                    "Resources/simple_fragment.spv",
                                                    "Resources/cubemap_render_vertex.spv",
                                                    "Resources/cubemap_render_fragment.spv");

    sckz::Model & m1 = s1.CreateModel("Resources/teapot.fbx",
                                      "Resources/barrelColor.png",
                                      "Resources/barrelNormal.png",
                                      "Resources/barrelSpecular.png",
                                      p1);

    sckz::Model & m3 = s1.CreateModel("Resources/room.obj", "Resources/RoomTextureAtlas.png", nullptr, nullptr, p1);
    sckz::Light & l1 = s1.CreateLight(true);

    sckz::ParticleSystem & pa1 = s1.CreateParticleSystem(1000, "Resources/fireParticles.png", 4, 4, 15);

    sckz::Light & l2 = s1.CreateLight(true);

    sckz::Entity & e1 = s1.CreateEntity(m1, true);
    sckz::Entity & e3 = s1.CreateEntity(m3, false);
    sckz::Camera & c1 = s1.CreateCamera(70, 0.1, 100);

    sckz::Timer & fullScreenTimer = vkan.CreateTimer();
    fullScreenTimer.ResetTimer();

    sckz::Timer & menuTimer = vkan.CreateTimer();
    menuTimer.ResetTimer();

    sckz::Button button;
    button.CreateButton(vkan, "Resources/icon.png");

    sckz::Button quitButton;
    quitButton.CreateButton(vkan, "Resources/Quit.png");
    quitButton.SetLocation(0, -120);

    sckz::Button backButton;
    backButton.CreateButton(vkan, "Resources/BackToApp.png");
    backButton.SetLocation(0, 120);

    sckz::Button fullScreenButton;
    fullScreenButton.CreateButton(vkan, "Resources/ToggleFullScreen.png");
    fullScreenButton.SetLocation(0, 0);

    // sckz::Gui & gui = vkan.CreateGUI("Resources/icon.png");

    sckz::Fbo & fbo = s1.CreateFbo();

    SoundDevice soundDevice;
    soundDevice.CreateSoundDevice(nullptr);

    MusicBuffer musicBuffer;
    musicBuffer.CreateMusicBuffer("Resources/Polaroid.wav");

    SoundBuffer & polaroid = soundDevice.CreateSoundBuffer("Resources/Polaroid.wav");
    SoundBuffer & bounce   = soundDevice.CreateSoundBuffer("Resources/bounce.wav");

    SoundSource musicSource;
    musicSource.CreateSource();

    SoundSource barrelSource;
    barrelSource.CreateSource();

    SoundSource sfxSource;
    sfxSource.CreateSource();

    c1.SetListener(&soundDevice);
    c1.SetMusicSource(&musicSource);
    c1.SetSfxSource(&sfxSource);
    musicBuffer.Play();

    e1.SetSoundSource(&barrelSource);

    sckz::Bloom bloom;
    bloom.CreateBloom(s1);

    sckz::Blur blur;
    blur.CreateBlur(s1);

    button.SetScale(200, 200);
    // gui.SetLocation(500, 300);
    // gui.SetRotationPoint(100, 100);

    e1.SetShine(1, 10);
    e1.SetRotation(90, 0, 0);
    e1.SetLocation(0, 0, 0);
    e1.SetScale(0.1, 0.1, 0.1);
    e1.SetReflectRefractValues(1 / 1.33, 0.6);

    l1.SetColor(0, 0.5, 1);
    l1.SetLocation(0, 0, 0);
    l1.SetAttenuation(1, 0.01, 0.002);
    l1.SetDirection(0, -1, 0);
    l1.SetCutoff(12.5, 17.5);

    l2.SetColor(1, 0.5, 0);
    l2.SetLocation(0, 0, 0);
    l2.SetAttenuation(1, 0.01, 0.002);
    l2.SetDirection(-1, 0, 0);
    l2.SetCutoff(12.5, 17.5);

    // e2.SetShine(1, 10);
    // e2.SetRotation(90, 0, 0);
    // e2.SetLocation(0, 0, 0);
    // e2.SetScale(0.1, 0.1, 0.1);

    e3.SetShine(1, 30);
    e3.SetRotation(90, 0, 0);
    e3.SetLocation(0, 0, 0);
    e3.SetReflectRefractValues(1, 0);

    c1.SetLocation(0, 0, 0);
    c1.SetRotation(-90, 0, 0);

    e1.GetSoundSource()->Play(polaroid);
    e1.GetSoundSource()->SetShouldLoop(true);

    while (!win.QueryClose())
    {
        musicBuffer.UpdateBufferStream();
        if (win.QueryKey('w') && !isMenuOpen)
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y - (1 * vkan.GetDeltaT()), c1.GetLocation().z);
        }

        if (win.QueryKey('S') && !isMenuOpen)
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y + (1 * vkan.GetDeltaT()), c1.GetLocation().z);
        }

        if (win.QueryKey('a') && !isMenuOpen)
        {
            c1.SetLocation(c1.GetLocation().x + (1 * vkan.GetDeltaT()), c1.GetLocation().y, c1.GetLocation().z);
        }

        if (win.QueryKey('d') && !isMenuOpen)
        {
            c1.SetLocation(c1.GetLocation().x - (1 * vkan.GetDeltaT()), c1.GetLocation().y, c1.GetLocation().z);
        }

        if (win.QueryKey('z') && !isMenuOpen)
        {
            c1.SetRotation(c1.GetRotation().x, c1.GetRotation().y, c1.GetRotation().z - (30 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('c') && !isMenuOpen)
        {
            c1.SetRotation(c1.GetRotation().x, c1.GetRotation().y, c1.GetRotation().z + (30 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('q') && !isMenuOpen)
        {
            c1.SetRotation(c1.GetRotation().x - (30 * vkan.GetDeltaT()), c1.GetRotation().y, c1.GetRotation().z);
        }

        if (win.QueryKey('e') && !isMenuOpen)
        {
            c1.SetRotation(c1.GetRotation().x + (30 * vkan.GetDeltaT()), c1.GetRotation().y, c1.GetRotation().z);
        }

        if (win.QueryKey(GLFW_KEY_LEFT_SHIFT) && !isMenuOpen)
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y, c1.GetLocation().z + (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey(GLFW_KEY_SPACE) && !isMenuOpen)
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y, c1.GetLocation().z - (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey(GLFW_KEY_TAB) && !isMenuOpen)
        {
            pa1.AddParticle(0, 0, 0, sckz::GetRandomFloat(1, -1), sckz::GetRandomFloat(1, -1), 3);
        }

        if (win.QueryKey('p') && !isMenuOpen)
        {
            vkan.SetFPS(30);
        }

        if (win.QueryKey('l') && !isMenuOpen)
        {
            vkan.SetFPS(-1);
        }

        if (win.QueryKey('i') && !isMenuOpen)
        {
            musicBuffer.SetGain(musicBuffer.GetGain() - 0.0001);
        }

        if (win.QueryKey('o') && !isMenuOpen)
        {
            musicBuffer.SetGain(musicBuffer.GetGain() + 0.0001);
        }

        if (win.QueryKey('g') && !isMenuOpen)
        {
            s1.SetMSAA(8);
        }

        s1.Render(c1, vkan.GetDeltaT());

        if (win.QueryKey('y'))
        {
            vkan.Present(s1.GetRenderedImage(), f1, s1.GetMsaaSamples());
        }
        else
        {
            if (isMenuOpen)
            {
                sckz::Fbo & blurFbo = blur.ApplyBlur();
                vkan.Present(blurFbo, f1, s1.GetMsaaSamples());
            }
            else
            {
                sckz::Fbo & bloomFbo = bloom.ApplyBloom();
                vkan.Present(bloomFbo, f1, s1.GetMsaaSamples());
            }
        }

        if (win.QueryKey('5'))
        {
            l1.SetLocation(l1.GetLocation().x, l1.GetLocation().y + (vkan.GetDeltaT()), l1.GetLocation().z);
        }
        if (win.QueryKey('4'))
        {
            l1.SetLocation(l1.GetLocation().x, l1.GetLocation().y - (vkan.GetDeltaT()), l1.GetLocation().z);
        }
        if (win.QueryKey('1'))
        {
            l1.SetCutoff(l1.GetCutoff(), l1.GetOuterCutoff() + (vkan.GetDeltaT()));
        }
        if (win.QueryKey('2'))
        {
            l1.SetCutoff(l1.GetCutoff() + (vkan.GetDeltaT()), l1.GetOuterCutoff());
        }

        if (win.QueryKey('6'))
        {
            e1.SetLocation(e1.GetLocation().x, e1.GetLocation().y - (10 * vkan.GetDeltaT()), e1.GetLocation().z);
        }

        if (win.QueryKey('7'))
        {
            e1.SetLocation(e1.GetLocation().x, e1.GetLocation().y + (10 * vkan.GetDeltaT()), e1.GetLocation().z);
        }

        if (fullScreenButton.IsPressed(0) && fullScreenTimer.GetCurrentTime() > 1)
        {
            if (win.IsFullScreen())
            {
                win.GoWindowed();
            }
            else
            {
                win.GoFullScreen();
            }

            sfxSource.Play(bounce);

            fullScreenTimer.ResetTimer();
        }

        if ((win.QueryKey(GLFW_KEY_ESCAPE) || backButton.IsPressed(0)) && menuTimer.GetCurrentTime() > 0.25)
        {
            if (isMenuOpen)
            {
                quitButton.SetScale(-1, -1);
                backButton.SetScale(-1, -1);
                fullScreenButton.SetScale(-1, -1);
            }
            else
            {
                quitButton.SetScale(600, 100);
                backButton.SetScale(600, 100);
                fullScreenButton.SetScale(600, 100);
            }

            sfxSource.Play(bounce);

            menuTimer.ResetTimer();
            isMenuOpen = !isMenuOpen;
        }

        if (isMenuOpen && quitButton.IsPressed(0))
        {
            sfxSource.Play(bounce);
            break;
        }

        if (button.IsPressed(0))
        {
            button.SetLocation(win.GetMousePosition().x, win.GetMousePosition().y);
        }

        win.Update();
    }

    musicBuffer.DestroyMusicBuffer();
    musicSource.DestroySource();
    musicSource.Stop();
    sfxSource.DestroySource();
    sfxSource.Stop();
    barrelSource.DestroySource();
    barrelSource.Stop();
    soundDevice.DestroySoundDevice();

    s1.DestroyScene();
    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}