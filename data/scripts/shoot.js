var shootspeed = 100;
var shootvelo = 15;
var lastshot = 0;

game.player.shoot = function(key, state)
{
    this.shootFunction();
}

unbind(BUTTON_0);
bind(BUTTON_0, game.player, game.player.shoot, STATE_DOWN);
game.player.shootFunction = hamShoot;
game.player.createProjectile = createHam;


function shootEntity(entity, pos, direction, speed) {
    entity.setPos(pos);
    entity.setRot(direction);

    var force = new Vector(0, 0, 1);
    force.rotate(direction);
    entity.applyForce(force.mul(speed));
    return entity;
}

function playerShoot()
{
    if(lastshot + shootspeed < system.time.ms)
        lastshot = system.time.ms;
    else
        return;
        
    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.getRot());
    var pos = new Vector(direction);
    pos.mul(5);
    pos.add(game.player.getPos());

    var projectile = game.player.createProjectile();
    
    shootEntity(projectile, pos, direction, shootvelo);
    projectile.remove = function(){ removeEntity(this.name); };
    timer.addTimer(projectile.name + "_timer", projectile, projectile.remove, 0, system.time.ms + 10000);
}

function hamShoot() {
    if (lastshot + shootspeed < system.time.ms)
        lastshot = system.time.ms;
    else
        return;

    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.getRot());
    var pos = new Vector(direction);
    pos.mul(1);
    pos.add(game.player.getPos());

    var projectile = createHam();

    shootEntity(projectile, pos, game.player.getRot(), shootvelo);
    projectile.remove = function() { removeTimer(this.name + "_slice_timer"); removeEntity(this.name); };
    timer.addTimer(projectile.name + "_timer", projectile, projectile.remove, 0, system.time.ms + 10000);

    projectile.spawnSlice = function() { shootSlice(this); };
    timer.addTimer(projectile.name + "_slice_timer", projectile, projectile.spawnSlice, shootspeed, 0);
}

function shootSlice(ham) {
    var shootdirection = new Vector(0, 0, 1);
    shootdirection.rotate(ham.getRot());
    var shootpos = new Vector(shootdirection);
    shootpos.mul(0.1);
    shootpos.add(ham.getPos());

    var slice = createSlice();

    shootEntity(slice, shootpos, new Vector(ham.getRot()), 5);
    slice.remove = function() { removeEntity(this.name); };
    timer.addTimer(slice.name + "_timer", slice, slice.remove, 0, system.time.ms + 10000);
}