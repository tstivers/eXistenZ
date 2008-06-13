unbind(KEY_M);
unbind(KEY_B);
unbind(KEY_N);
unbind(KEY_COMMA);

bind(KEY_M, createFridge);
bind(KEY_B, createBigDaddy);
bind(KEY_N, createSplicer);
bind(KEY_COMMA, createHam, STATE_DOWN);

function createModelEntity(meshname) {
    entity = createMeshEntity("model" + num_entities++, meshname);
    entities[entity.name] = entity;
    system.scene.addEntity(entity);
    entity.pos = game.player.pos;
    entity.rot = game.player.rot;
    entity.rot.y = -90;
    return entity;
}

function createFridge() {
    fridge = createModelEntity("meshes/fridge.fbx#Fridge01");
    fridge.pos.y -= game.player.size.y;
    return fridge;
}

function createBigDaddy() {
    daddy = createModelEntity("meshes/bigdaddy.fbx#10840-mesh");
    daddy.rot.x -= 90;
    return daddy;
}

function createSplicer() {
    splicer = createModelEntity("meshes/splicer.fbx#5084-splicer");
    splicer.rot.x -= 90;
    return splicer;
}

function createHam() {
    ham = createModelEntity("meshes/ham.fbx#Cone01");
    ham.rot.x -= 90;
    return ham;
}

function createSphere() {
    var sphere = createSphereEntity("sphere" + num_entities++, textures[++current_texture % textures.length]);
    entities[sphere.name] = sphere;
    system.scene.addEntity(sphere);

    return sphere;
}
