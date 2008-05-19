var shootspeed = 100;
var shootvelo = 15;
var lastshot = 0;

game.player.shoot = function(key, state)
{
    this.shootFunction();
}

unbind(BUTTON_0);
bind(BUTTON_0, game.player, game.player.shoot, STATE_DOWN);
game.player.shootFunction = playerShootSphere;

function shootSphere(pos, direction, speed)
{
    sphere = createSphereEntity("sphere" + num_entities++, textures[++current_texture % textures.length]);
    entities[sphere.name] = sphere;
    system.scene.addEntity(sphere);
    sphere.pos = pos;
    sphere.radius = ((Math.random() * 12) + 3) * 0.03;
    sphere.applyForce(direction.mul(speed));
    return sphere;
}

function playerShootSphere()
{
    if(lastshot + shootspeed < system.time.ms)
        lastshot = system.time.ms;
    else
        return;
        
    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.rot);
    var pos = new Vector(direction);
    pos.mul(2);
    pos.add(game.player.pos);
    sphere = shootSphere(pos, direction, shootvelo);
    sphere.remove = function(){ removeEntity(this.name); };
    timer.addTimer(sphere.name + "_timer", sphere, sphere.remove, 0, system.time.ms + 10000);
}
