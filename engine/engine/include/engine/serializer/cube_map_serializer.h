#include "engine/serializer/serializer_utils.h"

#include "engine/renderer/cube_map.h"

namespace light
{
	class CubeMapSerializer
	{
	public:
		CubeMapSerializer(CubeMap* cubemap);

		void SerializeText(const std::string& filepath);
		bool DeserializeText(const std::string& filepath);
	private:
		Ref<CubeMap> cubemap_;
	};
}