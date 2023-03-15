#ifndef COMPUTE_DEFFERED_H
#define COMPUTE_DEFFERED_H
#include "ComputeSubsystem.h"
//#include "AAVasiliev/PlatformPoint.h"
#include "ComputeLuminanceCB.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include <memory>


namespace BruteForce
{
	namespace Compute
	{
		class ComputeDeffered : public ComputeSubsystem
		{
		private:

			std::shared_ptr<DescriptorHeapRange> LuminanceUavDescriptors;
			std::shared_ptr<DescriptorHeapRange> CbvRange;
			std::shared_ptr<DescriptorHeapRange> RTLuminanceSrvDescriptors;
			ConstantBuffer<ComputeLuminanceCB>* m_LuminanceBuffers;

		public:
			static const int GetLuminanceBufferSize() { return 1024; }
			ComputeDeffered();
			virtual ~ComputeDeffered();
			virtual void Update(float delta_time, uint8_t frame_index);
			virtual void LoadContent(Device& device, uint8_t frames_count, DescriptorHeapManager& descriptor_heap_manager);
			virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareComputeHelper&);
		};
	}

}
#endif
