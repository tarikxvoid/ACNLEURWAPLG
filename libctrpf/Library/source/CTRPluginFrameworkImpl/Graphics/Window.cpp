#include "CTRPluginFrameworkImpl/Graphics/Window.hpp"
#include "CTRPluginFrameworkImpl/Preferences.hpp"
#include "CTRPluginFramework/Graphics/Color.hpp"
#include "CTRPluginFrameworkImpl/Graphics/Icon.hpp"
#include "CTRPluginFrameworkImpl/Graphics/Renderer.hpp"
#include "CTRPluginFramework/System/Lock.hpp"
#include "CTRPluginFramework/System/System.hpp"

namespace CTRPluginFramework
{
	// DO NOT REMOVE THIS COPYRIGHT NOTICE
    static const char g_ctrpfText[] = "Powered by CTRPluginFramework";
    static const char g_copyrightText[] = "Copyright (c) The Pixellizer Group";
    //vapecord infobar
    static const char g_credits[] = "Made by cr4zyskye (Forked from Vapecord-ACNL-Plugin)";
    static const char g_discord[] = "Support Server: discord.gg/QwqdBpKWf3";
    
    static u32 g_textXpos[4] = { 0 };
    
    Window  Window::BottomWindow = Window(20, 20, 280, 200, true, nullptr);
    Window  Window::TopWindow = Window(30, 20, 340, 200, false, nullptr);
    
    //vapecord infobar
    Window  Window::TopWindow_TopBar = Window(30, 0, 340, 22, false, nullptr);
    Window  Window::TopWindow_BottomBar = Window(30, 218, 340, 22, false, nullptr);

    Window  Window::BottomWindow_TopBar = Window(20, 0, 280, 22, false, nullptr);
    Window  Window::BottomWindow_BottomBar = Window(20, 218, 280, 22, false, nullptr);

    Window::Window(u32 posX, u32 posY, u32 width, u32 height, bool closeBtn, BMPImage *image) :
    _rect(posX, posY, width, height),
    _border(posX + 2, posY + 2, width - 4, height - 4),
    _image(image)
    {
        if (closeBtn)
        {
            _closeBtn = new Button(Button::Icon, IntRect(posX + width - 25, posY + 4, 20, 20), Icon::DrawClose);
            _closeBtn->SetAcceptSoundEvent(SoundEngine::Event::NUM_EVENTS);
        }

        else
            _closeBtn = nullptr;
    }

    Window::~Window(void)
    {
        delete _closeBtn;
        _closeBtn = nullptr;
    }

    void    Window::Draw(void) const
    {
        // Background
        if (_image != nullptr)
            _image->Draw(_rect.leftTop);
        else
        {
            Renderer::DrawRect2(_rect, Preferences::Settings.BackgroundMainColor, Preferences::Settings.BackgroundSecondaryColor);
            Renderer::DrawRect(_border, Preferences::Settings.BackgroundBorderColor, false);
        }

        // Close button
        if (_closeBtn != nullptr)
            _closeBtn->Draw();
    }

    void    Window::Draw(const std::string& title) const
    {
        // Background
        if (_image != nullptr)
            _image->Draw(_rect.leftTop);
        else
        {
            Renderer::DrawRect2(_rect, Preferences::Settings.BackgroundMainColor, Preferences::Settings.BackgroundSecondaryColor);
            Renderer::DrawRect(_border, Preferences::Settings.BackgroundBorderColor, false);
        }

        // Title
        Color &color = Preferences::Settings.WindowTitleColor;
        int posY = _rect.leftTop.y + 5;
        int posX = _rect.leftTop.x + 10;
        int xx = Renderer::DrawSysString(title.c_str(),posX, posY, 330, color);
        Renderer::DrawLine(posX, posY, xx, color);

        // Close button
        if (_closeBtn != nullptr)
            _closeBtn->Draw();
    }

