#include "asset_browser_panel.h"

#include "engine/editor/imgui_utils.h"
#include "engine/renderer/cube_map.h"

#include "material_panel.h"
#include "cubemap_panel.h"



namespace light::editor
{
	void AssetBrowserPanel::Init()
	{
		current_path_ = AssetManager::GetAssetPath();

		folder_icon_ = AssetManager::LoadAsset<rhi::Texture>("editor/icons/folder.png");
		file_icon_ = AssetManager::LoadAsset<rhi::Texture>("editor/icons/file.png");
		
		auto command_list = Application::Get().GetDevice()->GetCommandList(rhi::CommandListType::kDirect);
		command_list->TransitionBarrier(folder_icon_, rhi::ResourceStates::kPixelShaderResource);
		command_list->ExecuteCommandList();

		Application::Get().GetDevice()->Flush();
	}

	void AssetBrowserPanel::OnImguiRender()
	{
		ImGui::Begin("Asset Browser");

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::BeginMenu("Create Asset"))
			{
				if (ImGui::MenuItem("Directory"))
				{
					//std::filesystem::create_directory(current_path_ / "directory");
				}

				if (ImGui::MenuItem("Material"))
				{
					AssetManager::CreateAsset(AssetType::kMaterial,current_path_,"unnamed");
				}

				if (ImGui::MenuItem("CubeMap"))
				{
					AssetManager::CreateAsset(AssetType::kCubeMap, current_path_, "unnamed");
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		bool is_show = false;

		std::filesystem::path jump_path;
		
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		for (const auto& path : current_path_)
		{
			jump_path /= path;

			std::string name = path.string();
			if (!is_show)
			{
				if (name == "assets")
				{
					is_show = true;
				}
				else
				{
					continue;
				}
			}

			name += "  >";

			if (is_show)
			{
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				if (ImGui::Button(name.c_str()))
				{
					current_path_ = jump_path;
					ImGui::PopStyleColor();
					break;
				}

				ImGui::PopStyleColor();
			}
		}

		ImGui::PopStyleVar();

		ImGui::Separator();

		ImVec2 size = ImGui::GetContentRegionAvail();
		uint32_t padding = ImGui::GetStyle().ItemSpacing.x;
		uint32_t line_count = std::max(1,static_cast<int>(size.x / (kIconSize + padding * 2)));
		uint32_t index = 0;
		int n = 0;
		for (auto entry : std::filesystem::directory_iterator(current_path_))
		{
			if (!entry.is_directory())
			{
				continue;
			}

			ImGui::PushID(n++);

			DrawEntry(entry);

			if (index++ < line_count)
			{
				ImGui::SameLine();
			}
			else
			{
				index = 0;
			}

			ImGui::PopID();
		}

		for (auto entry : std::filesystem::directory_iterator(current_path_))
		{
			std::string ext = entry.path().extension().string();
			if (ToAssetType(ext) == AssetType::kInvalid || entry.is_directory())
			{
				continue;
			}

			ImGui::PushID(n++);

			DrawEntry(entry);

			if (index++ < line_count)
			{
				ImGui::SameLine();
			}
			else
			{
				index = 0;
			}

			ImGui::PopID();
		}

		ImGui::End();
	}

	void AssetBrowserPanel::OnEvent(const Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowFileDropedEvent>([this](const WindowFileDropedEvent& droped_event)
			{
				for (const auto& file : droped_event.files)
				{
					std::filesystem::directory_entry entry(file);
					ImportAsset(current_path_, entry);
				}
			});
	}

	void AssetBrowserPanel::ImportAsset(const std::filesystem::path& parent_path, const std::filesystem::directory_entry& entry)
	{
		if (entry.is_directory())
		{
			std::filesystem::directory_iterator iter(entry);
			for (auto e : iter)
			{
				ImportAsset(parent_path / entry.path().filename(), e);
			}

			return;
		}
		

		if (ToAssetType(entry.path().extension().string()) == AssetType::kInvalid)
		{
			return;
		}

		std::filesystem::path target_path = parent_path / entry.path().filename();

		if (std::filesystem::exists(target_path))
		{
			return;
		}

		if (!std::filesystem::exists(parent_path))
		{
			std::error_code code;
			if (!std::filesystem::create_directories(parent_path, code))
			{
				return;
			}
			
		}

		if (std::filesystem::copy_file(entry.path(), target_path))
		{
			std::filesystem::path path = AssetManager::GetAssetRelativePath(target_path);
			AssetManager::ImportAsset(path);
		}
	}

	void AssetBrowserPanel::DrawEntry(const std::filesystem::directory_entry& entry)
	{
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Button, select_entry_ == entry ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered) : ImVec4{ 0, 0, 0, 0 });

