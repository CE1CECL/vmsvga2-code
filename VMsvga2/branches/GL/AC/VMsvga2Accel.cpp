/*
 *  VMsvga2Accel.cpp
 *  VMsvga2Accel
 *
 *  Created by Zenith432 on July 29th 2009.
 *  Copyright 2009-2010 Zenith432. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/graphics/IOGraphicsInterfaceTypes.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
#include "IOSurfaceRoot.h"
#endif
#include "vmw_options_ac.h"
#include "VLog.h"
#include "VMsvga2Accel.h"
#include "VMsvga2Surface.h"
#include "VMsvga2GLContext.h"
#include "VMsvga22DContext.h"
#include "VMsvga2DVDContext.h"
#include "VMsvga2Device.h"
#include "VMsvga2OCDContext.h"
#include "VMsvga2Allocator.h"
#include "VMsvga2.h"

#define CLASS VMsvga2Accel
#define super IOAccelerator
OSDefineMetaClassAndStructors(VMsvga2Accel, IOAccelerator);

#define HIDDEN __attribute__((visibility("hidden")))

static __attribute__((used)) char const copyright[] = "Copyright 2009-2010 Zenith432";

HIDDEN
uint32_t vmw_options_ac = 0;

#if LOGGING_LEVEL >= 1
#define ACLog(log_level, ...) do { if (log_level <= m_log_level_ac) VLog("IOAC: ", ##__VA_ARGS__); } while(false)
#else
#define ACLog(log_level, ...)
#endif

#define FMT_D(x) static_cast<int>(x)
#define FMT_U(x) static_cast<unsigned>(x)
#define FMT_LU(x) static_cast<size_t>(x)

#pragma mark -
#pragma mark Static Functions
#pragma mark -

template<unsigned N>
union DefineRegion
{
	uint8_t b[sizeof(IOAccelDeviceRegion) + N * sizeof(IOAccelBounds)];
	IOAccelDeviceRegion r;
};

static inline
int log_2_32(uint32_t x)
{
	int y = 0;

	if (x & 0xFFFF0000U) { y |= 16; x >>= 16; }
	if (x & 0xFF00U) { y |= 8; x >>= 8; }
	if (x & 0xF0U) { y |= 4; x >>= 4; }
	if (x & 0xCU) { y |= 2; x >>= 2; }
	if (x & 0x2U) { y |= 1; x >>= 1; }
	if (!(x & 0x1U)) return -1;
	return y;
}

static inline
int log_2_64(uint64_t x)
{
	int y = 0;

	if (x & 0xFFFFFFFF00000000ULL) { y |= 32; x >>= 32; }
	y |= log_2_32(static_cast<uint32_t>(x));
	return y;
}

static inline
void memset32(void* dest, uint32_t value, size_t size)
{
	asm volatile ("cld; rep stosl" : "+c" (size), "+D" (dest) : "a" (value) : "memory");
}

static
void set_region(IOAccelDeviceRegion* rgn,
				uint32_t x,
				uint32_t y,
				uint32_t w,
				uint32_t h)
{
	rgn->num_rects = 1;
	rgn->bounds.x = static_cast<int16_t>(x);
	rgn->bounds.y = static_cast<int16_t>(y);
	rgn->bounds.w = static_cast<int16_t>(w);
	rgn->bounds.h = static_cast<int16_t>(h);
	memcpy(&rgn->rect[0], &rgn->bounds, sizeof rgn->bounds);
}

#if 0
static
void set_region(IOAccelDeviceRegion* rgn,
				IOBlitRectangleStruct const* rects,
				size_t numRects)
{
	size_t i;

	rgn->num_rects = static_cast<uint32_t>(numRects);
	rgn->bounds.x = rects->x;
	rgn->bounds.y = rects->y;
	rgn->bounds.w = rects->x + rects->width;
	rgn->bounds.h = rects->y + rects->height;
	for (i = 1U; i < numRects; ++i) {
		IOBlitRectangleStruct const* src_rect = &rects[i];
		IOAccelBounds* dst_rect = &rgn->rect[i];
		dst_rect->x = src_rect->x;
		dst_rect->y = src_rect->y;
		dst_rect->w = src_rect->width;
		dst_rect->h = src_rect->height;
		if (dst_rect->x < rgn->bounds.x)
			rgn->bounds.x = dst_rect->x;
		if (dst_rect->x + dst_rect->w > rgn->bounds.w)
			rgn->bounds.w = dst_rect->x + dst_rect->w;
		if (dst_rect->y < rgn->bounds.y)
			rgn->bounds.y = dst_rect->y;
		if (dst_rect->y + dst_rect->h > rgn->bounds.h)
			rgn->bounds.h = dst_rect->y + dst_rect->h;
	}
	rgn->bounds.w -= rgn->bounds.x;
	rgn->bounds.h -= rgn->bounds.y;
}
#endif

#if 0
static
void clip_rect(IOBlitRectangleStruct* rect, int w, int h)
{
	if (rect->x < 0) rect->x = 0;
	if (rect->y < 0) rect->y = 0;
	if (rect->width < 0) rect->width = 0;
	if (rect->height < 0) rect->height = 0;
	if (rect->x + rect->width > w) rect->width = w - rect->x;
	if (rect->y + rect->height > h) rect->height = h - rect->y;
}
#endif

static
void convert_rect(IOAccelBounds const* src_rect,
				  SVGASignedRect* dest_rect,
				  SVGASignedPoint const* delta = 0)
{
	dest_rect->left = src_rect->x + (delta ? delta->x : 0);
	dest_rect->right = dest_rect->left + src_rect->w;
	dest_rect->top = src_rect->y + (delta ? delta->y : 0);
	dest_rect->bottom = dest_rect->top + src_rect->h;
}

#pragma mark -
#pragma mark Private Methods
#pragma mark -

HIDDEN
void CLASS::Cleanup()
{
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
	if (m_surface_root) {
		m_surface_root->release();
		m_surface_root = 0;
	}
#endif
	cleanGLStuff();
	if (bHaveSVGA3D) {
		bHaveSVGA3D = false;
		svga3d.Init(0);
	}
	if (bHaveScreenObject) {
		bHaveScreenObject = false;
		screen.Init(0);
	}
#ifdef FB_NOTIFIER
	if (m_fbNotifier) {
		m_fbNotifier->remove();
		m_fbNotifier = 0;
	}
#endif
	if (m_allocator) {
		m_allocator->release();
		m_allocator = 0;
	}
	if (m_bar1) {
		m_bar1->release();
		m_bar1 = 0;
	}
	if (m_framebuffer) {
		m_svga = 0;
		m_framebuffer->release();
		m_framebuffer = 0;
	}
	if (m_iolock) {
		IOLockFree(m_iolock);
		m_iolock = 0;
	}
}

#ifdef TIMING
HIDDEN
void CLASS::timeSyncs()
{
	static uint32_t first_stamp = 0;
	static uint32_t num_syncs = 0;
	static uint32_t last_stamp;
	uint32_t current_stamp;
	uint32_t dummy;
	float freq;

	if (!first_stamp) {
		clock_get_system_microtime(&first_stamp, &dummy);
		last_stamp = first_stamp;
		return;
	}
	++num_syncs;
	clock_get_system_microtime(&current_stamp, &dummy);
	if (current_stamp >= last_stamp + 60) {
		last_stamp = current_stamp;
		freq = static_cast<float>(num_syncs)/static_cast<float>(current_stamp - first_stamp);
		if (m_framebuffer)
			m_framebuffer->setProperty("VMwareSVGASyncFrequency", static_cast<uint64_t>(freq * 1000.0F), 64U);
	}
}
#endif

HIDDEN
bool CLASS::createMasterSurface(uint32_t width, uint32_t height)
{
	bool rc;
	uint32_t cid;

	m_master_surface_id = AllocSurfaceID();
	cid = AllocContextID();
	rc = createClearSurface(m_master_surface_id,
							cid,
							SVGA3D_X8R8G8B8,
							width,
							height);
	FreeContextID(cid);
	if (!rc)
		FreeSurfaceID(m_master_surface_id);
	return rc;
}

HIDDEN
void CLASS::destroyMasterSurface()
{
	destroySurface(m_master_surface_id);
	FreeSurfaceID(m_master_surface_id);
}

HIDDEN
void CLASS::processOptions()
{
	uint32_t boot_arg;

	vmw_options_ac = VMW_OPTION_AC_GL_CONTEXT | VMW_OPTION_AC_2D_CONTEXT | VMW_OPTION_AC_SURFACE_CONNECT;
	if (PE_parse_boot_argn("vmw_options_ac", &boot_arg, sizeof boot_arg))
		vmw_options_ac = boot_arg;
	if (PE_parse_boot_argn("-svga3d", &boot_arg, sizeof boot_arg))
		vmw_options_ac |= VMW_OPTION_AC_SVGA3D;
	if (PE_parse_boot_argn("-vmw_no_yuv", &boot_arg, sizeof boot_arg))
		vmw_options_ac |= VMW_OPTION_AC_NO_YUV;
	if (PE_parse_boot_argn("-vmw_direct_blit", &boot_arg, sizeof boot_arg))
		vmw_options_ac |= VMW_OPTION_AC_DIRECT_BLIT;
	if (PE_parse_boot_argn("-vmw_no_screen_object", &boot_arg, sizeof boot_arg))
		vmw_options_ac |= VMW_OPTION_AC_NO_SCREEN_OBJECT;
	if (PE_parse_boot_argn("-vmw_qe", &boot_arg, sizeof boot_arg))
		vmw_options_ac |= VMW_OPTION_AC_QE | VMW_OPTION_AC_GLD;
	if (PE_parse_boot_argn("-vmw_gld", &boot_arg, sizeof boot_arg))
		vmw_options_ac |= VMW_OPTION_AC_GLD;
	setProperty("VMwareSVGAAccelOptions", static_cast<uint64_t>(vmw_options_ac), 32U);
	if (PE_parse_boot_argn("vmw_options_ga", &boot_arg, sizeof boot_arg)) {
		m_options_ga = boot_arg;
		setProperty("VMwareSVGAGAOptions", static_cast<uint64_t>(m_options_ga), 32U);
	}
	if (PE_parse_boot_argn("vmw_log_ac", &boot_arg, sizeof boot_arg))
		m_log_level_ac = static_cast<int>(boot_arg);
	setProperty("VMwareSVGAAccelLogLevel", static_cast<uint64_t>(m_log_level_ac), 32U);
	if (PE_parse_boot_argn("vmw_log_ga", &boot_arg, sizeof boot_arg)) {
		m_log_level_ga = static_cast<int>(boot_arg);
		setProperty("VMwareSVGAGALogLevel", static_cast<uint64_t>(m_log_level_ga), 32U);
	}
	if (PE_parse_boot_argn("vmw_log_gld", &boot_arg, sizeof boot_arg)) {
		m_log_level_gld = static_cast<SInt>(boot_arg);
		setProperty("VMwareSVGAGLDLogLevel", static_cast<uint64_t>(m_log_level_gld), 32U);
	}
}

HIDDEN
IOReturn CLASS::findFramebuffer()
{
	OSIterator* it;
	OSObject* obj;
	VMsvga2* fb = 0;

	if (!m_provider)
		return kIOReturnNotReady;
	it = m_provider->getClientIterator();
	if (!it)
		return kIOReturnNotFound;
	while ((obj = it->getNextObject()) != 0) {
		fb = OSDynamicCast(VMsvga2, obj);
		if (!fb)
			continue;
		if (!fb->supportsAccel()) {
			fb = 0;
			continue;
		}
		break;
	}
	if (!fb) {
		it->release();
		return kIOReturnNotFound;
	}
	fb->retain();
	m_framebuffer = fb;
	it->release();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::setupAllocator()
{
	IOReturn rc;
	IOByteCount bytes_reserve, s;
	void* p;

	if (m_bar1)
		return kIOReturnSuccess;
	if (!m_framebuffer)
		return kIOReturnNotReady;
	m_bar1 = m_framebuffer->getVRAMRange();
	if (!m_bar1)
		return kIOReturnInternalError;
	p = reinterpret_cast<void*>(m_framebuffer->getVRAMPtr());
	s = m_bar1->getLength();
	if (m_svga->getVRAMSize() < s)
		s = m_svga->getVRAMSize();
	s &= ~(static_cast<IOByteCount>(PAGE_SIZE - 1));

	if (!p || !s) {
		rc = kIOReturnNoMemory;
		goto exit;
	}
	rc = m_allocator->Init(p, s);
	if (rc != kIOReturnSuccess) {
		ACLog(1, "%s: Allocator Init(%p, %#lx) failed with %#x\n",
			  __FUNCTION__, p, FMT_LU(s), rc);
		goto exit;
	}
	bytes_reserve = SVGA_FB_MAX_TRACEABLE_SIZE;
	if (bytes_reserve < s) {
		rc = m_allocator->Release(bytes_reserve, s);
		if (rc != kIOReturnSuccess) {
			ACLog(1, "%s: Allocator Release(%#lx, %#lx) failed with %#x\n",
				  __FUNCTION__, FMT_LU(bytes_reserve), FMT_LU(s), rc);
			goto exit;
		}
		s = bytes_reserve;
		bytes_reserve = 0;
	}
	if (HaveFrontBuffer()) {
		rc = m_allocator->Release(0, s);
		if (rc != kIOReturnSuccess) {
			ACLog(1, "%s: Allocator Release(%#x, %#lx) failed with %#x\n",
				  __FUNCTION__, 0, FMT_LU(s), rc);
			if (!bytes_reserve)			// We already got some memory, so it's ok
				rc = kIOReturnSuccess;
		}
	}
exit:
	if (rc != kIOReturnSuccess) {
		m_bar1->release();
		m_bar1 = 0;
	}
	return rc;
}

#ifdef FB_NOTIFIER
HIDDEN
IOReturn CLASS::fbNotificationHandler(void* ref,
									  class IOFramebuffer* framebuffer,
									  SInt32 event,
									  void* info)
{
	ACLog(3, "%s: ref == %p, framebuffer == %p, event == %d, info == %p\n",
		  __FUNCTION__, ref, framebuffer, event, info);
	return kIOReturnSuccess;
}
#endif

HIDDEN
void CLASS::initGLStuff()
{
	m_channel_memory = IOBufferMemoryDescriptor::withOptions(kIOMemoryKernelUserShared |
															 kIOMemoryPageable |
															 kIODirectionOut,
															 page_size,
															 page_size);
	if (m_channel_memory)
		m_channel_memory->prepare(kIODirectionInOut);
}

HIDDEN
void CLASS::cleanGLStuff()
{
	if (m_channel_memory) {
		m_channel_memory->complete();
		m_channel_memory->release();
		m_channel_memory = 0;
	}
}

#pragma mark -
#pragma mark Methods from IOService
#pragma mark -

bool CLASS::init(OSDictionary* dictionary)
{
	if (!super::init(dictionary))
		return false;
	svga3d.Init(0);
	screen.Init(0);
	m_log_level_ac = LOGGING_LEVEL;
	m_log_level_ga = -1;
	m_log_level_gld = -1;
	m_blitbug_result = kIOReturnNotFound;
	m_present_tracker.init();
	return true;
}

bool CLASS::start(IOService* provider)
{
	char pathbuf[256];
	int len;
	OSObject* plug;

	m_provider = OSDynamicCast(IOPCIDevice, provider);
	if (!m_provider)
		return false;
	if (!super::start(provider))
		return false;
	IOLog("IOAC: start\n");
	VMLog_SendString("log IOAC: start\n");
	processOptions();
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
	if (checkOptionAC(VMW_OPTION_AC_GLD)) {
		m_surface_root = static_cast<IOSurfaceRoot*>(IOService::waitForService(IOService::nameMatching("IOSurfaceRoot")));
		if (m_surface_root) {
			m_surface_root->retain();
			m_surface_root_uuid = m_surface_root->generateUniqueAcceleratorID(this);
		}
	}
#endif
	/*
	 * TBD: is there a possible race condition here where VMsvga2Accel::start
	 *   is called before VMsvga2 gets attached to its provider?
	 */
	if (findFramebuffer() != kIOReturnSuccess) {
		ACLog(1, "Unable to locate suitable framebuffer\n");
		stop(provider);
		return false;
	}
	m_iolock = IOLockAlloc();
	if (!m_iolock) {
		ACLog(1, "Unable to allocate IOLock\n");
		stop(provider);
		return false;
	}
	m_allocator = VMsvga2Allocator::factory();
	if (!m_allocator) {
		ACLog(1, "Unable to create Allocator\n");
		stop(provider);
		return false;
	}
	m_svga = m_framebuffer->getDevice();
	if (!checkOptionAC(VMW_OPTION_AC_NO_SCREEN_OBJECT) && screen.Init(m_svga)) {
		bHaveScreenObject = true;
		ACLog(1, "Screen Object On\n");
	}
	if ((bHaveScreenObject || checkOptionAC(VMW_OPTION_AC_SVGA3D)) && svga3d.Init(m_svga)) {
		uint32_t hwv = svga3d.getHWVersion();
		bHaveSVGA3D = true;
		ACLog(1, "SVGA3D On, 3D HWVersion == %u.%u\n", SVGA3D_MAJOR_HWVERSION(hwv), SVGA3D_MINOR_HWVERSION(hwv));
	}
	if (checkOptionAC(VMW_OPTION_AC_NO_YUV))
		ACLog(1, "YUV Off\n");
	plug = getProperty(kIOCFPlugInTypesKey);
	if (plug)
		m_framebuffer->setProperty(kIOCFPlugInTypesKey, plug);
	len = sizeof pathbuf;
	if (getPath(&pathbuf[0], &len, gIOServicePlane)) {
		m_framebuffer->setProperty(kIOAccelTypesKey, pathbuf);
		m_framebuffer->setProperty(kIOAccelIndexKey, 0ULL, 32U);
		m_framebuffer->setProperty(kIOAccelRevisionKey, static_cast<uint64_t>(kCurrentGraphicsInterfaceRevision), 32U);
	}
	setProperty(kIOAccelRevisionKey, static_cast<uint64_t>(kCurrentGraphicsInterfaceRevision), 32U);
	if (checkOptionAC(VMW_OPTION_AC_QE))
		setProperty("AccelCaps", 3ULL, 32U);