    void    Window::DrawButton(void) const
    {
        // Close button
        if (_closeBtn != nullptr)
            _closeBtn->Draw();
    }

    void    Window::Update(const bool isTouched, const IntVector &touchPos) const
    {
        if (_closeBtn != nullptr)
            _closeBtn->Update(isTouched, touchPos);
    }

    bool    Window::MustClose(void) const
    {
        if (_closeBtn == nullptr)
            return (false);

        return ((*_closeBtn)());
    }

    void    Window::Close(void) const
    {
        if (_closeBtn)
        {
            _closeBtn->SetState(true);
            _closeBtn->Execute();
        }
    }

    const   IntRect &Window::GetRect(void) const
    {
        return (_rect);
    }

    void    Window::UpdateBackgrounds(void)
    {
        BottomWindow._image = Preferences::bottomBackgroundImage;
        TopWindow._image = Preferences::topBackgroundImage;
    }

	//vapecord infobar
	void GetTimeStamp(char* out)
    {
        time_t rawtime;
		struct tm * timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(out, 80, "%r | %F", timeinfo);
    }

    bool FCLoaded = false;
    void GetStringFC(std::string &FC) 
    {
        if(FCLoaded)
            return;

		std::string str = Utils::Format("%012lld", System::GetFriendCode());
		FC = Utils::Format("FC: %s - %s - %s", str.substr(0, 4).c_str(), str.substr(4, 4).c_str(), str.substr(8, 4).c_str());
        FCLoaded = true;
    }

    bool posLoaded = false;
    void LoadPos(void)
    {
        if(posLoaded)
            return;

        // Get strings x position
        g_textXpos[0] = (320 - Renderer::LinuxFontSize(g_ctrpfText)) / 2;
        g_textXpos[1] = (320 - Renderer::LinuxFontSize(g_copyrightText)) / 2;
        g_textXpos[2] = (320 - Renderer::LinuxFontSize(g_discord)) / 2;
        g_textXpos[3] = (320 - Renderer::LinuxFontSize(g_credits)) / 2;
        posLoaded = true;
    }

    void    Window::DrawTopInfoBar(void)
    {
        const Color &maintext = Preferences::Settings.MainTextColor;
        int posY = 3;

        Window::TopWindow_TopBar.Draw();
        Window::TopWindow_BottomBar.Draw();

        //draw info for top bar
        if (!System::IsCitra()) {
            char timestamp[80];
            GetTimeStamp(timestamp);
            float percentage = System::GetBatteryPercentage();
            
            Renderer::DrawSysString(timestamp, 35, posY, 400, maintext);
            posY = 3;
            Renderer::DrawSysString(Utils::Format("%d%%", (u32)percentage).c_str(), 310, posY, 400, maintext);
        }
        else
            Renderer::DrawSysString("Citra", 35, posY, 400, maintext);
        //draw info for bottom bar
        static std::string FC = "";
        GetStringFC(FC);

        posY = 221;
        Renderer::DrawSysString(FC.c_str(), 35, posY, 400, maintext);
    }

    void    Window::DrawBottomInfoBar(void)
    {
        const Color& blank = Color::White;
        static Clock creditClock;
        static bool framework = true;

        Window::BottomWindow_TopBar.Draw();
        Window::BottomWindow_BottomBar.Draw();
        LoadPos();

        int posY = 6;

        //draw info for top bar
        if (framework)
            Renderer::DrawString(g_discord, g_textXpos[2], posY, blank);
        else
            Renderer::DrawString(g_credits, g_textXpos[3], posY, blank);

        //draw info for bottom bar
        posY = 224;
        if (framework)
            Renderer::DrawString(g_ctrpfText, g_textXpos[0], posY, blank);
        else
            Renderer::DrawString(g_copyrightText, g_textXpos[1], posY, blank);

        if (creditClock.HasTimePassed(Seconds(5)))
        {
            creditClock.Restart();
            framework = !framework;
        }
    }
}
