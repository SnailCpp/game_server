/*
 *  Created on: Dec 16, 2015
 *      Author: zhangyalei
 */

#include "Public_Struct.h"
#include "Server_Config.h"
#include "Log.h"
#include "Mongo_Struct.h"
#include "Mysql_Struct.h"
#include "Log_Struct.h"

int load_struct(const char *path, DB_Type db_type, DB_Struct_Id_Map &db_struct_id_map, DB_Struct_Name_Map &db_struct_name_map){
	Xml xml;
	xml.load_xml(path);
	TiXmlNode *node = xml.get_root_node();
	XML_LOOP_BEGIN(node)
		DB_Struct *db_struct = nullptr;
		if (db_type == MONGODB)	{
			db_struct = new Mongo_Struct(xml, node);
		} else if (db_type == MYSQL) {
			db_struct = new Mysql_Struct(xml, node);
		} else if (db_type == LOGDB) {
			db_struct = new Log_Struct(xml, node);
		} else {
			LOG_FATAL("load_struct db_type = %d error abort", db_type);
		}

		if(db_struct->msg_id() != 0) {
			db_struct_id_map.insert(std::pair<int32_t, DB_Struct*>(db_struct->msg_id(), db_struct));
		}
		db_struct_name_map.insert(std::pair<std::string, DB_Struct*>(db_struct->struct_name(), db_struct));
	XML_LOOP_END(node)
	return 0;
}

void Server_Conf::init_server_conf(void) {
	const Json::Value &server_conf = SERVER_CONFIG->server_conf();
	Lib_Log::instance()->set_file_switcher(server_conf["lib_log_switcher"].asInt());
	Log::instance()->set_file_switcher(server_conf["server_log_switcher"].asInt());

	server_sleep_time = Time_Value(1, 0);					//1s
	receive_timeout = Time_Value(server_conf["receive_timeout"].asInt(), 0);	//900s
	server_send_interval = Time_Value(0, server_conf["server_send_interval"].asInt());				//100us
	connector_send_interval = Time_Value(0, server_conf["connector_send_interval"].asInt());	//100us

	server_ip = server_conf["server_ip"].asString();
	log_port = server_conf["log_server"]["port"].asInt();
	db_port = server_conf["db_server"]["port"].asInt();
	login_client_network_protocol = server_conf["login_server"]["client_network_protocol"].asInt();
	login_client_port = server_conf["login_server"]["client_port"].asInt();
	login_gate_port = server_conf["login_server"]["gate_port"].asInt();
	master_gate_port = server_conf["master_server"]["gate_port"].asInt();
	master_game_port = server_conf["master_server"]["game_port"].asInt();
	game_gate_port = server_conf["game_server"]["gate_port"].asInt();
	gate_client_network_protocol = server_conf["gate_server"]["client_network_protocol"].asInt();
	gate_client_port = server_conf["gate_server"]["client_port"].asInt();
}

Login_Player_Info::Login_Player_Info(void) {
	reset();
}

Login_Player_Info::~Login_Player_Info() {
}

void Login_Player_Info::serialize(Block_Buffer &buffer) const {
	buffer.write_string(account);
	buffer.write_string(gate_ip);
	buffer.write_int32(gate_port);
	buffer.write_string(session);
	buffer.write_int64(session_tick);
}

int Login_Player_Info::deserialize(Block_Buffer &buffer) {
	account = buffer.read_string();
	gate_ip = buffer.read_string();
	gate_port = buffer.read_int32();
	session = buffer.read_string();
	session_tick = buffer.read_int64();
	return 0;
}

void Login_Player_Info::reset(void) {
	account.clear();
	gate_ip.clear();
	gate_port = 0;
	session.clear();
	session_tick = 0;
}
