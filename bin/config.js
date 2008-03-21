log("loading configuration...");

// standard config settings
system.render.resolution.bitdepth = 32;
system.render.resolution.refreshrate = 60;
system.render.wait_vtrace = 0;

system.render.resolution.x = 1024;
system.render.resolution.y = 768;
system.render.fullscreen = 0;

system.render.resolution.x = 1280;
system.render.resolution.y = 1024;
system.render.fullscreen = 1;
system.render.wait_vtrace = 1;

system.render.refdevice = 0;
system.render.multisampletype = 2; // 2 = 2x antialiasing, 4 = 4x, etc.
system.vfs.addPath("../data");
system.vfs.addPath("../data/pak0.zip");
system.vfs.debug = 0;
system.render.texture.debug = 0;
system.render.bsp.debug = 1;
system.render.boost = 0;
system.render.gamma = 2.0;
system.render.transparency = 1;
system.render.lighting = 1;
system.window.position.x = 20;
system.window.position.y = 20;
system.window.title = "eXistenZ";
system.ui.draw = 1;
system.ui.console.x = 20;
system.ui.console.y = 30;
system.ui.console.height = system.render.resolution.y - system.ui.console.y - 50;
system.ui.fps.x = system.render.resolution.x - 150;
//system.debug.traceflags = 0xffff; // turn on debugger tracing
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
game.player.step_up = 25;

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
//bind(BUTTON_0, "*move_jump");
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
//bind(KEY_B, "toggle_bsp");
bind(KEY_H, "toggle_pos");
bind(KEY_K, "toggle_diffuse");
//bind(KEY_S, "toggle_sky");
bind(KEY_Z, "*exec move_up()");
bind(MWHEELUP, "exec print(\"mousewheel up\")");
bind(MWHEELDN, "exec print(\"mousewheel down\")");
bind(BUTTON_2, "exec bullet_time_toggle()");
bind(BUTTON_3, "+exec bullet_time_on()");
bind(BUTTON_3, "-exec bullet_time_off()");
//bind(KEY_N, "exec createBox()");
bind(KEY_V, "*exec createBox()");
bind(KEY_B, "*exec createSphere()");
bind(KEY_E, "toggle_entities");
bind(KEY_F, "toggle_movemode");
bind(KEY_Q, "exec doIt()");
bind(KEY_X, "exec explodeEverything()");
bind(KEY_R, "exec eraseEverything()");
bind(BUTTON_0, "*exec playerShootSphere()");
bind(KEY_N, "exec fountain(game.player.pos, game.player.rot)");

// functions

var message;
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

num_entities = 0;
current_texture = 0;
textures = new Array(
    "textures/gothic_block/block10d",
    "textures/gothic_block/blocks10",
    "textures/gothic_block/blocks11b",
    "textures/gothic_block/blocks11bbroke",
    "textures/gothic_block/blocks11bbroke3",
    "textures/gothic_block/blocks11d",
    "textures/gothic_block/blocks15",
    "textures/gothic_block/blocks15_blue",
    "textures/gothic_block/blocks15_c",
    "textures/gothic_block/blocks15_iron_r",
    "textures/gothic_block/blocks17",
    "textures/gothic_block/blocks17e",
    "textures/gothic_block/blocks17g",
    "textures/gothic_block/blocks17i",
    "textures/gothic_block/blocks17j",
    "textures/gothic_block/blocks17k");

var entities = new Object();

function createBox() {
    box = createBoxEntity("box" + num_entities++, textures[current_texture++]);
    if(current_texture >= textures.length)
        current_texture = 0;
    entities[box.name] = box;
    system.scene.addEntity(box);
    box.pos = game.player.pos;    
    print('added box ' + box.name);
    timer.addTimer("box" + num_entities + "_timer", "bounceEntity('" + box.name + "');", 500, 0);
    box.last_y = 0;
}

function createSphere() {
    sphere = createSphereEntity("sphere" + num_entities++, textures[current_texture++]);
    if(current_texture >= textures.length)
        current_texture = 0;
    entities[sphere.name] = sphere;
    system.scene.addEntity(sphere);
    sphere.pos = game.player.pos;    
    print('added sphere ' + sphere.name);
    timer.addTimer("sphere" + num_entities + "_timer", "bounceEntity('" + sphere.name + "');", 500, 0);
    sphere.last_y = 0;
}