		ImGui::ImageButton(entry.is_directory() ? folder_icon_->GetTextureID() : file_icon_->GetTextureID(), ImVec2(kIconSize, kIconSize), { 0.0f, 1.0f }, { 1.0f, 0.0f });
		if (ImGui::IsItemClicked(0))
		{
			select_entry_ = entry;

			if (ImGui::GetMouseClickedCount(0) >= 2)
			{
				if (entry.is_directory())
				{
					current_path_ = entry.path();
				}
				else 
				{
					std::string ext = entry.path().extension().string();
					AssetType type = ToAssetType(ext);
					if (type == AssetType::kMaterial)
					{
						Material* material = AssetManager::LoadAssetByPath<Material>(entry.path());
						if (material)
						{
							MaterialPanel::ShowMaterial(material);
						}
					}
					else if (type == AssetType::kCubeMap)
					{
						CubeMap* cubemap = AssetManager::LoadAssetByPath<CubeMap>(entry.path());
						if (cubemap)
						{
							CubeMapPanel::ShowCubeMap(cubemap);
						}
					}
				}
			}
		}

		if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Delete Asset"))
			{
				if (entry.is_directory())
				{
					// todo:
				}
				else
				{
					AssetManager::DeleteAsset(entry.path());
				}
			}

			if (ImGui::MenuItem("Rename"))
			{
				

				if (entry.is_directory())
				{
					// todo
				}
				else
				{
					rename_entry_ = entry;
				}
			}

			ImGui::EndPopup();
		}

		if (!entry.is_directory())
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				std::filesystem::path path = AssetManager::GetAssetRelativePath(entry.path());

				const AssetMeta& meta = AssetManager::GetAssetMeta(path.generic_string());

				if (meta.IsVaild())
				{
					size_t meta_address = (size_t)(&meta);

					ImGui::SetDragDropPayload(ImGuiEditor::kAssetMetaDragId, &meta_address, sizeof(size_t));

					std::string filename = entry.path().filename().string();
					ImGui::Text(filename.c_str());
				}

				ImGui::EndDragDropSource();
			}
		}

		ImGui::PopStyleColor();
		std::string name = entry.path().filename().string();

		
		if (rename_entry_ == entry)
		{
			static char buf[256] = {};
			uint32_t size = std::min(name.find_last_of('.'),name.size());
			memcpy(buf, name.c_str(), size);
			buf[size] = '\0';

			ImGui::PushItemWidth(ImGui::GetItemRectSize().x);
			ImGui::SetKeyboardFocusHere(0);

			struct Funcs
			{
				static int Filter(ImGuiInputTextCallbackData* data)
				{
					if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter)
					{
						if (data->EventChar >= 'A' && data->EventChar <= 'Z' || data->EventChar >= 'a' && data->EventChar <= 'z'
							|| data->EventChar >= '0' && data->EventChar <= '9' || data->EventChar == '_' || data->EventChar == '-')
						{
							return 0;
						}

						// 返回1表示丢弃字符串
						return 1;
					}
					return 0;
				}
			};

			ImGui::InputText("##", buf, 256, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackCharFilter, Funcs::Filter);
			
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				if (strcmp(buf, name.c_str()) != 0)
				{
					AssetManager::RenameAsset(entry.path(), buf);
				}
			}

			if (ImGui::IsItemDeactivated())
			{
				rename_entry_ = {};
			}

			ImGui::PopItemWidth();
		}
		else
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetItemRectSize().x - ImGui::CalcTextSize(name.c_str()).x) / 2.f);
			ImGui::Text(name.c_str());
		}

		ImGui::EndGroup();
	}
}