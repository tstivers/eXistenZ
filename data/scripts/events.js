
print("loading events");

var last_time = 0;
function on_tick() {
    if (last_time == 0)
        last_time = system.time.ms;

    //    system.render.drawline(game.player.getPos(), [0,0,0]);

    if (system.time.ms > last_time + 1000) {
        //print("tick! (" + arg + ")");
        last_time = system.time.ms;
    }
}

function on_resize(width, height) {
    system.ui.console.height = system.render.resolution.y - system.ui.console.y - 50;
    system.ui.fps.x = system.render.resolution.x - 150;
}

system.render.draw_velocity_lines = 0;
system.render.draw_entity_names = 0;
function on_render() {
    for (name in entities) {
        if(entities[name].velocity && system.render.draw_velocity_lines) {
            var v = new Vector(entities[name].velocity);
            var length = v.length();
            v.normalize();
            v.mul(length / 5);
            v.add(entities[name].getPos());
            system.render.drawline(entities[name].getPos(), v);
        }
        if (system.render.draw_entity_names) {
            var pos = new Vector(entities[name].getPos());
            pos.y += 1;
            drawtext(name, pos);   
        }
    }
}

print("loaded events");