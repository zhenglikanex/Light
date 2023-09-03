#pragma once

#include <cstdint>
#include <memory>

namespace light
{
	class RenderCommandStream
	{
	public:
		using RenderCommand = void(*)(uint8_t*);

		RenderCommandStream()
		{
			command_buffer_ = new uint8_t[10 * 1024 * 1024];
		}

		template<class Func>
		void Submit(Func&& func)
		{
			RenderCommand command = [](uint8_t* data)
			{
				Func* t_func = reinterpret_cast<Func*>(data);

				(*t_func)();

				t_func->~Func();
			};

			uint8_t* buffer = Allocate(command,sizeof(Func));
			new (buffer) Func(std::move(func));

			++command_count_;
		}

		void Execute()
		{
			uint8_t* buffer = command_buffer_;
			for (uint32_t i = 0; i < command_count_; ++i)
			{
				auto command = reinterpret_cast<RenderCommand*>(buffer);

				buffer += sizeof(RenderCommand);

				auto data_bytes = reinterpret_cast<uint32_t*>(buffer);
				buffer += sizeof(uint32_t);

				(*command)(buffer);

				buffer += *data_bytes;
			}

			Clear();
		}

		void Clear()
		{
			allocate_bytes_ = 0;
			command_count_ = 0;
		}
	private:
		
		uint8_t* Allocate(RenderCommand command,uint32_t bytes)
		{
			// sizeof(RenderCommand) + sizeof(uint32_t) + bytes

			uint8_t* buffer = command_buffer_ + allocate_bytes_;
			
			new (buffer) RenderCommand(command);

			buffer += sizeof(RenderCommand);

			new (buffer) uint32_t(bytes);

			buffer += sizeof(uint32_t);

			allocate_bytes_ += sizeof(RenderCommand) + sizeof(uint32_t) + bytes;

			return buffer;
		}

		uint32_t command_count_ = 0;
		uint8_t* command_buffer_ = nullptr;
		uint32_t allocate_bytes_ = 0;
	};
}