#ifdef FB_NOTIFIER
	m_fbNotifier = m_framebuffer->addFramebufferNotification(OSMemberFunctionCast(IOFramebufferNotificationHandler, this, &CLASS::fbNotificationHandler), this, 0);
	if (!m_fbNotifier)
		ACLog(1, "Unable to register framebuffer notification handler\n");
#endif
	/*
	 * Options Are:
	 *   ATIRadeonX1000GLDriver
	 *   ATIRadeonX2000GLDriver
	 *   AppleIntelGMA950GLDriver
	 *   AppleIntelGMAX3100GLDriver
	 *   GeForce7xxxGLDriver
	 *   GeForce8xxxGLDriver
	 */
	if (checkOptionAC(VMW_OPTION_AC_GLD)) {
		setProperty("IOGLBundleName", "VMsvga2GLDriver");
		initGLStuff();
	}
	/*
	 * Stupid bug in AppleVA attempts to CFRelease a NULL pointer
	 *   if it can't find this property.
	 */
	setProperty("IODVDBundleName", "AppleVADriver");
	return true;
}

void CLASS::stop(IOService* provider)
{
	ACLog(2, "%s\n", __FUNCTION__);

	Cleanup();
	super::stop(provider);
}

IOReturn CLASS::newUserClient(task_t owningTask,
							  void* securityID,
							  UInt32 type,
							  IOUserClient ** handler)
{
	IOUserClient* client;

	/*
	 * Client Types
	 * 0 - Surface
	 * 1 - GL Context
	 * 2 - 2D Context
	 * 3 - DVD Context
	 * 4 - Device (duplicate for DVD Context in OS 10.5)
	 * 5 - OCD Context
	 */
	ACLog(2, "%s: owningTask==%p, securityID==%p, type==%u\n",
		  __FUNCTION__,
		  owningTask,
		  securityID,
		  FMT_U(type));
	if (!handler)
		return kIOReturnBadArgument;
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	switch (type) {
		case kIOAccelSurfaceClientType:
			if (!checkOptionAC(VMW_OPTION_AC_SURFACE_CONNECT))
				return kIOReturnUnsupported;
			if (setupAllocator() != kIOReturnSuccess)
				return kIOReturnNoMemory;
			client = VMsvga2Surface::withTask(owningTask, securityID, type);
			break;
		case 1:
			if (!checkOptionAC(VMW_OPTION_AC_GL_CONTEXT))
				return kIOReturnUnsupported;
			client = VMsvga2GLContext::withTask(owningTask, securityID, type);
			break;
		case 2:
			if (!checkOptionAC(VMW_OPTION_AC_2D_CONTEXT))
				return kIOReturnUnsupported;
			client = VMsvga22DContext::withTask(owningTask, securityID, type);
			break;
		case 3:
			if (!checkOptionAC(VMW_OPTION_AC_DVD_CONTEXT))
				return kIOReturnUnsupported;
			client = VMsvga2DVDContext::withTask(owningTask, securityID, type);
			break;
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
		case 4:
			if (!checkOptionAC(VMW_OPTION_AC_GL_CONTEXT))
				return kIOReturnUnsupported;
			client = VMsvga2Device::withTask(owningTask, securityID, type);
			break;
		case 5:
			if (!checkOptionAC(VMW_OPTION_AC_GL_CONTEXT))
				return kIOReturnUnsupported;
			client = VMsvga2OCDContext::withTask(owningTask, securityID, type);
			break;
#endif
		default:
			return kIOReturnUnsupported;
	}
	if (!client)
		return kIOReturnNoResources;
	if (!client->attach(this)) {
		client->release();
		return kIOReturnInternalError;
	}
	if (!client->start(this)) {
		client->detach(this);
		client->release();
		return kIOReturnInternalError;
	}
	*handler = client;
	return kIOReturnSuccess;
}

