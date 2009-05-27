function HamGrenade() { // name, position, rotation, direction, speed

    this.createUniqueName = HamGrenade_createUniqueName;
    this.remove = HamGrenade_remove;
    this.shoot = HamGrenade_shoot;
    this.explode = HamGrenade_explode;

    system.scene.entities.createEntity(this.createUniqueName(), this);

    print(this.name);

    this.createPosComponent("pos");
    print("created pos");
    this.createMeshComponent("mesh", { mesh: "meshes/ham.fbx#Box01", transform: "pos" });
    this.createDynamicActorComponent("actor", { shapesXml: "meshes/ham_DYNAMIC.xml", transform: "pos" });
    this.createTimerComponent("lifetime", { delay: 10000, action: HamGrenade_remove });
    this.createTimerComponent("fuse", { delay: 3000, action: HamGrenade_explode });
    this.acquire();

    system.scene.sound.playSound("sound/weapons/grenade/grenlf1a.wav", 0.2);
    this.shoot();
}

function HamGrenade_shoot() {
    var direction = new Vector(0, 0, 1);
    direction.rotate(game.player.getRot());
    var position = new Vector(direction);
    position.mul(5);
    position.add(game.player.getPos());
    
    this.pos.setPos(position);
    this.pos.setRot(game.player.getRot());
    this.components.actor.setLinearVelocity(direction.mul(15));
}

var HamGrenade_count = 0;

function HamGrenade_createUniqueName() {
    if (typeof HamGrenade_createUniqueName.count == undefined) {
        HamGrenade_createUniqueName.count = 0;
        print("created count");
    }

    return "hamgrenade_" + HamGrenade_count++;
}

function HamGrenade_remove(c) { // note to self: move to Entity
    if (c) return c.entity.remove();

    system.scene.entities.removeEntity(this.name);
}

function HamGrenade_explode(c) {
    if (c) return c.entity.explode();

    var origin = this.components.pos.getPos();
    system.scene.sound.playSound3d("sound/weapons/rocket/rocklx1a.wav", origin, 1.0);
    this.removeComponent("mesh");
    this.removeComponent("actor");
    this.removeComponent("fuse");
    var blasted = system.scene.physics.getActorsInSphere(origin, 5);
    for (var i in blasted) {
        var v = blasted[i].transform.getPos();
        v.sub(origin);
        var distance = v.length() + 1;
        v.normalize();
        v.mul(10 / (distance * distance));
        blasted[i].addForceType(v, NX_IMPULSE);
    }

    for (var i = 0; i < 50; i++) {
        this.createPosComponent("p" + i, { pos: origin, rot: [Math.random() * 360, Math.random() * 360, Math.random() * 360] });
        this.createMeshComponent("m" + i, { mesh: "meshes/hamslice.fbx#Slice01", transform: "p" + i }).acquire();
        var actor = this.createDynamicActorComponent("a" + i, { shapesXml: "meshes/hamslice_DYNAMIC.xml", transform: "p" + i });
        actor.acquire();
        actor.setShapesGroup(10);
        actor.setLinearVelocity([(Math.random() * 10) - 5, (Math.random() * 10), (Math.random() * 10) - 5]);
        actor.setAngularVelocity([Math.random() * 100, Math.random() * 100, Math.random() * 100]);
    }
}