function shootSphere(pos, direction, speed)
{
    var sphere = createSphereEntity("sphere" + num_entities++, textures[++current_texture % textures.length]);
    entities[sphere.name] = sphere;
    system.scene.addEntity(sphere);
    sphere.pos = pos;
    //direction.mul(speed);
    sphere.applyForce(direction.mul(speed));
    return sphere;
}

var shootspeed = 100;
var shootvelo = 2500;
var lastshot = 0;

function playerShootSphere()
{
    if(lastshot + shootspeed < system.time.ms)
        lastshot = system.time.ms;
    else
        return;
        
    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.rot);
    var pos = new Vector(direction);
    pos.mul(50);
    pos.add(game.player.pos);
    var sphere = shootSphere(pos, direction, shootvelo);
    timer.addTimer(sphere.name + "_timer", "removeEntity('" + sphere.name + "');", 0, system.time.ms + 10000);
}

function bounceEntity(entityName)
{
    var entity = entities[entityName];
    if(Math.abs(entity.last_y - entity.pos.y) <= 5)
    {
        var vec = new Vector();
        vec.x = game.player.pos.x - entity.pos.x
        vec.z = game.player.pos.z - entity.pos.z;
        vec.normalize();
        entity.applyForce(Math.random() * 100 * vec.x, Math.random() * 100, Math.random() * 100 * vec.z);
    } else 
        entity.last_y = entity.pos.y;
}

Vector.prototype.toString = function()
{
    return "(" + this.x + ", " + this.y + ", " + this.z + ")";
}

Vector.prototype.mul = function(length)
{
    this.x *= length;
    this.y *= length;
    this.z *= length;
    return this;
}

Vector.prototype.sub = function(other)
{
    this.x -= other.x;
    this.y -= other.y;
    this.z -= other.z;
    return this;
} 

Vector.prototype.add = function(other)
{
    this.x += other.x;
    this.y += other.y;
    this.z += other.z;
    return this;
} 

function createBoxStack(pos, height, width)
{
    for(i = 0; i < width; i++)
        for(j = 0; j < height; j++)
        {
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            box.pos.x = pos.x + (i * 30);
            box.pos.y = pos.y + (j * 30);
            box.pos.z = pos.z;
        }
}

function createBoxPyramid(pos, width)
{
    for(i = 0; i < width; i++)
        for(j = 0; j < width - i; j++)
        {
            box = createBoxEntity("box" + num_entities++, textures[++current_texture % textures.length]);
            system.scene.addEntity(box);
            entities[box.name] = box;
            box.pos.x = pos.x + ((j * 40) + (i * 20));
            box.pos.y = pos.y + (i * 30);
            box.pos.z = pos.z;
            box.sleeping = true;
        }
}

var stackwidth = 40;
var stackheight = 20;

function explodeEverything()
{
    for(i in entities)
        entities[i].applyForce((Math.random() * 100) - 50, Math.random() * 100, (Math.random() * 100) - 50);
}

function removeEntity(name)
{
    if(entities[name])
    {
        system.scene.removeEntity(entities[name]);
        delete entities[name];
    }
}

function eraseEverything()
{
    for(i in entities)
    {
        system.scene.removeEntity(entities[i]);
        delete entities[i];
    }
}

function doIt()
{
    var vec = new Vector(game.player.pos);
    vec.z -= 300;
    vec.y -= 15;
    //createBoxStack(vec, 10, 10);
    createBoxPyramid(vec, stackwidth);
}

var fountains = new Array();
var fountain_speed = 75;

function fountain(pos, rot)
{
    var fountain = new Object();
    fountain.pos = new Vector(pos);
    fountain.direction = new Vector(0, 0, 1);
    fountain.direction.rotate(rot);
    fountain.last_shot = 0;
    fountain.speed = fountain_speed;
    timer.addTimer("fountain" + fountains.length + "_timer", "fireFountain(" + fountains.length + ");", fountain_speed, 0);
    fountains.push(fountain);
}

function fireFountain(index)
{
    var fountain = fountains[index];
    var direction = new Vector(fountain.direction);
    direction.rotate((Math.random() * 60) - 30, (Math.random() * 60) - 30, 0);
    var sphere = shootSphere(fountain.pos, direction, 500 + (Math.random() * 250));
    //timer.addTimer(sphere.name + "_timer", "removeEntity('" + sphere.name + "');", 0, system.time.ms + 10000);
}
    
// log our start date and time
print("eXistenZ engine started on " + Date());
