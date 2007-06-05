#include "precompiled.h"
#include "input/bind.h"
#include "input/input.h"
#include "input/keys.h"
#include "console/console.h"

namespace input {
	char binds[256][3][128] = {0};	
	char* keyName(int id);
	int keyCode(char* name);
}

void input::bindKey(int key, char* cmd)
{
	char* real_cmd = cmd;
	if(*real_cmd == '+' || *real_cmd == '-' || *real_cmd == '*')
		real_cmd++;

	if(key < 0 || key >= 256) {
		LOG("[input::bindKey] invalid key (%i) for function \"%s\"", key, cmd);
		return;
	}

	// TODO: proper command parsing
	//if(!console::isCommand(real_cmd)) {
	//	LOG("[input::bindKey] tried to bind %s to invalid function \"%s\"", keyName(key), cmd);
	//	return;
	//}

	switch(*cmd) {
		case '-':
			strcpy(binds[key][2], real_cmd);
			break;
		case '*':
			strcpy(binds[key][1], real_cmd);
			break;
		case '+':
			strcpy(binds[key][0], real_cmd);
			break;
		default:
			strcpy(binds[key][0], cmd);
			break;
	}
}

void input::unbind(int key)
{
	binds[key][0][0] = 0;
	binds[key][1][0] = 0;
	binds[key][2][0] = 0;
}

void input::listBinds()
{
	LOG("Key Bindings:");
	for(int key_idx = 0; key_idx < 256; key_idx++) {
		if(binds[key_idx][0][0])
			LOG("  [%s] = \"+%s\"", keyName(key_idx), binds[key_idx][0]);
		if(binds[key_idx][1][0])
			LOG("  [%s] = \"*%s\"", keyName(key_idx), binds[key_idx][1]);
		if(binds[key_idx][2][0])
			LOG("  [%s] = \"-%s\"", keyName(key_idx), binds[key_idx][2]);
	}
}

void input::processBinds()
{
	for(int i = 0; i < 256; i++) {
		if(KEYPRESSED(i) && binds[i][0][0])
			console::executeCommand(&binds[i][0][0]);
		if(KEYDOWN(i) && binds[i][1][0])
			console::executeCommand(&binds[i][1][0]);
		if(KEYRELEASED(i) && binds[i][2][0])
			console::executeCommand(&binds[i][2][0]);
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
		if(!_stricmp(keys[key_index].name, name))
			return(keys[key_index].value);

	return -1;
}