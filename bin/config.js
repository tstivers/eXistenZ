log("loading configuration...");

// standard config settings
system.render.resolution.bitdepth = 32;
system.render.resolution.refreshrate = 60;
system.render.wait_vtrace = 0;
system.render.resolution.x = 1024;
system.render.resolution.y = 768;
system.render.fullscreen = 0;
system.vfs.addPath("../data/");
system.vfs.addPath("c:/games/eXistenZ/data");
system.vfs.addPath("c:/games/eXistenZ/data/pak0.zip");
system.vfs.debug = 0;
system.render.texture.debug = 0;
system.render.bsp.debug = 1;
system.render.boost = 0;
system.render.gamma = 2.0;
system.render.transparency = 1;
system.window.position.x = 20;
system.window.position.y = 20;
system.window.title = "eXistenZ";
system.ui.draw = 1;
system.ui.console.x = 20;
system.ui.console.y = 30;
system.ui.console.height = system.render.resolution.y - system.ui.console.y - 50;
system.ui.fps.x = system.render.resolution.x - 150;
system.debug.traceflags = 0xffff; // turn on debugger tracing
game.camera.pos.x = 0;
game.camera.pos.y = 0;
game.camera.pos.z = 0;
game.mouse.sensitivity.x = 0.5;
game.mouse.sensitivity.y = 0.5;
system.render.tesselation = 6;
game.init_command = "";
system.render.use_scenegraph = 0;
system.render.bsp.convert = 0;
system.render.skybox.texture = "textures/skybox/cx";

// source keys (will fix this later)
execfile("scripts/keys.js");

// map keys here
bind(KEY_PGUP, "*move_up");
bind(KEY_PGDN, "*move_down");
bind(KEY_LEFT, "*move_left");
bind(KEY_RIGHT, "*move_right");
bind(KEY_UP, "*move_forward");
bind(KEY_DOWN, "*move_back");

bind(KEY_NUMPAD8, "*move_forward");
bind(KEY_NUMPAD5, "*move_back");
bind(KEY_NUMPAD2, "*move_back");
bind(KEY_NUMPAD4, "*move_left");
bind(KEY_NUMPAD6, "*move_right");
bind(KEY_NUMPAD0, "move_jump");
bind(BUTTON_0, "*move_jump");
bind(BUTTON_1, "move_jump");

bind(KEY_W, "toggle_wireframe");
bind(KEY_L, "toggle_lightmap");
bind(KEY_T, "toggle_transparency");
bind(KEY_P, "toggle_patches");
bind(KEY_C, "toggle_clipping");
bind(KEY_TAB, "toggle_console");
bind(KEY_RETURN, "focus_console");
bind(KEY_SLASH, "focus_console_slash");
bind(KEY_M, "add_marker");
bind(KEY_D, "dbg_break");
bind(KEY_U, "toggle_ui");
bind(KEY_SPACE, "log_frame");
bind(KEY_LCONTROL, "*log_frame");
bind(KEY_B, "toggle_bsp");
bind(KEY_H, "toggle_pos");
bind(KEY_K, "toggle_diffuse");
bind(KEY_S, "toggle_sky");
bind(KEY_Z, "*exec move_up()");
bind(MWHEELUP, "exec print(\"mousewheel up\")");
bind(MWHEELDN, "exec print(\"mousewheel down\")");
bind(BUTTON_2, "exec bullet_time_toggle()");
bind(BUTTON_3, "+exec bullet_time_on()");
bind(BUTTON_3, "-exec bullet_time_off()");
bind(KEY_N, "exec createBox()");

// functions

function print(message) {
	log(message);
}

function map(map_name) {
	game.startMap(map_name);
}

function home() {
	game.player.pos.x = 0;
	game.player.pos.y = 0;
	game.player.pos.z = 0;
	game.player.rot.x = 0;
	game.player.rot.y = 0;
}

function stress() {
	map("q3dm1");
	map("q3dm2");
	map("q3dm3");
	map("q3dm4");
	map("q3dm5");
	map("q3dm6");
	map("q3dm7");
	map("q3dm8");
	map("q3dm9");
	map("q3dm10");
	map("q3dm11");
	map("q3dm12");
	map("q3dm13");
	map("q3dm14");
	map("q3dm15");
	map("q3ctf1");
	map("q3ctf2");
	map("q3ctf3");
	map("q3ctf4");
}

function ice() {
	game.physics.friction = 0.001;
	game.player.accel = 0.01;
}

function normal() {
	game.physics.friction = 0.04;
	game.player.accel = 0.05;
}

function bullet_time_on() {
	system.time.scale = 0.2;
	print("bullet time enabled!");
}

function bullet_time_off() {
	system.time.scale = 1;
	print("bullet time disabled!");
}

bullet_time_function = bullet_time_on;
function bullet_time_toggle() {
	bullet_time_function();
	if(bullet_time_function == bullet_time_on)
		bullet_time_function = bullet_time_off;
	else
		bullet_time_function = bullet_time_on;
}

function dump(obj) {
	for (i in obj) {
		print(typeof(obj[i]) + "." + i + " = " + obj[i]);
			dump(i);
	}
}

function markerfun() {
	bind(BUTTON_0, "*add_marker");
	bind(BUTTON_2, "*del_marker");
	print("go go gadget markers");
}

num_boxes = 0;
function createBox() {
    box = createBoxEntity("box" + num_boxes++, "");
    box.pos.x = game.player.pos.x;
    box.pos.y = game.player.pos.y;
    box.pos.z = game.player.pos.z;
    box.update();
}

// log our start date and time
print("eXistenZ engine started on " + Date());
