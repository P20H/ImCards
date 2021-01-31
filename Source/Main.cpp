#include <ImFramework.h>
#include <imgui_markdown.h>

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <random>
#include <algorithm>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//-----------------------------------------------------------------------------
// [SECTION] Example App: Docking, DockSpace / ShowExampleAppDockSpace()
//-----------------------------------------------------------------------------

// Demonstrate using DockSpace() to create an explicit docking node within an existing window.
// Note that you dock windows into each others _without_ a dockspace, by just clicking on
// a window title bar and moving it (+ hold SHIFT if io.ConfigDockingWithShift is set).
// DockSpace() and DockSpaceOverViewport() are only useful to construct a central docking
// location for your application.
void ShowExampleAppDockSpace(bool* p_open)
{
    // In 99% case you should be able to just call DockSpaceOverViewport() and ignore all the code below!
    // In this specific demo, we are not using DockSpaceOverViewport() because:
    // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
    // - we allow the host window to have padding (when opt_padding == true)
    // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
    // TL;DR; this demo is more complicated than what you would normally use.
    // If we removed all the options we are showcasing, this demo would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //     }

    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        ///ShowDockingDisabledMessage();
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
            ImGui::Separator();

            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                *p_open = false;
            ImGui::EndMenu();
        }
       /* HelpMarker(
            "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n\n"
            " > if io.ConfigDockingWithShift==false (default):" "\n"
            "   drag windows from title bar to dock" "\n"
            " > if io.ConfigDockingWithShift==true:" "\n"
            "   drag windows from anywhere and hold Shift to dock" "\n\n"
            "This demo app has nothing to do with it!" "\n\n"
            "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)." "\n\n"
            "ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame." "\n\n"
            "(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)"
        );*/

        ImGui::EndMenuBar();
    }

    ImGui::End();
}





bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

static std::string currCardSetPath = "";
static std::string flashcardPath = "";

inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_)
{
    // In your application you would load an image based on data_ input. Here we just use the imgui font texture.
    //ImTextureID image = ImGui::GetIO().Fonts->TexID;
    // > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };

    std::string link = data_.link;

    // 2 -> asume that the relative path begins with ./
    link = link.substr(2, data_.linkLength -2);
    static std::filesystem::path imgPath = "";


    static ImGui::MarkdownImageData imageData;

    auto tmpImgPath = std::filesystem::path(currCardSetPath).parent_path() / std::filesystem::path(link);;
    if (tmpImgPath.compare(imgPath) == 0) {
        return imageData;
    }
    else {
        imgPath = tmpImgPath;
    }

    if (!std::filesystem::exists(imgPath)) {
        return imageData;
    }

    GLuint texture = 0;
    int width = 0;
    int height = 0;
    if (!LoadTextureFromFile(imgPath.string().c_str(), &texture, &width, &height)) {
        return imageData;
    }

    if (imageData.user_texture_id != 0) {
        glDeleteTextures(0, (GLuint*)imageData.user_texture_id);
    }


    imageData.isValid = true;
    imageData.useLinkCallback = false;
    imageData.user_texture_id = (ImTextureID)texture;
    imageData.size = ImVec2(width, height);


    // For image resize when available size.x > image width, add
    ImVec2 const contentSize = ImGui::GetContentRegionAvail();
    if (imageData.size.x > contentSize.x)
    {
        float const ratio = imageData.size.y / imageData.size.x;
        imageData.size.x = contentSize.x;
        imageData.size.y = contentSize.x * ratio;
    }

    return imageData;
}


