var shootspeed = 100;
var shootvelo = 15;
var lastshot = 0;

game.player.shoot = function(key, state)
{
    this.shootFunction();
}

unbind(BUTTON_0);
bind(BUTTON_0, game.player, game.player.shoot, STATE_DOWN);
game.player.shootFunction = playerShoot;
game.player.createProjectile = createHam;


function shootEntity(entity, pos, direction, speed)
{
    entity.pos = pos;
    entity.applyForce(direction.mul(speed));
    return entity;
}

function playerShoot()
{
    if(lastshot + shootspeed < system.time.ms)
        lastshot = system.time.ms;
    else
        return;
        
    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.rot);
    var pos = new Vector(direction);
    pos.mul(5);
    pos.add(game.player.pos);

    var projectile = game.player.createProjectile();
    
    shootEntity(projectile, pos, direction, shootvelo);
    projectile.remove = function(){ removeEntity(this.name); };
    timer.addTimer(projectile.name + "_timer", projectile, projectile.remove, 0, system.time.ms + 10000);
}
