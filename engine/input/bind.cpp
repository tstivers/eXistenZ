/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bind.cpp,v 1.2 2003/12/24 01:45:45 tstivers Exp $
//

#include "precompiled.h"
#include "input/bind.h"
#include "input/input.h"
#include "input/keys.h"
#include "console/console.h"

namespace input {
	char binds[256][128] = {0};	
	char* keyName(int id);
	int keyCode(char* name);
}

void input::bindKey(int key, char* cmd)
{
	char* real_cmd = cmd;
	if(*real_cmd == '+' || *real_cmd == '-')
		real_cmd++;

	if(key < 0 || key >= 256) {
		LOG3("[input::bindKey] invalid key (%i) for function \"%s\"", key, cmd);
		return;
	}

	// TODO: proper command parsing
	//if(!con::isCommand(real_cmd)) {
	//	LOG3("[input::bindKey] tried to bind %s to invalid function \"%s\"", keyName(key), cmd);
	//	return;
	//}

	strcpy(binds[key], cmd);
}

void input::unnbind(int key)
{
	binds[key][0] = 0;
}

void input::listBinds()
{
	LOG("Key Bindings:");
	for(int key_idx = 0; key_idx < 256; key_idx++) 	
		if(binds[key_idx][0])
			LOG3("  [%s] = \"%s\"", keyName(key_idx), binds[key_idx]);
}

void input::processBinds()
{
	for(int key_idx = 0; key_idx < 256; key_idx++) {
		if(binds[key_idx][0]) {
			
			if(binds[key_idx][0] == '+') {
				if(KEYDOWN(key_idx)) {
					con::executeCommand(&binds[key_idx][1]);
					continue;
				}
			}
			
			if(binds[key_idx][0] == '-') {
				// do something here;
				continue;
			}

			if(KEYPRESSED(key_idx))
				con::executeCommand(binds[key_idx]);
		}
	}
}

void input::con_bind(int argc, char* argv[], void* user)
{
	if(argc != 3) {
		LOG("usage: bind <key> <function>");
		return;
	}

	int key = keyCode(argv[1]);

	bindKey(key, argv[2]);
	return;
}
	
void input::con_listBinds(char* cmd, char* args, void* user) {
	listBinds();
}

char* input::keyName(int id) {	
	char* name = "KEY_UNKNOWN";
	for(int key_index = 0; keys[key_index].value != -1; key_index++)
		if(keys[key_index].value == id)
			name = keys[key_index].name;

	return name;
}

int input::keyCode(char* name) {	
	for(int key_index = 0; keys[key_index].value != -1; key_index++)
		if(!stricmp(keys[key_index].name, name))
			return(keys[key_index].value);

	return -1;
}