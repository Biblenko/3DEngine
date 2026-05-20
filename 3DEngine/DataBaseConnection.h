#pragma once

class DataBaseConnection
{
public:
	DataBaseConnection() = default;
	~DataBaseConnection() = default;

	DataBaseConnection(const DataBaseConnection&) = delete;
	DataBaseConnection& operator=(const DataBaseConnection&) = delete;

	///*std::mutex db_mutex;     
	//std::vector<int> objects_to_paint_red;
	//bool is_running = true;*/

	void DataBaseLoopCheck();
};

