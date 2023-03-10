#ifndef COMPUTE_LUMINANCE_H
#define COMPUTE_LUMINANCE_H
#include "ComputeSubsystem.h"
//#include "AAVasiliev/PlatformPoint.h"
#include "TerrainShadowCB.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include <memory>


namespace BruteForce
{
	namespace Compute
	{
		class ComputeLuminance : public ComputeSubsystem
		{
		private:
			
			std::shared_ptr<DescriptorHeapRange> LuminanceUavDescriptors;
			std::shared_ptr<DescriptorHeapRange> CbvRange;
			std::shared_ptr<DescriptorHeapRange> RTLuminanceSrvDescriptors;
			ConstantBuffer<TerrainShadowCB>* m_LuminanceBuffers;
		public:
			static const int GetLuminanceBufferSize() { return 1024; }
			ComputeLuminance();
			virtual ~ComputeLuminance();
			virtual void Update(float delta_time, uint8_t frame_index);
			virtual void LoadContent(Device& device, uint8_t frames_count, DescriptorHeapManager& descriptor_heap_manager);
			virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareComputeHelper&);
		};
	}

}
#endif
