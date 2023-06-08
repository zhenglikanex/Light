#pragma once

#include "glm/glm.hpp"
#include "yaml-cpp/yaml.h"

namespace YAML
{
	inline Emitter& operator<<(Emitter& out, const glm::vec2& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < 2; ++i)
		{
			out << YAML::Value << value[i];
		}
		out << YAML::EndSeq;

		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::vec3& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < 3; ++i)
		{
			out << YAML::Value << value[i];
		}
		out << YAML::EndSeq;

		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::vec4& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < 4; ++i)
		{
			out << YAML::Value << value[i];
		}
		out << YAML::EndSeq;
		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::mat3& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int col = 0; col < 3; ++col)
		{
			for (int row = 0; row < 3; ++row)
				out << YAML::Value << value[col][row];
		}
		out << YAML::EndSeq;

		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::mat4& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int col = 0; col < 4; ++col)
		{
			for (int row = 0; row < 4; ++row)
				out << YAML::Value << value[col][row];
		}
		out << YAML::EndSeq;

		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::ivec2& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < 2; ++i)
		{
			out << YAML::Value << value[i];
		}
		out << YAML::EndSeq;

		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::ivec3& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < 3; ++i)
		{
			out << YAML::Value << value[i];
		}
		out << YAML::EndSeq;

		return out;
	}

	inline Emitter& operator<<(Emitter& out, const glm::ivec4& value)
	{
		out << YAML::Flow;
		out << YAML::Value << YAML::BeginSeq;
		for (int i = 0; i < 4; ++i)
		{
			out << YAML::Value << value[i];
		}
		out << YAML::EndSeq;
		return out;
	}

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& value)
		{
			Node node;
			for (int i = 0; i < value.length(); ++i)
			{
				node.push_back(value[i]);
			}

			return node;
		}

		static bool decode(const Node& node, glm::vec4& value)
		{
			if (!node.IsSequence() || node.size() != value.length())
			{
				return false;
			}

			for (int i = 0; i < value.length(); ++i)
			{
				value[i] = node[i].as<float>();
			}

			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& value)
		{
			Node node;
			for (int i = 0; i < value.length(); ++i)
			{
				node.push_back(value[i]);
			}

			return node;
		}

		static bool decode(const Node& node, glm::vec3& value)
		{
			if (!node.IsSequence() || node.size() != value.length())
			{
				return false;
			}

			for (int i = 0; i < value.length(); ++i)
			{
				value[i] = node[i].as<float>();
			}

			return true;
		}
	};

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& value)
		{
			Node node;
			for (int i = 0; i < value.length(); ++i)
			{
				node.push_back(value[i]);
			}

			return node;
		}

		static bool decode(const Node& node, glm::vec2& value)
		{
			if (!node.IsSequence() || node.size() != value.length())
			{
				return false;
			}

			for (int i = 0; i < value.length(); ++i)
			{
				value[i] = node[i].as<float>();
			}

			return true;
		}
	};

	template<>
	struct convert<glm::ivec4>
	{
		static Node encode(const glm::ivec4& value)
		{
			Node node;
			for (int i = 0; i < value.length(); ++i)
			{
				node.push_back(value[i]);
			}

			return node;
		}

		static bool decode(const Node& node, glm::ivec4& value)
		{
			if (!node.IsSequence() || node.size() != value.length())
			{
				return false;
			}

			for (int i = 0; i < value.length(); ++i)
			{
				value[i] = node[i].as<int>();
			}

			return true;
		}
	};

	template<>
	struct convert<glm::ivec3>
	{
		static Node encode(const glm::ivec3& value)
		{
			Node node;
			for (int i = 0; i < value.length(); ++i)
			{
				node.push_back(value[i]);
			}

			return node;
		}

		static bool decode(const Node& node, glm::ivec3& value)
		{
			if (!node.IsSequence() || node.size() != value.length())
			{
				return false;
			}

			for (int i = 0; i < value.length(); ++i)
			{
				value[i] = node[i].as<int>();
			}

			return true;
		}
	};

	template<>
	struct convert<glm::ivec2>
	{
		static Node encode(const glm::ivec2& value)
		{
			Node node;
			for (int i = 0; i < value.length(); ++i)
			{
				node.push_back(value[i]);
			}

			return node;
		}

		static bool decode(const Node& node, glm::ivec2& value)
		{
			if (!node.IsSequence() || node.size() != value.length())
			{
				return false;
			}

			for (int i = 0; i < value.length(); ++i)
			{
				value[i] = node[i].as<int>();
			}

			return true;
		}
	};


	template<>
	struct convert<glm::mat3>
	{
		static Node encode(const glm::mat3& value)
		{
			Node node;
			for (int col = 0; col < value.length(); ++col)
			{
				for (int row = 0; row < value.length(); ++row)
				{
					node.push_back(value[col][row]);
				}
			}

			return node;
		}

		static bool decode(const Node& node, glm::mat3& value)
		{
			if (!node.IsSequence() || node.size() != value.length() * value.length())
			{
				return false;
			}

			int index = 0;
			for (int col = 0; col < value.length(); ++col)
			{
				for (int row = 0; row < value.length(); ++row)
				{
					value[col][row] = node[index++].as<float>();
				}
			}

			return true;
		}
	};

	template<>
	struct convert<glm::mat4>
	{
		static Node encode(const glm::mat4& value)
		{
			Node node;
			for (int col = 0; col < value.length(); ++col)
			{
				for (int row = 0; row < value.length(); ++row)
				{
					node.push_back(value[col][row]);
				}
			}

			return node;
		}

		static bool decode(const Node& node, glm::mat4& value)
		{
			if (!node.IsSequence() || node.size() != value.length() * value.length())
			{
				return false;
			}

			int index = 0;
			for (int col = 0; col < value.length(); ++col)
			{
				for (int row = 0; row < value.length(); ++row)
				{
					value[col][row] = node[index++].as<float>();
				}
			}

			return true;
		}
	};
}