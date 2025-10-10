#include "UI/AssetsPanel/AssetsPanel.hpp"

AssetsPanel::AssetsPanel(SceneManager& sceneManager, ComponentRegistry& componentRegistry)
    : _sceneManager(sceneManager), _componentRegistry(componentRegistry) {}

void AssetsPanel::render()
{
    if (ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Imported Assets: %zu", this->_assets.size());
        ImGui::Separator();

        if (this->_assets.empty()) {
            ImGui::TextDisabled("No assets imported yet");
            ImGui::Text("Use the Import button to add images or 3D models");
        } else {
            float thumbnailSize = 80.0f;
            float cardWidth = 100.0f;
            float cardHeight = 120.0f;
            float spacing = 10.0f;
            float windowWidth = ImGui::GetContentRegionAvail().x;
            int columns = std::max(1, (int)(windowWidth / (cardWidth + spacing)));

            for (size_t i = 0; i < this->_assets.size(); ++i) {
                ImGui::PushID(i);

                const AssetInfo& asset = this->_assets[i];

                ImGui::BeginGroup();

                this->_renderAssetThumbnail(asset);

                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + cardWidth);
                ImGui::Text("%s", asset.name.c_str());
                ImGui::PopTextWrapPos();

                if (asset.isImage) {
                    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Image");
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "Model");
                }

                ImGui::EndGroup();

                if ((i + 1) % columns != 0 && i < this->_assets.size() - 1) {
                    ImGui::SameLine(0.0f, spacing);
                }

                ImGui::PopID();
            }
        }
    }
    ImGui::End();
}

void AssetsPanel::addAsset(const std::string& name, EntityID entityId, bool isImage)
{
    AssetInfo info;
    info.name = name;
    info.entityId = entityId;
    info.isImage = isImage;
    info.texture = nullptr;
    info.filepath = "";

    this->_assets.push_back(info);
}

void AssetsPanel::addImageAsset(const std::string& name, std::shared_ptr<ofTexture> texture)
{
    AssetInfo info;
    info.name = name;
    info.entityId = INVALID_ENTITY;
    info.isImage = true;
    info.texture = texture;
    info.filepath = "";

    this->_assets.push_back(info);
}

void AssetsPanel::addModelAsset(const std::string& name, const std::string& filepath)
{
    AssetInfo info;
    info.name = name;
    info.entityId = INVALID_ENTITY;
    info.isImage = false;
    info.texture = nullptr;
    info.filepath = filepath;

    this->_assets.push_back(info);
}

void AssetsPanel::clear()
{
    this->_assets.clear();
}

const AssetInfo* AssetsPanel::getAsset(size_t index) const
{
    if (index >= this->_assets.size()) return nullptr;
    return &this->_assets[index];
}

void AssetsPanel::loadAssetsFromDataFolder()
{
    std::string dataPath = ofToDataPath("", true);
    ofDirectory dir(dataPath);

    if (!dir.exists()) {
        ofLogWarning("AssetsPanel") << "Data folder does not exist: " << dataPath;
        return;
    }

    dir.listDir();

    for (size_t i = 0; i < dir.size(); i++) {
        ofFile file = dir.getFile(i);

        if (file.isDirectory()) continue;

        std::string filename = file.getFileName();
        std::string filepath = file.getAbsolutePath();
        std::string name = file.getBaseName();

        if (this->_isImageFile(filename)) {
            ofImage image;
            if (image.load(filepath)) {
                auto texture = std::make_shared<ofTexture>(image.getTexture());
                this->addImageAsset(name, texture);
                ofLogNotice("AssetsPanel") << "Loaded image: " << name;
            }
        } else if (this->_isModelFile(filename)) {
            this->addModelAsset(name, filepath);
            ofLogNotice("AssetsPanel") << "Found 3D model: " << name;
        }
    }

    ofLogNotice("AssetsPanel") << "Loaded " << this->_assets.size() << " assets from data folder";
}

bool AssetsPanel::_isImageFile(const std::string& filename)
{
    std::string ext = ofToLower(ofFilePath::getFileExt(filename));
    return (ext == "png" || ext == "jpg" || ext == "jpeg" ||
            ext == "bmp" || ext == "gif" || ext == "tga");
}

bool AssetsPanel::_isModelFile(const std::string& filename)
{
    std::string ext = ofToLower(ofFilePath::getFileExt(filename));
    return (ext == "obj" || ext == "fbx" || ext == "3ds" ||
            ext == "dae" || ext == "blend" || ext == "ply");
}

void AssetsPanel::_renderAssetThumbnail(const AssetInfo& asset)
{
    float thumbnailSize = 80.0f;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImU32 color = asset.isImage ?
        IM_COL32(100, 150, 255, 255) :
        IM_COL32(255, 200, 100, 255);

    drawList->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + thumbnailSize, cursorPos.y + thumbnailSize),
        color,
        4.0f
    );

    const char* icon = asset.isImage ? "IMG" : "3D";
    ImVec2 textSize = ImGui::CalcTextSize(icon);
    ImVec2 textPos = ImVec2(
        cursorPos.x + (thumbnailSize - textSize.x) / 2,
        cursorPos.y + (thumbnailSize - textSize.y) / 2
    );

    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), icon);

    ImGui::SetCursorScreenPos(cursorPos);
    ImGui::InvisibleButton("##thumbnail", ImVec2(thumbnailSize, thumbnailSize));

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        size_t assetIndex = &asset - &this->_assets[0];

        if (asset.isImage) {
            ImGui::SetDragDropPayload("ASSET_IMAGE", &assetIndex, sizeof(size_t));
        } else {
            ImGui::SetDragDropPayload("ASSET_MODEL", &assetIndex, sizeof(size_t));
        }

        ImGui::Text("%s", asset.name.c_str());
        if (asset.isImage) {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Image");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "3D Model");
        }

        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemClicked()) {
        if (asset.entityId != INVALID_ENTITY) {
            ofLogNotice("AssetsPanel") << "Asset clicked: " << asset.name;
        }
    }
}