#pragma mark -
#pragma mark SVGA FIFO Sync Methods
#pragma mark -

HIDDEN
IOReturn CLASS::SyncFIFO()
{
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	m_svga->SyncFIFO();
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::RingDoorBell()
{
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	m_svga->RingDoorBell();
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::SyncToFence(uint32_t fence)
{
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	m_svga->SyncToFence(fence);
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

#pragma mark -
#pragma mark SVGA FIFO Acceleration Methods for 2D Context
#pragma mark -

HIDDEN
IOReturn CLASS::useAccelUpdates(bool state, task_t owningTask)
{
	if (m_updating_ga != 0 && m_updating_ga != owningTask)
		return kIOReturnSuccess;
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_updating_ga = state ? owningTask : 0;
	m_framebuffer->useAccelUpdates(state);
	return kIOReturnSuccess;
}

/*
 * Note: RectCopy, UpdateFramebuffer* and CopyRegion don't work in SVGA3D
 *   mode.  This is ok, since the WindowServer doesn't use them.
 *   UpdateFramebuffer* are called from IOFBSynchronize in GA.  The other functions
 *   are called from the various blitters.  In SVGA3D mode the WindowServer doesn't
 *   use IOFBSynchronize (it uses surface_flush instead), and CopyRegion blits
 *   are done to a surface destination.
 */
HIDDEN
IOReturn CLASS::RectCopy(uint32_t framebufferIndex,
						 struct IOBlitCopyRectangleStruct const* copyRects,
						 size_t copyRectsSize)
{
	size_t i, count = copyRectsSize / sizeof(IOBlitCopyRectangle);
	bool rc;

	if (!count || !copyRects)
		return kIOReturnBadArgument;
	if (HaveFrontBuffer()) {
		DefineRegion<1U> tmpRegion;
		IOReturn rv = kIOReturnSuccess;

		for (i = 0; i < count; ++i) {
			struct IOBlitCopyRectangleStruct const* rect = &copyRects[i];
			set_region(&tmpRegion.r,
					   rect->sourceX,
					   rect->sourceY,
					   rect->width,
					   rect->height);
			rv = CopyRegion(framebufferIndex,
							rect->x,
							rect->y,
							&tmpRegion.r,
							sizeof tmpRegion);
			if (rv != kIOReturnSuccess)
				break;
		}
		return rv;
	}
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	for (i = 0; i < count; ++i) {
		rc = m_svga->RectCopy(reinterpret_cast<UInt32 const*>(&copyRects[i]));
		if (!rc)
			break;
	}
	m_framebuffer->unlockDevice();
	return rc ? kIOReturnSuccess : kIOReturnNoMemory;
}

#if 0
HIDDEN
IOReturn CLASS::RectFillScreen(uint32_t framebufferIndex,
							   uint32_t color,
							   struct IOBlitRectangleStruct const* rects,
							   size_t numRects)
{
	SVGAColorBGRX c;
	size_t s1, s2;
	void* p;
	IOAccelDeviceRegion* rgn;
	VMsvga2Accel::ExtraInfo extra;

	s1 = sizeof(IOAccelDeviceRegion) * numRects * sizeof(IOAccelBounds);
	rgn = static_cast<IOAccelDeviceRegion*>(IOMalloc(s1));
	if (!rgn)
		return kIOReturnNoMemory;
	set_region(rgn, rects, numRects);

	s2 = rgn->bounds.w * rgn->bounds.h * sizeof(uint32_t);
	p = VRAMMalloc(s2);
	if (!p) {
		IOFree(rgn, s1);
		return kIOReturnNoMemory;
	}
	memset32(p, color, s2 / sizeof(uint32_t));
	bzero(&extra, sizeof extra);
	extra.mem_pitch = rgn->bounds.w * sizeof(uint32_t);
	extra.srcDeltaX = -static_cast<int>(rgn->bounds.x);
	extra.srcDeltaY = -static_cast<int>(rgn->bounds.y);
	c.value = color;
	m_framebuffer->lockDevice();
	extra.mem_offset_in_bar1 = reinterpret_cast<vm_offset_t>(p) - m_framebuffer->getVRAMPtr();
	screen.AnnotateFill(c);
	m_framebuffer->unlockDevice();
	blitToScreen(framebufferIndex,
				 rgn,
				 &extra);
	SyncFIFO();
	VRAMFree(p);
	IOFree(rgn, s1);
	return kIOReturnSuccess;
}
#endif

#if 0
HIDDEN
IOReturn CLASS::RectFill3D(uint32_t color,
						   struct IOBlitRectangleStruct const* rects,
						   size_t numRects)
{
	size_t s;
	uint32_t sid, cid;
	IOAccelDeviceRegion* rgn;
	VMsvga2Accel::ExtraInfo extra;

	if (m_master_surface_retain_count <= 0)
		return kIOReturnSuccess;		// Nothing to do

	s = sizeof(IOAccelDeviceRegion) * numRects * sizeof(IOAccelBounds);
	rgn = static_cast<IOAccelDeviceRegion*>(IOMalloc(s));
	if (!rgn)
		return kIOReturnNoMemory;
	set_region(rgn, rects, numRects);

	sid = getMasterSurfaceID();
	cid = AllocContextID();
	if (createContext(cid) != kIOReturnSuccess) {
		FreeContextID(cid);
		IOFree(rgn, s);
		return kIOReturnError;
	}
	setupRenderContext(cid, sid);
	clearContext(cid,
				 static_cast<SVGA3dClearFlag>(SVGA3D_CLEAR_COLOR),
				 rgn,
				 color,
				 1.0F,
				 0);
	destroyContext(cid);
	FreeContextID(cid);
	bzero(&extra, sizeof extra);
	surfacePresentAutoSync(sid,
						   rgn,
						   &extra);
	IOFree(rgn, s);
	return kIOReturnSuccess;
}
#endif

HIDDEN
IOReturn CLASS::RectFill(uint32_t framebufferIndex,
						 uint32_t color,
						 struct IOBlitRectangleStruct const* rects,
						 size_t rectsSize)
{
	size_t i, count = rectsSize / sizeof(IOBlitRectangle);
	bool rc;

	if (!count || !rects)
		return kIOReturnBadArgument;
	ACLog(2, "%s: color == %#x, numRects == %lu, [%d, %d, %d, %d]\n",
		  __FUNCTION__, color, count, FMT_D(rects->x), FMT_D(rects->y), FMT_D(rects->width), FMT_D(rects->height));
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	for (i = 0; i < count; ++i) {
		rc = m_svga->RectFill(color, reinterpret_cast<UInt32 const*>(&rects[i]));
		if (!rc)
			break;
	}
	m_framebuffer->unlockDevice();
	return rc ? kIOReturnSuccess : kIOReturnNoMemory;
}

HIDDEN
IOReturn CLASS::UpdateFramebufferAutoRing(UInt32 const* rect)
{
	if (!rect)
		return kIOReturnBadArgument;
	if (HaveFrontBuffer())
		return kIOReturnSuccess;
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	m_svga->UpdateFramebuffer2(rect);
	m_svga->RingDoorBell();
	m_framebuffer->unlockDevice();
#ifdef TIMING
	timeSyncs();
#endif
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::CopyRegion(uint32_t framebufferIndex,
						   int destX,
						   int destY,
						   void /* IOAccelDeviceRegion */ const* region,
						   size_t regionSize)
{
	IOAccelDeviceRegion const* rgn = static_cast<IOAccelDeviceRegion const*>(region);
	IOAccelBounds const* rect;
	int deltaX, deltaY;
	UInt32 i, copyRect[6];
	bool rc;

	if (!rgn || regionSize < IOACCEL_SIZEOF_DEVICE_REGION(rgn))
		return kIOReturnBadArgument;
	if (HaveFrontBuffer()) {
		ACLog(1, "%s: called with SVGAScreen or SVGA3D - unsupported\n", __FUNCTION__);
		return kIOReturnSuccess /* pretend to work... kIOReturnUnsupported */;
	}
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	rect = &rgn->bounds;
	if (checkOptionAC(VMW_OPTION_AC_REGION_BOUNDS_COPY)) {
		copyRect[0] = rect->x;
		copyRect[1] = rect->y;
		copyRect[2] = static_cast<uint32_t>(destX);
		copyRect[3] = static_cast<uint32_t>(destY);
		copyRect[4] = rect->w;
		copyRect[5] = rect->h;
		rc = m_svga->RectCopy(&copyRect[0]);
	} else {
		deltaX = destX - rect->x;
		deltaY = destY - rect->y;
		for (i = 0; i < rgn->num_rects; ++i) {
			rect = &rgn->rect[i];
			copyRect[0] = rect->x;
			copyRect[1] = rect->y;
			copyRect[2] = rect->x + deltaX;
			copyRect[3] = rect->y + deltaY;
			copyRect[4] = rect->w;
			copyRect[5] = rect->h;
			rc = m_svga->RectCopy(&copyRect[0]);
			if (!rc)
				break;
		}
	}
	m_framebuffer->unlockDevice();
	return rc ? kIOReturnSuccess : kIOReturnNoMemory;
}

#pragma mark -
#pragma mark Acceleration Methods for Surfaces
#pragma mark -

HIDDEN
IOReturn CLASS::createSurface(uint32_t sid,
							  SVGA3dSurfaceFlags surfaceFlags,
							  SVGA3dSurfaceFormat surfaceFormat,
							  uint32_t width,
							  uint32_t height)
{
	bool rc;
	SVGA3dSize* mipSizes;
	SVGA3dSurfaceFace* faces;

	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	rc = svga3d.BeginDefineSurface(sid, surfaceFlags, surfaceFormat, &faces, &mipSizes, 1U);
	if (!rc)
		goto exit;
	faces[0].numMipLevels = 1;
	mipSizes[0].width = width;
	mipSizes[0].height = height;
	mipSizes[0].depth = 1;
	m_svga->FIFOCommitAll();
exit:
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::destroySurface(uint32_t sid)
{
	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	svga3d.DestroySurface(sid);
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::surfaceDMA2D(uint32_t sid,
							 SVGA3dTransferType transfer,
							 void /* IOAccelDeviceRegion */ const* region,
							 ExtraInfo const* extra,
							 uint32_t* fence)
{
	bool rc;
	uint32_t i, numCopyBoxes;
	SVGA3dCopyBox* copyBoxes;
	SVGA3dGuestImage guestImage;
	SVGA3dSurfaceImageId hostImage;
	IOAccelDeviceRegion const* rgn;

	if (!extra)
		return kIOReturnBadArgument;
	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numCopyBoxes = rgn ? rgn->num_rects : 0;
	hostImage.sid = sid;
	hostImage.face = 0;
	hostImage.mipmap = 0;
	guestImage.ptr.gmrId = static_cast<uint32_t>(-2) /* SVGA_GMR_FRAMEBUFFER */;
	guestImage.ptr.offset = static_cast<uint32_t>(extra->mem_offset_in_bar1);
	guestImage.pitch = static_cast<uint32_t>(extra->mem_pitch);
	m_framebuffer->lockDevice();
	rc = svga3d.BeginSurfaceDMA(&guestImage, &hostImage, transfer, &copyBoxes, numCopyBoxes);
	if (!rc)
		goto exit;
	for (i = 0; i < numCopyBoxes; ++i) {
		IOAccelBounds const* src = &rgn->rect[i];
		SVGA3dCopyBox* dst = &copyBoxes[i];
		dst->srcx = src->x + extra->srcDeltaX;
		dst->srcy = src->y + extra->srcDeltaY;
		dst->x = src->x + extra->dstDeltaX;
		dst->y = src->y + extra->dstDeltaY;
		dst->w = src->w;
		dst->h = src->h;
		dst->d = 1;
	}
	m_svga->FIFOCommitAll();
	if (fence)
		*fence = m_svga->InsertFence();
exit:
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::surfaceCopy(uint32_t src_sid,
							uint32_t dst_sid,
							void /* IOAccelDeviceRegion */ const* region,
							ExtraInfo const* extra)
{
	bool rc;
	uint32_t i, numCopyBoxes;
	SVGA3dCopyBox* copyBoxes;
	SVGA3dSurfaceImageId srcImage, dstImage;
	IOAccelDeviceRegion const* rgn;

	if (!extra)
		return kIOReturnBadArgument;
	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numCopyBoxes = rgn ? rgn->num_rects : 0;
	bzero(&srcImage, sizeof srcImage);
	bzero(&dstImage, sizeof dstImage);
	srcImage.sid = src_sid;
	dstImage.sid = dst_sid;
	m_framebuffer->lockDevice();
	rc = svga3d.BeginSurfaceCopy(&srcImage, &dstImage, &copyBoxes, numCopyBoxes);
	if (!rc)
		goto exit;
	for (i = 0; i < numCopyBoxes; ++i) {
		IOAccelBounds const* src = &rgn->rect[i];
		SVGA3dCopyBox* dst = &copyBoxes[i];
		dst->srcx = src->x + extra->srcDeltaX;
		dst->srcy = src->y + extra->srcDeltaY;
		dst->x = src->x + extra->dstDeltaX;
		dst->y = src->y + extra->dstDeltaY;
		dst->w = src->w;
		dst->h = src->h;
		dst->d = 1;
	}
	m_svga->FIFOCommitAll();
exit:
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::surfaceStretch(uint32_t src_sid,
							   uint32_t dst_sid,
							   SVGA3dStretchBltMode mode,
							   void /* IOAccelBounds */ const* src_rect,
							   void /* IOAccelBounds */ const* dest_rect)
{
	SVGA3dBox srcBox, dstBox;
	SVGA3dSurfaceImageId srcImage, dstImage;
	IOAccelBounds const* s_rect;
	IOAccelBounds const* d_rect;

	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	s_rect = static_cast<IOAccelBounds const*>(src_rect);
	d_rect = static_cast<IOAccelBounds const*>(dest_rect);
	bzero(&srcImage, sizeof srcImage);
	bzero(&dstImage, sizeof dstImage);
	srcImage.sid = src_sid;
	dstImage.sid = dst_sid;
	srcBox.x = static_cast<uint32_t>(s_rect->x);
	srcBox.y = static_cast<uint32_t>(s_rect->y);
	srcBox.z = 0;
	srcBox.w = static_cast<uint32_t>(s_rect->w);
	srcBox.h = static_cast<uint32_t>(s_rect->h);
	srcBox.d = 1;
	dstBox.x = static_cast<uint32_t>(d_rect->x);
	dstBox.y = static_cast<uint32_t>(d_rect->y);
	dstBox.z = 0;
	dstBox.w = static_cast<uint32_t>(d_rect->w);
	dstBox.h = static_cast<uint32_t>(d_rect->h);
	dstBox.d = 1;
	m_framebuffer->lockDevice();
	svga3d.SurfaceStretchBlt(&srcImage, &dstImage, &srcBox, &dstBox, mode);
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::surfacePresentAutoSync(uint32_t sid,
									   void /* IOAccelDeviceRegion */ const* region,
									   ExtraInfo const* extra)
{
	bool rc;
	uint32_t i, numCopyRects;
	SVGA3dCopyRect* copyRects;
	IOAccelDeviceRegion const* rgn;

	if (!extra)
		return kIOReturnBadArgument;
	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numCopyRects = rgn ? rgn->num_rects : 0;
	m_framebuffer->lockDevice();
	m_svga->SyncToFence(m_present_tracker.before());
	rc = svga3d.BeginPresent(sid, &copyRects, numCopyRects);
	if (!rc)
		goto exit;
	for (i = 0; i < numCopyRects; ++i) {
		IOAccelBounds const* src = &rgn->rect[i];
		SVGA3dCopyRect* dst = &copyRects[i];
		dst->srcx = src->x + extra->srcDeltaX;
		dst->srcy = src->y + extra->srcDeltaY;
		dst->x = src->x + extra->dstDeltaX;
		dst->y = src->y + extra->dstDeltaY;
		dst->w = src->w;
		dst->h = src->h;
	}
	m_svga->FIFOCommitAll();
	m_present_tracker.after(m_svga->InsertFence());
exit:
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::surfacePresentReadback(void /* IOAccelDeviceRegion */ const* region)
{
	bool rc;
	uint32_t i, numRects;
	SVGA3dRect* rects;
	IOAccelDeviceRegion const* rgn;

	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numRects = rgn ? rgn->num_rects : 0;
	m_framebuffer->lockDevice();
	rc = svga3d.BeginPresentReadback(&rects, numRects);
	if (!rc)
		goto exit;
	for (i = 0; i < numRects; ++i) {
		IOAccelBounds const* src = &rgn->rect[i];
		SVGA3dRect* dst = &rects[i];
		dst->x = src->x;
		dst->y = src->y;
		dst->w = src->w;
		dst->h = src->h;
	}
	m_svga->FIFOCommitAll();
exit:
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

#if 0
HIDDEN
IOReturn CLASS::setupRenderContext(uint32_t cid,
								   uint32_t color_sid,
								   uint32_t depth_sid,
								   uint32_t width,
								   uint32_t height)
{
	SVGA3dRenderState* rs;
	SVGA3dSurfaceImageId colorImage;
	SVGA3dSurfaceImageId depthImage;
	SVGA3dRect rect;

	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	bzero(&colorImage, sizeof(SVGA3dSurfaceImageId));
	bzero(&depthImage, sizeof(SVGA3dSurfaceImageId));
	bzero(&rect, sizeof(SVGA3dRect));
	colorImage.sid = color_sid;
	depthImage.sid = depth_sid;
	rect.w = width;
	rect.h = height;
	m_framebuffer->lockDevice();
	svga3d.SetRenderTarget(cid, SVGA3D_RT_COLOR0, &colorImage);
	svga3d.SetRenderTarget(cid, SVGA3D_RT_DEPTH, &depthImage);
	svga3d.SetViewport(cid, &rect);
	svga3d.SetZRange(cid, 0.0F, 1.0F);
	if (svga3d.BeginSetRenderState(cid, &rs, 1)) {
		rs->state = SVGA3D_RS_SHADEMODE;
		rs->uintValue = SVGA3D_SHADEMODE_SMOOTH;
		m_svga->FIFOCommitAll();
	}
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}
#else
HIDDEN
IOReturn CLASS::setupRenderContext(uint32_t cid,
								   uint32_t color_sid)
{
	SVGA3dSurfaceImageId colorImage;

	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	bzero(&colorImage, sizeof(SVGA3dSurfaceImageId));
	colorImage.sid = color_sid;
	m_framebuffer->lockDevice();
	svga3d.SetRenderTarget(cid, SVGA3D_RT_COLOR0, &colorImage);
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}
#endif

HIDDEN
IOReturn CLASS::clearContext(uint32_t cid,
							 SVGA3dClearFlag flags,
							 void /* IOAccelDeviceRegion */ const* region,
							 uint32_t color,
							 float depth,
							 uint32_t stencil)
{
	bool rc;
	uint32_t i, numRects;
	SVGA3dRect* rects;
	IOAccelDeviceRegion const* rgn;

	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numRects = rgn ? rgn->num_rects : 0;
	m_framebuffer->lockDevice();
	rc = svga3d.BeginClear(cid, flags, color, depth, stencil, &rects, numRects);
	if (!rc)
		goto exit;
	for (i = 0; i < numRects; ++i) {
		rects[i].x = rgn->rect[i].x;
		rects[i].y = rgn->rect[i].y;
		rects[i].w = rgn->rect[i].w;
		rects[i].h = rgn->rect[i].h;
	}
	m_svga->FIFOCommitAll();
exit:
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::createContext(uint32_t cid)
{
	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	svga3d.DefineContext(cid);
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::destroyContext(uint32_t cid)
{
	if (!bHaveSVGA3D)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	svga3d.DestroyContext(cid);
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
bool CLASS::createClearSurface(uint32_t sid,
							   uint32_t cid,
							   SVGA3dSurfaceFormat format,
							   uint32_t width,
							   uint32_t height,
							   uint32_t color)
{
	DefineRegion<1U> tmpRegion;

	if (!width || !height)
		return false;
	/*
	 * Note: according to VMware documentation, if a surface with the same
	 *   sid already exists, the old surface is deleted and a new one
	 *   is created in its place.
	 */
	if (createSurface(sid,
					  SVGA3dSurfaceFlags(0),
					  format,
					  width,
					  height) != kIOReturnSuccess)
		return false;
	if (createContext(cid) != kIOReturnSuccess) {
		destroySurface(sid);
		return false;
	}
	setupRenderContext(cid, sid);
	set_region(&tmpRegion.r, 0, 0, width, height);
	clearContext(cid,
				 static_cast<SVGA3dClearFlag>(SVGA3D_CLEAR_COLOR),
				 &tmpRegion.r,
				 color,
				 1.0F,
				 0);
	destroyContext(cid);
	return true;
}

#pragma mark -
#pragma mark Screen Support Methods
#pragma mark -

HIDDEN
IOReturn CLASS::blitFromScreen(uint32_t srcScreenId,
							   void /* IOAccelDeviceRegion */ const* region,
							   ExtraInfo const* extra,
							   uint32_t* fence)
{
	uint32_t i, numRects;
	SVGAGuestPtr guestPtr;
	SVGAGMRImageFormat fmt;
	IOAccelDeviceRegion const* rgn;

	if (!extra)
		return kIOReturnBadArgument;
	if (!bHaveScreenObject)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numRects = rgn ? rgn->num_rects : 0;
	guestPtr.gmrId = static_cast<uint32_t>(-2) /* SVGA_GMR_FRAMEBUFFER */;
	guestPtr.offset = static_cast<uint32_t>(extra->mem_offset_in_bar1);
	fmt.value = 0x1820U;
	m_framebuffer->lockDevice();
	screen.DefineGMRFB(guestPtr,
					   static_cast<uint32_t>(extra->mem_pitch),
					   fmt);
	for (i = 0; i < numRects; ++i) {
		IOAccelBounds const* src = &rgn->rect[i];
		SVGASignedPoint destOrigin;
		SVGASignedRect srcRect;
		convert_rect(src, &srcRect, reinterpret_cast<SVGASignedPoint const*>(&extra->srcDeltaX));
		destOrigin.x = src->x + extra->dstDeltaX;
		destOrigin.y = src->y + extra->dstDeltaY;
		screen.BlitToGMRFB(&destOrigin, &srcRect, srcScreenId);
	}
	if (fence)
		*fence = m_svga->InsertFence();
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::blitToScreen(uint32_t destScreenId,
							 void /* IOAccelDeviceRegion */ const* region,
							 ExtraInfo const* extra,
							 uint32_t* fence)
{
	uint32_t i, numRects;
	SVGAGuestPtr guestPtr;
	SVGAGMRImageFormat fmt;
	IOAccelDeviceRegion const* rgn;

	if (!extra)
		return kIOReturnBadArgument;
	if (!bHaveScreenObject)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numRects = rgn ? rgn->num_rects : 0;
	guestPtr.gmrId = static_cast<uint32_t>(-2) /* SVGA_GMR_FRAMEBUFFER */;
	guestPtr.offset = static_cast<uint32_t>(extra->mem_offset_in_bar1);
	fmt.value = 0x1820U;
	m_framebuffer->lockDevice();
	screen.DefineGMRFB(guestPtr,
					   static_cast<uint32_t>(extra->mem_pitch),
					   fmt);
	for (i = 0; i < numRects; ++i) {
		IOAccelBounds const* src = &rgn->rect[i];
		SVGASignedPoint srcOrigin;
		SVGASignedRect destRect;
		convert_rect(src, &destRect, reinterpret_cast<SVGASignedPoint const*>(&extra->dstDeltaX));
		srcOrigin.x = src->x + extra->srcDeltaX;
		srcOrigin.y = src->y + extra->srcDeltaY;
		screen.BlitFromGMRFB(&srcOrigin, &destRect, destScreenId);
	}
	if (fence)
		*fence = m_svga->InsertFence();
	m_svga->RingDoorBell();
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::blitSurfaceToScreen(uint32_t src_sid,
									uint32_t destScreenId,
									void /* IOAccelBounds */ const* src_rect,
									void /* IOAccelDeviceRegion */ const* dest_region)
{
	bool rc;
	uint32_t i, numRects;
	SVGA3dSurfaceImageId srcImage;
	SVGASignedRect srcRect;
	SVGASignedRect destRect;
	SVGASignedRect* clipRects;
	IOAccelDeviceRegion const* rgn;
	IOAccelBounds const* s_rect;

	if (!bHaveScreenObject)
		return kIOReturnNoDevice;
	rgn = static_cast<IOAccelDeviceRegion const*>(dest_region);
	numRects = rgn ? rgn->num_rects : 0;
	s_rect = static_cast<IOAccelBounds const*>(src_rect);
	bzero(&srcImage, sizeof srcImage);
	srcImage.sid = src_sid;
	convert_rect(s_rect, &srcRect);
	convert_rect(&rgn->bounds, &destRect);
	m_framebuffer->lockDevice();
	rc = svga3d.BeginBlitSurfaceToScreen(&srcImage,
										 &srcRect,
										 destScreenId,
										 &destRect,
										 &clipRects,
										 numRects);
	if (!rc)
		goto exit;
	for (i = 0; i < numRects; ++i)
		convert_rect(&rgn->rect[i], &clipRects[i]);
	m_svga->FIFOCommitAll();
	m_svga->RingDoorBell();
exit:
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

#pragma mark -
#pragma mark GFB Methods
#pragma mark -

HIDDEN
IOReturn CLASS::blitGFB(uint32_t framebufferIndex,
						void /* IOAccelDeviceRegion */ const* region,
						ExtraInfo const* extra,
						vm_size_t limit,
						int direction)
{
	uint32_t i, numRects;
	int gfb_pitch, buffer_pitch;
	int const bytes_per_pixel = sizeof(uint32_t);
	IOVirtualAddress vram_ptr, gfb_start, gfb_end, buffer_start, buffer_end;
	IOAccelDeviceRegion const* rgn;

	if (!extra)
		return kIOReturnBadArgument;
	if (!m_framebuffer)
		return kIOReturnNotReady;
	rgn = static_cast<IOAccelDeviceRegion const*>(region);
	numRects = rgn ? rgn->num_rects : 0;
	m_framebuffer->lockDevice();
	vram_ptr = m_framebuffer->getVRAMPtr();
	gfb_start = vram_ptr + m_svga->getCurrentFBOffset();
	gfb_pitch = m_svga->getCurrentPitch();
	gfb_end = gfb_start + m_svga->getCurrentFBSize();
	/*
	 * TBD: should we lock for the entire blit?
	 */
	m_framebuffer->unlockDevice();
	buffer_start = vram_ptr + extra->mem_offset_in_bar1;
	buffer_end = vram_ptr + limit;
	buffer_pitch = static_cast<int>(extra->mem_pitch);
	for (i = 0; i < numRects; ++i) {
		IOAccelBounds const* rect = &rgn->rect[i];
		IOVirtualAddress addr1 = gfb_start +
			(rect->y + extra->srcDeltaY) * gfb_pitch +
			(rect->x + extra->srcDeltaX) * bytes_per_pixel;
		IOVirtualAddress addr2 = buffer_start +
			(rect->y + extra->dstDeltaY) * buffer_pitch +
			(rect->x + extra->dstDeltaX) * bytes_per_pixel;
		size_t l = static_cast<size_t>(rect->w * bytes_per_pixel);
		for (int16_t j = 0; j < rect->h; ++j) {
			if (addr1 < gfb_start || addr1 + l > gfb_end)
				continue;
			if (addr2 < buffer_start || addr2 + l > buffer_end)
				continue;
			if (direction)
				memcpy(reinterpret_cast<void*>(addr1),
					   reinterpret_cast<void const*>(addr2),
					   l);
			else
				memcpy(reinterpret_cast<void*>(addr2),
					   reinterpret_cast<void const*>(addr1),
					   l);
			addr1 += gfb_pitch;
			addr2 += buffer_pitch;
		}
	}
	return kIOReturnSuccess;
}

#if 0
HIDDEN
IOReturn CLASS::clearGFB(uint32_t color,
						 struct IOBlitRectangleStruct const* rects,
						 size_t numRects)
{
	size_t i;
	int gfb_pitch, gfb_w, gfb_h;
	int const bytes_per_pixel = sizeof(uint32_t);
	IOVirtualAddress gfb_start;

	if (!m_framebuffer)
		return kIOReturnNotReady;
	m_framebuffer->lockDevice();
	gfb_start = m_framebuffer->getVRAMPtr() + m_svga->getCurrentFBOffset();
	gfb_w = m_svga->getCurrentWidth();
	gfb_h = m_svga->getCurrentHeight();
	gfb_pitch = m_svga->getCurrentPitch();
	/*
	 * TBD: should we lock for the entire blit?
	 */
	m_framebuffer->unlockDevice();
	for (i = 0; i < numRects; ++i) {
		IOBlitRectangleStruct rect = rects[i];
		clip_rect(&rect, gfb_w, gfb_h);
		IOVirtualAddress addr = gfb_start + rect.y * gfb_pitch + rect.x * bytes_per_pixel;
		if (rect.width * bytes_per_pixel == gfb_pitch) {
			memset32(reinterpret_cast<void*>(addr), color, rect.height * rect.width);
			continue;
		}
		for (int j = 0; j < rect.height; ++j) {
			memset32(reinterpret_cast<void*>(addr), color, rect.width);
			addr += gfb_pitch;
		}
	}
	return kIOReturnSuccess;
}
#endif

#pragma mark -
#pragma mark Misc Methods
#pragma mark -

HIDDEN
IOReturn CLASS::getScreenInfo(IOAccelSurfaceReadData* info)
{
	if (!info)
		return kIOReturnBadArgument;
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	bzero(info, sizeof *info);
	m_framebuffer->lockDevice();
	info->x = m_svga->getCurrentFBOffset();
	info->y = m_svga->getCurrentFBSize();
	info->w = m_svga->getCurrentWidth();
	info->h = m_svga->getCurrentHeight();
#if IOACCELTYPES_10_5 || (IOACCEL_TYPES_REV < 12 && !defined(kIODescriptionKey))
	info->client_addr = reinterpret_cast<void*>(m_framebuffer->getVRAMPtr());
#else
	info->client_addr = static_cast<mach_vm_address_t>(m_framebuffer->getVRAMPtr());
#endif
	info->client_row_bytes = m_svga->getCurrentPitch();
	m_framebuffer->unlockDevice();
	ACLog(2, "%s: width == %d, height == %d\n", __FUNCTION__, FMT_D(info->w), FMT_D(info->h));
	return kIOReturnSuccess;
}

HIDDEN
bool CLASS::retainMasterSurface()
{
	bool rc = true;
	uint32_t width, height;

	if (!bHaveSVGA3D)
		return false;
	if (__sync_fetch_and_add(&m_master_surface_retain_count, 1) == 0) {
		m_framebuffer->lockDevice();
		width = m_svga->getCurrentWidth();
		height = m_svga->getCurrentHeight();
		m_framebuffer->unlockDevice();
		ACLog(2, "%s: Master Surface width == %u, height == %u\n", __FUNCTION__, width, height);
		rc = createMasterSurface(width, height);
		if (!rc)
			m_master_surface_retain_count = 0;
	}
	return rc;
}

HIDDEN
void CLASS::releaseMasterSurface()
{
	if (!bHaveSVGA3D)
		return;
	int v = __sync_fetch_and_add(&m_master_surface_retain_count, -1);
	if (v <= 0) {
		m_master_surface_retain_count = 0;
		return;
	}
	if (v == 1)
		destroyMasterSurface();
}

HIDDEN
void CLASS::lockAccel()
{
	IOLockLock(m_iolock);
}

HIDDEN
void CLASS::unlockAccel()
{
	IOLockUnlock(m_iolock);
}

HIDDEN
uint32_t CLASS::getVRAMSize() const
{
	if (m_svga)
		return m_svga->getVRAMSize();
	if (m_bar1)
		return static_cast<uint32_t>(m_bar1->getLength());
	return 134217728U;
}

#pragma mark -
#pragma mark Video Methods
#pragma mark -

HIDDEN
IOReturn CLASS::VideoSetRegsInRange(uint32_t streamId,
									struct SVGAOverlayUnit const* regs,
									uint32_t regMin,
									uint32_t regMax,
									uint32_t* fence)
{
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	if (regs)
		m_svga->VideoSetRegsInRange(streamId, regs, regMin, regMax);
	m_svga->VideoFlush(streamId);
	if (fence)
		*fence = m_svga->InsertFence();
	m_svga->RingDoorBell();
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

HIDDEN
IOReturn CLASS::VideoSetReg(uint32_t streamId,
							uint32_t registerId,
							uint32_t value,
							uint32_t* fence)
{
	if (!m_framebuffer)
		return kIOReturnNoDevice;
	m_framebuffer->lockDevice();
	m_svga->VideoSetReg(streamId, registerId, value);
	m_svga->VideoFlush(streamId);
	if (fence)
		*fence = m_svga->InsertFence();
	m_svga->RingDoorBell();
	m_framebuffer->unlockDevice();
	return kIOReturnSuccess;
}

#pragma mark -
#pragma mark ID Allocation Methods
#pragma mark -

HIDDEN
uint32_t CLASS::AllocSurfaceID()
{
	int i;
	uint64_t x;

	lockAccel();
	x = (~m_surface_id_mask) & (m_surface_id_mask + 1);
	m_surface_id_mask |= x;
	i = log_2_64(x);
	if (i < 0)
		i = static_cast<int>(8U * sizeof(uint64_t) + m_surface_ids_unmanaged++);
	unlockAccel();
	return static_cast<uint32_t>(i);
}

HIDDEN
void CLASS::FreeSurfaceID(uint32_t sid)
{
	if (sid >= 8U * static_cast<uint32_t>(sizeof(uint64_t)))
		return;
	lockAccel();
	m_surface_id_mask &= ~(1ULL << sid);
	unlockAccel();
}

HIDDEN
uint32_t CLASS::AllocContextID()
{
	int i;
	uint64_t x;

	lockAccel();
	x = (~m_context_id_mask) & (m_context_id_mask + 1);
	m_context_id_mask |= x;
	i = log_2_64(x);
	if (i < 0)
		i = static_cast<int>(8U * sizeof(uint64_t) + m_context_ids_unmanaged++);
	unlockAccel();
	return static_cast<uint32_t>(i);
}

HIDDEN
void CLASS::FreeContextID(uint32_t cid)
{
	if (cid >= 8U * static_cast<uint32_t>(sizeof(uint64_t)))
		return;
	lockAccel();
	m_context_id_mask &= ~(1ULL << cid);
	unlockAccel();
}

HIDDEN
uint32_t CLASS::AllocStreamID()
{
	int i;
	uint32_t x;

	lockAccel();
	x = (~m_stream_id_mask) & (m_stream_id_mask + 1);
	m_stream_id_mask |= x;
	i = log_2_32(x);
	unlockAccel();
	return static_cast<uint32_t>(i);
}

HIDDEN
void CLASS::FreeStreamID(uint32_t streamId)
{
	if (streamId >= 8U * static_cast<uint32_t>(sizeof(uint32_t)))
		return;
	lockAccel();
	m_stream_id_mask &= ~(1U << streamId);
	unlockAccel();
}

HIDDEN
uint32_t CLASS::AllocGMRID()
{
	int i;
	uint64_t x;

	lockAccel();
	x = (~m_gmr_id_mask) & (m_gmr_id_mask + 1);
	m_gmr_id_mask |= x;
	i = log_2_64(x);
	unlockAccel();
	return static_cast<uint32_t>(i);
}

HIDDEN
void CLASS::FreeGMRID(uint32_t gmrId)
{
	if (gmrId >= 8U * static_cast<uint32_t>(sizeof(uint64_t)))
		return;
	lockAccel();
	m_gmr_id_mask &= ~(1ULL << gmrId);
	unlockAccel();
}

#pragma mark -
#pragma mark Memory Methods
#pragma mark -

HIDDEN
void* CLASS::VRAMMalloc(size_t bytes)
{
	IOReturn rc;
	void* p = 0;

	if (!m_allocator)
		return 0;
	lockAccel();
	rc = m_allocator->Malloc(bytes, &p);
	unlockAccel();
	if (rc != kIOReturnSuccess)
		ACLog(1, "%s(%lu) failed\n", __FUNCTION__, bytes);
	return p;
}

HIDDEN
void* CLASS::VRAMRealloc(void* ptr, size_t bytes)
{
	IOReturn rc;
	void* newp = 0;

	if (!m_allocator)
		return 0;
	lockAccel();
	rc = m_allocator->Realloc(ptr, bytes, &newp);
	unlockAccel();
	if (rc != kIOReturnSuccess)
		ACLog(1, "%s(%p, %lu) failed\n", __FUNCTION__, ptr, bytes);
	return newp;
}

HIDDEN
void CLASS::VRAMFree(void* ptr)
{
	IOReturn rc;

	if (!m_allocator)
		return;
	lockAccel();
	rc = m_allocator->Free(ptr);
	unlockAccel();
	if (rc != kIOReturnSuccess)
		ACLog(1, "%s(%p) failed\n", __FUNCTION__, ptr);
}

HIDDEN
IOMemoryMap* CLASS::mapVRAMRangeForTask(task_t task, vm_offset_t offset_in_bar1, vm_size_t size)
{
	if (!m_bar1)
		return 0;
	return m_bar1->createMappingInTask(task,
									   0,
									   kIOMapAnywhere,
									   offset_in_bar1,
									   size);
}