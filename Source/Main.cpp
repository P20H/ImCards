#include <ImFramework.h>
#include <imgui_markdown.h>

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <random>
#include <algorithm>

int main(int argc, char** argv) {
    ImFramework::Init();

    static ImGui::MarkdownConfig mdConfig;

    struct Card {
        std::string Question = "";
        std::string Answer   = "";
    };

    std::string currCardSetPath = "";
    std::vector<std::string> files;

    int currCard = 0;
    std::vector<Card> cards;


    // Load cards
    {
        auto p = std::filesystem::path(std::string(argv[0]));
        auto q = p.parent_path().string() + "\\FlashCards";
        
        if (!std::filesystem::exists(std::filesystem::path(q))) {
            std::cout << "FlashCards folder does not exits" << std::endl;

            return -1;
        }



        for (const auto& entry : std::filesystem::recursive_directory_iterator(q)) {
            auto currPath = entry.path().string();
            if (currPath.find(".md") != std::string::npos) {
                std::cout << currPath << std::endl;
                files.push_back(currPath);
            }
        }
    }


    while (ImFramework::Begin()) 
    {
        ImFramework::BeginWindow();
        {

            // Select set
            {
                ImGui::Begin("Card sets");
                int id = 0;

                

                for (int i = 0; i < files.size(); i++) {
                    ImGui::PushID(id++);

                    ImGui::Text(files[i].c_str());
                    ImGui::SameLine();
                    if (ImGui::Button("Select")) {
                        currCardSetPath = files[i];
                        cards.clear();
                        
                        std::ifstream file(currCardSetPath);
                        std::string str;
                        while (std::getline(file, str))
                        {
                            Card newCard;
                            if (str.find("##") != std::string::npos) {
                               
                                // parse question
                                std::replace(
                                    str.begin(), 
                                    str.end(), 
                                    '#',
                                    ' '
                                );
                                newCard.Question = str;

                                // read answer
                                while (std::getline(file, str)) {
                                    if (str.find("##") != std::string::npos) {
                                        break;
                                    }
                                    else {
                                        newCard.Answer.append(str);
                                        newCard.Answer.append("\n");
                                    }
                                }

                                cards.push_back(newCard);
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

                if (cards.size() > 0) {

                    ImGui::Markdown(cards[currCard].Question.c_str(), cards[currCard].Question.length(), mdConfig);
                    ImGui::Separator();

                    static bool showAnswer = false;
                    if (showAnswer) {
                        ImGui::Markdown(cards[currCard].Answer.c_str(), cards[currCard].Answer.length(), mdConfig);
                    }


                    // Buttons
                    {
                        auto& io = ImGui::GetIO();

                        if (ImGui::Button("Show answer") || 
                            ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_DownArrow]) || 
                            ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_UpArrow])) {

                            showAnswer = !showAnswer;
                        }

                        if (ImGui::Button("Random question")) {
                            std::random_device rd; // obtain a random number from hardware
                            std::mt19937 gen(rd()); // seed the generator
                            std::uniform_int_distribution<> distr(0, cards.size() - 1); // define the range

                            currCard = distr(gen);
                        }



                        if (ImGui::Button("Shuffle")) {
                            auto rng = std::default_random_engine{};
                            std::shuffle(std::begin(cards), std::end(cards), rng);
                        }


                        if (ImGui::Button("Back") || ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_LeftArrow])) {
                            if (currCard > 0) {
                                currCard--;
                            }
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Next") || ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_RightArrow])) {
                            if (currCard != cards.size() - 1) {
                                currCard++;
                            }
                        }

                        
                    }

                }

                ImGui::End();

            }
        }
        ImFramework::EndWindow();


        ImFramework::End();
    }

    ImFramework::Destroy();

    return 0;
}