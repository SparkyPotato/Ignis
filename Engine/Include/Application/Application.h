/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Entry handler for Ignis applications.

#pragma once
#include "Core/Platform.h"

namespace Ignis {

/// Interface for Applications using Ignis.
class IGNIS_API Application
{
public:
	/// Destructor.
	virtual ~Application() = default;

	/// Initialize the application. Before this point, the Engine Runtime is not initialized,
	/// and no related functions should be called.
	///
	/// \return If initialization succeeded.
	/// If false is returned, the Engine Loop immediately cleans up and terminates, it returning false itself.
	virtual bool Initialize() {}
};

}
