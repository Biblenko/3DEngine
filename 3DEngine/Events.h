#pragma once

namespace Engine
{
	struct EventEntitySelect
	{
		size_t entity;
	};

	struct EventEntityChanged
	{
		size_t entity;
	};

	struct EventEntityAlarm
	{
		size_t entity;
		bool alarm;
	};
}