int main(int argc, char** argv) {
    ImFramework::Init();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    static ImGui::MarkdownConfig mdConfig;


    struct Card {
        std::string Question = "";
        std::string Answer   = "";
    };

    std::vector<std::string> files;

    bool showAnswer = false;

    int currCard = 0;
    std::vector<Card> cards;

    auto exePath = std::filesystem::path(std::string(argv[0]));


    flashcardPath = exePath.parent_path().string() + "\\FlashCards";

    // Load cards
    {

        if (!std::filesystem::exists(std::filesystem::path(flashcardPath))) {
            std::cout << "FlashCards folder does not exits" << std::endl;
            return -1;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(flashcardPath)) {
            auto currPath = entry.path().string();
            if (currPath.find(".md") != std::string::npos) {
                files.push_back(currPath);
            }
        }

    }

    while (ImFramework::Begin()) 
    {

        ImFramework::BeginWindow();
        {
            static bool show = true;

            //mdConfig.linkCallback = LinkCallback;
            mdConfig.tooltipCallback = NULL;
            mdConfig.imageCallback = ImageCallback;
            //mdConfig.linkIcon = ICON_FA_LINK;
            //mdConfig.headingFormats[0] = { H1, true };
            //mdConfig.headingFormats[1] = { H2, true };
            //mdConfig.headingFormats[2] = { H3, false };
            mdConfig.userData = NULL;
           // mdConfig.formatCallback = ExampleMarkdownFormatCallback;



            ShowExampleAppDockSpace(&show);

            // Select set
            {
                ImGui::Begin("Card sets");
                int id = 0;

                static char filterText[255] = {0};

                ImGui::InputText("Filter", filterText, IM_ARRAYSIZE(filterText));

                for (int i = 0; i < files.size(); i++) {

                    // filter
                    if (files[i].find(filterText) != std::string::npos) {
                        // do nothing
                    } else {
                        continue;
                    }

                    ImGui::PushID(id++);

                    ImGui::Text(files[i].substr(flashcardPath.size() + 1, files[i].size() - flashcardPath.size()).c_str());

                    ImGui::SameLine();

                    if (ImGui::Button("Select")) {
                        currCardSetPath = files[i];

                        cards.clear();
                        currCard = 0;

                        std::ifstream file(currCardSetPath);
                        std::string str;
                        while (std::getline(file, str))
                        {
                            if (str.find("##") != std::string::npos) {
                                Card newCard;

                                // parse question
                                std::replace(
                                    str.begin(),
                                    str.end(),
                                    '#',
                                    ' '
                                );

                                newCard.Question = str;
                                cards.push_back(newCard);

                                continue;
                            }
                            if (cards.size() > 0) {
                                cards[cards.size() - 1].Answer.append(str);
                                cards[cards.size() - 1].Answer.append("\n");
                            }
                        }
                    }
                    ImGui::PopID();
                }

                ImGui::End();
            }

            // Current Card
            {

                ImGui::Begin("Current card");

                ImGui::SetWindowFontScale(2);

                if (cards.size() > 0) {
                    ImGui::Markdown(cards[currCard].Question.c_str(), cards[currCard].Question.length(), mdConfig);
                    
                    ImGui::Separator();

                    if (showAnswer) {
                        ImGui::Markdown(cards[currCard].Answer.c_str(), cards[currCard].Answer.length(), mdConfig);
                    }



                }

                ImGui::End();

            }

            // Buttons
            {
                if (cards.size() > 0) {

                    ImGui::Begin("Buttons");

                    {


                        ImGui::Text(currCardSetPath.c_str());
                        ImGui::Text("Anz. Begriffe: %i", cards.size());

                        ImGui::Separator();


                        auto& io = ImGui::GetIO();



                        if (ImGui::Button("Show answer") ||
                            ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_DownArrow]) ||
                            ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_UpArrow])) {

                            showAnswer = !showAnswer;
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Random question")) {
                            std::random_device rd; // obtain a random number from hardware
                            std::mt19937 gen(rd()); // seed the generator
                            std::uniform_int_distribution<> distr(0, cards.size() - 1); // define the range

                            currCard = distr(gen);
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Shuffle")) {
                            auto rng = std::default_random_engine{};
                            std::shuffle(std::begin(cards), std::end(cards), rng);
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Back") || ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_LeftArrow])) {
                            if (currCard > 0) {
                                currCard--;
                            }
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Next") || ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_RightArrow])) {
                            if (currCard != cards.size() - 1) {
                                showAnswer = false;
                                currCard++;
                            }
                        }
                    }

                    ImGui::End();
                }
            }

        }

        ImFramework::EndWindow();


        ImFramework::End();
    }

    ImFramework::Destroy();

    return 0;
}