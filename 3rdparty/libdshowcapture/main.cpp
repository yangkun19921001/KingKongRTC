
#include "dshowcapture.h"
#include <iostream>.
#include <objbase.h>
using namespace DShow;
static void OnVideoData(const VideoConfig& config, unsigned char* data,
	size_t size, long long startTime,
	long long endTime, long rotation)
{
	std::cout << " OnVideoData " << config.cx<<":"<<config.cy_abs << (int)config.format<< std::endl;
}

int main() {

	DShow::Device device;
	CoInitialize(0);
	if (!device.ResetGraph()) {
		std::cout << " ResetGraph error " << std::endl;
		return false;
	}
	std::vector<VideoDevice> devices;
	DShow::Device::EnumVideoDevices(devices);
	DShow::VideoConfig videoConfig;
	videoConfig.format = VideoFormat::I420;
	videoConfig.frameInterval = 30;
	videoConfig.internalFormat = VideoFormat::I420;
	videoConfig.callback = OnVideoData;
	videoConfig.name = devices[1].name;
	videoConfig.path = devices[1].path;
	videoConfig.cx = 1920;
	videoConfig.cy_abs = 1080;
	videoConfig.useDefaultConfig = false;
	device.SetVideoConfig(&videoConfig);
	if (!device.ConnectFilters())
		return false;

	if (device.Start() != Result::Success)
		return false;
	getchar();
	return 0;
}