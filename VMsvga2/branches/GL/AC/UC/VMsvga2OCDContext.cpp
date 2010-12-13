/*
 *  VMsvga2OCDContext.cpp
 *  VMsvga2Accel
 *
 *  Created by Zenith432 on October 11th 2009.
 *  Copyright 2009-2010 Zenith432. All rights reserved.
 *  Portions Copyright (c) Apple Computer, Inc.
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

#include <IOKit/IOLib.h>
#include "Vlog.h"
#include "VMsvga2Accel.h"
#include "VMsvga2OCDContext.h"
#include "ACMethods.h"
#include "UCTypes.h"

#define CLASS VMsvga2OCDContext
#define super IOUserClient
OSDefineMetaClassAndStructors(VMsvga2OCDContext, IOUserClient);

#if LOGGING_LEVEL >= 1
#define OCDLog(log_level, ...) do { if (log_level <= m_log_level) VLog("IOOCD: ", ##__VA_ARGS__); } while (false)
#else
#define OCDLog(log_level, ...)
#endif

static
IOExternalMethod iofbFuncsCache[kIOVMOCDNumMethods] =
{
// IONVOCDContext
	{0, reinterpret_cast<IOMethod>(&CLASS::finish), kIOUCScalarIScalarO, 0, 0},
	{0, reinterpret_cast<IOMethod>(&CLASS::wait_for_stamp), kIOUCScalarIScalarO, 1, 0},
// NVOCDContext
	{0, reinterpret_cast<IOMethod>(&CLASS::check_error_notifier), kIOUCScalarIStructO, 0, 16U},
	{0, reinterpret_cast<IOMethod>(&CLASS::mark_texture_for_ocd_use), kIOUCScalarIScalarO, 1, 0},
	{0, reinterpret_cast<IOMethod>(&CLASS::FreeEvent), kIOUCScalarIScalarO, 0, 0},
	{0, reinterpret_cast<IOMethod>(&CLASS::GetHandleIndex), kIOUCScalarIScalarO, 0, 2},
};

#pragma mark -
#pragma mark IOUserClient Methods
#pragma mark -

IOExternalMethod* CLASS::getTargetAndMethodForIndex(IOService** targetP, UInt32 index)
{
	if (index >= kIOVMOCDNumMethods)
		OCDLog(1, "%s(target_out, %u)\n", __FUNCTION__, static_cast<unsigned>(index));
	if (!targetP || index >= kIOVMOCDNumMethods)
		return 0;
#if 0
	if (index >= kIOVMOCDNumMethods) {
		if (m_provider)
			*targetP = m_provider;
		else
			return 0;
	} else
		*targetP = this;
#else
	*targetP = this;
#endif
	return &iofbFuncsCache[index];
}

IOReturn CLASS::clientClose()
{
	OCDLog(2, "%s\n", __FUNCTION__);
	if (!terminate(0))
		IOLog("%s: terminate failed\n", __FUNCTION__);
	m_owning_task = 0;
	m_provider = 0;
	return kIOReturnSuccess;
}

IOReturn CLASS::clientMemoryForType(UInt32 type, IOOptionBits* options, IOMemoryDescriptor** memory)
{
	OCDLog(1, "%s(%u, options_out, memory_out)\n", __FUNCTION__, static_cast<unsigned>(type));
	return super::clientMemoryForType(type, options, memory);
}

IOReturn CLASS::connectClient(IOUserClient* client)
{
	OCDLog(1, "%s(%p), name == %s\n", __FUNCTION__, client, client ? client->getName() : "NULL");
	return super::connectClient(client);
}

IOReturn CLASS::registerNotificationPort(mach_port_t port, UInt32 type, UInt32 refCon)
{
	OCDLog(1, "%s(%p, %u, %u)\n", __FUNCTION__, port, static_cast<unsigned>(type), static_cast<unsigned>(refCon));
	return super::registerNotificationPort(port, type, refCon);
}

bool CLASS::start(IOService* provider)
{
	m_provider = OSDynamicCast(VMsvga2Accel, provider);
	if (!m_provider)
		return false;
	m_log_level = imax(m_provider->getLogLevelGLD(), m_provider->getLogLevelAC());
	return super::start(provider);
}

bool CLASS::initWithTask(task_t owningTask, void* securityToken, UInt32 type)
{
	m_log_level = LOGGING_LEVEL;
	if (!super::initWithTask(owningTask, securityToken, type))
		return false;
	m_owning_task = owningTask;
	return true;
}

__attribute__((visibility("hidden")))
CLASS* CLASS::withTask(task_t owningTask, void* securityToken, uint32_t type)
{
	CLASS* inst;

	inst = new CLASS;

	if (inst && !inst->initWithTask(owningTask, securityToken, type))
	{
		inst->release();
		inst = 0;
	}

	return (inst);
}

#pragma mark -
#pragma mark IONVOCDContext Methods
#pragma mark -

__attribute__((visibility("hidden")))
IOReturn CLASS::finish()
{
	OCDLog(2, "%s()\n", __FUNCTION__);
	return kIOReturnUnsupported;
}

__attribute__((visibility("hidden")))
IOReturn CLASS::wait_for_stamp(uintptr_t c1)
{
	OCDLog(2, "%s(%lu)\n", __FUNCTION__, c1);
	return kIOReturnUnsupported;
}

#pragma mark -
#pragma mark NVOCDContext Methods
#pragma mark -

__attribute__((visibility("hidden")))
IOReturn CLASS::check_error_notifier(struct NvNotificationRec volatile* struct_out, size_t* struct_out_size)
{
	OCDLog(2, "%s(struct_out, %lu)\n", __FUNCTION__, *struct_out_size);
	if (*struct_out_size < sizeof *struct_out)
		return kIOReturnBadArgument;
	return kIOReturnUnsupported;
}

__attribute__((visibility("hidden")))
IOReturn CLASS::mark_texture_for_ocd_use(uintptr_t c1)
{
	OCDLog(2, "%s(%lu)\n", __FUNCTION__, c1);
	return kIOReturnUnsupported;
}

__attribute__((visibility("hidden")))
IOReturn CLASS::FreeEvent()
{
	OCDLog(2, "%s()\n", __FUNCTION__);
	return kIOReturnUnsupported;
}

__attribute__((visibility("hidden")))
IOReturn CLASS::GetHandleIndex(uint32_t*, uint32_t*)
{
	OCDLog(2, "%s(out1, out2)\n", __FUNCTION__);
	return kIOReturnUnsupported;
}