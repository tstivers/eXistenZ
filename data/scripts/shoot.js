var shootspeed = 500;
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

function shootEntity(entity, pos, direction, speed) {
    entity.components.pos.setPos(pos);
    entity.components.pos.setRot(direction);

    var velocity = new Vector(0, 0, 1);
    velocity.rotate(direction);
    entity.components.actor.setLinearVelocity(velocity.mul(speed));
    entity.components.actor.setContactReportFlags(NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_FORCES);
    var cc = entity.createContactCallbackComponent("bounce");
    entity.components.actor.contactCallback = cc;
    cc.onContact = grenadeBounce;
    cc.acquire();
    
    return entity;
}

function grenadeBounce(actor) {    
    system.scene.sound.playSound3d("sound/weapons/grenade/hgrenb1a.wav", actor.transform.getPos(), 0.5);
}

function sliceBounce(actor, args) {
    //system.scene.sound.playSound3d("sound/weapons/grenade/hgrenb2a.wav", actor.transform.getPos(), 0.3);
    if (actor.bounced) {
        var entity = actor.entity;
        entity.removeComponent(actor.mesh_component.name);
        entity.removeComponent(actor.pos_component.name);
        entity.removeComponent(actor.name);
    } else
        actor.bounced = true;
}

function playerShoot()
{
    if(lastshot + shootspeed < system.time.ms)
        lastshot = system.time.ms;
    else
        return;
    system.scene.sound.playSound("sound/weapons/grenade/grenlf1a.wav", 0.2);
    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.getRot());
    
    var pos = new Vector(direction);
    pos.mul(5);
    pos.add(game.player.getPos());

    var projectile = game.player.createProjectile();
    
    shootEntity(projectile, pos, game.player.getRot(), shootvelo);
    hamgrenade(projectile);
    projectile.createTimerComponent("remove", { delay: 10000, action: removeEntityByComponent });
}

function removeEntityByComponent(c) {
    system.scene.entities.removeEntity(c.entity.name);
}

function probeShoot() {
    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.getRot());
    var pos = new Vector(direction);
    pos.mul(3);
    pos.add(game.player.getPos());
    var component = system.scene.physics.getFirstActorInRay(pos, direction, 100.0);
    if (component)
        print("hit component " + component.name);

    system.render.drawline(game.player.getPos(), pos);
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

var grenade_slices = 50;
function hamgrenade(ham) {
    system.scene.physics.setGroupCollisionFlag(10, 10, false);
    system.scene.physics.setGroupCollisionFlag(10, 0, true);
    ham.createTimerComponent("fuse", { delay: 1500, action: doExplosion });
}

function doExplosion(c) {
    var entity = c.entity;

    var origin = entity.components.pos.getPos();
    system.scene.sound.playSound3d("sound/weapons/rocket/rocklx1a.wav", origin, 1.0);
    entity.removeComponent("mesh");
    entity.removeComponent("actor");
    blasted = system.scene.physics.getActorsInSphere(origin, 5);
    for (i in blasted) {
        v = blasted[i].transform.getPos();
        v.sub(origin);
        distance = v.length() + 1;
        v.normalize();
        v.mul(10 / (distance * distance));
        blasted[i].addForceType(v, NX_IMPULSE);
    }

    entity.components.bounce.onContact = sliceBounce;
    
    for (i = 0; i < grenade_slices; i++) {
        var pos_component = entity.createPosComponent("p" + i, { pos: origin, rot: [Math.random() * 360, Math.random() * 360, Math.random() * 360] });
        var mesh_component = entity.createMeshComponent("m" + i, { mesh: "meshes/hamslice.fbx#Slice01", transform: "p" + i });
        var actor = entity.createDynamicActorComponent("a" + i, { shapesXml: "meshes/hamslice_DYNAMIC.xml", transform: "p" + i });

        actor.mesh_component = mesh_component;
        actor.pos_component = pos_component;

        actor.acquire();
        mesh_component.acquire();
        
        actor.setShapesGroup(10);
        actor.setContactReportFlags(NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_FORCES);
        actor.contactCallback = entity.components.bounce;        
        actor.setLinearVelocity([(Math.random() * 10) - 5, (Math.random() * 10), (Math.random() * 10) - 5]);
        actor.setAngularVelocity([Math.random() * 100, Math.random() * 100, Math.random() * 100]);
    }
}