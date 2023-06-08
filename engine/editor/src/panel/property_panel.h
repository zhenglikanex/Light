#pragma once

#include "engine/light.h"
#include "engine/scene/components.h"
#include "../editor_event.h"


namespace light::editor
{
	class PropertyPanel
	{
	public:
		PropertyPanel() = default;

		void OnImguiRender();

		void OnEvent(const Event& e);
	private:
		void OnSelectEntityEvent(const SelectEntityEvent& e);

		void DrawComponentProperty(meta::Any instance);
		void DrawTypeProperty(meta::Any& instance);

		template<typename T>
		void SetFieldValue(meta::Any& instance,const meta::Field& field,const T& value)
		{
			//find set field method
			std::string field_name = std::string(field.GetName());
			for (size_t index = 0; index < field_name.size(); ++index)
			{
				if (index == 0)
				{
					field_name[index] = toupper(field_name[index]);
				}
				else if ((field_name[index] == '_' && index + 1 < field_name.size()))
				{
					field_name[index + 1] = toupper(field_name[index + 1]);
				}
			}
			std::erase_if(field_name, [](char ch)
				{
					return ch == '_';
				});

			field_name.insert(field_name.begin(), { 'S','e','t' });
			const meta::Method& method = instance.GetType().GetMethod(field_name);
			if (method.IsValid())
			{
				method.Invoke(instance, value);
			}
			else
			{
				field.SetValue(instance, value);
			}
		}

		Entity select_entity_;
	};
}