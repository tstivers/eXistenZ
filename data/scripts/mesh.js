unbind(KEY_M);
unbind(KEY_B);
unbind(KEY_N);
unbind(KEY_COMMA);
unbind(KEY_R);

bind(KEY_M, createFridge);
bind(KEY_B, createBigDaddy);
bind(KEY_N, createSplicer);
bind(KEY_COMMA, createHam, STATE_DOWN);
bind(KEY_R, removeEntities);

function createModelEntity(meshname, dynamicsname) {
    entity = system.scene.entities.createEntity("model" + num_entities++);
    entity.createPosComponent("pos", { pos: game.player.getPos(), rot: game.player.getRot() });
    entity.createMeshComponent("mesh", { mesh: meshname, transform: "pos" });
    entity.createActorComponent("actor", { shapesXml: dynamicsname, transform: "pos" });
    entity.acquire();
    return entity;
}

function createFridge() {
    fridge = createModelEntity("meshes/fridge.fbx#Fridge01", "meshes/fridge_DYNAMIC.xml");
    return fridge;
}

function createBigDaddy() {
    return createModelEntity("meshes/bigdaddy.fbx#10840-mesh", "meshes/bigdaddy_DYNAMIC.xml");
}

function createSplicer() {
    return createModelEntity("meshes/splicer.fbx#5084-splicer");
}

function createHam() {
    return createModelEntity("meshes/ham.fbx#Box01", "meshes/ham_DYNAMIC.xml");
}

function createSlice() {
    return createModelEntity("meshes/hamslice.fbx#Slice01", "meshes/hamslice_DYNAMIC.xml");
}

function createSphere() {
    var sphere = createSphereEntity("sphere" + num_entities++, textures[++current_texture % textures.length]);
    entities[sphere.name] = sphere;
    system.scene.addEntity(sphere);

    return sphere;
}


function removeEntities() {
    for (i in system.scene.entities)
        system.scene.entities.removeEntity(system.scene.entities[i].name);
    for (i in system.scene.entities)
        print(i);
}