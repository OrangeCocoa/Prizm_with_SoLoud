#pragma once

#include<string>
#include<iostream>

#include<DirectXTK\SimpleMath.h>

#define DLL_EXPORT __declspec(dllimport)

#include"..\..\Utilities\Log.h"
#include"..\..\Utilities\ResourcePool.h"
namespace Prizm
{
	namespace Adx2Manager
	{
		enum SceneAcb
		{
			GENERAL,
			TITLE,
			GAME,
			RESULT,
			MAX
		};
	};
}